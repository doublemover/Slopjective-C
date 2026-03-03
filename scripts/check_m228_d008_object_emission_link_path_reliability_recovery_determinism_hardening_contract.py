#!/usr/bin/env python3
"""Fail-closed validator for M228-D008 object emission/link-path recovery and determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d008-object-emission-link-path-reliability-recovery-determinism-hardening-contract-v1"

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
    "d007_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_diagnostics_hardening_d007_expectations.md",
    "d007_checker": ROOT
    / "scripts"
    / "check_m228_d007_object_emission_link_path_reliability_diagnostics_hardening_contract.py",
    "d007_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_d007_object_emission_link_path_reliability_diagnostics_hardening_contract.py",
    "d007_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d007_object_emission_link_path_reliability_diagnostics_hardening_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_recovery_determinism_hardening_d008_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D008-SUR-01", "bool recovery_determinism_consistent = false;"),
        ("M228-D008-SUR-02", "bool recovery_determinism_ready = false;"),
        ("M228-D008-SUR-03", "bool recovery_determinism_key_ready = false;"),
        ("M228-D008-SUR-04", "std::string recovery_determinism_key;"),
        (
            "M228-D008-SUR-05",
            "BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey(",
        ),
        ("M228-D008-SUR-06", "surface.recovery_determinism_consistent ="),
        ("M228-D008-SUR-07", "surface.recovery_determinism_ready ="),
        ("M228-D008-SUR-08", "surface.recovery_determinism_key ="),
        ("M228-D008-SUR-09", "surface.recovery_determinism_key_ready ="),
        (
            "M228-D008-SUR-10",
            "surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.recovery_determinism_ready;",
        ),
        (
            "M228-D008-SUR-11",
            "surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.recovery_determinism_key_ready;",
        ),
        ("M228-D008-SUR-12", ";recovery_determinism_consistent="),
        ("M228-D008-SUR-13", ";recovery_determinism_ready="),
        ("M228-D008-SUR-14", ";recovery_determinism_key_ready="),
        (
            "M228-D008-SUR-15",
            "toolchain/runtime recovery and determinism hardening is inconsistent",
        ),
        (
            "M228-D008-SUR-16",
            "toolchain/runtime recovery and determinism hardening is not ready",
        ),
        (
            "M228-D008-SUR-17",
            "toolchain/runtime recovery and determinism hardening key is not ready",
        ),
        (
            "M228-D008-SUR-18",
            "inline bool IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(",
        ),
        (
            "M228-D008-SUR-19",
            "surface.recovery_determinism_key.find(\";diagnostics_hardening_key_ready=true\") != std::string::npos;",
        ),
    ),
    "architecture_doc": (
        ("M228-D008-ARC-01", "M228 lane-D D008 recovery and determinism hardening anchors deterministic"),
        ("M228-D008-ARC-02", "recovery_determinism_*"),
    ),
    "lowering_spec": (
        (
            "M228-D008-SPC-01",
            "toolchain/runtime recovery and determinism hardening shall remain",
        ),
    ),
    "metadata_spec": (
        (
            "M228-D008-META-01",
            "deterministic lane-D toolchain/runtime recovery and determinism",
        ),
    ),
    "package_json": (
        (
            "M228-D008-CFG-01",
            '"check:objc3c:m228-d007-object-emission-link-path-reliability-diagnostics-hardening-contract"',
        ),
        (
            "M228-D008-CFG-02",
            '"check:objc3c:m228-d008-object-emission-link-path-reliability-recovery-determinism-hardening-contract"',
        ),
        (
            "M228-D008-CFG-03",
            '"test:tooling:m228-d008-object-emission-link-path-reliability-recovery-determinism-hardening-contract"',
        ),
        ("M228-D008-CFG-04", '"check:objc3c:m228-d008-lane-d-readiness"'),
        (
            "M228-D008-CFG-05",
            "npm run check:objc3c:m228-d007-lane-d-readiness && npm run check:objc3c:m228-d008-object-emission-link-path-reliability-recovery-determinism-hardening-contract",
        ),
    ),
    "d007_contract_doc": (
        (
            "M228-D008-DEP-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-diagnostics-hardening/m228-d007-v1`",
        ),
    ),
    "d007_checker": (
        (
            "M228-D008-DEP-02",
            'MODE = "m228-d007-object-emission-link-path-reliability-diagnostics-hardening-contract-v1"',
        ),
    ),
    "d007_tooling_test": (
        (
            "M228-D008-DEP-03",
            "check_m228_d007_object_emission_link_path_reliability_diagnostics_hardening_contract",
        ),
    ),
    "d007_packet_doc": (
        ("M228-D008-DEP-04", "Packet: `M228-D007`"),
        ("M228-D008-DEP-05", "Dependencies: `M228-D006`"),
    ),
    "contract_doc": (
        (
            "M228-D008-DOC-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-recovery-determinism-hardening/m228-d008-v1`",
        ),
        ("M228-D008-DOC-02", "Dependencies: `M228-D007`"),
        ("M228-D008-DOC-03", "recovery_determinism_consistent"),
        ("M228-D008-DOC-04", "recovery_determinism_ready"),
        ("M228-D008-DOC-05", "recovery_determinism_key_ready"),
        (
            "M228-D008-DOC-06",
            "BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey",
        ),
        (
            "M228-D008-DOC-07",
            "IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady",
        ),
        (
            "M228-D008-DOC-08",
            "scripts/check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py",
        ),
        (
            "M228-D008-DOC-09",
            "tests/tooling/test_check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py",
        ),
        (
            "M228-D008-DOC-10",
            "tmp/reports/m228/M228-D008/object_emission_link_path_reliability_recovery_determinism_hardening_contract_summary.json",
        ),
        ("M228-D008-DOC-11", "Shared-file deltas required for full lane-D readiness"),
        ("M228-D008-DOC-12", "package.json"),
        ("M228-D008-DOC-13", "native/objc3c/src/ARCHITECTURE.md"),
        ("M228-D008-DOC-14", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-D008-DOC-15", "spec/MODULE_METADATA_AND_ABI_TABLES.md"),
    ),
    "planning_packet": (
        (
            "M228-D008-PKT-01",
            "# M228-D008 Object Emission and Link Path Reliability Recovery and Determinism Hardening Packet",
        ),
        ("M228-D008-PKT-02", "Packet: `M228-D008`"),
        ("M228-D008-PKT-03", "Milestone: `M228`"),
        ("M228-D008-PKT-04", "Dependencies: `M228-D007`"),
        (
            "M228-D008-PKT-05",
            "docs/contracts/m228_object_emission_link_path_reliability_recovery_determinism_hardening_d008_expectations.md",
        ),
        (
            "M228-D008-PKT-06",
            "scripts/check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py",
        ),
        (
            "M228-D008-PKT-07",
            "tests/tooling/test_check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py",
        ),
        (
            "M228-D008-PKT-08",
            "tmp/reports/m228/M228-D008/object_emission_link_path_reliability_recovery_determinism_hardening_contract_summary.json",
        ),
        ("M228-D008-PKT-09", "recovery and determinism hardening"),
        ("M228-D008-PKT-10", "Shared-file deltas required for full lane-D readiness"),
        (
            "M228-D008-PKT-11",
            "python scripts/check_m228_d007_object_emission_link_path_reliability_diagnostics_hardening_contract.py",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D008-FORB-01", "surface.recovery_determinism_ready = true;"),
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
            "tmp/reports/m228/M228-D008/"
            "object_emission_link_path_reliability_recovery_determinism_hardening_contract_summary.json"
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
