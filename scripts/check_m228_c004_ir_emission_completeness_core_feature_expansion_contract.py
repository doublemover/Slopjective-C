#!/usr/bin/env python3
"""Fail-closed validator for M228-C004 IR emission completeness core-feature expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c004-ir-emission-completeness-core-feature-expansion-contract-v1"

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
    "c003_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_core_feature_implementation_c003_expectations.md",
    "c003_checker": ROOT / "scripts" / "check_m228_c003_ir_emission_completeness_core_feature_implementation_contract.py",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_core_feature_expansion_c004_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c004_ir_emission_completeness_core_feature_expansion_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-C004-H-00", "struct Objc3IREmissionCoreFeatureImplementationSurface {"),
        ("M228-C004-H-01", "bool pass_graph_expansion_ready = false;"),
        ("M228-C004-H-02", "bool expansion_metadata_transport_ready = false;"),
        ("M228-C004-H-03", "bool core_feature_expansion_ready = false;"),
        ("M228-C004-H-04", "std::string expansion_key;"),
        ("M228-C004-H-05", "std::string expansion_failure_reason;"),
        ("M228-C004-H-06", "BuildObjc3IREmissionCoreFeatureExpansionKey("),
        ("M228-C004-H-07", "surface.pass_graph_expansion_ready ="),
        ("M228-C004-H-08", "surface.expansion_metadata_transport_ready = !scaffold.expansion_key.empty();"),
        ("M228-C004-H-09", "surface.core_feature_expansion_ready ="),
        ("M228-C004-H-10", "surface.expansion_key = BuildObjc3IREmissionCoreFeatureExpansionKey(surface);"),
        ("M228-C004-H-11", "IsObjc3IREmissionCoreFeatureExpansionReady("),
        (
            "M228-C004-H-12",
            '"IR emission core feature expansion metadata transport is not ready"',
        ),
    ),
    "artifacts_source": (
        (
            "M228-C004-ART-00",
            "BuildObjc3IREmissionCoreFeatureImplementationSurface(pipeline_result);",
        ),
        ("M228-C004-ART-01", "IsObjc3IREmissionCoreFeatureExpansionReady("),
        ("M228-C004-ART-02", '"O3L314"'),
        (
            "M228-C004-ART-03",
            "IR emission core feature expansion check failed",
        ),
        ("M228-C004-ART-04", "ir_frontend_metadata.ir_emission_core_feature_expansion_ready ="),
        ("M228-C004-ART-05", "ir_frontend_metadata.ir_emission_core_feature_expansion_key ="),
    ),
    "ir_header": (
        ("M228-C004-IRH-01", "bool ir_emission_core_feature_expansion_ready = false;"),
        ("M228-C004-IRH-02", "std::string ir_emission_core_feature_expansion_key;"),
    ),
    "ir_source": (
        ("M228-C004-IRS-01", 'out << "; ir_emission_core_feature_expansion = "'),
        ("M228-C004-IRS-02", 'out << "; ir_emission_core_feature_expansion_ready = "'),
    ),
    "architecture_doc": (
        (
            "M228-C004-ARCH-01",
            "M228 lane-C C004 core feature expansion anchors IR emission",
        ),
        (
            "M228-C004-ARCH-02",
            "pipeline/objc3_ir_emission_core_feature_implementation_surface.h",
        ),
    ),
    "lowering_spec": (
        (
            "M228-C004-SPC-01",
            "IR-emission core-feature expansion shall remain deterministic",
        ),
    ),
    "metadata_spec": (
        (
            "M228-C004-META-01",
            "deterministic IR-emission core-feature expansion readiness/key anchors",
        ),
    ),
    "c003_contract_doc": (
        (
            "M228-C004-C003-01",
            "Contract ID: `objc3c-ir-emission-completeness-core-feature-implementation/m228-c003-v1`",
        ),
    ),
    "c003_checker": (
        (
            "M228-C004-C003-02",
            'MODE = "m228-c003-ir-emission-completeness-core-feature-implementation-contract-v1"',
        ),
    ),
    "contract_doc": (
        (
            "M228-C004-DOC-01",
            "Contract ID: `objc3c-ir-emission-completeness-core-feature-expansion/m228-c004-v1`",
        ),
        ("M228-C004-DOC-02", "BuildObjc3IREmissionCoreFeatureExpansionKey"),
        ("M228-C004-DOC-03", "IsObjc3IREmissionCoreFeatureExpansionReady"),
        ("M228-C004-DOC-04", "O3L314"),
        (
            "M228-C004-DOC-05",
            "scripts/check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py",
        ),
        (
            "M228-C004-DOC-06",
            "tests/tooling/test_check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py",
        ),
        (
            "M228-C004-DOC-07",
            "spec/planning/compiler/m228/m228_c004_ir_emission_completeness_core_feature_expansion_packet.md",
        ),
        ("M228-C004-DOC-08", "npm run check:objc3c:m228-c004-lane-c-readiness"),
        ("M228-C004-DOC-09", "npm run build:objc3c-native"),
        (
            "M228-C004-DOC-10",
            "tmp/reports/m228/M228-C004/ir_emission_completeness_core_feature_expansion_contract_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M228-C004-PKT-01",
            "# M228-C004 IR Emission Completeness Core Feature Expansion Packet",
        ),
        ("M228-C004-PKT-02", "Packet: `M228-C004`"),
        ("M228-C004-PKT-03", "Dependencies: `M228-C003`"),
        (
            "M228-C004-PKT-04",
            "m228_ir_emission_completeness_core_feature_expansion_c004_expectations.md",
        ),
        (
            "M228-C004-PKT-05",
            "scripts/check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py",
        ),
        (
            "M228-C004-PKT-06",
            "tests/tooling/test_check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py",
        ),
        ("M228-C004-PKT-07", "`check:objc3c:m228-c004-lane-c-readiness`"),
        ("M228-C004-PKT-08", "`test:objc3c:lowering-replay-proof`"),
    ),
    "package_json": (
        (
            "M228-C004-CFG-01",
            '"check:objc3c:m228-c004-ir-emission-completeness-core-feature-expansion-contract"',
        ),
        (
            "M228-C004-CFG-02",
            '"test:tooling:m228-c004-ir-emission-completeness-core-feature-expansion-contract"',
        ),
        ("M228-C004-CFG-03", '"check:objc3c:m228-c004-lane-c-readiness"'),
        (
            "M228-C004-CFG-04",
            "npm run check:objc3c:m228-c003-lane-c-readiness && npm run check:objc3c:m228-c004-ir-emission-completeness-core-feature-expansion-contract && npm run test:tooling:m228-c004-ir-emission-completeness-core-feature-expansion-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-C004-FORB-01", "surface.core_feature_expansion_ready = true;"),
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
            "tmp/reports/m228/M228-C004/"
            "ir_emission_completeness_core_feature_expansion_contract_summary.json"
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
