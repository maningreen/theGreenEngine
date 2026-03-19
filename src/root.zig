//! Root zig file.
//! Has general functions, and all exported functions.

const std = @import("std");
const Line = @import("line.zig");
const Laser = @import("laser.zig");
pub const rl = @cImport({
    @cInclude("raylib.h");
});
pub const Vector2 = rl.Vector2;

pub fn distance(a: Vector2, b: Vector2) f32 {
    const delta = Vector2{.x = a.x - b.x, .y = a.y - b.y};
    return @sqrt(delta.x * delta.x + delta.y * delta.y);
}

pub fn getLineFromPoints(a: rl.Vector2, b: rl.Vector2) Line {
    const dif: Vector2 = .{ .x = a.x - b.x, .y = a.y - b.y };
    const m = dif.y / dif.x;
    return .{ .m = m, .b = a.y + m * a.x };
}

export fn getLineFromPointsAndDistanceTo(a: rl.Vector2, b: rl.Vector2, c: rl.Vector2) callconv(.c) f32 {
    const line = getLineFromPoints(a, b);
    const dist = line.getDistanceFromPoint(.{ .x = c.x, .y = c.y }); // this is necessary for some reason :p
    return dist;
}

export fn drawLaser(length: f32, theta: f32, pos: rl.Vector2, wrapLength: f32, col: rl.Color) callconv(.c) void {
    const laser: Laser = .{ .pos = .{ .x = pos.x, .y = pos.y }, .length = length, .theta = theta };
    laser.draw(.{ .r = col.r, .g = col.g, .b = col.b, .a = col.a }, wrapLength);
}

// export fn getDistanceToLaser(length: f32, theta: f32, pos: Vector2, wrapLength: f32, p: Vector2) callconv(.c) f32 {
    // return (Laser{.length = length, .pos = pos, .theta = theta}).getDistanceToSplitLaser(wrapLength, p);
// }
