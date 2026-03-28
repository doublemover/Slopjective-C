#!/usr/bin/env python3
"""Shared executable acceptance harness for live objc3c runtime validation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import asdict, dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

from check_objc3c_runtime_acceptance import (
    COMPILE_PROVENANCE_CONTRACT_ID,
    COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
    RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
)


ROOT = Path(__file__).resolve().parents[1]
HARNESS_REPORT_ROOT = ROOT / "tmp" / "reports" / "runtime" / "shared-executable-acceptance-harness"
HARNESS_CONTRACT_ID = "objc3c.shared.executable.acceptance.harness.v1"
HARNESS_SUMMARY_CONTRACT_ID = "objc3c.shared.executable.acceptance.harness.summary.v1"
CLAIM_BOUNDARY_CONTRACT_ID = "objc3c.runtime.execution.claim.boundary.v1"


@dataclass(frozen=True)
class SurfaceRequirement:
    key: str
    contract_id: str
    required_fields: tuple[str, ...] = ()


@dataclass(frozen=True)
class SuiteEntry:
    suite_id: str
    summary: str
    execution_kind: str
    command: tuple[str, ...]
    report_path: str
    guarantee_owner: str
    validation_owner: str
    required_surfaces: tuple[SurfaceRequirement, ...]


COMMON_SURFACES = (
    SurfaceRequirement("claim_boundary", CLAIM_BOUNDARY_CONTRACT_ID),
    SurfaceRequirement(
        "runtime_state_publication_surface",
        RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
        (
            "publication_surface_kind",
            "compile_artifact_set",
            "public_runtime_abi_boundary",
        ),
    ),
    SurfaceRequirement(
        "acceptance_suite_surface",
        RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
        (
            "suite_path",
            "report_path",
            "authoritative_claim_classes",
            "compile_output_provenance_contract_id",
            "compile_output_truthfulness_contract_id",
        ),
    ),
    SurfaceRequirement(
        "runtime_error_execution_cleanup_source_surface",
        RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_catch_filter_finalization_source_surface",
        RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_error_propagation_cleanup_semantics_surface",
        RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_bridging_filter_unwind_diagnostics_surface",
        RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_error_lowering_unwind_bridge_helper_surface",
        RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_error_runtime_abi_cleanup_surface",
        RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
        (
            "authoritative_case_ids",
            "authoritative_probe_path",
            "public_runtime_abi_boundary",
        ),
    ),
    SurfaceRequirement(
        "runtime_object_model_realization_source_surface",
        RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "private_object_model_query_boundary",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_block_arc_unified_source_surface",
        RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_ownership_transfer_capture_family_source_surface",
        RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
        (
            "block_capture_ownership_contract_id",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_block_arc_lowering_helper_surface",
        RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        (
            "runtime_block_arc_runtime_abi_surface_contract_id",
            "semantic_surface_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_block_arc_runtime_abi_surface",
        RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        (
            "block_arc_runtime_abi_snapshot_symbol",
            "arc_debug_state_snapshot_symbol",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_property_ivar_storage_accessor_source_surface",
        RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "storage_accessor_runtime_abi_surface",
        RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        (
            "property_registry_state_snapshot_symbol",
            "current_property_read_symbol",
            "weak_current_property_store_symbol",
        ),
    ),
    SurfaceRequirement(
        "runtime_property_ivar_accessor_reflection_implementation_surface",
        RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        (
            "implementation_snapshot_symbol",
            "property_registry_state_snapshot_symbol",
            "property_entry_snapshot_symbol",
        ),
    ),
    SurfaceRequirement(
        "runtime_property_atomicity_synthesis_reflection_source_surface",
        RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "authoritative_code_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_realization_lowering_reflection_artifact_surface",
        RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "lowering_artifact_boundary_model",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_dispatch_table_reflection_record_lowering_surface",
        RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "dispatch_table_lowering_model",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_cross_module_realized_metadata_replay_preservation_surface",
        RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "realized_metadata_replay_preservation_model",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_object_model_abi_query_surface",
        RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "private_object_model_query_boundary",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_realization_lookup_reflection_implementation_surface",
        RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        (
            "source_contract_ids",
            "object_model_query_state_snapshot_symbol",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_reflection_query_surface",
        RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
        (
            "query_api_boundary_model",
            "private_query_symbols",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_realization_lookup_semantics_surface",
        RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        (
            "private_lookup_query_boundary",
            "lookup_resolution_order_model",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_class_metaclass_protocol_realization_surface",
        RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        (
            "private_realization_query_boundary",
            "metaclass_lineage_model",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_category_attachment_merged_dispatch_surface",
        RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
        (
            "private_category_query_boundary",
            "merged_dispatch_resolution_model",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_reflection_visibility_coherence_diagnostics_surface",
        RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        (
            "private_coherence_query_boundary",
            "runtime_coherence_diagnostic_model",
            "authoritative_case_ids",
        ),
    ),
)


SUITES: tuple[SuiteEntry, ...] = (
    SuiteEntry(
        suite_id="runtime-acceptance",
        summary="direct runtime acceptance suite over the live compile and runtime path",
        execution_kind="direct-executable-suite",
        command=(sys.executable, "scripts/check_objc3c_runtime_acceptance.py"),
        report_path="tmp/reports/runtime/acceptance/summary.json",
        guarantee_owner="runtime acceptance and compile-coupled runtime publication proof",
        validation_owner="scripts/check_objc3c_runtime_acceptance.py",
        required_surfaces=COMMON_SURFACES,
    ),
    SuiteEntry(
        suite_id="public-test-fast",
        summary="composite fast public workflow carrying runtime acceptance surfaces forward",
        execution_kind="composite-executable-suite",
        command=(sys.executable, "scripts/objc3c_public_workflow_runner.py", "test-fast"),
        report_path="tmp/reports/objc3c-public-workflow/test-fast.json",
        guarantee_owner="bounded smoke, runtime acceptance, and replay through the public entrypoint",
        validation_owner="scripts/objc3c_public_workflow_runner.py",
        required_surfaces=COMMON_SURFACES,
    ),
    SuiteEntry(
        suite_id="public-test-full",
        summary="composite full developer workflow carrying runtime acceptance surfaces forward",
        execution_kind="composite-executable-suite",
        command=(sys.executable, "scripts/objc3c_public_workflow_runner.py", "test-full"),
        report_path="tmp/reports/objc3c-public-workflow/test-full.json",
        guarantee_owner="full developer validation without recovery fan-out",
        validation_owner="scripts/objc3c_public_workflow_runner.py",
        required_surfaces=COMMON_SURFACES,
    ),
)


SUITE_MAP = {entry.suite_id: entry for entry in SUITES}


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def emit_json(payload: object) -> int:
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_report(report_path: Path) -> dict[str, Any]:
    if not report_path.is_file():
        raise RuntimeError(f"expected suite report was not published: {repo_rel(report_path)}")
    try:
        payload = json.loads(report_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"invalid suite report JSON at {repo_rel(report_path)}: {exc}") from exc
    if not isinstance(payload, dict):
        raise RuntimeError(f"suite report at {repo_rel(report_path)} did not contain a JSON object")
    return payload


def require_surface(report: dict[str, Any], requirement: SurfaceRequirement) -> dict[str, Any]:
    surface = report.get(requirement.key)
    if not isinstance(surface, dict):
        raise RuntimeError(f"suite report did not publish {requirement.key}")
    if surface.get("contract_id") != requirement.contract_id:
        raise RuntimeError(
            f"suite report published the wrong contract for {requirement.key}: "
            f"{surface.get('contract_id')!r}"
        )
    for field in requirement.required_fields:
        if field not in surface:
            raise RuntimeError(f"suite report surface {requirement.key} is missing required field {field}")
    return surface


def validate_suite_report(entry: SuiteEntry, report: dict[str, Any]) -> dict[str, dict[str, Any]]:
    if report.get("status") != "PASS":
        raise RuntimeError(f"suite report status was not PASS for {entry.suite_id}")

    surfaces = {
        requirement.key: require_surface(report, requirement)
        for requirement in entry.required_surfaces
    }
    acceptance_suite_surface = surfaces["acceptance_suite_surface"]
    if (
        acceptance_suite_surface.get("compile_output_provenance_contract_id")
        != COMPILE_PROVENANCE_CONTRACT_ID
    ):
        raise RuntimeError("acceptance_suite_surface drifted from the compile provenance contract")
    if (
        acceptance_suite_surface.get("compile_output_truthfulness_contract_id")
        != COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID
    ):
        raise RuntimeError("acceptance_suite_surface drifted from the compile truthfulness contract")

    if entry.suite_id == "runtime-acceptance":
        if report.get("case_count", 0) <= 0:
            raise RuntimeError("runtime acceptance report did not publish any cases")
        if acceptance_suite_surface.get("suite_path") != "scripts/check_objc3c_runtime_acceptance.py":
            raise RuntimeError("runtime acceptance suite surface drifted from the runtime acceptance suite path")
        if acceptance_suite_surface.get("report_path") != entry.report_path:
            raise RuntimeError("runtime acceptance suite surface drifted from the expected report path")
    else:
        if report.get("runner_path") != "scripts/objc3c_public_workflow_runner.py":
            raise RuntimeError("composite suite report drifted from the public workflow runner path")
        steps = report.get("steps")
        if not isinstance(steps, list) or not steps:
            raise RuntimeError("composite suite report did not publish child steps")
        if acceptance_suite_surface.get("suite_path") != "scripts/check_objc3c_runtime_acceptance.py":
            raise RuntimeError("composite suite did not carry forward the runtime acceptance suite path")
        if acceptance_suite_surface.get("report_path") != "tmp/reports/runtime/acceptance/summary.json":
            raise RuntimeError("composite suite did not carry forward the runtime acceptance report path")

    return surfaces


def build_harness_surface(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    return {
        "contract_id": HARNESS_CONTRACT_ID,
        "harness_path": "scripts/shared_compiler_runtime_acceptance_harness.py",
        "suite_ids": [entry.suite_id for entry in selected],
        "requires_real_executable_suite_reports": True,
        "authoritative_child_report_contracts": [
            CLAIM_BOUNDARY_CONTRACT_ID,
            RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
            RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
            RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
            RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ],
        "shared_compile_truth_contracts": [
            COMPILE_PROVENANCE_CONTRACT_ID,
            COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
        ],
    }


def build_catalog_payload(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    return {
        "contract_id": HARNESS_CONTRACT_ID,
        "harness_path": "scripts/shared_compiler_runtime_acceptance_harness.py",
        "suite_count": len(selected),
        "suites": [asdict(entry) for entry in selected],
    }


def check_catalog(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    suite_results: list[dict[str, Any]] = []
    ok = True
    for entry in selected:
        command_results = []
        for token in entry.command[1:]:
            candidate = ROOT / token
            if candidate.exists():
                command_results.append({"path": token, "exists": True})
            elif token.startswith("scripts/") or token.startswith("tests/"):
                command_results.append({"path": token, "exists": False})
                ok = False
        report_parent = (ROOT / entry.report_path).parent
        report_parent_exists = report_parent.exists()
        ok = ok and report_parent_exists
        suite_results.append(
            {
                "suite_id": entry.suite_id,
                "command_paths": command_results,
                "report_parent": repo_rel(report_parent),
                "report_parent_exists": report_parent_exists,
            }
        )
    return {"ok": ok, "suite_results": suite_results}


def summarize_report(entry: SuiteEntry, report: dict[str, Any], surfaces: dict[str, dict[str, Any]]) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "suite_id": entry.suite_id,
        "summary": entry.summary,
        "execution_kind": entry.execution_kind,
        "validation_owner": entry.validation_owner,
        "guarantee_owner": entry.guarantee_owner,
        "report_path": entry.report_path,
        "claim_boundary": surfaces["claim_boundary"],
        "runtime_state_publication_surface": surfaces["runtime_state_publication_surface"],
        "acceptance_suite_surface": surfaces["acceptance_suite_surface"],
        "runtime_error_execution_cleanup_source_surface": surfaces[
            "runtime_error_execution_cleanup_source_surface"
        ],
        "runtime_catch_filter_finalization_source_surface": surfaces[
            "runtime_catch_filter_finalization_source_surface"
        ],
        "runtime_error_propagation_cleanup_semantics_surface": surfaces[
            "runtime_error_propagation_cleanup_semantics_surface"
        ],
        "runtime_bridging_filter_unwind_diagnostics_surface": surfaces[
            "runtime_bridging_filter_unwind_diagnostics_surface"
        ],
        "runtime_error_lowering_unwind_bridge_helper_surface": surfaces[
            "runtime_error_lowering_unwind_bridge_helper_surface"
        ],
        "runtime_error_runtime_abi_cleanup_surface": surfaces[
            "runtime_error_runtime_abi_cleanup_surface"
        ],
        "runtime_object_model_realization_source_surface": surfaces[
            "runtime_object_model_realization_source_surface"
        ],
        "runtime_block_arc_unified_source_surface": surfaces[
            "runtime_block_arc_unified_source_surface"
        ],
        "runtime_ownership_transfer_capture_family_source_surface": surfaces[
            "runtime_ownership_transfer_capture_family_source_surface"
        ],
        "runtime_block_arc_lowering_helper_surface": surfaces[
            "runtime_block_arc_lowering_helper_surface"
        ],
        "runtime_block_arc_runtime_abi_surface": surfaces[
            "runtime_block_arc_runtime_abi_surface"
        ],
        "runtime_property_ivar_storage_accessor_source_surface": surfaces[
            "runtime_property_ivar_storage_accessor_source_surface"
        ],
        "storage_accessor_runtime_abi_surface": surfaces[
            "storage_accessor_runtime_abi_surface"
        ],
        "runtime_property_ivar_accessor_reflection_implementation_surface": surfaces[
            "runtime_property_ivar_accessor_reflection_implementation_surface"
        ],
        "runtime_property_atomicity_synthesis_reflection_source_surface": surfaces[
            "runtime_property_atomicity_synthesis_reflection_source_surface"
        ],
        "runtime_realization_lowering_reflection_artifact_surface": surfaces[
            "runtime_realization_lowering_reflection_artifact_surface"
        ],
        "runtime_dispatch_table_reflection_record_lowering_surface": surfaces[
            "runtime_dispatch_table_reflection_record_lowering_surface"
        ],
        "runtime_cross_module_realized_metadata_replay_preservation_surface": surfaces[
            "runtime_cross_module_realized_metadata_replay_preservation_surface"
        ],
        "runtime_object_model_abi_query_surface": surfaces[
            "runtime_object_model_abi_query_surface"
        ],
        "runtime_realization_lookup_reflection_implementation_surface": surfaces[
            "runtime_realization_lookup_reflection_implementation_surface"
        ],
        "runtime_reflection_query_surface": surfaces["runtime_reflection_query_surface"],
        "runtime_realization_lookup_semantics_surface": surfaces[
            "runtime_realization_lookup_semantics_surface"
        ],
        "runtime_class_metaclass_protocol_realization_surface": surfaces[
            "runtime_class_metaclass_protocol_realization_surface"
        ],
        "runtime_category_attachment_merged_dispatch_surface": surfaces[
            "runtime_category_attachment_merged_dispatch_surface"
        ],
        "runtime_reflection_visibility_coherence_diagnostics_surface": surfaces[
            "runtime_reflection_visibility_coherence_diagnostics_surface"
        ],
    }
    for optional_surface_key in (
        "runtime_installation_abi_surface",
        "runtime_loader_lifecycle_surface",
    ):
        optional_surface = report.get(optional_surface_key)
        if isinstance(optional_surface, dict):
            payload[optional_surface_key] = optional_surface
    if entry.suite_id == "runtime-acceptance":
        payload["case_count"] = report.get("case_count", 0)
    else:
        payload["step_count"] = len(report.get("steps", []))
        payload["runner_path"] = report.get("runner_path")
    return payload


def run_suite(entry: SuiteEntry) -> dict[str, Any]:
    command = [str(token) for token in entry.command]
    result = run_capture(command)
    if result.returncode != 0:
        raise RuntimeError(f"suite execution failed for {entry.suite_id} with exit code {result.returncode}")
    report_path = ROOT / entry.report_path
    report = load_report(report_path)
    surfaces = validate_suite_report(entry, report)
    return summarize_report(entry, report, surfaces)


def write_summary(summary_out: Path, payload: dict[str, Any]) -> None:
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def default_summary_path(run_all: bool, suite_id: str | None) -> Path:
    if run_all:
        return HARNESS_REPORT_ROOT / "all-suites.json"
    if suite_id is None:
        raise RuntimeError("suite_id must be provided for single-suite summary path resolution")
    return HARNESS_REPORT_ROOT / suite_id / "summary.json"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--list-suites", action="store_true")
    parser.add_argument("--show-suite")
    parser.add_argument("--check-catalog", action="store_true")
    parser.add_argument("--check-roots", action="store_true")
    parser.add_argument("--run-suite")
    parser.add_argument("--run-all", action="store_true")
    parser.add_argument("--summary-out", type=Path)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    selected = list(SUITES)

    if args.show_suite:
        selected = [SUITE_MAP[args.show_suite]] if args.show_suite in SUITE_MAP else []
        if not selected:
            print(json.dumps({"ok": False, "error": f"unknown suite: {args.show_suite}"}, indent=2), file=sys.stderr)
            return 1

    if args.list_suites:
        return emit_json(build_catalog_payload(selected))

    if args.show_suite:
        return emit_json(build_catalog_payload(selected))

    if args.check_catalog or args.check_roots:
        payload = build_catalog_payload(selected)
        payload.update(check_catalog(selected))
        rendered = json.dumps(payload, indent=2)
        if args.summary_out is not None:
            write_summary(args.summary_out, payload)
        print(rendered)
        return 0 if payload["ok"] else 1

    if args.run_all and args.run_suite:
        print(json.dumps({"ok": False, "error": "choose either --run-suite or --run-all"}, indent=2), file=sys.stderr)
        return 2

    if args.run_suite:
        entry = SUITE_MAP.get(args.run_suite)
        if entry is None:
            print(json.dumps({"ok": False, "error": f"unknown suite: {args.run_suite}"}, indent=2), file=sys.stderr)
            return 1
        results = [run_suite(entry)]
        summary_out = args.summary_out or default_summary_path(False, entry.suite_id)
    elif args.run_all:
        results = [run_suite(entry) for entry in SUITES]
        summary_out = args.summary_out or default_summary_path(True, None)
    else:
        print(json.dumps({"ok": False, "error": "no mode selected"}, indent=2), file=sys.stderr)
        return 1

    payload = {
        "contract_id": HARNESS_SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "harness_surface": build_harness_surface(SUITES if args.run_all else [SUITE_MAP[results[0]["suite_id"]]]),
        "suite_count": len(results),
        "suites": results,
    }
    write_summary(summary_out, payload)
    print(f"summary_path: {repo_rel(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
