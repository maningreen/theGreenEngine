const std = @import("std");

pub fn isFundamental(comptime T: type) bool {
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

pub fn ensure(x: bool) error{False}!void {
    if (!x) return error.False;
}

pub fn getBaseName(comptime T: type) []const u8 {
    const full_name = @typeName(T);
    // Find the last index of '.'
    if (std.mem.lastIndexOfScalar(u8, full_name, '.')) |index| {
        return full_name[index + 1 ..];
    }
    return comptime full_name;
}
