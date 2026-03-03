#!/usr/bin/env python3
"""Fail-closed checker for M248-E012 lane-E CI governance gate/closeout policy cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-e012-lane-e-ci-governance-gate-closeout-policy-cross-lane-integration-sync-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_lane_e_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_e012_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_packet.md"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-E012/lane_e_ci_governance_gate_closeout_policy_cross_lane_integration_sync_summary.json"
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
        "M248-E012-E011-01",
        "M248-E011",
        Path("docs/contracts/m248_lane_e_ci_governance_gate_and_closeout_policy_performance_and_quality_guardrails_e011_expectations.md"),
    ),
    AssetCheck(
        "M248-E012-E011-02",
        "M248-E011",
        Path("scripts/check_m248_e011_ci_governance_gate_and_closeout_policy_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M248-E012-E011-03",
        "M248-E011",
        Path("tests/tooling/test_check_m248_e011_ci_governance_gate_and_closeout_policy_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M248-E012-A012-01",
        "M248-A012",
        Path("docs/contracts/m248_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_a012_expectations.md"),
    ),
    AssetCheck(
        "M248-E012-A012-02",
        "M248-A012",
        Path("spec/planning/compiler/m248/m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_packet.md"),
    ),
    AssetCheck(
        "M248-E012-A012-03",
        "M248-A012",
        Path("scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E012-A012-04",
        "M248-A012",
        Path("tests/tooling/test_check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E012-B012-01",
        "M248-B012",
        Path("docs/contracts/m248_semantic_lowering_test_architecture_cross_lane_integration_sync_b012_expectations.md"),
    ),
    AssetCheck(
        "M248-E012-B012-02",
        "M248-B012",
        Path("spec/planning/compiler/m248/m248_b012_semantic_lowering_test_architecture_cross_lane_integration_sync_packet.md"),
    ),
    AssetCheck(
        "M248-E012-B012-03",
        "M248-B012",
        Path("scripts/check_m248_b012_semantic_lowering_test_architecture_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E012-B012-04",
        "M248-B012",
        Path("tests/tooling/test_check_m248_b012_semantic_lowering_test_architecture_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E012-C012-01",
        "M248-C012",
        Path("docs/contracts/m248_replay_harness_and_artifact_contracts_cross_lane_integration_sync_c012_expectations.md"),
    ),
    AssetCheck(
        "M248-E012-C012-02",
        "M248-C012",
        Path("spec/planning/compiler/m248/m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_packet.md"),
    ),
    AssetCheck(
        "M248-E012-C012-03",
        "M248-C012",
        Path("scripts/check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E012-C012-04",
        "M248-C012",
        Path("tests/tooling/test_check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E012-D012-01",
        "M248-D012",
        Path("docs/contracts/m248_runner_reliability_and_platform_operations_cross_lane_integration_sync_d012_expectations.md"),
    ),
    AssetCheck(
        "M248-E012-D012-02",
        "M248-D012",
        Path("spec/planning/compiler/m248/m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_packet.md"),
    ),
    AssetCheck(
        "M248-E012-D012-03",
        "M248-D012",
        Path("scripts/check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E012-D012-04",
        "M248-D012",
        Path("tests/tooling/test_check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E012-DOC-EXP-01",
        "# M248 Lane E CI Governance Gate and Closeout Policy Cross-Lane Integration Sync Expectations (E012)",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-cross-lane-integration-sync/m248-e012-v1`",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-03",
        "Issue `#6872` defines canonical lane-E cross-lane integration sync scope.",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-04",
        "Dependencies: `M248-E011`, `M248-A012`, `M248-B012`, `M248-C012`, `M248-D012`",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-05",
        "Fail closed unless M248 lane-E cross-lane integration sync dependency",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-06",
        "Readiness command chain enforces E011 and lane A/B/C/D dependency",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-07",
        "`check:objc3c:m248-e012-lane-e-readiness` remains chained from:",
    ),
    SnippetCheck("M248-E012-DOC-EXP-08", "`check:objc3c:m248-e011-lane-e-readiness`"),
    SnippetCheck("M248-E012-DOC-EXP-09", "`check:objc3c:m248-a012-lane-a-readiness`"),
    SnippetCheck("M248-E012-DOC-EXP-10", "`check:objc3c:m248-b012-lane-b-readiness`"),
    SnippetCheck("M248-E012-DOC-EXP-11", "`check:objc3c:m248-c012-lane-c-readiness`"),
    SnippetCheck("M248-E012-DOC-EXP-12", "`check:objc3c:m248-d012-lane-d-readiness`"),
    SnippetCheck(
        "M248-E012-DOC-EXP-13",
        "under repeated validation runs with stable failure ordering.",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-14",
        "`check:objc3c:m248-e012-ci-governance-gate-closeout-policy-cross-lane-integration-sync-contract`",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-15",
        "`test:tooling:m248-e012-ci-governance-gate-closeout-policy-cross-lane-integration-sync-contract`",
    ),
    SnippetCheck(
        "M248-E012-DOC-EXP-16",
        "`tmp/reports/m248/M248-E012/lane_e_ci_governance_gate_closeout_policy_cross_lane_integration_sync_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E012-DOC-PKT-01", "Packet: `M248-E012`"),
    SnippetCheck("M248-E012-DOC-PKT-02", "Issue: `#6872`"),
    SnippetCheck(
        "M248-E012-DOC-PKT-03",
        "Dependencies: `M248-E011`, `M248-A012`, `M248-B012`, `M248-C012`, `M248-D012`",
    ),
    SnippetCheck(
        "M248-E012-DOC-PKT-04",
        "`scripts/check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M248-E012-DOC-PKT-05",
        "`tests/tooling/test_check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M248-E012-DOC-PKT-06",
        "`check:objc3c:m248-e012-ci-governance-gate-closeout-policy-cross-lane-integration-sync-contract`",
    ),
    SnippetCheck(
        "M248-E012-DOC-PKT-07",
        "`test:tooling:m248-e012-ci-governance-gate-closeout-policy-cross-lane-integration-sync-contract`",
    ),
    SnippetCheck("M248-E012-DOC-PKT-08", "`check:objc3c:m248-e012-lane-e-readiness`"),
    SnippetCheck("M248-E012-DOC-PKT-09", "`check:objc3c:m248-e011-lane-e-readiness`"),
    SnippetCheck("M248-E012-DOC-PKT-10", "`check:objc3c:m248-a012-lane-a-readiness`"),
    SnippetCheck("M248-E012-DOC-PKT-11", "`check:objc3c:m248-b012-lane-b-readiness`"),
    SnippetCheck("M248-E012-DOC-PKT-12", "`check:objc3c:m248-c012-lane-c-readiness`"),
    SnippetCheck("M248-E012-DOC-PKT-13", "`check:objc3c:m248-d012-lane-d-readiness`"),
    SnippetCheck(
        "M248-E012-DOC-PKT-14",
        "`tmp/reports/m248/M248-E012/lane_e_ci_governance_gate_closeout_policy_cross_lane_integration_sync_summary.json`",
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
        (args.expectations_doc, "M248-E012-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M248-E012-DOC-PKT-EXISTS", PACKET_SNIPPETS),
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
