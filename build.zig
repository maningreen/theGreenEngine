const std = @import("std");
const builtin = std.builtin;
const ArrayList = std.ArrayList;
const mem = std.mem;
const Allocator = mem.Allocator;
const FixedBufferAllocator = std.heap.FixedBufferAllocator;
const fs = std.fs;
const Build = std.Build;
const Module = Build.Module;
const CSourceLanguage = Module.CSourceLanguage;

const additional_flags: []const []const u8 = &.{};
const debug_flags = runtime_check_flags ++ warning_flags;

const library = struct { name: []const u8, path: ?[]const u8 = null };

const libraries: []const library = &.{
    .{ .name = "raylib" },
    .{ .name = "lua" },
};

pub fn build(b: *std.Build) void {
    var threaded = std.Io.Threaded.init_single_threaded;
    defer threaded.deinit();

    const io = threaded.io();

    const target = b.standardTargetOptions(.{});

    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.addModule("exe", .{
        .target = target,
        .optimize = optimize,
        .link_libcpp = true, // May need to change this to linkLibC() for your project
    });

    const exe = b.addExecutable(.{
        .name = "engine",
        .root_module = exe_mod,
    });

    const exe_flags = getBuildFlags(
        exe,
        optimize,
    ) catch |err|
        @panic(@errorName(err));

    const exe_files = getCSrcFiles(
        b.allocator,
        io,
        .{
            .dir_path = "src/",
            .flags = exe_flags,
            .language = .cpp,
        },
    ) catch |err|
        @panic(@errorName(err));

    exe_mod.addCSourceFiles(exe_files);
    // Build and Link zig -> c code -------------------------------------------
    const zigMod = b.createModule(.{
        .root_source_file = b.path("src/root.zig"),
        .target = target,
        .optimize = optimize,
        .link_libcpp = true,
    });
    zigMod.addIncludePath(b.path("src"));
    const zig_lib = b.addLibrary(.{
        .name = "engine",
        .root_module = zigMod,
        .linkage = .static,
    });

    exe.root_module.linkLibrary(zig_lib);

    for (libraries) |lib| {
        if (lib.path) |p| {
            zig_lib.root_module.addLibraryPath(b.path(p));
            zig_lib.root_module.addIncludePath(b.path(p));
            exe.root_module.addLibraryPath(b.path(p));
            exe.root_module.addIncludePath(b.path(p));
        }
        exe.root_module.linkSystemLibrary(lib.name, .{ .needed = true });
        zig_lib.root_module.linkSystemLibrary(lib.name, .{ .needed = true });
    }

    b.installArtifact(exe);
    const exe_run = b.addRunArtifact(exe);

    exe_run.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        exe_run.addArgs(args);
    }

    const run_step = b.step("run", "runs the application");
    run_step.dependOn(&exe_run.step);
}

/// Used to recursively fetch source files from a directory
pub fn getCSrcFiles(
    alloc: Allocator,
    io: std.Io,
    opts: struct {
        dir_path: []const u8 = "./src/",
        language: CSourceLanguage,
        flags: []const []const u8 = &.{},
    },
) !Module.AddCSourceFilesOptions {
    const src = std.Io.Dir.cwd().openDir(io, opts.dir_path, .{ .iterate = true }) catch unreachable;

    var file_list = ArrayList([]const u8).empty;
    errdefer file_list.deinit(alloc);

    const extension = @tagName(opts.language); // Will break for obj-c and assembly

    var src_iterator = src.iterate();
    while (try src_iterator.next(io)) |entry| {
        switch (entry.kind) {
            .file => {
                if (!mem.endsWith(u8, entry.name, extension))
                    continue;

                const path = try fs.path.join(alloc, &.{ opts.dir_path, entry.name });

                try file_list.append(alloc, path);
            },
            .directory => {
                var dir_opts = opts;
                dir_opts.dir_path = try fs.path.join(alloc, &.{ opts.dir_path, entry.name });

                try file_list.appendSlice(alloc, (try getCSrcFiles(alloc, io, dir_opts)).files);
            },
            else => continue,
        }
    }

    return Module.AddCSourceFilesOptions{
        .files = try file_list.toOwnedSlice(alloc),
        .language = opts.language,
        .flags = opts.flags,
    };
}

const runtime_check_flags: []const []const u8 = &.{
    "-g",
    "-fno-omit-frame-pointer",
};

const warning_flags: []const []const u8 = &.{};

fn getBuildFlags(
    exe: *std.Build.Step.Compile,
    optimize: std.builtin.OptimizeMode,
) ![]const []const u8 {
    var cpp_flags: []const []const u8 = undefined;

    if (optimize == .Debug) {
        cpp_flags = additional_flags ++ debug_flags;
        if (exe.rootModuleTarget().os.tag == .windows)
            return cpp_flags;
    } else {
        cpp_flags = additional_flags;
    }
    return cpp_flags;
}
