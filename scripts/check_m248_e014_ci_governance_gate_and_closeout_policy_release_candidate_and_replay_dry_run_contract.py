#!/usr/bin/env python3
"""Fail-closed checker for M248-E014 lane-E CI governance release-candidate replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-e014-lane-e-ci-governance-gate-closeout-policy-release-candidate-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_lane_e_ci_governance_gate_and_closeout_policy_release_candidate_and_replay_dry_run_e014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_e014_ci_governance_gate_and_closeout_policy_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_FINAL_READINESS_SURFACE = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_final_readiness_gate_core_feature_implementation_surface.h"
)
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-E014/lane_e_ci_governance_gate_closeout_policy_release_candidate_and_replay_dry_run_summary.json"
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
        "M248-E014-E013-01",
        "M248-E013",
        Path("docs/contracts/m248_lane_e_ci_governance_gate_and_closeout_policy_docs_and_operator_runbook_synchronization_e013_expectations.md"),
    ),
    AssetCheck(
        "M248-E014-E013-02",
        "M248-E013",
        Path("spec/planning/compiler/m248/m248_e013_ci_governance_gate_and_closeout_policy_docs_and_operator_runbook_synchronization_packet.md"),
    ),
    AssetCheck(
        "M248-E014-E013-03",
        "M248-E013",
        Path("scripts/check_m248_e013_ci_governance_gate_and_closeout_policy_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M248-E014-E013-04",
        "M248-E013",
        Path("tests/tooling/test_check_m248_e013_ci_governance_gate_and_closeout_policy_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck("M248-E014-E013-05", "M248-E013", Path("scripts/run_m248_e013_lane_e_readiness.py")),
    AssetCheck(
        "M248-E014-A005-01",
        "M248-A005",
        Path("docs/contracts/m248_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_a005_expectations.md"),
    ),
    AssetCheck(
        "M248-E014-A005-02",
        "M248-A005",
        Path("scripts/check_m248_a005_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M248-E014-A005-03",
        "M248-A005",
        Path("tests/tooling/test_check_m248_a005_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M248-E014-B006-01",
        "M248-B006",
        Path("docs/contracts/m248_semantic_lowering_test_architecture_edge_case_expansion_and_robustness_b006_expectations.md"),
    ),
    AssetCheck(
        "M248-E014-B006-02",
        "M248-B006",
        Path("scripts/check_m248_b006_semantic_lowering_test_architecture_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M248-E014-B006-03",
        "M248-B006",
        Path("tests/tooling/test_check_m248_b006_semantic_lowering_test_architecture_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M248-E014-C008-01",
        "M248-C008",
        Path("docs/contracts/m248_replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_c008_expectations.md"),
    ),
    AssetCheck(
        "M248-E014-C008-02",
        "M248-C008",
        Path("scripts/check_m248_c008_replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M248-E014-C008-03",
        "M248-C008",
        Path("tests/tooling/test_check_m248_c008_replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M248-E014-D010-01",
        "M248-D010",
        Path("docs/contracts/m248_runner_reliability_and_platform_operations_conformance_corpus_expansion_d010_expectations.md"),
    ),
    AssetCheck(
        "M248-E014-D010-02",
        "M248-D010",
        Path("scripts/check_m248_d010_runner_reliability_and_platform_operations_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M248-E014-D010-03",
        "M248-D010",
        Path("tests/tooling/test_check_m248_d010_runner_reliability_and_platform_operations_conformance_corpus_expansion_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E014-DOC-EXP-01",
        "# M248 Lane E CI Governance Gate and Closeout Policy Release-Candidate and Replay Dry-Run Expectations (E014)",
    ),
    SnippetCheck(
        "M248-E014-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-release-candidate-replay-dry-run/m248-e014-v1`",
    ),
    SnippetCheck(
        "M248-E014-DOC-EXP-03",
        "Dependencies: `M248-E013`, `M248-A005`, `M248-B006`, `M248-C008`, `M248-D010`",
    ),
    SnippetCheck(
        "M248-E014-DOC-EXP-04",
        "Issue `#6874` defines canonical lane-E release-candidate and replay dry-run scope.",
    ),
    SnippetCheck("M248-E014-DOC-EXP-05", "`release_candidate_replay_dry_run_consistent`"),
    SnippetCheck("M248-E014-DOC-EXP-06", "`release_candidate_replay_dry_run_ready`"),
    SnippetCheck("M248-E014-DOC-EXP-07", "`release_candidate_replay_dry_run_key`"),
    SnippetCheck("M248-E014-DOC-EXP-08", "`check:objc3c:m248-e014-lane-e-readiness`"),
    SnippetCheck("M248-E014-DOC-EXP-09", "`check:objc3c:m248-e013-lane-e-readiness`"),
    SnippetCheck("M248-E014-DOC-EXP-10", "`check:objc3c:m248-a005-lane-a-readiness`"),
    SnippetCheck("M248-E014-DOC-EXP-11", "`check:objc3c:m248-b006-lane-b-readiness`"),
    SnippetCheck("M248-E014-DOC-EXP-12", "`check:objc3c:m248-c008-lane-c-readiness`"),
    SnippetCheck("M248-E014-DOC-EXP-13", "`check:objc3c:m248-d010-lane-d-readiness`"),
    SnippetCheck(
        "M248-E014-DOC-EXP-14",
        "`tmp/reports/m248/M248-E014/lane_e_ci_governance_gate_closeout_policy_release_candidate_and_replay_dry_run_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E014-DOC-PKT-01", "Packet: `M248-E014`"),
    SnippetCheck("M248-E014-DOC-PKT-02", "Issue: `#6874`"),
    SnippetCheck(
        "M248-E014-DOC-PKT-03",
        "Dependencies: `M248-E013`, `M248-A005`, `M248-B006`, `M248-C008`, `M248-D010`",
    ),
    SnippetCheck(
        "M248-E014-DOC-PKT-04",
        "`scripts/check_m248_e014_ci_governance_gate_and_closeout_policy_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M248-E014-DOC-PKT-05",
        "`tests/tooling/test_check_m248_e014_ci_governance_gate_and_closeout_policy_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck("M248-E014-DOC-PKT-06", "`scripts/run_m248_e014_lane_e_readiness.py`"),
    SnippetCheck("M248-E014-DOC-PKT-07", "`release_candidate_replay_dry_run_consistent`"),
    SnippetCheck("M248-E014-DOC-PKT-08", "`release_candidate_replay_dry_run_ready`"),
    SnippetCheck("M248-E014-DOC-PKT-09", "`release_candidate_replay_dry_run_key`"),
    SnippetCheck("M248-E014-DOC-PKT-10", "`check:objc3c:m248-e014-lane-e-readiness`"),
    SnippetCheck("M248-E014-DOC-PKT-11", "`check:objc3c:m248-e013-lane-e-readiness`"),
    SnippetCheck("M248-E014-DOC-PKT-12", "`check:objc3c:m248-a005-lane-a-readiness`"),
    SnippetCheck("M248-E014-DOC-PKT-13", "`check:objc3c:m248-b006-lane-b-readiness`"),
    SnippetCheck("M248-E014-DOC-PKT-14", "`check:objc3c:m248-c008-lane-c-readiness`"),
    SnippetCheck("M248-E014-DOC-PKT-15", "`check:objc3c:m248-d010-lane-d-readiness`"),
)

FINAL_READINESS_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E014-SUR-01", "BuildObjc3FinalReadinessGateReleaseCandidateReplayDryRunKey("),
    SnippetCheck("M248-E014-SUR-02", "const bool lane_release_candidate_replay_dry_run_consistent ="),
    SnippetCheck("M248-E014-SUR-03", "const bool release_candidate_replay_dry_run_consistent ="),
    SnippetCheck("M248-E014-SUR-04", "const bool release_candidate_replay_dry_run_ready ="),
    SnippetCheck("M248-E014-SUR-05", "surface.release_candidate_replay_dry_run_consistent ="),
    SnippetCheck("M248-E014-SUR-06", "surface.release_candidate_replay_dry_run_ready ="),
    SnippetCheck("M248-E014-SUR-07", "surface.release_candidate_replay_dry_run_key ="),
    SnippetCheck("M248-E014-SUR-08", "final-readiness-gate-release-candidate-replay-dry-run:v1:"),
    SnippetCheck("M248-E014-SUR-09", ";release_candidate_replay_dry_run_consistent="),
    SnippetCheck("M248-E014-SUR-10", ";release_candidate_replay_dry_run_ready="),
    SnippetCheck("M248-E014-SUR-11", ";release_candidate_replay_dry_run_key_ready="),
    SnippetCheck("M248-E014-SUR-12", "surface.release_candidate_replay_dry_run_ready &&"),
    SnippetCheck(
        "M248-E014-SUR-13",
        "final readiness gate release candidate replay dry-run is inconsistent",
    ),
    SnippetCheck(
        "M248-E014-SUR-14",
        "final readiness gate release candidate replay dry-run consistency is not satisfied",
    ),
    SnippetCheck("M248-E014-SUR-15", "final readiness gate release candidate replay dry-run is not ready"),
    SnippetCheck("M248-E014-SUR-16", "final readiness gate release candidate replay dry-run key is not ready"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E014-TYP-01", "bool release_candidate_replay_dry_run_consistent = false;"),
    SnippetCheck("M248-E014-TYP-02", "bool release_candidate_replay_dry_run_ready = false;"),
    SnippetCheck("M248-E014-TYP-03", "std::string release_candidate_replay_dry_run_key;"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E014-ARC-01", "release_candidate_replay_dry_run_*"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E014-CFG-01",
        '"check:objc3c:m248-e014-ci-governance-gate-closeout-policy-release-candidate-replay-dry-run-contract"',
    ),
    SnippetCheck(
        "M248-E014-CFG-02",
        '"test:tooling:m248-e014-ci-governance-gate-closeout-policy-release-candidate-replay-dry-run-contract"',
    ),
    SnippetCheck("M248-E014-CFG-03", '"check:objc3c:m248-e014-lane-e-readiness"'),
    SnippetCheck("M248-E014-CFG-04", "check:objc3c:m248-e013-lane-e-readiness"),
    SnippetCheck("M248-E014-CFG-05", "check:objc3c:m248-a005-lane-a-readiness"),
    SnippetCheck("M248-E014-CFG-06", "check:objc3c:m248-b006-lane-b-readiness"),
    SnippetCheck("M248-E014-CFG-07", "check:objc3c:m248-c008-lane-c-readiness"),
    SnippetCheck("M248-E014-CFG-08", "check:objc3c:m248-d010-lane-d-readiness"),
)

FORBIDDEN_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E014-FORB-01", "surface.release_candidate_replay_dry_run_ready = true;"),
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
    parser.add_argument("--final-readiness-surface", type=Path, default=DEFAULT_FINAL_READINESS_SURFACE)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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
        (args.expectations_doc, "M248-E014-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M248-E014-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.final_readiness_surface, "M248-E014-SUR-EXISTS", FINAL_READINESS_SURFACE_SNIPPETS, FORBIDDEN_SURFACE_SNIPPETS),
        (args.frontend_types, "M248-E014-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS, ()),
        (args.architecture_doc, "M248-E014-ARC-EXISTS", ARCHITECTURE_SNIPPETS, ()),
        (args.package_json, "M248-E014-CFG-EXISTS", PACKAGE_SNIPPETS, ()),
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
