from __future__ import annotations

import re
from dataclasses import dataclass
from typing import Iterable, Pattern

from .files import is_excluded
from .pattern_model import ForbiddenPattern
from .patterns import FORBIDDEN_PATTERNS
from .roots import DEFAULT_EXCLUDES, DEFAULT_SCAN_ROOTS


@dataclass(frozen=True)
class SourceHygieneScanConfig:
    scan_roots: tuple[str, ...] = DEFAULT_SCAN_ROOTS
    excludes: tuple[str, ...] = DEFAULT_EXCLUDES
    patterns: tuple[ForbiddenPattern, ...] = FORBIDDEN_PATTERNS


@dataclass(frozen=True)
class CompiledPolicyPattern:
    pattern: ForbiddenPattern
    regex: Pattern[str]


DEFAULT_SCAN_CONFIG = SourceHygieneScanConfig()


def pattern_in_scope(pattern: ForbiddenPattern, repo_path: str) -> bool:
    if pattern.include_paths and not is_excluded(repo_path, pattern.include_paths):
        return False
    if pattern.exclude_paths and is_excluded(repo_path, pattern.exclude_paths):
        return False
    return True


def compile_policy_patterns(
    patterns: Iterable[ForbiddenPattern],
) -> tuple[CompiledPolicyPattern, ...]:
    return tuple(
        CompiledPolicyPattern(pattern, re.compile(pattern.regex, re.IGNORECASE))
        for pattern in patterns
    )
