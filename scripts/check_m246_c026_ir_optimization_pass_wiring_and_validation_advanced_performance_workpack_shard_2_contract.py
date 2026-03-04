#!/usr/bin/env python3
"""Fail-closed checker for M246-C026 IR optimization pass wiring advanced performance workpack."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-c026-ir-optimization-pass-wiring-validation-advanced-performance-workpack-shard-2-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_c026_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_packet.md"
)
DEFAULT_C001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_c001_expectations.md"
)
DEFAULT_C002_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_c002_expectations.md"
)
DEFAULT_C025_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_c025_expectations.md"
)
DEFAULT_C025_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_c025_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_packet.md"
)
DEFAULT_C025_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_c025_lane_c_readiness.py"
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_c026_lane_c_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-C026/ir_optimization_pass_wiring_validation_advanced_performance_workpack_shard_2_summary.json"
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
        "M246-C026-DEP-C001-01",
        Path("docs/contracts/m246_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_c001_expectations.md"),
    ),
    AssetCheck(
        "M246-C026-DEP-C001-02",
        Path("spec/planning/compiler/m246/m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M246-C026-DEP-C001-03",
        Path("scripts/check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py"),
    ),
    AssetCheck(
        "M246-C026-DEP-C001-04",
        Path("tests/tooling/test_check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py"),
    ),
    AssetCheck(
        "M246-C026-DEP-C002-01",
        Path("docs/contracts/m246_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_c002_expectations.md"),
    ),
    AssetCheck(
        "M246-C026-DEP-C002-02",
        Path("spec/planning/compiler/m246/m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_packet.md"),
    ),
    AssetCheck(
        "M246-C026-DEP-C002-03",
        Path("scripts/check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M246-C026-DEP-C002-04",
        Path("tests/tooling/test_check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M246-C026-DEP-C025-01",
        Path("docs/contracts/m246_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_c025_expectations.md"),
    ),
    AssetCheck(
        "M246-C026-DEP-C025-02",
        Path("spec/planning/compiler/m246/m246_c025_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_packet.md"),
    ),
    AssetCheck(
        "M246-C026-DEP-C025-03",
        Path("scripts/check_m246_c025_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_contract.py"),
    ),
    AssetCheck(
        "M246-C026-DEP-C025-04",
        Path("tests/tooling/test_check_m246_c025_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_contract.py"),
    ),
    AssetCheck(
        "M246-C026-DEP-C025-05",
        Path("scripts/run_m246_c025_lane_c_readiness.py"),
    ),
    AssetCheck(
        "M246-C026-OWN-01",
        Path("docs/contracts/m246_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_c026_expectations.md"),
    ),
    AssetCheck(
        "M246-C026-OWN-02",
        Path("spec/planning/compiler/m246/m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_packet.md"),
    ),
    AssetCheck(
        "M246-C026-OWN-03",
        Path("scripts/check_m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_contract.py"),
    ),
    AssetCheck(
        "M246-C026-OWN-04",
        Path("tests/tooling/test_check_m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_contract.py"),
    ),
    AssetCheck(
        "M246-C026-OWN-05",
        Path("scripts/run_m246_c026_lane_c_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-C026-DOC-EXP-01",
        "# M246 IR Optimization Pass Wiring and Validation Advanced Performance Workpack (Shard 2) Expectations (C026)",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-02",
        "Contract ID: `objc3c-ir-optimization-pass-wiring-validation-advanced-performance-workpack-shard-2/m246-c026-v1`",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-03",
        "Issue `#5102` defines canonical lane-C advanced performance workpack (shard 2) scope.",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-04",
        "Dependencies: `M246-C025`",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-05",
        "Predecessor anchors inherited via `M246-C025`: `M246-C001`, `M246-C002`, `M246-C003`, `M246-C004`, `M246-C005`, `M246-C006`, `M246-C007`, `M246-C008`, `M246-C009`, `M246-C010`, `M246-C011`, `M246-C012`.",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-06",
        "docs/contracts/m246_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_c025_expectations.md",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-07",
        "scripts/run_m246_c025_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-08",
        "scripts/check_m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_contract.py",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-09",
        "tests/tooling/test_check_m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_contract.py",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-10",
        "improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M246-C026-DOC-EXP-11",
        "tmp/reports/m246/M246-C026/ir_optimization_pass_wiring_validation_advanced_performance_workpack_shard_2_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-C026-DOC-PKT-01",
        "# M246-C026 IR Optimization Pass Wiring and Validation Advanced Performance Workpack (Shard 2) Packet",
    ),
    SnippetCheck("M246-C026-DOC-PKT-02", "Packet: `M246-C026`"),
    SnippetCheck("M246-C026-DOC-PKT-03", "Issue: `#5102`"),
    SnippetCheck("M246-C026-DOC-PKT-04", "Freeze date: `2026-03-04`"),
    SnippetCheck("M246-C026-DOC-PKT-05", "Dependencies: `M246-C025`"),
    SnippetCheck(
        "M246-C026-DOC-PKT-06",
        "Predecessor anchors inherited via `M246-C025`: `M246-C001`, `M246-C002`, `M246-C003`, `M246-C004`, `M246-C005`, `M246-C006`, `M246-C007`, `M246-C008`, `M246-C009`, `M246-C010`, `M246-C011`, `M246-C012`.",
    ),
    SnippetCheck(
        "M246-C026-DOC-PKT-07",
        "scripts/check_m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_contract.py",
    ),
    SnippetCheck(
        "M246-C026-DOC-PKT-08",
        "tests/tooling/test_check_m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_contract.py",
    ),
    SnippetCheck(
        "M246-C026-DOC-PKT-09",
        "scripts/run_m246_c026_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M246-C026-DOC-PKT-10",
        "scripts/run_m246_c025_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M246-C026-DOC-PKT-11",
        "treated as mandatory scope",
    ),
    SnippetCheck(
        "M246-C026-DOC-PKT-12",
        "tmp/reports/m246/M246-C026/ir_optimization_pass_wiring_validation_advanced_performance_workpack_shard_2_summary.json",
    ),
)

C001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-C026-C001-01",
        "Contract ID: `objc3c-ir-optimization-pass-wiring-validation/m246-c001-v1`",
    ),
    SnippetCheck("M246-C026-C001-02", "Dependencies: none"),
)

C002_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-C026-C002-01",
        "Contract ID: `objc3c-ir-optimization-pass-wiring-validation-modular-split-scaffolding/m246-c002-v1`",
    ),
    SnippetCheck(
        "M246-C026-C002-02",
        "Dependencies: `M246-C001`",
    ),
)

C025_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-C026-C025-EXP-01",
        "Contract ID: `objc3c-ir-optimization-pass-wiring-validation-advanced-integration-workpack-shard-2/m246-c025-v1`",
    ),
    SnippetCheck(
        "M246-C026-C025-EXP-02",
        "Dependencies: `M246-C024`",
    ),
    SnippetCheck(
        "M246-C026-C025-EXP-03",
        "scripts/run_m246_c024_lane_c_readiness.py",
    ),
)

C025_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-C026-C025-PKT-01", "Packet: `M246-C025`"),
    SnippetCheck("M246-C026-C025-PKT-02", "Dependencies: `M246-C024`"),
    SnippetCheck("M246-C026-C025-PKT-03", "scripts/run_m246_c024_lane_c_readiness.py"),
)

C025_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-C026-C025-RUN-01",
        "scripts/run_m246_c024_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M246-C026-C025-RUN-02",
        "scripts/check_m246_c025_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_contract.py",
    ),
    SnippetCheck(
        "M246-C026-C025-RUN-03",
        "tests/tooling/test_check_m246_c025_ir_optimization_pass_wiring_and_validation_advanced_integration_workpack_shard_2_contract.py",
    ),
    SnippetCheck(
        "M246-C026-C025-RUN-04",
        "[ok] M246-C025 lane-C readiness chain completed",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-C026-RUN-01",
        "scripts/run_m246_c025_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M246-C026-RUN-02",
        "scripts/check_m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_contract.py",
    ),
    SnippetCheck(
        "M246-C026-RUN-03",
        "tests/tooling/test_check_m246_c026_ir_optimization_pass_wiring_and_validation_advanced_performance_workpack_shard_2_contract.py",
    ),
    SnippetCheck(
        "M246-C026-RUN-04",
        "[ok] M246-C026 lane-C readiness chain completed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-C026-PKG-01",
        '"check:objc3c:m246-c002-lane-c-readiness": ',
    ),
    SnippetCheck("M246-C026-PKG-02", '"compile:objc3c": '),
    SnippetCheck("M246-C026-PKG-03", '"proof:objc3c": '),
    SnippetCheck("M246-C026-PKG-04", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M246-C026-PKG-05", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--c001-expectations-doc", type=Path, default=DEFAULT_C001_EXPECTATIONS_DOC)
    parser.add_argument("--c002-expectations-doc", type=Path, default=DEFAULT_C002_EXPECTATIONS_DOC)
    parser.add_argument("--c025-expectations-doc", type=Path, default=DEFAULT_C025_EXPECTATIONS_DOC)
    parser.add_argument("--c025-packet-doc", type=Path, default=DEFAULT_C025_PACKET_DOC)
    parser.add_argument("--c025-readiness-script", type=Path, default=DEFAULT_C025_READINESS_SCRIPT)
    parser.add_argument("--readiness-script", type=Path, default=DEFAULT_READINESS_SCRIPT)
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


def check_text_artifact(
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
        (args.expectations_doc, "M246-C026-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-C026-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c001_expectations_doc, "M246-C026-C001-EXISTS", C001_EXPECTATIONS_SNIPPETS),
        (args.c002_expectations_doc, "M246-C026-C002-EXISTS", C002_EXPECTATIONS_SNIPPETS),
        (args.c025_expectations_doc, "M246-C026-C025-EXP-EXISTS", C025_EXPECTATIONS_SNIPPETS),
        (args.c025_packet_doc, "M246-C026-C025-PKT-EXISTS", C025_PACKET_SNIPPETS),
        (args.c025_readiness_script, "M246-C026-C025-RUN-EXISTS", C025_READINESS_SNIPPETS),
        (args.readiness_script, "M246-C026-RUN-EXISTS", READINESS_SNIPPETS),
        (args.package_json, "M246-C026-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_text_artifact(path=path, exists_check_id=exists_check_id, snippets=snippets)
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








