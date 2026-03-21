const std = @import("std");

pub fn main(init: std.process.Init) !void {
    var stdin = std.Io.File.stdin();
    defer stdin.close(init.io);

    var stdout = std.Io.File.stdout();
    defer stdout.close(init.io);

    var outBuffer: [1028]u8 = undefined;

    var output = stdout.writerStreaming(init.io, &outBuffer);

    var inputBuffer: [1028]u8 = undefined;

    var input = stdin.reader(init.io, &inputBuffer);

    while (true) {
        const line = input.interface.takeSentinel('\n') catch |err| switch (err) {
            error.EndOfStream => break,
            else => return err,
        };
        const toParse = std.mem.trim(u8, line, "#\n \t\n\t");
        std.log.debug("{s}\n", .{toParse});
        const parsed = try std.fmt.parseInt(u24, toParse, 16);
        const r: f128 = @as(f128, @floatFromInt((parsed & 0x0000FF) >> 0)) / 255.0;
        const g: f128 = @as(f128, @floatFromInt((parsed & 0x00FF00) >> 8)) / 255.0;
        const b: f128 = @as(f128, @floatFromInt((parsed & 0xFF0000) >> 16)) / 255.0;
        try output.interface.print("vec4({d}, {d}, {d}, 1),\n", .{r, g, b});
    }

    try output.flush();
}
