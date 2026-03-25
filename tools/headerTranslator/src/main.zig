const std = @import("std");
const mvzr = @import("mvzr");

pub const ParsedStruct = struct {
    name: []const u8,
    members: [][]const u8,

    pub const parseError = error{
        MultipleOpenings,
        NoOpening,
        NoClosing,
    };

    /// This function allocates space for the members, not for the strings
    /// Strings are expected to be pre-allocated.
    /// Ignores all `function` tokens.
    pub fn init(gpa: std.mem.Allocator, tokens: []const Token) (parseError || std.mem.Allocator.Error)!?ParsedStruct {
        var name: ?[]const u8 = null;

        var members = std.ArrayList([]const u8).empty;
        errdefer members.deinit(gpa);

        var closing = false;
        for (tokens) |token| {
            switch (token) {
                .structOpening => |n| {
                    if (name) |_| {
                        return parseError.MultipleOpenings;
                    } else name = n;
                },
                .structMember => |member| {
                    if (name) |_|
                        try members.append(gpa, member)
                    else
                        return parseError.NoOpening;
                },
                .structClosing => {
                    if (name) |_| {
                        closing = true;
                        break;
                    } else return parseError.NoOpening;
                },
                .function => continue,
            }
        }

        if (!closing)
            return parseError.NoClosing;

        return if (name) |n| ParsedStruct{ .name = n, .members = try members.toOwnedSlice(gpa) } else return parseError.NoOpening;
    }

    pub fn deinit(self: *ParsedStruct, gpa: std.mem.Allocator) void {
        gpa.free(self.members);
    }
};

const Token = union(TokenType) {
    structOpening: []u8,
    structMember: []u8,
    structClosing: []u8,
    function: []u8,
    const TokenType = enum {
        structOpening,
        structMember,
        structClosing,
        function,
    };
    pub fn init(t: TokenType, value: anytype) Token {
        switch (t) {
            inline else => |v| {
                return @unionInit(Token, @tagName(v), value);
            },
        }
    }
};

const Parser = struct {
    pub const functionReg: mvzr.Regex = mvzr.Regex.compile(functionPattern) orelse unreachable;
    pub const structOpeningReg: mvzr.Regex = mvzr.Regex.compile(structOpeningPattern) orelse unreachable;
    pub const structMemberReg: mvzr.Regex = mvzr.Regex.compile(structMemberPattern) orelse unreachable;
    pub const structClosingReg: mvzr.Regex = mvzr.Regex.compile(structClosingPattern) orelse unreachable;

    pub const functionPattern =
        \\RLAPI\s\w+\s(\w+)\(.*\);\s+//\s(.+)
    ;
    pub const structOpeningPattern =
        \\typedef\sstruct\s\w+\s\{
    ;
    pub const structMemberPattern =
        \\\s+\w+\s\w+;\s+//\s.+
    ;
    pub const structClosingPattern =
        \\\}\s\w+;
    ;

    pub fn getPattern(t: Token.TokenType) []const u8 {
        switch (t) {
            inline else => |v| {
                const key: []const u8 = std.fmt.comptimePrint("{s}Pattern", .{@tagName(v)});
                const pattern = @field(@This(), key);
                return pattern;
            },
        }
    }

    pub fn getRegex(t: Token.TokenType) mvzr.Regex {
        switch (t) {
            inline else => |v| {
                const key: []const u8 = std.fmt.comptimePrint("{s}Reg", .{@tagName(v)});
                const r = @field(@This(), key);
                return r;
            },
        }
    }

    pub fn getType(str: []const u8) ?Token.TokenType {
        inline for (std.enums.values(Token.TokenType)) |t| {
            const regex = getRegex(t);
            if (regex.match(str)) |_|
                return t;
        }
        return null;
    }

    pub fn getCaptures(str: []const u8) ?[]const u8 {
        const t = getType(str) orelse return null;
        const subReg = mvzr.compile(switch (t) {
            .function => "\\w+\\(",
            .structOpening => "\\w+\\s\\{",
            .structMember, .structClosing => "\\w+;",
        }) orelse unreachable;
        var ret = (subReg.match(str) orelse unreachable).slice;
        switch (t) {
            .function => {
                // in the subReg we define the str for regex with the (
                // here we remove that
                ret.len -= 1;
            },
            .structOpening => {
                // in subReg it has \s and \{
                ret.len -= 2;
            },
            .structMember, .structClosing => {
                ret.len -= 1;
            },
        }
        return ret;
    }

    /// Dynamically allocates space for the token (str). User takes ownership of the data.
    pub fn buildToken(gpa: std.mem.Allocator, str: []const u8) !?Token {
        const tokenType = getType(str) orelse return null;
        const capture = getCaptures(str) orelse return null;
        switch (tokenType) {
            else => {
                const hString = try gpa.alloc(u8, capture.len);
                @memmove(hString, capture);
                return Token.init(tokenType, hString);
            },
        }
    }
};

/// compiles tokens into a string for the `sol` library
/// caller owns returned memory
pub fn buildTokens(gpa: std.mem.Allocator, tokens: []Token) ![]u8 {
    // step one, create structures
    var functions = std.ArrayList(Token).empty;
    defer functions.deinit(gpa);
    var structs = std.ArrayList(ParsedStruct).empty;
    defer {
        for (structs.items) |*item|
            item.deinit(gpa);
        structs.deinit(gpa);
    }
    for (tokens, 0..) |token, i| {
        switch (token) {
            .structClosing, .structMember => continue,
            .structOpening => try structs.append(gpa, (try ParsedStruct.init(gpa, tokens[i..])) orelse unreachable),
            .function => try functions.append(gpa, token),
        }
    }
    const structOpeningFmt =
        \\{{
        \\    sol::usertype<{s}> {s} = lua.new_usertype<{s}>("{s}");
        \\
    ;
    const structMemberFmt =
        \\    {s}["{s}"] = &{s}::{s};
        \\
    ;
    const structCloseFmt =
        \\}}
        \\
    ;
    const functionFmt =
        \\lua["{s}"] = &{s};
        \\
    ;

    var writer = try std.Io.Writer.Allocating.initCapacity(gpa, tokens.len + @divTrunc((structCloseFmt.len + structMemberFmt.len + structOpeningFmt.len + functionFmt.len), 4));
    defer writer.deinit();

    for (functions.items) |f|
        switch (f) {
            .function => |name| _ = try writer.writer.print(functionFmt, .{ name, name }),
            else => unreachable,
        };

    for (structs.items) |s| {
        try writer.writer.print(structOpeningFmt, .{ s.name, s.name, s.name, s.name });
        for (s.members) |member|
            try writer.writer.print(structMemberFmt, .{ s.name, member, s.name, member });
        try writer.writer.print(structCloseFmt, .{});
    }

    return try writer.toOwnedSlice();
}

fn manageFile(gpa: std.mem.Allocator, io: std.Io, file: std.Io.File, out: *std.Io.Writer) !void {
    var readBuf: [1028]u8 = undefined;
    var reader = file.reader(io, &readBuf);

    var tokens = try std.ArrayList(Token).initCapacity(gpa, 10);

    defer {
        for (tokens.items) |token|
            switch (token) {
                inline else => |v| gpa.free(v),
            };
        tokens.deinit(gpa);
    }

    while (try reader.interface.takeDelimiter('\n')) |line| {
        const token = try Parser.buildToken(gpa, line) orelse continue;
        try tokens.append(gpa, token);
    }

    const x = try buildTokens(gpa, tokens.items);
    defer gpa.free(x);
    _ = try out.writeAll(x);
}

pub fn main(init: std.process.Init) !void {
    if (init.minimal.args.vector.len > 1) {} else {}

    const stdout = std.Io.File.stdout();
    var writeBuf: [1028]u8 = undefined;
    var writer = stdout.writer(init.io, &writeBuf);

    for (init.minimal.args.vector[1..]) |arg| {
        const r = try std.zig.system.NativePaths.detect(init.arena.allocator(), init.io, &@import("builtin").target, init.environ_map);
        std.log.info("Searching for: {s}", .{arg});
        items: for (r.include_dirs.items) |path| {
            const dir = try std.Io.Dir.openDirAbsolute(init.io, path, .{ .iterate = true });
            defer dir.close(init.io);
            var it = dir.iterate();
            while (try it.next(init.io)) |f| {
                if (f.kind != .file) continue;
                if (std.mem.endsWith(u8, f.name, arg[0..std.mem.len(arg)])) {
                    std.log.info("{s} found, parsing...", .{arg});
                    const file = try dir.openFile(init.io, f.name, .{});
                    defer file.close(init.io);
                    try manageFile(init.gpa, init.io, file, &writer.interface);
                    std.log.info("done parsing!", .{});
                    break :items;
                }
            }
        } else std.log.err("{s} not found, continuing", .{arg});
    } else if (init.minimal.args.vector.len == 0) {
        std.log.info("Listening on stdin", .{});
        const file = std.Io.File.stdin();
        defer file.close(init.io);
        try manageFile(init.gpa, init.io, file, &writer.interface);
    }
}
