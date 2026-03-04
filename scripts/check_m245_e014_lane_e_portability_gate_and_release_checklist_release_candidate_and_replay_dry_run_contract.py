#!/usr/bin/env python3
"""Fail-closed checker for M245-E014 portability gate/release checklist release-candidate and replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-e014-lane-e-portability-gate-release-checklist-release-candidate-and-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_lane_e_portability_gate_and_release_checklist_release_candidate_and_replay_dry_run_e014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_e014_lane_e_portability_gate_and_release_checklist_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-E014/lane_e_portability_gate_release_checklist_release_candidate_replay_dry_run_summary.json"
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
        "M245-E014-DEP-E013-01",
        "M245-E013",
        Path("docs/contracts/m245_lane_e_portability_gate_and_release_checklist_docs_and_operator_runbook_synchronization_e013_expectations.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-E013-02",
        "M245-E013",
        Path("spec/planning/compiler/m245/m245_e013_lane_e_portability_gate_and_release_checklist_docs_and_operator_runbook_synchronization_packet.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-E013-03",
        "M245-E013",
        Path("scripts/check_m245_e013_lane_e_portability_gate_and_release_checklist_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-E013-04",
        "M245-E013",
        Path("tests/tooling/test_check_m245_e013_lane_e_portability_gate_and_release_checklist_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-A005-01",
        "M245-A005",
        Path("docs/contracts/m245_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_a005_expectations.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-A005-02",
        "M245-A005",
        Path("spec/planning/compiler/m245/m245_a005_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-A005-03",
        "M245-A005",
        Path("scripts/check_m245_a005_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-A005-04",
        "M245-A005",
        Path("tests/tooling/test_check_m245_a005_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-B006-01",
        "M245-B006",
        Path("docs/contracts/m245_semantic_parity_and_platform_constraints_edge_case_expansion_and_robustness_b006_expectations.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-B006-02",
        "M245-B006",
        Path("spec/planning/compiler/m245/m245_b006_semantic_parity_and_platform_constraints_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-B006-03",
        "M245-B006",
        Path("scripts/check_m245_b006_semantic_parity_and_platform_constraints_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-B006-04",
        "M245-B006",
        Path("tests/tooling/test_check_m245_b006_semantic_parity_and_platform_constraints_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-C008-01",
        "M245-C008",
        Path("docs/contracts/m245_lowering_ir_portability_contracts_recovery_and_determinism_hardening_c008_expectations.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-C008-02",
        "M245-C008",
        Path("spec/planning/compiler/m245/m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_packet.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-C008-03",
        "M245-C008",
        Path("scripts/check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-C008-04",
        "M245-C008",
        Path("tests/tooling/test_check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-D010-01",
        "M245-D010",
        Path("docs/contracts/m245_build_link_runtime_reproducibility_operations_conformance_corpus_expansion_d010_expectations.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-D010-02",
        "M245-D010",
        Path("spec/planning/compiler/m245/m245_d010_build_link_runtime_reproducibility_operations_conformance_corpus_expansion_packet.md"),
    ),
    AssetCheck(
        "M245-E014-DEP-D010-03",
        "M245-D010",
        Path("scripts/check_m245_d010_build_link_runtime_reproducibility_operations_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M245-E014-DEP-D010-04",
        "M245-D010",
        Path("tests/tooling/test_check_m245_d010_build_link_runtime_reproducibility_operations_conformance_corpus_expansion_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-E014-DOC-EXP-01",
        "# M245 Lane E Portability Gate and Release Checklist Release-Candidate and Replay Dry-Run Expectations (E014)",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-portability-gate-release-checklist-release-candidate-replay-dry-run/m245-e014-v1`",
    ),
    SnippetCheck("M245-E014-DOC-EXP-03", "- Issue: `#5032`"),
    SnippetCheck(
        "M245-E014-DOC-EXP-04",
        "Dependencies: `M245-E013`, `M245-A005`, `M245-B006`, `M245-C008`, `M245-D010`",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-05",
        "Predecessor anchor: `M245-E013` docs and operator runbook synchronization continuity is the mandatory baseline for E014.",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-06",
        "docs/contracts/m245_lane_e_portability_gate_and_release_checklist_docs_and_operator_runbook_synchronization_e013_expectations.md",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-07",
        "scripts/check_m245_a005_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-08",
        "scripts/check_m245_b006_semantic_parity_and_platform_constraints_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-09",
        "scripts/check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-10",
        "scripts/check_m245_d010_build_link_runtime_reproducibility_operations_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-11",
        "release-candidate and replay dry-run traceability, and milestone optimization inputs as mandatory scope inputs.",
    ),
    SnippetCheck("M245-E014-DOC-EXP-12", "`check:objc3c:m245-e013-lane-e-readiness`"),
    SnippetCheck("M245-E014-DOC-EXP-13", "`check:objc3c:m245-a005-lane-a-readiness`"),
    SnippetCheck("M245-E014-DOC-EXP-14", "`check:objc3c:m245-b006-lane-b-readiness`"),
    SnippetCheck("M245-E014-DOC-EXP-15", "`check:objc3c:m245-c008-lane-c-readiness`"),
    SnippetCheck("M245-E014-DOC-EXP-16", "`check:objc3c:m245-d010-lane-d-readiness`"),
    SnippetCheck(
        "M245-E014-DOC-EXP-17",
        "`scripts/check_m245_e014_lane_e_portability_gate_and_release_checklist_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-18",
        "`tests/tooling/test_check_m245_e014_lane_e_portability_gate_and_release_checklist_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M245-E014-DOC-EXP-19",
        "`tmp/reports/m245/M245-E014/lane_e_portability_gate_release_checklist_release_candidate_replay_dry_run_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-E014-DOC-PKT-01",
        "# M245-E014 Lane-E Portability Gate and Release Checklist Release-Candidate and Replay Dry-Run Packet",
    ),
    SnippetCheck("M245-E014-DOC-PKT-02", "Packet: `M245-E014`"),
    SnippetCheck("M245-E014-DOC-PKT-03", "Issue: `#5032`"),
    SnippetCheck(
        "M245-E014-DOC-PKT-04",
        "Dependencies: `M245-E013`, `M245-A005`, `M245-B006`, `M245-C008`, `M245-D010`",
    ),
    SnippetCheck("M245-E014-DOC-PKT-05", "Predecessor: `M245-E013`"),
    SnippetCheck("M245-E014-DOC-PKT-06", "Theme: release-candidate and replay dry-run"),
    SnippetCheck(
        "M245-E014-DOC-PKT-07",
        "docs/contracts/m245_lane_e_portability_gate_and_release_checklist_release_candidate_and_replay_dry_run_e014_expectations.md",
    ),
    SnippetCheck(
        "M245-E014-DOC-PKT-08",
        "scripts/check_m245_e014_lane_e_portability_gate_and_release_checklist_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M245-E014-DOC-PKT-09",
        "tests/tooling/test_check_m245_e014_lane_e_portability_gate_and_release_checklist_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M245-E014-DOC-PKT-10",
        "spec/planning/compiler/m245/m245_e013_lane_e_portability_gate_and_release_checklist_docs_and_operator_runbook_synchronization_packet.md",
    ),
    SnippetCheck("M245-E014-DOC-PKT-11", "check:objc3c:m245-e013-lane-e-readiness"),
    SnippetCheck("M245-E014-DOC-PKT-12", "check:objc3c:m245-a005-lane-a-readiness"),
    SnippetCheck("M245-E014-DOC-PKT-13", "check:objc3c:m245-b006-lane-b-readiness"),
    SnippetCheck("M245-E014-DOC-PKT-14", "check:objc3c:m245-c008-lane-c-readiness"),
    SnippetCheck("M245-E014-DOC-PKT-15", "check:objc3c:m245-d010-lane-d-readiness"),
    SnippetCheck(
        "M245-E014-DOC-PKT-16",
        "tmp/reports/m245/M245-E014/lane_e_portability_gate_release_checklist_release_candidate_replay_dry_run_summary.json",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-E014-PKG-01", '"check:objc3c:m245-e013-lane-e-readiness": '),
    SnippetCheck("M245-E014-PKG-02", '"check:objc3c:m245-a005-lane-a-readiness": '),
    SnippetCheck("M245-E014-PKG-03", '"check:objc3c:m245-b006-lane-b-readiness": '),
    SnippetCheck("M245-E014-PKG-04", '"check:objc3c:m245-c008-lane-c-readiness": '),
    SnippetCheck("M245-E014-PKG-05", '"check:objc3c:m245-d010-lane-d-readiness": '),
    SnippetCheck("M245-E014-PKG-06", '"compile:objc3c": '),
    SnippetCheck("M245-E014-PKG-07", '"proof:objc3c": '),
    SnippetCheck("M245-E014-PKG-08", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M245-E014-PKG-09", '"test:objc3c:perf-budget": '),
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
        (args.expectations_doc, "M245-E014-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M245-E014-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.package_json, "M245-E014-PKG-EXISTS", PACKAGE_SNIPPETS),
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

