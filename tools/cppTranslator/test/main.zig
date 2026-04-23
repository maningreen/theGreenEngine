const std = @import("std");
const Testing = @import("testing");

export fn callstack() callconv(.c) void {
    std.debug.dumpCurrentStackTrace(.{});
}

pub fn main() !void {
    const t: f32 = Testing.Foo.sum(3, 3);
    std.debug.print("{d}", .{ t });
}
