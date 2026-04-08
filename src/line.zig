//! A struct representing a linear equation in the form `mx + b`
//! There's one edge case:
//!     `m == null`
//! plotted on a cartesian coordinate system, this would be a vertical line
//! if this is the case, the member `b` is expected to satisfy the following equation
//! `x = b`

const std = @import("std");
const rl = @cImport({
    @cInclude("raylib.h");
});
pub const Vector2 = @import("root").Vector2;

pub fn Line(T: type) type {
    return struct {
        /// slope of the line.
        /// if m == null, line is vertical
        m: ?T,
        /// if m == null, b is assumed to be what x =
        b: T,

        const EvaluateErrors = error{ VerticalLine, HorizontalLine };

        /// calculates f(x) according to mx + b
        /// Will not return EvaluateErrors.HorizontalLine
        pub fn f(self: @This(), x: T) EvaluateErrors!T {
            if (self.m) |m|
                return (m * x) + self.b
            else if (self.b == x)
                return x
            else
                return error.VerticalLine;
        }

        /// given Y, solve for X
        /// will not return EvaluateErrors.VerticalLine
        pub fn inverseF(self: @This(), y: T) EvaluateErrors!T {
            if (self.m) |m|
                return y / m - self.b * m
            else if (self.b == y)
                return y
            else
                return error.HorizontalLine;
        }

        /// calculates the position of a point on the line given the X
        pub fn point(self: @This(), x: T) EvaluateErrors!Vector2 {
            return .{ .x = x, .y = try self.f(x) };
        }

        /// calculates the position of a point on the line given the Y
        pub fn inversePoint(self: @This(), y: T) EvaluateErrors!Vector2 {
            return .{ .x = try self.inverseF(y), .y = y };
        }

        /// returns the closest point on the line to the provided point
        pub fn getClosestPointToPoint(self: @This(), x: Vector2) Vector2 {
            if (self.m) |m| {
                if (m == 0)
                    return .{ .x = x.x, .y = self.b };
                const fPrime: @This() = .{ .m = -1 / m, .b = x.y + x.x / m };
                // the slopes are defined to be different, causing both exceptions to be impossible
                return self.solve(fPrime) catch unreachable;
            } else return .{ .x = self.b, .y = x.y };
        }

        /// given a point, calculates the closest point on the line to the point,
        /// and the distance
        /// internally calls `getDistanceFromPointSqr`
        pub fn getDistanceFromPoint(self: @This(), x: Vector2) T {
            return @sqrt(self.getDistanceFromPointSqr(x));
        }

        /// given a point, calculates the closest point on the line to the point,
        /// and the distance squared
        pub fn getDistanceFromPointSqr(self: @This(), x: Vector2) T {
            const closestPoint = self.getClosestPointToPoint(x);
            const delta: Vector2 = .{ .x = closestPoint.x - x.x, .y = closestPoint.y - x.y };
            return delta.x * delta.x + delta.y * delta.y;
        }

        pub const SolveErrors = error{ InfiniteCollision, NoCollision };

        /// solves a system of equations
        pub fn solve(alpha: @This(), beta: @This()) SolveErrors!Vector2 {
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
    };
}
