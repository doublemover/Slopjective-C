#!/usr/bin/env python3
"""Fail-closed validator for M228-C002 IR emission completeness modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c002-ir-emission-completeness-modular-split-scaffolding-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ir_emission_completeness_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ir_emission_completeness_scaffold.cpp",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "cmake": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "c001_contract_doc": ROOT / "docs" / "contracts" / "m228_ir_emission_completeness_contract_freeze_c001_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_modular_split_scaffolding_c002_expectations.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M228-C002-TYP-01", "struct Objc3IREmissionCompletenessScaffold {"),
        ("M228-C002-TYP-02", "bool metadata_transport_ready = false;"),
        ("M228-C002-TYP-03", "bool modular_split_ready = false;"),
        (
            "M228-C002-TYP-04",
            "Objc3IREmissionCompletenessScaffold ir_emission_completeness_scaffold;",
        ),
    ),
    "scaffold_header": (
        ("M228-C002-SCFH-01", "std::string BuildObjc3IREmissionCompletenessScaffoldKey("),
        ("M228-C002-SCFH-02", "Objc3IREmissionCompletenessScaffold BuildObjc3IREmissionCompletenessScaffold("),
        ("M228-C002-SCFH-03", "bool IsObjc3IREmissionCompletenessCoreFeatureReady("),
        ("M228-C002-SCFH-04", "bool IsObjc3IREmissionCompletenessExpansionReady("),
        ("M228-C002-SCFH-05", "bool IsObjc3IREmissionCompletenessEdgeCaseCompatibilityReady("),
    ),
    "scaffold_source": (
        ("M228-C002-SCFS-01", "BuildObjc3IREmissionCompletenessScaffoldKey("),
        ("M228-C002-SCFS-02", "scaffold.metadata_transport_ready ="),
        ("M228-C002-SCFS-03", "scaffold.modular_split_ready ="),
        ("M228-C002-SCFS-04", "scaffold.scaffold_key = BuildObjc3IREmissionCompletenessScaffoldKey(scaffold);"),
        ("M228-C002-SCFS-05", 'scaffold.failure_reason = "pass-graph expansion is not ready";'),
        ("M228-C002-SCFS-06", 'scaffold.failure_reason = "IR emission completeness metadata transport is not ready";'),
    ),
    "frontend_pipeline_source": (
        ("M228-C002-PIPE-01", '#include "pipeline/objc3_ir_emission_completeness_scaffold.h"'),
        ("M228-C002-PIPE-02", "result.ir_emission_completeness_scaffold ="),
        ("M228-C002-PIPE-03", "BuildObjc3IREmissionCompletenessScaffold(result);"),
    ),
    "artifacts_source": (
        ("M228-C002-ART-01", "IsObjc3IREmissionCompletenessCoreFeatureReady("),
        ("M228-C002-ART-02", "IsObjc3IREmissionCompletenessExpansionReady("),
        ("M228-C002-ART-03", "IsObjc3IREmissionCompletenessEdgeCaseCompatibilityReady("),
        ("M228-C002-ART-04", '"O3L304"'),
        ("M228-C002-ART-05", "ir_frontend_metadata.ir_emission_completeness_modular_split_ready ="),
        ("M228-C002-ART-06", "ir_frontend_metadata.ir_emission_completeness_modular_split_key ="),
    ),
    "ir_header": (
        ("M228-C002-IRH-01", "bool ir_emission_completeness_modular_split_ready = false;"),
        ("M228-C002-IRH-02", "std::string ir_emission_completeness_modular_split_key;"),
    ),
    "ir_source": (
        ("M228-C002-IRS-01", 'out << "; ir_emission_completeness_modular_split = "'),
        ("M228-C002-IRS-02", 'out << "; ir_emission_completeness_modular_split_ready = "'),
    ),
    "cmake": (
        ("M228-C002-CMAKE-01", "src/pipeline/objc3_ir_emission_completeness_scaffold.cpp"),
    ),
    "build_script": (
        ("M228-C002-BLD-01", "native/objc3c/src/pipeline/objc3_ir_emission_completeness_scaffold.cpp"),
    ),
    "architecture_doc": (
        ("M228-C002-ARCH-01", "M228 lane-C C002 modular split scaffolding anchors IR emission completeness"),
        ("M228-C002-ARCH-02", "pipeline/objc3_ir_emission_completeness_scaffold.cpp"),
    ),
    "lowering_spec": (
        ("M228-C002-SPC-01", "IR-emission completeness modular split scaffolding shall remain deterministic"),
    ),
    "metadata_spec": (
        ("M228-C002-META-01", "deterministic IR-emission completeness modular split scaffold key/ready"),
    ),
    "c001_contract_doc": (
        ("M228-C002-C001-01", "Contract ID: `objc3c-ir-emission-completeness-freeze/m228-c001-v1`"),
    ),
    "contract_doc": (
        ("M228-C002-DOC-01", "Contract ID: `objc3c-ir-emission-completeness-modular-split-scaffolding/m228-c002-v1`"),
        ("M228-C002-DOC-02", "Objc3IREmissionCompletenessScaffold"),
        ("M228-C002-DOC-03", "O3L302`/`O3L303`/`O3L304"),
        ("M228-C002-DOC-04", "python scripts/check_m228_c002_ir_emission_completeness_modular_split_scaffolding_contract.py"),
        ("M228-C002-DOC-05", "python -m pytest tests/tooling/test_check_m228_c002_ir_emission_completeness_modular_split_scaffolding_contract.py -q"),
        ("M228-C002-DOC-06", "npm run check:objc3c:m228-c002-lane-c-readiness"),
        ("M228-C002-DOC-07", "tmp/reports/m228/M228-C002/ir_emission_completeness_modular_split_scaffolding_contract_summary.json"),
    ),
    "package_json": (
        ("M228-C002-CFG-01", '"check:objc3c:m228-c002-ir-emission-completeness-modular-split-scaffolding-contract"'),
        ("M228-C002-CFG-02", '"test:tooling:m228-c002-ir-emission-completeness-modular-split-scaffolding-contract"'),
        ("M228-C002-CFG-03", '"check:objc3c:m228-c002-lane-c-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_source": (
        ("M228-C002-FORB-01", "scaffold.modular_split_ready = true;"),
        ("M228-C002-FORB-02", "scaffold.core_feature_ready = true;"),
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
            "tmp/reports/m228/M228-C002/"
            "ir_emission_completeness_modular_split_scaffolding_contract_summary.json"
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
