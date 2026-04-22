const std = @import("std");
const Testing = @import("testing");

extern "c" fn _Z8testFuncv() u64;
extern "c" fn _ZN3Foo5Test2C1Ev(*Testing.Test2) void;
extern "c" fn _ZN3Foo5Test215testingFunctionEd(*Testing.Test2, f64) void;

pub fn main() !void {
    std.debug.print("{s}", .{"Hello, World!\n"});
    var t: Testing.Test2 = undefined;
    _ZN3Foo5Test2C1Ev(&t);
}
