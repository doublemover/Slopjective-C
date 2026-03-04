#!/usr/bin/env python3
"""Fail-closed checker for M248-C014 replay-harness/artifact release replay dry-run contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-c014-replay-harness-and-artifact-contracts-release-candidate-and-replay-dry-run-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m248_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_c014_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_packet.md",
    "c013_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m248_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_c013_expectations.md",
    "c013_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_packet.md",
    "c013_checker": ROOT
    / "scripts"
    / "check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py",
    "c013_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py",
    "c013_readiness_runner": ROOT / "scripts" / "run_m248_c013_lane_c_readiness.py",
    "c014_run_script": ROOT
    / "scripts"
    / "run_m248_c014_replay_harness_and_artifact_contracts_release_replay_dry_run.ps1",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M248-C014-DOC-01",
            "Contract ID: `objc3c-replay-harness-and-artifact-contracts-release-candidate-and-replay-dry-run/m248-c014-v1`",
        ),
        ("M248-C014-DOC-02", "Dependencies: `M248-C013`"),
        ("M248-C014-DOC-03", "Issue `#6830`"),
        (
            "M248-C014-DOC-04",
            "Scope: lane-C replay harness/artifact release-candidate and replay dry-run continuity",
        ),
        ("M248-C014-DOC-05", "toolchain_runtime_ga_operations_cross_lane_integration_consistent"),
        ("M248-C014-DOC-06", "toolchain_runtime_ga_operations_docs_runbook_sync_consistent"),
        ("M248-C014-DOC-07", "long_tail_grammar_integration_closeout_consistent"),
        (
            "M248-C014-DOC-08",
            "scripts/check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py",
        ),
        (
            "M248-C014-DOC-09",
            "tests/tooling/test_check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py",
        ),
        (
            "M248-C014-DOC-10",
            "scripts/run_m248_c014_replay_harness_and_artifact_contracts_release_replay_dry_run.ps1",
        ),
        ("M248-C014-DOC-11", "tmp/reports/m248/M248-C014/replay_dry_run_summary.json"),
        (
            "M248-C014-DOC-12",
            "tmp/reports/m248/M248-C014/replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract_summary.json",
        ),
        (
            "M248-C014-DOC-13",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M248-C014-DOC-14", "## Milestone Optimization Inputs (Mandatory Scope Inputs)"),
        ("M248-C014-DOC-15", "`test:objc3c:perf-budget`"),
        ("M248-C014-DOC-16", "python scripts/run_m248_c013_lane_c_readiness.py"),
        (
            "M248-C014-DOC-17",
            "python scripts/check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py --emit-json",
        ),
    ),
    "packet_doc": (
        ("M248-C014-PKT-01", "Packet: `M248-C014`"),
        ("M248-C014-PKT-02", "Issue: `#6830`"),
        ("M248-C014-PKT-03", "Dependencies: `M248-C013`"),
        ("M248-C014-PKT-04", "## Dependency Anchors (M248-C013)"),
        (
            "M248-C014-PKT-05",
            "`scripts/check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py`",
        ),
        (
            "M248-C014-PKT-06",
            "`tests/tooling/test_check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py`",
        ),
        (
            "M248-C014-PKT-07",
            "`scripts/run_m248_c014_replay_harness_and_artifact_contracts_release_replay_dry_run.ps1`",
        ),
        ("M248-C014-PKT-08", "`python scripts/run_m248_c013_lane_c_readiness.py`"),
        ("M248-C014-PKT-09", "`module.object-backend.txt`"),
        ("M248-C014-PKT-10", "`tmp/reports/m248/M248-C014/replay_dry_run_summary.json`"),
        (
            "M248-C014-PKT-11",
            "`tmp/reports/m248/M248-C014/replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract_summary.json`",
        ),
        (
            "M248-C014-PKT-12",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
    ),
    "c013_expectations_doc": (
        (
            "M248-C014-DEP-01",
            "Contract ID: `objc3c-replay-harness-artifact-contracts-docs-operator-runbook-synchronization/m248-c013-v1`",
        ),
        ("M248-C014-DEP-02", "Dependencies: `M248-C012`"),
        ("M248-C014-DEP-03", "Issue `#6829`"),
    ),
    "c013_packet_doc": (
        ("M248-C014-DEP-04", "Packet: `M248-C013`"),
        ("M248-C014-DEP-05", "Issue: `#6829`"),
        ("M248-C014-DEP-06", "Dependencies: `M248-C012`"),
    ),
    "c013_checker": (
        (
            "M248-C014-DEP-07",
            "m248-c013-replay-harness-and-artifact-contracts-docs-operator-runbook-synchronization-contract-v1",
        ),
    ),
    "c013_tooling_test": (
        (
            "M248-C014-DEP-08",
            "check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract",
        ),
    ),
    "c013_readiness_runner": (
        (
            "M248-C014-DEP-09",
            "scripts/check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py",
        ),
        (
            "M248-C014-DEP-10",
            "tests/tooling/test_check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py",
        ),
    ),
    "c014_run_script": (
        ("M248-C014-RUN-01", '"module.manifest.json"'),
        ("M248-C014-RUN-02", '"module.diagnostics.json"'),
        ("M248-C014-RUN-03", '"module.ll"'),
        ("M248-C014-RUN-04", '"module.object-backend.txt"'),
        ("M248-C014-RUN-05", "--out-dir $run1"),
        ("M248-C014-RUN-06", "--out-dir $run2"),
        ("M248-C014-RUN-07", "deterministic replay drift detected in $fileName"),
        ("M248-C014-RUN-08", "readiness.ready_for_lowering"),
        ("M248-C014-RUN-09", "readiness.parse_artifact_replay_key_deterministic"),
        ("M248-C014-RUN-10", "readiness.parse_recovery_determinism_hardening_consistent"),
        ("M248-C014-RUN-11", "readiness.parse_lowering_conformance_matrix_consistent"),
        ("M248-C014-RUN-12", "readiness.parse_lowering_conformance_corpus_consistent"),
        ("M248-C014-RUN-13", "readiness.parse_lowering_performance_quality_guardrails_consistent"),
        ("M248-C014-RUN-14", "readiness.toolchain_runtime_ga_operations_cross_lane_integration_consistent"),
        ("M248-C014-RUN-15", "readiness.toolchain_runtime_ga_operations_docs_runbook_sync_consistent"),
        ("M248-C014-RUN-16", "readiness.long_tail_grammar_integration_closeout_consistent"),
        ("M248-C014-RUN-17", "readiness.long_tail_grammar_gate_signoff_ready"),
        ("M248-C014-RUN-18", "toolchain_runtime_ga_operations_cross_lane_integration_key="),
        ("M248-C014-RUN-19", "toolchain_runtime_ga_operations_docs_runbook_sync_key="),
        (
            "M248-C014-RUN-20",
            "objc3c-replay-harness-and-artifact-contracts-release-candidate-and-replay-dry-run/m248-c014-v1",
        ),
        ("M248-C014-RUN-21", "replay_dry_run_summary.json"),
    ),
    "architecture_doc": (
        (
            "M248-C014-ARC-01",
            "M248 lane-C C001 replay harness and artifact contract anchors",
        ),
        (
            "M248-C014-ARC-02",
            "M248 lane-C C002 replay harness and artifact modular split/scaffolding",
        ),
    ),
    "lowering_spec": (
        (
            "M248-C014-SPC-01",
            "conformance matrix implementation shall include deterministic conformance",
        ),
        (
            "M248-C014-SPC-02",
            "consistency and conformance-matrix readiness/key gates that fail closed",
        ),
    ),
    "metadata_spec": (
        (
            "M248-C014-META-01",
            "deterministic lane-C replay metadata anchors for `M248-C001`",
        ),
        (
            "M248-C014-META-02",
            "contract evidence and execution replay continuity so CI replay drift fails",
        ),
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
        default=Path(
            "tmp/reports/m248/M248-C014/replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise FileNotFoundError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for artifact, path in ARTIFACTS.items():
        checks_total += 1
        try:
            text = load_text(path, artifact=artifact)
            checks_passed += 1
        except FileNotFoundError as exc:
            findings.append(Finding(artifact, f"M248-C014-MISSING-{artifact.upper()}", str(exc)))
            continue

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    findings = sorted(findings, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }

    summary_out = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if summary["ok"]:
        if not args.emit_json:
            print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
        return 0

    if not args.emit_json:
        print(f"{MODE}: contract drift detected ({len(findings)} failed check(s)).", file=sys.stderr)
        for finding in findings:
            print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
