#!/usr/bin/env python3
"""Fail-closed validator for M228-A001 lowering pipeline decomposition/pass-graph freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-a001-lowering-pipeline-decomposition-pass-graph-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "pipeline_contract_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "lowering_contract_source": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_contract_freeze_a001_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "pipeline_contract_header": (
        ("M228-A001-CON-01", "enum class StageId : std::uint8_t {"),
        ("M228-A001-CON-02", "Lower = 3,"),
        ("M228-A001-CON-03", "Emit = 4,"),
        ("M228-A001-CON-04", "inline constexpr std::array<StageId, 5> kStageOrder = {"),
        ("M228-A001-CON-05", "StageId::Lex,"),
        ("M228-A001-CON-06", "StageId::Parse,"),
        ("M228-A001-CON-07", "StageId::Sema,"),
        ("M228-A001-CON-08", "StageId::Lower,"),
        ("M228-A001-CON-09", "StageId::Emit,"),
        ("M228-A001-CON-10", "ErrorPropagationModel::NoThrowFailClosed"),
        ("M228-A001-CON-11", "StageResult lower;"),
        ("M228-A001-CON-12", "StageResult emit;"),
    ),
    "pipeline_source": (
        ("M228-A001-PIP-01", "Objc3FrontendPipelineResult RunObjc3FrontendPipeline("),
        ("M228-A001-PIP-02", "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);"),
        ("M228-A001-PIP-03", "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);"),
        (
            "M228-A001-PIP-04",
            "result.typed_sema_to_lowering_contract_surface =\n      BuildObjc3TypedSemaToLoweringContractSurface(result, options);",
        ),
        ("M228-A001-PIP-05", "result.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(result, options);"),
        (
            "M228-A001-PIP-06",
            "result.semantic_stability_spec_delta_closure_scaffold =\n      BuildObjc3SemanticStabilitySpecDeltaClosureScaffold(",
        ),
        (
            "M228-A001-PIP-07",
            "result.lowering_runtime_stability_invariant_scaffold =\n      BuildObjc3LoweringRuntimeStabilityInvariantScaffold(",
        ),
        (
            "M228-A001-PIP-08",
            "result.lowering_runtime_stability_core_feature_implementation_surface =\n      BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(",
        ),
        ("M228-A001-PIP-09", "TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);"),
    ),
    "artifacts_source": (
        ("M228-A001-ART-01", "Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts("),
        ("M228-A001-ART-02", "IsObjc3ParseLoweringReadinessSurfaceReady(bundle.parse_lowering_readiness_surface,"),
        ("M228-A001-ART-03", "BuildMessageSendSelectorLoweringContract(program);"),
        (
            "M228-A001-ART-04",
            "BuildDispatchAbiMarshallingContract(program, options.lowering.max_message_send_args);",
        ),
        (
            "M228-A001-ART-05",
            "if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {",
        ),
    ),
    "lowering_contract_source": (
        ("M228-A001-LOW-01", "bool TryBuildObjc3LoweringIRBoundary("),
        ("M228-A001-LOW-02", "boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;"),
        ("M228-A001-LOW-03", "boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;"),
        ("M228-A001-LOW-04", "std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary) {"),
        ("M228-A001-LOW-05", "std::string Objc3RuntimeDispatchDeclarationReplayKey("),
    ),
    "architecture_doc": (
        ("M228-A001-ARC-01", "M228 lane-A A001 lowering pipeline decomposition/pass-graph freeze anchors"),
        ("M228-A001-ARC-02", "`pipeline/frontend_pipeline_contract.h`"),
        ("M228-A001-ARC-03", "`pipeline/objc3_frontend_pipeline.cpp`"),
    ),
    "contract_doc": (
        ("M228-A001-DOC-01", "Contract ID: `objc3c-lowering-pipeline-pass-graph-freeze/m228-a001-v1`"),
        ("M228-A001-DOC-02", "frontend_pipeline_contract.h"),
        ("M228-A001-DOC-03", "BuildObjc3AstFromTokens"),
        ("M228-A001-DOC-04", "EmitObjc3IRText"),
        (
            "M228-A001-DOC-05",
            "python scripts/check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract.py",
        ),
        (
            "M228-A001-DOC-06",
            "python -m pytest tests/tooling/test_check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract.py -q",
        ),
        (
            "M228-A001-DOC-07",
            "tmp/reports/m228/m228_a001_lowering_pipeline_decomposition_pass_graph_contract_summary.json",
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
        default=Path("tmp/reports/m228/m228_a001_lowering_pipeline_decomposition_pass_graph_contract_summary.json"),
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
