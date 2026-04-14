from __future__ import annotations

import re
from pathlib import Path
from typing import Any, Iterable, Sequence

from objc3c_tooling.paths import ROOT, repo_rel

RUNTIME_ACCEPTANCE_PASS_RE = re.compile(r"runtime-acceptance:\s+PASS\s+\((.+)\)")
RUNNABLE_REPORT_PREFIXES = ("summary_path", "public-workflow-report")
PUBLIC_WORKFLOW_REPORT_PREFIXES = (
    "summary_path",
    "dump_path",
    "workspace_path",
    "template_path",
    "harness_path",
    "out_dir",
)


def normalize_newlines(value: str) -> str:
    return value.replace("\r\n", "\n").replace("\r", "\n")


def extract_line_value(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line[len(prefix) :].strip()
    return ""


def extract_output_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    value = extract_line_value(stdout, prefix)
    return value if value != "" else None


def to_repo_relative(raw_path: str, *, root: Path = ROOT) -> str:
    stripped = raw_path.strip()
    candidate = Path(stripped)
    try:
        if candidate.is_absolute():
            return candidate.resolve().relative_to(root.resolve()).as_posix()
    except ValueError:
        return stripped.replace("\\", "/")
    return stripped.replace("\\", "/")


def _append_prefixed_path(report_paths: list[str], line: str, prefixes: Iterable[str], *, root: Path) -> bool:
    for prefix in prefixes:
        marker = f"{prefix}:"
        if line.startswith(marker):
            report_paths.append(to_repo_relative(line.split(":", 1)[1], root=root))
            return True
    return False


def extract_report_paths(
    stdout: str,
    *,
    prefixes: Sequence[str] = RUNNABLE_REPORT_PREFIXES,
    include_runtime_acceptance: bool = True,
    root: Path = ROOT,
) -> list[str]:
    report_paths: list[str] = []
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if _append_prefixed_path(report_paths, line, prefixes, root=root):
            continue
        if include_runtime_acceptance:
            match = RUNTIME_ACCEPTANCE_PASS_RE.search(line)
            if match:
                candidate = Path(match.group(1).strip())
                try:
                    report_paths.append(repo_rel(candidate, root=root))
                except ValueError:
                    report_paths.append(match.group(1).strip().replace("\\", "/"))
    return report_paths


def extract_public_workflow_report_paths(stdout: str, *, root: Path = ROOT) -> list[str]:
    return extract_report_paths(
        stdout,
        prefixes=PUBLIC_WORKFLOW_REPORT_PREFIXES,
        include_runtime_acceptance=True,
        root=root,
    )


def case_ids_from_acceptance(report: dict[str, Any]) -> set[str]:
    return {
        case.get("case_id")
        for case in report.get("cases", [])
        if isinstance(case, dict) and isinstance(case.get("case_id"), str) and case.get("case_id")
    }
