const std = @import("std");
const Line = @import("line.zig");
const rl = @cImport({
    @cInclude("raylib.h");
    @cInclude("raymath.h");
});

pub fn getLineFromPoints(a: rl.Vector2, b: rl.Vector2) Line {
    const dif = rl.Vector2Subtract(a, b);
    const m = dif.y / dif.x;
    return .{ .m = m, .b = a.y + m * a.x };
}

export fn getLineFromPointsAndDistanceTo(a: rl.Vector2, b: rl.Vector2, c: rl.Vector2) callconv(.c) f32 {
    const line = getLineFromPoints(a, b);
    const dist = line.getDistanceFromPoint(.{ .x = c.x, .y = c.y }); // this is necessary for some reason :p
    std.debug.print("{d}\n", .{dist});
    return dist;
}
