#!/usr/bin/env python3
"""Fail-closed architecture freeze checker for runtime-facing type metadata semantics (M227-D001)."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-d001-runtime-facing-type-metadata-semantics-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_contract_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h",
    "sema_pass_manager_contract_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager_contract.h",
    "frontend_pipeline_contract_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "frontend_pipeline_contract.h",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "frontend_artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_runtime_facing_type_metadata_semantics_expectations.md",
    "planning_doc": ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_d001_runtime_facing_type_metadata_semantics_contract_freeze.md",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

ARTIFACT_ORDER: tuple[str, ...] = tuple(ARTIFACTS.keys())
ARTIFACT_RANK = {name: index for index, name in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_contract_header": (
        ("M227-D001-SEM-REQ-01", "inline constexpr std::array<ValueType, 6> kObjc3CanonicalReferenceTypeForms = {"),
        ("M227-D001-SEM-REQ-02", "inline bool IsObjc3CanonicalMessageSendTypeForm(ValueType type) {"),
        ("M227-D001-SEM-REQ-03", "return IsObjc3CanonicalReferenceTypeForm(type);"),
        (
            "M227-D001-SEM-REQ-04",
            "inline constexpr const char *kObjc3RuntimeShimHostLinkDefaultDispatchSymbol =",
        ),
        ("M227-D001-SEM-REQ-05", "struct Objc3SemanticTypeMetadataHandoff {"),
        ("M227-D001-SEM-REQ-06", "Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;"),
        ("M227-D001-SEM-REQ-07", "Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;"),
        ("M227-D001-SEM-REQ-08", "Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;"),
        ("M227-D001-SEM-REQ-09", "Objc3RetainReleaseOperationSummary retain_release_operation_summary;"),
    ),
    "sema_pass_manager_contract_header": (
        ("M227-D001-PAR-01", "struct Objc3SemaParityContractSurface {"),
        ("M227-D001-PAR-02", "bool deterministic_runtime_shim_host_link_handoff = false;"),
        ("M227-D001-PAR-03", "bool deterministic_retain_release_operation_handoff = false;"),
        ("M227-D001-PAR-04", "Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;"),
        ("M227-D001-PAR-05", "Objc3RetainReleaseOperationSummary retain_release_operation_summary;"),
        ("M227-D001-PAR-06", "surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots =="),
        ("M227-D001-PAR-07", "surface.runtime_shim_host_link_summary.runtime_dispatch_symbol =="),
        ("M227-D001-PAR-08", "surface.deterministic_runtime_shim_host_link_handoff &&"),
        ("M227-D001-PAR-09", "surface.deterministic_retain_release_operation_handoff &&"),
    ),
    "frontend_pipeline_contract_header": (
        ("M227-D001-CON-01", "inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;"),
        ("M227-D001-CON-02", "inline constexpr const char *kRuntimeDispatchDefaultSymbol ="),
        ("M227-D001-CON-03", "std::size_t runtime_dispatch_arg_slots = kRuntimeDispatchDefaultArgs;"),
        ("M227-D001-CON-04", "std::string runtime_dispatch_symbol = kRuntimeDispatchDefaultSymbol;"),
        ("M227-D001-CON-05", 'std::string selector_global_ordering = "lexicographic";'),
    ),
    "frontend_types_header": (
        ("M227-D001-TYP-01", "Objc3SemanticTypeMetadataHandoff sema_type_metadata_handoff;"),
        ("M227-D001-TYP-02", "Objc3FrontendObjectPointerNullabilityGenericsSummary object_pointer_nullability_generics_summary;"),
        ("M227-D001-TYP-03", "Objc3FrontendSymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;"),
        ("M227-D001-TYP-04", "Objc3SemaParityContractSurface sema_parity_surface;"),
    ),
    "pipeline_source": (
        ("M227-D001-PIPE-01", "BuildObjectPointerNullabilityGenericsSummary(Objc3ParsedProgramAst(result.program));"),
        ("M227-D001-PIPE-02", "BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,"),
        ("M227-D001-PIPE-03", "BuildSymbolGraphScopeResolutionSummary(result.integration_surface,"),
        ("M227-D001-PIPE-04", "result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);"),
        ("M227-D001-PIPE-05", "result.sema_parity_surface = sema_result.parity_surface;"),
    ),
    "frontend_artifacts_source": (
        (
            "M227-D001-ART-01",
            "const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff = pipeline_result.sema_type_metadata_handoff;",
        ),
        ("M227-D001-ART-02", "BuildRuntimeShimHostLinkContract("),
        ("M227-D001-ART-03", "BuildOwnershipQualifierLoweringContract(pipeline_result.sema_parity_surface);"),
        ("M227-D001-ART-04", "BuildRetainReleaseOperationLoweringContract(pipeline_result.sema_parity_surface);"),
        ("M227-D001-ART-05", "deterministic_type_metadata_handoff"),
        ("M227-D001-ART-06", "deterministic_runtime_shim_host_link_handoff"),
        ("M227-D001-ART-07", "deterministic_retain_release_operation_lowering_handoff"),
        ("M227-D001-ART-08", "ir_frontend_metadata.deterministic_runtime_shim_host_link_handoff ="),
        ("M227-D001-ART-09", "ir_frontend_metadata.deterministic_retain_release_operation_lowering_handoff ="),
        ("M227-D001-ART-10", "ir_frontend_metadata.object_pointer_type_spellings ="),
        ("M227-D001-ART-11", "ir_frontend_metadata.deterministic_symbol_graph_scope_resolution_handoff_key ="),
    ),
    "contract_doc": (
        (
            "M227-D001-DOC-01",
            "# Runtime-Facing Type Metadata Semantics Contract and Architecture Freeze Expectations (M227-D001)",
        ),
        (
            "M227-D001-DOC-02",
            "Contract ID: `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`",
        ),
        ("M227-D001-DOC-03", "`M227-D001-INV-02`"),
        (
            "M227-D001-DOC-04",
            "`python scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`",
        ),
        (
            "M227-D001-DOC-05",
            "`python -m pytest tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py -q`",
        ),
        (
            "M227-D001-DOC-06",
            "`tmp/reports/m227/M227-D001/runtime_type_metadata_semantics_contract_summary.json`",
        ),
        (
            "M227-D001-DOC-07",
            "`check:objc3c:m227-d001-lane-d-readiness`",
        ),
        (
            "M227-D001-DOC-08",
            "`spec/LOWERING_AND_RUNTIME_CONTRACTS.md`",
        ),
        (
            "M227-D001-DOC-09",
            "`spec/MODULE_METADATA_AND_ABI_TABLES.md`",
        ),
    ),
    "planning_doc": (
        ("M227-D001-PLN-01", "# M227-D001 Runtime-Facing Type Metadata Semantics Contract Freeze Packet"),
        ("M227-D001-PLN-02", "Packet: `M227-D001`"),
        ("M227-D001-PLN-03", "Freeze date: `2026-03-03`"),
        ("M227-D001-PLN-04", "Dependencies: none"),
        ("M227-D001-PLN-05", "## Determinism Criteria"),
        (
            "M227-D001-PLN-06",
            "- Runtime dispatch symbol defaults are cross-layer consistent (`objc3_msgsend_i32`) between sema and pipeline contracts.",
        ),
        (
            "M227-D001-PLN-07",
            "`check:objc3c:m227-d001-lane-d-readiness`",
        ),
    ),
    "architecture_doc": (
        (
            "M227-D001-ARC-01",
            "architecture freeze anchors explicit lane-D contract-freeze artifacts in",
        ),
        (
            "M227-D001-ARC-02",
            "docs/contracts/m227_runtime_facing_type_metadata_semantics_expectations.md",
        ),
        (
            "M227-D001-ARC-03",
            "`check:objc3c:m227-d001-lane-d-readiness`",
        ),
    ),
    "lowering_spec": (
        (
            "M227-D001-SPC-01",
            "runtime-facing type metadata semantics governance shall preserve",
        ),
        (
            "M227-D001-SPC-02",
            "continuity (`M227-D001`).",
        ),
    ),
    "metadata_spec": (
        (
            "M227-D001-META-01",
            "deterministic lane-D runtime-facing type metadata metadata anchors for `M227-D001`",
        ),
        (
            "M227-D001-META-02",
            "continuity (`objc3_msgsend_i32`), and fail-closed sema/pipeline/artifact",
        ),
    ),
    "package_json": (
        (
            "M227-D001-PKG-01",
            '"check:objc3c:m227-d001-runtime-facing-type-metadata-semantics-contract": "python scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py"',
        ),
        (
            "M227-D001-PKG-02",
            '"test:tooling:m227-d001-runtime-facing-type-metadata-semantics-contract": "python -m pytest tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py -q"',
        ),
        (
            "M227-D001-PKG-03",
            '"check:objc3c:m227-d001-lane-d-readiness": "npm run check:objc3c:m227-d001-runtime-facing-type-metadata-semantics-contract && npm run test:tooling:m227-d001-runtime-facing-type-metadata-semantics-contract"',
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "pipeline_source": (
        ("M227-D001-PIPE-FORBID-01", "BuildSemanticTypeMetadataHandoff("),
    ),
}

CANONICAL_REFERENCE_TYPE_FORMS: tuple[str, ...] = (
    "ValueType::ObjCId",
    "ValueType::ObjCClass",
    "ValueType::ObjCSel",
    "ValueType::ObjCProtocol",
    "ValueType::ObjCInstancetype",
    "ValueType::ObjCObjectPtr",
)

SEMANTIC_CHECK_TOTAL = 3


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
        default=Path("tmp/reports/m227/M227-D001/runtime_type_metadata_semantics_contract_summary.json"),
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


def find_string_constant(text: str, constant_name: str) -> str | None:
    pattern = re.compile(
        rf'inline constexpr const char \*{re.escape(constant_name)}\s*=\s*"([^"]+)";'
    )
    match = pattern.search(text)
    if match is None:
        return None
    return match.group(1)


def collect_semantic_findings(texts: dict[str, str]) -> list[Finding]:
    findings: list[Finding] = []

    sema_header = texts["sema_contract_header"]
    forms_match = re.search(
        r"inline constexpr std::array<ValueType,\s*6>\s*kObjc3CanonicalReferenceTypeForms\s*=\s*\{(.*?)\};",
        sema_header,
        re.S,
    )
    if forms_match is None:
        findings.append(
            Finding(
                artifact="sema_contract_header",
                check_id="M227-D001-SEM-01",
                detail="unable to parse kObjc3CanonicalReferenceTypeForms",
            )
        )
    else:
        forms = tuple(re.findall(r"ValueType::[A-Za-z0-9_]+", forms_match.group(1)))
        if forms != CANONICAL_REFERENCE_TYPE_FORMS:
            findings.append(
                Finding(
                    artifact="sema_contract_header",
                    check_id="M227-D001-SEM-01",
                    detail=(
                        "canonical reference type form order/content drifted: "
                        f"expected {CANONICAL_REFERENCE_TYPE_FORMS}, got {forms}"
                    ),
                )
            )

    sema_symbol = find_string_constant(
        texts["sema_contract_header"],
        "kObjc3RuntimeShimHostLinkDefaultDispatchSymbol",
    )
    pipeline_symbol = find_string_constant(
        texts["frontend_pipeline_contract_header"],
        "kRuntimeDispatchDefaultSymbol",
    )
    if sema_symbol is None or pipeline_symbol is None:
        findings.append(
            Finding(
                artifact="frontend_pipeline_contract_header",
                check_id="M227-D001-SEM-02",
                detail="unable to parse runtime dispatch default symbol constants",
            )
        )
    elif sema_symbol != pipeline_symbol:
        findings.append(
            Finding(
                artifact="frontend_pipeline_contract_header",
                check_id="M227-D001-SEM-02",
                detail=(
                    "runtime dispatch symbol mismatch between sema and pipeline contracts: "
                    f"{sema_symbol} != {pipeline_symbol}"
                ),
            )
        )

    if sema_symbol != "objc3_msgsend_i32" or pipeline_symbol != "objc3_msgsend_i32":
        findings.append(
            Finding(
                artifact="frontend_pipeline_contract_header",
                check_id="M227-D001-SEM-03",
                detail=(
                    "runtime dispatch default symbol must remain objc3_msgsend_i32 "
                    f"(sema={sema_symbol}, pipeline={pipeline_symbol})"
                ),
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
        print(f"m227-d001-runtime-facing-type-metadata-semantics: error: {exc}", file=sys.stderr)
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
            "m227-d001-runtime-facing-type-metadata-semantics: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m227-d001-runtime-facing-type-metadata-semantics: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(f"m227-d001-runtime-facing-type-metadata-semantics: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
