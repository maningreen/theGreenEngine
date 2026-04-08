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
    const headerLib = header.artifact("helper");
    const installHeaderExe = b.addInstallArtifact(headerExe, .{});
    const installHeaderLib = b.addInstallArtifact(headerLib, .{});
    b.getInstallStep().dependOn(&installHeaderExe.step);
    b.getInstallStep().dependOn(&installHeaderLib.step);

    const libMod = b.createModule(.{
        .optimize = opt,
        .target = target,
        .root_source_file = b.path("./tools.zig"),
        .imports = &.{
            .{
                .name = "helper",
                .module = headerLib.root_module,
            },
        },
    });
    const lib = b.addLibrary(.{
        .name = "tools",
        .root_module = libMod,
    });
    b.getInstallStep().dependOn(&lib.step);
}
