const std = @import("std");

pub fn build(b: *std.Build) void {
    const color = b.dependency("colorHelper", .{});
    const colorHelperExe = b.addExecutable(.{ .name = "colorHelper", .root_module = color.module("root") });
    const installColor = b.addInstallArtifact(colorHelperExe, .{});
    b.getInstallStep().dependOn(&installColor.step);

    const header = b.dependency("headerTranslator", .{});
    const headerExe = b.addExecutable(.{ .name = "translator", .root_module = header.module("root") });
    const installHeader = b.addInstallArtifact(headerExe, .{});
    b.getInstallStep().dependOn(&installHeader.step);
}
