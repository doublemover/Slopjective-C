from __future__ import annotations

import argparse
import shutil
import subprocess
from pathlib import Path
from typing import Any

from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs
from objc3c_tooling.json_io import load_json_any as load_json

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "reports" / "claimability" / "type-semantic-model-closure"
JSON_OUT = REPORT_DIR / "type_semantic_model_closure_summary.json"
MD_OUT = REPORT_DIR / "type_semantic_model_closure_summary.md"
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "type-semantic-model-closure"

CONTRACT_ID = "objc3c.semantic.type-semantic-model-closure.v1"
ISSUE = "#8013"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "type_semantic_model_closure_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_duplicate_protocol_composition.objc3"
SEMANTIC_MANIFEST = ROOT / "tests" / "conformance" / "semantic" / "manifest.json"
SEMANTIC_README = ROOT / "tests" / "conformance" / "semantic" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-02.json"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"

SUMMARY_FIELDS = [
    "optional_binding_sites",
    "optional_binding_clause_sites",
    "guard_binding_sites",
    "optional_send_sites",
    "nil_coalescing_sites",
    "optional_propagation_sites",
    "optional_flow_refinement_sites",
    "guard_binding_exit_enforcement_sites",
    "typed_keypath_literal_sites",
    "typed_keypath_self_root_sites",
    "typed_keypath_class_root_sites",
    "object_pointer_semantic_sites",
    "protocol_composition_semantic_sites",
    "generic_suffix_semantic_sites",
    "generic_erasure_semantic_sites",
    "nullability_suffix_semantic_sites",
    "nullability_semantic_sites",
    "invalid_generic_suffix_semantic_sites",
    "invalid_nullability_suffix_semantic_sites",
    "invalid_protocol_composition_semantic_sites",
    "optional_binding_contract_violation_sites",
    "optional_send_contract_violation_sites",
    "optional_flow_contract_violation_sites",
    "typed_keypath_contract_violation_sites",
    "ready_for_lowering_and_runtime",
    "deterministic",
    "replay_key",
]

POSITIVE_MIN_COUNTS = {
    "optional_binding_sites": 2,
    "optional_binding_clause_sites": 2,
    "guard_binding_sites": 1,
    "optional_send_sites": 1,
    "nil_coalescing_sites": 1,
    "optional_propagation_sites": 1,
    "optional_flow_refinement_sites": 2,
    "guard_binding_exit_enforcement_sites": 1,
    "typed_keypath_literal_sites": 1,
    "typed_keypath_class_root_sites": 1,
    "object_pointer_semantic_sites": 7,
    "protocol_composition_semantic_sites": 4,
    "generic_suffix_semantic_sites": 4,
    "generic_erasure_semantic_sites": 4,
    "nullability_suffix_semantic_sites": 5,
    "nullability_semantic_sites": 5,
}

ZERO_FIELDS = [
    "invalid_generic_suffix_semantic_sites",
    "invalid_nullability_suffix_semantic_sites",
    "invalid_protocol_composition_semantic_sites",
    "optional_binding_contract_violation_sites",
    "optional_send_contract_violation_sites",
    "optional_flow_contract_violation_sites",
    "typed_keypath_contract_violation_sites",
]

REPLAY_KEY_SEGMENTS = [
    "bindings=",
    "guards=",
    "optional-sends=",
    "coalescing=",
    "propagation=",
    "refinement=",
    "guard-exit=",
    "keypaths=",
    "keypath-class=",
    "object-pointers=",
    "protocol-compositions=",
    "generic-suffixes=",
    "generic-erasure=",
    "nullability-suffixes=",
    "nullability=",
    "invalid-generic-suffixes=",
    "invalid-nullability-suffixes=",
    "invalid-protocol-compositions=",
]

STATIC_FIELD_TOKENS = [
    "object_pointer_semantic_sites",
    "protocol_composition_semantic_sites",
    "generic_suffix_semantic_sites",
    "nullability_suffix_semantic_sites",
    "invalid_generic_suffix_semantic_sites",
    "invalid_nullability_suffix_semantic_sites",
    "invalid_protocol_composition_semantic_sites",
]

SEMANTIC_PASS_TOKENS = [
    "BuildTypeSystemTypeSemanticModelSummary",
    "type_annotation_surface_summary.object_pointer_type_sites",
    "protocol_qualified_object_type_summary.protocol_composition_sites",
    "type_annotation_surface_summary.generic_suffix_sites",
    "generic_metadata_abi_summary.generic_metadata_abi_sites",
    "type_annotation_surface_summary.nullability_suffix_sites",
    "nullability_flow_warning_precision_summary.nullability_flow_sites",
    "type_annotation_surface_summary.invalid_generic_suffix_sites",
    "type_annotation_surface_summary.invalid_nullability_suffix_sites",
    "protocol_qualified_object_type_summary.contract_violation_sites",
]


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")



def contains_all(text: str, tokens: list[str]) -> dict[str, bool]:
    return {token: token in text for token in tokens}


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


def find_type_semantic_model_manifest(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if set(SUMMARY_FIELDS).issubset(node.keys()):
            return node
        for value in node.values():
            found = find_type_semantic_model_manifest(value)
            if found is not None:
                return found
    elif isinstance(node, list):
        for value in node:
            found = find_type_semantic_model_manifest(value)
            if found is not None:
                return found
    return None


def nested_semantic_model(manifest: dict[str, Any] | None) -> dict[str, Any] | None:
    if not isinstance(manifest, dict):
        return None
    node: Any = manifest
    for key in ["frontend", "pipeline", "semantic_surface", "objc_type_system_type_semantic_model"]:
        if not isinstance(node, dict) or key not in node:
            return find_type_semantic_model_manifest(manifest)
        node = node[key]
    return node if isinstance(node, dict) else find_type_semantic_model_manifest(manifest)


def diagnostic_matches(diagnostics: list[dict[str, Any]], code: str, line: int, column: int) -> bool:
    return any(
        diag.get("code") == code
        and int(diag.get("line", -1)) == line
        and int(diag.get("column", -1)) == column
        for diag in diagnostics
    )


def compile_positive_summary(run: dict[str, Any]) -> tuple[dict[str, Any] | None, dict[str, bool]]:
    model = nested_semantic_model(run.get("manifest"))
    if not model:
        return None, {"manifest_has_type_semantic_model": False}
    replay_key = str(model.get("replay_key", ""))
    checks = {
        "positive_fixture_compiles": run["exit_code"] == 0,
        "positive_manifest_emitted": run["manifest_path"] is not None,
        "positive_llvm_ir_emitted": run["llvm_ir_path"] is not None,
        "manifest_has_type_semantic_model": True,
        "all_summary_fields_emitted": all(field in model for field in SUMMARY_FIELDS),
        "ready_for_lowering_and_runtime": bool(model.get("ready_for_lowering_and_runtime")),
        "deterministic": bool(model.get("deterministic")),
        "minimum_positive_counts_observed": all(int(model.get(field, -1)) >= minimum for field, minimum in POSITIVE_MIN_COUNTS.items()),
        "positive_contract_violations_zero": all(int(model.get(field, -1)) == 0 for field in ZERO_FIELDS),
        "replay_key_covers_typed_surfaces": all(segment in replay_key for segment in REPLAY_KEY_SEGMENTS),
        "protocol_generic_nullability_counts_bound_to_object_pointers": int(model.get("protocol_composition_semantic_sites", -1)) <= int(model.get("object_pointer_semantic_sites", -2))
        and int(model.get("generic_suffix_semantic_sites", -1)) <= int(model.get("object_pointer_semantic_sites", -2))
        and int(model.get("nullability_suffix_semantic_sites", -1)) <= int(model.get("object_pointer_semantic_sites", -2)),
    }
    return model, checks


def build_summary() -> dict[str, Any]:
    positive_run = run_compiler(POSITIVE_FIXTURE, TMP_ROOT / "positive")
    negative_run = run_compiler(NEGATIVE_FIXTURE, TMP_ROOT / "negative-duplicate-protocol")
    model, positive_checks = compile_positive_summary(positive_run)

    sema_contract_text = read(SEMA_CONTRACT)
    semantic_passes_text = read(SEMANTIC_PASSES)
    artifacts_text = read(FRONTEND_ARTIFACTS)
    lowering_text = read(LOWERING_CONTRACT)
    ir_text = read(IR_EMITTER)
    manifest_text = read(SEMANTIC_MANIFEST)
    readme_text = read(SEMANTIC_README)
    stress_manifest_text = read(STRESS_MANIFEST)
    conformance_positive = load_json(CONFORMANCE_POSITIVE)
    conformance_negative = load_json(CONFORMANCE_NEGATIVE)

    static_presence = {
        "sema_contract_fields": contains_all(sema_contract_text, STATIC_FIELD_TOKENS),
        "semantic_pass_sources_and_replay_key": contains_all(semantic_passes_text, STATIC_FIELD_TOKENS + SEMANTIC_PASS_TOKENS + REPLAY_KEY_SEGMENTS),
        "artifact_json_fields": contains_all(artifacts_text, STATIC_FIELD_TOKENS),
        "lowering_contract_runtime_surface_present": contains_all(lowering_text, ["Lowering", "runtime"]),
        "ir_emitter_runtime_surface_present": contains_all(ir_text, ["Objc3", "Emit"]),
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
        SEMANTIC_PASSES,
        FRONTEND_ARTIFACTS,
        LOWERING_CONTRACT,
        IR_EMITTER,
    ]
    no_tmp_source_truth = all(not rel(path).startswith("tmp/") for path in source_truth_paths)

    negative_checks = {
        "negative_fixture_fails_closed": negative_run["exit_code"] != 0,
        "negative_diagnostics_json_emitted": negative_run["diagnostics_path"] is not None,
        "negative_duplicate_protocol_diagnostic_observed": diagnostic_matches(negative_run["diagnostics"], "O3S206", 7, 21),
    }

    conformance_checks = {
        "semantic_manifest_indexes_typ_8013_01": "TYP-8013-01.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_02": "TYP-8013-02.json" in manifest_text,
        "semantic_readme_mentions_issue_8013": "#8013" in readme_text,
        "semantic_readme_mentions_positive_fixture": rel(POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_negative_fixture": rel(NEGATIVE_FIXTURE) in readme_text,
        "positive_conformance_references_fixture": rel(POSITIVE_FIXTURE) in conformance_positive.get("references", []),
        "negative_conformance_references_fixture": rel(NEGATIVE_FIXTURE) in conformance_negative.get("references", []),
        "negative_conformance_expects_o3s206_location": conformance_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S206", "line": 7, "column": 21}],
        "stress_manifest_compiles_positive_fixture": rel(POSITIVE_FIXTURE) in stress_manifest_text,
        "no_tmp_source_truth": no_tmp_source_truth,
    }

    checks = {
        **positive_checks,
        **negative_checks,
        **conformance_checks,
        "static_sema_contract_fields_present": all(static_presence["sema_contract_fields"].values()),
        "static_semantic_pass_sources_present": all(static_presence["semantic_pass_sources_and_replay_key"].values()),
        "static_artifact_json_fields_present": all(static_presence["artifact_json_fields"].values()),
        "runtime_lowering_and_ir_source_refs_exist": LOWERING_CONTRACT.is_file() and IR_EMITTER.is_file(),
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
        "type_semantic_model": model,
        "required_summary_fields": SUMMARY_FIELDS,
        "positive_minimum_counts": POSITIVE_MIN_COUNTS,
        "zero_contract_violation_fields": ZERO_FIELDS,
        "required_replay_key_segments": REPLAY_KEY_SEGMENTS,
        "validation_commands": [
            "python scripts/build_objc3c_type_semantic_model_closure.py --check",
            "python -m pytest tests/tooling/test_build_objc3c_type_semantic_model_closure.py",
            "npm run test:objc3c:execution-replay-proof",
            "npm run test:objc3c:lowering-runtime-stress",
            "npm run test:objc3c:full",
        ],
    }


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Type Semantic Model Closure",
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
    model = summary.get("type_semantic_model") or {}
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
    parser.add_argument("--check", action="store_true")
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
            raise SystemExit("type semantic model closure summary failed")
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
