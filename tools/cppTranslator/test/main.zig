const std = @import("std");
const Testing = @import("testing");

export fn callstack() callconv(.c) void {
    std.debug.dumpCurrentStackTrace(.{});
}

pub fn main() !void {
    var t = Testing.Foo.@"Test2".init0();
    const sum = Testing.Foo.sum(30);
    defer t.deinit();
    t.testingFunction(30);
    
    std.log.debug("sum: {d}", .{ sum });
}
