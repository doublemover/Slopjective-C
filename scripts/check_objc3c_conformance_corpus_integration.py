#!/usr/bin/env python3
"""Validate the integrated ObjC 3 conformance corpus surface."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
SURFACE_CHECK_PY = ROOT / "scripts" / "check_conformance_corpus_surface.py"
INDEX_PY = ROOT / "scripts" / "generate_conformance_corpus_index.py"
SUITE_GATE_PS1 = ROOT / "scripts" / "check_conformance_suite.ps1"
SURFACE_SUMMARY = ROOT / "tmp" / "reports" / "conformance" / "corpus-surface-summary.json"
INDEX_SUMMARY = ROOT / "tmp" / "reports" / "conformance" / "corpus-index.json"
SUPPORT_CLAIM_TRACEABILITY_SUMMARY = (
    ROOT / "tmp" / "reports" / "conformance" / "support-claim-runnable-evidence-summary.json"
)
REPORT_PATH = ROOT / "tmp" / "reports" / "conformance" / "corpus-integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.conformance.corpus.integration.summary.v1"






def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    steps = [
        ("check-conformance-corpus-surface", run_capture(python_script_command(SURFACE_CHECK_PY))),
        ("generate-conformance-corpus-index", run_capture(python_script_command(INDEX_PY))),
    ]

    failures: list[str] = []
    for name, result in steps:
        expect(result.returncode == 0, f"{name} failed", failures)

    expect(SURFACE_SUMMARY.is_file(), f"missing corpus surface summary: {repo_rel(SURFACE_SUMMARY)}", failures)
    expect(INDEX_SUMMARY.is_file(), f"missing corpus index summary: {repo_rel(INDEX_SUMMARY)}", failures)
    expect(
        SUPPORT_CLAIM_TRACEABILITY_SUMMARY.is_file(),
        f"missing support claim traceability summary: {repo_rel(SUPPORT_CLAIM_TRACEABILITY_SUMMARY)}",
        failures,
    )

    surface_summary = load_json(SURFACE_SUMMARY) if SURFACE_SUMMARY.is_file() else {}
    index_summary = load_json(INDEX_SUMMARY) if INDEX_SUMMARY.is_file() else {}
    traceability_summary = (
        load_json(SUPPORT_CLAIM_TRACEABILITY_SUMMARY)
        if SUPPORT_CLAIM_TRACEABILITY_SUMMARY.is_file()
        else {}
    )

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
        traceability_summary.get("contract_id")
        == "objc3c.conformance.support_claim_runnable_evidence.summary.v1",
        "unexpected support claim runnable evidence summary contract id",
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
    support_claim_traceability = index_summary.get("support_claim_traceability")
    expect(
        isinstance(support_claim_traceability, dict)
        and support_claim_traceability.get("row_count", 0) >= 1
        and "objc3c.behavior.runtime.object-model-interface-method-table"
        in support_claim_traceability.get("support_claims", []),
        "conformance corpus index missing support-claim runnable traceability",
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
        "child_report_paths": [
            repo_rel(SURFACE_SUMMARY),
            repo_rel(INDEX_SUMMARY),
            repo_rel(SUPPORT_CLAIM_TRACEABILITY_SUMMARY),
        ],
        "workflow_actions": [
            "validate-conformance-corpus",
            "check-conformance-corpus-surface",
            "generate-conformance-corpus-index",
        ],
        "legacy_suite_gate_script": repo_rel(SUITE_GATE_PS1),
        "retained_suite_count": len(retained_partition) if isinstance(retained_partition, list) else 0,
        "manifest_summary_count": len(manifest_summaries) if isinstance(manifest_summaries, list) else 0,
        "support_claim_traceability_row_count": (
            support_claim_traceability.get("row_count", 0)
            if isinstance(support_claim_traceability, dict)
            else 0
        ),
        "failures": failures,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
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
