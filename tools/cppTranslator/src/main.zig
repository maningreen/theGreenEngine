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
fn isNamespaceType(comptime T: type) bool {
    return switch (@typeInfo(T)) {
        .@"enum", .@"union", .@"struct", .@"opaque" => true,
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
        data: Data,

        const Data: type = blk: {
            const count: comptime_int = std.enums.values(item.token.type).len;
            var typeNames: [count][]const u8 = undefined;
            var typeAttrs: [count]std.builtin.Type.StructField.Attributes = undefined;
            var typeTypes: [count]type = undefined;
            for (std.enums.values(item.token.type), 0..) |value, i| {
                typeNames[i] = @tagName(value);
                typeTypes[i] = std.StringArrayHashMap(item.token.structType(value));
                typeAttrs[i] = std.builtin.Type.StructField.Attributes{
                    .@"align" = null,
                    .@"comptime" = false,
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

        const TokenUnion: type = blk: {
            const types = std.enums.values(item.token.type);
            var typeNames: [types.len][]const u8 = undefined;
            var fieldTypes: [types.len]type = undefined;
            var fieldAttrs = [1]std.builtin.Type.UnionField.Attributes{.{ .@"align" = null }} ** types.len;
            for (types, 0..) |t, i| {
                typeNames[i] = @tagName(t);
                fieldTypes[i] = item.token.structType(t);
            }
            break :blk @Union(.auto, item.token.type, &typeNames, &fieldTypes, &fieldAttrs);
        };

        pub fn init(gpa: std.mem.Allocator) TokenContainer {
            var self: TokenContainer = undefined;
            inline for (@typeInfo(Data).@"struct".fields) |value| {
                @field(self.data, value.name) = .init(gpa);
            }
            return self;
        }

        pub fn get(self: @This(), comptime t: item.token.type) std.StringArrayHashMap(item.token.structType(t)) {
            return @field(self.data, @tagName(t));
        }
        pub fn appendExplicit(self: *@This(), gpa: std.mem.Allocator, comptime t: item.type, value: item.token.structType(t)) !void {
            return @field(self.data, @tagName(t)).append(gpa, value);
        }

        pub fn append(self: *@This(), value: anytype) !void {
            const inType = @TypeOf(value);
            ensure(@hasField(@TypeOf(self.data), getBaseName(inType))) catch {
                @panic("Error! " ++ comptime getBaseName(inType) ++ " is not acceptable!");
            };

            try @field(self.data, getBaseName(inType)).put(value.id, value);
        }

        pub fn deinit(self: *@This(), gpa: std.mem.Allocator) void {
            inline for (comptime std.enums.values(token.type)) |t| {
                for (@field(self.data, @tagName(t)).values()) |*value|
                    deinitToken(@TypeOf(value.*))(value, gpa);
                @field(self.data, @tagName(t)).deinit();
            }
        }

        pub fn getType(self: @This(), id: []const u8) ?token.type {
            inline for (@typeInfo(Data).@"struct".fields, std.enums.values(token.type)) |field, tokenType| {
                const set = @field(self.data, field.name);
                if (set.contains(id))
                    return tokenType;
            }
            return null;
        }
        pub fn find(self: @This(), id: []const u8) ?TokenUnion {
            inline for (std.enums.values(token.type)) |tokenType|
                if (@field(self.data, @tagName(tokenType)).get(id)) |val|
                    return @unionInit(TokenUnion, @tagName(tokenType), val);
            return null;
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
            Struct,
            FundamentalType,
            Field,
            Constructor,
            Enumeration,
            PointerType,
            ReferenceType,
            Destructor,
            Namespace,
            Typedef,
            ArrayType,
            CvQualifiedType,
            Function,
        };
        const Method = struct {
            name: []u8,
            mangled: []u8,
            id: []u8,
            returns: []u8,
            // context: []u8,
            arguments: ?[]Argument = null,
            access: Access,
            virtual: bool,
        };
        const Class = struct {
            name: []u8,
            members: []u8,
            id: []u8,
            // context: []u8,
            size: u64,
            @"align": u64,
            bases: ?[]u8 = null,

            pub fn write(self: Class, data: TokenContainer, writer: *std.Io.Writer) !void {
                var paddingIndex: u64 = 0;
                try writer.print("pub const @\"{s}\" = struct {{\n", .{self.name});
                var memberIterator = std.mem.splitScalar(u8, self.members, ' ');
                while (memberIterator.next()) |member| {
                    const containerChild = data.find(member) orelse continue;
                    switch (containerChild) {
                        .Method => |method| {
                            if (!method.virtual) {
                                switch (method.access) {
                                    .public => {
                                        try writer.print("pub const @\"{s}\" = @\"{s}\";\n", .{ method.name, method.mangled });
                                        try writer.print("extern \"c\" fn @\"{s}\"(*{s}, ", .{ method.mangled, self.name });
                                        for (method.arguments orelse &.{}) |arg|
                                            try writer.print("{s}, ", .{arg.type});
                                        try writer.print(") {s};\n", .{method.returns});
                                    },
                                    else => continue,
                                }
                            } else {
                                // GOD DAMN IT.; we just return
                                try writer.print("{s}: *const fn (*{s}, ", .{ method.name, self.name });
                                for (method.arguments orelse &.{}) |arg| {
                                    try writer.print("{s}, ", .{arg.type});
                                }
                                try writer.print(") callconv(.c) {s},\n", .{method.returns});
                                paddingIndex += 1;
                            }
                        },
                        .Field => |field| {
                            switch (field.access) {
                                .public => {
                                    try writer.print("@\"{s}\": {s},\n", .{ field.name, field.type });
                                },
                                else => {
                                    // Get size of type
                                    const tType = data.find(field.type) orelse @panic("Errror! type not found!");
                                    const size = switch (tType) {
                                        inline .Class,
                                        .Struct,
                                        .FundamentalType,
                                        .Enumeration,
                                        .PointerType,
                                        .ReferenceType,
                                        => |t| t.size,
                                        .ArrayType => @bitSizeOf(usize),
                                        else => undefined,
                                    };
                                    try writer.print("_{d}: u{d},\n", .{ paddingIndex, size });
                                },
                            }
                        },
                        .Constructor => {},
                        .Enumeration => {},
                        .Destructor => {},
                        .Typedef => {},
                        else => continue,
                    }
                }
                try writer.print("}};\nconst {s} = @\"{s}\";\n", .{ self.id, self.name });
            }
        };
        const Struct = Class;
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
            // context: []u8,
            access: Access,
            offset: u64,
        };
        const Constructor = struct {
            id: []u8,
            // context: []u8,
            access: Access,
            arguments: ?[]Argument = null,
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
            // context: []u8,
            access: Access,
            @"inline": bool,
        };
        const Namespace = struct {
            id: []u8,
            name: []u8,
            // context: ?[]u8 = null,
        };
        const Typedef = struct {
            id: []u8,
            name: []u8,
            type: []u8,
            // context: []u8,
        };
        const ArrayType = struct {
            id: []u8,
            type: []u8,
            min: u64,
            max: u64,
        };
        const CvQualifiedType = struct {
            id: []u8,
            type: []u8,
            @"const": bool,
        };
        const Function = struct {
            id: []u8,
            name: []u8,
            returns: []u8,
            // context: []u8,
            mangled: []u8,
            arguments: ?[]Argument = null,
        };
        const Argument = struct {
            type: []u8,
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
                    return T;
                };
            return null;
        }
    };

    /// Given a type, creates a `deinit` function for the type.
    fn deinitToken(comptime T: type) fn (*T, std.mem.Allocator) void {
        const fun = (struct {
            fn function(self: *T, gpa: std.mem.Allocator) void {
                switch (@typeInfo(T)) {
                    .pointer => |ptr| {
                        switch (ptr.size) {
                            .slice => {
                                if (!isFundamental(ptr.child)) for (self.*) |*i|
                                    deinitToken(ptr.child)(i, gpa);
                                gpa.free(self.*);
                            },
                            else => unreachable,
                        }
                    },
                    .@"struct" => |str| {
                        if (@hasDecl(T, "deinit"))
                            self.deinit(gpa)
                        else inline for (str.fields) |field| {
                            if (!isFundamental(field.type))
                                deinitToken(field.type)(&@field(self, field.name), gpa);
                        }
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

    var container = item.TokenContainer.init(init.gpa);
    defer container.deinit(init.gpa);

    var state: ?item.TokenContainer.TokenUnion = null;
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
                const t = item.token.getItem(element_name.local);
                if (std.mem.eql(u8, element_name.local, getBaseName(item.token.Argument))) if (state != null) switch (state.?) {
                    inline else => |*s| {
                        if (@hasField(@TypeOf(s.*), "arguments")) {
                            if (s.arguments != null) {
                                s.arguments = try init.gpa.realloc(s.arguments.?, s.arguments.?.len + 1);
                                var arg: item.token.Argument = undefined;
                                for (0..reader.attributeCount()) |i| {
                                    const attribute_name = reader.attributeNameNs(i);
                                    const value = try reader.attributeValue(i);
                                    try item.token.setValue(item.token.Argument, &arg, init.gpa, attribute_name.local, value);
                                }
                                s.arguments.?[s.arguments.?.len - 1] = arg;
                            } else {
                                s.arguments = try init.gpa.alloc(item.token.Argument, 1);
                                var arg: item.token.Argument = undefined;
                                for (0..reader.attributeCount()) |i| {
                                    const attribute_name = reader.attributeNameNs(i);
                                    const value = try reader.attributeValue(i);
                                    try item.token.setValue(item.token.Argument, &arg, init.gpa, attribute_name.local, value);
                                }
                                s.arguments.?[s.arguments.?.len - 1] = arg;
                            }
                        }
                        continue;
                    },
                };
                if (state != null) continue;
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
                        state = @unionInit(item.TokenContainer.TokenUnion, getBaseName(T), m);
                    },
                }
            },
            .element_end => {
                switch (state orelse continue) {
                    inline else => |v| try container.append(v),
                }
                state = null;
            },
            else => continue,
        }
    }
    var out = std.Io.Writer.Allocating.init(init.gpa);
    defer out.deinit();
    const fundamentalTypes = container.get(.FundamentalType);
    for (fundamentalTypes.values()) |t| {
        const signed: std.builtin.Signedness = if (std.mem.indexOf(u8, t.name, "unsigned") == null) .signed else .unsigned;
        const prefix: u8 = switch (signed) {
            .signed => 'i',
            .unsigned => 'u',
        };
        try out.writer.print("const {s} = {c}{d};\n", .{ t.id, prefix, t.size });
    }
    const typedefs = container.get(.Typedef);
    for (typedefs.values()) |t| {
        try out.writer.print("const {s} = {s};\n", .{ t.id, t.type });
        try out.writer.print("const {s} = {s};\n", .{ t.name, t.id });
    }
    const fields = container.get(.Field);
    const classes = container.get(.Class);
    for (classes.values()) |class| {
        // try out.writer.print("pub const {s} = struct {{\n", .{class.name});
        // {
        // var it = std.mem.splitScalar(u8, class.members, ' ');
        // var i: u64 = 0;
        // while (it.next()) |str| : (i += 1) {
        // if (fields.get(str)) |t|
        // try out.writer.print("@\"{s}\": {s},\n", .{ t.name, t.type });
        // }
        // }
        // blk: {
        // var it = std.mem.splitScalar(u8, class.bases orelse break :blk, ' ');
        // var i: u64 = 0;
        // while (it.next()) |str| : (i += 1) {
        // if (classes.get(str)) |t|
        // try out.writer.print("@\"{s}\": {s},\n", .{ t.name, t.id });
        // }
        // }
        // try out.writer.print("}};\nconst {s} = {s};\n", .{ class.id, class.name });
        try class.write(container, &out.writer);
    }
    const structs = container.get(.Struct);
    for (structs.values()) |class| {
        try out.writer.print("pub const {s} = struct {{\n", .{class.name});
        {
            var it = std.mem.splitScalar(u8, class.members, ' ');
            var i: u64 = 0;
            while (it.next()) |str| : (i += 1) {
                if (fields.get(str)) |t|
                    try out.writer.print("@\"{s}\": {s},\n", .{ t.name, t.type });
            }
        }
        blk: {
            var it = std.mem.splitScalar(u8, class.bases orelse break :blk, ' ');
            var i: u64 = 0;
            while (it.next()) |str| : (i += 1) {
                if (fields.get(str)) |t|
                    try out.writer.print("@\"{s}\": {s},\n", .{ t.name, t.type });
            }
        }
        try out.writer.print("}};\nconst {s} = {s};\n", .{ class.id, class.name });
    }
    const ptrTypes = container.get(.PointerType);
    for (ptrTypes.values()) |ptrT| {
        try out.writer.print("const {s} = *{s};\n", .{ ptrT.id, ptrT.type });
    }
    const arrayTypes = container.get(.ArrayType);
    for (arrayTypes.values()) |arrT| {
        try out.writer.print("const {s} = [*c]{s};\n", .{ arrT.id, arrT.type });
    }
    const cvTypes = container.get(.CvQualifiedType);
    for (cvTypes.values()) |cvT| {
        try out.writer.print("const {s} = {s};\n", .{ cvT.id, cvT.type });
    }
    std.debug.print("{s}\n", .{out.written()});
    // const fmt = std.json.fmt(container.get(.FundamentalType).items, .{ .whitespace = .indent_4 });
    // std.debug.print("{f}\n", .{fmt});
}
