#!/usr/bin/env python3
"""Fail-closed validator for M227-D002 runtime-facing type metadata modular split scaffold contract."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-d002-runtime-facing-type-metadata-modular-split-scaffold-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "parser_handoff_scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_parser_sema_handoff_scaffold.h",
    "sema_pass_flow_scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_flow_scaffold.h",
    "sema_pass_flow_scaffold_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_flow_scaffold.cpp",
    "sema_pass_manager_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager.cpp",
    "sema_pass_manager_contract_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager_contract.h",
    "frontend_pipeline_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_pipeline.cpp",
    "frontend_artifacts_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_artifacts.cpp",
    "cmake_file": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_runtime_facing_type_metadata_modular_split_d002_expectations.md",
    "planning_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_d002_runtime_facing_type_metadata_modular_split_packet.md",
    "d001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_runtime_facing_type_metadata_semantics_expectations.md",
    "d001_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_d001_runtime_facing_type_metadata_semantics_contract_freeze.md",
    "d001_checker": ROOT / "scripts" / "check_m227_d001_runtime_facing_type_metadata_semantics_contract.py",
    "d001_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

ARTIFACT_ORDER: tuple[str, ...] = tuple(ARTIFACTS.keys())
ARTIFACT_RANK = {name: index for index, name in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_handoff_scaffold_header": (
        (
            "M227-D002-HOFF-01",
            "inline constexpr std::size_t kObjc3ParserSemaHandoffScaffoldBuilderMaxLines = 80u;",
        ),
        ("M227-D002-HOFF-02", "struct Objc3ParserSemaHandoffScaffold {"),
        (
            "M227-D002-HOFF-03",
            "inline Objc3ParserSemaHandoffScaffold BuildObjc3ParserSemaHandoffScaffold(const Objc3SemaPassManagerInput &input) {",
        ),
        ("M227-D002-HOFF-04", "scaffold.parser_sema_performance_quality_guardrails ="),
        (
            "M227-D002-HOFF-05",
            "guardrails.handoff_scaffold_builder_max_lines = kObjc3ParserSemaHandoffScaffoldBuilderMaxLines;",
        ),
    ),
    "sema_pass_flow_scaffold_header": (
        (
            "M227-D002-SFLOW-H-01",
            "void MarkObjc3SemaPassExecuted(Objc3SemaPassFlowSummary &summary, Objc3SemaPassId pass);",
        ),
        ("M227-D002-SFLOW-H-02", "void FinalizeObjc3SemaPassFlowSummary("),
        ("M227-D002-SFLOW-H-03", "const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,"),
    ),
    "sema_pass_flow_scaffold_source": (
        ("M227-D002-SFLOW-CPP-01", '#include "sema/objc3_sema_pass_flow_scaffold.h"'),
        (
            "M227-D002-SFLOW-CPP-02",
            "summary.type_metadata_global_entries = type_metadata_handoff.global_names_lexicographic.size();",
        ),
        (
            "M227-D002-SFLOW-CPP-03",
            "summary.type_metadata_interface_entries = type_metadata_handoff.interfaces_lexicographic.size();",
        ),
        ("M227-D002-SFLOW-CPP-04", "summary.symbol_flow_counts_consistent ="),
        ("M227-D002-SFLOW-CPP-05", "summary.deterministic_handoff_key = handoff_key.str();"),
        ("M227-D002-SFLOW-CPP-06", "deterministic_type_metadata_handoff;"),
    ),
    "sema_pass_manager_source": (
        ("M227-D002-MGR-01", '#include "sema/objc3_parser_sema_handoff_scaffold.h"'),
        ("M227-D002-MGR-02", '#include "sema/objc3_sema_pass_flow_scaffold.h"'),
        (
            "M227-D002-MGR-03",
            "const Objc3ParserSemaHandoffScaffold handoff = BuildObjc3ParserSemaHandoffScaffold(input);",
        ),
        ("M227-D002-MGR-04", "MarkObjc3SemaPassExecuted(result.sema_pass_flow_summary, pass);"),
        ("M227-D002-MGR-05", "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);"),
        ("M227-D002-MGR-06", "result.deterministic_type_metadata_handoff ="),
        ("M227-D002-MGR-07", "FinalizeObjc3SemaPassFlowSummary(result.sema_pass_flow_summary,"),
        (
            "M227-D002-MGR-08",
            "result.sema_pass_flow_summary.diagnostics_hardening_satisfied = result.diagnostics_hardening_satisfied;",
        ),
    ),
    "sema_pass_manager_contract_header": (
        ("M227-D002-MGR-CON-01", "std::size_t handoff_scaffold_builder_max_lines = 0;"),
        ("M227-D002-MGR-CON-02", "bool handoff_scaffold_builder_budget_guarded = false;"),
        ("M227-D002-MGR-CON-03", "bool deterministic_type_metadata_handoff = false;"),
        ("M227-D002-MGR-CON-04", "Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;"),
        ("M227-D002-MGR-CON-05", "Objc3RetainReleaseOperationSummary retain_release_operation_summary;"),
        ("M227-D002-MGR-CON-06", "surface.deterministic_runtime_shim_host_link_handoff &&"),
        ("M227-D002-MGR-CON-07", "surface.deterministic_retain_release_operation_handoff &&"),
    ),
    "frontend_pipeline_source": (
        (
            "M227-D002-PIPE-01",
            "result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);",
        ),
        ("M227-D002-PIPE-02", "result.sema_parity_surface = sema_result.parity_surface;"),
        (
            "M227-D002-PIPE-03",
            "BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,",
        ),
        ("M227-D002-PIPE-04", "BuildSymbolGraphScopeResolutionSummary(result.integration_surface,"),
        ("M227-D002-PIPE-05", "result.object_pointer_nullability_generics_summary ="),
        ("M227-D002-PIPE-06", "result.symbol_graph_scope_resolution_summary ="),
    ),
    "frontend_artifacts_source": (
        (
            "M227-D002-ART-01",
            "const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff = pipeline_result.sema_type_metadata_handoff;",
        ),
        ("M227-D002-ART-02", "BuildRuntimeShimHostLinkContract("),
        (
            "M227-D002-ART-03",
            "BuildRetainReleaseOperationLoweringContract(pipeline_result.sema_parity_surface);",
        ),
        (
            "M227-D002-ART-04",
            '<< (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")',
        ),
        ("M227-D002-ART-05", "ir_frontend_metadata.deterministic_runtime_shim_host_link_handoff ="),
        (
            "M227-D002-ART-06",
            "ir_frontend_metadata.deterministic_retain_release_operation_lowering_handoff =",
        ),
        (
            "M227-D002-ART-07",
            "ir_frontend_metadata.lowering_runtime_shim_host_link_replay_key = runtime_shim_host_link_replay_key;",
        ),
        ("M227-D002-ART-08", "ir_frontend_metadata.lowering_retain_release_operation_replay_key ="),
        ('M227-D002-ART-09', '<< ",\\"deterministic_type_metadata_handoff\\":"'),
        ('M227-D002-ART-10', '<< ",\\"deterministic_runtime_shim_host_link_handoff\\":"'),
        ('M227-D002-ART-11', '<< ",\\"deterministic_retain_release_operation_lowering_handoff\\":"'),
    ),
    "cmake_file": (
        ("M227-D002-BLD-01", "src/sema/objc3_sema_pass_flow_scaffold.cpp"),
    ),
    "build_script": (
        ("M227-D002-BLD-02", '"native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp"'),
    ),
    "contract_doc": (
        (
            "M227-D002-DOC-01",
            "Contract ID: `objc3c-runtime-facing-type-metadata-modular-split-scaffold/m227-d002-v1`",
        ),
        (
            "M227-D002-DOC-02",
            "Issue `#5148` defines canonical lane-D modular split/scaffolding scope.",
        ),
        (
            "M227-D002-DOC-03",
            "Dependencies: `M227-D001`",
        ),
        (
            "M227-D002-DOC-04",
            "m227_d002_runtime_facing_type_metadata_modular_split_packet.md",
        ),
        (
            "M227-D002-DOC-05",
            "`python scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`",
        ),
        (
            "M227-D002-DOC-06",
            "`python -m pytest tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py -q`",
        ),
        ("M227-D002-DOC-07", "`npm run check:objc3c:m227-d002-lane-d-readiness`"),
        (
            "M227-D002-DOC-08",
            "`tmp/reports/m227/M227-D002/runtime_facing_type_metadata_modular_split_contract_summary.json`",
        ),
        ("M227-D002-DOC-09", "`compile:objc3c`"),
        ("M227-D002-DOC-10", "`proof:objc3c`"),
        ("M227-D002-DOC-11", "`test:objc3c:execution-replay-proof`"),
        ("M227-D002-DOC-12", "`test:objc3c:perf-budget`"),
    ),
    "planning_doc": (
        ("M227-D002-PLN-01", "# M227-D002 Runtime-Facing Type Metadata Modular Split Packet"),
        ("M227-D002-PLN-02", "Packet: `M227-D002`"),
        ("M227-D002-PLN-03", "Issue: `#5148`"),
        ("M227-D002-PLN-04", "Freeze date: `2026-03-03`"),
        ("M227-D002-PLN-05", "Dependencies: `M227-D001`"),
        (
            "M227-D002-PLN-06",
            "scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py",
        ),
        (
            "M227-D002-PLN-07",
            "`check:objc3c:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract`",
        ),
        (
            "M227-D002-PLN-08",
            "`test:tooling:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract`",
        ),
        (
            "M227-D002-PLN-09",
            "`check:objc3c:m227-d002-lane-d-readiness`",
        ),
        (
            "M227-D002-PLN-10",
            "tmp/reports/m227/M227-D002/runtime_facing_type_metadata_modular_split_contract_summary.json",
        ),
        ("M227-D002-PLN-11", "`compile:objc3c`"),
        ("M227-D002-PLN-12", "`proof:objc3c`"),
        ("M227-D002-PLN-13", "`test:objc3c:execution-replay-proof`"),
        ("M227-D002-PLN-14", "`test:objc3c:perf-budget`"),
    ),
    "d001_contract_doc": (
        (
            "M227-D002-DEP-D001-01",
            "Contract ID: `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`",
        ),
    ),
    "d001_packet_doc": (
        ("M227-D002-DEP-D001-02", "Packet: `M227-D001`"),
        ("M227-D002-DEP-D001-03", "Dependencies: none"),
    ),
    "d001_checker": (
        ("M227-D002-DEP-D001-04", 'MODE = "m227-d001-runtime-facing-type-metadata-semantics-contract-v1"'),
    ),
    "d001_tooling_test": (
        ("M227-D002-DEP-D001-05", "check_m227_d001_runtime_facing_type_metadata_semantics_contract"),
    ),
    "architecture_doc": (
        (
            "M227-D002-ARCH-01",
            "M227 lane-D D002 runtime-facing type metadata modular split/scaffolding anchors",
        ),
        (
            "M227-D002-ARCH-02",
            "docs/contracts/m227_runtime_facing_type_metadata_modular_split_d002_expectations.md",
        ),
    ),
    "lowering_spec": (
        (
            "M227-D002-SPC-01",
            "runtime-facing type metadata modular split/scaffolding governance shall preserve explicit",
        ),
        (
            "M227-D002-SPC-02",
            "lane-D dependency anchors (`M227-D001`) and fail closed on sema handoff scaffold/pass-flow scaffold",
        ),
    ),
    "metadata_spec": (
        (
            "M227-D002-META-01",
            "deterministic lane-D runtime-facing type metadata modular split/scaffolding metadata anchors for `M227-D002`",
        ),
        (
            "M227-D002-META-02",
            "explicit `M227-D001` dependency continuity so sema scaffold/runtime metadata handoff drift fails closed",
        ),
    ),
    "package_json": (
        (
            "M227-D002-PKG-01",
            '"check:objc3c:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract": '
            '"python scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py"',
        ),
        (
            "M227-D002-PKG-02",
            '"test:tooling:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract": '
            '"python -m pytest tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py -q"',
        ),
        (
            "M227-D002-PKG-03",
            '"check:objc3c:m227-d002-lane-d-readiness": '
            '"npm run check:objc3c:m227-d001-lane-d-readiness '
            '&& npm run check:objc3c:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract '
            '&& npm run test:tooling:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract"',
        ),
        ("M227-D002-PKG-04", '"compile:objc3c": '),
        ("M227-D002-PKG-05", '"proof:objc3c": '),
        ("M227-D002-PKG-06", '"test:objc3c:execution-replay-proof": '),
        ("M227-D002-PKG-07", '"test:objc3c:perf-budget": '),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_manager_source": (
        ("M227-D002-MGR-FORB-01", "++result.sema_pass_flow_summary.executed_pass_count;"),
        ("M227-D002-MGR-FORB-02", "result.sema_pass_flow_summary.pass_execution_fingerprint ="),
        ("M227-D002-MGR-FORB-03", "result.sema_pass_flow_summary.deterministic_handoff_key ="),
    ),
}

SEMANTIC_CHECK_TOTAL = 4


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
            "tmp/reports/m227/M227-D002/runtime_facing_type_metadata_modular_split_contract_summary.json"
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


def collect_forbidden_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
        if snippet in text:
            findings.append(
                Finding(
                    artifact=artifact,
                    check_id=check_id,
                    detail=f"forbidden snippet present: {snippet}",
                )
            )
    return findings


def collect_semantic_findings(texts: dict[str, str]) -> list[Finding]:
    findings: list[Finding] = []

    scaffold_header = texts["parser_handoff_scaffold_header"]
    max_lines_match = re.search(
        r"inline constexpr std::size_t kObjc3ParserSemaHandoffScaffoldBuilderMaxLines = (\d+)u;",
        scaffold_header,
    )
    if max_lines_match is None:
        findings.append(
            Finding(
                artifact="parser_handoff_scaffold_header",
                check_id="M227-D002-SEM-01",
                detail="unable to parse kObjc3ParserSemaHandoffScaffoldBuilderMaxLines",
            )
        )
    else:
        max_lines = int(max_lines_match.group(1))
        if max_lines != 80:
            findings.append(
                Finding(
                    artifact="parser_handoff_scaffold_header",
                    check_id="M227-D002-SEM-02",
                    detail=(
                        "handoff scaffold builder max-lines constant drifted: "
                        f"expected 80, got {max_lines}"
                    ),
                )
            )

    manager_source = texts["sema_pass_manager_source"]
    handoff_idx = manager_source.find(
        "const Objc3ParserSemaHandoffScaffold handoff = BuildObjc3ParserSemaHandoffScaffold(input);"
    )
    type_metadata_idx = manager_source.find(
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);"
    )
    finalize_idx = manager_source.find(
        "FinalizeObjc3SemaPassFlowSummary(result.sema_pass_flow_summary,"
    )
    if handoff_idx < 0 or type_metadata_idx < 0 or finalize_idx < 0:
        findings.append(
            Finding(
                artifact="sema_pass_manager_source",
                check_id="M227-D002-SEM-03",
                detail="unable to locate sema handoff/type-metadata/finalize scaffold ordering anchors",
            )
        )
    elif not (handoff_idx < type_metadata_idx < finalize_idx):
        findings.append(
            Finding(
                artifact="sema_pass_manager_source",
                check_id="M227-D002-SEM-03",
                detail="expected scaffold ordering handoff -> type metadata -> finalize was not preserved",
            )
        )

    artifacts_source = texts["frontend_artifacts_source"]
    runtime_contract_idx = artifacts_source.find(
        "const Objc3RuntimeShimHostLinkContract runtime_shim_host_link_contract ="
    )
    runtime_projection_idx = artifacts_source.find(
        "ir_frontend_metadata.deterministic_runtime_shim_host_link_handoff ="
    )
    retain_contract_idx = artifacts_source.find(
        "const Objc3RetainReleaseOperationLoweringContract retain_release_operation_lowering_contract ="
    )
    retain_projection_idx = artifacts_source.find(
        "ir_frontend_metadata.deterministic_retain_release_operation_lowering_handoff ="
    )
    if (
        runtime_contract_idx < 0
        or runtime_projection_idx < 0
        or retain_contract_idx < 0
        or retain_projection_idx < 0
    ):
        findings.append(
            Finding(
                artifact="frontend_artifacts_source",
                check_id="M227-D002-SEM-04",
                detail="unable to locate runtime contract/projection ordering anchors",
            )
        )
    elif not (runtime_contract_idx < runtime_projection_idx and retain_contract_idx < retain_projection_idx):
        findings.append(
            Finding(
                artifact="frontend_artifacts_source",
                check_id="M227-D002-SEM-04",
                detail="runtime shim/retain-release contract projection ordering drifted",
            )
        )

    return findings


def total_checks() -> int:
    required_total = sum(len(snippets) for snippets in REQUIRED_SNIPPETS.values())
    forbidden_total = sum(len(snippets) for snippets in FORBIDDEN_SNIPPETS.values())
    return required_total + forbidden_total + SEMANTIC_CHECK_TOTAL


def finding_sort_key(finding: Finding) -> tuple[int, str, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in findings: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        texts = {artifact: load_text(path, artifact=artifact) for artifact, path in ARTIFACTS.items()}
        findings: list[Finding] = []
        for artifact, text in texts.items():
            findings.extend(collect_required_findings(artifact=artifact, text=text))
            findings.extend(collect_forbidden_findings(artifact=artifact, text=text))
        findings.extend(collect_semantic_findings(texts))
        findings = sorted(findings, key=finding_sort_key)
    except ValueError as exc:
        print(
            "m227-d002-runtime-facing-type-metadata-modular-split-scaffold: "
            f"error: {exc}",
            file=sys.stderr,
        )
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
            "m227-d002-runtime-facing-type-metadata-modular-split-scaffold: "
            f"contract drift detected ({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m227-d002-runtime-facing-type-metadata-modular-split-scaffold: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(
            "m227-d002-runtime-facing-type-metadata-modular-split-scaffold: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
