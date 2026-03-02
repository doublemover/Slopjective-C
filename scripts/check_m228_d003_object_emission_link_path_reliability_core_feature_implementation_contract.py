#!/usr/bin/env python3
"""Fail-closed validator for M228-D003 object emission/link-path core feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d003-object-emission-link-path-reliability-core-feature-implementation-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    "frontend_anchor_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "d001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_contract_freeze_d001_expectations.md",
    "d002_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_modular_split_scaffolding_d002_expectations.md",
    "d001_checker": ROOT / "scripts" / "check_m228_d001_object_emission_link_path_reliability_contract.py",
    "d002_checker": ROOT
    / "scripts"
    / "check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py",
    "d002_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py",
    "d002_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d002_object_emission_link_path_modular_split_scaffolding_packet.md",
    "package_json": ROOT / "package.json",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_core_feature_implementation_d003_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d003_object_emission_link_path_reliability_core_feature_implementation_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D003-SUR-01", "struct Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface {"),
        ("M228-D003-SUR-02", "bool scaffold_ready = false;"),
        ("M228-D003-SUR-03", "bool backend_route_deterministic = false;"),
        ("M228-D003-SUR-04", "bool compile_status_success = false;"),
        ("M228-D003-SUR-05", "bool backend_output_recorded = false;"),
        ("M228-D003-SUR-06", "bool backend_dispatch_consistent = false;"),
        ("M228-D003-SUR-07", "bool core_feature_impl_ready = false;"),
        ("M228-D003-SUR-08", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureKey("),
        ("M228-D003-SUR-09", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface("),
        (
            "M228-D003-SUR-10",
            "surface.backend_dispatch_consistent = surface.compile_status_success && surface.backend_output_recorded;",
        ),
        ("M228-D003-SUR-11", "surface.core_feature_impl_ready ="),
        ("M228-D003-SUR-12", "IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady("),
        (
            "M228-D003-SUR-13",
            'surface.failure_reason = "toolchain/runtime backend object emission command failed";',
        ),
        (
            "M228-D003-SUR-14",
            'surface.failure_reason = "toolchain/runtime core feature implementation is not ready";',
        ),
    ),
    "frontend_anchor_source": (
        ("M228-D003-ANC-01", '#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"'),
        (
            "M228-D003-ANC-02",
            'const std::filesystem::path backend_out = out_dir / (emit_prefix + ".object-backend.txt");',
        ),
        (
            "M228-D003-ANC-03",
            'const std::string backend_text = wants_clang_backend ? "clang\\n" : "llvm-direct\\n";',
        ),
        ("M228-D003-ANC-04", "bool backend_output_recorded = false;"),
        ("M228-D003-ANC-05", "WriteTextFile(backend_out, backend_text, backend_output_error)"),
        (
            "M228-D003-ANC-06",
            "const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface toolchain_runtime_core_feature_surface =",
        ),
        ("M228-D003-ANC-07", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface("),
        ("M228-D003-ANC-08", "IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady("),
        ("M228-D003-ANC-09", "toolchain/runtime core feature fail-closed: "),
    ),
    "architecture_doc": (
        (
            "M228-D003-ARC-01",
            "M228 lane-D D003 core feature implementation anchors toolchain/runtime",
        ),
        (
            "M228-D003-ARC-02",
            "`io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h`",
        ),
    ),
    "lowering_spec": (
        (
            "M228-D003-SPC-01",
            "toolchain/runtime core feature implementation shall remain fail-closed",
        ),
    ),
    "metadata_spec": (
        (
            "M228-D003-META-01",
            "deterministic toolchain/runtime core-feature implementation keys for",
        ),
    ),
    "d001_contract_doc": (
        (
            "M228-D003-DEP-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-freeze/m228-d001-v1`",
        ),
    ),
    "d002_contract_doc": (
        (
            "M228-D003-DEP-02",
            "Contract ID: `objc3c-object-emission-link-path-modular-split-scaffolding/m228-d002-v1`",
        ),
    ),
    "d001_checker": (
        (
            "M228-D003-DEP-03",
            'MODE = "m228-d001-object-emission-link-path-reliability-freeze-contract-v1"',
        ),
    ),
    "d002_checker": (
        (
            "M228-D003-DEP-04",
            'MODE = "m228-d002-object-emission-link-path-modular-split-scaffolding-contract-v1"',
        ),
    ),
    "d002_tooling_test": (
        (
            "M228-D003-DEP-05",
            "check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract",
        ),
    ),
    "d002_packet_doc": (
        ("M228-D003-DEP-06", "Packet: `M228-D002`"),
        ("M228-D003-DEP-07", "Dependencies: `M228-D001`"),
    ),
    "package_json": (
        (
            "M228-D003-CFG-01",
            '"check:objc3c:m228-d003-object-emission-link-path-reliability-core-feature-implementation-contract"',
        ),
        (
            "M228-D003-CFG-02",
            '"test:tooling:m228-d003-object-emission-link-path-reliability-core-feature-implementation-contract"',
        ),
        (
            "M228-D003-CFG-03",
            '"check:objc3c:m228-d003-lane-d-readiness"',
        ),
        (
            "M228-D003-CFG-04",
            "check:objc3c:m228-d002-lane-d-readiness && npm run check:objc3c:m228-d003-object-emission-link-path-reliability-core-feature-implementation-contract && npm run test:tooling:m228-d003-object-emission-link-path-reliability-core-feature-implementation-contract",
        ),
        ("M228-D003-CFG-05", '"compile:objc3c":'),
        ("M228-D003-CFG-06", '"proof:objc3c":'),
        ("M228-D003-CFG-07", '"test:objc3c:execution-replay-proof":'),
        ("M228-D003-CFG-08", '"test:objc3c:perf-budget":'),
    ),
    "contract_doc": (
        (
            "M228-D003-DOC-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-core-feature-implementation/m228-d003-v1`",
        ),
        ("M228-D003-DOC-02", "Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface"),
        ("M228-D003-DOC-03", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface"),
        ("M228-D003-DOC-04", "O3E002"),
        (
            "M228-D003-DOC-05",
            "scripts/check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract.py",
        ),
        (
            "M228-D003-DOC-06",
            "tests/tooling/test_check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract.py",
        ),
        ("M228-D003-DOC-07", "npm run check:objc3c:m228-d003-lane-d-readiness"),
        ("M228-D003-DOC-08", "npm run build:objc3c-native"),
        (
            "M228-D003-DOC-09",
            "tmp/reports/m228/M228-D003/object_emission_link_path_reliability_core_feature_implementation_contract_summary.json",
        ),
        (
            "M228-D003-DOC-10",
            "Code/spec anchors and milestone optimization improvements are mandatory scope",
        ),
        ("M228-D003-DOC-11", "Dependencies: `M228-D002`"),
        (
            "M228-D003-DOC-12",
            "spec/planning/compiler/m228/m228_d003_object_emission_link_path_reliability_core_feature_implementation_packet.md",
        ),
    ),
    "planning_packet": (
        (
            "M228-D003-PKT-01",
            "# M228-D003 Object Emission and Link Path Reliability Core Feature Implementation Packet",
        ),
        ("M228-D003-PKT-02", "Packet: `M228-D003`"),
        ("M228-D003-PKT-03", "Milestone: `M228`"),
        ("M228-D003-PKT-04", "Dependencies: `M228-D002`"),
        (
            "M228-D003-PKT-05",
            "docs/contracts/m228_object_emission_link_path_reliability_core_feature_implementation_d003_expectations.md",
        ),
        (
            "M228-D003-PKT-06",
            "scripts/check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract.py",
        ),
        (
            "M228-D003-PKT-07",
            "tests/tooling/test_check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract.py",
        ),
        ("M228-D003-PKT-08", "check:objc3c:m228-d003-lane-d-readiness"),
        (
            "M228-D003-PKT-09",
            "tmp/reports/m228/M228-D003/object_emission_link_path_reliability_core_feature_implementation_contract_summary.json",
        ),
        ("M228-D003-PKT-10", "code/spec anchors and milestone optimization"),
        ("M228-D003-PKT-11", "improvements as mandatory scope inputs"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D003-FORB-01", "surface.core_feature_impl_ready = true;"),
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
            "tmp/reports/m228/M228-D003/"
            "object_emission_link_path_reliability_core_feature_implementation_contract_summary.json"
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

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
