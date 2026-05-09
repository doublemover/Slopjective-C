from __future__ import annotations

import re
from dataclasses import asdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Iterable

from .allowlist import allowlist_matches, is_allowed, load_allowlist
from .config import (
    DEFAULT_ALLOWLIST,
    DEFAULT_JSON_REPORT,
    DEFAULT_TEXT_REPORT,
    REPORT_SCHEMA_VERSION,
)
from .files import is_excluded, iter_scan_files, normalize_path
from .generated_reports import tracked_generated_reports
from .patterns import FORBIDDEN_PATTERNS
from .report_writer import write_reports
from .roots import DEFAULT_EXCLUDES, DEFAULT_SCAN_ROOTS

__all__ = [
    "DEFAULT_ALLOWLIST",
    "DEFAULT_JSON_REPORT",
    "DEFAULT_TEXT_REPORT",
    "REPORT_SCHEMA_VERSION",
    "allowlist_matches",
    "build_report",
    "is_allowed",
    "is_excluded",
    "iter_scan_files",
    "load_allowlist",
    "normalize_path",
    "scan_forbidden_patterns",
    "tracked_generated_reports",
    "write_reports",
]


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
        for line_number, line in enumerate(lines, start=1):
            for pattern, regex in compiled:
                if regex.search(line):
                    if is_negative_test_assertion(repo_path, line):
                        continue
                    if is_guardrail_identifier(line):
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


def is_negative_test_assertion(repo_path: str, line: str) -> bool:
    return repo_path.startswith("tests/") and "assert" in line and " not in " in line


def is_guardrail_identifier(line: str) -> bool:
    return any(
        marker in line
        for marker in (
            "no-milestone-local",
            "no milestone-local",
            "no-duplicate-milestone-local",
            "no-proof-only",
        )
    )


def build_report(
    *,
    root: Path,
    scan_roots: Iterable[str] = DEFAULT_SCAN_ROOTS,
    excludes: Iterable[str] = DEFAULT_EXCLUDES,
    allowlist_path: Path | None = None,
) -> dict[str, Any]:
    allowlist = load_allowlist(allowlist_path)
    findings = scan_forbidden_patterns(root, scan_roots, excludes)
    allowed_findings = [finding for finding in findings if is_allowed(finding, allowlist)]
    active_findings = [finding for finding in findings if not is_allowed(finding, allowlist)]
    generated_reports = tracked_generated_reports(root)
    generated_reports_ok = not generated_reports or allowlist.get("allow_tracked_generated_reports", False)
    return {
        "schema_version": REPORT_SCHEMA_VERSION,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "root": str(root),
        "scan_roots": list(scan_roots),
        "excluded_globs": list(excludes),
        "allowlist_path": str(allowlist_path) if allowlist_path else "",
        "allowlist_active": bool(allowlist_path and allowlist_path.is_file()),
        "forbidden_patterns": [asdict(pattern) for pattern in FORBIDDEN_PATTERNS],
        "findings": findings,
        "allowed_findings": allowed_findings,
        "active_findings": active_findings,
        "tracked_generated_reports": generated_reports,
        "stats": {
            "finding_count": len(findings),
            "allowed_finding_count": len(allowed_findings),
            "active_finding_count": len(active_findings),
            "tracked_generated_report_count": len(generated_reports),
        },
        "ok": len(active_findings) == 0 and generated_reports_ok,
    }
