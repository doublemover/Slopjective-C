"""Open-blocker audit input validation and markdown scope resolution."""

from __future__ import annotations

from .audit_scope_constants import DEFAULT_EXCLUDE_PATHS
from .audit_scope_extractor_excludes import build_extractor_exclude_paths
from .audit_scope_globs import (
    normalize_exclude_paths,
    normalize_include_globs,
    resolve_effective_audit_root,
)
from .audit_scope_markdown import resolve_markdown_scope
from .audit_scope_validation import (
    validate_generated_at_utc,
    validate_snapshot_source,
)


__all__ = [
    "DEFAULT_EXCLUDE_PATHS",
    "build_extractor_exclude_paths",
    "normalize_exclude_paths",
    "normalize_include_globs",
    "resolve_effective_audit_root",
    "resolve_markdown_scope",
    "validate_generated_at_utc",
    "validate_snapshot_source",
]
