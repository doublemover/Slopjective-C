#!/usr/bin/env python3
"""Fail-closed checker for M247-E005 performance SLO edge-case and compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-e005-performance-slo-gate-reporting-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_edge_case_and_compatibility_completion_e005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e005_performance_slo_gate_and_reporting_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_E004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_core_feature_expansion_e004_expectations.md"
)
DEFAULT_D004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_core_feature_expansion_d004_expectations.md"
)
DEFAULT_RUNNER_SCRIPT = ROOT / "scripts" / "run_m247_e005_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-E005/performance_slo_gate_and_reporting_edge_case_compatibility_completion_contract_summary.json"
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
        "M247-E005-DEP-E004-01",
        Path("docs/contracts/m247_lane_e_performance_slo_gate_and_reporting_core_feature_expansion_e004_expectations.md"),
    ),
    AssetCheck(
        "M247-E005-DEP-E004-02",
        Path("spec/planning/compiler/m247/m247_e004_performance_slo_gate_and_reporting_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M247-E005-DEP-E004-03",
        Path("scripts/check_m247_e004_performance_slo_gate_and_reporting_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M247-E005-DEP-E004-04",
        Path("tests/tooling/test_check_m247_e004_performance_slo_gate_and_reporting_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M247-E005-DEP-E004-05",
        Path("scripts/run_m247_e004_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M247-E005-DEP-D004-01",
        Path("docs/contracts/m247_runtime_link_build_throughput_optimization_core_feature_expansion_d004_expectations.md"),
    ),
    AssetCheck(
        "M247-E005-DEP-D004-02",
        Path("spec/planning/compiler/m247/m247_d004_runtime_link_build_throughput_optimization_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M247-E005-DEP-D004-03",
        Path("scripts/check_m247_d004_runtime_link_build_throughput_optimization_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M247-E005-DEP-D004-04",
        Path("tests/tooling/test_check_m247_d004_runtime_link_build_throughput_optimization_core_feature_expansion_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E005-DOC-EXP-01",
        "# M247 Lane E Performance SLO Gate and Reporting Edge-Case and Compatibility Completion Expectations (E005)",
    ),
    SnippetCheck(
        "M247-E005-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-edge-case-and-compatibility-completion-contract/m247-e005-v1`",
    ),
    SnippetCheck("M247-E005-DOC-EXP-03", "`M247-E004`"),
    SnippetCheck("M247-E005-DOC-EXP-04", "`M247-A005`"),
    SnippetCheck("M247-E005-DOC-EXP-05", "`M247-B006`"),
    SnippetCheck(
        "M247-E005-DOC-EXP-06",
        "| `M247-C005` | Dependency token `M247-C005` is mandatory as pending seeded lane-C edge-case and compatibility completion assets. |",
    ),
    SnippetCheck("M247-E005-DOC-EXP-07", "`M247-D004`"),
    SnippetCheck("M247-E005-DOC-EXP-08", "`native/objc3c/src/ARCHITECTURE.md`"),
    SnippetCheck("M247-E005-DOC-EXP-09", "`spec/LOWERING_AND_RUNTIME_CONTRACTS.md`"),
    SnippetCheck("M247-E005-DOC-EXP-10", "`spec/MODULE_METADATA_AND_ABI_TABLES.md`"),
    SnippetCheck(
        "M247-E005-DOC-EXP-11",
        "`scripts/run_m247_e005_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M247-E005-DOC-EXP-12",
        "`check:objc3c:m247-e005-lane-e-readiness`",
    ),
    SnippetCheck(
        "M247-E005-DOC-EXP-13",
        "`tmp/reports/m247/M247-E005/performance_slo_gate_and_reporting_edge_case_compatibility_completion_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E005-DOC-PKT-01",
        "# M247-E005 Performance SLO Gate and Reporting Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M247-E005-DOC-PKT-02", "Packet: `M247-E005`"),
    SnippetCheck("M247-E005-DOC-PKT-03", "Issue: `#6776`"),
    SnippetCheck(
        "M247-E005-DOC-PKT-04",
        "Dependencies: `M247-E004`, `M247-A005`, `M247-B006`, `M247-C005`, `M247-D004`",
    ),
    SnippetCheck(
        "M247-E005-DOC-PKT-05",
        "`scripts/check_m247_e005_performance_slo_gate_and_reporting_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M247-E005-DOC-PKT-06",
        "`scripts/run_m247_e005_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M247-E005-DOC-PKT-07",
        "`tests/tooling/test_check_m247_e005_performance_slo_gate_and_reporting_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M247-E005-DOC-PKT-08",
        "`check:objc3c:m247-e005-lane-e-readiness`",
    ),
    SnippetCheck(
        "M247-E005-DOC-PKT-09",
        "`tmp/reports/m247/M247-E005/performance_slo_gate_and_reporting_edge_case_compatibility_completion_contract_summary.json`",
    ),
)

E004_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E005-E004-01",
        "# M247 Lane E Performance SLO Gate and Reporting Core Feature Expansion Expectations (E004)",
    ),
    SnippetCheck(
        "M247-E005-E004-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-core-feature-expansion-contract/m247-e004-v1`",
    ),
)

D004_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E005-D004-01",
        "# M247 Runtime/Link/Build Throughput Optimization Core Feature Expansion Expectations (D004)",
    ),
    SnippetCheck(
        "M247-E005-D004-02",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-core-feature-expansion-contract/m247-d004-v1`",
    ),
)

RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E005-RUN-01",
        '"""Run M247-E005 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck(
        "M247-E005-RUN-02",
        'BASELINE_DEPENDENCIES = ("M247-E004",)',
    ),
    SnippetCheck(
        "M247-E005-RUN-03",
        'PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A005", "M247-B006", "M247-C005", "M247-D004")',
    ),
    SnippetCheck("M247-E005-RUN-04", "check:objc3c:m247-e004-lane-e-readiness"),
    SnippetCheck("M247-E005-RUN-05", "check:objc3c:m247-a005-lane-a-readiness"),
    SnippetCheck("M247-E005-RUN-06", "check:objc3c:m247-b006-lane-b-readiness"),
    SnippetCheck("M247-E005-RUN-07", "check:objc3c:m247-c005-lane-c-readiness"),
    SnippetCheck("M247-E005-RUN-08", "check:objc3c:m247-d004-lane-d-readiness"),
    SnippetCheck(
        "M247-E005-RUN-09",
        "scripts/check_m247_e005_performance_slo_gate_and_reporting_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M247-E005-RUN-10",
        "tests/tooling/test_check_m247_e005_performance_slo_gate_and_reporting_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck("M247-E005-RUN-11", "[ok] M247-E005 lane-E readiness chain completed"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E005-PKG-01",
        '"check:objc3c:m247-e005-performance-slo-gate-reporting-edge-case-and-compatibility-completion-contract": '
        '"python scripts/check_m247_e005_performance_slo_gate_and_reporting_edge_case_and_compatibility_completion_contract.py"',
    ),
    SnippetCheck(
        "M247-E005-PKG-02",
        '"test:tooling:m247-e005-performance-slo-gate-reporting-edge-case-and-compatibility-completion-contract": '
        '"python -m pytest tests/tooling/test_check_m247_e005_performance_slo_gate_and_reporting_edge_case_and_compatibility_completion_contract.py -q"',
    ),
    SnippetCheck(
        "M247-E005-PKG-03",
        '"check:objc3c:m247-e005-lane-e-readiness": '
        '"python scripts/run_m247_e005_lane_e_readiness.py"',
    ),
    SnippetCheck("M247-E005-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M247-E005-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M247-E005-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M247-E005-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--e004-expectations-doc", type=Path, default=DEFAULT_E004_EXPECTATIONS_DOC)
    parser.add_argument("--d004-expectations-doc", type=Path, default=DEFAULT_D004_EXPECTATIONS_DOC)
    parser.add_argument("--runner-script", type=Path, default=DEFAULT_RUNNER_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-E005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-E005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e004_expectations_doc, "M247-E005-E004-DOC-EXISTS", E004_SNIPPETS),
        (args.d004_expectations_doc, "M247-E005-D004-DOC-EXISTS", D004_SNIPPETS),
        (args.runner_script, "M247-E005-RUN-EXISTS", RUNNER_SNIPPETS),
        (args.package_json, "M247-E005-PKG-EXISTS", PACKAGE_SNIPPETS),
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

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
