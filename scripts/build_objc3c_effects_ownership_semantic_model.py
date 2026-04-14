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
REPORT_DIR = ROOT / "reports" / "claimability" / "effects-ownership-semantic-model"
JSON_OUT = REPORT_DIR / "effects_ownership_semantic_model_summary.json"
MD_OUT = REPORT_DIR / "effects_ownership_semantic_model_summary.md"
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "effects-ownership-semantic-model"

CONTRACT_ID = "objc3c.effects.ownership.semantic.model.closure.v1"
ISSUE = "#8014"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "effects_ownership_semantic_model_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_effects_ownership_async_throws.objc3"
SEMANTIC_MANIFEST = ROOT / "tests" / "conformance" / "semantic" / "manifest.json"
SEMANTIC_README = ROOT / "tests" / "conformance" / "semantic" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "EFF-8014-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "EFF-8014-02.json"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMANTIC_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"

SUMMARY_FIELDS = [
    "arc_ownership_qualified_sites",
    "retain_insertion_sites",
    "release_insertion_sites",
    "autorelease_insertion_sites",
    "weak_zeroing_sites",
    "unowned_reference_sites",
    "weak_unowned_conflict_sites",
    "autoreleasepool_scope_sites",
    "cleanup_order_exit_sites",
    "block_literal_sites",
    "stack_to_heap_promotion_sites",
    "byref_forwarding_cell_sites",
    "copy_helper_required_sites",
    "dispose_helper_required_sites",
    "copy_helper_symbolized_sites",
    "dispose_helper_symbolized_sites",
    "captured_object_lifetime_sites",
    "throws_propagation_sites",
    "unwind_cleanup_sites",
    "bridged_error_sites",
    "nested_cleanup_sites",
    "foreign_boundary_sites",
    "async_continuation_sites",
    "continuation_resume_sites",
    "continuation_suspend_sites",
    "async_state_machine_sites",
    "cancellation_propagation_sites",
    "actor_isolation_sites",
    "actor_hop_sites",
    "sendability_check_sites",
    "reentrancy_policy_sites",
    "imported_actor_api_sites",
    "contract_violation_sites",
    "arc_semantics_landed",
    "block_escape_semantics_landed",
    "throws_cleanup_semantics_landed",
    "async_task_semantics_landed",
    "actor_semantics_landed",
    "foreign_boundary_semantics_landed",
    "deterministic",
    "ready_for_lowering_and_runtime",
    "replay_key",
]

POSITIVE_MIN_COUNTS = {
    "arc_ownership_qualified_sites": 2,
    "retain_insertion_sites": 1,
    "release_insertion_sites": 1,
    "weak_zeroing_sites": 1,
    "autoreleasepool_scope_sites": 2,
    "cleanup_order_exit_sites": 2,
    "block_literal_sites": 1,
    "stack_to_heap_promotion_sites": 1,
    "byref_forwarding_cell_sites": 1,
    "copy_helper_required_sites": 1,
    "dispose_helper_required_sites": 1,
    "captured_object_lifetime_sites": 3,
    "throws_propagation_sites": 2,
    "bridged_error_sites": 8,
    "foreign_boundary_sites": 7,
    "async_continuation_sites": 19,
    "cancellation_propagation_sites": 1,
    "reentrancy_policy_sites": 1,
    "imported_actor_api_sites": 1,
}

LANDED_FLAGS = [
    "arc_semantics_landed",
    "block_escape_semantics_landed",
    "throws_cleanup_semantics_landed",
    "async_task_semantics_landed",
    "actor_semantics_landed",
    "foreign_boundary_semantics_landed",
]

SOURCE_REPLAY_SEGMENTS = [
    "arc=",
    "weak=",
    "autoreleasepool=",
    "blocks=",
    "throws=",
    "async=",
    "actors=",
    "foreign=",
    "violations=",
]

RUNTIME_REPLAY_SEGMENTS = [
    *SOURCE_REPLAY_SEGMENTS[:-1],
    "violations=0",
]

CONTRACT_TOKENS = [
    "Objc3EffectsOwnershipSemanticModelSummary",
    "kObjc3EffectsOwnershipSemanticModelContractId",
    "kObjc3EffectsOwnershipSemanticModelSurfacePath",
]

SEMANTIC_PASS_TOKENS = [
    "BuildEffectsOwnershipSemanticModelSummary",
    "retain_release_operation_summary",
    "weak_unowned_semantics_summary",
    "autoreleasepool_scope_summary",
    "block_storage_escape_semantics_summary",
    "block_copy_dispose_semantics_summary",
    "throws_propagation_summary",
    "unwind_cleanup_summary",
    "ns_error_bridging_summary",
]

FRONTEND_ARTIFACT_TOKENS = [
    "BuildEffectsOwnershipSemanticModelSummaryJson",
    "objc_effects_ownership_semantic_model",
    "effects_ownership_semantic_model_summary",
]

LOWERING_CONTRACT_TOKENS = [
    "kObjc3RetainReleaseOperationLoweringLaneContract",
    "kObjc3AutoreleasePoolScopeLoweringLaneContract",
    "kObjc3WeakUnownedSemanticsLoweringLaneContract",
    "kObjc3BlockStorageEscapeLoweringLaneContract",
    "kObjc3BlockCopyDisposeLoweringLaneContract",
    "kObjc3ThrowsPropagationLoweringLaneContract",
    "kObjc3NSErrorBridgingLoweringLaneContract",
    "kObjc3UnwindCleanupLoweringLaneContract",
    "kObjc3AsyncContinuationLoweringLaneContract",
    "kObjc3ActorIsolationSendabilityLoweringLaneContract",
    "kObjc3ConcurrencyActorLoweringMetadataLaneContract",
    "kObjc3InteropForeignCallLifetimeLoweringDependencyContractId",
    "kObjc3RuntimeAllocateAsyncContinuationI32Symbol",
    "kObjc3RuntimeResumeAsyncContinuationI32Symbol",
]

IR_METADATA_TOKENS = [
    "lowering_retain_release_operation_replay_key",
    "lowering_autoreleasepool_scope_replay_key",
    "lowering_weak_unowned_semantics_replay_key",
    "lowering_block_storage_escape_replay_key",
    "lowering_block_copy_dispose_replay_key",
    "lowering_throws_propagation_replay_key",
    "lowering_ns_error_bridging_replay_key",
    "lowering_unwind_cleanup_replay_key",
    "lowering_async_continuation_replay_key",
    "lowering_actor_isolation_sendability_replay_key",
    "lowering_actor_lowering_metadata_replay_key",
    "lowering_interop_foreign_call_lifetime_replay_key",
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
    manifest = None
    if manifest_path.is_file():
        manifest = load_json(manifest_path)
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


def find_effects_model(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if node.get("contract_id") == CONTRACT_ID:
            return node
        for value in node.values():
            found = find_effects_model(value)
            if found is not None:
                return found
    elif isinstance(node, list):
        for value in node:
            found = find_effects_model(value)
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
    negative_run = run_compiler(NEGATIVE_FIXTURE, TMP_ROOT / "negative-async-throws")
    model = find_effects_model(positive_run.get("manifest")) if positive_run.get("manifest") else None
    replay_key = str((model or {}).get("replay_key", ""))

    manifest_text = read(SEMANTIC_MANIFEST)
    readme_text = read(SEMANTIC_README)
    stress_manifest_text = read(STRESS_MANIFEST)
    conformance_positive = load_json(CONFORMANCE_POSITIVE)
    conformance_negative = load_json(CONFORMANCE_NEGATIVE)

    static_presence = {
        "sema_contract": contains_all(read(SEMA_CONTRACT), CONTRACT_TOKENS + SUMMARY_FIELDS),
        "semantic_passes_header": contains_all(read(SEMANTIC_PASSES_H), ["BuildEffectsOwnershipSemanticModelSummary"]),
        "semantic_passes_cpp": contains_all(read(SEMANTIC_PASSES), SEMANTIC_PASS_TOKENS + SOURCE_REPLAY_SEGMENTS),
        "frontend_types": contains_all(read(FRONTEND_TYPES), ["effects_ownership_semantic_model_summary"]),
        "frontend_pipeline": contains_all(read(FRONTEND_PIPELINE), ["BuildEffectsOwnershipSemanticModelSummary", "effects_ownership_semantic_model_summary"]),
        "frontend_artifacts": contains_all(read(FRONTEND_ARTIFACTS), FRONTEND_ARTIFACT_TOKENS + SUMMARY_FIELDS),
        "lowering_contract": contains_all(read(LOWERING_CONTRACT), LOWERING_CONTRACT_TOKENS),
        "ir_emitter_metadata": contains_all(read(IR_EMITTER_H) + read(IR_EMITTER), IR_METADATA_TOKENS),
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
        IR_EMITTER,
    ]

    checks = {
        "positive_fixture_compiles": positive_run["exit_code"] == 0,
        "positive_manifest_emitted": positive_run["manifest_path"] is not None,
        "positive_llvm_ir_emitted": positive_run["llvm_ir_path"] is not None,
        "manifest_has_effects_ownership_surface": model is not None,
        "all_summary_fields_emitted": bool(model) and all(field in model for field in SUMMARY_FIELDS),
        "positive_minimum_counts_observed": bool(model) and all(int(model.get(field, -1)) >= minimum for field, minimum in POSITIVE_MIN_COUNTS.items()),
        "positive_landed_flags_true": bool(model) and all(bool(model.get(flag)) for flag in LANDED_FLAGS),
        "positive_contract_violations_zero": bool(model) and int(model.get("contract_violation_sites", -1)) == 0,
        "positive_ready_and_deterministic": bool(model) and bool(model.get("deterministic")) and bool(model.get("ready_for_lowering_and_runtime")),
        "replay_key_covers_effects_axes": all(segment in replay_key for segment in RUNTIME_REPLAY_SEGMENTS),
        "negative_fixture_fails_closed": negative_run["exit_code"] != 0,
        "negative_diagnostics_json_emitted": negative_run["diagnostics_path"] is not None,
        "negative_async_throws_diagnostic_observed": diagnostic_matches(negative_run["diagnostics"], "O3S226", 4, 10),
        "semantic_manifest_indexes_eff_8014_01": "EFF-8014-01.json" in manifest_text,
        "semantic_manifest_indexes_eff_8014_02": "EFF-8014-02.json" in manifest_text,
        "semantic_readme_mentions_issue_8014": "#8014" in readme_text,
        "semantic_readme_mentions_positive_fixture": rel(POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_negative_fixture": rel(NEGATIVE_FIXTURE) in readme_text,
        "positive_conformance_references_fixture": rel(POSITIVE_FIXTURE) in conformance_positive.get("references", []),
        "negative_conformance_references_fixture": rel(NEGATIVE_FIXTURE) in conformance_negative.get("references", []),
        "negative_conformance_expects_o3s226_location": conformance_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S226", "line": 4, "column": 10}],
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
        "effects_ownership_semantic_model": model,
        "positive_minimum_counts": POSITIVE_MIN_COUNTS,
        "landed_flags": LANDED_FLAGS,
        "required_replay_key_segments": RUNTIME_REPLAY_SEGMENTS,
        "validation_commands": [
            "python scripts/build_objc3c_effects_ownership_semantic_model.py --check",
            "python -m pytest tests/tooling/test_build_objc3c_effects_ownership_semantic_model.py",
            "npm run test:objc3c:execution-replay-proof",
            "npm run test:objc3c:lowering-runtime-stress",
            "npm run test:objc3c:full",
        ],
    }


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Effects Ownership Semantic Model",
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
    model = summary.get("effects_ownership_semantic_model") or {}
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
            raise SystemExit("effects ownership semantic model summary failed")
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
