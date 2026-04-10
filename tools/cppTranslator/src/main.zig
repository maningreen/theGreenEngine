const std = @import("std");

/// Caller owns memory
fn getAST(io: std.Io, gpa: std.mem.Allocator, path: []const u8) ![]u8 {
    const argv = &.{
        "zig",
        "c++",
        "-x",
        "c++",
        "-std=c++11",
        "-Xclang",
        "-ast-dump=json",
        path,
    };
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
const Item = struct {
    kind: enum {
        CXXConstructExpr,
        ReturnStmt,
        PointerType,
        BuiltinType,
        TypedefDecl,
        ConstantArrayType,
        RecordType,
        CXXRecordDecl,
        AccessSpecDecl,
        CXXCtorInitializer,
        FieldDecl,
        IntegerLiteral,
        CXXDestructorDecl,
        CompoundStmt,
        CXXMethodDecl,
        CXXConstructorDecl,
        VisibilityAttr,
        ReturnsNonNullAttr,
        AllocSizeAttr,
        FunctionDecl,
        ParmVarDecl,
        TranslationUnitDecl,
    },
    name: ?[]const u8 = null,
    mangledName: ?[]const u8 = null,
    inner: ?[]Item = null,
    tagUsed: ?[]const u8 = null,

    type: ?struct {
        qualType: []const u8,
    } = null,

    isReference: ?bool = null,
    completeDefinition: ?bool = null,
    isImplicit: ?bool = null,
    virtual: ?bool = null,

    pub fn print(i: Item, gpa: std.mem.Allocator) !void {
        try printT(gpa, 0, "", i);
    }

    fn printT(gpa: std.mem.Allocator, indentation: u8, prefix: []const u8, val: anytype) std.mem.Allocator.Error!void {
        const fieldInfo = @typeInfo(@TypeOf(val));
        const iprefix: []u8 = try gpa.alloc(u8, indentation);
        for (iprefix) |*item| {
            item.* = ' ';
        }
        defer gpa.free(iprefix);
        if (@TypeOf(val) == []const u8) {
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
            else => unreachable,
        }
    }
};

pub fn main(init: std.process.Init) !void {
    const conts = try getAST(init.io, init.arena.allocator(), "test.cpp");
    const ret = try std.json.parseFromSlice(Item, init.arena.allocator(), conts, .{
        .allocate = .alloc_always,
        .duplicate_field_behavior = .use_first,
        .ignore_unknown_fields = true,
    });
    try ret.value.print(init.arena.allocator());
}
