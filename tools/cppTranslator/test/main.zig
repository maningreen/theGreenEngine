const std = @import("std");
const Testing = @import("testing");

pub fn main() !void {
    std.debug.print("{s}", .{"Hello, World!\n"});
    var t = Testing.@"Test<Bar>".init0();
    defer t.deinit();
}
