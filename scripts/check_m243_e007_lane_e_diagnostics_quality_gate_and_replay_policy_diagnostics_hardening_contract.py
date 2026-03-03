#!/usr/bin/env python3
"""Fail-closed checker for M243-E007 lane-E diagnostics gate/replay-policy diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m243_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_e007_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_e007_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m243/M243-E007/lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract_summary.json"
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
        "M243-E007-DEP-E006-01",
        Path("docs/contracts/m243_lane_e_diagnostics_quality_gate_and_replay_policy_edge_case_expansion_and_robustness_e006_expectations.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-E006-02",
        Path("spec/planning/compiler/m243/m243_e006_lane_e_diagnostics_quality_gate_and_replay_policy_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-E006-03",
        Path("scripts/check_m243_e006_lane_e_diagnostics_quality_gate_and_replay_policy_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-E006-04",
        Path("tests/tooling/test_check_m243_e006_lane_e_diagnostics_quality_gate_and_replay_policy_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-A003-01",
        Path("docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a003_expectations.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-A003-02",
        Path("spec/planning/compiler/m243/m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_packet.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-A003-03",
        Path("scripts/check_m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-A003-04",
        Path("tests/tooling/test_check_m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-B003-01",
        Path("docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_b003_expectations.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-B003-02",
        Path("spec/planning/compiler/m243/m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_packet.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-B003-03",
        Path("scripts/check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-B003-04",
        Path("tests/tooling/test_check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-C004-01",
        Path("docs/contracts/m243_lowering_runtime_diagnostics_surfacing_core_feature_expansion_c004_expectations.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-C004-02",
        Path("spec/planning/compiler/m243/m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-C004-03",
        Path("scripts/check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-C004-04",
        Path("tests/tooling/test_check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-D005-01",
        Path("docs/contracts/m243_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_d005_expectations.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-D005-02",
        Path("spec/planning/compiler/m243/m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M243-E007-DEP-D005-03",
        Path("scripts/check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M243-E007-DEP-D005-04",
        Path("tests/tooling/test_check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E007-DOC-EXP-01",
        "# M243 Lane-E Diagnostics Quality Gate and Replay Policy Diagnostics Hardening Expectations (E007)",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening/m243-e007-v1`",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-03",
        "Dependencies: `M243-E006`, `M243-A003`, `M243-B003`, `M243-C004`, `M243-D005`",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-04",
        "Readiness command chain enforces E006 and lane A/B/C/D dependency",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-05",
        "prerequisites before E007 evidence checks run.",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-06",
        "`check:objc3c:m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract`",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-07",
        "`test:tooling:m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract`",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-08",
        "`check:objc3c:m243-e007-lane-e-readiness`",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-09",
        "`python scripts/check_m243_e007_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract.py --emit-json`",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-10",
        "`python -m pytest tests/tooling/test_check_m243_e007_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract.py -q`",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-11",
        "`tmp/reports/m243/M243-E007/lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract_summary.json`",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-12",
        "Code/spec anchors and milestone optimization improvements are mandatory",
    ),
    SnippetCheck(
        "M243-E007-DOC-EXP-13",
        "scope inputs.",
    ),
    SnippetCheck("M243-E007-DOC-EXP-14", "`compile:objc3c`"),
    SnippetCheck("M243-E007-DOC-EXP-15", "`proof:objc3c`"),
    SnippetCheck("M243-E007-DOC-EXP-16", "`test:objc3c:perf-budget`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E007-DOC-PKT-01",
        "# M243-E007 Lane-E Diagnostics Quality Gate and Replay Policy Diagnostics Hardening Packet",
    ),
    SnippetCheck("M243-E007-DOC-PKT-02", "Packet: `M243-E007`"),
    SnippetCheck(
        "M243-E007-DOC-PKT-03",
        "Dependencies: `M243-E006`, `M243-A003`, `M243-B003`, `M243-C004`, `M243-D005`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-04",
        "`scripts/check_m243_e007_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-05",
        "`tests/tooling/test_check_m243_e007_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-06",
        "`check:objc3c:m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-07",
        "`test:tooling:m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-08",
        "`check:objc3c:m243-e007-lane-e-readiness`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-09",
        "`scripts/check_m243_e006_lane_e_diagnostics_quality_gate_and_replay_policy_edge_case_expansion_and_robustness_contract.py`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-10",
        "`tests/tooling/test_check_m243_e006_lane_e_diagnostics_quality_gate_and_replay_policy_edge_case_expansion_and_robustness_contract.py`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-11",
        "`tmp/reports/m243/M243-E007/lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract_summary.json`",
    ),
    SnippetCheck(
        "M243-E007-DOC-PKT-12",
        "Milestone Optimization Improvements (Mandatory Scope Inputs)",
    ),
    SnippetCheck("M243-E007-DOC-PKT-13", "`compile:objc3c`"),
    SnippetCheck("M243-E007-DOC-PKT-14", "`proof:objc3c`"),
    SnippetCheck("M243-E007-DOC-PKT-15", "`test:objc3c:perf-budget`"),
    SnippetCheck(
        "M243-E007-DOC-PKT-16",
        "`python scripts/check_m243_e007_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract.py --emit-json`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E007-ARCH-01",
        "M243 lane-E E007 diagnostics quality gate/replay policy diagnostics hardening anchors dependency references",
    ),
    SnippetCheck(
        "M243-E007-ARCH-02",
        "(`M243-E006`, `M243-A003`, `M243-B003`, `M243-C004`, and `M243-D005`)",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E007-SPC-01",
        "diagnostics quality gate and replay policy diagnostics hardening wiring shall preserve explicit",
    ),
    SnippetCheck(
        "M243-E007-SPC-02",
        "lane-E dependency anchors (`M243-E006`, `M243-A003`, `M243-B003`, `M243-C004`, and",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E007-META-01",
        "deterministic lane-E diagnostics quality gate and replay policy diagnostics hardening dependency anchors for",
    ),
    SnippetCheck(
        "M243-E007-META-02",
        "`M243-E006`, `M243-A003`, `M243-B003`, `M243-C004`, and `M243-D005`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E007-PKG-01",
        '"check:objc3c:m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract": '
        '"python scripts/check_m243_e007_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract.py"',
    ),
    SnippetCheck(
        "M243-E007-PKG-02",
        '"test:tooling:m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract": '
        '"python -m pytest tests/tooling/test_check_m243_e007_lane_e_diagnostics_quality_gate_and_replay_policy_diagnostics_hardening_contract.py -q"',
    ),
    SnippetCheck(
        "M243-E007-PKG-03",
        '"check:objc3c:m243-e007-lane-e-readiness": '
        '"npm run check:objc3c:m243-e006-lane-e-readiness '
        "&& npm run check:objc3c:m243-a003-lane-a-readiness "
        "&& npm run check:objc3c:m243-b003-lane-b-readiness "
        "&& npm run check:objc3c:m243-c004-lane-c-readiness "
        "&& npm run check:objc3c:m243-d005-lane-d-readiness "
        "&& npm run check:objc3c:m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract "
        '&& npm run test:tooling:m243-e007-lane-e-diagnostics-quality-gate-replay-policy-diagnostics-hardening-contract"',
    ),
    SnippetCheck("M243-E007-PKG-04", '"test:objc3c:diagnostics-replay-proof": '),
    SnippetCheck("M243-E007-PKG-05", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M243-E007-PKG-06", '"compile:objc3c": '),
    SnippetCheck("M243-E007-PKG-07", '"proof:objc3c": '),
    SnippetCheck("M243-E007-PKG-08", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
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
        findings.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M243-E007-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M243-E007-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M243-E007-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M243-E007-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M243-E007-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M243-E007-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

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
        print(canonical_json(summary_payload), end="")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
