const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const main = b.addModule("root", .{
        .root_source_file = b.path("src/main.zig"),
        .optimize = optimize,
        .target = target,
        .link_libc = true,
    });

    const exe = b.addExecutable(.{
        .root_module = main,
        .name = "translator",
    });

    const llvmBindings = b.dependency("llvm_zig", .{ .target = target, .optimize = optimize });
    const clangBindings = llvmBindings.module("clang-zig");
    main.addImport("clang", clangBindings);

    main.linkSystemLibrary("clang", .{ .needed = true });

    const install = b.addInstallArtifact(exe, .{});
    b.getInstallStep().dependOn(&install.step);

    const runStep = b.step("run", "Runs the artifact");
    const run = b.addRunArtifact(exe);
    runStep.dependOn(&run.step);

    if (b.args) |args| {
        run.addArgs(args);
    }
}
