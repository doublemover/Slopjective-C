#!/usr/bin/env python3
"""Fail-closed validator for M228-A002 lowering pipeline pass-graph modular split."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-a002-lowering-pipeline-pass-graph-modular-split-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_lowering_pipeline_pass_graph_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_lowering_pipeline_pass_graph_scaffold.cpp",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "cmake": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "ir_emitter_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "lowering_contract_source": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_modular_split_a002_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-A002-H-01", "std::string BuildObjc3LoweringPipelinePassGraphScaffoldKey("),
        ("M228-A002-H-02", "Objc3LoweringPipelinePassGraphScaffold BuildObjc3LoweringPipelinePassGraphScaffold("),
        ("M228-A002-H-03", "bool IsObjc3LoweringPipelinePassGraphScaffoldReady("),
    ),
    "scaffold_source": (
        ("M228-A002-CPP-01", '#include "ir/objc3_ir_emitter.h"'),
        ("M228-A002-CPP-02", "TryBuildObjc3LoweringIRBoundary(options.lowering, boundary, boundary_error)"),
        ("M228-A002-CPP-03", "Objc3RuntimeDispatchDeclarationReplayKey(boundary);"),
        ("M228-A002-CPP-04", "scaffold.ir_emission_entrypoint_ready = (&EmitObjc3IRText != nullptr);"),
        ("M228-A002-CPP-05", "scaffold.pass_graph_key = BuildObjc3LoweringPipelinePassGraphScaffoldKey(scaffold);"),
    ),
    "frontend_types_header": (
        ("M228-A002-TYP-01", "struct Objc3LoweringPipelinePassGraphScaffold {"),
        ("M228-A002-TYP-02", "bool runtime_dispatch_declaration_ready = false;"),
        ("M228-A002-TYP-03", "Objc3LoweringPipelinePassGraphScaffold lowering_pipeline_pass_graph_scaffold;"),
    ),
    "pipeline_source": (
        ("M228-A002-PIPE-01", '#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"'),
        ("M228-A002-PIPE-02", "result.lowering_pipeline_pass_graph_scaffold ="),
        ("M228-A002-PIPE-03", "BuildObjc3LoweringPipelinePassGraphScaffold(result, options);"),
        ("M228-A002-PIPE-04", "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);"),
    ),
    "artifacts_source": (
        ("M228-A002-ART-01", '#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"'),
        ("M228-A002-ART-02", "IsObjc3LoweringPipelinePassGraphScaffoldReady("),
        ("M228-A002-ART-03", '"O3L301"'),
    ),
    "cmake": (
        ("M228-A002-CMAKE-01", "src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp"),
    ),
    "build_script": (
        ("M228-A002-BLD-01", "native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp"),
    ),
    "architecture_doc": (
        ("M228-A002-ARCH-01", "M228 lane-A A002 modular split scaffolding extracts pass-graph readiness"),
        ("M228-A002-ARCH-02", "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp"),
    ),
    "lowering_spec": (
        ("M228-A002-SPC-01", "### C.3.0 Lowering pass-graph scaffold (normative for implementations)"),
        ("M228-A002-SPC-02", "frontend stage handoff (`lex -> parse -> sema`)"),
    ),
    "metadata_spec": (
        ("M228-A002-META-01", "7. **Lowering pass-graph replay anchors**:"),
        ("M228-A002-META-02", "runtime dispatch declaration synthesis used by direct IR emission"),
    ),
    "ir_emitter_source": (
        ("M228-A002-IR-01", "bool EmitObjc3IRText(const Objc3Program &program,"),
    ),
    "lowering_contract_source": (
        ("M228-A002-LOW-01", "bool TryBuildObjc3LoweringIRBoundary("),
        ("M228-A002-LOW-02", "std::string Objc3RuntimeDispatchDeclarationReplayKey("),
    ),
    "contract_doc": (
        ("M228-A002-DOC-01", "Contract ID: `objc3c-lowering-pipeline-pass-graph-modular-split/m228-a002-v1`"),
        ("M228-A002-DOC-02", "objc3_lowering_pipeline_pass_graph_scaffold.cpp"),
        ("M228-A002-DOC-03", "O3L301"),
        ("M228-A002-DOC-04", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-A002-DOC-05", "tmp/reports/m228/M228-A002/lowering_pipeline_pass_graph_modular_split_contract_summary.json"),
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
        default=Path("tmp/reports/m228/M228-A002/lowering_pipeline_pass_graph_modular_split_contract_summary.json"),
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
