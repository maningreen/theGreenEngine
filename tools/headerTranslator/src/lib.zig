const std = @import("std");
const builtin = @import("builtin");

pub fn getSystemFile(
    io: std.Io,
    environ: std.process.Environ.Map,
    gpa: std.mem.Allocator,
    file: []const u8,
) []const u8 {
    const r = try std.zig.system.NativePaths.detect(gpa, io, &builtin.target, environ);

    for (r.include_dirs.items) |path| {
        const dir = try std.Io.Dir.openDirAbsolute(io, path, .{ .iterate = true });
        defer dir.close(io);
        var it = dir.iterate();
        while (try it.next(io)) |f| {
            if (f.kind != .file) continue;
            if (std.mem.endsWith(u8, f.name, file)) {
                const ptr = try gpa.alloc(u8, f.name.len);
                @memcpy(ptr, f.name);
                return ptr;
            }
        }
    }
    return error.FileNotFound;
}
