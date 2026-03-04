#!/usr/bin/env python3
"""Fail-closed checker for M245-E011 portability gate/release checklist performance and quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-e011-lane-e-portability-gate-release-checklist-performance-and-quality-guardrails-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_lane_e_portability_gate_and_release_checklist_performance_and_quality_guardrails_e011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_e011_lane_e_portability_gate_and_release_checklist_performance_and_quality_guardrails_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-E011/lane_e_portability_gate_release_checklist_performance_and_quality_guardrails_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
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
        "M245-E011-DEP-E010-01",
        Path("docs/contracts/m245_lane_e_portability_gate_and_release_checklist_conformance_corpus_expansion_e010_expectations.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-E010-02",
        Path("spec/planning/compiler/m245/m245_e010_lane_e_portability_gate_and_release_checklist_conformance_corpus_expansion_packet.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-E010-03",
        Path("scripts/check_m245_e010_lane_e_portability_gate_and_release_checklist_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-E010-04",
        Path("tests/tooling/test_check_m245_e010_lane_e_portability_gate_and_release_checklist_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-A004-01",
        Path("docs/contracts/m245_frontend_behavior_parity_across_toolchains_core_feature_expansion_a004_expectations.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-A004-02",
        Path("spec/planning/compiler/m245/m245_a004_frontend_behavior_parity_across_toolchains_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-A004-03",
        Path("scripts/check_m245_a004_frontend_behavior_parity_across_toolchains_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-A004-04",
        Path("tests/tooling/test_check_m245_a004_frontend_behavior_parity_across_toolchains_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-B005-01",
        Path("docs/contracts/m245_semantic_parity_and_platform_constraints_edge_case_and_compatibility_completion_b005_expectations.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-B005-02",
        Path("spec/planning/compiler/m245/m245_b005_semantic_parity_and_platform_constraints_edge_case_and_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-B005-03",
        Path("scripts/check_m245_b005_semantic_parity_and_platform_constraints_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-B005-04",
        Path("tests/tooling/test_check_m245_b005_semantic_parity_and_platform_constraints_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-C006-01",
        Path("docs/contracts/m245_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_c006_expectations.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-C006-02",
        Path("spec/planning/compiler/m245/m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-C006-03",
        Path("scripts/check_m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-C006-04",
        Path("tests/tooling/test_check_m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-D008-01",
        Path("docs/contracts/m245_build_link_runtime_reproducibility_operations_recovery_and_determinism_hardening_d008_expectations.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-D008-02",
        Path("spec/planning/compiler/m245/m245_d008_build_link_runtime_reproducibility_operations_recovery_and_determinism_hardening_packet.md"),
    ),
    AssetCheck(
        "M245-E011-DEP-D008-03",
        Path("scripts/check_m245_d008_build_link_runtime_reproducibility_operations_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M245-E011-DEP-D008-04",
        Path("tests/tooling/test_check_m245_d008_build_link_runtime_reproducibility_operations_recovery_and_determinism_hardening_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-E011-DOC-EXP-01",
        "# M245 Lane E Portability Gate and Release Checklist Performance and Quality Guardrails Expectations (E011)",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-portability-gate-release-checklist-performance-and-quality-guardrails/m245-e011-v1`",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-03",
        "Issue `#6683` defines canonical lane-E performance and quality guardrails scope.",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-04",
        "Dependencies: `M245-E010`, `M245-A004`, `M245-B005`, `M245-C006`, `M245-D008`",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-05",
        "docs/contracts/m245_lane_e_portability_gate_and_release_checklist_conformance_corpus_expansion_e010_expectations.md",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-06",
        "scripts/check_m245_a004_frontend_behavior_parity_across_toolchains_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-07",
        "scripts/check_m245_b005_semantic_parity_and_platform_constraints_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-08",
        "scripts/check_m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-09",
        "scripts/check_m245_d008_build_link_runtime_reproducibility_operations_recovery_and_determinism_hardening_contract.py",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-10",
        "performance and quality guardrails traceability, and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-11",
        "Predecessor anchor: `M245-E010` conformance corpus expansion continuity is the mandatory baseline for E011.",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-12",
        "`scripts/check_m245_e011_lane_e_portability_gate_and_release_checklist_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-13",
        "`tests/tooling/test_check_m245_e011_lane_e_portability_gate_and_release_checklist_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck(
        "M245-E011-DOC-EXP-14",
        "`tmp/reports/m245/M245-E011/lane_e_portability_gate_release_checklist_performance_and_quality_guardrails_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-E011-DOC-PKT-01",
        "# M245-E011 Lane-E Portability Gate and Release Checklist Performance and Quality Guardrails Packet",
    ),
    SnippetCheck("M245-E011-DOC-PKT-02", "Packet: `M245-E011`"),
    SnippetCheck("M245-E011-DOC-PKT-03", "Issue: `#6683`"),
    SnippetCheck(
        "M245-E011-DOC-PKT-04",
        "Dependencies: `M245-E010`, `M245-A004`, `M245-B005`, `M245-C006`, `M245-D008`",
    ),
    SnippetCheck("M245-E011-DOC-PKT-05", "Predecessor: `M245-E010`"),
    SnippetCheck("M245-E011-DOC-PKT-06", "Theme: performance and quality guardrails"),
    SnippetCheck(
        "M245-E011-DOC-PKT-07",
        "docs/contracts/m245_lane_e_portability_gate_and_release_checklist_performance_and_quality_guardrails_e011_expectations.md",
    ),
    SnippetCheck(
        "M245-E011-DOC-PKT-08",
        "scripts/check_m245_e011_lane_e_portability_gate_and_release_checklist_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M245-E011-DOC-PKT-09",
        "tests/tooling/test_check_m245_e011_lane_e_portability_gate_and_release_checklist_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M245-E011-DOC-PKT-10",
        "spec/planning/compiler/m245/m245_e010_lane_e_portability_gate_and_release_checklist_conformance_corpus_expansion_packet.md",
    ),
    SnippetCheck(
        "M245-E011-DOC-PKT-11",
        "performance and quality guardrails traceability, and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-E011-DOC-PKT-12",
        "tmp/reports/m245/M245-E011/lane_e_portability_gate_release_checklist_performance_and_quality_guardrails_summary.json",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-E011-PKG-01", '"compile:objc3c": '),
    SnippetCheck("M245-E011-PKG-02", '"proof:objc3c": '),
    SnippetCheck("M245-E011-PKG-03", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M245-E011-PKG-04", '"test:objc3c:perf-budget": '),
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
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    failures: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            failures.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            failures.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, failures


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    failures: list[Finding] = []
    if not path.exists():
        failures.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
        return checks_total, failures
    if not path.is_file():
        failures.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, failures

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, failures


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
        (args.expectations_doc, "M245-E011-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M245-E011-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.package_json, "M245-E011-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, finding_batch = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(finding_batch)

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

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
