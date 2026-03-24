//! A simple representation of the `c++` Laser class.
//! Used as the backend for drawing.

const std = @import("std");
const Line = @import("line.zig");
const Laser = @This();
const Vector2 = @import("root").Vector2;
const rl = @import("root").rl;
const distance = @import("root").distance;

/// if `fun` returns .eq, returns `a`
/// see also, `minBy`
fn minBy(T: type, comptime fun: fn (T, T) std.math.Order, a: T, b: T) T {
    const order = fun(a, b);
    return switch (order) {
        .lt, .eq => a,
        .gt => b,
    };
}

/// if `fun` returns .eq, returns `a`
/// see also, `maxBy`
fn maxBy(T: type, comptime fun: fn (T, T) std.math.Order, a: T, b: T) T {
    const order = fun(a, b);
    return switch (order) {
        .lt, .eq => b,
        .gt => a,
    };
}

fn clampBy(T: type, comptime fun: fn (T, T) std.math.Order, lower: T, upper: T, value: T) T {
    return if (fun(lower, value) == .gt)
        lower
    else if (fun(upper, value) == .lt)
        upper
    else
        value;
}

/// The length of the laser.
length: f32,
/// The angle of the laser, Radians.
theta: f32,
/// The origin of the laser.
pos: Vector2,

pub fn calculateOffset(self: Laser) Vector2 {
    return .{ .x = @cos(self.theta) * self.length, .y = @sin(self.theta) * self.length };
}

pub fn calculateLocalEndPoint(self: Laser) Vector2 {
    const off = self.calculateOffset();
    return .{ .x = off.x + self.pos.x, .y = off.y + self.pos.y };
}

pub fn calculateGlobalEndPoint(self: Laser, wrapL: f32) Vector2 {
    return wrap(self.calculateLocalEndPoint(), wrapL);
}

fn wrap(x: Vector2, length: f32) Vector2 {
    return .{
        .x = x.x - 2 * length * @divTrunc(x.x, length),
        .y = x.y - 2 * length * @divTrunc(x.y, length),
    };
}

fn toLine(self: Laser) Line {
    const m = @tan(self.theta);
    return .{ .m = m, .b = m * -self.pos.x + self.pos.y };
}

fn veq(a: Vector2, b: Vector2) bool {
    return a.x == b.x and a.y == b.y;
}

pub fn draw(self: Laser, col: rl.Color, wrapL: f32) void {
    var it = self.iterate(wrapL);
    while (it.next()) |las| {
        const end = las.calculateLocalEndPoint();
        rl.DrawLineV(las.pos, end, col);
    }
}

/// returns a slice of lasers, of which are allocated with `gpa`
/// Takes in one laser, splitting it up so no laser will draw outside a rectangle defined by `wrapL`
pub fn breakUpLaser(self: Laser, gpa: std.mem.Allocator, wrapL: f32) ![]Laser {
    // we'll have at least one
    var lasers = try std.ArrayList(Laser).initCapacity(gpa, 1);

    const end = self.calculateGlobalEndPoint(wrapL);
    const preWrap = self.calculateLocalEndPoint();
    if (veq(end, preWrap) and self.length > 0)
        // we init with cap 1
        return std.ArrayList(Laser).addOneAssumeCapacity(self)
    else {
        const laser = self.toLine();

        const offset = self.calculateOffset();
        const xBorderVertical = if (offset.x < 0) -wrapL else wrapL;
        const yBorderVertical = if (offset.y < 0) -wrapL else wrapL;

        const xBorder: Line = .{ .m = null, .b = xBorderVertical };
        const yBorder: Line = .{ .m = 0, .b = yBorderVertical };

        const xBorderCollision = laser.solve(xBorder) catch unreachable;
        const yBorderCollision = laser.solve(yBorder) catch unreachable;

        const collidesX: bool = @abs(xBorderCollision.x) <= @abs(yBorderCollision.x);

        const collisionPoint = if (collidesX) xBorderCollision else yBorderCollision;

        const flipPoint = if (collidesX) Vector2{
            .x = -collisionPoint.x,
            .y = collisionPoint.y,
        } else Vector2{
            .x = collisionPoint.x,
            .y = -collisionPoint.y,
        };

        const l = distance(collisionPoint, self.pos);
        const nextLaser = Laser{ .pos = flipPoint, .length = self.length - l, .theta = self.theta };
        const thisSector = Laser{ .pos = self.pos, .length = l, .theta = self.theta };
        const all = try nextLaser.breakUpLaser(gpa, wrapL);
        defer gpa.free(all);
        lasers.addOneAssumeCapacity(thisSector);
        lasers.appendSlice(gpa, all);
    }
}

fn getDistanceToLaserCmp(a: Vector2, b: Vector2) std.math.Order {
    return if (a.x <= b.x)
        .lt
    else
        .gt;
}

/// returns the distance to the closest point on the laser, does not wrap
pub fn getDistanceToLaser(self: Laser, p: Vector2) f32 {
    const line = self.toLine();
    const closestPoint = line.getClosestPointToPoint(p);
    const endPos = self.calculateLocalEndPoint();
    const leftBound = minBy(Vector2, getDistanceToLaserCmp, self.pos, endPos);
    const rightBound = maxBy(Vector2, getDistanceToLaserCmp, self.pos, endPos);
    const clampedClosest = clampBy(Vector2, getDistanceToLaserCmp, leftBound, rightBound, closestPoint);
    return distance(clampedClosest, p);
}

pub fn getDistanceToSplitLaser(self: Laser, wrapL: f32, p: Vector2) f32 {
    var it = self.iterate(wrapL);
    var minD: f32 = std.math.floatMax(f32);
    while (it.next()) |next| {
        const dist = @floor(next.getDistanceToLaser(p) / (wrapL * 2)) * wrapL * 2;
        if (minD > dist)
            minD = dist;
    }
    return minD;
}

pub fn iterate(self: Laser, wrapL: f32) SectionIterator {
    return .{ .laser = self, .wrapLength = wrapL };
}

pub const SectionIterator = struct {
    laser: ?Laser,
    wrapLength: f32,

    pub fn next(self: *SectionIterator) ?Laser {
        // we'll have at least one
        if (self.laser) |laser| {
            const end = laser.calculateGlobalEndPoint(self.wrapLength);
            const preWrap = laser.calculateLocalEndPoint();
            if (veq(end, preWrap) and laser.length > 0) {
                const thisSector = self.laser;
                self.laser = null;
                return thisSector;
            } else {
                const line = laser.toLine();

                const offset = laser.calculateOffset();
                const xBorderVertical = if (offset.x < 0) -self.wrapLength else self.wrapLength;
                const yBorderVertical = if (offset.y < 0) -self.wrapLength else self.wrapLength;

                const xBorder: Line = .{ .m = null, .b = xBorderVertical };
                const yBorder: Line = .{ .m = 0, .b = yBorderVertical };

                const xBorderCollision = line.solve(xBorder) catch unreachable;
                const yBorderCollision = line.solve(yBorder) catch unreachable;

                const collidesX: bool = @abs(xBorderCollision.x) <= @abs(yBorderCollision.x);

                const collisionPoint = if (collidesX) xBorderCollision else yBorderCollision;

                const flipPoint = if (collidesX) Vector2{
                    .x = -collisionPoint.x,
                    .y = collisionPoint.y,
                } else Vector2{
                    .x = collisionPoint.x,
                    .y = -collisionPoint.y,
                };

                const l = distance(collisionPoint, laser.pos);
                const nextLaser = Laser{ .pos = flipPoint, .length = laser.length - l, .theta = laser.theta };
                const thisSector = Laser{ .pos = laser.pos, .length = l, .theta = laser.theta };
                self.laser = nextLaser;
                return thisSector;
            }
        } else return null;
    }
};
