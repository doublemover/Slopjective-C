#!/usr/bin/env python3
"""Fail-closed checker for M248-E029 lane-E CI governance advanced diagnostics workpack shard3."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-e029-lane-e-ci-governance-gate-closeout-policy-advanced-diagnostics-workpack-shard3-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_lane_e_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_e029_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m248_e029_lane_e_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-E029/lane_e_ci_governance_gate_closeout_policy_advanced_diagnostics_workpack_shard3_summary.json"
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
        "M248-E029-A009-01",
        "M248-A011",
        Path("docs/contracts/m248_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_a011_expectations.md"),
    ),
    AssetCheck(
        "M248-E029-A009-02",
        "M248-A011",
        Path("spec/planning/compiler/m248/m248_a011_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_packet.md"),
    ),
    AssetCheck(
        "M248-E029-A009-03",
        "M248-A011",
        Path("scripts/check_m248_a011_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M248-E029-A009-04",
        "M248-A011",
        Path("tests/tooling/test_check_m248_a011_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M248-E029-B011-01",
        "M248-B011",
        Path("docs/contracts/m248_semantic_lowering_test_architecture_performance_and_quality_guardrails_b011_expectations.md"),
    ),
    AssetCheck(
        "M248-E029-B011-02",
        "M248-B011",
        Path("spec/planning/compiler/m248/m248_b011_semantic_lowering_test_architecture_performance_and_quality_guardrails_packet.md"),
    ),
    AssetCheck(
        "M248-E029-B011-03",
        "M248-B011",
        Path("scripts/check_m248_b011_semantic_lowering_test_architecture_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M248-E029-B011-04",
        "M248-B011",
        Path("tests/tooling/test_check_m248_b011_semantic_lowering_test_architecture_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M248-E029-C012-01",
        "M248-C012",
        Path("docs/contracts/m248_replay_harness_and_artifact_contracts_cross_lane_integration_sync_c012_expectations.md"),
    ),
    AssetCheck(
        "M248-E029-C012-02",
        "M248-C012",
        Path("spec/planning/compiler/m248/m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_packet.md"),
    ),
    AssetCheck(
        "M248-E029-C012-03",
        "M248-C012",
        Path("scripts/check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E029-C012-04",
        "M248-C012",
        Path("tests/tooling/test_check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M248-E029-D019-01",
        "M248-D021",
        Path("docs/contracts/m248_runner_reliability_and_platform_operations_advanced_core_workpack_shard2_d021_expectations.md"),
    ),
    AssetCheck(
        "M248-E029-D019-02",
        "M248-D021",
        Path("spec/planning/compiler/m248/m248_d021_runner_reliability_and_platform_operations_advanced_core_workpack_shard2_packet.md"),
    ),
    AssetCheck(
        "M248-E029-D019-03",
        "M248-D021",
        Path("scripts/check_m248_d021_runner_reliability_and_platform_operations_advanced_core_workpack_shard2_contract.py"),
    ),
    AssetCheck(
        "M248-E029-D019-04",
        "M248-D021",
        Path("tests/tooling/test_check_m248_d021_runner_reliability_and_platform_operations_advanced_core_workpack_shard2_contract.py"),
    ),
    AssetCheck(
        "M248-E029-D019-05",
        "M248-D021",
        Path("scripts/run_m248_d021_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E029-DOC-EXP-01",
        "# M248 Lane E CI Governance Gate and Closeout Policy Advanced Diagnostics Workpack (Shard 3) Expectations (E029)",
    ),
    SnippetCheck(
        "M248-E029-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-advanced-diagnostics-workpack-shard3/m248-e029-v1`",
    ),
    SnippetCheck(
        "M248-E029-DOC-EXP-03",
        "Dependencies: `M248-E028`, `M248-A011`, `M248-B011`, `M248-C012`, `M248-D021`",
    ),
    SnippetCheck("M248-E029-DOC-EXP-04", "- Issue: `#6889`"),
    SnippetCheck("M248-E029-DOC-EXP-05", "`check:objc3c:m248-e028-lane-e-readiness`"),
    SnippetCheck("M248-E029-DOC-EXP-06", "`check:objc3c:m248-a011-lane-a-readiness`"),
    SnippetCheck("M248-E029-DOC-EXP-07", "`check:objc3c:m248-b011-lane-b-readiness`"),
    SnippetCheck("M248-E029-DOC-EXP-08", "`check:objc3c:m248-c012-lane-c-readiness`"),
    SnippetCheck("M248-E029-DOC-EXP-09", "`check:objc3c:m248-d021-lane-d-readiness`"),
    SnippetCheck(
        "M248-E029-DOC-EXP-10",
        "`scripts/check_m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_contract.py`",
    ),
    SnippetCheck(
        "M248-E029-DOC-EXP-11",
        "`tests/tooling/test_check_m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_contract.py`",
    ),
    SnippetCheck(
        "M248-E029-DOC-EXP-12",
        "`tmp/reports/m248/M248-E029/lane_e_ci_governance_gate_closeout_policy_advanced_diagnostics_workpack_shard3_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E029-DOC-PKT-01",
        "# M248-E029 CI Governance Gate and Closeout Policy Advanced Diagnostics Workpack (Shard 3) Packet",
    ),
    SnippetCheck("M248-E029-DOC-PKT-02", "Packet: `M248-E029`"),
    SnippetCheck("M248-E029-DOC-PKT-03", "Issue: `#6889`"),
    SnippetCheck(
        "M248-E029-DOC-PKT-04",
        "Dependencies: `M248-E028`, `M248-A011`, `M248-B011`, `M248-C012`, `M248-D021`",
    ),
    SnippetCheck(
        "M248-E029-DOC-PKT-05",
        "scripts/check_m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_contract.py",
    ),
    SnippetCheck(
        "M248-E029-DOC-PKT-06",
        "tests/tooling/test_check_m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_contract.py",
    ),
    SnippetCheck(
        "M248-E029-DOC-PKT-07",
        "scripts/run_m248_e029_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M248-E029-DOC-PKT-08",
        "Chains through `check:objc3c:m248-e028-lane-e-readiness` before E029 checks.",
    ),
    SnippetCheck(
        "M248-E029-DOC-PKT-09",
        "including advanced diagnostics improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M248-E029-DOC-PKT-10",
        "tmp/reports/m248/M248-E029/lane_e_ci_governance_gate_closeout_policy_advanced_diagnostics_workpack_shard3_summary.json",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E029-CFG-01",
        '"check:objc3c:m248-e029-ci-governance-gate-closeout-policy-advanced-diagnostics-workpack-shard3-contract": "python scripts/check_m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_contract.py"',
    ),
    SnippetCheck(
        "M248-E029-CFG-02",
        '"test:tooling:m248-e029-ci-governance-gate-closeout-policy-advanced-diagnostics-workpack-shard3-contract": "python -m pytest tests/tooling/test_check_m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_contract.py -q"',
    ),
    SnippetCheck(
        "M248-E029-CFG-03",
        '"check:objc3c:m248-e029-lane-e-readiness": "python scripts/run_m248_e029_lane_e_readiness.py"',
    ),
    SnippetCheck("M248-E029-CFG-04", '"check:objc3c:m248-e028-lane-e-readiness": '),
    SnippetCheck("M248-E029-CFG-05", '"check:objc3c:m248-a011-lane-a-readiness": '),
    SnippetCheck("M248-E029-CFG-06", '"check:objc3c:m248-b011-lane-b-readiness": '),
    SnippetCheck("M248-E029-CFG-07", '"check:objc3c:m248-c012-lane-c-readiness": '),
    SnippetCheck("M248-E029-CFG-08", '"check:objc3c:m248-d021-lane-d-readiness": '),
    SnippetCheck("M248-E029-CFG-09", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M248-E029-CFG-10", '"test:objc3c:parser-ast-extraction": '),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E029-RUN-01",
        '"""Run M248-E029 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M248-E029-RUN-02", "check:objc3c:m248-e028-lane-e-readiness"),
    SnippetCheck("M248-E029-RUN-03", "check:objc3c:m248-a011-lane-a-readiness"),
    SnippetCheck("M248-E029-RUN-04", "check:objc3c:m248-b011-lane-b-readiness"),
    SnippetCheck("M248-E029-RUN-05", "check:objc3c:m248-c012-lane-c-readiness"),
    SnippetCheck("M248-E029-RUN-06", "check:objc3c:m248-d021-lane-d-readiness"),
    SnippetCheck(
        "M248-E029-RUN-07",
        "scripts/check_m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_contract.py",
    ),
    SnippetCheck(
        "M248-E029-RUN-08",
        "tests/tooling/test_check_m248_e029_ci_governance_gate_and_closeout_policy_advanced_diagnostics_workpack_shard3_contract.py",
    ),
    SnippetCheck("M248-E029-RUN-09", "[ok] M248-E029 lane-E readiness chain completed"),
)

FORBIDDEN_READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E029-RUN-FORB-01", "check:objc3c:m248-e021-lane-e-readiness"),
    SnippetCheck("M248-E029-RUN-FORB-02", "scripts/run_m248_e021_lane_e_readiness.py"),
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
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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
        (args.expectations_doc, "M248-E029-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M248-E029-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.package_json, "M248-E029-CFG-EXISTS", PACKAGE_SNIPPETS, ()),
        (
            args.readiness_runner,
            "M248-E029-RUN-EXISTS",
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
