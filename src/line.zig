const std = @import("std");
const Line = @This();
const rl = @cImport({
    @cInclude("raylib.h");
    @cInclude("raymath.h");
});

/// if m == null, line is vertical
m: ?f32,
/// if m == null, b is assumed to be what x =
b: f32,

const EvaluateErrors = error{ VerticalLine, HorizontalLine };

/// calculates f(x) = mx + b
/// Will not return EvaluateErrors.HorizontalLine
pub fn f(self: Line, x: f32) EvaluateErrors!f32 {
    if (self.m) |m|
        return m * x + self.b
    else
        return error.VerticalLine;
}

/// given Y, solve for X
/// will not return EvaluateErrors.VerticalLine
pub fn inverseF(self: Line, y: f32) EvaluateErrors!f32 {
    if (self.m) |m|
        return y / m - self.b * m
    else
        return error.HorizontalLine;
}

/// calculates the position of a point on the line given the X
pub fn point(self: Line, x: f32) rl.Vector2 {
    return .{ .x = x, .y = self.f(x) };
}

/// returns the closest point on the line to the provided point
pub fn getClosestPointToPoint(self: Line, x: rl.Vector2) rl.Vector2 {
    if (self.m) |m| {
        if (m == 0) {
            return .{ .x = x.x, .y = self.b };
        }
        const fPrime: Line = .{ .m = -1 / m, .b = x.y + x.x / m };
        return self.solve(fPrime) catch |err| switch (err) {
            SolveErrors.InfiniteCollision => x,
            SolveErrors.NoCollision => unreachable,
        };
    } else return .{ .x = self.b, .y = x.y };
}

/// given a point, calculates the closest point on the line to the point,
/// and the distance
pub fn getDistanceFromPoint(self: Line, x: rl.Vector2) f32 {
    const closestPoint = self.getClosestPointToPoint(x);
    return x.Vector2Distance(closestPoint);
}

pub const SolveErrors = error{ InfiniteCollision, NoCollision };

/// solves a system of equations
pub fn solve(alpha: Line, beta: Line) SolveErrors!Vector2 {
    if (alpha.m == beta.m) {
        return if (alpha.b == beta.b)
            SolveErrors.InfiniteCollision
        else
            SolveErrors.NoCollision;
    }
    if (alpha.m) |am| {
        if (beta.m) |bm| {
            const x = (alpha.b - beta.b) / (bm - am);
            return Vector2{ .x = x, .y = alpha.f(x) catch unreachable };
        } else return Vector2{ .x = beta.b, .y = alpha.f(beta.b) catch unreachable };
    } else return Vector2{ .x = alpha.b, .y = beta.f(alpha.b) catch unreachable };
}
