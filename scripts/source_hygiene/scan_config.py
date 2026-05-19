from __future__ import annotations

import re
from dataclasses import dataclass
from typing import Iterable, Pattern

from .files import is_excluded
from .pattern_model import ForbiddenPattern, pattern_group_report_records
from .patterns import FORBIDDEN_PATTERN_GROUPS, FORBIDDEN_PATTERNS
from .roots import DEFAULT_EXCLUDES, DEFAULT_SCAN_ROOTS

SOURCE_HYGIENE_SCAN_CONFIG_CONTRACT_ID = "source-hygiene-scan-config-v1"
SOURCE_HYGIENE_SCAN_CONFIG_OWNER_SURFACE = "scripts/source_hygiene/scan_config.py"


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


def scan_config_contract_summary(
    config: SourceHygieneScanConfig = DEFAULT_SCAN_CONFIG,
) -> dict[str, object]:
    return {
        "contract_id": SOURCE_HYGIENE_SCAN_CONFIG_CONTRACT_ID,
        "owner_surface": SOURCE_HYGIENE_SCAN_CONFIG_OWNER_SURFACE,
        "scan_root_count": len(config.scan_roots),
        "exclude_count": len(config.excludes),
        "pattern_count": len(config.patterns),
        "scan_roots": list(config.scan_roots),
        "excludes": list(config.excludes),
        "pattern_owner_surfaces": sorted(
            {pattern.owner_surface for pattern in config.patterns}
        ),
        "pattern_groups": pattern_group_report_records(FORBIDDEN_PATTERN_GROUPS),
        "path_scope_is_fail_closed": True,
        "compiled_patterns_are_case_insensitive": True,
    }


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


__all__ = [
    "CompiledPolicyPattern",
    "DEFAULT_SCAN_CONFIG",
    "SOURCE_HYGIENE_SCAN_CONFIG_CONTRACT_ID",
    "SOURCE_HYGIENE_SCAN_CONFIG_OWNER_SURFACE",
    "SourceHygieneScanConfig",
    "compile_policy_patterns",
    "pattern_in_scope",
    "scan_config_contract_summary",
]
