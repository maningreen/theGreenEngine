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

    const xmlDep = b.dependency("xml", .{ .target = target, .optimize = optimize });
    const xml = xmlDep.module("xml");
    main.addImport("xml", xml);

    _ = b.installArtifact(exe);

    const runStep = b.step("run", "Runs the artifact");
    const run = b.addRunArtifact(exe);
    runStep.dependOn(&run.step);

    if (b.args) |args| {
        run.addArgs(args);
    }
}
