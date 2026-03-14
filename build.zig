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

const zcc = @import("compile_commands");

const additional_flags: []const []const u8 = &.{};
const debug_flags = runtime_check_flags ++ warning_flags;

const library = struct { name: []const u8, path: ?[]const u8 = null };

const libraries: []const library = &.{
    .{ .name = "raylib" },
    .{ .name = "lua" },
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});

    const optimize = b.standardOptimizeOption(.{});

    // const raylib_dep = b.dependency("raylib_zig", .{
        // .target = target,
        // .optimize = optimize,
    // });

    // const raylib = raylib_dep.module("raylib"); // main raylib module
    // const raygui = raylib_dep.module("raygui"); // raygui module
    // const raylibC = raylib_dep.artifact("raylib"); // raylib C library

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
        b.allocator,
        exe,
        optimize,
    ) catch |err|
        @panic(@errorName(err));

    const exe_files = getCSrcFiles(
        b.allocator,
        .{
            .dir_path = "src/",
            .flags = exe_flags,
            .language = .cpp,
        },
    ) catch |err|
        @panic(@errorName(err));

    // Setup exe executable
    {
        exe_mod.addCSourceFiles(exe_files);
        exe.addIncludePath(b.path("external/"));
    }

    // Build and Link zig -> c code -------------------------------------------
    // const zig_lib = b.addLibrary(.{
    // .name = "engine",
    // .root_module = b.createModule(.{
    // .root_source_file = b.path("src/root.zig"),
    // .target = target,
    // .optimize = optimize,
    // }),
    // .linkage = .static,
    // });
    // zig_lib.linkLibCpp();
    // zig_lib.addIncludePath(b.path("src/"));
    //
    // exe.root_module.linkLibrary(zig_lib);
    // exe.root_module.linkLibrary(raylibC);

    for (libraries) |lib| {
        if (lib.path) |p| {
            exe.root_module.addLibraryPath(b.path(p));
            exe.root_module.addIncludePath(b.path(p));
        }
        exe.root_module.linkSystemLibrary(lib.name, .{ .needed = true });
    }

    b.installArtifact(exe);
    const exe_run = b.addRunArtifact(exe);

    exe_run.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        exe_run.addArgs(args);
    }

    const run_step = b.step("run", "runs the application");
    run_step.dependOn(&exe_run.step);

    var targets = ArrayList(*std.Build.Step.Compile).empty;
    defer targets.deinit(b.allocator);

    targets.append(b.allocator, exe) catch |err| @panic(@errorName(err));

    // Used to generate compile_commands.json
    _ = zcc.createStep(
        b,
        "cmds",
        targets.toOwnedSlice(b.allocator) catch |err|
            @panic(@errorName(err)),
    );
}

/// Used to recursively fetch source files from a directory
pub fn getCSrcFiles(
    alloc: std.mem.Allocator,
    opts: struct {
        dir_path: []const u8 = "./src/",
        language: CSourceLanguage,
        flags: []const []const u8 = &.{},
    },
) !Module.AddCSourceFilesOptions {
    const src = try fs.cwd().openDir(opts.dir_path, .{ .iterate = true });

    var file_list = ArrayList([]const u8).empty;
    errdefer file_list.deinit(alloc);

    const extension = @tagName(opts.language); // Will break for obj-c and assembly

    var src_iterator = src.iterate();
    while (try src_iterator.next()) |entry| {
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

                try file_list.appendSlice(alloc, (try getCSrcFiles(alloc, dir_opts)).files);
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

/// Returns the path of the system installation of clang sanitizers
fn getClangPath(alloc: std.mem.Allocator, target: std.Target) ![]const u8 {
    const asan_lib = if (target.os.tag == .windows) "clang_rt.asan_dynamic-x86_64.dll" else "libclang_rt.asan-x86_64.so";
    var child_proc = std.process.Child.init(&.{
        "clang",
        try std.mem.concat(alloc, u8, &.{ "-print-file-name=", asan_lib }),
    }, alloc);
    child_proc.stdout_behavior = .Pipe;

    try child_proc.spawn();

    const reader_buff: []u8 = try alloc.alloc(u8, 512);
    var child_stdout_reader = child_proc.stdout.?.reader(reader_buff);
    const child_stdout = &child_stdout_reader.interface;

    var output = try child_stdout.takeDelimiterExclusive('\n');

    _ = try child_proc.wait();

    const file_delim = if (target.os.tag == .windows) "\\" else "/";

    if (mem.lastIndexOf(u8, output, file_delim)) |last_path_sep| {
        output.len = last_path_sep + 1;
    } else {
        @panic("Path Not Formatted Correctly");
    }
    return output;
}

const runtime_check_flags: []const []const u8 = &.{
    // "-fsanitize=array-bounds,null,alignment,unreachable,address,leak", // asan and leak are linux/macos only in 0.14.1
    "-fstack-protector-strong",
    "-fno-omit-frame-pointer",
};

const warning_flags: []const []const u8 = &.{};

fn getBuildFlags(
    alloc: Allocator,
    exe: *std.Build.Step.Compile,
    optimize: std.builtin.OptimizeMode,
) ![]const []const u8 {
    var cpp_flags: []const []const u8 = undefined;

    if (optimize == .Debug) {
        cpp_flags = additional_flags ++ debug_flags;
        if (exe.rootModuleTarget().os.tag == .windows)
            return cpp_flags;

        exe.addLibraryPath(.{ .cwd_relative = try getClangPath(alloc, exe.rootModuleTarget()) });
        const asan_lib = if (exe.rootModuleTarget().os.tag == .windows) "clang_rt.asan_dynamic-x86_64" // Won't be triggered in current version
            else "clang_rt.asan-x86_64";

        exe.linkSystemLibrary(asan_lib);
    } else {
        cpp_flags = additional_flags;
    }
    return cpp_flags;
}
