const std = @import("std");

export fn testFunction() void {
    var stdout = std.fs.File.stdout();
    _ = stdout.write("WOOOOOOOOO\n") catch @panic("FUCK");
    stdout.close();
}
