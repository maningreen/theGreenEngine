const std = @import("std");
const Testing = @import("testing");

export fn callstack() callconv(.c) void {
    std.debug.dumpCurrentStackTrace(.{});
}

pub fn main() !void {
    var class = Testing.Foo.Test2.init1();

    const vtable: [*]*anyopaque = @ptrCast(@alignCast(class._vtable));
    _ = @as(*const fn (*Testing.Foo.@"Test2", f64) f64, @alignCast(@ptrCast(vtable[0])))(&class, 30.0);
    _ = class.testingFunction(30);
    class.deinit();
}
