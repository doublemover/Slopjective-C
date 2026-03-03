#!/usr/bin/env python3
"""Fail-closed validator for M228-C008 IR emission recovery/determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract-v1"

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
    "c007_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_diagnostics_hardening_c007_expectations.md",
    "c007_checker": ROOT
    / "scripts"
    / "check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py",
    "c007_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py",
    "c007_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c007_ir_emission_completeness_diagnostics_hardening_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_recovery_determinism_hardening_c008_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c008_ir_emission_completeness_recovery_determinism_hardening_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-C008-SUR-01", "bool pass_graph_recovery_determinism_ready = false;"),
        (
            "M228-C008-SUR-02",
            "bool parse_artifact_recovery_determinism_hardening_consistent = false;",
        ),
        ("M228-C008-SUR-03", "bool recovery_determinism_consistent = false;"),
        ("M228-C008-SUR-04", "bool recovery_determinism_key_transport_ready = false;"),
        ("M228-C008-SUR-05", "bool core_feature_recovery_determinism_ready = false;"),
        ("M228-C008-SUR-06", "std::string pass_graph_recovery_determinism_key;"),
        (
            "M228-C008-SUR-07",
            "std::string parse_artifact_recovery_determinism_hardening_key;",
        ),
        ("M228-C008-SUR-08", "std::string recovery_determinism_key;"),
        ("M228-C008-SUR-09", "std::string recovery_determinism_failure_reason;"),
        (
            "M228-C008-SUR-10",
            "BuildObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningKey(",
        ),
        ("M228-C008-SUR-11", "surface.pass_graph_recovery_determinism_ready ="),
        (
            "M228-C008-SUR-12",
            "surface.parse_artifact_recovery_determinism_hardening_consistent =",
        ),
        ("M228-C008-SUR-13", "surface.recovery_determinism_consistent ="),
        ("M228-C008-SUR-14", "surface.recovery_determinism_key_transport_ready ="),
        ("M228-C008-SUR-15", "surface.core_feature_recovery_determinism_ready ="),
        ("M228-C008-SUR-16", "surface.recovery_determinism_key ="),
        (
            "M228-C008-SUR-17",
            "IR emission core feature parse artifact recovery determinism hardening is inconsistent",
        ),
        (
            "M228-C008-SUR-18",
            "IR emission core feature recovery determinism hardening is inconsistent",
        ),
        (
            "M228-C008-SUR-19",
            "IR emission core feature recovery determinism hardening key transport is not ready",
        ),
        (
            "M228-C008-SUR-20",
            "inline bool IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(",
        ),
        ("M228-C008-SUR-21", "!surface.recovery_determinism_key.empty())"),
    ),
    "artifacts_source": (
        (
            "M228-C008-ART-01",
            "IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(",
        ),
        ("M228-C008-ART-02", '"O3L319"'),
        (
            "M228-C008-ART-03",
            "IR emission core feature recovery determinism hardening check failed",
        ),
        (
            "M228-C008-ART-04",
            "ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_ready =",
        ),
        (
            "M228-C008-ART-05",
            "ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_key =",
        ),
    ),
    "ir_header": (
        (
            "M228-C008-IRH-01",
            "bool ir_emission_core_feature_recovery_determinism_ready = false;",
        ),
        (
            "M228-C008-IRH-02",
            "std::string ir_emission_core_feature_recovery_determinism_key;",
        ),
    ),
    "ir_source": (
        (
            "M228-C008-IRS-01",
            'out << "; ir_emission_core_feature_recovery_determinism = "',
        ),
        (
            "M228-C008-IRS-02",
            'out << "; ir_emission_core_feature_recovery_determinism_ready = "',
        ),
    ),
    "architecture_doc": (
        (
            "M228-C008-ARC-01",
            "M228 lane-C C008 recovery and determinism hardening anchors deterministic",
        ),
        ("M228-C008-ARC-02", "recovery_determinism_*"),
    ),
    "lowering_spec": (
        (
            "M228-C008-SPC-01",
            "IR-emission recovery and determinism hardening shall remain deterministic",
        ),
    ),
    "metadata_spec": (
        (
            "M228-C008-META-01",
            "deterministic IR-emission recovery and determinism hardening",
        ),
    ),
    "package_json": (
        (
            "M228-C008-CFG-01",
            '"check:objc3c:m228-c007-ir-emission-completeness-diagnostics-hardening-contract"',
        ),
        (
            "M228-C008-CFG-02",
            '"check:objc3c:m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract"',
        ),
        (
            "M228-C008-CFG-03",
            '"test:tooling:m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract"',
        ),
        ("M228-C008-CFG-04", '"check:objc3c:m228-c008-lane-c-readiness"'),
        (
            "M228-C008-CFG-05",
            "npm run check:objc3c:m228-c007-lane-c-readiness && npm run check:objc3c:m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract",
        ),
    ),
    "c007_contract_doc": (
        (
            "M228-C008-DEP-01",
            "Contract ID: `objc3c-ir-emission-completeness-diagnostics-hardening/m228-c007-v1`",
        ),
    ),
    "c007_checker": (
        (
            "M228-C008-DEP-02",
            'MODE = "m228-c007-ir-emission-completeness-diagnostics-hardening-contract-v1"',
        ),
    ),
    "c007_tooling_test": (
        (
            "M228-C008-DEP-03",
            "check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract",
        ),
    ),
    "c007_packet_doc": (
        ("M228-C008-DEP-04", "Packet: `M228-C007`"),
        ("M228-C008-DEP-05", "Dependencies: `M228-C006`"),
    ),
    "contract_doc": (
        (
            "M228-C008-DOC-01",
            "Contract ID: `objc3c-ir-emission-completeness-recovery-determinism-hardening/m228-c008-v1`",
        ),
        (
            "M228-C008-DOC-02",
            "BuildObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningKey",
        ),
        (
            "M228-C008-DOC-03",
            "IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady",
        ),
        ("M228-C008-DOC-04", "O3L319"),
        ("M228-C008-DOC-05", "Dependencies: `M228-C007`"),
        (
            "M228-C008-DOC-06",
            "scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py",
        ),
        (
            "M228-C008-DOC-07",
            "tests/tooling/test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py",
        ),
        (
            "M228-C008-DOC-08",
            "spec/planning/compiler/m228/m228_c008_ir_emission_completeness_recovery_determinism_hardening_packet.md",
        ),
        (
            "M228-C008-DOC-09",
            "tmp/reports/m228/M228-C008/ir_emission_completeness_recovery_determinism_hardening_contract_summary.json",
        ),
        ("M228-C008-DOC-10", "package.json"),
        ("M228-C008-DOC-11", "native/objc3c/src/ARCHITECTURE.md"),
        ("M228-C008-DOC-12", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-C008-DOC-13", "spec/MODULE_METADATA_AND_ABI_TABLES.md"),
    ),
    "packet_doc": (
        (
            "M228-C008-PKT-01",
            "# M228-C008 IR Emission Completeness Recovery and Determinism Hardening Packet",
        ),
        ("M228-C008-PKT-02", "Packet: `M228-C008`"),
        ("M228-C008-PKT-03", "Milestone: `M228`"),
        ("M228-C008-PKT-04", "Dependencies: `M228-C007`"),
        (
            "M228-C008-PKT-05",
            "docs/contracts/m228_ir_emission_completeness_recovery_determinism_hardening_c008_expectations.md",
        ),
        (
            "M228-C008-PKT-06",
            "scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py",
        ),
        (
            "M228-C008-PKT-07",
            "tests/tooling/test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py",
        ),
        (
            "M228-C008-PKT-08",
            "tmp/reports/m228/M228-C008/ir_emission_completeness_recovery_determinism_hardening_contract_summary.json",
        ),
        ("M228-C008-PKT-09", "Shared-file deltas required for full lane-C readiness"),
        (
            "M228-C008-PKT-10",
            "python scripts/check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        (
            "M228-C008-FORB-01",
            "surface.core_feature_recovery_determinism_ready = true;",
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
            "tmp/reports/m228/M228-C008/"
            "ir_emission_completeness_recovery_determinism_hardening_contract_summary.json"
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
