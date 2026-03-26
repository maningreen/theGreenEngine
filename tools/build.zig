const std = @import("std");

pub fn build(b: *std.Build) void {
    const color = b.dependency("colorHelper", .{});
    const colorHelperExe = color.artifact("colorHelper");
    const installColor = b.addInstallArtifact(colorHelperExe, .{});
    b.getInstallStep().dependOn(&installColor.step);

    const header = b.dependency("headerTranslator", .{});
    const headerExe = header.artifact("translator");
    const installHeader = b.addInstallArtifact(headerExe, .{});
    b.getInstallStep().dependOn(&installHeader.step);
}
