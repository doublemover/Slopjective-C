from __future__ import annotations

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
from .gate_contracts import build_gate_stats, gate_contract_summary
from .generated_reports import (
    GENERATED_TRUTH_BOUNDARIES,
    generated_truth_boundary_report,
    tracked_generated_reports,
)
from .guardrails import is_canonical_guardrail_context
from .pattern_model import ForbiddenPattern
from .patterns import FORBIDDEN_PATTERNS
from .owners import source_hygiene_owner_contract_summary
from .report_writer import write_reports
from .roots import DEFAULT_EXCLUDES, DEFAULT_SCAN_ROOTS
from .scan_config import (
    SourceHygieneScanConfig,
    compile_policy_patterns,
    pattern_in_scope,
    scan_config_contract_summary,
)
from .violations import build_pattern_violation

__all__ = [
    "DEFAULT_JSON_REPORT",
    "DEFAULT_TEXT_REPORT",
    "REPORT_SCHEMA_VERSION",
    "build_report",
    "is_excluded",
    "iter_scan_files",
    "normalize_path",
    "scan_forbidden_patterns",
    "generated_truth_boundary_report",
    "tracked_generated_reports",
    "write_reports",
]


def scan_forbidden_patterns(
    root: Path,
    scan_roots: Iterable[str],
    excludes: Iterable[str],
    patterns: Iterable[ForbiddenPattern] = FORBIDDEN_PATTERNS,
) -> list[dict[str, Any]]:
    config = SourceHygieneScanConfig(
        scan_roots=tuple(scan_roots),
        excludes=tuple(excludes),
        patterns=tuple(patterns),
    )
    compiled = compile_policy_patterns(config.patterns)
    findings: list[dict[str, Any]] = []
    for path in iter_scan_files(root, config.scan_roots, config.excludes):
        repo_path = normalize_path(path, root)
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except UnicodeDecodeError:
            lines = path.read_text(encoding="utf-8", errors="ignore").splitlines()
        for index, line in enumerate(lines):
            line_number = index + 1
            previous_line = lines[index - 1] if index > 0 else ""
            next_line = lines[index + 1] if index + 1 < len(lines) else ""
            for compiled_pattern in compiled:
                pattern = compiled_pattern.pattern
                if not pattern_in_scope(pattern, repo_path):
                    continue
                if compiled_pattern.regex.search(line):
                    if is_canonical_guardrail_context(
                        repo_path,
                        previous_line,
                        line,
                        next_line,
                    ):
                        continue
                    findings.append(
                        build_pattern_violation(
                            pattern=pattern,
                            repo_path=repo_path,
                            line_number=line_number,
                            line=line,
                        )
                    )
    findings.sort(key=lambda item: (item["path"], item["line"], item["pattern_id"]))
    return findings


def build_report(
    *,
    root: Path,
    scan_roots: Iterable[str] = DEFAULT_SCAN_ROOTS,
    excludes: Iterable[str] = DEFAULT_EXCLUDES,
) -> dict[str, Any]:
    config = SourceHygieneScanConfig(
        scan_roots=tuple(scan_roots),
        excludes=tuple(excludes),
    )
    findings = scan_forbidden_patterns(
        root,
        config.scan_roots,
        config.excludes,
        config.patterns,
    )
    generated_reports = tracked_generated_reports(root)
    generated_truth = generated_truth_boundary_report(root)
    generated_truth_findings = generated_truth["findings"]
    stats = build_gate_stats(
        finding_count=len(findings),
        tracked_generated_report_count=len(generated_reports),
        generated_truth_boundary_finding_count=len(generated_truth_findings),
    )
    owner_contract = source_hygiene_owner_contract_summary(
        scan_roots=config.scan_roots,
        pattern_owner_surfaces=(
            pattern.owner_surface for pattern in config.patterns
        ),
        generated_truth_outputs=(
            boundary.output_path for boundary in GENERATED_TRUTH_BOUNDARIES
        ),
    )
    return {
        "schema_version": REPORT_SCHEMA_VERSION,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "root": str(root),
        "scan_roots": list(config.scan_roots),
        "excluded_globs": list(config.excludes),
        "gate_contract": gate_contract_summary(),
        "owner_contract": owner_contract,
        "scan_config_contract": scan_config_contract_summary(config),
        "blocker_metadata": owner_contract["blocker_metadata"],
        "forbidden_patterns": [asdict(pattern) for pattern in config.patterns],
        "findings": findings,
        "active_findings": findings,
        "tracked_generated_reports": generated_reports,
        "generated_truth_boundaries": generated_truth["boundaries"],
        "generated_truth_boundary_findings": generated_truth_findings,
        "stats": stats,
        "ok": len(findings) == 0
        and not generated_reports
        and not generated_truth_findings,
    }
