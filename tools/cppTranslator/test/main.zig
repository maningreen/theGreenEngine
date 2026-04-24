const std = @import("std");
const Testing = @import("testing");

export fn callstack() callconv(.c) void {
    std.debug.dumpCurrentStackTrace(.{});
}

pub fn main() !void {
    var class = Testing.Foo.@"Test2".init1();
    defer class.deinit();
}
