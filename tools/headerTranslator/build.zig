const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const main = b.addModule("root", .{ .root_source_file = b.path("src/main.zig"), .optimize = optimize, .target = target });

    const exe = b.addExecutable(.{ .root_module = main, .name = "translator" });

    const dep = b.dependency("mvzr", .{});
    main.addImport("mvzr", dep.module("mvzr"));

    const install = b.addInstallArtifact(exe, .{});
    b.getInstallStep().dependOn(&install.step);

    const runStep = b.step("run", "Runs the artifact");
    const run = b.addRunArtifact(exe);
    runStep.dependOn(&run.step);

    if (b.args) |args| {
        run.addArgs(args);
    }
}
