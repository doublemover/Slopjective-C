#!/usr/bin/env python3
"""Fail-closed validator for M228-A003 lowering pipeline pass-graph core feature."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-a003-lowering-pipeline-pass-graph-core-feature-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_lowering_pipeline_pass_graph_core_feature_surface.h",
    "core_surface_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "cmake": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "lowering_contract_source": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_core_feature_a003_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-A003-H-01", "BuildObjc3LoweringPipelinePassGraphCoreFeatureKey("),
        ("M228-A003-H-02", "BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface("),
        ("M228-A003-H-03", "IsObjc3LoweringPipelinePassGraphCoreFeatureSurfaceReady("),
    ),
    "core_surface_source": (
        ("M228-A003-CPP-01", "surface.lowering_boundary_replay_key_consistent ="),
        ("M228-A003-CPP-02", "surface.runtime_dispatch_declaration_consistent ="),
        ("M228-A003-CPP-03", "surface.dispatch_shape_sharding_ready ="),
        ("M228-A003-CPP-04", "surface.llc_object_emission_route_deterministic ="),
        ("M228-A003-CPP-05", "surface.core_feature_key ="),
    ),
    "frontend_types_header": (
        ("M228-A003-TYP-01", "struct Objc3LoweringPipelinePassGraphCoreFeatureSurface {"),
        ("M228-A003-TYP-02", "bool dispatch_shape_sharding_ready = false;"),
        ("M228-A003-TYP-03", "Objc3LoweringPipelinePassGraphCoreFeatureSurface lowering_pipeline_pass_graph_core_feature_surface;"),
    ),
    "pipeline_source": (
        ("M228-A003-PIPE-01", '#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"'),
        ("M228-A003-PIPE-02", "result.lowering_pipeline_pass_graph_core_feature_surface ="),
        ("M228-A003-PIPE-03", "BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(result, options);"),
    ),
    "artifacts_source": (
        ("M228-A003-ART-01", '#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"'),
        ("M228-A003-ART-02", "IsObjc3LoweringPipelinePassGraphCoreFeatureSurfaceReady("),
        ("M228-A003-ART-03", '"O3L302"'),
        ("M228-A003-ART-04", "ir_frontend_metadata.lowering_pass_graph_core_feature_ready ="),
        ("M228-A003-ART-05", "ir_frontend_metadata.lowering_pass_graph_core_feature_key ="),
    ),
    "ir_header": (
        ("M228-A003-IRH-01", "bool lowering_pass_graph_core_feature_ready = false;"),
        ("M228-A003-IRH-02", "std::string lowering_pass_graph_core_feature_key;"),
    ),
    "ir_source": (
        ("M228-A003-IRS-01", "if (!frontend_metadata_.lowering_pass_graph_core_feature_key.empty()) {"),
        ("M228-A003-IRS-02", 'out << "; lowering_pass_graph_core_feature = "'),
        ("M228-A003-IRS-03", 'out << "; lowering_pass_graph_core_feature_ready = "'),
    ),
    "cmake": (
        ("M228-A003-CMAKE-01", "src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp"),
    ),
    "build_script": (
        ("M228-A003-BLD-01", "native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp"),
    ),
    "architecture_doc": (
        ("M228-A003-ARCH-01", "M228 lane-A A003"),
    ),
    "lowering_spec": (
        ("M228-A003-SPC-01", "pass-graph gate shall validate"),
    ),
    "metadata_spec": (
        ("M228-A003-META-01", "Lowering pass-graph replay anchors"),
    ),
    "lowering_contract_source": (
        ("M228-A003-LOW-01", "bool IsValidRuntimeDispatchSymbol(const std::string &symbol)"),
    ),
    "contract_doc": (
        ("M228-A003-DOC-01", "Contract ID: `objc3c-lowering-pipeline-pass-graph-core-feature/m228-a003-v1`"),
        ("M228-A003-DOC-02", "O3L302"),
        ("M228-A003-DOC-03", "lowering_pass_graph_core_feature_key"),
        ("M228-A003-DOC-04", "tmp/reports/m228/M228-A003/lowering_pipeline_pass_graph_core_feature_contract_summary.json"),
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
        default=Path("tmp/reports/m228/M228-A003/lowering_pipeline_pass_graph_core_feature_contract_summary.json"),
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
