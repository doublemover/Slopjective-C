#!/usr/bin/env python3
"""Fail-closed validator for M228-D009 object emission/link-path conformance matrix implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d009-object-emission-link-path-reliability-conformance-matrix-implementation-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "d008_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_recovery_determinism_hardening_d008_expectations.md",
    "d008_checker": ROOT
    / "scripts"
    / "check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py",
    "d008_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py",
    "d008_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_conformance_matrix_implementation_d009_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D009-SUR-01", "bool conformance_matrix_consistent = false;"),
        ("M228-D009-SUR-02", "bool conformance_matrix_ready = false;"),
        ("M228-D009-SUR-03", "bool conformance_matrix_key_ready = false;"),
        ("M228-D009-SUR-04", "std::string conformance_matrix_key;"),
        (
            "M228-D009-SUR-05",
            "BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(",
        ),
        ("M228-D009-SUR-06", "surface.conformance_matrix_consistent ="),
        ("M228-D009-SUR-07", "surface.conformance_matrix_ready ="),
        ("M228-D009-SUR-08", "surface.conformance_matrix_key ="),
        ("M228-D009-SUR-09", "surface.conformance_matrix_key_ready ="),
        (
            "M228-D009-SUR-10",
            "surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.conformance_matrix_ready;",
        ),
        (
            "M228-D009-SUR-11",
            "surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.conformance_matrix_key_ready;",
        ),
        ("M228-D009-SUR-12", ";conformance_matrix_consistent="),
        ("M228-D009-SUR-13", ";conformance_matrix_ready="),
        ("M228-D009-SUR-14", ";conformance_matrix_key_ready="),
        ("M228-D009-SUR-15", "toolchain/runtime conformance matrix is inconsistent"),
        ("M228-D009-SUR-16", "toolchain/runtime conformance matrix is not ready"),
        ("M228-D009-SUR-17", "toolchain/runtime conformance matrix key is not ready"),
        (
            "M228-D009-SUR-18",
            "inline bool IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady(",
        ),
        (
            "M228-D009-SUR-19",
            "surface.conformance_matrix_key.find(\";recovery_determinism_key_ready=true\") != std::string::npos;",
        ),
    ),
    "architecture_doc": (
        (
            "M228-D009-ARC-01",
            "M228 lane-D D009 conformance matrix implementation anchors deterministic",
        ),
        ("M228-D009-ARC-02", "(`conformance_matrix_*`)"),
    ),
    "lowering_spec": (
        (
            "M228-D009-SPC-01",
            "toolchain/runtime conformance matrix implementation shall remain",
        ),
    ),
    "metadata_spec": (
        (
            "M228-D009-META-01",
            "deterministic lane-D toolchain/runtime conformance matrix",
        ),
    ),
    "package_json": (
        (
            "M228-D009-CFG-01",
            '"check:objc3c:m228-d008-object-emission-link-path-reliability-recovery-determinism-hardening-contract"',
        ),
        (
            "M228-D009-CFG-02",
            '"check:objc3c:m228-d009-object-emission-link-path-reliability-conformance-matrix-implementation-contract"',
        ),
        (
            "M228-D009-CFG-03",
            '"test:tooling:m228-d009-object-emission-link-path-reliability-conformance-matrix-implementation-contract"',
        ),
        ("M228-D009-CFG-04", '"check:objc3c:m228-d009-lane-d-readiness"'),
        (
            "M228-D009-CFG-05",
            "npm run check:objc3c:m228-d008-lane-d-readiness && npm run check:objc3c:m228-d009-object-emission-link-path-reliability-conformance-matrix-implementation-contract",
        ),
    ),
    "d008_contract_doc": (
        (
            "M228-D009-DEP-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-recovery-determinism-hardening/m228-d008-v1`",
        ),
    ),
    "d008_checker": (
        (
            "M228-D009-DEP-02",
            'MODE = "m228-d008-object-emission-link-path-reliability-recovery-determinism-hardening-contract-v1"',
        ),
    ),
    "d008_tooling_test": (
        (
            "M228-D009-DEP-03",
            "check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract",
        ),
    ),
    "d008_packet_doc": (
        ("M228-D009-DEP-04", "Packet: `M228-D008`"),
        ("M228-D009-DEP-05", "Dependencies: `M228-D007`"),
    ),
    "contract_doc": (
        (
            "M228-D009-DOC-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-conformance-matrix-implementation/m228-d009-v1`",
        ),
        ("M228-D009-DOC-02", "Dependencies: `M228-D008`"),
        ("M228-D009-DOC-03", "conformance_matrix_consistent"),
        ("M228-D009-DOC-04", "conformance_matrix_ready"),
        ("M228-D009-DOC-05", "conformance_matrix_key_ready"),
        ("M228-D009-DOC-06", "BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey"),
        (
            "M228-D009-DOC-07",
            "IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady",
        ),
        (
            "M228-D009-DOC-08",
            "scripts/check_m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_contract.py",
        ),
        (
            "M228-D009-DOC-09",
            "tests/tooling/test_check_m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_contract.py",
        ),
        (
            "M228-D009-DOC-10",
            "tmp/reports/m228/M228-D009/object_emission_link_path_reliability_conformance_matrix_implementation_contract_summary.json",
        ),
        ("M228-D009-DOC-11", "Shared-file deltas required for full lane-D readiness"),
        ("M228-D009-DOC-12", "package.json"),
        ("M228-D009-DOC-13", "native/objc3c/src/ARCHITECTURE.md"),
        ("M228-D009-DOC-14", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-D009-DOC-15", "spec/MODULE_METADATA_AND_ABI_TABLES.md"),
    ),
    "planning_packet": (
        (
            "M228-D009-PKT-01",
            "# M228-D009 Object Emission and Link Path Reliability Conformance Matrix Implementation Packet",
        ),
        ("M228-D009-PKT-02", "Packet: `M228-D009`"),
        ("M228-D009-PKT-03", "Milestone: `M228`"),
        ("M228-D009-PKT-04", "Dependencies: `M228-D008`"),
        (
            "M228-D009-PKT-05",
            "docs/contracts/m228_object_emission_link_path_reliability_conformance_matrix_implementation_d009_expectations.md",
        ),
        (
            "M228-D009-PKT-06",
            "scripts/check_m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_contract.py",
        ),
        (
            "M228-D009-PKT-07",
            "tests/tooling/test_check_m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_contract.py",
        ),
        (
            "M228-D009-PKT-08",
            "tmp/reports/m228/M228-D009/object_emission_link_path_reliability_conformance_matrix_implementation_contract_summary.json",
        ),
        ("M228-D009-PKT-09", "conformance matrix implementation"),
        ("M228-D009-PKT-10", "Shared-file deltas required for full lane-D readiness"),
        (
            "M228-D009-PKT-11",
            "python scripts/check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D009-FORB-01", "surface.conformance_matrix_ready = true;"),
        ("M228-D009-FORB-02", "surface.conformance_matrix_consistent = true;"),
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
            "tmp/reports/m228/M228-D009/"
            "object_emission_link_path_reliability_conformance_matrix_implementation_contract_summary.json"
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
