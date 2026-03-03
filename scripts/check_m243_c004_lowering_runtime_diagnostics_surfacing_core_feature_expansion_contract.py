#!/usr/bin/env python3
"""Fail-closed checker for M243-C004 lowering/runtime diagnostics expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-c004-lowering-runtime-diagnostics-surfacing-core-feature-expansion-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_core_feature_expansion_c004_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_packet.md",
    "c003_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_core_feature_implementation_c003_expectations.md",
    "surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h",
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
            "M243-C004-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-core-feature-expansion/m243-c004-v1`",
        ),
        ("M243-C004-DOC-02", "Dependencies: `M243-C003`"),
        ("M243-C004-DOC-03", "npm run check:objc3c:m243-c004-lane-c-readiness"),
        (
            "M243-C004-DOC-04",
            "`Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface`",
        ),
        (
            "M243-C004-DOC-05",
            "`native/objc3c/src/pipeline/objc3_frontend_types.h`",
        ),
        (
            "M243-C004-DOC-06",
            "`native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h`",
        ),
    ),
    "packet_doc": (
        ("M243-C004-PKT-01", "Packet: `M243-C004`"),
        ("M243-C004-PKT-02", "Dependencies: `M243-C003`"),
        (
            "M243-C004-PKT-03",
            "scripts/check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py",
        ),
        ("M243-C004-PKT-04", "Surface type anchor:"),
        ("M243-C004-PKT-05", "Surface derivation anchor:"),
    ),
    "c003_expectations_doc": (
        (
            "M243-C004-DEP-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-core-feature-implementation/m243-c003-v1`",
        ),
    ),
    "surface_header": (
        (
            "M243-C004-SUR-01",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionKey(",
        ),
        (
            "M243-C004-SUR-02",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(",
        ),
        ("M243-C004-SUR-03", "surface.diagnostics_hardening_key_consistent ="),
        ("M243-C004-SUR-04", "surface.diagnostics_payload_accounting_consistent ="),
        ("M243-C004-SUR-05", "surface.expansion_replay_keys_ready ="),
        (
            "M243-C004-SUR-06",
            "surface.core_feature_expansion_ready =",
        ),
        (
            "M243-C004-SUR-07",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(",
        ),
    ),
    "frontend_types": (
        (
            "M243-C004-TYP-01",
            "struct Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface {",
        ),
        ("M243-C004-TYP-02", "bool diagnostics_hardening_key_consistent = false;"),
        ("M243-C004-TYP-03", "bool diagnostics_payload_accounting_consistent = false;"),
        ("M243-C004-TYP-04", "bool expansion_replay_keys_ready = false;"),
        ("M243-C004-TYP-05", "bool core_feature_expansion_ready = false;"),
        (
            "M243-C004-TYP-06",
            "Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface",
        ),
        (
            "M243-C004-TYP-07",
            "lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface;",
        ),
    ),
    "frontend_pipeline": (
        (
            "M243-C004-PIP-01",
            '#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"',
        ),
        (
            "M243-C004-PIP-02",
            "result.lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface =",
        ),
    ),
    "frontend_artifacts": (
        (
            "M243-C004-ART-01",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(",
        ),
        ("M243-C004-ART-02", '"O3L322"'),
    ),
    "architecture_doc": (
        ("M243-C004-ARC-01", "M243 lane-C C004 core feature expansion"),
    ),
    "lowering_spec": (
        (
            "M243-C004-SPC-01",
            "lowering/runtime diagnostics surfacing core feature expansion",
        ),
    ),
    "metadata_spec": (
        (
            "M243-C004-META-01",
            "deterministic lane-C lowering/runtime diagnostics surfacing core feature",
        ),
        ("M243-C004-META-02", "`M243-C004`"),
    ),
    "package_json": (
        (
            "M243-C004-CFG-01",
            '"check:objc3c:m243-c004-lowering-runtime-diagnostics-surfacing-core-feature-expansion-contract"',
        ),
        (
            "M243-C004-CFG-02",
            '"test:tooling:m243-c004-lowering-runtime-diagnostics-surfacing-core-feature-expansion-contract"',
        ),
        ("M243-C004-CFG-03", '"check:objc3c:m243-c004-lane-c-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "surface_header": (
        ("M243-C004-FORB-01", "surface.core_feature_expansion_ready = true;"),
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
            "tmp/reports/m243/M243-C004/lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract_summary.json"
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
