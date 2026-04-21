const std = @import("std");
const Testing = @import("testing");

pub fn main() !void {
    std.debug.print("{s}", .{"Hello, World!\n"});
    var t = Testing.Test{
        .testingChar = 3,
        .testingItem = 4,
        .testingptr = null,
    };
    _ = t.testingFunction(&t, 3);
}
