const std = @import("std");
const rl = @cImport({
    @cInclude("raylib.h");
    @cInclude("raymath.h");
});

const Line = struct {
    m: f32,
    b: f32,

    fn f(self: Line, x: f32) f32 {
        return self.m * x + self.b;
    }

    /// given Y, solve for X
    fn inverseF(self: Line, y: f32) f32 {
        return y / self.m - self.b * self.m;
    }

    fn point(self: Line, x: f32) rl.Vector2 {
        return .{ .x = x, .y = self.f(x) };
    }

    /// returns the closest point on the line to the provided point
    fn getClosestPointToPoint(self: Line, x: rl.Vector2) rl.Vector2 {
        const fPrime: Line = .{ .m = -1 / self.m, .b = x.y + x.x / self.m };
        return self.solve(fPrime) catch |err| switch (err) {
            SolveErrors.InfiniteCollision => x,
            SolveErrors.NoCollision => unreachable,
        };
    }

    fn getDistanceFromPoint(self: Line, x: rl.Vector2) f32 {
        const closestPoint = self.getClosestPointToPoint(x);
        return x.Vector2Distance(closestPoint);
    }

    /// solves a system of equations
    fn solve(a: Line, b: Line) SolveErrors!rl.Vector2 {
        if (a.m == b.m) {
            return if (a.b == b.b)
                SolveErrors.InfiniteCollision
            else
                SolveErrors.NoCollision;
        }
        const y = (b.b - a.b) / (b.m - a.m);
        return rl.Vector2{ .x = a.inverseF(y), .y = y };
    }

    const SolveErrors = error{ InfiniteCollision, NoCollision };
};

pub fn getLineFromPoints(a: rl.Vector2, b: rl.Vector2) Line {
    const dif = rl.Vector2Subtract(a, b);
    const m = dif.y / dif.x;
    return .{ .m = m, .b = a.y + m * a.x };
}

export fn getLineFromPointsAndDistanceTo(a: rl.Vector2, b: rl.Vector2, c: rl.Vector2) callconv(.c) f32 {
    const dist = getLineFromPoints(a, b).getDistanceFromPoint(c);
    std.debug.print("{d}\n", .{dist});
    return dist;
}
