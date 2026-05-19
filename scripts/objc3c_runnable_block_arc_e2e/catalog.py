"""Case catalog for runnable block/ARC end-to-end validation."""

from __future__ import annotations

BLOCK_ARC_SMOKE_FIXTURES = (
    "positive/arc_block_autorelease_return_positive.objc3",
    "positive/arc_cleanup_scope_positive.objc3",
    "positive/arc_implicit_cleanup_void_positive.objc3",
)


__all__ = ["BLOCK_ARC_SMOKE_FIXTURES"]
