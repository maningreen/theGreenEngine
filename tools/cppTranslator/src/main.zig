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
    return switch (@typeInfo(T)) {};
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
        .stderr = .ignore, // why is this ignore? well, because we want to ignore global headers.
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
                typeTypes[i] = std.array_hash_map.String(item.token.structType(value));
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

        pub fn init() TokenContainer {
            var self: TokenContainer = undefined;
            inline for (@typeInfo(Data).@"struct".fields) |value| {
                @field(self.data, value.name) = .empty;
            }
            return self;
        }

        pub fn get(self: @This(), comptime t: item.token.type) std.array_hash_map.String(item.token.structType(t)) {
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

            try @field(self.data, getBaseName(inType)).put(gpa, value.id, value);
        }

        pub fn deinit(self: *@This(), gpa: std.mem.Allocator) void {
            inline for (comptime std.enums.values(token.type)) |t| {
                for (@field(self.data, @tagName(t)).values()) |*value|
                    deinitToken(@TypeOf(value.*))(value, gpa);
                @field(self.data, @tagName(t)).deinit(gpa);
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
            @"inline": bool,
            @"const": bool,
        };
        const Class = struct {
            name: []u8,
            members: []u8,
            id: []u8,
            context: []u8,
            size: u64,
            @"align": u64,
            bases: ?[]u8 = null,
            incomplete: bool = false,

            pub fn write(self: Class, gpa: std.mem.Allocator, data: TokenContainer, writer: *std.Io.Writer) !void {
                if (self.incomplete) return;
                var paddingIndex: u64 = 0;
                try writer.print("pub const @\"{s}\" = extern struct {{\n", .{self.name});
                var memberIterator = std.mem.splitScalar(u8, self.members, ' ');
                var initIterator: u64 = 1;

                // second member of tuple is `pass`
                const order = [_]@Tuple(&.{ @"type", u8 }){
                    .{ .Method, 0 },
                    .{ .Field, 0 },
                    .{ .Constructor, 0 },
                    .{ .Destructor, 0 },
                    .{ .Method, 1 },
                };

                inline for (order) |t| {
                    while (memberIterator.next()) |member| {
                        const containerChild = data.find(member) orelse continue;
                        if (containerChild != t[0]) continue;
                        switch (containerChild) {
                            .Method => |method| {
                                // inline methods have no labels, so we can't link
                                if (method.@"inline") continue;
                                const prefix = if (method.@"const") "" else "*";
                                switch (t[1]) {
                                    0 => {
                                        switch (method.access) {
                                            .public => {
                                                if (method.virtual) {
                                                    try writer.print("{s}: *const fn ({s}@This(),", .{ method.name, prefix });
                                                    for (method.arguments orelse &.{}) |arg|
                                                        try writer.print("{s}, ", .{arg.type});
                                                    try writer.print(") callconv(.c) {s} = {s},\n", .{ method.returns, method.mangled });
                                                } else continue;
                                            },
                                            else => {
                                                try writer.print("_{d}: u{d},\n", .{ paddingIndex, @bitSizeOf(usize) });
                                                paddingIndex += 1;
                                            },
                                        }
                                    },
                                    1 => {
                                        switch (method.access) {
                                            .public => {
                                                if (!method.virtual) {
                                                    try writer.print("pub const @\"{s}\" = @\"{s}\";\n", .{ method.name, method.mangled });
                                                }
                                                try writer.print("extern \"c\" fn @\"{s}\"({s}@This(), \n", .{ method.mangled, prefix });
                                                for (method.arguments orelse &.{}) |arg|
                                                    try writer.print("{s},\n", .{arg.type});
                                                try writer.print(") callconv(.c) {s};\n", .{method.returns});
                                            },
                                            else => continue,
                                        }
                                    },
                                    else => undefined,
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
                                            inline .Class, .Struct, .FundamentalType, .Enumeration, .PointerType, .ReferenceType => |j| j.size,
                                            .ArrayType => @bitSizeOf(usize),
                                            else => undefined,
                                        };
                                        try writer.print("_{d}: u{d},\n", .{ paddingIndex, size });
                                    },
                                }
                            },
                            .Constructor => |constructor| {
                                switch (constructor.access) {
                                    .public => {
                                        if (!constructor.@"inline") {
                                            try constructor.writeMangled(initIterator, gpa, data, writer);
                                            initIterator += 1;
                                        }
                                    },
                                    else => continue,
                                }
                            },
                            .Destructor => |destructor| {
                                if (!destructor.@"inline") {
                                    try destructor.writeMangled(gpa, data, writer);
                                } else {
                                    // write a dummy deinit
                                    try writer.print("pub const deinit = (struct {{ pub fn f(_: anytype) void {{}}}}).f;", .{});
                                }
                            },
                            .Typedef => |td| {
                                try writer.print("pub const @\"{s}\" = @\"{s}\"\n", .{ td.name, td.type });
                            },
                            else => continue,
                        }
                    }
                    memberIterator = std.mem.splitScalar(u8, self.members, ' ');
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
            context: []u8,
            access: Access,
            arguments: ?[]Argument = null,
            @"inline": bool = false,

            pub fn writeMangled(self: Constructor, constructorIndex: u64, gpa: std.mem.Allocator, data: item.TokenContainer, writer: *std.Io.Writer) (error{ Inline, OutOfMemory } || std.Io.Writer.Error)!void {
                if (self.@"inline") return error.Inline;

                const manglePrefix = "_Z";
                const rootNamespace = "::";

                var mangledName = std.Io.Writer.Allocating.init(gpa);
                defer mangledName.deinit();

                var parents: std.ArrayList([]const u8) = try .initCapacity(gpa, 1);
                defer parents.deinit(gpa);

                {
                    var parent = self.context;
                    while (data.find(parent)) |grandparent| {
                        switch (grandparent) {
                            inline .Class, .Struct => |parentVal| {
                                parent = parentVal.context;
                                try parents.append(gpa, parentVal.name);
                            },
                            .Namespace => |namespace| {
                                parent = namespace.context orelse break;
                                try parents.append(gpa, namespace.name);
                            },
                            else => unreachable,
                        }
                    }
                }

                try mangledName.writer.print(manglePrefix, .{});
                if (parents.items.len >= 1) {
                    try mangledName.writer.print("N", .{});
                }

                const ltEscape = "<";
                const gtEscape = ">";

                const typeMap = std.static_string_map.StaticStringMap(struct { []const u8, enum {
                    custom,
                    primitive,
                } }).initComptime(&.{
                    .{ "void", .{ "v", .primitive } },
                    .{ "wchar_t", .{ "w", .primitive } },
                    .{ "bool", .{ "b", .primitive } },
                    .{ "char", .{ "c", .primitive } },
                    .{ "signed char", .{ "a", .primitive } },
                    .{ "unsigned char", .{ "h", .primitive } },
                    .{ "short", .{ "s", .primitive } },
                    .{ "unsigned short", .{ "t", .primitive } },
                    .{ "int", .{ "i", .primitive } },
                    .{ "unsigned int", .{ "j", .primitive } },
                    .{ "long", .{ "l", .primitive } },
                    .{ "unsigned long", .{ "m", .primitive } },
                    .{ "long long", .{ "x", .primitive } },
                    .{ "unsigned long long", .{ "y", .primitive } },
                    .{ "__int128", .{ "n", .primitive } },
                    .{ "unsigned __int128", .{ "o", .primitive } },
                    .{ "float", .{ "f", .primitive } },
                    .{ "double", .{ "d", .primitive } },
                    .{ "long double", .{ "e", .primitive } },
                    .{ "__float128", .{ "g", .primitive } },
                    .{ "char32_t", .{ "Di", .primitive } },
                    .{ "char16_t", .{ "Ds", .primitive } },
                    .{ "auto", .{ "Da", .primitive } },
                    .{ "std::nullptr_t", .{ "Dn", .primitive } },
                });

                for (0..parents.items.len) |i| {
                    const parent = parents.items[parents.items.len - 1 - i];
                    if (std.mem.find(u8, parent, ltEscape)) |lt| lbl: {
                        const gt = std.mem.find(u8, parent, gtEscape) orelse break :lbl;
                        const templateType = parent[lt + ltEscape.len .. gt];
                        const t = typeMap.get(templateType) orelse .{ templateType, .custom };
                        switch (t.@"1") {
                            .custom => try mangledName.writer.print("I{d}{s}E", .{ t.@"0".len, t.@"0" }),
                            .primitive => try mangledName.writer.print("I{s}E", .{t.@"0"}),
                        }
                        continue;
                    }
                    if (std.mem.eql(u8, parent, rootNamespace)) continue;
                    try mangledName.writer.print("{d}{s}", .{ parent.len, parent });
                }
                try mangledName.writer.print("C{d}E", .{constructorIndex});
                for (self.arguments orelse &.{}) |arg| {
                    var t: []const u8 = arg.type;
                    while (true) {
                        switch ((data.find(t) orelse unreachable)) {
                            .FundamentalType => |fund| {
                                const value = typeMap.get(fund.name) orelse std.debug.panic("reached invalid type: {s}", .{fund.name});
                                try mangledName.writer.print("{s}", .{value.@"0"});
                                break;
                            },
                            inline .ArrayType, .PointerType => |ptr| {
                                try mangledName.writer.print("P", .{});
                                t = ptr.type;
                            },
                            .ReferenceType => |ref| {
                                try mangledName.writer.print("R", .{});
                                t = ref.type;
                            },
                            inline .Class, .Struct => |cplx| {
                                try mangledName.writer.print("{d}{s}", .{ cplx.name.len, cplx.name });
                                break;
                            },
                            .CvQualifiedType => |cv| {
                                try mangledName.writer.print("{s}", .{if (cv.@"const") "K" else ""});
                                t = cv.type;
                            },
                            else => unreachable,
                        }
                    }
                } else {
                    // the only way to get here is if there're no arguments
                    // we break from every other case
                    try mangledName.writer.print(comptime typeMap.get("void").?.@"0", .{});
                }

                try writer.print("pub const init{d} = {s};\n", .{ constructorIndex, mangledName.writer.buffered() });
                try writer.print("extern \"c\" fn {s}(*@This()) void;\n", .{mangledName.writer.buffered()});
            }
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
            @"inline": bool = false,

            pub fn writeMangled(self: Destructor, gpa: std.mem.Allocator, data: item.TokenContainer, writer: *std.Io.Writer) (error{ Inline, OutOfMemory } || std.Io.Writer.Error)!void {
                if (self.@"inline") return error.Inline;

                const manglePrefix = "_Z";
                const rootNamespace = "::";
                const totalSuffix = "D1Ev";

                var mangledName = std.Io.Writer.Allocating.init(gpa);
                defer mangledName.deinit();

                var parents: std.ArrayList([]const u8) = try .initCapacity(gpa, 1);
                defer parents.deinit(gpa);

                {
                    var parent = self.context;
                    while (data.find(parent)) |grandparent| {
                        switch (grandparent) {
                            inline .Class, .Struct => |parentVal| {
                                parent = parentVal.context;
                                try parents.append(gpa, parentVal.name);
                            },
                            .Namespace => |namespace| {
                                parent = namespace.context orelse break;
                                try parents.append(gpa, namespace.name);
                            },
                            else => unreachable,
                        }
                    }
                }

                try mangledName.writer.print(manglePrefix, .{});
                if (parents.items.len >= 1) {
                    try mangledName.writer.print("N", .{});
                }

                const ltEscape = "<";
                const gtEscape = ">";

                const typeMap = std.static_string_map.StaticStringMap(struct { []const u8, enum {
                    custom,
                    primitive,
                } }).initComptime(&.{
                    .{ "void", .{ "v", .primitive } },
                    .{ "wchar_t", .{ "w", .primitive } },
                    .{ "bool", .{ "b", .primitive } },
                    .{ "char", .{ "c", .primitive } },
                    .{ "signed char", .{ "a", .primitive } },
                    .{ "unsigned char", .{ "h", .primitive } },
                    .{ "short", .{ "s", .primitive } },
                    .{ "unsigned short", .{ "t", .primitive } },
                    .{ "int", .{ "i", .primitive } },
                    .{ "unsigned int", .{ "j", .primitive } },
                    .{ "long", .{ "l", .primitive } },
                    .{ "unsigned long", .{ "m", .primitive } },
                    .{ "long long", .{ "x", .primitive } },
                    .{ "unsigned long long", .{ "y", .primitive } },
                    .{ "__int128", .{ "n", .primitive } },
                    .{ "unsigned __int128", .{ "o", .primitive } },
                    .{ "float", .{ "f", .primitive } },
                    .{ "double", .{ "d", .primitive } },
                    .{ "long double", .{ "e", .primitive } },
                    .{ "__float128", .{ "g", .primitive } },
                    .{ "char32_t", .{ "Di", .primitive } },
                    .{ "char16_t", .{ "Ds", .primitive } },
                    .{ "auto", .{ "Da", .primitive } },
                    .{ "std::nullptr_t", .{ "Dn", .primitive } },
                });

                for (0..parents.items.len) |i| {
                    const parent = parents.items[parents.items.len - 1 - i];
                    if (std.mem.find(u8, parent, ltEscape)) |lt| lbl: {
                        const gt = std.mem.find(u8, parent, gtEscape) orelse break :lbl;
                        const templateType = parent[lt + ltEscape.len .. gt];
                        const t = typeMap.get(templateType) orelse .{ templateType, .custom };
                        switch (t.@"1") {
                            .custom => try mangledName.writer.print("I{d}{s}E", .{ t.@"0".len, t.@"0" }),
                            .primitive => try mangledName.writer.print("I{s}E", .{t.@"0"}),
                        }
                        continue;
                    }
                    if (std.mem.eql(u8, parent, rootNamespace)) continue;
                    try mangledName.writer.print("{d}{s}", .{ parent.len, parent });
                }
                try mangledName.writer.print(totalSuffix, .{});

                try writer.print("pub const deinit = {s};\n", .{mangledName.writer.buffered()});
                try writer.print("extern \"c\" fn {s}(*@This()) void;\n", .{mangledName.writer.buffered()});
            }
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
                            inline for (comptime std.enums.values(field.type)) |tag| {
                                if (std.mem.eql(u8, @tagName(tag), value)) {
                                    @field(m, field.name) = tag;
                                    break;
                                }
                            } else if (!e.is_exhaustive) @field(m, field.name) = try std.fmt.parseInt(e.tag_type, value, 0);
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
                if (std.mem.eql(u8, str, @tagName(T)))
                    return T;
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
                                for (self.*) |*i|
                                    deinitToken(ptr.child)(i, gpa);
                                gpa.free(self.*);
                            },
                            .one => {
                                deinitToken(ptr.child)(&self.*);
                                gpa.destroy(self);
                            },
                            .many => {
                                for (self.*) |*v|
                                    deinitToken(ptr.child)(v, gpa);
                                gpa.free(self);
                            },
                            .c => {
                                deinitToken(ptr.child)(&self.*);
                                gpa.destroy(self);
                            },
                        }
                    },
                    .@"struct" => |str| {
                        if (@hasDecl(T, "deinit"))
                            self.deinit(gpa)
                        else inline for (str.fields) |field|
                            if (!isFundamental(field.type))
                                deinitToken(field.type)(&@field(self, field.name), gpa);
                    },
                    .optional => |opt| {
                        if (self.*) |*val|
                            deinitToken(opt.child)(val, gpa);
                    },
                    .@"union" => {
                        switch (self.*) {
                            inline else => |*v| {
                                deinitToken(@TypeOf(v.*))(v, gpa);
                            },
                        }
                    },
                    .vector => |v| {
                        for (v) |*value|
                            deinitToken(v.child)(value, gpa);
                    },
                    else => {},
                }
            }
        }).function;
        return fun;
    }
};

/// returned memory is owned by caller.
fn parseTokens(gpa: std.mem.Allocator, input: []const u8) !item.TokenContainer {
    var xmlReader = std.Io.Reader.fixed(input);

    var streaming_reader: xml.Reader.Streaming = .init(gpa, &xmlReader, .{});
    defer streaming_reader.deinit();

    const reader = &streaming_reader.interface;

    var container = item.TokenContainer.init();
    errdefer container.deinit(gpa);

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
                                s.arguments = try gpa.realloc(s.arguments.?, s.arguments.?.len + 1);
                                var arg: item.token.Argument = undefined;
                                for (0..reader.attributeCount()) |i| {
                                    const attribute_name = reader.attributeNameNs(i);
                                    const value = try reader.attributeValue(i);
                                    try item.token.setValue(item.token.Argument, &arg, gpa, attribute_name.local, value);
                                }
                                s.arguments.?[s.arguments.?.len - 1] = arg;
                            } else {
                                s.arguments = try gpa.alloc(item.token.Argument, 1);
                                var arg: item.token.Argument = undefined;
                                for (0..reader.attributeCount()) |i| {
                                    const attribute_name = reader.attributeNameNs(i);
                                    const value = try reader.attributeValue(i);
                                    try item.token.setValue(item.token.Argument, &arg, gpa, attribute_name.local, value);
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
                            try item.token.setValue(item.token.structType(v), &m, gpa, attribute_name.local, value);
                        }
                        state = @unionInit(item.TokenContainer.TokenUnion, getBaseName(T), m);
                    },
                }
            },
            .element_end => {
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
    const fundamentalTypes = container.get(.FundamentalType);
    for (fundamentalTypes.values()) |t| {
        const FundTypes = enum {
            float,
            int,
            bool,
            void,
        };

        const @"type": FundTypes =
            if (std.mem.find(u8, t.name, "float") != null or std.mem.find(u8, t.name, "double") != null)
                .float
            else if (std.mem.eql(u8, t.name, "void"))
                .void
            else if (std.mem.eql(u8, t.name, "bool"))
                .bool
            else
                .int;

        switch (@"type") {
            .float => {
                try out.print("const {s} = f{d};\n", .{ t.id, t.size });
            },
            .int => {
                const signed: std.builtin.Signedness = if (std.mem.find(u8, t.name, "unsigned") == null) .signed else .unsigned;

                const prefix: u8 = switch (signed) {
                    .signed => 'i',
                    .unsigned => 'u',
                };
                try out.print("const {s} = {c}{d};\n", .{ t.id, prefix, t.size });
            },
            inline .bool, .void => |v| {
                try out.print("const {s} = " ++ @tagName(v) ++ ";\n", .{t.id});
            },
        }
    }
    const typedefs = container.get(.Typedef);
    for (typedefs.values()) |t| {
        try out.print("const {s} = {s};\n", .{ t.id, t.type });
        try out.print("const {s} = {s};\n", .{ t.name, t.id });
    }
    const classes = container.get(.Class);
    for (classes.values()) |class|
        try class.write(gpa, container, out);
    const structs = container.get(.Struct);
    for (structs.values()) |class|
        try class.write(gpa, container, out);
    const ptrTypes = container.get(.PointerType);
    for (ptrTypes.values()) |ptrT|
        try out.print("const {s} = ?*{s};\n", .{ ptrT.id, ptrT.type });
    const arrayTypes = container.get(.ArrayType);
    for (arrayTypes.values()) |arrT|
        try out.print("const {s} = ?[*]{s};\n", .{ arrT.id, arrT.type });
    const cvTypes = container.get(.CvQualifiedType);
    for (cvTypes.values()) |cvT|
        try out.print("const {s} = {s};\n", .{ cvT.id, cvT.type });
    const refTypes = container.get(.ReferenceType);
    for (refTypes.values()) |rt|
        try out.print("const {s} = *{s};\n", .{ rt.id, rt.type });
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
