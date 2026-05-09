from __future__ import annotations

import re
from dataclasses import asdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Iterable

from .config import (
    DEFAULT_JSON_REPORT,
    DEFAULT_TEXT_REPORT,
    REPORT_SCHEMA_VERSION,
)
from .files import is_excluded, iter_scan_files, normalize_path
from .generated_reports import tracked_generated_reports
from .guardrails import is_canonical_guardrail_context
from .pattern_model import ForbiddenPattern
from .patterns import FORBIDDEN_PATTERNS
from .report_writer import write_reports
from .roots import DEFAULT_EXCLUDES, DEFAULT_SCAN_ROOTS

__all__ = [
    "DEFAULT_JSON_REPORT",
    "DEFAULT_TEXT_REPORT",
    "REPORT_SCHEMA_VERSION",
    "build_report",
    "is_excluded",
    "iter_scan_files",
    "normalize_path",
    "scan_forbidden_patterns",
    "tracked_generated_reports",
    "write_reports",
]


def _is_pattern_in_scope(pattern: ForbiddenPattern, repo_path: str) -> bool:
    if pattern.include_paths and not is_excluded(repo_path, pattern.include_paths):
        return False
    if pattern.exclude_paths and is_excluded(repo_path, pattern.exclude_paths):
        return False
    return True


def scan_forbidden_patterns(
    root: Path,
    scan_roots: Iterable[str],
    excludes: Iterable[str],
) -> list[dict[str, Any]]:
    compiled = [
        (pattern, re.compile(pattern.regex, re.IGNORECASE))
        for pattern in FORBIDDEN_PATTERNS
    ]
    findings: list[dict[str, Any]] = []
    for path in iter_scan_files(root, scan_roots, excludes):
        repo_path = normalize_path(path, root)
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except UnicodeDecodeError:
            lines = path.read_text(encoding="utf-8", errors="ignore").splitlines()
        for index, line in enumerate(lines):
            line_number = index + 1
            previous_line = lines[index - 1] if index > 0 else ""
            next_line = lines[index + 1] if index + 1 < len(lines) else ""
            for pattern, regex in compiled:
                if not _is_pattern_in_scope(pattern, repo_path):
                    continue
                if regex.search(line):
                    if is_canonical_guardrail_context(
                        repo_path,
                        previous_line,
                        line,
                        next_line,
                    ):
                        continue
                    findings.append(
                        {
                            "pattern_id": pattern.pattern_id,
                            "severity": pattern.severity,
                            "description": pattern.description,
                            "path": repo_path,
                            "line": line_number,
                            "excerpt": line.strip()[:240],
                        }
                    )
    findings.sort(key=lambda item: (item["path"], item["line"], item["pattern_id"]))
    return findings


def build_report(
    *,
    root: Path,
    scan_roots: Iterable[str] = DEFAULT_SCAN_ROOTS,
    excludes: Iterable[str] = DEFAULT_EXCLUDES,
) -> dict[str, Any]:
    findings = scan_forbidden_patterns(root, scan_roots, excludes)
    generated_reports = tracked_generated_reports(root)
    return {
        "schema_version": REPORT_SCHEMA_VERSION,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "root": str(root),
        "scan_roots": list(scan_roots),
        "excluded_globs": list(excludes),
        "forbidden_patterns": [asdict(pattern) for pattern in FORBIDDEN_PATTERNS],
        "findings": findings,
        "active_findings": findings,
        "tracked_generated_reports": generated_reports,
        "stats": {
            "finding_count": len(findings),
            "active_finding_count": len(findings),
            "tracked_generated_report_count": len(generated_reports),
        },
        "ok": len(findings) == 0 and not generated_reports,
    }
