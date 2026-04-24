const std = @import("std");
const xml = @import("xml");
const TokenContainer = @import("container.zig");
const token = @import("token.zig");
const debug = std.debug;

/// Caller owns memory
fn getAST(io: std.Io, gpa: std.mem.Allocator, path: []const u8) ![]u8 {
    const argv = &.{ "castxml", "--castxml-output=1", path, "-o", "-" };
    var child = try std.process.spawn(io, .{
        .argv = argv,
        .stderr = .close, // why is this ignore? well, because we want to ignore global headers.
        .cwd = .inherit,
        .stdin = .ignore,
        .stdout = .pipe,
    });
    const file = child.stdout orelse unreachable;
    var readBuf: [1028]u8 = undefined;
    var stdin = file.reader(io, &readBuf);
    var contents: []u8 = try gpa.alloc(u8, try file.length(io));
    while (true) {
        const line = try stdin.interface.takeDelimiter('\n') orelse break;
        contents = try gpa.realloc(contents, contents.len + line.len + 1);
        std.mem.copyForwards(u8, contents[contents.len - (line.len + 1) ..], line);
        contents[contents.len - 1] = '\n';
    }
    _ = try child.wait(io);
    return contents;
}

/// returned memory is owned by caller.
fn parseTokens(gpa: std.mem.Allocator, input: []const u8) !TokenContainer {
    var xmlReader = std.Io.Reader.fixed(input);

    var streaming_reader: xml.Reader.Streaming = .init(gpa, &xmlReader, .{});
    defer streaming_reader.deinit();

    const reader = &streaming_reader.interface;

    var container = TokenContainer.init();
    errdefer container.deinit(gpa);

    var state: ?TokenContainer.TokenUnion = null;
    while (true) {
        const node = reader.read() catch |err| switch (err) {
            error.MalformedXml => {
                const loc = reader.errorLocation();
                std.log.err("{}:{}: {}", .{ loc.line, loc.column, reader.errorCode() });
                return error.MalformedXml;
            },
            else => |other| return other,
        };
        switch (node) {
            .eof => {
                break;
            },
            .element_start => {
                const element_name = reader.elementNameNs();
                const t = token.getItem(element_name.local);
                if (std.mem.eql(u8, element_name.local, getBaseName(token.Argument))) {
                    if (state != null) switch (state.?) {
                        inline else => |*s| {
                            if (@hasField(@TypeOf(s.*), "arguments")) {
                                if (s.arguments) |*args| {
                                    args.* = try gpa.realloc(args.*, args.len + 1);
                                    var arg: token.Argument = undefined;
                                    for (0..reader.attributeCount()) |i| {
                                        const attribute_name = reader.attributeNameNs(i);
                                        const value = try reader.attributeValue(i);
                                        try token.setValue(token.Argument, &arg, gpa, attribute_name.local, value);
                                    }
                                    args.*[args.len - 1] = arg;
                                } else {
                                    s.arguments = try gpa.alloc(token.Argument, 1);
                                    var arg: token.Argument = undefined;
                                    for (0..reader.attributeCount()) |i| {
                                        const attribute_name = reader.attributeNameNs(i);
                                        const value = try reader.attributeValue(i);
                                        try token.setValue(token.Argument, &arg, gpa, attribute_name.local, value);
                                    }
                                    s.arguments.?[s.arguments.?.len - 1] = arg;
                                }
                            }
                            continue;
                        },
                    };
                }
                if (state != null) continue;
                switch (t orelse continue) {
                    inline else => |v| {
                        const T = token.structType(v);
                        var m: T = undefined;
                        inline for (@typeInfo(T).@"struct".fields) |field| {
                            if (field.defaultValue()) |d| @field(m, field.name) = d;
                        }
                        for (0..reader.attributeCount()) |i| {
                            const attribute_name = reader.attributeNameNs(i);
                            const value = try reader.attributeValue(i);
                            try token.setValue(token.structType(v), &m, gpa, attribute_name.local, value);
                        }
                        state = @unionInit(TokenContainer.TokenUnion, getBaseName(T), m);
                    },
                }
            },
            .element_end => {
                if (std.mem.eql(u8, reader.elementName(), getBaseName(token.Argument)))
                    continue;

                switch (state orelse continue) {
                    inline else => |v| try container.append(gpa, v),
                }

                state = null;
            },
            else => continue,
        }
    }
    return container;
}

fn printFile(io: std.Io, gpa: std.mem.Allocator, out: *std.Io.Writer, file: []const u8) !void {
    const ret = try getAST(io, gpa, file);
    defer gpa.free(ret);
    var container = try parseTokens(gpa, ret);
    defer container.deinit(gpa);
    try token.Namespace.write(null, gpa, container, out);
}

pub fn main(init: std.process.Init) !void {
    if (init.minimal.args.vector.len == 1) return;
    const file = std.Io.File.stdout();
    defer file.close(init.io);
    var writeBuf: [1028]u8 = undefined;
    var writer = file.writer(init.io, &writeBuf);
    for (init.minimal.args.vector[1..]) |arg| {
        const l = std.mem.len(arg);
        try printFile(init.io, init.gpa, &writer.interface, arg[0..l]);
    }
    try writer.flush();
}
