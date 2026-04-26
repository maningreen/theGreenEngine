const std = @import("std");
const Testing = @import("testing");

export fn callstack() callconv(.c) void {
    std.debug.dumpCurrentStackTrace(.{});
}

pub fn main() !void {
    var t = Testing.Foo.@"Test2".init0();
    defer t.deinit();
    _ = t._vtable.@"testingFunction"(&t, 30);
}
