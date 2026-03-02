#!/usr/bin/env python3
"""Fail-closed validator for M228-A009 lowering pipeline pass-graph conformance matrix implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-a009-lowering-pipeline-pass-graph-conformance-matrix-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_lowering_pipeline_pass_graph_core_feature_surface.h",
    "core_surface_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "a008_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_recovery_determinism_hardening_a008_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_conformance_matrix_implementation_a009_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-A009-H-01", "BuildObjc3LoweringPipelinePassGraphConformanceMatrixKey("),
        ("M228-A009-H-02", "IsObjc3LoweringPipelinePassGraphConformanceMatrixReady("),
    ),
    "core_surface_source": (
        ("M228-A009-CPP-01", "surface.conformance_matrix_consistent ="),
        ("M228-A009-CPP-02", "surface.conformance_matrix_ready ="),
        ("M228-A009-CPP-03", "surface.conformance_matrix_key ="),
        ("M228-A009-CPP-04", "BuildObjc3LoweringPipelinePassGraphConformanceMatrixKey(surface);"),
        ("M228-A009-CPP-05", "surface.failure_reason = \"pass-graph conformance matrix is inconsistent\";"),
        ("M228-A009-CPP-06", "surface.failure_reason = \"pass-graph conformance matrix is not ready\";"),
        ("M228-A009-CPP-07", "surface.failure_reason = \"pass-graph conformance matrix key is not ready\";"),
    ),
    "frontend_types_header": (
        ("M228-A009-TYP-01", "bool conformance_matrix_consistent = false;"),
        ("M228-A009-TYP-02", "bool conformance_matrix_ready = false;"),
        ("M228-A009-TYP-03", "std::string conformance_matrix_key;"),
    ),
    "artifacts_source": (
        ("M228-A009-ART-01", "IsObjc3LoweringPipelinePassGraphConformanceMatrixReady("),
        ("M228-A009-ART-02", '"O3L311"'),
        ("M228-A009-ART-03", "ir_frontend_metadata.lowering_pass_graph_conformance_matrix_ready ="),
        ("M228-A009-ART-04", "ir_frontend_metadata.lowering_pass_graph_conformance_matrix_key ="),
    ),
    "ir_header": (
        ("M228-A009-IRH-01", "bool lowering_pass_graph_conformance_matrix_ready = false;"),
        ("M228-A009-IRH-02", "std::string lowering_pass_graph_conformance_matrix_key;"),
    ),
    "ir_source": (
        ("M228-A009-IRS-01", 'out << "; lowering_pass_graph_conformance_matrix = "'),
        ("M228-A009-IRS-02", 'out << "; lowering_pass_graph_conformance_matrix_ready = "'),
    ),
    "package_json": (
        (
            "M228-A009-CFG-01",
            '"check:objc3c:m228-a009-lowering-pipeline-pass-graph-conformance-matrix-contract"',
        ),
        (
            "M228-A009-CFG-02",
            '"test:tooling:m228-a009-lowering-pipeline-pass-graph-conformance-matrix-contract"',
        ),
        ("M228-A009-CFG-03", '"check:objc3c:m228-a009-lane-a-readiness"'),
        (
            "M228-A009-CFG-04",
            "npm run check:objc3c:m228-a008-lane-a-readiness && npm run check:objc3c:m228-a009-lowering-pipeline-pass-graph-conformance-matrix-contract",
        ),
    ),
    "architecture_doc": (
        ("M228-A009-ARCH-01", "M228 lane-A A009 conformance matrix implementation extends"),
        ("M228-A009-ARCH-02", "conformance_matrix_consistent"),
    ),
    "lowering_spec": (
        ("M228-A009-SPC-01", "conformance matrix implementation shall include deterministic conformance"),
    ),
    "metadata_spec": (
        ("M228-A009-META-01", "deterministic conformance-matrix readiness/key anchors for lowering"),
    ),
    "a008_contract_doc": (
        (
            "M228-A009-DEP-01",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-recovery-determinism-hardening/m228-a008-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M228-A009-DOC-01",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-conformance-matrix-implementation/m228-a009-v1`",
        ),
        ("M228-A009-DOC-02", "O3L311"),
        ("M228-A009-DOC-03", "lowering_pass_graph_conformance_matrix_key"),
        (
            "M228-A009-DOC-04",
            "tmp/reports/m228/M228-A009/lowering_pipeline_pass_graph_conformance_matrix_contract_summary.json",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_source": (
        ("M228-A009-FORB-01", "surface.conformance_matrix_ready = true;"),
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
        default=Path("tmp/reports/m228/M228-A009/lowering_pipeline_pass_graph_conformance_matrix_contract_summary.json"),
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
