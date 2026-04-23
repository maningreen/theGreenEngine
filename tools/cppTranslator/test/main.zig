const std = @import("std");
const Testing = @import("testing");

export fn callstack() callconv(.c) void {
    std.debug.dumpCurrentStackTrace(.{});
}

pub fn main() !void {
    var j = Testing.@"Test<Bar>".init2(3);
    j.deinit();
    var t = Testing.Test2.init1();
    t.deinit();
}
