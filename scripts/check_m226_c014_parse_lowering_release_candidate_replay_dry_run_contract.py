#!/usr/bin/env python3
"""Fail-closed validator for M226-C014 release-candidate replay dry-run contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-c014-parse-lowering-release-replay-dry-run-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "c013_contract_doc": ROOT / "docs" / "contracts" / "m226_parse_lowering_docs_runbook_sync_c013_expectations.md",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parse_lowering_release_candidate_replay_dry_run_c014_expectations.md",
    "run_script": ROOT / "scripts" / "run_m226_c014_parse_lowering_release_replay_dry_run.ps1",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "c013_contract_doc": (
        (
            "M226-C014-DEP-01",
            "Contract ID: `objc3c-parse-lowering-docs-runbook-sync-contract/m226-c013-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M226-C014-DOC-01",
            "Contract ID: `objc3c-parse-lowering-release-replay-dry-run-contract/m226-c014-v1`",
        ),
        ("M226-C014-DOC-02", "scripts/run_m226_c014_parse_lowering_release_replay_dry_run.ps1"),
        ("M226-C014-DOC-03", "toolchain_runtime_ga_operations_cross_lane_integration_consistent"),
        ("M226-C014-DOC-04", "toolchain_runtime_ga_operations_docs_runbook_sync_consistent"),
        ("M226-C014-DOC-05", "tmp/reports/m226/M226-C014/replay_dry_run_summary.json"),
    ),
    "run_script": (
        ("M226-C014-RUN-01", '"module.manifest.json"'),
        ("M226-C014-RUN-02", '"module.diagnostics.json"'),
        ("M226-C014-RUN-03", '"module.ll"'),
        ("M226-C014-RUN-04", '"module.object-backend.txt"'),
        ("M226-C014-RUN-05", "readiness.ready_for_lowering"),
        ("M226-C014-RUN-06", "readiness.parse_artifact_replay_key_deterministic"),
        ("M226-C014-RUN-07", "readiness.parse_recovery_determinism_hardening_consistent"),
        ("M226-C014-RUN-08", "readiness.parse_lowering_conformance_matrix_consistent"),
        ("M226-C014-RUN-09", "readiness.parse_lowering_conformance_corpus_consistent"),
        ("M226-C014-RUN-10", "readiness.parse_lowering_performance_quality_guardrails_consistent"),
        ("M226-C014-RUN-11", "readiness.toolchain_runtime_ga_operations_cross_lane_integration_consistent"),
        ("M226-C014-RUN-12", "readiness.toolchain_runtime_ga_operations_cross_lane_integration_ready"),
        ("M226-C014-RUN-13", "readiness.toolchain_runtime_ga_operations_docs_runbook_sync_consistent"),
        ("M226-C014-RUN-14", "readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready"),
        ("M226-C014-RUN-15", "toolchain_runtime_ga_operations_cross_lane_integration_key="),
        ("M226-C014-RUN-16", "toolchain_runtime_ga_operations_docs_runbook_sync_key="),
        ("M226-C014-RUN-17", "objc3c-parse-lowering-release-replay-dry-run-contract/m226-c014-v1"),
        ("M226-C014-RUN-18", "replay_dry_run_summary.json"),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m226/m226_c014_parse_lowering_release_replay_dry_run_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
