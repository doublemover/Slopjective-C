from __future__ import annotations

import fnmatch
import json
import re
import subprocess
from dataclasses import asdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Iterable

from .patterns import DEFAULT_EXCLUDES, DEFAULT_SCAN_ROOTS, FORBIDDEN_PATTERNS, TEXT_SUFFIXES

REPORT_SCHEMA_VERSION = "source-hygiene-hard-cutover-report-v1"
DEFAULT_ALLOWLIST = Path("tmp/source-hygiene-hard-cutover-allowlist.json")
DEFAULT_JSON_REPORT = Path("tmp/reports/source_hygiene/hard-cutover/report.json")
DEFAULT_TEXT_REPORT = Path("tmp/reports/source_hygiene/hard-cutover/report.txt")


def normalize_path(path: Path, root: Path) -> str:
    return path.resolve().relative_to(root.resolve()).as_posix()


def is_excluded(repo_path: str, excludes: Iterable[str]) -> bool:
    normalized = repo_path.replace("\\", "/")
    return any(fnmatch.fnmatch(normalized, pattern) for pattern in excludes)


def iter_scan_files(root: Path, scan_roots: Iterable[str], excludes: Iterable[str]) -> Iterable[Path]:
    for raw_scan_root in scan_roots:
        scan_root = root / raw_scan_root
        if not scan_root.exists():
            continue
        candidates = [scan_root] if scan_root.is_file() else scan_root.rglob("*")
        for candidate in candidates:
            if not candidate.is_file():
                continue
            repo_path = normalize_path(candidate, root)
            if is_excluded(repo_path, excludes):
                continue
            if candidate.suffix.lower() not in TEXT_SUFFIXES and candidate.name not in {"CMakeLists.txt", "package.json"}:
                continue
            yield candidate


def load_allowlist(path: Path | None) -> dict[str, Any]:
    if path is None or not path.is_file():
        return {"allowed": [], "allow_tracked_generated_reports": False}
    payload = json.loads(path.read_text(encoding="utf-8"))
    allowed = payload.get("allowed", [])
    if not isinstance(allowed, list):
        raise ValueError(f"allowlist {path} must contain an 'allowed' list")
    return {
        "allowed": allowed,
        "allow_tracked_generated_reports": bool(payload.get("allow_tracked_generated_reports", False)),
    }


def allowlist_matches(finding: dict[str, Any], allowlist_entry: Any) -> bool:
    if isinstance(allowlist_entry, str):
        return fnmatch.fnmatch(finding["path"], allowlist_entry)
    if not isinstance(allowlist_entry, dict):
        return False
    pattern_id = allowlist_entry.get("pattern_id", "*")
    if pattern_id != "*" and pattern_id != finding["pattern_id"]:
        return False
    path_glob = allowlist_entry.get("path_glob") or allowlist_entry.get("path")
    if path_glob and not fnmatch.fnmatch(finding["path"], str(path_glob)):
        return False
    line = allowlist_entry.get("line")
    return line is None or int(line) == int(finding["line"])


def is_allowed(finding: dict[str, Any], allowlist: dict[str, Any]) -> bool:
    return any(allowlist_matches(finding, entry) for entry in allowlist.get("allowed", []))


def scan_forbidden_patterns(root: Path, scan_roots: Iterable[str], excludes: Iterable[str]) -> list[dict[str, Any]]:
    compiled = [(pattern, re.compile(pattern.regex, re.IGNORECASE)) for pattern in FORBIDDEN_PATTERNS]
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


def tracked_generated_reports(root: Path) -> list[str]:
    try:
        result = subprocess.run(
            ["git", "ls-files", "reports"],
            cwd=root,
            check=False,
            text=True,
            capture_output=True,
        )
    except OSError:
        return []
    if result.returncode != 0:
        return []
    return sorted(line.strip().replace("\\", "/") for line in result.stdout.splitlines() if line.strip())


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


def write_reports(report: dict[str, Any], json_path: Path, text_path: Path) -> None:
    json_path.parent.mkdir(parents=True, exist_ok=True)
    text_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    lines = [
        f"schema_version: {report['schema_version']}",
        f"ok: {str(report['ok']).lower()}",
        f"active_findings: {report['stats']['active_finding_count']}",
        f"allowed_findings: {report['stats']['allowed_finding_count']}",
        f"tracked_generated_reports: {report['stats']['tracked_generated_report_count']}",
    ]
    for finding in report["active_findings"][:100]:
        lines.append(
            f"{finding['path']}:{finding['line']}: {finding['pattern_id']}: {finding['excerpt']}"
        )
    text_path.write_text("\n".join(lines) + "\n", encoding="utf-8")

