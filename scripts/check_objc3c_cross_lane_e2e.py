#!/usr/bin/env python3
"""Validate the #8200 cross-lane end-to-end scaffold manifest."""

from __future__ import annotations

import sys
import os
import shutil
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from check_objc3c_advanced_runtime_closure import validate_advanced_runtime_closure
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "cross_lane_e2e" / "manifest.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "conformance" / "cross-lane-e2e-summary.json"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "cross-lane-e2e"
WORKFLOW_PATH = ROOT / ".github" / "workflows" / "conformance-minima.yml"
OPTIMIZATION_BEFORE_IR_PATH = ROOT / "tests" / "native" / "ir" / "optimization" / "semantic_pipeline_direct_dispatch.before.ll"
OPTIMIZATION_AFTER_IR_PATH = ROOT / "tests" / "native" / "ir" / "optimization" / "semantic_pipeline_direct_dispatch.after.ll"
ADVANCED_RUNTIME_FAMILY_ID = "advanced_runtime_closure"
ADVANCED_RUNTIME_PUBLIC_COMMAND = "npm run objc3c -- validate-advanced-runtime-closure"
ADVANCED_RUNTIME_POSITIVE_FIXTURE = "tests/native/runtime/advanced_closure/combined_positive.objc3"
ADVANCED_RUNTIME_NEGATIVE_MATRIX = "tests/native/runtime/advanced_closure/negative_matrix.contract.json"
ADVANCED_RUNTIME_COMBINED_IDENTITY_CONTRACT = (
    "tests/tooling/fixtures/advanced_runtime_closure/combined_runtime_identity_contract.json"
)
ADVANCED_RUNTIME_SOURCE_DEBUG_MAP_BUNDLE = (
    "tests/tooling/fixtures/advanced_runtime_closure/combined_runtime_source_debug_map.json"
)

MANIFEST_CONTRACT_ID = "objc3c.cross_lane_e2e.manifest.v1"
EXPECTATION_CONTRACT_ID = "objc3c.cross_lane_e2e.family_expectation.v1"
WORKSPACE_CONTRACT_ID = "objc3c.cross_lane_e2e.workspace.v1"
SUMMARY_CONTRACT_ID = "objc3c.cross_lane_e2e.summary.v1"
PUBLIC_ACTION = "validate-cross-lane-e2e"
PUBLIC_COMMAND = "npm run objc3c -- validate-cross-lane-e2e"

REQUIRED_FAMILY_IDS = (
    "text_collections_package",
    "object_reflection_debugger",
    "optimization_runtime_equivalence",
    "advanced_runtime_closure",
    "distribution_package_lifecycle",
)
REQUIRED_EXPECTATION_SECTIONS = (
    "diagnostics",
    "runtime",
    "compile_manifest",
    "source_graph",
    "debug_source_map",
    "optimization_trace",
    "negative_cases",
)
FORBIDDEN_SOURCE_PREFIXES = ("tmp/", "temp/", "generated/", "build/", "dist/")
ALLOWED_EXPECTATION_STATES = ("expected-pass", "scaffold-blocked", "reserved")
ALLOWED_SECTION_STATES = ("expected-pass", "scaffold-blocked", "reserved")
BLOCKED_STATES = ("scaffold-blocked", "reserved")


def fail(message: str) -> int:
    print(f"objc3c-cross-lane-e2e: FAIL\n- {message}", file=sys.stderr)
    return 1


def require_object(value: Any, field: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise RuntimeError(f"{field} must be an object")
    return value


def require_list(value: Any, field: str) -> list[Any]:
    if not isinstance(value, list):
        raise RuntimeError(f"{field} must be an array")
    return value


def require_nonempty_string(value: Any, field: str) -> str:
    if not isinstance(value, str) or not value:
        raise RuntimeError(f"{field} must be a non-empty string")
    return value


def normalize_path(path: str) -> str:
    return path.replace("\\", "/").strip("/")


def slug_from_family_id(family_id: str) -> str:
    return family_id.replace("_", "-")


def resolve_native_exe() -> Path:
    configured = os.environ.get("OBJC3C_NATIVE_EXECUTABLE")
    path = Path(configured) if configured else ROOT / "artifacts" / "bin" / "objc3c-native.exe"
    if not path.is_absolute():
        path = ROOT / path
    if not path.is_file():
        raise RuntimeError(f"runtime status proof requires native compiler at {repo_rel(path)}")
    return path


def resolve_clangxx() -> str:
    configured = os.environ.get("OBJC3C_NATIVE_EXECUTION_CLANG_PATH")
    if configured:
        return configured
    llvm_root = os.environ.get("LLVM_ROOT")
    if llvm_root:
        candidate = Path(llvm_root) / "bin" / "clang++.exe"
        if candidate.is_file():
            return str(candidate)
    return shutil.which("clang++") or "clang++"


def resolve_llc() -> str:
    configured = os.environ.get("OBJC3C_NATIVE_EXECUTION_LLC_PATH")
    if configured:
        return configured
    return shutil.which("llc") or "llc"


def link_driver_args() -> list[str]:
    args = ["-std=c++20"]
    if os.name == "nt":
        args.extend(
            [
                "-fms-runtime-lib=dll",
                "-fuse-ld=lld",
                "-Xlinker",
                "/MANIFEST:EMBED",
                "-Xlinker",
                "/MANIFESTUAC:level='asInvoker' uiAccess='false'",
            ]
        )
    return args


def run_checked(
    command: list[str],
    *,
    cwd: Path,
    log_path: Path,
    domain: str,
    check: bool = True,
) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(command, cwd=cwd, text=True, capture_output=True, check=False)
    log_path.parent.mkdir(parents=True, exist_ok=True)
    log_path.write_text(
        "command: " + " ".join(command) + "\n"
        + f"exit_code: {result.returncode}\n"
        + "stdout:\n" + result.stdout
        + "\nstderr:\n" + result.stderr,
        encoding="utf-8",
    )
    if check and result.returncode != 0:
        raise RuntimeError(
            f"{domain} proof command failed with exit {result.returncode}: {repo_rel(log_path)}"
        )
    return result


def require_artifact(path: Path, label: str) -> None:
    if not path.is_file():
        raise RuntimeError(f"runtime status proof missing {label}: {repo_rel(path)}")


def load_runtime_launch_inputs(compile_dir: Path) -> tuple[Path, list[str]]:
    registration_manifest_path = compile_dir / "module.runtime-registration-manifest.json"
    main_manifest_path = compile_dir / "module.manifest.json"
    require_artifact(registration_manifest_path, "runtime registration manifest")
    require_artifact(main_manifest_path, "compile manifest")
    registration_manifest = load_json(registration_manifest_path)
    main_manifest = load_json(main_manifest_path)

    if registration_manifest.get("launch_integration_ready") is not True:
        raise RuntimeError("runtime status proof requires launch_integration_ready=true")
    archive_rel = normalize_path(str(registration_manifest.get("runtime_support_library_archive_relative_path", "")))
    if not archive_rel:
        raise RuntimeError("runtime status proof requires runtime_support_library_archive_relative_path")
    runtime_library = ROOT / archive_rel
    require_artifact(runtime_library, "runtime support library")

    driver_flags = registration_manifest.get("driver_linker_flags")
    if not isinstance(driver_flags, list) or not all(isinstance(flag, str) and flag for flag in driver_flags):
        raise RuntimeError("runtime status proof requires non-empty driver_linker_flags")

    semantic_surface = (
        main_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_runtime_translation_unit_registration_manifest", {})
    )
    if semantic_surface.get("manifest_artifact_relative_path") != "module.runtime-registration-manifest.json":
        raise RuntimeError("compile manifest must bind the runtime registration manifest")
    return runtime_library, driver_flags


def compile_native_module(
    *,
    source_path: Path,
    compile_dir: Path,
    compile_log: Path,
    family_id: str,
    domain: str,
    import_surfaces: list[Path] | None = None,
    bootstrap_order_ordinal: int | None = None,
) -> dict[str, Any]:
    if compile_dir.exists():
        shutil.rmtree(compile_dir)
    compile_dir.mkdir(parents=True, exist_ok=True)
    native_exe = resolve_native_exe()
    command = [
        str(native_exe),
        str(source_path),
        "--out-dir",
        str(compile_dir),
        "--emit-prefix",
        "module",
    ]
    if bootstrap_order_ordinal is not None:
        command.extend(["--objc3-bootstrap-registration-order-ordinal", str(bootstrap_order_ordinal)])
    for surface_path in import_surfaces or []:
        command.extend(["--objc3-import-runtime-surface", str(surface_path)])
    command.extend(["--llc", resolve_llc()])

    run_checked(
        command,
        cwd=ROOT,
        log_path=compile_log,
        domain=f"{family_id}.{domain}",
    )

    obj_path = compile_dir / "module.obj"
    ll_path = compile_dir / "module.ll"
    import_surface_path = compile_dir / "module.runtime-import-surface.json"
    require_artifact(obj_path, "native object")
    require_artifact(ll_path, "LLVM IR")
    require_artifact(import_surface_path, "runtime import surface")
    runtime_library, driver_flags = load_runtime_launch_inputs(compile_dir)
    return {
        "source": repo_rel(source_path),
        "compile_dir": repo_rel(compile_dir),
        "compile_log": repo_rel(compile_log),
        "obj_path": obj_path,
        "ll_path": ll_path,
        "import_surface_path": import_surface_path,
        "runtime_library": runtime_library,
        "driver_flags": driver_flags,
    }


def validate_executable_runtime_proof(
    family_id: str,
    source_path: Path,
    expectation: dict[str, Any],
    workspace: dict[str, Any],
) -> dict[str, Any]:
    runtime = require_object(expectation["runtime"], f"{family_id}.runtime")
    expected_exit_code = runtime.get("expected_exit_code")
    if not isinstance(expected_exit_code, int):
        raise RuntimeError(f"{family_id}.runtime.expected_exit_code must be an integer for expected-pass")

    artifact_dir = ARTIFACT_ROOT / slug_from_family_id(family_id)
    if artifact_dir.exists():
        shutil.rmtree(artifact_dir)
    compile_dir = artifact_dir / "compile"
    compile_dir.mkdir(parents=True, exist_ok=True)

    provider_proofs: list[dict[str, Any]] = []
    provider_objects: list[Path] = []
    import_surfaces: list[Path] = []
    provider_driver_flags: list[str] = []
    provider_runtime_libraries: list[Path] = []
    package_modules = workspace.get("package_modules", [])
    if isinstance(package_modules, list):
        for index, raw_module in enumerate(package_modules):
            module = require_object(raw_module, f"{family_id}.workspace.package_modules[{index}]")
            module_name = require_nonempty_string(
                module.get("module_name"),
                f"{family_id}.workspace.package_modules[{index}].module_name",
            )
            provider_source = require_source_owned_path(
                str(module.get("source", "")),
                f"{family_id}.workspace.package_modules[{index}].source",
            )
            provider_dir = artifact_dir / "packages" / module_name
            provider_dir.mkdir(parents=True, exist_ok=True)
            provider_compile_log = provider_dir / "compile.log"
            provider_compile = compile_native_module(
                source_path=provider_source,
                compile_dir=provider_dir,
                compile_log=provider_compile_log,
                family_id=family_id,
                domain="package trust diagnostic",
                bootstrap_order_ordinal=index + 1,
            )
            provider_objects.append(provider_compile["obj_path"])
            import_surfaces.append(provider_compile["import_surface_path"])
            provider_driver_flags.extend(provider_compile["driver_flags"])
            provider_runtime_libraries.append(provider_compile["runtime_library"])
            provider_proofs.append(
                {
                    "module_name": module_name,
                    "source": provider_compile["source"],
                    "compile_dir": provider_compile["compile_dir"],
                    "compile_log": provider_compile["compile_log"],
                    "runtime_import_surface": repo_rel(provider_compile["import_surface_path"]),
                    "object": repo_rel(provider_compile["obj_path"]),
                }
            )

    compile_log = artifact_dir / "compile.log"
    main_compile = compile_native_module(
        source_path=source_path,
        compile_dir=compile_dir,
        compile_log=compile_log,
        family_id=family_id,
        domain="lowering/IR diagnostic",
        import_surfaces=import_surfaces,
        bootstrap_order_ordinal=len(provider_objects) + 1,
    )

    obj_path = main_compile["obj_path"]
    runtime_library = main_compile["runtime_library"]
    driver_flags = list(main_compile["driver_flags"])
    for provider_runtime_library in provider_runtime_libraries:
        if provider_runtime_library != runtime_library:
            raise RuntimeError(
                f"{family_id}.package trust diagnostic resolved mismatched runtime support library"
            )
    all_driver_flags = [*driver_flags, *provider_driver_flags]

    exe_path = artifact_dir / "module.exe"
    link_log = artifact_dir / "link.log"
    run_checked(
        [
            resolve_clangxx(),
            *link_driver_args(),
            str(obj_path),
            *[str(provider_obj) for provider_obj in provider_objects],
            str(runtime_library),
            *all_driver_flags,
            "-o",
            str(exe_path),
            "-fno-color-diagnostics",
        ],
        cwd=ROOT,
        log_path=link_log,
        domain=f"{family_id}.runtime status",
    )
    require_artifact(exe_path, "linked executable")

    run_log = artifact_dir / "run.log"
    result = run_checked(
        [str(exe_path)],
        cwd=ROOT,
        log_path=run_log,
        domain=f"{family_id}.runtime status",
        check=False,
    )
    if result.returncode != expected_exit_code:
        raise RuntimeError(
            f"{family_id}.runtime status expected exit {expected_exit_code} but got {result.returncode}"
        )

    required_artifacts = runtime.get("required_artifacts", [])
    if isinstance(required_artifacts, list):
        for artifact in required_artifacts:
            artifact_name = require_nonempty_string(artifact, f"{family_id}.runtime.required_artifacts[]")
            candidate = exe_path if artifact_name == "module.exe" else compile_dir / artifact_name
            require_artifact(candidate, artifact_name)

    return {
        "status": "PASS",
        "expected_exit_code": expected_exit_code,
        "actual_exit_code": result.returncode,
        "compile_dir": repo_rel(compile_dir),
        "executable": repo_rel(exe_path),
        "compile_log": repo_rel(compile_log),
        "link_log": repo_rel(link_log),
        "run_log": repo_rel(run_log),
        "package_module_proofs": provider_proofs,
    }


def validate_optimization_trace_proof(
    family_id: str,
    expectation: dict[str, Any],
    executable_proof: dict[str, Any],
) -> dict[str, Any]:
    trace = require_object(expectation["optimization_trace"], f"{family_id}.optimization_trace")
    expected_path = require_nonempty_string(trace.get("expected_path"), f"{family_id}.optimization_trace.expected_path")
    trace_path = ROOT / normalize_path(expected_path)
    compile_dir = ROOT / normalize_path(require_nonempty_string(executable_proof.get("compile_dir"), f"{family_id}.compile_dir"))
    ll_path = compile_dir / "module.ll"
    require_artifact(ll_path, "optimization runtime LLVM IR")
    require_artifact(OPTIMIZATION_BEFORE_IR_PATH, "semantic optimization before IR reference")
    require_artifact(OPTIMIZATION_AFTER_IR_PATH, "semantic optimization after IR reference")

    ll_text = ll_path.read_text(encoding="utf-8")
    required_ir_tokens = {
        "direct_exact_call": "call i32 @objc3_method_CrossLaneOptimizedCounter_class_exactValue()",
        "direct_candidate_call": "call i32 @objc3_method_CrossLaneOptimizedCounter_class_inlineCandidate_",
        "cache_prepare": "objc3_runtime_prepare_cache_aware_dispatch_descriptor",
        "cache_checked_dispatch": "objc3_runtime_cache_aware_dispatch_i32_checked",
        "cache_source_map_anchor": "source-map.cache-aware-dispatch",
        "cache_optimization_anchor": "semantic-optimization.cache-aware-dispatch",
    }
    missing = [label for label, token in required_ir_tokens.items() if token not in ll_text]
    if missing:
        raise RuntimeError(
            f"{family_id}.optimization proof diagnostic missing IR tokens: " + ", ".join(missing)
        )

    payload = {
        "contract_id": "objc3c.cross_lane_e2e.optimization_trace.v1",
        "schema_version": 1,
        "issue": 8200,
        "family_id": family_id,
        "status": "PASS",
        "source_truth": False,
        "source": expectation["source"],
        "runtime_equivalence_exit_code": executable_proof["actual_exit_code"],
        "generated_ir": repo_rel(ll_path),
        "before_ir_reference": repo_rel(OPTIMIZATION_BEFORE_IR_PATH),
        "after_ir_reference": repo_rel(OPTIMIZATION_AFTER_IR_PATH),
        "direct_dispatch_evidence": {
            "exact_devirtualization_candidate": required_ir_tokens["direct_exact_call"],
            "method_candidate_site": required_ir_tokens["direct_candidate_call"],
        },
        "cache_aware_dispatch_evidence": {
            "prepare_descriptor": required_ir_tokens["cache_prepare"],
            "checked_dispatch": required_ir_tokens["cache_checked_dispatch"],
            "source_map_anchor": required_ir_tokens["cache_source_map_anchor"],
            "optimization_anchor": required_ir_tokens["cache_optimization_anchor"],
        },
        "reserved_rows_not_promoted": [
            "compiler.optimization.method-inlining"
        ],
    }
    write_json_file(trace_path, payload)
    return {
        "status": "PASS",
        "trace": repo_rel(trace_path),
        "generated_ir": repo_rel(ll_path),
        "before_ir_reference": repo_rel(OPTIMIZATION_BEFORE_IR_PATH),
        "after_ir_reference": repo_rel(OPTIMIZATION_AFTER_IR_PATH),
        "reserved_rows_not_promoted": payload["reserved_rows_not_promoted"],
    }


def require_expected_pass_section(section: dict[str, Any], field: str) -> None:
    if section.get("status") != "expected-pass":
        raise RuntimeError(f"{field}.status must be expected-pass for contract-backed proof")


def require_blocked_section(section: dict[str, Any], field: str) -> None:
    if section.get("status") not in BLOCKED_STATES:
        raise RuntimeError(f"{field}.status must remain blocker-explicit")


def require_path_field(section: dict[str, Any], field: str, expected_path: str) -> Path:
    actual = normalize_path(require_nonempty_string(section.get(field), field))
    expected = normalize_path(expected_path)
    if actual != expected:
        raise RuntimeError(f"{field} drifted: expected {expected}, got {actual}")
    return require_source_owned_path(actual, field)


def require_int_field(section: dict[str, Any], field: str, expected: int) -> None:
    actual = section.get(field)
    if actual != expected:
        raise RuntimeError(f"{field} must be {expected}, got {actual!r}")


def validate_advanced_runtime_contract_backed_proof(
    family_id: str,
    expectation: dict[str, Any],
    workspace: dict[str, Any],
) -> dict[str, Any]:
    evidence = require_object(
        workspace.get("canonical_production_evidence"),
        f"{family_id}.workspace.canonical_production_evidence",
    )
    if evidence.get("public_replay_command") != ADVANCED_RUNTIME_PUBLIC_COMMAND:
        raise RuntimeError(f"{family_id}.workspace canonical evidence command drifted")
    if evidence.get("umbrella_support_promoted") is not False:
        raise RuntimeError(f"{family_id}.workspace must not promote advanced-runtime umbrella support")
    require_path_field(evidence, "positive_fixture", ADVANCED_RUNTIME_POSITIVE_FIXTURE)
    require_path_field(
        evidence,
        "combined_identity_contract",
        ADVANCED_RUNTIME_COMBINED_IDENTITY_CONTRACT,
    )
    require_path_field(
        evidence,
        "canonical_source_debug_map_bundle",
        ADVANCED_RUNTIME_SOURCE_DEBUG_MAP_BUNDLE,
    )
    require_path_field(evidence, "negative_matrix", ADVANCED_RUNTIME_NEGATIVE_MATRIX)

    expected_evidence = require_object(
        expectation.get("canonical_production_evidence"),
        f"{family_id}.canonical_production_evidence",
    )
    if expected_evidence != evidence:
        raise RuntimeError(f"{family_id}.canonical_production_evidence drifted from workspace")

    diagnostics = require_object(expectation["diagnostics"], f"{family_id}.diagnostics")
    runtime = require_object(expectation["runtime"], f"{family_id}.runtime")
    compile_manifest = require_object(expectation["compile_manifest"], f"{family_id}.compile_manifest")
    source_graph = require_object(expectation["source_graph"], f"{family_id}.source_graph")
    debug_source_map = require_object(expectation["debug_source_map"], f"{family_id}.debug_source_map")
    optimization_trace = require_object(expectation["optimization_trace"], f"{family_id}.optimization_trace")

    require_expected_pass_section(diagnostics, f"{family_id}.diagnostics")
    require_expected_pass_section(runtime, f"{family_id}.runtime")
    require_expected_pass_section(source_graph, f"{family_id}.source_graph")
    require_expected_pass_section(debug_source_map, f"{family_id}.debug_source_map")
    require_blocked_section(compile_manifest, f"{family_id}.compile_manifest")
    require_blocked_section(optimization_trace, f"{family_id}.optimization_trace")

    if runtime.get("proof_kind") != "advanced-runtime-combined-runtime-state":
        raise RuntimeError(f"{family_id}.runtime.proof_kind drifted")
    if source_graph.get("proof_kind") != "advanced-runtime-compiler-owned-source-graph":
        raise RuntimeError(f"{family_id}.source_graph.proof_kind drifted")
    if debug_source_map.get("proof_kind") != "advanced-runtime-canonical-source-debug-map":
        raise RuntimeError(f"{family_id}.debug_source_map.proof_kind drifted")
    if runtime.get("umbrella_support_promoted") is not False:
        raise RuntimeError(f"{family_id}.runtime must not promote advanced-runtime umbrella support")
    if source_graph.get("source_truth") is True or debug_source_map.get("source_truth") is True:
        raise RuntimeError(f"{family_id} generated evidence must not be marked source truth")

    require_path_field(runtime, "combined_identity_contract", ADVANCED_RUNTIME_COMBINED_IDENTITY_CONTRACT)
    require_path_field(source_graph, "combined_identity_contract", ADVANCED_RUNTIME_COMBINED_IDENTITY_CONTRACT)
    require_path_field(debug_source_map, "canonical_bundle", ADVANCED_RUNTIME_SOURCE_DEBUG_MAP_BUNDLE)
    require_path_field(diagnostics, "negative_matrix", ADVANCED_RUNTIME_NEGATIVE_MATRIX)

    payload = validate_advanced_runtime_closure()
    if payload.get("status") != "PASS":
        failures = payload.get("failures", [])
        failure_text = "; ".join(str(item) for item in failures) if failures else "unknown failure"
        raise RuntimeError(f"{family_id}.advanced-runtime closure proof failed: {failure_text}")

    expected_counts = {
        "runtime.runtime_state_record_count": (
            runtime,
            "runtime_state_record_count",
            "advanced_runtime_combined_identity_runtime_state_record_count",
        ),
        "source_graph.source_graph_record_count": (
            source_graph,
            "source_graph_record_count",
            "advanced_runtime_combined_identity_source_graph_record_count",
        ),
        "debug_source_map.source_map_record_count": (
            debug_source_map,
            "source_map_record_count",
            "advanced_runtime_canonical_source_map_record_count",
        ),
        "debug_source_map.debug_map_record_count": (
            debug_source_map,
            "debug_map_record_count",
            "advanced_runtime_canonical_debug_map_record_count",
        ),
        "debug_source_map.native_line_table_record_count": (
            debug_source_map,
            "native_line_table_record_count",
            "advanced_runtime_canonical_native_line_table_record_count",
        ),
        "diagnostics.negative_matrix_case_count": (
            diagnostics,
            "negative_matrix_case_count",
            "advanced_runtime_negative_matrix_case_count",
        ),
    }
    for field_label, (section, field_name, payload_key) in expected_counts.items():
        expected = payload.get(payload_key)
        if not isinstance(expected, int):
            raise RuntimeError(f"{family_id}.{payload_key} was not reported as an integer")
        require_int_field(section, field_name, expected)
        if field_label.endswith("negative_matrix_case_count"):
            for index, raw_case in enumerate(require_list(expectation["negative_cases"], f"{family_id}.negative_cases")):
                case = require_object(raw_case, f"{family_id}.negative_cases[{index}]")
                if case.get("case_id") == "advanced-runtime-unsupported-combination-matrix":
                    require_int_field(case, "expected_case_count", expected)

    if payload.get("advanced_runtime_combined_identity_contract") != ADVANCED_RUNTIME_COMBINED_IDENTITY_CONTRACT:
        raise RuntimeError(f"{family_id}.combined identity contract path drifted")
    if payload.get("advanced_runtime_canonical_source_debug_map") != ADVANCED_RUNTIME_SOURCE_DEBUG_MAP_BUNDLE:
        raise RuntimeError(f"{family_id}.canonical source/debug-map bundle path drifted")
    if payload.get("advanced_runtime_negative_matrix") != ADVANCED_RUNTIME_NEGATIVE_MATRIX:
        raise RuntimeError(f"{family_id}.negative matrix path drifted")
    if payload.get("language_semantics_support_claim") != "objc3c.behavior.language.advanced-runtime-closure":
        raise RuntimeError(f"{family_id}.language semantics support claim drifted")

    return {
        "status": "PASS",
        "public_command": ADVANCED_RUNTIME_PUBLIC_COMMAND,
        "combined_identity_contract": ADVANCED_RUNTIME_COMBINED_IDENTITY_CONTRACT,
        "canonical_source_debug_map": ADVANCED_RUNTIME_SOURCE_DEBUG_MAP_BUNDLE,
        "negative_matrix": ADVANCED_RUNTIME_NEGATIVE_MATRIX,
        "runtime_state_record_count": payload["advanced_runtime_combined_identity_runtime_state_record_count"],
        "source_graph_record_count": payload["advanced_runtime_combined_identity_source_graph_record_count"],
        "debug_map_record_count": payload["advanced_runtime_combined_identity_debug_map_record_count"],
        "canonical_source_map_record_count": payload["advanced_runtime_canonical_source_map_record_count"],
        "canonical_native_line_table_record_count": payload[
            "advanced_runtime_canonical_native_line_table_record_count"
        ],
        "negative_matrix_case_count": payload["advanced_runtime_negative_matrix_case_count"],
        "compile_manifest_status": compile_manifest["status"],
        "optimization_trace_status": optimization_trace["status"],
        "umbrella_support_promoted": False,
    }


def require_source_owned_path(relative_path: str, field: str) -> Path:
    path_text = normalize_path(require_nonempty_string(relative_path, field))
    if path_text.startswith(FORBIDDEN_SOURCE_PREFIXES):
        raise RuntimeError(f"{field} uses generated or transient source truth: {path_text}")
    path = ROOT / path_text
    if not path.is_file():
        raise RuntimeError(f"{field} is missing: {path_text}")
    return path


def require_blocker_list(value: Any, field: str) -> list[dict[str, Any]]:
    blockers = require_list(value, field)
    if not blockers:
        raise RuntimeError(f"{field} must name at least one explicit blocker")
    normalized: list[dict[str, Any]] = []
    for index, raw_blocker in enumerate(blockers):
        blocker = require_object(raw_blocker, f"{field}[{index}]")
        require_nonempty_string(blocker.get("blocker_id"), f"{field}[{index}].blocker_id")
        require_nonempty_string(blocker.get("domain"), f"{field}[{index}].domain")
        require_nonempty_string(blocker.get("reason"), f"{field}[{index}].reason")
        normalized.append(blocker)
    return normalized


def validate_workspace(family_id: str, workspace_path: Path, expected_source: str) -> dict[str, Any]:
    workspace = load_json(workspace_path)
    if workspace.get("contract_id") != WORKSPACE_CONTRACT_ID:
        raise RuntimeError(f"{repo_rel(workspace_path)} contract_id drifted")
    if workspace.get("schema_version") != 1:
        raise RuntimeError(f"{repo_rel(workspace_path)} schema_version drifted")
    if workspace.get("family_id") != family_id:
        raise RuntimeError(f"{repo_rel(workspace_path)} family_id drifted")
    if workspace.get("public_replay_command") != PUBLIC_COMMAND:
        raise RuntimeError(f"{repo_rel(workspace_path)} public_replay_command drifted")
    module = require_object(workspace.get("module"), f"{family_id}.workspace.module")
    source = normalize_path(str(module.get("source", "")))
    if source != normalize_path(expected_source):
        raise RuntimeError(f"{repo_rel(workspace_path)} module.source drifted from manifest")
    require_source_owned_path(source, f"{family_id}.workspace.module.source")
    package_edges = require_list(workspace.get("package_edges"), f"{family_id}.workspace.package_edges")
    if not package_edges:
        raise RuntimeError(f"{repo_rel(workspace_path)} must publish at least one package edge")
    for index, raw_edge in enumerate(package_edges):
        edge = require_object(raw_edge, f"{family_id}.workspace.package_edges[{index}]")
        require_nonempty_string(edge.get("from"), f"{family_id}.workspace.package_edges[{index}].from")
        require_nonempty_string(edge.get("to"), f"{family_id}.workspace.package_edges[{index}].to")
        require_nonempty_string(
            edge.get("relationship"),
            f"{family_id}.workspace.package_edges[{index}].relationship",
        )
    package_modules = workspace.get("package_modules", [])
    if package_modules:
        module_names: set[str] = set()
        for index, raw_module in enumerate(require_list(package_modules, f"{family_id}.workspace.package_modules")):
            package_module = require_object(raw_module, f"{family_id}.workspace.package_modules[{index}]")
            module_name = require_nonempty_string(
                package_module.get("module_name"),
                f"{family_id}.workspace.package_modules[{index}].module_name",
            )
            if module_name in module_names:
                raise RuntimeError(f"{repo_rel(workspace_path)} duplicate package module {module_name}")
            module_names.add(module_name)
            require_nonempty_string(
                package_module.get("package_id"),
                f"{family_id}.workspace.package_modules[{index}].package_id",
            )
            require_source_owned_path(
                str(package_module.get("source", "")),
                f"{family_id}.workspace.package_modules[{index}].source",
            )
    return workspace


def validate_native_meta(family_id: str, meta_path: Path, source_path: Path) -> dict[str, Any]:
    meta = load_json(meta_path)
    if meta.get("schema_version") != 1:
        raise RuntimeError(f"{repo_rel(meta_path)} schema_version drifted")
    if meta.get("fixture") != source_path.name:
        raise RuntimeError(f"{repo_rel(meta_path)} fixture drifted from source")
    if meta.get("issue") != 8200:
        raise RuntimeError(f"{repo_rel(meta_path)} issue drifted from #8200")
    if meta.get("owner_phase") != "e2e":
        raise RuntimeError(f"{repo_rel(meta_path)} owner_phase must be e2e")
    if meta.get("behavior_family") != "cross_lane_e2e":
        raise RuntimeError(f"{repo_rel(meta_path)} behavior_family drifted")
    if meta.get("public_replay_command") != PUBLIC_COMMAND:
        raise RuntimeError(f"{repo_rel(meta_path)} public_replay_command drifted")
    expected = require_object(meta.get("expected"), f"{family_id}.native_meta.expected")
    require_nonempty_string(expected.get("stage"), f"{family_id}.native_meta.expected.stage")
    require_list(expected.get("required_tokens"), f"{family_id}.native_meta.expected.required_tokens")
    return meta


def validate_section_status(section: dict[str, Any], field: str, expectation_state: str) -> None:
    status = require_nonempty_string(section.get("status"), f"{field}.status")
    if status not in ALLOWED_SECTION_STATES:
        raise RuntimeError(f"{field}.status is unknown: {status}")
    if status in BLOCKED_STATES:
        require_nonempty_string(section.get("blocker_id"), f"{field}.blocker_id")
        require_nonempty_string(section.get("responsible_domain"), f"{field}.responsible_domain")
        require_nonempty_string(section.get("reason"), f"{field}.reason")
    if expectation_state in BLOCKED_STATES and status == "expected-pass":
        require_nonempty_string(section.get("responsible_domain"), f"{field}.responsible_domain")
        require_nonempty_string(section.get("reason"), f"{field}.reason")


def validate_expectation(
    family: dict[str, Any],
    expectation_path: Path,
    source_path: Path,
    workspace_path: Path,
) -> dict[str, Any]:
    family_id = str(family["family_id"])
    expectation = load_json(expectation_path)
    if expectation.get("contract_id") != EXPECTATION_CONTRACT_ID:
        raise RuntimeError(f"{repo_rel(expectation_path)} contract_id drifted")
    if expectation.get("schema_version") != 1:
        raise RuntimeError(f"{repo_rel(expectation_path)} schema_version drifted")
    if expectation.get("issue") != 8200:
        raise RuntimeError(f"{repo_rel(expectation_path)} issue drifted from #8200")
    if expectation.get("family_id") != family_id:
        raise RuntimeError(f"{repo_rel(expectation_path)} family_id drifted from manifest")
    if normalize_path(str(expectation.get("source", ""))) != repo_rel(source_path):
        raise RuntimeError(f"{repo_rel(expectation_path)} source drifted from manifest")
    if normalize_path(str(expectation.get("workspace_manifest", ""))) != repo_rel(workspace_path):
        raise RuntimeError(f"{repo_rel(expectation_path)} workspace_manifest drifted from manifest")
    if expectation.get("public_replay_command") != PUBLIC_COMMAND:
        raise RuntimeError(f"{repo_rel(expectation_path)} public_replay_command drifted")

    expectation_state = require_nonempty_string(
        expectation.get("expectation_state"),
        f"{family_id}.expectation_state",
    )
    if expectation_state not in ALLOWED_EXPECTATION_STATES:
        raise RuntimeError(f"{family_id}.expectation_state is unknown: {expectation_state}")
    if expectation_state != family.get("expected_state"):
        raise RuntimeError(f"{family_id}.expected_state drifted between manifest and expectation")
    if expectation_state in BLOCKED_STATES:
        require_blocker_list(expectation.get("blockers"), f"{family_id}.expectation.blockers")

    responsible_domains = {
        str(domain)
        for domain in require_list(
            expectation.get("responsible_domains"),
            f"{family_id}.responsible_domains",
        )
        if isinstance(domain, str) and domain
    }
    if not responsible_domains:
        raise RuntimeError(f"{family_id}.responsible_domains must name at least one domain")

    for section_name in REQUIRED_EXPECTATION_SECTIONS:
        if section_name not in expectation:
            raise RuntimeError(f"{repo_rel(expectation_path)} missing {section_name}")

    diagnostics = require_object(expectation["diagnostics"], f"{family_id}.diagnostics")
    validate_section_status(diagnostics, f"{family_id}.diagnostics", expectation_state)
    for index, raw_case in enumerate(require_list(expectation["negative_cases"], f"{family_id}.negative_cases")):
        case = require_object(raw_case, f"{family_id}.negative_cases[{index}]")
        require_nonempty_string(case.get("case_id"), f"{family_id}.negative_cases[{index}].case_id")
        validate_section_status(case, f"{family_id}.negative_cases[{index}]", expectation_state)

    for section_name in (
        "runtime",
        "compile_manifest",
        "source_graph",
        "debug_source_map",
        "optimization_trace",
    ):
        section = require_object(expectation[section_name], f"{family_id}.{section_name}")
        validate_section_status(section, f"{family_id}.{section_name}", expectation_state)
        if section.get("source_truth") is True:
            raise RuntimeError(f"{family_id}.{section_name} must not mark generated artifacts as source truth")

    boundary = require_object(expectation.get("capability_boundary"), f"{family_id}.capability_boundary")
    if boundary.get("cannot_promote_reserved_rows") is not True:
        raise RuntimeError(f"{family_id}.capability_boundary must forbid reserved-row promotion")
    require_list(boundary.get("referenced_capability_rows"), f"{family_id}.capability_boundary.rows")
    return expectation


def validate_source_tokens(family_id: str, source_path: Path, tokens: list[Any]) -> None:
    source_text = source_path.read_text(encoding="utf-8")
    for token in tokens:
        token_text = require_nonempty_string(token, f"{family_id}.required_source_tokens[]")
        if token_text not in source_text:
            raise RuntimeError(f"{family_id} source is missing required token: {token_text}")


def validate_family(family: dict[str, Any]) -> dict[str, Any]:
    family_id = require_nonempty_string(family.get("family_id"), "family.family_id")
    require_nonempty_string(family.get("display_name"), f"{family_id}.display_name")
    if family.get("public_replay_command") != PUBLIC_COMMAND:
        raise RuntimeError(f"{family_id}.public_replay_command drifted")
    expected_state = require_nonempty_string(family.get("expected_state"), f"{family_id}.expected_state")
    if expected_state not in ALLOWED_EXPECTATION_STATES:
        raise RuntimeError(f"{family_id}.expected_state is unknown: {expected_state}")
    if expected_state in BLOCKED_STATES:
        require_blocker_list(family.get("blockers"), f"{family_id}.blockers")

    source_path = require_source_owned_path(str(family.get("source", "")), f"{family_id}.source")
    if source_path.suffix != ".objc3":
        raise RuntimeError(f"{family_id}.source must be an .objc3 fixture")
    meta_path = require_source_owned_path(str(family.get("native_meta", "")), f"{family_id}.native_meta")
    workspace_path = require_source_owned_path(
        str(family.get("workspace_manifest", "")),
        f"{family_id}.workspace_manifest",
    )
    expectation_path = require_source_owned_path(
        str(family.get("expectation", "")),
        f"{family_id}.expectation",
    )

    validate_source_tokens(
        family_id,
        source_path,
        require_list(family.get("required_source_tokens"), f"{family_id}.required_source_tokens"),
    )
    workspace = validate_workspace(family_id, workspace_path, str(family["source"]))
    meta = validate_native_meta(family_id, meta_path, source_path)
    expectation = validate_expectation(family, expectation_path, source_path, workspace_path)
    executable_proof: dict[str, Any] | None = None
    advanced_runtime_contract_proof: dict[str, Any] | None = None
    optimization_trace_proof: dict[str, Any] | None = None
    if require_object(expectation["runtime"], f"{family_id}.runtime").get("status") == "expected-pass":
        if family_id == ADVANCED_RUNTIME_FAMILY_ID:
            advanced_runtime_contract_proof = validate_advanced_runtime_contract_backed_proof(
                family_id,
                expectation,
                workspace,
            )
        else:
            executable_proof = validate_executable_runtime_proof(family_id, source_path, expectation, workspace)
    if require_object(expectation["optimization_trace"], f"{family_id}.optimization_trace").get("status") == "expected-pass":
        if executable_proof is None:
            raise RuntimeError(f"{family_id}.optimization proof diagnostic requires executable runtime proof")
        optimization_trace_proof = validate_optimization_trace_proof(family_id, expectation, executable_proof)

    capability_rows = require_list(family.get("capability_rows"), f"{family_id}.capability_rows")
    support_claims = require_list(family.get("support_claims"), f"{family_id}.support_claims")
    evidence_refs = require_list(family.get("evidence_map_refs"), f"{family_id}.evidence_map_refs")
    if not capability_rows or not support_claims or not evidence_refs:
        raise RuntimeError(f"{family_id} must reference capability rows, support claims, and evidence-map entries")

    for index, raw_ref in enumerate(evidence_refs):
        ref = require_object(raw_ref, f"{family_id}.evidence_map_refs[{index}]")
        require_nonempty_string(ref.get("capability_id"), f"{family_id}.evidence_map_refs[{index}].capability_id")
        require_nonempty_string(ref.get("support_claim"), f"{family_id}.evidence_map_refs[{index}].support_claim")
        status = require_nonempty_string(ref.get("status"), f"{family_id}.evidence_map_refs[{index}].status")
        if status not in ("implemented", "blocked", "reserved"):
            raise RuntimeError(f"{family_id}.evidence_map_refs[{index}].status is unknown: {status}")
        require_nonempty_string(
            ref.get("claim_boundary"),
            f"{family_id}.evidence_map_refs[{index}].claim_boundary",
        )

    return {
        "family_id": family_id,
        "display_name": family["display_name"],
        "expected_state": expected_state,
        "source": repo_rel(source_path),
        "native_meta": repo_rel(meta_path),
        "workspace_manifest": repo_rel(workspace_path),
        "expectation": repo_rel(expectation_path),
        "package_edge_count": len(workspace.get("package_edges", [])),
        "required_token_count": len(family.get("required_source_tokens", [])),
        "capability_row_count": len(capability_rows),
        "support_claim_count": len(support_claims),
        "evidence_map_ref_count": len(evidence_refs),
        "negative_case_count": len(expectation.get("negative_cases", [])),
        "meta_fixture_kind": meta.get("fixture_kind"),
        "executable_proof": executable_proof,
        "advanced_runtime_contract_proof": advanced_runtime_contract_proof,
        "optimization_trace_proof": optimization_trace_proof,
    }


def validate_workflow_action_glue() -> dict[str, Any]:
    workflow_text = WORKFLOW_PATH.read_text(encoding="utf-8")
    required_tokens = (
        "tests/native/e2e/cross_lane/**",
        "tests/tooling/fixtures/cross_lane_e2e/**",
        "scripts/check_objc3c_cross_lane_e2e.py",
        "npm run objc3c -- validate-cross-lane-e2e",
    )
    missing = [token for token in required_tokens if token not in workflow_text]
    if missing:
        raise RuntimeError("workflow action glue missing cross-lane entries: " + ", ".join(missing))
    return {
        "workflow": repo_rel(WORKFLOW_PATH),
        "public_command": PUBLIC_COMMAND,
        "status": "wired",
    }


def validate_manifest() -> dict[str, Any]:
    manifest = load_json(MANIFEST_PATH)
    if manifest.get("contract_id") != MANIFEST_CONTRACT_ID:
        raise RuntimeError("cross-lane E2E manifest contract_id drifted")
    if manifest.get("schema_version") != 1:
        raise RuntimeError("cross-lane E2E manifest schema_version drifted")
    if manifest.get("issue") != 8200:
        raise RuntimeError("cross-lane E2E manifest issue drifted from #8200")
    if manifest.get("public_action") != PUBLIC_ACTION:
        raise RuntimeError("cross-lane E2E manifest public_action drifted")
    if manifest.get("public_replay_command") != PUBLIC_COMMAND:
        raise RuntimeError("cross-lane E2E manifest public_replay_command drifted")

    policy = require_object(manifest.get("claim_policy"), "claim_policy")
    if policy.get("integrated_success_promotes_reserved_rows") is not False:
        raise RuntimeError("claim_policy must not promote reserved rows")
    if policy.get("tmp_source_truth_allowed") is not False:
        raise RuntimeError("claim_policy must reject tmp source truth")

    families = require_list(manifest.get("families"), "families")
    family_ids = [require_nonempty_string(require_object(row, "family").get("family_id"), "family.family_id") for row in families]
    if tuple(family_ids) != REQUIRED_FAMILY_IDS:
        raise RuntimeError(
            "cross-lane E2E families must stay in required #8200 order: "
            + ", ".join(REQUIRED_FAMILY_IDS)
        )

    family_summaries = [validate_family(require_object(row, "family")) for row in families]
    workflow_action_glue = validate_workflow_action_glue()
    blocked_count = sum(1 for row in family_summaries if row["expected_state"] in BLOCKED_STATES)
    if blocked_count == 0:
        raise RuntimeError("cross-lane E2E scaffold must keep unsupported families blocker-explicit")

    return {
        "manifest_path": repo_rel(MANIFEST_PATH),
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_manifest_contract": MANIFEST_CONTRACT_ID,
        "issue": 8200,
        "public_action": PUBLIC_ACTION,
        "public_replay_command": PUBLIC_COMMAND,
        "workflow_action_glue": workflow_action_glue,
        "family_count": len(family_summaries),
        "blocked_or_reserved_family_count": blocked_count,
        "family_summaries": family_summaries,
    }


def main() -> int:
    try:
        payload = validate_manifest()
    except Exception as exc:
        return fail(str(exc))

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-cross-lane-e2e: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
