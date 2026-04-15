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

const Library = struct {
    name: []const u8,
    searchName: ?[]const u8 = null,
    type: enum {
        dependency,
        systemLib,
    },
};

const libraries: []const Library = &.{
    .{
        .name = "raylib",
        .type = .dependency,
    },
    .{
        .name = "lua",
        .type = .dependency,
    },
    .{
        .name = "GL",
        .type = .systemLib,
    },
};

pub fn build(b: *std.Build) void {
    var threaded = std.Io.Threaded.init_single_threaded;
    defer threaded.deinit();

    const io = threaded.io();

    const target = b.standardTargetOptions(.{});

    const optimize = b.standardOptimizeOption(.{});

    const cmds = b.step("cmds", "generates compile_commands.json");

    const exe_mod = b.addModule("exe", .{
        .target = target,
        .optimize = optimize,
        .link_libcpp = true,
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
        .name = "zig_engine",
        .root_module = zigMod,
        .linkage = .static,
    });

    exe.root_module.linkLibrary(zig_lib);

    for (libraries) |lib| {
        switch (lib.type) {
            .dependency => {
                const dep = b.dependency(lib.name, .{});
                const artifact = dep.artifact(lib.searchName orelse lib.name);

                zig_lib.installLibraryHeaders(artifact);
                exe.installLibraryHeaders(artifact);

                zig_lib.root_module.linkLibrary(artifact);
                exe.root_module.linkLibrary(artifact);
            },
            .systemLib => {
                if (lib.searchName) |p| {
                    zig_lib.root_module.addLibraryPath(b.path(p));
                    zig_lib.root_module.addIncludePath(b.path(p));
                    exe.root_module.addLibraryPath(b.path(p));
                    exe.root_module.addIncludePath(b.path(p));
                }
                exe.root_module.linkSystemLibrary(lib.name, .{ .preferred_link_mode = .dynamic });
                zig_lib.root_module.linkSystemLibrary(lib.name, .{ .preferred_link_mode = .dynamic });
            },
        }
    }

    b.installArtifact(exe);
    const exe_run = b.addRunArtifact(exe);

    exe_run.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        exe_run.addArgs(args);
    }

    const run_step = b.step("run", "runs the application");
    run_step.dependOn(&exe_run.step);

    const tools = b.dependency("tools", .{});
    const translator = tools.artifact("translator");

    const glueStep = b.step("glue", "generate glue for sol");
    exe.step.dependOn(glueStep);

    glueStep.dependOn(&translator.step);
    const runTranslator = b.addRunArtifact(translator);
    glueStep.dependOn(&runTranslator.step);

    const rl = b.dependency("raylib", .{});
    const rlArtifact = rl.artifact("raylib");

    runTranslator.step.dependOn(&rlArtifact.step);
    const headers = rlArtifact.installed_headers;
    for (headers.items) |header|
        if (std.mem.eql(u8, header.file.source.basename(b, &runTranslator.step), "raylib.h"))
            runTranslator.addArgs(&.{
                \\--silent
                ,
                \\--ignore=.{"capacity"}
                ,
                header.file.dest_rel_path,
            });
    const stdout = runTranslator.captureStdOut(.{ .basename = "glue.h" });
    exe_mod.addIncludePath(stdout.dirname());

    const cc = CompileCommands.fromCompile(b, exe) catch |err| @panic(@errorName(err));
    cmds.dependOn(&cc.step);
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

    const extension = switch (opts.language) {
        inline .c, .cpp => |c| @tagName(c),
        .assembly => ".s",
        .objective_c => ".m",
        .objective_cpp => ".mm",
        .assembly_with_preprocessor => undefined,
    };

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

const CompileCommands = struct {
    step: Build.Step,
    obj: []const std.Build.Module.LinkObject,
    gen: std.Build.GeneratedFile,

    pub fn fromCompile(b: *Build, cpl: *const std.Build.Step.Compile) !*CompileCommands {
        const cc = try CompileCommands.fromLinkObjects(b, cpl.root_module.link_objects.items);
        for (cpl.step.dependencies.items) |dep|
            cc.step.dependOn(dep);
        return cc;
    }

    fn fromLinkObjects(b: *Build, objs: []std.Build.Module.LinkObject) !*CompileCommands {
        const self = try b.allocator.create(CompileCommands);
        const step = Build.Step.init(.{
            .id = .custom,
            .name = "generate compile_commands.json",
            .owner = b,
            .makeFn = make,
        });
        self.* = .{
            .step = step,
            .gen = .{ .step = &self.step },
            .obj = objs,
        };
        for (self.obj) |object| {
            switch (object) {
                .other_step => |other| {
                    self.step.dependOn(other.getEmittedIncludeTree().generated.file.step);
                },
                else => {},
            }
        }
        return self;
    }

    fn make(step: *Build.Step, opts: Build.Step.MakeOptions) anyerror!void {
        _ = opts;
        var threaded = std.Io.Threaded.init_single_threaded;
        const io = threaded.io();
        const self: *CompileCommands = @fieldParentPtr("step", step);
        try self.write(io, step.owner);
    }

    pub fn write(self: *CompileCommands, io: std.Io, b: *Build) !void {
        const gpa = b.allocator;
        // the way this one works is *scanning*.
        // we go through one of the type, then the next.
        var sysLibs = std.ArrayList(Build.Module.SystemLib).empty;
        var localLibs = std.ArrayList(*Build.Step.Compile).empty;
        for (self.obj) |object| {
            switch (object) {
                .system_lib => |lib| try sysLibs.append(gpa, lib),
                .other_step => |s| {
                    switch (s.step.id) {
                        .compile => {
                            const comp: *std.Build.Step.Compile = @fieldParentPtr("step", &s.step);
                            switch (comp.kind) {
                                .lib => try localLibs.append(gpa, comp),
                                else => {},
                            }
                        },
                        else => {},
                    }
                },
                else => continue,
            }
        }
        var tFlags = std.StringHashMap(void).init(b.allocator);
        var paths = std.ArrayList([]const u8).empty;
        for (self.obj) |object| {
            switch (object) {
                .c_source_file => |src| {
                    for (src.flags) |flag|
                        try tFlags.put(flag, void{});
                    try paths.append(gpa, src.file.cwd_relative);
                },
                .c_source_files => |srcs| {
                    for (srcs.flags) |flag|
                        try tFlags.put(flag, void{});
                    try paths.appendSlice(gpa, srcs.files);
                },
                else => continue,
            }
        }

        var libFlags = std.ArrayList([]const u8).empty;
        for (sysLibs.items) |lib|
            try libFlags.append(
                gpa,
                try std.fmt.allocPrint(gpa, "-l{s}", .{lib.name}),
            );
        for (localLibs.items) |lib| {
            const p = try lib.getEmittedIncludeTree().getPath4(b, &self.step);
            try libFlags.appendSlice(gpa, &.{
                try std.fmt.allocPrint(gpa, "-L{s}", .{p.sub_path}),
                try std.fmt.allocPrint(gpa, "-I{s}", .{p.sub_path}),
                try std.fmt.allocPrint(gpa, "-l{s}", .{lib.name}),
            });
        }
        var parsedFlags = std.ArrayList([]const u8).empty;
        {
            var it = tFlags.keyIterator();
            while (it.next()) |flag|
                try parsedFlags.append(gpa, flag.*);
        }

        const Item = struct {
            file: []const u8,
            arguments: []const []const u8,
            directory: []const u8,
            output: []const u8,
        };
        var items = std.ArrayList(Item).empty;
        const cwd = try std.Io.Dir.cwd().realPathFileAlloc(io, ".", b.allocator);

        const flags: []const []const u8 = try std.mem.concat(
            b.allocator,
            []const u8,
            &.{
                parsedFlags.items,
                libFlags.items,
            },
        );

        for (paths.items) |value| {
            const localPath = value;
            try items.append(b.allocator, .{
                .file = localPath,
                .arguments = try std.mem.concat(b.allocator, []const u8, &.{
                    &.{
                        "clang",
                        "-c",
                        localPath,
                        "-o",
                        try std.fmt.allocPrint(
                            b.allocator,
                            "{s}.o",
                            .{
                                localPath,
                            },
                        ),
                    },
                    flags,
                    additional_flags,
                    debug_flags,
                }),
                .output = try std.fmt.allocPrint(gpa, "{s}.o", .{localPath}),
                .directory = cwd,
            });
        }
        var tempWriter = std.Io.Writer.Allocating.init(b.allocator);
        var stringifiery = std.json.Stringify{
            .writer = &tempWriter.writer,
            .options = .{
                .whitespace = .indent_4,
            },
        };
        try stringifiery.beginArray();
        for (items.items) |value|
            try stringifiery.write(value);
        try stringifiery.endArray();
        try b.cache_root.handle.writeFile(
            b.graph.io,
            .{
                .sub_path = "compile_commands.json",
                .data = try tempWriter.toOwnedSlice(),
            },
        );
        self.gen.path = try b.cache_root.join(
            b.allocator,
            &.{"compile_commands.json"},
        );
        const s = try self.step.installFile(
            .{
                .generated = .{
                    .file = &self.gen,
                },
            },
            try std.fmt.allocPrint(
                gpa,
                "{s}/compile_commands.json",
                .{
                    b.install_prefix,
                },
            ),
        );
        self.step.result_cached = s == .fresh;
    }
};
