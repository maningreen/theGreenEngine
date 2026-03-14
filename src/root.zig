const std = @import("std");

const main = @extern(*const fn () void, .{ .name = "main" });
