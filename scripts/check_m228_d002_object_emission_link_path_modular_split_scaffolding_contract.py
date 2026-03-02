#!/usr/bin/env python3
"""Fail-closed validator for M228-D002 object emission/link-path modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d002-object-emission-link-path-modular-split-scaffolding-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_toolchain_runtime_ga_operations_scaffold.h",
    "frontend_anchor_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d002_object_emission_link_path_modular_split_scaffolding_packet.md",
    "d001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_contract_freeze_d001_expectations.md",
    "d001_checker": ROOT / "scripts" / "check_m228_d001_object_emission_link_path_reliability_contract.py",
    "d001_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_d001_object_emission_link_path_reliability_contract.py",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_modular_split_scaffolding_d002_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-D002-SCA-01", "struct Objc3ToolchainRuntimeGaOperationsScaffold {"),
        ("M228-D002-SCA-02", "bool compile_route_ready = false;"),
        ("M228-D002-SCA-03", "bool modular_split_ready = false;"),
        ("M228-D002-SCA-04", "inline std::string BuildObjc3ToolchainRuntimeGaOperationsScaffoldKey("),
        ("M228-D002-SCA-05", "inline Objc3ToolchainRuntimeGaOperationsScaffold BuildObjc3ToolchainRuntimeGaOperationsScaffold("),
        ("M228-D002-SCA-06", "scaffold.compile_route_ready ="),
        ("M228-D002-SCA-07", "scaffold.modular_split_ready = scaffold.compile_route_ready &&"),
        ("M228-D002-SCA-08", 'scaffold.failure_reason = "llvm-direct backend unavailable in this build";'),
        ("M228-D002-SCA-09", 'scaffold.failure_reason = "toolchain/runtime compile route is not ready";'),
        ("M228-D002-SCA-10", "inline bool IsObjc3ToolchainRuntimeGaOperationsScaffoldReady("),
        (
            "M228-D002-SCA-11",
            'reason = scaffold.failure_reason.empty() ? "toolchain/runtime ga operations scaffold not ready"',
        ),
    ),
    "frontend_anchor_source": (
        ("M228-D002-ANC-01", '#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"'),
        (
            "M228-D002-ANC-02",
            "const Objc3ToolchainRuntimeGaOperationsScaffold toolchain_runtime_ga_operations_scaffold =",
        ),
        ("M228-D002-ANC-03", "BuildObjc3ToolchainRuntimeGaOperationsScaffold("),
        ("M228-D002-ANC-04", "IsObjc3ToolchainRuntimeGaOperationsScaffoldReady("),
        ("M228-D002-ANC-05", "toolchain/runtime readiness scaffold fail-closed: "),
        (
            "M228-D002-ANC-06",
            "compile_status = RunIRCompile(std::filesystem::path(options->clang_path), ir_out, object_out);",
        ),
        (
            "M228-D002-ANC-07",
            "compile_status = RunIRCompileLLVMDirect(std::filesystem::path(options->llc_path), ir_out, object_out, backend_error);",
        ),
    ),
    "architecture_doc": (
        (
            "M228-D002-ARCH-01",
            "M228 lane-D D002 modular split scaffolding anchors toolchain/runtime",
        ),
        (
            "M228-D002-ARCH-02",
            "`io/objc3_toolchain_runtime_ga_operations_scaffold.h`",
        ),
    ),
    "lowering_spec": (
        (
            "M228-D002-SPC-01",
            "toolchain/runtime modular split scaffolding shall synthesize deterministic",
        ),
    ),
    "metadata_spec": (
        (
            "M228-D002-META-01",
            "deterministic toolchain/runtime modular split scaffold keys for backend",
        ),
    ),
    "package_json": (
        (
            "M228-D002-CFG-01",
            '"check:objc3c:m228-d002-object-emission-link-path-modular-split-scaffolding-contract"',
        ),
        (
            "M228-D002-CFG-02",
            '"test:tooling:m228-d002-object-emission-link-path-modular-split-scaffolding-contract"',
        ),
        (
            "M228-D002-CFG-03",
            '"check:objc3c:m228-d002-lane-d-readiness"',
        ),
        (
            "M228-D002-CFG-04",
            '"check:objc3c:m228-d002-lane-d-readiness": "npm run check:objc3c:m228-d001-lane-d-readiness && npm run check:objc3c:m228-d002-object-emission-link-path-modular-split-scaffolding-contract && npm run test:tooling:m228-d002-object-emission-link-path-modular-split-scaffolding-contract"',
        ),
        (
            "M228-D002-CFG-05",
            '"compile:objc3c":',
        ),
        (
            "M228-D002-CFG-06",
            '"proof:objc3c":',
        ),
        (
            "M228-D002-CFG-07",
            '"test:objc3c:execution-replay-proof":',
        ),
        (
            "M228-D002-CFG-08",
            '"test:objc3c:perf-budget":',
        ),
    ),
    "packet_doc": (
        (
            "M228-D002-PKT-01",
            "# M228-D002 Object Emission and Link Path Reliability Modular Split/Scaffolding Packet",
        ),
        (
            "M228-D002-PKT-02",
            "Packet: `M228-D002`",
        ),
        (
            "M228-D002-PKT-03",
            "Dependencies: `M228-D001`",
        ),
        (
            "M228-D002-PKT-04",
            "scripts/check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py",
        ),
        (
            "M228-D002-PKT-05",
            "tests/tooling/test_check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py",
        ),
        (
            "M228-D002-PKT-06",
            "`check:objc3c:m228-d002-lane-d-readiness`",
        ),
        (
            "M228-D002-PKT-07",
            "`proof:objc3c`",
        ),
    ),
    "d001_contract_doc": (
        (
            "M228-D002-D001-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-freeze/m228-d001-v1`",
        ),
    ),
    "d001_checker": (
        (
            "M228-D002-D001-02",
            'MODE = "m228-d001-object-emission-link-path-reliability-freeze-contract-v1"',
        ),
    ),
    "d001_tooling_test": (
        (
            "M228-D002-D001-03",
            "check_m228_d001_object_emission_link_path_reliability_contract",
        ),
    ),
    "contract_doc": (
        (
            "M228-D002-DOC-01",
            "Contract ID: `objc3c-object-emission-link-path-modular-split-scaffolding/m228-d002-v1`",
        ),
        ("M228-D002-DOC-02", "BuildObjc3ToolchainRuntimeGaOperationsScaffold"),
        (
            "M228-D002-DOC-03",
            "Dependencies: `M228-D001`",
        ),
        (
            "M228-D002-DOC-04",
            "spec/planning/compiler/m228/m228_d002_object_emission_link_path_modular_split_scaffolding_packet.md",
        ),
        (
            "M228-D002-DOC-05",
            "python scripts/check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py",
        ),
        (
            "M228-D002-DOC-06",
            "python -m pytest tests/tooling/test_check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py -q",
        ),
        ("M228-D002-DOC-07", "npm run check:objc3c:m228-d002-lane-d-readiness"),
        (
            "M228-D002-DOC-08",
            "tmp/reports/m228/M228-D002/object_emission_link_path_modular_split_scaffolding_contract_summary.json",
        ),
        (
            "M228-D002-DOC-09",
            "`proof:objc3c`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-D002-FORB-01", "scaffold.compile_route_ready = true;"),
        ("M228-D002-FORB-02", "scaffold.modular_split_ready = true;"),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m228/M228-D002/object_emission_link_path_modular_split_scaffolding_contract_summary.json"
        ),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
