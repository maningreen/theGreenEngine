const std = @import("std");
const Testing = @import("testing");

pub fn main() !void {
    std.debug.print("{s}", .{"Hello, World!\n"});
    var t = Testing.Test{ .thingy = 'a' };
    defer t.deinit();
    _ = t.print(&t);
}
