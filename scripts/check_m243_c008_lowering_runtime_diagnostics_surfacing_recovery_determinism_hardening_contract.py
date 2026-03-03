#!/usr/bin/env python3
"""Fail-closed checker for M243-C008 lowering/runtime diagnostics recovery determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-c008-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_c008_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_packet.md",
    "c007_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_c007_expectations.md",
    "c007_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_packet.md",
    "c007_checker": ROOT
    / "scripts"
    / "check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py",
    "c007_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py",
    "surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "frontend_pipeline": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "frontend_artifacts": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-C008-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening/m243-c008-v1`",
        ),
        ("M243-C008-DOC-02", "Dependencies: `M243-C007`"),
        (
            "M243-C008-DOC-03",
            "Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface",
        ),
        (
            "M243-C008-DOC-04",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(...)",
        ),
        (
            "M243-C008-DOC-05",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(...)",
        ),
        ("M243-C008-DOC-06", "O3L326"),
        ("M243-C008-DOC-07", "npm run check:objc3c:m243-c008-lane-c-readiness"),
        (
            "M243-C008-DOC-08",
            "scripts/check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py",
        ),
        (
            "M243-C008-DOC-09",
            "tests/tooling/test_check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py",
        ),
    ),
    "packet_doc": (
        ("M243-C008-PKT-01", "Packet: `M243-C008`"),
        ("M243-C008-PKT-02", "Dependencies: `M243-C007`"),
        (
            "M243-C008-PKT-03",
            "scripts/check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py",
        ),
        (
            "M243-C008-PKT-04",
            "tests/tooling/test_check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py",
        ),
        ("M243-C008-PKT-05", "check:objc3c:m243-c008-lane-c-readiness"),
        (
            "M243-C008-PKT-06",
            "tmp/reports/m243/M243-C008/lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract_summary.json",
        ),
    ),
    "c007_expectations_doc": (
        (
            "M243-C008-DEP-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-diagnostics-hardening/m243-c007-v1`",
        ),
    ),
    "c007_packet_doc": (
        ("M243-C008-DEP-02", "Packet: `M243-C007`"),
        ("M243-C008-DEP-03", "Dependencies: `M243-C006`"),
    ),
    "c007_checker": (
        (
            "M243-C008-DEP-04",
            'MODE = "m243-c007-lowering-runtime-diagnostics-surfacing-diagnostics-hardening-contract-v1"',
        ),
    ),
    "c007_tooling_test": (
        (
            "M243-C008-DEP-05",
            "check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract",
        ),
    ),
    "surface_header": (
        (
            "M243-C008-SUR-01",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningKey(",
        ),
        (
            "M243-C008-SUR-02",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(",
        ),
        ("M243-C008-SUR-03", "surface.parse_recovery_determinism_consistent ="),
        ("M243-C008-SUR-04", "surface.semantic_recovery_determinism_consistent ="),
        ("M243-C008-SUR-05", "surface.lowering_pipeline_recovery_determinism_ready ="),
        ("M243-C008-SUR-06", "surface.recovery_determinism_consistent ="),
        ("M243-C008-SUR-07", "surface.recovery_determinism_key ="),
        ("M243-C008-SUR-08", "surface.recovery_determinism_ready ="),
        (
            "M243-C008-SUR-09",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(",
        ),
        (
            "M243-C008-SUR-10",
            "lowering/runtime diagnostics recovery/determinism hardening is inconsistent",
        ),
        (
            "M243-C008-SUR-11",
            "lowering/runtime diagnostics recovery/determinism hardening is not ready",
        ),
    ),
    "frontend_types": (
        (
            "M243-C008-TYP-01",
            "struct Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface {",
        ),
        ("M243-C008-TYP-02", "bool recovery_determinism_consistent = false;"),
        ("M243-C008-TYP-03", "bool recovery_determinism_ready = false;"),
        ("M243-C008-TYP-04", "std::string recovery_determinism_key;"),
        (
            "M243-C008-TYP-05",
            "lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface;",
        ),
    ),
    "frontend_pipeline": (
        (
            "M243-C008-PIP-01",
            '#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"',
        ),
        (
            "M243-C008-PIP-02",
            ".lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface =",
        ),
        (
            "M243-C008-PIP-03",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(",
        ),
    ),
    "frontend_artifacts": (
        (
            "M243-C008-ART-01",
            '#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"',
        ),
        (
            "M243-C008-ART-02",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(",
        ),
        ("M243-C008-ART-03", '"O3L326"'),
        (
            "M243-C008-ART-04",
            "lowering/runtime diagnostics surfacing recovery/determinism hardening check failed: ",
        ),
    ),
    "architecture_doc": (
        (
            "M243-C008-ARC-01",
            "M243 lane-C C008 recovery and determinism hardening anchors lowering/runtime diagnostics surfacing",
        ),
    ),
    "lowering_spec": (
        (
            "M243-C008-SPC-01",
            "lowering/runtime diagnostics surfacing recovery and determinism hardening shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M243-C008-META-01",
            "recovery and determinism hardening metadata anchors for `M243-C008` with explicit",
        ),
    ),
    "package_json": (
        (
            "M243-C008-CFG-01",
            '"check:objc3c:m243-c008-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening-contract"',
        ),
        (
            "M243-C008-CFG-02",
            '"test:tooling:m243-c008-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening-contract"',
        ),
        ("M243-C008-CFG-03", '"check:objc3c:m243-c008-lane-c-readiness"'),
        (
            "M243-C008-CFG-04",
            "npm run check:objc3c:m243-c007-lane-c-readiness && npm run check:objc3c:m243-c008-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening-contract && npm run test:tooling:m243-c008-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "surface_header": (
        ("M243-C008-FORB-01", "surface.recovery_determinism_ready = true;"),
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
            "tmp/reports/m243/M243-C008/lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit the summary payload to stdout as canonical JSON.",
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

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
