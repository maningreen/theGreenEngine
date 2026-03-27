const std = @import("std");

pub fn build(b: *std.Build) void {
    const opt = b.standardOptimizeOption(.{});
    const target = b.standardTargetOptions(.{});

    const mod = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .link_libc = false,
        .optimize = opt,
        .target = target,
    });

    const exe = b.addExecutable(.{
        .name = "ccHelper",
        .root_module = mod,
    });

    const install = b.addInstallArtifact(exe, .{});
    b.getInstallStep().dependOn(&install.step);

    const runStep = b.step("run", "runs the executable");
    const runExe = b.addRunArtifact(exe);
    if (b.args) |args|
        runExe.addArgs(args);
    runStep.dependOn(&runExe.step);
}
