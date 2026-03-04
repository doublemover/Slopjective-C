#!/usr/bin/env python3
"""Fail-closed checker for M249-E024 lane-E release gate/docs/runbooks integration closeout and gate signoff."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-e024-lane-e-release-gate-docs-runbooks-integration-closeout-and-gate-signoff-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_e024_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_packet.md"
)
DEFAULT_E023_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_advanced_diagnostics_workpack_shard2_e023_expectations.md"
)
DEFAULT_E023_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e023_lane_e_release_gate_docs_and_runbooks_advanced_diagnostics_workpack_shard2_packet.md"
)
DEFAULT_E023_CHECKER = (
    ROOT
    / "scripts"
    / "check_m249_e023_lane_e_release_gate_docs_and_runbooks_advanced_diagnostics_workpack_shard2_contract.py"
)
DEFAULT_E023_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m249_e023_lane_e_release_gate_docs_and_runbooks_advanced_diagnostics_workpack_shard2_contract.py"
)
DEFAULT_E023_READINESS_RUNNER = ROOT / "scripts" / "run_m249_e023_lane_e_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m249_e024_lane_e_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-E024/lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_summary.json"
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
        "M249-E024-E023-01",
        "M249-E023",
        Path("docs/contracts/m249_lane_e_release_gate_docs_and_runbooks_advanced_diagnostics_workpack_shard2_e023_expectations.md"),
    ),
    AssetCheck(
        "M249-E024-E023-02",
        "M249-E023",
        Path("spec/planning/compiler/m249/m249_e023_lane_e_release_gate_docs_and_runbooks_advanced_diagnostics_workpack_shard2_packet.md"),
    ),
    AssetCheck(
        "M249-E024-E023-03",
        "M249-E023",
        Path("scripts/check_m249_e023_lane_e_release_gate_docs_and_runbooks_advanced_diagnostics_workpack_shard2_contract.py"),
    ),
    AssetCheck(
        "M249-E024-E023-04",
        "M249-E023",
        Path("tests/tooling/test_check_m249_e023_lane_e_release_gate_docs_and_runbooks_advanced_diagnostics_workpack_shard2_contract.py"),
    ),
    AssetCheck(
        "M249-E024-E023-05",
        "M249-E023",
        Path("scripts/run_m249_e023_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M249-E024-A009-01",
        "M249-A009",
        Path("docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_integration_closeout_and_gate_signoff_a009_expectations.md"),
    ),
    AssetCheck(
        "M249-E024-A009-02",
        "M249-A009",
        Path("spec/planning/compiler/m249/m249_a009_feature_packaging_surface_and_compatibility_contracts_integration_closeout_and_gate_signoff_packet.md"),
    ),
    AssetCheck(
        "M249-E024-A009-03",
        "M249-A009",
        Path("scripts/check_m249_a009_feature_packaging_surface_and_compatibility_contracts_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M249-E024-A009-04",
        "M249-A009",
        Path("tests/tooling/test_check_m249_a009_feature_packaging_surface_and_compatibility_contracts_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M249-E024-A009-05",
        "M249-A009",
        Path("scripts/run_m249_a009_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M249-E024-B011-01",
        "M249-B011",
        Path("docs/contracts/m249_semantic_compatibility_and_migration_checks_lane_b_integration_closeout_gate_b011_expectations.md"),
    ),
    AssetCheck(
        "M249-E024-B011-02",
        "M249-B011",
        Path("spec/planning/compiler/m249/m249_b011_semantic_compatibility_and_migration_checks_lane_b_integration_closeout_gate_packet.md"),
    ),
    AssetCheck(
        "M249-E024-B011-03",
        "M249-B011",
        Path("scripts/check_m249_b011_semantic_compatibility_and_migration_checks_lane_b_integration_closeout_gate_contract.py"),
    ),
    AssetCheck(
        "M249-E024-B011-04",
        "M249-B011",
        Path("tests/tooling/test_check_m249_b011_semantic_compatibility_and_migration_checks_lane_b_integration_closeout_gate_contract.py"),
    ),
    AssetCheck(
        "M249-E024-B011-05",
        "M249-B011",
        Path("scripts/run_m249_b011_lane_b_readiness.py"),
    ),
    AssetCheck(
        "M249-E024-C012-01",
        "M249-C012",
        Path("docs/contracts/m249_ir_object_packaging_and_symbol_policy_cross_lane_integration_sync_c012_expectations.md"),
    ),
    AssetCheck(
        "M249-E024-C012-02",
        "M249-C012",
        Path("spec/planning/compiler/m249/m249_c012_ir_object_packaging_and_symbol_policy_cross_lane_integration_sync_packet.md"),
    ),
    AssetCheck(
        "M249-E024-C012-03",
        "M249-C012",
        Path("scripts/check_m249_c012_ir_object_packaging_and_symbol_policy_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M249-E024-C012-04",
        "M249-C012",
        Path("tests/tooling/test_check_m249_c012_ir_object_packaging_and_symbol_policy_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M249-E024-D020-01",
        "M249-D020",
        Path("docs/contracts/m249_installer_runtime_operations_and_support_tooling_integration_closeout_and_gate_signoff_d020_expectations.md"),
    ),
    AssetCheck(
        "M249-E024-D020-02",
        "M249-D020",
        Path("spec/planning/compiler/m249/m249_d020_installer_runtime_operations_and_support_tooling_integration_closeout_and_gate_signoff_packet.md"),
    ),
    AssetCheck(
        "M249-E024-D020-03",
        "M249-D020",
        Path("scripts/check_m249_d020_installer_runtime_operations_and_support_tooling_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M249-E024-D020-04",
        "M249-D020",
        Path("tests/tooling/test_check_m249_d020_installer_runtime_operations_and_support_tooling_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M249-E024-D020-05",
        "M249-D020",
        Path("scripts/run_m249_d020_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E024-DOC-EXP-01",
        "# M249 Lane E Release Gate, Docs, and Runbooks Integration Closeout and Gate Signoff Expectations (E024)",
    ),
    SnippetCheck(
        "M249-E024-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-integration-closeout-and-gate-signoff/m249-e024-v1`",
    ),
    SnippetCheck(
        "M249-E024-DOC-EXP-03",
        "Dependencies: `M249-E023`, `M249-A009`, `M249-B011`, `M249-C012`, `M249-D020`",
    ),
    SnippetCheck("M249-E024-DOC-EXP-04", "- Issue: `#6971`"),
    SnippetCheck("M249-E024-DOC-EXP-05", "`python scripts/run_m249_e023_lane_e_readiness.py`"),
    SnippetCheck("M249-E024-DOC-EXP-06", "`check:objc3c:m249-a009-lane-a-readiness`"),
    SnippetCheck("M249-E024-DOC-EXP-07", "`python scripts/run_m249_b011_lane_b_readiness.py`"),
    SnippetCheck("M249-E024-DOC-EXP-08", "`check:objc3c:m249-c012-lane-c-readiness`"),
    SnippetCheck("M249-E024-DOC-EXP-09", "`python scripts/run_m249_d020_lane_d_readiness.py`"),
    SnippetCheck(
        "M249-E024-DOC-EXP-10",
        "`scripts/check_m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_contract.py`",
    ),
    SnippetCheck(
        "M249-E024-DOC-EXP-11",
        "`tests/tooling/test_check_m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_contract.py`",
    ),
    SnippetCheck("M249-E024-DOC-EXP-12", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck("M249-E024-DOC-EXP-13", "`test:objc3c:parser-ast-extraction`"),
    SnippetCheck(
        "M249-E024-DOC-EXP-14",
        "`tmp/reports/m249/M249-E024/lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E024-DOC-PKT-01",
        "# M249-E024 Lane-E Release Gate, Docs, and Runbooks Integration Closeout and Gate Signoff Packet",
    ),
    SnippetCheck("M249-E024-DOC-PKT-02", "Packet: `M249-E024`"),
    SnippetCheck("M249-E024-DOC-PKT-03", "Issue: `#6971`"),
    SnippetCheck(
        "M249-E024-DOC-PKT-04",
        "Dependencies: `M249-E023`, `M249-A009`, `M249-B011`, `M249-C012`, `M249-D020`",
    ),
    SnippetCheck(
        "M249-E024-DOC-PKT-05",
        "scripts/check_m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M249-E024-DOC-PKT-06",
        "tests/tooling/test_check_m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M249-E024-DOC-PKT-07",
        "including integration closeout and gate signoff advancements as mandatory scope inputs.",
    ),
    SnippetCheck("M249-E024-DOC-PKT-08", "scripts/run_m249_e024_lane_e_readiness.py"),
    SnippetCheck("M249-E024-DOC-PKT-09", "scripts/run_m249_e023_lane_e_readiness.py"),
    SnippetCheck("M249-E024-DOC-PKT-10", "check:objc3c:m249-a009-lane-a-readiness"),
    SnippetCheck("M249-E024-DOC-PKT-11", "scripts/run_m249_b011_lane_b_readiness.py"),
    SnippetCheck("M249-E024-DOC-PKT-12", "check:objc3c:m249-c012-lane-c-readiness"),
    SnippetCheck("M249-E024-DOC-PKT-13", "scripts/run_m249_d020_lane_d_readiness.py"),
    SnippetCheck(
        "M249-E024-DOC-PKT-14",
        "integration closeout and gate signoff improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M249-E024-DOC-PKT-15",
        "tmp/reports/m249/M249-E024/lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_summary.json",
    ),
)

E023_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E024-E023-DOC-01",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-advanced-diagnostics-workpack-shard2/m249-e023-v1`",
    ),
    SnippetCheck(
        "M249-E024-E023-DOC-02",
        "Dependencies: `M249-E022`, `M249-A009`, `M249-B011`, `M249-C012`, `M249-D019`",
    ),
)

E023_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-E024-E023-PKT-01", "Packet: `M249-E023`"),
    SnippetCheck("M249-E024-E023-PKT-02", "Issue: `#6970`"),
    SnippetCheck(
        "M249-E024-E023-PKT-03",
        "Dependencies: `M249-E022`, `M249-A009`, `M249-B011`, `M249-C012`, `M249-D019`",
    ),
)

RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E024-RUN-01",
        '"""Run M249-E024 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M249-E024-RUN-02", "scripts/run_m249_e023_lane_e_readiness.py"),
    SnippetCheck("M249-E024-RUN-03", "check:objc3c:m249-a009-lane-a-readiness"),
    SnippetCheck("M249-E024-RUN-04", "scripts/run_m249_b011_lane_b_readiness.py"),
    SnippetCheck("M249-E024-RUN-05", "check:objc3c:m249-c012-lane-c-readiness"),
    SnippetCheck("M249-E024-RUN-06", "scripts/run_m249_d020_lane_d_readiness.py"),
    SnippetCheck(
        "M249-E024-RUN-07",
        "scripts/check_m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M249-E024-RUN-08",
        "tests/tooling/test_check_m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck("M249-E024-RUN-09", "[ok] M249-E024 lane-E readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E024-ARCH-01",
        "M249 lane-E E024 integration closeout and gate signoff anchors release gate/docs/runbooks continuity",
    ),
    SnippetCheck(
        "M249-E024-ARCH-02",
        "`M249-E023`, `M249-A009`, `M249-B011`, `M249-C012`, and `M249-D020`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E024-SPC-01",
        "release gate/docs/runbooks integration closeout and gate signoff governance shall preserve",
    ),
    SnippetCheck(
        "M249-E024-SPC-02",
        "explicit lane-E dependency anchors (`M249-E024`, `M249-E023`, `M249-A009`, `M249-B011`,",
    ),
    SnippetCheck(
        "M249-E024-SPC-03",
        "`M249-C012`, and `M249-D020`) and fail closed on",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E024-META-01",
        "deterministic lane-E release gate/docs/runbooks integration closeout and gate signoff metadata anchors for `M249-E024`",
    ),
    SnippetCheck(
        "M249-E024-META-02",
        "with explicit `M249-E023`, `M249-A009`, `M249-B011`, `M249-C012`, and `M249-D020` dependency continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E024-PKG-01",
        '"check:objc3c:m249-e024-lane-e-release-gate-docs-runbooks-integration-closeout-and-gate-signoff-contract": "python scripts/check_m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_contract.py"',
    ),
    SnippetCheck(
        "M249-E024-PKG-02",
        '"test:tooling:m249-e024-lane-e-release-gate-docs-runbooks-integration-closeout-and-gate-signoff-contract": "python -m pytest tests/tooling/test_check_m249_e024_lane_e_release_gate_docs_and_runbooks_integration_closeout_and_gate_signoff_contract.py -q"',
    ),
    SnippetCheck(
        "M249-E024-PKG-03",
        '"check:objc3c:m249-e024-lane-e-readiness": "python scripts/run_m249_e024_lane_e_readiness.py"',
    ),
    SnippetCheck("M249-E024-PKG-04", '"check:objc3c:m249-e023-lane-e-readiness": '),
    SnippetCheck("M249-E024-PKG-05", '"check:objc3c:m249-a009-lane-a-readiness": '),
    SnippetCheck("M249-E024-PKG-06", '"check:objc3c:m249-b011-lane-b-readiness": '),
    SnippetCheck("M249-E024-PKG-07", '"check:objc3c:m249-c012-lane-c-readiness": '),
    SnippetCheck("M249-E024-PKG-08", '"check:objc3c:m249-d020-lane-d-readiness": '),
    SnippetCheck("M249-E024-PKG-09", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M249-E024-PKG-10", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--e023-expectations-doc", type=Path, default=DEFAULT_E023_EXPECTATIONS_DOC)
    parser.add_argument("--e023-packet-doc", type=Path, default=DEFAULT_E023_PACKET_DOC)
    parser.add_argument("--e023-checker", type=Path, default=DEFAULT_E023_CHECKER)
    parser.add_argument("--e023-test", type=Path, default=DEFAULT_E023_TEST)
    parser.add_argument("--e023-readiness-runner", type=Path, default=DEFAULT_E023_READINESS_RUNNER)
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
        (args.expectations_doc, "M249-E024-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M249-E024-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e023_expectations_doc, "M249-E024-E023-DOC-EXISTS", E023_EXPECTATIONS_SNIPPETS),
        (args.e023_packet_doc, "M249-E024-E023-PKT-EXISTS", E023_PACKET_SNIPPETS),
        (args.runner_script, "M249-E024-RUN-EXISTS", RUNNER_SNIPPETS),
        (args.architecture_doc, "M249-E024-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M249-E024-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M249-E024-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M249-E024-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_text_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e023_checker, "M249-E024-DEP-E023-ARG-01"),
        (args.e023_test, "M249-E024-DEP-E023-ARG-02"),
        (args.e023_readiness_runner, "M249-E024-DEP-E023-ARG-03"),
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

