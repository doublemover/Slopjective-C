from __future__ import annotations

from .pattern_model import ForbiddenPattern


STRUCTURE_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "drained-include-shard-reference",
        "Drained include-shard references must not reappear on active hard-cutover surfaces.",
        r"\bobjc3_[A-Za-z0-9_]+_parts[\\/]+objc3_[A-Za-z0-9_]+_part_[0-9]{3}\.inc\b"
        r"|#\s*include\s+[<\"][^<>\"]*objc3_[A-Za-z0-9_]+_part_[0-9]{3}\.inc[>\"]",
    ),
    ForbiddenPattern(
        "include-sharded-surface-claim",
        "Include-sharded compiler/runtime structure is retired from the hard cutover.",
        r"\binclude[-_\s]+shard(?:ed|s|ing)?\b",
    ),
    ForbiddenPattern(
        "stale-monolithic-cmake",
        "The native CMake graph must not carry monolithic future-state commentary.",
        r"monolithic\s+`?src/main\.cpp`?|planned\s+steady-state\s+split",
    ),
)
