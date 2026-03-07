#!/usr/bin/env python3
"""Fail-closed checker for M235-E016 qualifier/generic conformance gate integration closeout and gate sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m235-e016-qualifier-generic-conformance-gate-integration-closeout-and-gate-sign-off-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m235_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_e016_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m235"
    / "m235_e016_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_packet.md"
)
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m235/M235-E016/local_check_summary.json")


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
        "M235-E016-DEP-E002-01",
        Path("docs/contracts/m235_qualifier_generic_conformance_gate_modular_split_scaffolding_e002_expectations.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-E002-02",
        Path("spec/planning/compiler/m235/m235_e002_qualifier_generic_conformance_gate_modular_split_scaffolding_packet.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-E002-03",
        Path("scripts/check_m235_e002_qualifier_generic_conformance_gate_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-E002-04",
        Path("tests/tooling/test_check_m235_e002_qualifier_generic_conformance_gate_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-A003-01",
        Path("docs/contracts/m235_qualifier_and_generic_grammar_normalization_core_feature_implementation_a003_expectations.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-A003-02",
        Path("spec/planning/compiler/m235/m235_a003_qualifier_and_generic_grammar_normalization_core_feature_implementation_packet.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-A003-03",
        Path("scripts/check_m235_a003_qualifier_and_generic_grammar_normalization_core_feature_implementation_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-A003-04",
        Path("tests/tooling/test_check_m235_a003_qualifier_and_generic_grammar_normalization_core_feature_implementation_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-B006-01",
        Path("docs/contracts/m235_qualifier_and_generic_semantic_inference_edge_case_expansion_and_robustness_b006_expectations.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-B006-02",
        Path("spec/planning/compiler/m235/m235_b006_qualifier_and_generic_semantic_inference_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-B006-03",
        Path("scripts/check_m235_b006_qualifier_and_generic_semantic_inference_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-B006-04",
        Path("tests/tooling/test_check_m235_b006_qualifier_and_generic_semantic_inference_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-C004-01",
        Path("docs/contracts/m235_qualified_type_lowering_and_abi_representation_core_feature_expansion_c004_expectations.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-C004-02",
        Path("spec/planning/compiler/m235/m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-C004-03",
        Path("scripts/check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-C004-04",
        Path("tests/tooling/test_check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-D002-01",
        Path("docs/contracts/m235_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_d002_expectations.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-D002-02",
        Path("spec/planning/compiler/m235/m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_packet.md"),
    ),
    AssetCheck(
        "M235-E016-DEP-D002-03",
        Path("scripts/check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M235-E016-DEP-D002-04",
        Path("tests/tooling/test_check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-E016-DOC-EXP-01",
        "# M235 Qualifier Generic Conformance Gate Integration Closeout and Gate Sign-off Expectations (E003)",
    ),
    SnippetCheck(
        "M235-E016-DOC-EXP-02",
        "Contract ID: `objc3c-qualifier-generic-conformance-gate-core-feature-implementation/m235-e003-v1`",
    ),
    SnippetCheck("M235-E016-DOC-EXP-03", "Issue: `#5855`"),
    SnippetCheck(
        "M235-E016-DOC-EXP-04",
        "Dependencies: `M235-E015`, `M235-A003`, `M235-B006`, `M235-C004`, `M235-D002`",
    ),
    SnippetCheck(
        "M235-E016-DOC-EXP-05",
        "dependency carry-forward from E002",
    ),
    SnippetCheck(
        "M235-E016-DOC-EXP-06",
        "Issue `#5855` governs lane-E core feature implementation scope and dependency continuity from E002.",
    ),
    SnippetCheck("M235-E016-DOC-EXP-07", "| `M235-E015` |"),
    SnippetCheck("M235-E016-DOC-EXP-08", "| `M235-A003` |"),
    SnippetCheck("M235-E016-DOC-EXP-09", "| `M235-B006` |"),
    SnippetCheck("M235-E016-DOC-EXP-10", "| `M235-C004` |"),
    SnippetCheck("M235-E016-DOC-EXP-11", "| `M235-D002` |"),
    SnippetCheck(
        "M235-E016-DOC-EXP-12",
        "scripts/check_m235_e016_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_contract.py",
    ),
    SnippetCheck(
        "M235-E016-DOC-EXP-13",
        "tests/tooling/test_check_m235_e016_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_contract.py",
    ),
    SnippetCheck(
        "M235-E016-DOC-EXP-14",
        "tmp/reports/m235/M235-E016/local_check_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-E016-DOC-PKT-01",
        "# M235-E016 Qualifier Generic Conformance Gate Integration Closeout and Gate Sign-off Packet",
    ),
    SnippetCheck("M235-E016-DOC-PKT-02", "Packet: `M235-E016`"),
    SnippetCheck("M235-E016-DOC-PKT-03", "Issue: `#5855`"),
    SnippetCheck(
        "M235-E016-DOC-PKT-04",
        "Dependencies: `M235-E015`, `M235-A003`, `M235-B006`, `M235-C004`, `M235-D002`",
    ),
    SnippetCheck(
        "M235-E016-DOC-PKT-05",
        "Dependency continuity from E002 remains fail-closed through these anchors:",
    ),
    SnippetCheck(
        "M235-E016-DOC-PKT-06",
        "docs/contracts/m235_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_e016_expectations.md",
    ),
    SnippetCheck(
        "M235-E016-DOC-PKT-07",
        "scripts/check_m235_e016_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_contract.py",
    ),
    SnippetCheck(
        "M235-E016-DOC-PKT-08",
        "tests/tooling/test_check_m235_e016_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_contract.py",
    ),
    SnippetCheck(
        "M235-E016-DOC-PKT-09",
        "python scripts/check_m235_e016_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_contract.py --summary-out tmp/reports/m235/M235-E016/local_check_summary.json",
    ),
    SnippetCheck(
        "M235-E016-DOC-PKT-10",
        "python -m pytest tests/tooling/test_check_m235_e016_qualifier_generic_conformance_gate_integration_closeout_and_gate_sign_off_contract.py -q",
    ),
    SnippetCheck(
        "M235-E016-DOC-PKT-11",
        "tmp/reports/m235/M235-E016/local_check_summary.json",
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
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
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
        (args.expectations_doc, "M235-E016-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M235-E016-DOC-PKT-EXISTS", PACKET_SNIPPETS),
    ):
        check_count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += check_count
        failures.extend(findings)

    failures.sort(key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
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













