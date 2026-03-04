#!/usr/bin/env python3
"""Fail-closed checker for M249-E019 lane-E release gate/docs/runbooks advanced integration workpack shard1."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-e019-lane-e-release-gate-docs-runbooks-advanced-integration-workpack-shard1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_e019_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_packet.md"
)
DEFAULT_E018_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_advanced_conformance_workpack_shard1_e018_expectations.md"
)
DEFAULT_E018_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e018_lane_e_release_gate_docs_and_runbooks_advanced_conformance_workpack_shard1_packet.md"
)
DEFAULT_E018_CHECKER = (
    ROOT
    / "scripts"
    / "check_m249_e018_lane_e_release_gate_docs_and_runbooks_advanced_conformance_workpack_shard1_contract.py"
)
DEFAULT_E018_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m249_e018_lane_e_release_gate_docs_and_runbooks_advanced_conformance_workpack_shard1_contract.py"
)
DEFAULT_E018_READINESS_RUNNER = ROOT / "scripts" / "run_m249_e018_lane_e_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m249_e019_lane_e_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-E019/lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_summary.json"
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
        "M249-E019-E018-01",
        "M249-E018",
        Path("docs/contracts/m249_lane_e_release_gate_docs_and_runbooks_advanced_conformance_workpack_shard1_e018_expectations.md"),
    ),
    AssetCheck(
        "M249-E019-E018-02",
        "M249-E018",
        Path("spec/planning/compiler/m249/m249_e018_lane_e_release_gate_docs_and_runbooks_advanced_conformance_workpack_shard1_packet.md"),
    ),
    AssetCheck(
        "M249-E019-E018-03",
        "M249-E018",
        Path("scripts/check_m249_e018_lane_e_release_gate_docs_and_runbooks_advanced_conformance_workpack_shard1_contract.py"),
    ),
    AssetCheck(
        "M249-E019-E018-04",
        "M249-E018",
        Path("tests/tooling/test_check_m249_e018_lane_e_release_gate_docs_and_runbooks_advanced_conformance_workpack_shard1_contract.py"),
    ),
    AssetCheck(
        "M249-E019-E018-05",
        "M249-E018",
        Path("scripts/run_m249_e018_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M249-E019-A007-01",
        "M249-A007",
        Path("docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_diagnostics_hardening_a007_expectations.md"),
    ),
    AssetCheck(
        "M249-E019-A007-02",
        "M249-A007",
        Path("spec/planning/compiler/m249/m249_a007_feature_packaging_surface_and_compatibility_contracts_diagnostics_hardening_packet.md"),
    ),
    AssetCheck(
        "M249-E019-A007-03",
        "M249-A007",
        Path("scripts/check_m249_a007_feature_packaging_surface_and_compatibility_contracts_diagnostics_hardening_contract.py"),
    ),
    AssetCheck(
        "M249-E019-A007-04",
        "M249-A007",
        Path("tests/tooling/test_check_m249_a007_feature_packaging_surface_and_compatibility_contracts_diagnostics_hardening_contract.py"),
    ),
    AssetCheck(
        "M249-E019-A007-05",
        "M249-A007",
        Path("scripts/run_m249_a007_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M249-E019-B009-01",
        "M249-B009",
        Path("docs/contracts/m249_semantic_compatibility_and_migration_checks_conformance_matrix_implementation_b009_expectations.md"),
    ),
    AssetCheck(
        "M249-E019-B009-02",
        "M249-B009",
        Path("spec/planning/compiler/m249/m249_b009_semantic_compatibility_and_migration_checks_conformance_matrix_implementation_packet.md"),
    ),
    AssetCheck(
        "M249-E019-B009-03",
        "M249-B009",
        Path("scripts/check_m249_b009_semantic_compatibility_and_migration_checks_conformance_matrix_implementation_contract.py"),
    ),
    AssetCheck(
        "M249-E019-B009-04",
        "M249-B009",
        Path("tests/tooling/test_check_m249_b009_semantic_compatibility_and_migration_checks_conformance_matrix_implementation_contract.py"),
    ),
    AssetCheck(
        "M249-E019-B009-05",
        "M249-B009",
        Path("scripts/run_m249_b009_lane_b_readiness.py"),
    ),
    AssetCheck(
        "M249-E019-C010-01",
        "M249-C010",
        Path("docs/contracts/m249_ir_object_packaging_and_symbol_policy_conformance_corpus_expansion_c010_expectations.md"),
    ),
    AssetCheck(
        "M249-E019-C010-02",
        "M249-C010",
        Path("spec/planning/compiler/m249/m249_c010_ir_object_packaging_and_symbol_policy_conformance_corpus_expansion_packet.md"),
    ),
    AssetCheck(
        "M249-E019-C010-03",
        "M249-C010",
        Path("scripts/check_m249_c010_ir_object_packaging_and_symbol_policy_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M249-E019-C010-04",
        "M249-C010",
        Path("tests/tooling/test_check_m249_c010_ir_object_packaging_and_symbol_policy_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M249-E019-D016-01",
        "M249-D016",
        Path("docs/contracts/m249_installer_runtime_operations_and_support_tooling_advanced_edge_compatibility_workpack_shard1_d016_expectations.md"),
    ),
    AssetCheck(
        "M249-E019-D016-02",
        "M249-D016",
        Path("spec/planning/compiler/m249/m249_d016_installer_runtime_operations_and_support_tooling_advanced_edge_compatibility_workpack_shard1_packet.md"),
    ),
    AssetCheck(
        "M249-E019-D016-03",
        "M249-D016",
        Path("scripts/check_m249_d016_installer_runtime_operations_and_support_tooling_advanced_edge_compatibility_workpack_shard1_contract.py"),
    ),
    AssetCheck(
        "M249-E019-D016-04",
        "M249-D016",
        Path("tests/tooling/test_check_m249_d016_installer_runtime_operations_and_support_tooling_advanced_edge_compatibility_workpack_shard1_contract.py"),
    ),
    AssetCheck(
        "M249-E019-D016-05",
        "M249-D016",
        Path("scripts/run_m249_d016_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E019-DOC-EXP-01",
        "# M249 Lane E Release Gate, Docs, and Runbooks Advanced Integration Workpack (Shard 1) Expectations (E019)",
    ),
    SnippetCheck(
        "M249-E019-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-advanced-integration-workpack-shard1/m249-e019-v1`",
    ),
    SnippetCheck(
        "M249-E019-DOC-EXP-03",
        "Dependencies: `M249-E018`, `M249-A007`, `M249-B009`, `M249-C010`, `M249-D016`",
    ),
    SnippetCheck("M249-E019-DOC-EXP-04", "- Issue: `#6966`"),
    SnippetCheck("M249-E019-DOC-EXP-05", "`python scripts/run_m249_e018_lane_e_readiness.py`"),
    SnippetCheck("M249-E019-DOC-EXP-06", "`check:objc3c:m249-a007-lane-a-readiness`"),
    SnippetCheck("M249-E019-DOC-EXP-07", "`python scripts/run_m249_b009_lane_b_readiness.py`"),
    SnippetCheck("M249-E019-DOC-EXP-08", "`check:objc3c:m249-c010-lane-c-readiness`"),
    SnippetCheck("M249-E019-DOC-EXP-09", "`python scripts/run_m249_d016_lane_d_readiness.py`"),
    SnippetCheck(
        "M249-E019-DOC-EXP-10",
        "`scripts/check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py`",
    ),
    SnippetCheck(
        "M249-E019-DOC-EXP-11",
        "`tests/tooling/test_check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py`",
    ),
    SnippetCheck("M249-E019-DOC-EXP-12", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck("M249-E019-DOC-EXP-13", "`test:objc3c:parser-ast-extraction`"),
    SnippetCheck(
        "M249-E019-DOC-EXP-14",
        "`tmp/reports/m249/M249-E019/lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E019-DOC-PKT-01",
        "# M249-E019 Lane-E Release Gate, Docs, and Runbooks Advanced Integration Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M249-E019-DOC-PKT-02", "Packet: `M249-E019`"),
    SnippetCheck("M249-E019-DOC-PKT-03", "Issue: `#6966`"),
    SnippetCheck(
        "M249-E019-DOC-PKT-04",
        "Dependencies: `M249-E018`, `M249-A007`, `M249-B009`, `M249-C010`, `M249-D016`",
    ),
    SnippetCheck(
        "M249-E019-DOC-PKT-05",
        "scripts/check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M249-E019-DOC-PKT-06",
        "tests/tooling/test_check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M249-E019-DOC-PKT-07",
        "including conformance improvements as mandatory scope inputs.",
    ),
    SnippetCheck("M249-E019-DOC-PKT-08", "scripts/run_m249_e019_lane_e_readiness.py"),
    SnippetCheck("M249-E019-DOC-PKT-09", "scripts/run_m249_e018_lane_e_readiness.py"),
    SnippetCheck("M249-E019-DOC-PKT-10", "check:objc3c:m249-a007-lane-a-readiness"),
    SnippetCheck("M249-E019-DOC-PKT-11", "scripts/run_m249_b009_lane_b_readiness.py"),
    SnippetCheck("M249-E019-DOC-PKT-12", "check:objc3c:m249-c010-lane-c-readiness"),
    SnippetCheck("M249-E019-DOC-PKT-13", "scripts/run_m249_d016_lane_d_readiness.py"),
    SnippetCheck(
        "M249-E019-DOC-PKT-14",
        "integration improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M249-E019-DOC-PKT-15",
        "tmp/reports/m249/M249-E019/lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_summary.json",
    ),
)

E018_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E019-E018-DOC-01",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-advanced-conformance-workpack-shard1/m249-e018-v1`",
    ),
    SnippetCheck(
        "M249-E019-E018-DOC-02",
        "Dependencies: `M249-E017`, `M249-A007`, `M249-B008`, `M249-C009`, `M249-D015`",
    ),
)

E018_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-E019-E018-PKT-01", "Packet: `M249-E018`"),
    SnippetCheck("M249-E019-E018-PKT-02", "Issue: `#6965`"),
    SnippetCheck(
        "M249-E019-E018-PKT-03",
        "Dependencies: `M249-E017`, `M249-A007`, `M249-B008`, `M249-C009`, `M249-D015`",
    ),
)

RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E019-RUN-01",
        '"""Run M249-E019 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M249-E019-RUN-02", "scripts/run_m249_e018_lane_e_readiness.py"),
    SnippetCheck("M249-E019-RUN-03", "check:objc3c:m249-a007-lane-a-readiness"),
    SnippetCheck("M249-E019-RUN-04", "scripts/run_m249_b009_lane_b_readiness.py"),
    SnippetCheck("M249-E019-RUN-05", "check:objc3c:m249-c010-lane-c-readiness"),
    SnippetCheck("M249-E019-RUN-06", "scripts/run_m249_d016_lane_d_readiness.py"),
    SnippetCheck(
        "M249-E019-RUN-07",
        "scripts/check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M249-E019-RUN-08",
        "tests/tooling/test_check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py",
    ),
    SnippetCheck("M249-E019-RUN-09", "[ok] M249-E019 lane-E readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E019-ARCH-01",
        "M249 lane-E E019 advanced integration workpack (shard 1) anchors release gate/docs/runbooks continuity",
    ),
    SnippetCheck(
        "M249-E019-ARCH-02",
        "`M249-E018`, `M249-A007`, `M249-B009`, `M249-C010`, and `M249-D016`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E019-SPC-01",
        "release gate/docs/runbooks advanced integration workpack (shard 1) governance shall preserve",
    ),
    SnippetCheck(
        "M249-E019-SPC-02",
        "explicit lane-E dependency anchors (`M249-E019`, `M249-E018`, `M249-A007`, `M249-B009`,",
    ),
    SnippetCheck(
        "M249-E019-SPC-03",
        "`M249-C010`, and `M249-D016`) and fail closed on",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E019-META-01",
        "deterministic lane-E release gate/docs/runbooks advanced integration workpack (shard 1) metadata anchors for `M249-E019`",
    ),
    SnippetCheck(
        "M249-E019-META-02",
        "with explicit `M249-E018`, `M249-A007`, `M249-B009`, `M249-C010`, and `M249-D016` dependency continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E019-PKG-01",
        '"check:objc3c:m249-e019-lane-e-release-gate-docs-runbooks-advanced-integration-workpack-shard1-contract": "python scripts/check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py"',
    ),
    SnippetCheck(
        "M249-E019-PKG-02",
        '"test:tooling:m249-e019-lane-e-release-gate-docs-runbooks-advanced-integration-workpack-shard1-contract": "python -m pytest tests/tooling/test_check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py -q"',
    ),
    SnippetCheck(
        "M249-E019-PKG-03",
        '"check:objc3c:m249-e019-lane-e-readiness": "python scripts/run_m249_e019_lane_e_readiness.py"',
    ),
    SnippetCheck("M249-E019-PKG-04", '"check:objc3c:m249-e018-lane-e-readiness": '),
    SnippetCheck("M249-E019-PKG-05", '"check:objc3c:m249-a007-lane-a-readiness": '),
    SnippetCheck("M249-E019-PKG-06", '"check:objc3c:m249-b009-lane-b-readiness": '),
    SnippetCheck("M249-E019-PKG-07", '"check:objc3c:m249-c010-lane-c-readiness": '),
    SnippetCheck("M249-E019-PKG-08", '"check:objc3c:m249-d016-lane-d-readiness": '),
    SnippetCheck("M249-E019-PKG-09", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M249-E019-PKG-10", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--e018-expectations-doc", type=Path, default=DEFAULT_E018_EXPECTATIONS_DOC)
    parser.add_argument("--e018-packet-doc", type=Path, default=DEFAULT_E018_PACKET_DOC)
    parser.add_argument("--e018-checker", type=Path, default=DEFAULT_E018_CHECKER)
    parser.add_argument("--e018-test", type=Path, default=DEFAULT_E018_TEST)
    parser.add_argument("--e018-readiness-runner", type=Path, default=DEFAULT_E018_READINESS_RUNNER)
    parser.add_argument(
        "--runner-script",
        "--readiness-runner",
        dest="runner_script",
        type=Path,
        default=DEFAULT_READINESS_RUNNER,
    )
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
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
        (args.expectations_doc, "M249-E019-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M249-E019-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e018_expectations_doc, "M249-E019-E018-DOC-EXISTS", E018_EXPECTATIONS_SNIPPETS),
        (args.e018_packet_doc, "M249-E019-E018-PKT-EXISTS", E018_PACKET_SNIPPETS),
        (args.runner_script, "M249-E019-RUN-EXISTS", RUNNER_SNIPPETS),
        (args.architecture_doc, "M249-E019-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M249-E019-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M249-E019-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M249-E019-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_text_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e018_checker, "M249-E019-DEP-E018-ARG-01"),
        (args.e018_test, "M249-E019-DEP-E018-ARG-02"),
        (args.e018_readiness_runner, "M249-E019-DEP-E018-ARG-03"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is not a file: {display_path(path)}",
                )
            )

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
