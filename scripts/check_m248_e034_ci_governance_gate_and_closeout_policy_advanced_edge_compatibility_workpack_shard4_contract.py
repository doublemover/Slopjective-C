#!/usr/bin/env python3
"""Fail-closed checker for M248-E034 lane-E CI governance advanced edge compatibility workpack shard4."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-e034-lane-e-ci-governance-gate-closeout-policy-advanced-edge-compatibility-workpack-shard4-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_lane_e_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard4_e034_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_e034_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard4_packet.md"
)
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m248_e034_lane_e_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-E034/lane_e_ci_governance_gate_closeout_policy_advanced_edge_compatibility_workpack_shard4_summary.json"
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
        "M248-E034-A013-01",
        "M248-A013",
        Path("docs/contracts/m248_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_a013_expectations.md"),
    ),
    AssetCheck(
        "M248-E034-A013-02",
        "M248-A013",
        Path("spec/planning/compiler/m248/m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_packet.md"),
    ),
    AssetCheck(
        "M248-E034-A013-03",
        "M248-A013",
        Path("scripts/check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M248-E034-A013-04",
        "M248-A013",
        Path("tests/tooling/test_check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M248-E034-A013-05",
        "M248-A013",
        Path("scripts/run_m248_a013_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M248-E034-B016-01",
        "M248-B016",
        Path("docs/contracts/m248_semantic_lowering_test_architecture_integration_closeout_and_gate_signoff_b016_expectations.md"),
    ),
    AssetCheck(
        "M248-E034-B016-02",
        "M248-B016",
        Path("spec/planning/compiler/m248/m248_b016_semantic_lowering_test_architecture_integration_closeout_and_gate_signoff_packet.md"),
    ),
    AssetCheck(
        "M248-E034-B016-03",
        "M248-B016",
        Path("scripts/check_m248_b016_semantic_lowering_test_architecture_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M248-E034-B016-04",
        "M248-B016",
        Path("tests/tooling/test_check_m248_b016_semantic_lowering_test_architecture_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M248-E034-B016-05",
        "M248-B016",
        Path("scripts/run_m248_b016_lane_b_readiness.py"),
    ),
    AssetCheck(
        "M248-E034-C018-01",
        "M248-C018",
        Path("docs/contracts/m248_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_c018_expectations.md"),
    ),
    AssetCheck(
        "M248-E034-C018-02",
        "M248-C018",
        Path("spec/planning/compiler/m248/m248_c018_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_packet.md"),
    ),
    AssetCheck(
        "M248-E034-C018-03",
        "M248-C018",
        Path("scripts/check_m248_c018_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_contract.py"),
    ),
    AssetCheck(
        "M248-E034-C018-04",
        "M248-C018",
        Path("tests/tooling/test_check_m248_c018_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_contract.py"),
    ),
    AssetCheck(
        "M248-E034-C018-05",
        "M248-C018",
        Path("scripts/run_m248_c018_lane_c_readiness.py"),
    ),
    AssetCheck(
        "M248-E034-D024-01",
        "M248-D024",
        Path("docs/contracts/m248_runner_reliability_and_platform_operations_advanced_conformance_workpack_shard2_d024_expectations.md"),
    ),
    AssetCheck(
        "M248-E034-D024-02",
        "M248-D024",
        Path("spec/planning/compiler/m248/m248_d024_runner_reliability_and_platform_operations_advanced_conformance_workpack_shard2_packet.md"),
    ),
    AssetCheck(
        "M248-E034-D024-03",
        "M248-D024",
        Path("scripts/check_m248_d024_runner_reliability_and_platform_operations_advanced_conformance_workpack_shard2_contract.py"),
    ),
    AssetCheck(
        "M248-E034-D024-04",
        "M248-D024",
        Path("tests/tooling/test_check_m248_d024_runner_reliability_and_platform_operations_advanced_conformance_workpack_shard2_contract.py"),
    ),
    AssetCheck(
        "M248-E034-D024-05",
        "M248-D024",
        Path("scripts/run_m248_d024_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E034-DOC-EXP-01",
        "# M248 Lane E CI Governance Gate and Closeout Policy Advanced Edge Compatibility Workpack (Shard 4) Expectations (E034)",
    ),
    SnippetCheck(
        "M248-E034-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-advanced-edge-compatibility-workpack-shard4/m248-e034-v1`",
    ),
    SnippetCheck(
        "M248-E034-DOC-EXP-03",
        "Dependencies: `M248-E033`, `M248-A013`, `M248-B016`, `M248-C018`, `M248-D024`",
    ),
    SnippetCheck(
        "M248-E034-DOC-EXP-04",
        "Issue `#6894` defines canonical lane-E advanced edge compatibility workpack (shard 4) scope.",
    ),
    SnippetCheck("M248-E034-DOC-EXP-05", "`advanced_edge_compatibility_shard4_consistent`"),
    SnippetCheck("M248-E034-DOC-EXP-06", "`advanced_edge_compatibility_shard4_ready`"),
    SnippetCheck("M248-E034-DOC-EXP-07", "`advanced_edge_compatibility_shard4_key`"),
    SnippetCheck("M248-E034-DOC-EXP-08", "`check:objc3c:m248-e033-lane-e-readiness`"),
    SnippetCheck("M248-E034-DOC-EXP-09", "`check:objc3c:m248-e034-lane-e-readiness`"),
    SnippetCheck(
        "M248-E034-DOC-EXP-10",
        "`tmp/reports/m248/M248-E034/lane_e_ci_governance_gate_closeout_policy_advanced_edge_compatibility_workpack_shard4_summary.json`",
    ),
    SnippetCheck(
        "M248-E034-DOC-EXP-11",
        "`docs/contracts/m248_lane_e_ci_governance_gate_and_closeout_policy_advanced_core_workpack_shard4_e033_expectations.md`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E034-DOC-PKT-01", "Packet: `M248-E034`"),
    SnippetCheck("M248-E034-DOC-PKT-02", "Issue: `#6894`"),
    SnippetCheck(
        "M248-E034-DOC-PKT-03",
        "Dependencies: `M248-E033`, `M248-A013`, `M248-B016`, `M248-C018`, `M248-D024`",
    ),
    SnippetCheck(
        "M248-E034-DOC-PKT-04",
        "`scripts/check_m248_e034_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard4_contract.py`",
    ),
    SnippetCheck(
        "M248-E034-DOC-PKT-05",
        "`tests/tooling/test_check_m248_e034_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard4_contract.py`",
    ),
    SnippetCheck("M248-E034-DOC-PKT-06", "`scripts/run_m248_e034_lane_e_readiness.py`"),
    SnippetCheck("M248-E034-DOC-PKT-07", "`advanced_edge_compatibility_shard4_consistent`"),
    SnippetCheck("M248-E034-DOC-PKT-08", "`advanced_edge_compatibility_shard4_ready`"),
    SnippetCheck("M248-E034-DOC-PKT-09", "`advanced_edge_compatibility_shard4_key`"),
    SnippetCheck("M248-E034-DOC-PKT-10", "`check:objc3c:m248-e033-lane-e-readiness`"),
    SnippetCheck("M248-E034-DOC-PKT-11", "`check:objc3c:m248-e034-lane-e-readiness`"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E034-RUN-01", "scripts/run_m248_e033_lane_e_readiness.py"),
    SnippetCheck("M248-E034-RUN-02", "scripts/run_m248_a013_lane_a_readiness.py"),
    SnippetCheck("M248-E034-RUN-03", "scripts/run_m248_b016_lane_b_readiness.py"),
    SnippetCheck("M248-E034-RUN-04", "scripts/run_m248_c018_lane_c_readiness.py"),
    SnippetCheck("M248-E034-RUN-05", "scripts/run_m248_d024_lane_d_readiness.py"),
    SnippetCheck(
        "M248-E034-RUN-06",
        "scripts/check_m248_e034_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard4_contract.py",
    ),
    SnippetCheck(
        "M248-E034-RUN-07",
        "tests/tooling/test_check_m248_e034_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard4_contract.py",
    ),
    SnippetCheck("M248-E034-RUN-08", "[ok] M248-E034 lane-E readiness chain completed"),
)

FORBIDDEN_READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E034-RUN-FORB-01", "scripts/run_m248_e028_lane_e_readiness.py"),
    SnippetCheck("M248-E034-RUN-FORB-02", "check:objc3c:m248-e028-lane-e-readiness"),
    SnippetCheck("M248-E034-RUN-FORB-03", "npm run "),
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
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
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
    required_snippets: tuple[SnippetCheck, ...],
    forbidden_snippets: tuple[SnippetCheck, ...] = (),
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

    for snippet in required_snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    for snippet in forbidden_snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"forbidden snippet present: {snippet.snippet}",
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

    text_contracts: tuple[tuple[Path, str, tuple[SnippetCheck, ...], tuple[SnippetCheck, ...]], ...] = (
        (args.expectations_doc, "M248-E034-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M248-E034-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (
            args.readiness_runner,
            "M248-E034-RUN-EXISTS",
            READINESS_RUNNER_SNIPPETS,
            FORBIDDEN_READINESS_RUNNER_SNIPPETS,
        ),
    )

    for path, exists_check_id, required_snippets, forbidden_snippets in text_contracts:
        doc_checks, doc_failures = check_text_contract(
            path=path,
            exists_check_id=exists_check_id,
            required_snippets=required_snippets,
            forbidden_snippets=forbidden_snippets,
        )
        checks_total += doc_checks
        failures.extend(doc_failures)

    failures.sort(key=finding_sort_key)
    summary = {
        "mode": MODE,
        "ok": len(failures) == 0,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        json.dump(summary, sys.stdout, indent=2)
        sys.stdout.write("\n")
    elif summary["ok"]:
        print(f"[ok] {MODE}: {summary['checks_passed']}/{summary['checks_total']} checks passed")

    if summary["ok"]:
        return 0
    for finding in failures:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
