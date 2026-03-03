#!/usr/bin/env python3
"""Fail-closed validator for M228-C009 IR emission conformance-matrix implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ir_emission_core_feature_implementation_surface.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "c008_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_recovery_determinism_hardening_c008_expectations.md",
    "c008_checker": ROOT
    / "scripts"
    / "check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py",
    "c008_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py",
    "c008_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c008_ir_emission_completeness_recovery_determinism_hardening_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_conformance_matrix_implementation_c009_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c009_ir_emission_completeness_conformance_matrix_implementation_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-C009-SUR-01", "bool pass_graph_conformance_matrix_ready = false;"),
        ("M228-C009-SUR-02", "bool parse_artifact_conformance_matrix_consistent = false;"),
        ("M228-C009-SUR-03", "bool conformance_matrix_consistent = false;"),
        ("M228-C009-SUR-04", "bool conformance_matrix_key_transport_ready = false;"),
        ("M228-C009-SUR-05", "bool core_feature_conformance_matrix_ready = false;"),
        ("M228-C009-SUR-06", "std::string pass_graph_conformance_matrix_key;"),
        ("M228-C009-SUR-07", "std::string parse_artifact_conformance_matrix_key;"),
        ("M228-C009-SUR-08", "std::string conformance_matrix_key;"),
        ("M228-C009-SUR-09", "std::string conformance_matrix_failure_reason;"),
        ("M228-C009-SUR-10", "BuildObjc3IREmissionCoreFeatureConformanceMatrixKey("),
        ("M228-C009-SUR-11", "surface.pass_graph_conformance_matrix_ready ="),
        ("M228-C009-SUR-12", "surface.parse_artifact_conformance_matrix_consistent ="),
        ("M228-C009-SUR-13", "surface.conformance_matrix_consistent ="),
        ("M228-C009-SUR-14", "surface.conformance_matrix_key_transport_ready ="),
        ("M228-C009-SUR-15", "surface.core_feature_conformance_matrix_ready ="),
        ("M228-C009-SUR-16", "surface.conformance_matrix_key ="),
        (
            "M228-C009-SUR-17",
            "IR emission core feature parse artifact conformance matrix is inconsistent",
        ),
        ("M228-C009-SUR-18", "IR emission core feature conformance matrix is inconsistent"),
        (
            "M228-C009-SUR-19",
            "IR emission core feature conformance matrix key transport is not ready",
        ),
        ("M228-C009-SUR-20", "inline bool IsObjc3IREmissionCoreFeatureConformanceMatrixReady("),
        ("M228-C009-SUR-21", "!surface.conformance_matrix_key.empty())"),
    ),
    "artifacts_source": (
        ("M228-C009-ART-01", "IsObjc3IREmissionCoreFeatureConformanceMatrixReady("),
        ("M228-C009-ART-02", '"O3L320"'),
        (
            "M228-C009-ART-03",
            "IR emission core feature conformance matrix check failed",
        ),
        (
            "M228-C009-ART-04",
            "ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_ready =",
        ),
        (
            "M228-C009-ART-05",
            "ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_key =",
        ),
    ),
    "ir_header": (
        ("M228-C009-IRH-01", "bool ir_emission_core_feature_conformance_matrix_ready = false;"),
        ("M228-C009-IRH-02", "std::string ir_emission_core_feature_conformance_matrix_key;"),
    ),
    "ir_source": (
        ("M228-C009-IRS-01", 'out << "; ir_emission_core_feature_conformance_matrix = "'),
        (
            "M228-C009-IRS-02",
            'out << "; ir_emission_core_feature_conformance_matrix_ready = "',
        ),
    ),
    "architecture_doc": (
        (
            "M228-C009-ARC-01",
            "M228 lane-C C009 conformance matrix implementation anchors deterministic",
        ),
        ("M228-C009-ARC-02", "(`conformance_matrix_*`)"),
    ),
    "lowering_spec": (
        (
            "M228-C009-SPC-01",
            "IR-emission conformance matrix implementation shall remain deterministic",
        ),
    ),
    "metadata_spec": (
        (
            "M228-C009-META-01",
            "deterministic IR-emission conformance matrix implementation",
        ),
    ),
    "package_json": (
        (
            "M228-C009-CFG-01",
            '"check:objc3c:m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract"',
        ),
        (
            "M228-C009-CFG-02",
            '"check:objc3c:m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract"',
        ),
        (
            "M228-C009-CFG-03",
            '"test:tooling:m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract"',
        ),
        ("M228-C009-CFG-04", '"check:objc3c:m228-c009-lane-c-readiness"'),
        (
            "M228-C009-CFG-05",
            "npm run check:objc3c:m228-c008-lane-c-readiness && npm run check:objc3c:m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract",
        ),
    ),
    "c008_contract_doc": (
        (
            "M228-C009-DEP-01",
            "Contract ID: `objc3c-ir-emission-completeness-recovery-determinism-hardening/m228-c008-v1`",
        ),
    ),
    "c008_checker": (
        (
            "M228-C009-DEP-02",
            'MODE = "m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract-v1"',
        ),
    ),
    "c008_tooling_test": (
        (
            "M228-C009-DEP-03",
            "check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract",
        ),
    ),
    "c008_packet_doc": (
        ("M228-C009-DEP-04", "Packet: `M228-C008`"),
        ("M228-C009-DEP-05", "Dependencies: `M228-C007`"),
    ),
    "contract_doc": (
        (
            "M228-C009-DOC-01",
            "Contract ID: `objc3c-ir-emission-completeness-conformance-matrix-implementation/m228-c009-v1`",
        ),
        ("M228-C009-DOC-02", "Dependencies: `M228-C008`"),
        ("M228-C009-DOC-03", "BuildObjc3IREmissionCoreFeatureConformanceMatrixKey"),
        ("M228-C009-DOC-04", "IsObjc3IREmissionCoreFeatureConformanceMatrixReady"),
        ("M228-C009-DOC-05", "O3L320"),
        (
            "M228-C009-DOC-06",
            "scripts/check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py",
        ),
        (
            "M228-C009-DOC-07",
            "tests/tooling/test_check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py",
        ),
        (
            "M228-C009-DOC-08",
            "spec/planning/compiler/m228/m228_c009_ir_emission_completeness_conformance_matrix_implementation_packet.md",
        ),
        (
            "M228-C009-DOC-09",
            "tmp/reports/m228/M228-C009/ir_emission_completeness_conformance_matrix_implementation_contract_summary.json",
        ),
        ("M228-C009-DOC-10", "Shared-file deltas required for full lane-C readiness"),
        ("M228-C009-DOC-11", "package.json"),
        ("M228-C009-DOC-12", "native/objc3c/src/ARCHITECTURE.md"),
        ("M228-C009-DOC-13", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-C009-DOC-14", "spec/MODULE_METADATA_AND_ABI_TABLES.md"),
    ),
    "packet_doc": (
        (
            "M228-C009-PKT-01",
            "# M228-C009 IR Emission Completeness Conformance Matrix Implementation Packet",
        ),
        ("M228-C009-PKT-02", "Packet: `M228-C009`"),
        ("M228-C009-PKT-03", "Milestone: `M228`"),
        ("M228-C009-PKT-04", "Dependencies: `M228-C008`"),
        (
            "M228-C009-PKT-05",
            "docs/contracts/m228_ir_emission_completeness_conformance_matrix_implementation_c009_expectations.md",
        ),
        (
            "M228-C009-PKT-06",
            "scripts/check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py",
        ),
        (
            "M228-C009-PKT-07",
            "tests/tooling/test_check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py",
        ),
        (
            "M228-C009-PKT-08",
            "tmp/reports/m228/M228-C009/ir_emission_completeness_conformance_matrix_implementation_contract_summary.json",
        ),
        ("M228-C009-PKT-09", "conformance matrix implementation"),
        ("M228-C009-PKT-10", "Shared-file deltas required for full lane-C readiness"),
        (
            "M228-C009-PKT-11",
            "python scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        (
            "M228-C009-FORB-01",
            "surface.core_feature_conformance_matrix_ready = true;",
        ),
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
            "tmp/reports/m228/M228-C009/"
            "ir_emission_completeness_conformance_matrix_implementation_contract_summary.json"
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
