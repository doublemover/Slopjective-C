#!/usr/bin/env python3
"""Validate the integrated ObjC 3 conformance corpus surface."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
SURFACE_CHECK_PY = ROOT / "scripts" / "check_conformance_corpus_surface.py"
INDEX_PY = ROOT / "scripts" / "generate_conformance_corpus_index.py"
SUITE_GATE_PS1 = ROOT / "scripts" / "check_conformance_suite.ps1"
SURFACE_SUMMARY = ROOT / "tmp" / "reports" / "conformance" / "corpus-surface-summary.json"
INDEX_SUMMARY = ROOT / "tmp" / "reports" / "conformance" / "corpus-index.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "conformance" / "corpus-integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.conformance.corpus.integration.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    steps = [
        ("check-conformance-corpus-surface", run_capture([sys.executable, str(SURFACE_CHECK_PY)])),
        ("generate-conformance-corpus-index", run_capture([sys.executable, str(INDEX_PY)])),
    ]

    failures: list[str] = []
    for name, result in steps:
        expect(result.returncode == 0, f"{name} failed", failures)

    expect(SURFACE_SUMMARY.is_file(), f"missing corpus surface summary: {repo_rel(SURFACE_SUMMARY)}", failures)
    expect(INDEX_SUMMARY.is_file(), f"missing corpus index summary: {repo_rel(INDEX_SUMMARY)}", failures)

    surface_summary = load_json(SURFACE_SUMMARY) if SURFACE_SUMMARY.is_file() else {}
    index_summary = load_json(INDEX_SUMMARY) if INDEX_SUMMARY.is_file() else {}

    expect(
        surface_summary.get("contract_id") == "objc3c.conformance.corpus.surface.summary.v1",
        "unexpected conformance corpus surface summary contract id",
        failures,
    )
    expect(
        index_summary.get("contract_id") == "objc3c.conformance.corpus.index.v1",
        "unexpected conformance corpus index contract id",
        failures,
    )
    expect(
        surface_summary.get("status") == "PASS",
        "conformance corpus surface summary did not report PASS",
        failures,
    )
    retained_partition = index_summary.get("retained_partition")
    expect(
        isinstance(retained_partition, list) and len(retained_partition) >= 4,
        "conformance corpus index missing retained longitudinal suites",
        failures,
    )
    manifest_summaries = index_summary.get("manifest_summaries")
    expect(
        isinstance(manifest_summaries, list) and len(manifest_summaries) >= 7,
        "conformance corpus index missing manifest summaries",
        failures,
    )
    workflow_surface = index_summary.get("workflow_surface")
    expect(
        isinstance(workflow_surface, dict)
        and workflow_surface.get("legacy_suite_gate_script") == "scripts/check_conformance_suite.ps1",
        "conformance corpus workflow surface drifted from the legacy suite gate contract",
        failures,
    )
    expect(
        SUITE_GATE_PS1.is_file(),
        "conformance corpus legacy suite gate script is missing from the live repo surface",
        failures,
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_conformance_corpus_integration.py",
        "child_report_paths": [repo_rel(SURFACE_SUMMARY), repo_rel(INDEX_SUMMARY)],
        "workflow_actions": [
            "validate-conformance-corpus",
            "check-conformance-corpus-surface",
            "generate-conformance-corpus-index",
        ],
        "legacy_suite_gate_script": repo_rel(SUITE_GATE_PS1),
        "retained_suite_count": len(retained_partition) if isinstance(retained_partition, list) else 0,
        "manifest_summary_count": len(manifest_summaries) if isinstance(manifest_summaries, list) else 0,
        "failures": failures,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    if failures:
        print("objc3c-conformance-corpus-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-conformance-corpus-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
