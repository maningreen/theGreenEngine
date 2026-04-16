const std = @import("std");
const xml = @import("xml");
const debug = std.debug;

fn ensure(x: bool) error{False}!void {
    if (!x) return error.False;
}

fn isFundamental(comptime T: type) bool {
    return switch (@typeInfo(T)) {
        .@"opaque",
        .null,
        .comptime_int,
        .comptime_float,
        .float,
        .int,
        .bool,
        .void,
        .type,
        .enum_literal,
        .@"enum",
        .error_set,
        .error_union,
        .@"fn",
        .noreturn,
        .frame,
        .@"anyframe",
        .undefined,
        => true,
        else => false,
    };
}

fn getBaseName(comptime T: type) []const u8 {
    const full_name = @typeName(T);
    // Find the last index of '.'
    if (std.mem.lastIndexOfScalar(u8, full_name, '.')) |index| {
        return full_name[index + 1 ..];
    }
    return comptime full_name;
}

/// Caller owns memory
fn getAST(io: std.Io, gpa: std.mem.Allocator, path: []const u8) ![]u8 {
    const argv = &.{ "castxml", "--castxml-output=1", path, "-o", "-" };
    var child = try std.process.spawn(io, .{
        .argv = argv,
        .stderr = .inherit,
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

fn printT(gpa: std.mem.Allocator, indentation: u8, prefix: []const u8, val: anytype) std.mem.Allocator.Error!void {
    const fieldInfo = @typeInfo(@TypeOf(val));
    const iprefix: []u8 = try gpa.alloc(u8, indentation);
    for (iprefix) |*element| {
        element.* = ' ';
    }
    defer gpa.free(iprefix);
    if (@TypeOf(val) == []const u8 or @TypeOf(val) == []u8) {
        std.debug.print("{s} {s}: {s}\n", .{ iprefix, prefix, val });
        return;
    }
    switch (fieldInfo) {
        .bool => {
            std.debug.print("{s} {s}: {s}\n", .{ iprefix, prefix, if (val) "true" else "false" });
        },
        .int, .float, .comptime_float, .comptime_int => {
            std.debug.print("{s} {s}: {d}\n", .{ iprefix, prefix, val });
        },
        .array => |ari| {
            const nprefix: []const u8 = try std.fmt.allocPrint(gpa, "\t{s}", .{iprefix});
            defer gpa.free(nprefix);
            std.debug.print("{s} {s}:\n", .{ iprefix, prefix });
            for (ari) |i| {
                try printT(gpa, indentation + 1, "", i);
            }
        },
        .optional => {
            if (val) |v|
                try printT(gpa, indentation, prefix, v)
            else
                std.debug.print("{s} {s}: null\n", .{ iprefix, prefix });
        },
        .@"struct" => |info| {
            inline for (info.fields) |field| {
                try printT(gpa, indentation + 1, field.name, @field(val, field.name));
            }
        },
        .@"union" => {
            try printT(gpa, indentation, "", @field(val, @tagName(val)));
        },
        .@"enum", .enum_literal => {
            std.debug.print("{s} {s}: {s}\n", .{ iprefix, prefix, @tagName(val) });
        },
        .pointer => |i| {
            switch (i.size) {
                .c, .one => {
                    @compileLog("one");
                    std.debug.print("{s} {s}: {x}", .{ iprefix, prefix, val });
                },
                .slice, .many => {
                    std.debug.print("{s} {s}:\n", .{ iprefix, prefix });
                    for (val) |value| {
                        try printT(gpa, indentation + 1, "", value);
                    }
                },
            }
        },
        else => undefined,
    }
}

const item = struct {
    const TokenContainer = struct {
        data: dataType = .{},

        const dataType: type = blk: {
            const count: comptime_int = std.enums.values(item.token.type).len;
            var typeNames: [count][]const u8 = undefined;
            var typeAttrs: [count]std.builtin.Type.StructField.Attributes = undefined;
            var typeTypes: [count]type = undefined;
            for (std.enums.values(item.token.type), 0..) |value, i| {
                typeNames[i] = @tagName(value);
                typeTypes[i] = std.ArrayList(item.token.structType(value));
                typeAttrs[i] = std.builtin.Type.StructField.Attributes{
                    .@"align" = null,
                    .@"comptime" = false,
                    .default_value_ptr = &typeTypes[i].empty,
                };
            }
            break :blk @Struct(
                .auto,
                null,
                &typeNames,
                &typeTypes,
                &typeAttrs,
            );
        };

        pub fn get(self: @This(), comptime t: item.token.type) std.ArrayList(item.token.structType(t)) {
            return @field(self.data, @tagName(t));
        }
        pub fn appendExplicit(self: *@This(), gpa: std.mem.Allocator, comptime t: item.type, value: item.token.structType(t)) !void {
            return @field(self.data, @tagName(t)).append(gpa, value);
        }

        pub fn append(self: *@This(), gpa: std.mem.Allocator, value: anytype) !void {
            const inType = @TypeOf(value);
            ensure(@hasField(@TypeOf(self.data), getBaseName(inType))) catch {
                @panic("Error! " ++ comptime getBaseName(inType) ++ " is not acceptable!");
            };

            try @field(self.data, getBaseName(inType)).append(gpa, value);
        }

        pub fn deinit(self: *@This(), gpa: std.mem.Allocator) void {
            inline for (comptime std.enums.values(token.type)) |t| {
                for (@field(self.data, @tagName(t)).items) |*value|
                    deinitToken(@TypeOf(value.*))(value, gpa);
                @field(self.data, @tagName(t)).deinit(gpa);
            }
        }
    };
    const token = struct {
        const Access = enum {
            public,
            private,
            protected,
        };
        const @"type" = enum {
            Method,
            Class,
            FundamentalType,
            Field,
            Constructor,
            Enumeration,
            PointerType,
            ReferenceType,
            Destructor,
            Namespace,
            Typedef,
        };
        const Method = struct {
            name: []u8,
            mangled: []u8,
            id: []u8,
            returns: []u8,
            context: []u8,
            access: Access,
            virtual: bool,
        };
        const Class = struct {
            name: []u8,
            members: []u8,
            id: []u8,
            context: []u8,
            size: u64,
            @"align": u64,
        };
        const FundamentalType = struct {
            id: []u8,
            name: []u8,
            size: u64,
            @"align": u64,
        };
        const Field = struct {
            id: []u8,
            name: []u8,
            type: []u8,
            context: []u8,
            access: Access,
            offset: u64,
        };
        const Constructor = struct {
            id: []u8,
            context: []u8,
            access: Access,
            @"inline": bool,
        };
        const Enumeration = struct {
            id: []u8,
            name: []u8,
            type: []u8,
            scoped: bool,
            size: u64,
            @"align": u64,
        };
        const PointerType = struct {
            id: []u8,
            type: []u8,
            size: u64,
            @"align": u64,
        };
        const ReferenceType = struct {
            id: []u8,
            type: []u8,
            size: u64,
            @"align": u64,
        };
        const Destructor = struct {
            id: []u8,
            context: []u8,
            access: Access,
            @"inline": bool,
        };
        const Namespace = struct {
            id: []u8,
            name: []u8,
            context: ?[]u8 = null,
        };
        const Typedef = struct {
            id: []u8,
            name: []u8,
            type: []u8,
            context: []u8,
        };

        pub fn setValue(
            T: type,
            m: *T,
            gpa: std.mem.Allocator,
            name: []const u8,
            value: []const u8,
        ) error{ InvalidField, InvalidType, InvalidCharacter, Overflow, OutOfMemory }!void {
            inline for (@typeInfo(T).@"struct".fields) |field| {
                if (std.mem.eql(u8, field.name, name)) {
                    if (field.type == []u8 or field.type == []const u8) {
                        std.debug.print("{s}, setting {s} ([]const u8) to {s} (alloc)\n", .{ @typeName(T), field.name, value });
                        const mem = try gpa.alloc(u8, value.len);
                        @memcpy(mem, value);
                        @field(m, field.name) = mem;
                    } else switch (@typeInfo(field.type)) {
                        .int => {
                            const val = try std.fmt.parseInt(field.type, value, 0);
                            @field(m, field.name) = val;
                        },
                        .float => {
                            const val = try std.fmt.parseFloat(field.type, value, 0);
                            @field(m, field.name) = val;
                        },
                        .@"enum" => |e| {
                            if (!e.is_exhaustive) lbl: {
                                @field(m, field.name) = try std.fmt.parseInt(e.tag_type, value, 0) catch break :lbl;
                            }
                            inline for (comptime std.enums.values(field.type)) |tag| {
                                if (std.mem.eql(u8, @tagName(tag), value)) {
                                    @field(m, field.name) = tag;
                                    break;
                                }
                            }
                        },
                        .bool => {
                            @field(m, field.name) = std.mem.eql(u8, "1", value);
                        },
                        .optional => |opt| {
                            if (value.len != 0) {
                                const Temporary = struct {
                                    x: opt.child,
                                };
                                var t: Temporary = undefined;
                                try setValue(Temporary, &t, gpa, "x", value);
                                @field(m, field.name) = t.x;
                            } else @field(m, field.name) = null;
                        },
                        else => @panic("Reached " ++ @typeName(field.type) ++ " (Not implimented)"),
                    }
                }
            }
        }

        pub fn structType(comptime t: @"type") type {
            return @field(item.token, @tagName(t));
        }

        pub fn getItem(str: []const u8) ?@"type" {
            inline for (std.enums.values(@"type")) |T|
                if (std.mem.eql(u8, str, @tagName(T))) {
                    std.debug.print("Matched with " ++ @tagName(T) ++ "\n", .{});
                    return T;
                };
            std.debug.print("No match\n", .{});
            return null;
        }
    };

    /// Given a type, creates a `deinit` function for the type.
    fn deinitToken(comptime T: type) fn (*T, std.mem.Allocator) void {
        const fun = (struct {
            fn function(self: *T, gpa: std.mem.Allocator) void {
                std.debug.print("Freeing {s}\n", .{@typeName(T)});
                switch (@typeInfo(T)) {
                    .pointer => |ptr| {
                        switch (ptr.size) {
                            .slice => {
                                if (self.len > 0) {
                                    std.debug.print("{s}\n", .{self.*});
                                }
                                if (!isFundamental(ptr.child)) for (self.*) |*i|
                                    deinitToken(ptr.child)(i, gpa);
                                gpa.free(self.*);
                            },
                            else => unreachable,
                        }
                    },
                    .@"struct" => |str| {
                        inline for (str.fields) |value|
                            if (!isFundamental(value.type))
                                deinitToken(value.type)(&@field(self, value.name), gpa);
                    },
                    .optional => |opt| {
                        if (self.*) |*val|
                            if (!isFundamental(opt.child))
                                deinitToken(opt.child)(val, gpa);
                    },
                    else => {},
                }
            }
        }).function;
        return fun;
    }
};

pub fn main(init: std.process.Init) !void {
    const ret = try getAST(init.io, init.arena.allocator(), "test.cpp");
    var xmlReader = std.Io.Reader.fixed(ret);
    var streaming_reader: xml.Reader.Streaming = .init(init.arena.allocator(), &xmlReader, .{});
    const reader = &streaming_reader.interface;

    var x = item.TokenContainer{};
    defer x.deinit(init.gpa);

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
            .xml_declaration => {
                debug.print("xml_declaration: version={s} encoding={?s} standalone={?}\n", .{
                    reader.xmlDeclarationVersion(),
                    reader.xmlDeclarationEncoding(),
                    reader.xmlDeclarationStandalone(),
                });
            },
            .element_start => {
                const element_name = reader.elementNameNs();
                const t = item.token.getItem(element_name.local);
                switch (t orelse continue) {
                    inline else => |v| {
                        const T = item.token.structType(v);
                        var m: T = undefined;
                        inline for (@typeInfo(T).@"struct".fields) |field| {
                            if (field.defaultValue()) |d| @field(m, field.name) = d;
                        }
                        for (0..reader.attributeCount()) |i| {
                            const attribute_name = reader.attributeNameNs(i);
                            const value = try reader.attributeValue(i);
                            try item.token.setValue(item.token.structType(v), &m, init.gpa, attribute_name.local, value);
                        }
                        try x.append(init.gpa, m);
                    },
                }
            },
            .element_end, .comment => {},
            .pi => {
                debug.print("pi: \"{f}\" \"{f}\"\n", .{
                    std.zig.fmtString(reader.piTarget()),
                    std.zig.fmtString(try reader.piData()),
                });
            },
            .cdata => {
                debug.print("cdata: \"{f}\"\n", .{
                    std.zig.fmtString(try reader.cdata()),
                });
            },
            .entity_reference => {
                debug.print("entity_reference: \"{f}\"\n", .{
                    std.zig.fmtString(reader.entityReferenceName()),
                });
            },
            .character_reference => {
                var buf: [4]u8 = undefined;
                const len = std.unicode.utf8Encode(reader.characterReferenceChar(), &buf) catch unreachable;
                debug.print("character_reference: {} (\"{f}\")\n", .{
                    reader.characterReferenceChar(),
                    std.zig.fmtString(buf[0..len]),
                });
            },
            else => continue,
        }
    }
    std.debug.print("Finished\n", .{});
    try printT(init.arena.allocator(), 0, "", x);
}
