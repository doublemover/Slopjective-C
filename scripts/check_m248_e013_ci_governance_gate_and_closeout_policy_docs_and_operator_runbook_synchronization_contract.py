#!/usr/bin/env python3
"""Fail-closed checker for M248-E013 lane-E CI governance gate/closeout policy docs/runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-e013-lane-e-ci-governance-gate-closeout-policy-docs-operator-runbook-synchronization-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_lane_e_ci_governance_gate_and_closeout_policy_docs_and_operator_runbook_synchronization_e013_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_e013_ci_governance_gate_and_closeout_policy_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-E013/lane_e_ci_governance_gate_closeout_policy_docs_and_operator_runbook_synchronization_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M248-E013-E012-01",
        "M248-E012",
        Path("docs/contracts/m248_lane_e_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_e012_expectations.md"),
    ),
    AssetCheck(
        "M248-E013-E012-02",
        "M248-E012",
        Path("scripts/check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E013-E012-03",
        "M248-E012",
        Path("tests/tooling/test_check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E013-A013-01",
        "M248-A013",
        Path("docs/contracts/m248_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_a013_expectations.md"),
    ),
    AssetCheck(
        "M248-E013-A013-02",
        "M248-A013",
        Path("spec/planning/compiler/m248/m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_packet.md"),
    ),
    AssetCheck(
        "M248-E013-A013-03",
        "M248-A013",
        Path("scripts/check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M248-E013-A013-04",
        "M248-A013",
        Path("tests/tooling/test_check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M248-E013-B013-01",
        "M248-B013",
        Path("docs/contracts/m248_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_b013_expectations.md"),
    ),
    AssetCheck(
        "M248-E013-B013-02",
        "M248-B013",
        Path("spec/planning/compiler/m248/m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_packet.md"),
    ),
    AssetCheck(
        "M248-E013-B013-03",
        "M248-B013",
        Path("scripts/check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M248-E013-B013-04",
        "M248-B013",
        Path("tests/tooling/test_check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M248-E013-C013-01",
        "M248-C013",
        Path("docs/contracts/m248_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_c013_expectations.md"),
    ),
    AssetCheck(
        "M248-E013-C013-02",
        "M248-C013",
        Path("spec/planning/compiler/m248/m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_packet.md"),
    ),
    AssetCheck(
        "M248-E013-C013-03",
        "M248-C013",
        Path("scripts/check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M248-E013-C013-04",
        "M248-C013",
        Path("tests/tooling/test_check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M248-E013-D013-01",
        "M248-D013",
        Path("docs/contracts/m248_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_d013_expectations.md"),
    ),
    AssetCheck(
        "M248-E013-D013-02",
        "M248-D013",
        Path("spec/planning/compiler/m248/m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_packet.md"),
    ),
    AssetCheck(
        "M248-E013-D013-03",
        "M248-D013",
        Path("scripts/check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M248-E013-D013-04",
        "M248-D013",
        Path("tests/tooling/test_check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E013-DOC-EXP-01",
        "# M248 Lane E CI Governance Gate and Closeout Policy Docs and Operator Runbook Synchronization Expectations (E013)",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-docs-operator-runbook-synchronization/m248-e013-v1`",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-03",
        "Issue `#6873` defines canonical lane-E docs and operator runbook synchronization scope.",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-04",
        "Dependencies: `M248-E012`, `M248-A013`, `M248-B013`, `M248-C013`, `M248-D013`",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-05",
        "Fail closed unless M248 lane-E docs/operator runbook synchronization dependency",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-06",
        "Readiness command chain enforces E012 and lane A/B/C/D dependency",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-07",
        "`check:objc3c:m248-e013-lane-e-readiness` remains chained from:",
    ),
    SnippetCheck("M248-E013-DOC-EXP-08", "`check:objc3c:m248-e012-lane-e-readiness`"),
    SnippetCheck("M248-E013-DOC-EXP-09", "`check:objc3c:m248-a013-lane-a-readiness`"),
    SnippetCheck("M248-E013-DOC-EXP-10", "`check:objc3c:m248-b013-lane-b-readiness`"),
    SnippetCheck("M248-E013-DOC-EXP-11", "`check:objc3c:m248-c013-lane-c-readiness`"),
    SnippetCheck("M248-E013-DOC-EXP-12", "`check:objc3c:m248-d013-lane-d-readiness`"),
    SnippetCheck(
        "M248-E013-DOC-EXP-13",
        "under repeated validation runs with stable failure ordering.",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-14",
        "`check:objc3c:m248-e013-ci-governance-gate-closeout-policy-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-15",
        "`test:tooling:m248-e013-ci-governance-gate-closeout-policy-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck(
        "M248-E013-DOC-EXP-16",
        "`tmp/reports/m248/M248-E013/lane_e_ci_governance_gate_closeout_policy_docs_and_operator_runbook_synchronization_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E013-DOC-PKT-01", "Packet: `M248-E013`"),
    SnippetCheck("M248-E013-DOC-PKT-02", "Issue: `#6873`"),
    SnippetCheck(
        "M248-E013-DOC-PKT-03",
        "Dependencies: `M248-E012`, `M248-A013`, `M248-B013`, `M248-C013`, `M248-D013`",
    ),
    SnippetCheck(
        "M248-E013-DOC-PKT-04",
        "`scripts/check_m248_e013_ci_governance_gate_and_closeout_policy_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M248-E013-DOC-PKT-05",
        "`tests/tooling/test_check_m248_e013_ci_governance_gate_and_closeout_policy_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M248-E013-DOC-PKT-06",
        "`check:objc3c:m248-e013-ci-governance-gate-closeout-policy-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck(
        "M248-E013-DOC-PKT-07",
        "`test:tooling:m248-e013-ci-governance-gate-closeout-policy-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck("M248-E013-DOC-PKT-08", "`check:objc3c:m248-e013-lane-e-readiness`"),
    SnippetCheck("M248-E013-DOC-PKT-09", "`check:objc3c:m248-e012-lane-e-readiness`"),
    SnippetCheck("M248-E013-DOC-PKT-10", "`check:objc3c:m248-a013-lane-a-readiness`"),
    SnippetCheck("M248-E013-DOC-PKT-11", "`check:objc3c:m248-b013-lane-b-readiness`"),
    SnippetCheck("M248-E013-DOC-PKT-12", "`check:objc3c:m248-c013-lane-c-readiness`"),
    SnippetCheck("M248-E013-DOC-PKT-13", "`check:objc3c:m248-d013-lane-d-readiness`"),
    SnippetCheck(
        "M248-E013-DOC-PKT-14",
        "`tmp/reports/m248/M248-E013/lane_e_ci_governance_gate_closeout_policy_docs_and_operator_runbook_synchronization_summary.json`",
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_text_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required text artifact is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"unable to read required text artifact: {exc}",
            )
        )
        return checks_total, findings

    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def finding_sort_key(finding: Finding) -> tuple[str, str, str]:
    return (finding.artifact, finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M248-E013-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M248-E013-DOC-PKT-EXISTS", PACKET_SNIPPETS),
    ):
        count, findings = check_text_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        sys.stdout.write(canonical_json(summary_payload))

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
