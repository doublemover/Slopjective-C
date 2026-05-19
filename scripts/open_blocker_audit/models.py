"""Open-blocker audit runner models."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from scripts.open_blocker_extraction.audit_runner import CommandResult, CommandSpec


@dataclass(frozen=True)
class AuditScope:
    audit_root: Path
    effective_audit_root: Path
    include_globs: tuple[str, ...]
    exclude_paths: tuple[str, ...]
    included_markdown_paths: tuple[str, ...]
    excluded_markdown_paths: tuple[str, ...]
    extractor_exclude_paths: tuple[str, ...]


@dataclass(frozen=True)
class SnapshotMetadata:
    generated_at_utc: str | None
    source: str | None


@dataclass(frozen=True)
class AuditOutputPaths:
    output_dir: Path
    snapshot_json_path: Path
    extract_log_path: Path
    summary_json_path: Path
    report_md_path: Path
    contract_check_transcript_path: Path
    contract_check_stderr_path: Path


__all__ = [
    "AuditOutputPaths",
    "AuditScope",
    "CommandResult",
    "CommandSpec",
    "SnapshotMetadata",
]
