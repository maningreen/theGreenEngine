const std = @import("std");

fn writeContents(w: *std.Io.Writer, r: *std.Io.Reader) !void {
    var buf: [1028]u8 = undefined;
    while (true) {
        const l = try r.readSliceShort(buf[0..]);
        try w.writeAll(buf[0..l]);
        if (l < buf.len)
            break;
    }
    try w.writeAll("\n");
}

fn readFile(io: std.Io, out: *std.Io.Writer, dir: std.Io.Dir, subPath: []const u8) !void {
    const file = try dir.openFile(io, subPath, .{ .mode = .read_only });
    var readBuf: [1028]u8 = undefined;
    var reader = file.reader(io, readBuf[0..]);
    try writeContents(out, &reader.interface);
}

pub fn main(init: std.process.Init) !void {
    const stdoutFile = std.Io.File.stdout();
    defer stdoutFile.close(init.io);

    var writeBuf: [1028]u8 = undefined;
    var stdoutWriter = stdoutFile.writer(init.io, writeBuf[0..]);
    try stdoutWriter.flush();

    if (init.minimal.args.vector.len == 1)
        std.log.err("requires 1 argument.", .{});

    try stdoutWriter.interface.writeAll("[\n");

    for (init.minimal.args.vector[1..]) |argument| {
        const arg: []const u8 = std.mem.span(argument);
        var dir = try std.Io.Dir.cwd().openDir(init.io, arg, .{ .iterate = true });
        defer dir.close(init.io);
        var it = dir.iterate();
        while (try it.next(init.io)) |f| {
            if (f.kind != .file) continue;
            var file = try dir.openFile(init.io, f.name, .{ .mode = .read_only });
            defer file.close(init.io);

            try readFile(init.io, &stdoutWriter.interface, dir, f.name);
        }
    }
    try stdoutWriter.interface.writeAll("]");
    try stdoutWriter.flush();
}
