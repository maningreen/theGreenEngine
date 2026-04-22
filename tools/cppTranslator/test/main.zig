const std = @import("std");
const Testing = @import("testing");

extern "c" fn _Z8testFuncv() u64;

pub fn main() !void {
    std.debug.print("{s}", .{"Hello, World!\n"});
    var t: Testing.Test2 = undefined;

    t.testingFunction = @as(*@TypeOf(t.testingFunction), @ptrFromInt(_Z8testFuncv())).*;

    t.testingFunction(&t, 30);
    t.init1();
    std.log.info("{any}", .{t.testingFunction});
    t.testingFunction(&t, 30);
}
