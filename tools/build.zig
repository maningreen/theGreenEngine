const std = @import("std");

pub fn build(b: *std.Build) void {
    const opt = b.standardOptimizeOption(.{});
    const target = b.standardTargetOptions(.{});

    const color = b.dependency("colorHelper", .{ .target = target, .optimize = opt });
    const colorHelperExe = color.artifact("colorHelper");
    const installColor = b.addInstallArtifact(colorHelperExe, .{});
    b.getInstallStep().dependOn(&installColor.step);

    const header = b.dependency("headerTranslator", .{ .target = target, .optimize = opt });
    const headerExe = header.artifact("translator");
    const installHeader = b.addInstallArtifact(headerExe, .{});
    b.getInstallStep().dependOn(&installHeader.step);
}
