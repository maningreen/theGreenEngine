const std = @import("std");
const Testing = @import("testing");

pub fn main() !void {
    const t = Testing.@"Test2".init1();
    _ = t;
}
