from __future__ import annotations

import argparse
import shutil
import subprocess
from pathlib import Path
from typing import Any

from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs
from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.validation import contains_all

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "reports" / "claimability" / "cross-module-semantic-contracts-diagnostics"
JSON_OUT = REPORT_DIR / "cross_module_semantic_contracts_diagnostics_summary.json"
MD_OUT = REPORT_DIR / "cross_module_semantic_contracts_diagnostics_summary.md"
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "cross-module-semantic-contracts-diagnostics"

CONTRACT_ID = "objc3c.cross_module.semantic.contracts.diagnostics.closure.v1"
ISSUE = "#8015"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "cross_module_semantic_contracts_diagnostics_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_cross_module_semantic_contracts_duplicate_module.objc3"
SEMANTIC_MANIFEST = ROOT / "tests" / "conformance" / "semantic" / "manifest.json"
SEMANTIC_README = ROOT / "tests" / "conformance" / "semantic" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "XMOD-8015-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "XMOD-8015-02.json"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMANTIC_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"

SUMMARY_FIELDS = [
    "module_import_graph_sites",
    "import_edge_candidate_sites",
    "namespace_segment_sites",
    "object_pointer_type_sites",
    "pointer_declarator_sites",
    "namespace_collision_shadowing_sites",
    "public_private_api_partition_sites",
    "incremental_module_cache_invalidation_sites",
    "cross_module_conformance_sites",
    "normalized_cross_module_sites",
    "cache_invalidation_candidate_sites",
    "diagnostic_recovery_sites",
    "diagnostic_emit_sites",
    "recovery_anchor_sites",
    "recovery_boundary_sites",
    "fail_closed_diagnostic_sites",
    "diagnostic_normalized_sites",
    "diagnostic_gate_blocked_sites",
    "interop_import_module_annotation_sites",
    "interop_imported_module_name_sites",
    "contract_violation_sites",
    "module_import_graph_semantics_landed",
    "namespace_collision_semantics_landed",
    "public_private_partition_semantics_landed",
    "incremental_cache_semantics_landed",
    "cross_module_conformance_semantics_landed",
    "diagnostic_recovery_semantics_landed",
    "interop_import_semantics_landed",
    "deterministic",
    "ready_for_lowering_and_runtime",
    "replay_key",
]

POSITIVE_MIN_COUNTS = {
    "module_import_graph_sites": 4,
    "import_edge_candidate_sites": 4,
    "namespace_segment_sites": 4,
    "object_pointer_type_sites": 7,
    "pointer_declarator_sites": 4,
    "namespace_collision_shadowing_sites": 4,
    "public_private_api_partition_sites": 4,
    "incremental_module_cache_invalidation_sites": 4,
    "cross_module_conformance_sites": 4,
    "normalized_cross_module_sites": 4,
    "interop_import_module_annotation_sites": 1,
    "interop_imported_module_name_sites": 1,
}

LANDED_FLAGS = [
    "module_import_graph_semantics_landed",
    "namespace_collision_semantics_landed",
    "public_private_partition_semantics_landed",
    "incremental_cache_semantics_landed",
    "cross_module_conformance_semantics_landed",
    "diagnostic_recovery_semantics_landed",
    "interop_import_semantics_landed",
]

REPLAY_SEGMENTS = [
    "module-import=",
    "namespace=",
    "api-partition=",
    "incremental-cache=",
    "cross-module=",
    "diagnostics=",
    "interop-import=",
    "violations=0",
]


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")




def run_compiler(source: Path, out_dir: Path) -> dict[str, Any]:
    if not COMPILER.is_file():
        raise SystemExit(f"missing native compiler at {rel(COMPILER)}; run scripts/build_objc3c_native.ps1 first")
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = subprocess.run(
        [str(COMPILER), str(source), "--out-dir", str(out_dir), "--emit-prefix", "module"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    diagnostics_path = out_dir / "module.diagnostics.json"
    manifest_path = out_dir / "module.manifest.json"
    llvm_ir_path = out_dir / "module.ll"
    diagnostics = []
    if diagnostics_path.is_file():
        diagnostics = load_json(diagnostics_path).get("diagnostics", [])
    manifest = load_json(manifest_path) if manifest_path.is_file() else None
    return {
        "source": rel(source),
        "out_dir": rel(out_dir),
        "exit_code": completed.returncode,
        "stdout": completed.stdout.strip(),
        "stderr": completed.stderr.strip(),
        "diagnostics_path": rel(diagnostics_path) if diagnostics_path.is_file() else None,
        "manifest_path": rel(manifest_path) if manifest_path.is_file() else None,
        "llvm_ir_path": rel(llvm_ir_path) if llvm_ir_path.is_file() else None,
        "diagnostics": diagnostics,
        "manifest": manifest,
    }


def find_model(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if node.get("contract_id") == CONTRACT_ID:
            return node
        for value in node.values():
            found = find_model(value)
            if found is not None:
                return found
    elif isinstance(node, list):
        for value in node:
            found = find_model(value)
            if found is not None:
                return found
    return None


def diagnostic_matches(diagnostics: list[dict[str, Any]], code: str, line: int, column: int) -> bool:
    return any(
        diag.get("code") == code
        and int(diag.get("line", -1)) == line
        and int(diag.get("column", -1)) == column
        for diag in diagnostics
    )


def build_summary() -> dict[str, Any]:
    positive_run = run_compiler(POSITIVE_FIXTURE, TMP_ROOT / "positive")
    negative_run = run_compiler(NEGATIVE_FIXTURE, TMP_ROOT / "negative-duplicate-module")
    model = find_model(positive_run.get("manifest")) if positive_run.get("manifest") else None
    replay_key = str((model or {}).get("replay_key", ""))

    manifest_text = read(SEMANTIC_MANIFEST)
    readme_text = read(SEMANTIC_README)
    stress_manifest_text = read(STRESS_MANIFEST)
    conformance_positive = load_json(CONFORMANCE_POSITIVE)
    conformance_negative = load_json(CONFORMANCE_NEGATIVE)

    static_presence = {
        "sema_contract": contains_all(read(SEMA_CONTRACT), ["Objc3CrossModuleSemanticContractsDiagnosticsSummary", "kObjc3CrossModuleSemanticContractsDiagnosticsContractId"] + SUMMARY_FIELDS),
        "semantic_passes_header": contains_all(read(SEMANTIC_PASSES_H), ["BuildCrossModuleSemanticContractsDiagnosticsSummary"]),
        "semantic_passes_cpp": contains_all(read(SEMANTIC_PASSES), ["BuildCrossModuleSemanticContractsDiagnosticsSummary", "module_import_graph_summary", "namespace_collision_shadowing_summary", "public_private_api_partition_summary", "incremental_module_cache_invalidation_summary", "cross_module_conformance_summary", "error_diagnostics_recovery_summary"] + REPLAY_SEGMENTS[:-1] + ["violations="]),
        "frontend_types": contains_all(read(FRONTEND_TYPES), ["cross_module_semantic_contracts_diagnostics_summary"]),
        "frontend_pipeline": contains_all(read(FRONTEND_PIPELINE), ["BuildCrossModuleSemanticContractsDiagnosticsSummary", "cross_module_semantic_contracts_diagnostics_summary"]),
        "frontend_artifacts": contains_all(read(FRONTEND_ARTIFACTS), ["BuildCrossModuleSemanticContractsDiagnosticsSummaryJson", "objc_cross_module_semantic_contracts_and_diagnostics", "cross_module_semantic_contracts_diagnostics_summary"] + SUMMARY_FIELDS),
        "lowering_contract": contains_all(read(LOWERING_CONTRACT), ["kObjc3ModuleImportGraphLoweringLaneContract", "kObjc3NamespaceCollisionShadowingLoweringLaneContract", "kObjc3PublicPrivateApiPartitionLoweringLaneContract", "kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract", "kObjc3CrossModuleConformanceLoweringLaneContract", "kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract"]),
        "ir_emitter_metadata": contains_all(read(IR_EMITTER_H), ["lowering_module_import_graph_replay_key", "lowering_namespace_collision_shadowing_replay_key", "lowering_public_private_api_partition_replay_key", "lowering_incremental_module_cache_invalidation_replay_key", "lowering_cross_module_conformance_replay_key", "lowering_error_diagnostics_recovery_replay_key"]),
    }

    source_truth_paths = [
        POSITIVE_FIXTURE,
        NEGATIVE_FIXTURE,
        CONFORMANCE_POSITIVE,
        CONFORMANCE_NEGATIVE,
        SEMANTIC_MANIFEST,
        SEMANTIC_README,
        STRESS_MANIFEST,
        SEMA_CONTRACT,
        SEMANTIC_PASSES_H,
        SEMANTIC_PASSES,
        FRONTEND_TYPES,
        FRONTEND_PIPELINE,
        FRONTEND_ARTIFACTS,
        LOWERING_CONTRACT,
        IR_EMITTER_H,
    ]

    checks = {
        "positive_fixture_compiles": positive_run["exit_code"] == 0,
        "positive_manifest_emitted": positive_run["manifest_path"] is not None,
        "positive_llvm_ir_emitted": positive_run["llvm_ir_path"] is not None,
        "manifest_has_cross_module_surface": model is not None,
        "all_summary_fields_emitted": bool(model) and all(field in model for field in SUMMARY_FIELDS),
        "positive_minimum_counts_observed": bool(model) and all(int(model.get(field, -1)) >= minimum for field, minimum in POSITIVE_MIN_COUNTS.items()),
        "positive_landed_flags_true": bool(model) and all(bool(model.get(flag)) for flag in LANDED_FLAGS),
        "positive_contract_violations_zero": bool(model) and int(model.get("contract_violation_sites", -1)) == 0,
        "positive_ready_and_deterministic": bool(model) and bool(model.get("deterministic")) and bool(model.get("ready_for_lowering_and_runtime")),
        "replay_key_covers_cross_module_axes": all(segment in replay_key for segment in REPLAY_SEGMENTS),
        "negative_fixture_fails_closed": negative_run["exit_code"] != 0,
        "negative_diagnostics_json_emitted": negative_run["diagnostics_path"] is not None,
        "negative_duplicate_module_diagnostic_observed": diagnostic_matches(negative_run["diagnostics"], "O3S200", 3, 8),
        "semantic_manifest_indexes_xmod_8015_01": "XMOD-8015-01.json" in manifest_text,
        "semantic_manifest_indexes_xmod_8015_02": "XMOD-8015-02.json" in manifest_text,
        "semantic_readme_mentions_issue_8015": "#8015" in readme_text,
        "semantic_readme_mentions_positive_fixture": rel(POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_negative_fixture": rel(NEGATIVE_FIXTURE) in readme_text,
        "positive_conformance_references_fixture": rel(POSITIVE_FIXTURE) in conformance_positive.get("references", []),
        "negative_conformance_references_fixture": rel(NEGATIVE_FIXTURE) in conformance_negative.get("references", []),
        "negative_conformance_expects_o3s200_location": conformance_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S200", "line": 3, "column": 8}],
        "stress_manifest_compiles_positive_fixture": rel(POSITIVE_FIXTURE) in stress_manifest_text,
        "no_tmp_source_truth": all(not rel(path).startswith("tmp/") for path in source_truth_paths),
        "static_sources_thread_surface": all(all(values.values()) for values in static_presence.values()),
    }
    status = "PASS" if all(checks.values()) else "FAIL"
    return {
        "contract_id": CONTRACT_ID,
        "issue": ISSUE,
        "status": status,
        "checks": checks,
        "static_presence": static_presence,
        "source_truth_paths": [rel(path) for path in source_truth_paths],
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "positive_compile": {key: value for key, value in positive_run.items() if key != "manifest"},
        "negative_compile": {key: value for key, value in negative_run.items() if key != "manifest"},
        "cross_module_semantic_contracts_diagnostics_model": model,
        "positive_minimum_counts": POSITIVE_MIN_COUNTS,
        "landed_flags": LANDED_FLAGS,
        "required_replay_key_segments": REPLAY_SEGMENTS,
        "validation_commands": [
            "python scripts/build_objc3c_cross_module_semantic_contracts_diagnostics.py --check",
            "python -m pytest tests/tooling/test_build_objc3c_cross_module_semantic_contracts_diagnostics.py",
            "npm run test:objc3c:negative-expectations",
            "npm run test:objc3c:execution-replay-proof",
            "npm run test:objc3c:lowering-runtime-stress",
            "npm run test:objc3c:full",
        ],
    }


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Cross-Module Semantic Contracts Diagnostics",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        f"- Negative fixture: `{summary['negative_fixture']}`",
        "",
        "## Checks",
    ]
    for name, passed in summary["checks"].items():
        lines.append(f"- `{name}`: `{'PASS' if passed else 'FAIL'}`")
    lines.extend(["", "## Observed Positive Counts"])
    model = summary.get("cross_module_semantic_contracts_diagnostics_model") or {}
    for field in summary["positive_minimum_counts"]:
        lines.append(f"- `{field}`: `{model.get(field)}`")
    lines.extend(["", "## Validation Commands"])
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)


def write_outputs(summary: dict[str, Any]) -> None:
    write_report_outputs(
        summary=summary,
        json_path=JSON_OUT,
        markdown_path=MD_OUT,
        markdown=render_markdown(summary),
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    add_check_argument(parser)
    args = parser.parse_args()
    summary = build_summary()
    expected_json = expected_json_report(summary)
    expected_md = render_markdown(summary)
    if args.check:
        if not JSON_OUT.is_file() or JSON_OUT.read_text(encoding="utf-8") != expected_json:
            raise SystemExit(f"{rel(JSON_OUT)} is stale; run this script without --check")
        if not MD_OUT.is_file() or MD_OUT.read_text(encoding="utf-8") != expected_md:
            raise SystemExit(f"{rel(MD_OUT)} is stale; run this script without --check")
        if summary["status"] != "PASS":
            raise SystemExit("cross-module semantic contracts diagnostics summary failed")
        print(f"status: {summary['status']}")
        print(f"summary_path: {rel(JSON_OUT)}")
        return 0
    write_outputs(summary)
    print(f"wrote: {rel(JSON_OUT)}")
    print(f"wrote: {rel(MD_OUT)}")
    print(f"status: {summary['status']}")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
