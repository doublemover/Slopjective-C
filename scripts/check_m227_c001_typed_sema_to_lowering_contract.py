#!/usr/bin/env python3
"""Fail-closed contract checker for M227-C001 typed sema-to-lowering freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-c001-typed-sema-to-lowering-contract-and-architecture-freeze-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_pipeline_contract_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "frontend_artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "semantics_fragment": ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md",
    "artifacts_fragment": ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md",
    "expectations_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_contract_expectations.md",
    "packet_doc": (
        ROOT
        / "spec"
        / "planning"
        / "compiler"
        / "m227"
        / "m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md"
    ),
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

ARTIFACT_ORDER: tuple[str, ...] = tuple(ARTIFACTS.keys())
ARTIFACT_RANK = {name: index for index, name in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_pipeline_contract_header": (
        ("M227-C001-CON-01", "struct FunctionSignatureSurface {"),
        ("M227-C001-CON-02", "struct SemaStageOutput {"),
        ("M227-C001-CON-03", "FunctionSignatureSurface function_signature_surface;"),
        ("M227-C001-CON-04", "struct LowerStageInput {"),
        ("M227-C001-CON-05", "std::size_t runtime_dispatch_arg_slots = kRuntimeDispatchDefaultArgs;"),
        ("M227-C001-CON-06", 'std::string selector_global_ordering = "lexicographic";'),
        ("M227-C001-CON-07", "struct FrontendPipelineOutput {"),
        ("M227-C001-CON-08", "ErrorPropagationModel error_model = ErrorPropagationModel::NoThrowFailClosed;"),
    ),
    "frontend_types_header": (
        ("M227-C001-TYP-01", "struct Objc3FrontendObjectPointerNullabilityGenericsSummary {"),
        (
            "M227-C001-TYP-02",
            "bool deterministic_object_pointer_nullability_generics_handoff = true;",
        ),
        ("M227-C001-TYP-03", "struct Objc3FrontendSymbolGraphScopeResolutionSummary {"),
        ("M227-C001-TYP-04", "bool deterministic_symbol_graph_handoff = true;"),
        ("M227-C001-TYP-05", "bool deterministic_scope_resolution_handoff = true;"),
        ("M227-C001-TYP-06", "std::string deterministic_handoff_key;"),
        ("M227-C001-TYP-07", "struct Objc3FrontendPipelineResult {"),
        ("M227-C001-TYP-08", "Objc3SemanticIntegrationSurface integration_surface;"),
        ("M227-C001-TYP-09", "Objc3SemanticTypeMetadataHandoff sema_type_metadata_handoff;"),
        (
            "M227-C001-TYP-10",
            "Objc3FrontendObjectPointerNullabilityGenericsSummary object_pointer_nullability_generics_summary;",
        ),
        (
            "M227-C001-TYP-11",
            "Objc3FrontendSymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;",
        ),
        ("M227-C001-TYP-12", "Objc3SemaParityContractSurface sema_parity_surface;"),
    ),
    "pipeline_source": (
        ("M227-C001-PIP-01", "sema_input.program = &result.program;"),
        ("M227-C001-PIP-02", "sema_input.validation_options = semantic_options;"),
        (
            "M227-C001-PIP-03",
            "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        ),
        (
            "M227-C001-PIP-04",
            "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        ),
        (
            "M227-C001-PIP-05",
            "result.integration_surface = std::move(sema_result.integration_surface);",
        ),
        (
            "M227-C001-PIP-06",
            "result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);",
        ),
        ("M227-C001-PIP-07", "BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),"),
        (
            "M227-C001-PIP-08",
            "BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,",
        ),
        ("M227-C001-PIP-09", "BuildSymbolGraphScopeResolutionSummary(result.integration_surface,"),
        ("M227-C001-PIP-10", "result.sema_parity_surface = sema_result.parity_surface;"),
        (
            "M227-C001-PIP-11",
            "result.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(result, options);",
        ),
        (
            "M227-C001-PIP-12",
            "TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);",
        ),
    ),
    "frontend_artifacts_source": (
        (
            "M227-C001-ART-01",
            "const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff = pipeline_result.sema_type_metadata_handoff;",
        ),
        (
            "M227-C001-ART-02",
            "const Objc3FrontendObjectPointerNullabilityGenericsSummary &object_pointer_nullability_generics_summary =",
        ),
        (
            "M227-C001-ART-03",
            "const Objc3FrontendSymbolGraphScopeResolutionSummary &symbol_graph_scope_resolution_summary =",
        ),
        ("M227-C001-ART-04", "BuildIdClassSelObjectPointerTypecheckContract(program);"),
        ("M227-C001-ART-05", "BuildMessageSendSelectorLoweringContract(program);"),
        (
            "M227-C001-ART-06",
            "BuildDispatchAbiMarshallingContract(program, options.lowering.max_message_send_args);",
        ),
        ("M227-C001-ART-07", "BuildRuntimeShimHostLinkContract("),
        (
            "M227-C001-ART-08",
            "BuildOwnershipQualifierLoweringContract(pipeline_result.sema_parity_surface);",
        ),
        (
            "M227-C001-ART-09",
            "BuildRetainReleaseOperationLoweringContract(pipeline_result.sema_parity_surface);",
        ),
        ("M227-C001-ART-10", '<< ",\\"deterministic_type_metadata_handoff\\":"'),
        (
            "M227-C001-ART-11",
            '<< (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")',
        ),
        ("M227-C001-ART-12", "ir_frontend_metadata.deterministic_interface_implementation_handoff ="),
        ("M227-C001-ART-13", "ir_frontend_metadata.deterministic_symbol_graph_handoff ="),
        ("M227-C001-ART-14", "ir_frontend_metadata.deterministic_scope_resolution_handoff ="),
        ("M227-C001-ART-15", "ir_frontend_metadata.deterministic_symbol_graph_scope_resolution_handoff_key ="),
        (
            "M227-C001-ART-16",
            "if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {",
        ),
    ),
    "semantics_fragment": (
        ("M227-C001-SPEC-01", "Pipeline derive transport anchors:"),
        (
            "M227-C001-SPEC-02",
            "`Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);`",
        ),
        (
            "M227-C001-SPEC-03",
            "`result.integration_surface = std::move(sema_result.integration_surface);`",
        ),
        (
            "M227-C001-SPEC-04",
            "`result.sema_parity_surface = sema_result.parity_surface;`",
        ),
        (
            "M227-C001-SPEC-05",
            "M151-B extends sema/type metadata and pass-manager parity surfaces with deterministic symbol-graph",
        ),
        ("M227-C001-SPEC-06", "Sema/type metadata handoff contract:"),
    ),
    "artifacts_fragment": (
        ("M227-C001-SPEC-07", "`frontend.pipeline.sema_pass_manager.deterministic_interface_implementation_handoff`"),
        ("M227-C001-SPEC-08", "`frontend.pipeline.sema_pass_manager.deterministic_symbol_graph_handoff`"),
        ("M227-C001-SPEC-09", "`frontend.pipeline.sema_pass_manager.deterministic_scope_resolution_handoff`"),
        (
            "M227-C001-SPEC-10",
            "`frontend.pipeline.sema_pass_manager.symbol_graph_scope_resolution_handoff_key`",
        ),
        (
            "M227-C001-SPEC-11",
            "`frontend.pipeline.semantic_surface.objc_symbol_graph_scope_resolution_surface`",
        ),
    ),
    "expectations_doc": (
        (
            "M227-C001-DOC-EXP-01",
            "# M227 Typed Sema-to-Lowering Contracts Contract and Architecture Freeze Expectations (C001)",
        ),
        (
            "M227-C001-DOC-EXP-02",
            "Contract ID: `objc3c-typed-sema-to-lowering-contract-and-architecture-freeze/m227-c001-v1`",
        ),
        (
            "M227-C001-DOC-EXP-03",
            "Issue `#5121` defines canonical lane-C contract freeze scope.",
        ),
        ("M227-C001-DOC-EXP-04", "Dependencies: none"),
        (
            "M227-C001-DOC-EXP-05",
            "m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md",
        ),
        (
            "M227-C001-DOC-EXP-06",
            "`check:objc3c:m227-c001-lane-c-readiness`",
        ),
        (
            "M227-C001-DOC-EXP-07",
            "`tmp/reports/m227/M227-C001/typed_sema_to_lowering_contract_and_architecture_freeze_summary.json`",
        ),
    ),
    "packet_doc": (
        (
            "M227-C001-DOC-PKT-01",
            "# M227-C001 Typed Sema-to-Lowering Contracts Contract and Architecture Freeze Packet",
        ),
        ("M227-C001-DOC-PKT-02", "Packet: `M227-C001`"),
        ("M227-C001-DOC-PKT-03", "Issue: `#5121`"),
        ("M227-C001-DOC-PKT-04", "Freeze date: `2026-03-03`"),
        ("M227-C001-DOC-PKT-05", "Dependencies: none"),
        (
            "M227-C001-DOC-PKT-06",
            "`check:objc3c:m227-c001-typed-sema-to-lowering-contract`",
        ),
        (
            "M227-C001-DOC-PKT-07",
            "`check:objc3c:m227-c001-lane-c-readiness`",
        ),
        ("M227-C001-DOC-PKT-08", "`compile:objc3c`"),
    ),
    "architecture_doc": (
        (
            "M227-C001-ARCH-01",
            "M227 lane-C C001 typed sema-to-lowering contracts contract and architecture freeze anchors",
        ),
        (
            "M227-C001-ARCH-02",
            "docs/contracts/m227_typed_sema_to_lowering_contract_expectations.md",
        ),
        (
            "M227-C001-ARCH-03",
            "spec/planning/compiler/m227/m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md",
        ),
        (
            "M227-C001-ARCH-04",
            "check:objc3c:m227-c001-lane-c-readiness",
        ),
    ),
    "lowering_spec": (
        (
            "M227-C001-SPC-01",
            "typed sema-to-lowering contracts governance shall preserve explicit lane-C typed sema handoff anchors",
        ),
        (
            "M227-C001-SPC-02",
            "lane-C dependency anchors (`M227-C001`) and fail closed on typed sema transport or lowering metadata drift",
        ),
    ),
    "metadata_spec": (
        (
            "M227-C001-META-01",
            "deterministic lane-C typed sema-to-lowering metadata anchors for `M227-C001`",
        ),
        (
            "M227-C001-META-02",
            "typed sema handoff evidence and lowering metadata continuity",
        ),
    ),
    "package_json": (
        (
            "M227-C001-PKG-01",
            '"check:objc3c:m227-c001-typed-sema-to-lowering-contract": "python scripts/check_m227_c001_typed_sema_to_lowering_contract.py"',
        ),
        (
            "M227-C001-PKG-02",
            '"test:tooling:m227-c001-typed-sema-to-lowering-contract": "python -m pytest tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py -q"',
        ),
        (
            "M227-C001-PKG-03",
            '"check:objc3c:m227-c001-lane-c-readiness": "npm run check:objc3c:m227-c001-typed-sema-to-lowering-contract && npm run test:tooling:m227-c001-typed-sema-to-lowering-contract"',
        ),
        ("M227-C001-PKG-04", '"compile:objc3c": '),
        ("M227-C001-PKG-05", '"proof:objc3c": '),
        ("M227-C001-PKG-06", '"test:objc3c:execution-replay-proof": '),
        ("M227-C001-PKG-07", '"test:objc3c:perf-budget": '),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists():
        raise ValueError(f"{artifact} file does not exist: {display_path(path)}")
    if not path.is_file():
        raise ValueError(f"{artifact} path is not a file: {display_path(path)}")
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise ValueError(f"{artifact} file is not valid UTF-8: {display_path(path)}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read {artifact} file {display_path(path)}: {exc}") from exc


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m227/M227-C001/typed_sema_to_lowering_contract_and_architecture_freeze_summary.json"
        ),
    )
    return parser.parse_args(argv)


def collect_required_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
        if snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact,
                    check_id=check_id,
                    detail=f"expected snippet missing: {snippet}",
                )
            )
    return findings


def total_checks() -> int:
    return sum(len(snippets) for snippets in REQUIRED_SNIPPETS.values())


def finding_sort_key(finding: Finding) -> tuple[int, str, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in findings: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        findings: list[Finding] = []
        for artifact, path in ARTIFACTS.items():
            text = load_text(path, artifact=artifact)
            findings.extend(collect_required_findings(artifact=artifact, text=text))
        findings = sorted(findings, key=finding_sort_key)
    except ValueError as exc:
        print(f"m227-c001-typed-sema-to-lowering-contract: error: {exc}", file=sys.stderr)
        return 2

    checks_total = total_checks()
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
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
        print(
            "m227-c001-typed-sema-to-lowering-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m227-c001-typed-sema-to-lowering-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(f"m227-c001-typed-sema-to-lowering-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
