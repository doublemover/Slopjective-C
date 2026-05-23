#!/usr/bin/env python3
"""Validate the #8200 cross-lane end-to-end scaffold manifest."""

from __future__ import annotations

import sys
import os
import shutil
import subprocess
import hashlib
from copy import deepcopy
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from check_objc3c_advanced_runtime_closure import validate_advanced_runtime_closure
from objc3c_package_manager.install_distribution import collect_install_distribution_failures
from objc3c_package_manager.model import PACKAGE_MANAGER_TAMPER_CODE
from objc3c_editor_tooling.input_loading import load_editor_tooling_inputs, run_frontend_compile
from objc3c_editor_tooling.model import build_editor_tooling_model
from objc3c_editor_tooling.paths import paths_for_source, resolve_source
from objc3c_editor_tooling.publication import publish_editor_tooling_surface
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel
from check_objc3c_public_runtime_reflection_api import validate_public_runtime_reflection_api
from objc3c_object_model_debugger_proof import (
    DEFAULT_CONTRACT_PATH as OBJECT_MODEL_DEBUGGER_CONTRACT_PATH,
    validate_contract_path as validate_object_model_debugger_contract,
)


ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "cross_lane_e2e" / "manifest.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "conformance" / "cross-lane-e2e-summary.json"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "cross-lane-e2e"
RELEASE_OPERATIONS_PREFLIGHT_ARTIFACT_DIR = ARTIFACT_ROOT / "release-operations-preflight"
WORKFLOW_PATH = ROOT / ".github" / "workflows" / "conformance-minima.yml"
OPTIMIZATION_BEFORE_IR_PATH = ROOT / "tests" / "native" / "ir" / "optimization" / "semantic_pipeline_direct_dispatch.before.ll"
OPTIMIZATION_AFTER_IR_PATH = ROOT / "tests" / "native" / "ir" / "optimization" / "semantic_pipeline_direct_dispatch.after.ll"
OPTIMIZATION_METHOD_INLINING_BEFORE_IR_PATH = (
    ROOT / "tests" / "native" / "ir" / "optimization" / "semantic_pipeline_method_inlining.before.ll"
)
OPTIMIZATION_METHOD_INLINING_AFTER_IR_PATH = (
    ROOT / "tests" / "native" / "ir" / "optimization" / "semantic_pipeline_method_inlining.after.ll"
)
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
PACKAGE_INSTALL_SUMMARY_PATH = (
    ROOT / "tmp" / "reports" / "package-ecosystem" / "install-distribution-credibility-summary.json"
)
PACKAGE_INSTALL_VERIFICATION_PATH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "package-ecosystem"
    / "install-validation"
    / "objc3c-install-distribution-verification.json"
)
PACKAGE_INSTALL_PROOF_PATH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "package-ecosystem"
    / "install-validation"
    / "objc3c-install-proof-manifest.json"
)
PACKAGE_LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
PACKAGE_UPDATE_RECEIPT_PATH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "package-ecosystem"
    / "install-validation"
    / "clean-root"
    / "objc3c"
    / "receipts"
    / "objc3c-update-plan-receipt.json"
)
PACKAGE_UNINSTALL_RECEIPT_PATH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "package-ecosystem"
    / "install-validation"
    / "clean-root"
    / "objc3c"
    / "receipts"
    / "objc3c-uninstall-plan-receipt.json"
)
PACKAGE_INSTALL_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "install_distribution_credibility_contract.json"
)
PACKAGE_INSTALL_MIRROR_PATH = (
    ROOT / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "offline-mirror-index.json"
)
PACKAGE_INSTALL_REGISTRY_PATH = (
    ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "local-package-index.json"
)
PACKAGE_INSTALL_PUBLICATION_PATH = (
    ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "publication-metadata.json"
)
PACKAGE_INSTALL_RESTORE_RECEIPT_PATH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "package-ecosystem"
    / "offline-install"
    / "objc3c-offline-mirror-restore-receipt.json"
)
RELEASE_OPERATIONS_SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "end-to-end-summary.json"
RELEASE_OPERATIONS_REPORT_ROOT = ROOT / "tmp" / "reports" / "release-operations"
RELEASE_OPERATIONS_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "release-operations"
PACKAGE_CHANNELS_REPORT_ROOT = ROOT / "tmp" / "reports" / "package-channels"
PACKAGE_CHANNELS_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "package-channels"
RELEASE_FOUNDATION_REPORT_ROOT = ROOT / "tmp" / "reports" / "release-foundation"
RELEASE_FOUNDATION_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "release-foundation"
RELEASE_EVIDENCE_REPORT_ROOT = ROOT / "tmp" / "reports" / "release_evidence"
PLATFORM_HARDENING_REPORT_ROOT = ROOT / "tmp" / "reports" / "platform-hardening"
PLATFORM_HARDENING_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "platform-hardening"
RELEASE_UPDATE_MANIFEST_PATH = (
    ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
)
RELEASE_CHANNEL_MANIFEST_PATH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "release-operations"
    / "channel-manifest"
    / "objc3c-release-channel-manifest.json"
)
RELEASE_CHANNEL_OPERATIONS_MODEL_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "channel_operations_model.json"
)

MANIFEST_CONTRACT_ID = "objc3c.cross_lane_e2e.manifest.v1"
EXPECTATION_CONTRACT_ID = "objc3c.cross_lane_e2e.family_expectation.v1"
WORKSPACE_CONTRACT_ID = "objc3c.cross_lane_e2e.workspace.v1"
SUMMARY_CONTRACT_ID = "objc3c.cross_lane_e2e.summary.v1"
PUBLIC_ACTION = "validate-cross-lane-e2e"
PUBLIC_COMMAND = "npm run objc3c -- validate-cross-lane-e2e"
PUBLIC_CONFORMANCE_SUITE_COMMAND = "npm run objc3c -- validate-public-conformance-suite"
TEST_FULL_COMMAND = "npm run objc3c -- test-full"
REQUIRED_PUBLIC_COMMANDS = (
    PUBLIC_COMMAND,
    PUBLIC_CONFORMANCE_SUITE_COMMAND,
    TEST_FULL_COMMAND,
)
PUBLIC_CONFORMANCE_SUITE_MANIFEST_PATH = ROOT / "tests" / "conformance" / "public_suite_manifest.json"
OBJECT_REFLECTION_DEBUGGER_PROOF_CONTRACT_ID = (
    "objc3c.cross_lane_e2e.object_reflection_debugger_proof.v1"
)
TEXT_COLLECTIONS_PACKAGE_PROOF_CONTRACT_ID = (
    "objc3c.cross_lane_e2e.text_collections_package_proof.v1"
)
CANONICAL_MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
SUPPORT_CLAIM_EVIDENCE_CATALOG_PATH = (
    ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
)
DIRECT_IMPORT_CAPABILITY_ID = "modules.direct-import-syntax"
DIRECT_IMPORT_SUPPORT_CLAIM = "objc3c.behavior.modules.direct-import-syntax"
DIRECT_IMPORT_OWNER_PHASE = "parser"
DIRECT_IMPORT_PUBLIC_COMMAND = "npm run objc3c -- validate-direct-import-module-syntax"
TEXT_PACKAGE_RUNTIME_FFI_BLOCKER_ID = "cross-lane-text-package-runtime-import-ffi-preservation-reserved"
TEXT_PACKAGE_RUNTIME_FFI_BLOCKER_TEXT = (
    "cross-module runtime link-plan Part 11 ffi preservation surface incomplete "
    "for CrossLaneFixtureProvider"
)
NATIVE_OBJECT_EMISSION_MISSING_LLC_STATUS = "native_object_emission_missing_llc"
NATIVE_OBJECT_EMISSION_MISSING_LLC_DIAGNOSTIC = (
    f"{NATIVE_OBJECT_EMISSION_MISSING_LLC_STATUS}: native object emission "
    "fail-closed; llc executable not found; set "
    "OBJC3C_NATIVE_EXECUTION_LLC_PATH or install LLVM with llc on PATH; no "
    "native object, package, or execution success claim is published"
)

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
ISSUE_FAILURE_DOMAINS = (
    "parser/sema diagnostic",
    "lowering/IR diagnostic",
    "runtime status",
    "package trust diagnostic",
    "source-map/debug diagnostic",
    "optimization proof diagnostic",
    "capability/evidence mismatch",
)


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


def require_failure_domain(value: Any, field: str) -> str:
    domain = require_nonempty_string(value, field)
    if domain not in ISSUE_FAILURE_DOMAINS:
        raise RuntimeError(
            f"{field} must use one of the #8200 failure domains: "
            + ", ".join(ISSUE_FAILURE_DOMAINS)
        )
    return domain


def require_string_list(value: Any, field: str) -> list[str]:
    return [
        require_nonempty_string(item, f"{field}[{index}]")
        for index, item in enumerate(require_list(value, field))
    ]


def manifest_family(row: Any, field: str) -> dict[str, Any]:
    row_object = require_object(row, field)
    if "family" in row_object:
        return require_object(row_object.get("family"), f"{field}.family")
    return row_object


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
    llvm_root = os.environ.get("LLVM_ROOT")
    if llvm_root:
        candidate = Path(llvm_root) / "bin" / "llc.exe"
        if candidate.is_file():
            return str(candidate)
    program_files_candidate = Path("C:/Program Files/LLVM/bin/llc.exe")
    if program_files_candidate.is_file():
        return str(program_files_candidate)
    resolved = shutil.which("llc")
    if resolved:
        return resolved
    raise RuntimeError(NATIVE_OBJECT_EMISSION_MISSING_LLC_DIAGNOSTIC)


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
        print(f"[fail] {domain} command failed; captured log follows: {repo_rel(log_path)}", file=sys.stderr)
        print(log_path.read_text(encoding="utf-8"), file=sys.stderr)
        raise RuntimeError(
            f"{domain} proof command failed with exit {result.returncode}: {repo_rel(log_path)}"
        )
    return result


def require_artifact(path: Path, label: str) -> None:
    if not path.is_file():
        raise RuntimeError(f"runtime status proof missing {label}: {repo_rel(path)}")


def sha256_file(path: Path) -> str:
    require_artifact(path, "digest input")
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


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
    require_artifact(
        OPTIMIZATION_METHOD_INLINING_BEFORE_IR_PATH,
        "semantic optimization method-inlining before IR reference",
    )
    require_artifact(
        OPTIMIZATION_METHOD_INLINING_AFTER_IR_PATH,
        "semantic optimization method-inlining after IR reference",
    )

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

    method_before_text = OPTIMIZATION_METHOD_INLINING_BEFORE_IR_PATH.read_text(encoding="utf-8")
    method_after_text = OPTIMIZATION_METHOD_INLINING_AFTER_IR_PATH.read_text(encoding="utf-8")
    required_method_reference_tokens = {
        "method_before_call": "call i32 @objc3_inlineable_Math_addOne",
        "method_before_body_identity": "callee body identity: body:Math.addOne:v1",
        "method_after_inline_frame": "source-map inline frame preserved",
        "method_after_invalidation": "semantic-optimization.invalidate-global-proof-state",
    }
    missing_method_reference = [
        label
        for label, token in required_method_reference_tokens.items()
        if token not in (method_before_text if label.startswith("method_before") else method_after_text)
    ]
    if missing_method_reference:
        raise RuntimeError(
            f"{family_id}.method inlining reference proof missing tokens: "
            + ", ".join(missing_method_reference)
        )

    negative_cases = require_list(expectation.get("negative_cases"), f"{family_id}.negative_cases")
    method_fail_closed_case = None
    for raw_case in negative_cases:
        case = require_object(raw_case, f"{family_id}.negative_cases[]")
        if case.get("case_id") == "optimization-runtime-method-inlining-fail-closed":
            method_fail_closed_case = case
            break
    if method_fail_closed_case is None:
        raise RuntimeError(f"{family_id}.negative_cases missing method-inlining fail-closed case")
    if method_fail_closed_case.get("status") != "expected-pass":
        raise RuntimeError(f"{family_id}.method inlining fail-closed case must be expected-pass")

    required_generated_ir_tokens = [
        require_nonempty_string(token, f"{family_id}.method_inlining.required_generated_ir_tokens[]")
        for token in require_list(
            method_fail_closed_case.get("required_generated_ir_tokens"),
            f"{family_id}.method_inlining.required_generated_ir_tokens",
        )
    ]
    forbidden_generated_ir_tokens = [
        require_nonempty_string(token, f"{family_id}.method_inlining.forbidden_generated_ir_tokens[]")
        for token in require_list(
            method_fail_closed_case.get("forbidden_generated_ir_tokens"),
            f"{family_id}.method_inlining.forbidden_generated_ir_tokens",
        )
    ]
    missing_required_inline_tokens = [
        token for token in required_generated_ir_tokens if token not in ll_text
    ]
    if missing_required_inline_tokens:
        raise RuntimeError(
            f"{family_id}.method inlining fail-closed proof missing generated IR tokens: "
            + ", ".join(missing_required_inline_tokens)
        )
    promoted_inline_tokens = [
        token for token in forbidden_generated_ir_tokens if token in ll_text
    ]
    if promoted_inline_tokens:
        raise RuntimeError(
            f"{family_id}.method inlining fail-closed proof saw promoted IR tokens: "
            + ", ".join(promoted_inline_tokens)
        )

    expected_references = {
        repo_rel(OPTIMIZATION_METHOD_INLINING_BEFORE_IR_PATH),
        repo_rel(OPTIMIZATION_METHOD_INLINING_AFTER_IR_PATH),
    }
    actual_references = {
        normalize_path(str(path))
        for path in require_list(
            method_fail_closed_case.get("reference_artifacts"),
            f"{family_id}.method_inlining.reference_artifacts",
        )
    }
    if expected_references.difference(actual_references):
        raise RuntimeError(f"{family_id}.method inlining fail-closed references drifted")

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
        "method_inlining_fail_closed_evidence": {
            "status": "REJECTED_FAIL_CLOSED",
            "case_id": method_fail_closed_case["case_id"],
            "candidate_site": required_ir_tokens["direct_candidate_call"],
            "generated_ir_retains_candidate_call": True,
            "generated_ir_contains_inline_frame": False,
            "generated_ir_contains_global_invalidation": False,
            "before_ir_reference": repo_rel(OPTIMIZATION_METHOD_INLINING_BEFORE_IR_PATH),
            "after_ir_reference": repo_rel(OPTIMIZATION_METHOD_INLINING_AFTER_IR_PATH),
            "reason": (
                "cross-lane production IR has a method-inlining candidate call, "
                "but does not carry inlined-body, inline-frame, or invalidation proof"
            ),
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
        "method_inlining_fail_closed_evidence": payload["method_inlining_fail_closed_evidence"],
        "reserved_rows_not_promoted": payload["reserved_rows_not_promoted"],
    }


def validate_minimum_count(
    *,
    family_id: str,
    actual: Any,
    minimums: dict[str, Any],
    minimum_key: str,
    domain: str,
) -> int:
    expected = minimums.get(minimum_key)
    if not isinstance(expected, int) or isinstance(expected, bool):
        raise RuntimeError(f"{family_id}.{domain} minimum must be an integer: {minimum_key}")
    if not isinstance(actual, int) or isinstance(actual, bool):
        actual = 0
    if actual < expected:
        raise RuntimeError(
            f"{family_id}.{domain} expected {minimum_key} >= {expected} but got {actual}"
        )
    return actual


def validate_object_typed_keypath_reflection_proof(
    *,
    family_id: str,
    source_text: str,
    proof: dict[str, Any],
    emitted_artifacts: dict[str, str],
) -> dict[str, Any]:
    typed_keypath = require_object(
        proof.get("typed_keypath_object_reflection"),
        f"{family_id}.object_reflection_debugger_proof.typed_keypath_object_reflection",
    )
    if typed_keypath.get("status") != "expected-pass":
        raise RuntimeError(f"{family_id}.typed keypath object reflection proof must be expected-pass")
    source_literal = require_nonempty_string(
        typed_keypath.get("source_literal"),
        f"{family_id}.typed_keypath_object_reflection.source_literal",
    )
    if source_literal not in source_text:
        raise RuntimeError(f"{family_id}.typed keypath source literal is missing: {source_literal}")
    expected_component_count = typed_keypath.get("expected_component_count")
    if expected_component_count != 1:
        raise RuntimeError(f"{family_id}.typed keypath proof must stay single-component")
    if typed_keypath.get("expected_root_is_self") is not False:
        raise RuntimeError(f"{family_id}.typed keypath proof must stay class-root scoped")
    for raw_symbol in require_list(
        typed_keypath.get("required_runtime_helper_symbols"),
        f"{family_id}.typed_keypath_object_reflection.required_runtime_helper_symbols",
    ):
        symbol = require_nonempty_string(
            raw_symbol,
            f"{family_id}.typed_keypath_object_reflection.required_runtime_helper_symbols[]",
        )
        if symbol not in source_text:
            raise RuntimeError(f"{family_id}.typed keypath runtime helper missing from source: {symbol}")

    manifest_path = ROOT / normalize_path(
        require_nonempty_string(emitted_artifacts.get("manifest"), f"{family_id}.emitted_artifacts.manifest")
    )
    manifest = _require_json_artifact(manifest_path, "object typed keypath manifest")
    frontend = require_object(manifest.get("frontend"), f"{family_id}.manifest.frontend")
    pipeline = require_object(frontend.get("pipeline"), f"{family_id}.manifest.frontend.pipeline")
    semantic_surface = require_object(
        pipeline.get("semantic_surface"),
        f"{family_id}.manifest.frontend.pipeline.semantic_surface",
    )
    semantic_model = require_object(
        semantic_surface.get("objc_type_system_type_semantic_model"),
        f"{family_id}.manifest.semantic_surface.objc_type_system_type_semantic_model",
    )
    lowering_contract = require_object(
        semantic_surface.get("objc_type_system_optional_keypath_lowering_contract"),
        f"{family_id}.manifest.semantic_surface.objc_type_system_optional_keypath_lowering_contract",
    )
    runtime_helper_contract = require_object(
        semantic_surface.get("objc_type_system_optional_keypath_runtime_helper_contract"),
        f"{family_id}.manifest.semantic_surface.objc_type_system_optional_keypath_runtime_helper_contract",
    )
    minimums = require_object(
        typed_keypath.get("minimums"),
        f"{family_id}.typed_keypath_object_reflection.minimums",
    )
    semantic_typed_keypath_literal_sites = validate_minimum_count(
        family_id=family_id,
        actual=semantic_model.get("typed_keypath_literal_sites"),
        minimums=minimums,
        minimum_key="semantic_typed_keypath_literal_sites",
        domain="typed keypath semantic model",
    )
    lowering_typed_keypath_literal_sites = validate_minimum_count(
        family_id=family_id,
        actual=lowering_contract.get("typed_keypath_literal_sites"),
        minimums=minimums,
        minimum_key="lowering_typed_keypath_literal_sites",
        domain="typed keypath lowering contract",
    )
    live_typed_keypath_artifact_sites = validate_minimum_count(
        family_id=family_id,
        actual=lowering_contract.get("live_typed_keypath_artifact_sites"),
        minimums=minimums,
        minimum_key="live_typed_keypath_artifact_sites",
        domain="typed keypath lowering contract",
    )
    typed_keypath_descriptor_publication_sites = validate_minimum_count(
        family_id=family_id,
        actual=lowering_contract.get("typed_keypath_descriptor_publication_sites"),
        minimums=minimums,
        minimum_key="typed_keypath_descriptor_publication_sites",
        domain="typed keypath lowering contract",
    )
    typed_keypath_source_map_evidence_sites = validate_minimum_count(
        family_id=family_id,
        actual=lowering_contract.get("typed_keypath_source_map_evidence_sites"),
        minimums=minimums,
        minimum_key="typed_keypath_source_map_evidence_sites",
        domain="typed keypath lowering contract",
    )
    typed_keypath_runtime_handle_evidence_sites = validate_minimum_count(
        family_id=family_id,
        actual=lowering_contract.get("typed_keypath_runtime_handle_evidence_sites"),
        minimums=minimums,
        minimum_key="typed_keypath_runtime_handle_evidence_sites",
        domain="typed keypath lowering contract",
    )
    if (
        typed_keypath_descriptor_publication_sites != live_typed_keypath_artifact_sites
        or typed_keypath_source_map_evidence_sites != live_typed_keypath_artifact_sites
        or typed_keypath_runtime_handle_evidence_sites != live_typed_keypath_artifact_sites
    ):
        raise RuntimeError(f"{family_id}.typed keypath evidence counters drifted from live artifacts")
    if runtime_helper_contract.get("typed_keypath_descriptor_handles_ready") is not True:
        raise RuntimeError(f"{family_id}.typed keypath descriptor handles must be runtime-ready")
    if runtime_helper_contract.get("typed_keypath_source_map_evidence_ready") is not True:
        raise RuntimeError(f"{family_id}.typed keypath source-map evidence must be runtime-ready")
    if runtime_helper_contract.get("typed_keypath_runtime_handle_evidence_ready") is not True:
        raise RuntimeError(f"{family_id}.typed keypath runtime-handle evidence must be runtime-ready")
    if runtime_helper_contract.get("typed_keypath_runtime_execution_helper_landed") is not True:
        raise RuntimeError(f"{family_id}.typed keypath runtime helper must be landed")

    ir_path = ROOT / normalize_path(
        require_nonempty_string(emitted_artifacts.get("ir"), f"{family_id}.emitted_artifacts.ir")
    )
    require_artifact(ir_path, "object typed keypath IR")
    ir_text = ir_path.read_text(encoding="utf-8")
    required_ir_tokens = [
        require_nonempty_string(
            raw_token,
            f"{family_id}.typed_keypath_object_reflection.required_ir_tokens[]",
        )
        for raw_token in require_list(
            typed_keypath.get("required_ir_tokens"),
            f"{family_id}.typed_keypath_object_reflection.required_ir_tokens",
        )
    ]
    for token in required_ir_tokens:
        if token not in ir_text:
            raise RuntimeError(f"{family_id}.typed keypath IR missing required token: {token}")

    return {
        "status": "PASS",
        "source_literal": source_literal,
        "expected_component_count": expected_component_count,
        "expected_root_is_self": False,
        "semantic_typed_keypath_literal_sites": semantic_typed_keypath_literal_sites,
        "lowering_typed_keypath_literal_sites": lowering_typed_keypath_literal_sites,
        "live_typed_keypath_artifact_sites": live_typed_keypath_artifact_sites,
        "typed_keypath_descriptor_publication_sites": typed_keypath_descriptor_publication_sites,
        "typed_keypath_source_map_evidence_sites": typed_keypath_source_map_evidence_sites,
        "typed_keypath_runtime_handle_evidence_sites": typed_keypath_runtime_handle_evidence_sites,
        "descriptor_handles_ready": True,
        "source_map_evidence_ready": True,
        "runtime_handle_evidence_ready": True,
        "runtime_execution_helper_landed": True,
        "required_ir_tokens": required_ir_tokens,
    }


def validate_object_reflection_debugger_proof(
    family_id: str,
    source_path: Path,
    expectation: dict[str, Any],
    executable_proof: dict[str, Any] | None,
) -> dict[str, Any] | None:
    if family_id != "object_reflection_debugger":
        return None

    proof = require_object(
        expectation.get("object_reflection_debugger_proof"),
        f"{family_id}.object_reflection_debugger_proof",
    )
    if proof.get("contract_id") != OBJECT_REFLECTION_DEBUGGER_PROOF_CONTRACT_ID:
        raise RuntimeError(f"{family_id}.object_reflection_debugger_proof contract_id drifted")
    if executable_proof is None:
        raise RuntimeError(f"{family_id}.object reflection debugger proof requires executable runtime proof")
    if proof.get("runs_canonical_frontend") is not True:
        raise RuntimeError(f"{family_id}.object reflection debugger proof must run the canonical frontend")
    if proof.get("requires_public_runtime_reflection_api") is not True:
        raise RuntimeError(f"{family_id}.object reflection debugger proof must require public runtime reflection")
    if proof.get("requires_object_model_debugger_proof") is not True:
        raise RuntimeError(f"{family_id}.object reflection debugger proof must require object-model debugger proof")
    source_text = source_path.read_text(encoding="utf-8")

    paths = paths_for_source(resolve_source(repo_rel(source_path)))
    compile_result = run_frontend_compile(paths)
    if compile_result.returncode != 0 or not compile_result.summary_available:
        raise RuntimeError(
            f"{family_id}.source-map/debug diagnostic production frontend probe failed "
            f"with exit {compile_result.returncode}"
        )
    inputs = load_editor_tooling_inputs(paths)
    model = build_editor_tooling_model(paths, inputs)
    published = publish_editor_tooling_surface(paths=paths, inputs=inputs, model=model)

    if inputs.summary.get("success") is not True or inputs.summary.get("status") != 0:
        raise RuntimeError(f"{family_id}.production frontend summary must report success")
    if normalize_path(str(inputs.summary.get("input_path", ""))) != repo_rel(source_path):
        raise RuntimeError(f"{family_id}.production frontend summary input path drifted")

    summary_paths = require_object(inputs.summary.get("paths"), f"{family_id}.compile_summary.paths")
    required_artifacts = require_list(
        proof.get("required_artifact_kinds"),
        f"{family_id}.object_reflection_debugger_proof.required_artifact_kinds",
    )
    artifact_path_keys = {
        "manifest": "manifest",
        "ir": "ir",
        "object": "object",
        "runtime-metadata-binary": "runtime_metadata_binary",
    }
    emitted_artifacts: dict[str, str] = {}
    for raw_kind in required_artifacts:
        kind = require_nonempty_string(
            raw_kind,
            f"{family_id}.object_reflection_debugger_proof.required_artifact_kinds[]",
        )
        if kind == "summary":
            candidate = paths.compile_summary
        elif kind == "source-graph":
            candidate = paths.source_graph
        elif kind == "artifact-inspector":
            candidate = paths.artifact_inspector
        elif kind == "debug-map":
            candidate = paths.debug_map
        else:
            path_key = artifact_path_keys.get(kind)
            if path_key is None:
                raise RuntimeError(f"{family_id}.object reflection proof unknown artifact kind: {kind}")
            candidate = ROOT / normalize_path(
                require_nonempty_string(summary_paths.get(path_key), f"{family_id}.summary.paths.{path_key}")
            )
        require_artifact(candidate, f"object reflection debugger {kind}")
        emitted_artifacts[kind] = repo_rel(candidate)

    minimums = require_object(
        proof.get("runtime_inventory_minimums"),
        f"{family_id}.object_reflection_debugger_proof.runtime_inventory_minimums",
    )
    source_graph = require_object(model.source_graph, f"{family_id}.source_graph")
    if source_graph.get("available") is not True:
        raise RuntimeError(f"{family_id}.source graph must be available on the production artifact path")
    if source_graph.get("fail_closed") is not True:
        raise RuntimeError(f"{family_id}.source graph must keep unsupported reference consumers fail-closed")
    source_graph_nodes = validate_minimum_count(
        family_id=family_id,
        actual=source_graph.get("node_count"),
        minimums=minimums,
        minimum_key="source_graph_nodes",
        domain="source graph",
    )

    artifact_inspector = require_object(model.artifact_inspector, f"{family_id}.artifact_inspector")
    if artifact_inspector.get("supported") is not True:
        raise RuntimeError(f"{family_id}.artifact inspector must be supported")
    if artifact_inspector.get("support_class") != "compile-artifact-inspector":
        raise RuntimeError(f"{family_id}.artifact inspector inventory must be ready")
    inventory_validation = require_object(
        artifact_inspector.get("inventory_validation"),
        f"{family_id}.artifact_inspector.inventory_validation",
    )
    if inventory_validation.get("inventory_ready") is not True or inventory_validation.get("fail_closed") is True:
        raise RuntimeError(f"{family_id}.artifact inspector inventory is not production-ready")
    runtime_inventory = require_object(
        artifact_inspector.get("runtime_inventory"),
        f"{family_id}.artifact_inspector.runtime_inventory",
    )
    if runtime_inventory.get("available") is not True:
        raise RuntimeError(f"{family_id}.runtime inventory must be available")
    require_nonempty_string(
        runtime_inventory.get("reflection_abi_version"),
        f"{family_id}.runtime_inventory.reflection_abi_version",
    )
    runtime_counts = {
        "class_records": validate_minimum_count(
            family_id=family_id,
            actual=runtime_inventory.get("class_record_count"),
            minimums=minimums,
            minimum_key="class_records",
            domain="runtime inventory",
        ),
        "protocol_records": validate_minimum_count(
            family_id=family_id,
            actual=runtime_inventory.get("protocol_record_count"),
            minimums=minimums,
            minimum_key="protocol_records",
            domain="runtime inventory",
        ),
        "category_records": validate_minimum_count(
            family_id=family_id,
            actual=runtime_inventory.get("category_record_count"),
            minimums=minimums,
            minimum_key="category_records",
            domain="runtime inventory",
        ),
        "property_records": validate_minimum_count(
            family_id=family_id,
            actual=runtime_inventory.get("property_record_count"),
            minimums=minimums,
            minimum_key="property_records",
            domain="runtime inventory",
        ),
        "method_records": validate_minimum_count(
            family_id=family_id,
            actual=runtime_inventory.get("method_record_count"),
            minimums=minimums,
            minimum_key="method_records",
            domain="runtime inventory",
        ),
    }

    debug_map = require_object(model.debug, f"{family_id}.debug_source_map")
    if debug_map.get("supported") is not True or debug_map.get("object_artifact_present") is not True:
        raise RuntimeError(f"{family_id}.debug map must be tied to the emitted object artifact")
    if debug_map.get("source_map_supported") is not True:
        raise RuntimeError(f"{family_id}.debug map must publish object-model source maps")
    if debug_map.get("statement_level_stepping") is not True:
        raise RuntimeError(f"{family_id}.debug map must publish object-model statement stepping")
    declaration_breakpoint_anchors = validate_minimum_count(
        family_id=family_id,
        actual=debug_map.get("declaration_breakpoint_anchor_count"),
        minimums=minimums,
        minimum_key="declaration_breakpoint_anchors",
        domain="debug map",
    )
    typed_keypath_object_reflection = validate_object_typed_keypath_reflection_proof(
        family_id=family_id,
        source_text=source_text,
        proof=proof,
        emitted_artifacts=emitted_artifacts,
    )

    debugger_contract = ROOT / normalize_path(
        str(proof.get("object_model_debugger_contract") or repo_rel(OBJECT_MODEL_DEBUGGER_CONTRACT_PATH))
    )
    debugger_result = validate_object_model_debugger_contract(
        debugger_contract,
        run_production_probe=True,
    )
    if debugger_result.ok is not True:
        diagnostics = ", ".join(diagnostic.code for diagnostic in debugger_result.diagnostics)
        raise RuntimeError(f"{family_id}.object-model debugger proof failed: {diagnostics}")

    public_reflection = validate_public_runtime_reflection_api()
    if public_reflection.get("status") != "PASS":
        failures = ", ".join(str(failure) for failure in public_reflection.get("failures", []))
        raise RuntimeError(f"{family_id}.public runtime reflection proof failed: {failures}")
    public_reflection_report_path = (
        ROOT / "tmp" / "reports" / "runtime" / "public-runtime-reflection-api.json"
    )
    write_json_file(public_reflection_report_path, public_reflection)

    reserved_rows = require_list(
        proof.get("reserved_rows_not_promoted"),
        f"{family_id}.object_reflection_debugger_proof.reserved_rows_not_promoted",
    )
    for required_row in ("runtime.debug-trace.full-source-map-publication",):
        if required_row not in reserved_rows:
            raise RuntimeError(f"{family_id}.object reflection proof must keep {required_row} reserved")
    for promoted_row in (
        "runtime.object-model.full-realization",
        "runtime.debug-trace.statement-stepping",
    ):
        if promoted_row in reserved_rows:
            raise RuntimeError(f"{family_id}.object reflection proof must not reserve promoted row {promoted_row}")

    return {
        "status": "PASS",
        "compile_summary": repo_rel(paths.compile_summary),
        "source_graph": published.source_graph_path,
        "artifact_inspector": published.artifact_inspector_path,
        "debug_map": published.debug_path,
        "emitted_artifacts": emitted_artifacts,
        "runtime_inventory_counts": runtime_counts,
        "source_graph_nodes": source_graph_nodes,
        "declaration_breakpoint_anchors": declaration_breakpoint_anchors,
        "typed_keypath_object_reflection": typed_keypath_object_reflection,
        "public_runtime_reflection_report": repo_rel(public_reflection_report_path),
        "object_model_debugger_contract": repo_rel(debugger_contract),
        "reserved_rows_not_promoted": reserved_rows,
    }


def _require_expected_generated_path(
    *,
    actual_path: str,
    expected_path: Any,
    field: str,
) -> None:
    actual = normalize_path(actual_path)
    expected = normalize_path(require_nonempty_string(expected_path, field))
    if actual != expected:
        raise RuntimeError(f"{field} drifted: expected {expected}, got {actual}")
    require_artifact(ROOT / expected, field)


def _require_expected_string_members(
    *,
    actual_values: set[str],
    expected_values: list[Any],
    field: str,
) -> list[str]:
    expected = [
        require_nonempty_string(value, f"{field}[{index}]")
        for index, value in enumerate(expected_values)
    ]
    missing = sorted(set(expected).difference(actual_values))
    if missing:
        raise RuntimeError(f"{field} missing expected values: " + ", ".join(missing))
    return expected


def _workspace_edge_labels(workspace: dict[str, Any], family_id: str) -> set[str]:
    labels: set[str] = set()
    for index, raw_edge in enumerate(
        require_list(workspace.get("package_edges"), f"{family_id}.workspace.package_edges")
    ):
        edge = require_object(raw_edge, f"{family_id}.workspace.package_edges[{index}]")
        labels.add(
            require_nonempty_string(edge.get("from"), f"{family_id}.workspace.package_edges[{index}].from")
            + " -> "
            + require_nonempty_string(edge.get("to"), f"{family_id}.workspace.package_edges[{index}].to")
        )
    return labels


def _require_path_under_root(path: Path, root: Path, field: str) -> Path:
    resolved_path = path.resolve()
    resolved_root = root.resolve()
    try:
        resolved_path.relative_to(resolved_root)
    except ValueError as exc:
        raise RuntimeError(
            f"{field} must stay under {repo_rel(resolved_root)}: {resolved_path.as_posix()}"
        ) from exc
    return resolved_path


def _require_generated_artifact_under(path_text: str, root: Path, field: str) -> str:
    normalized = normalize_path(require_nonempty_string(path_text, field))
    path = ROOT / normalized
    _require_path_under_root(path, root, field)
    require_artifact(path, field)
    return repo_rel(path)


def _reset_editor_tooling_probe_outputs(paths: Any) -> None:
    report_root = ROOT / "tmp" / "reports" / "developer-tooling" / "editor-surface"
    artifact_root = ROOT / "tmp" / "artifacts" / "developer-tooling" / "editor-surface"
    for path, root, field in (
        (paths.report_dir, report_root, "editor tooling report directory"),
        (paths.artifact_dir, artifact_root, "editor tooling artifact directory"),
    ):
        _require_path_under_root(path, root, field)
        if path.exists():
            shutil.rmtree(path)


def _validate_editor_tooling_probe_freshness(
    *,
    family_id: str,
    paths: Any,
    inputs: Any,
    published: Any,
    graph: dict[str, Any],
) -> dict[str, str]:
    report_root = ROOT / "tmp" / "reports" / "developer-tooling" / "editor-surface"
    artifact_root = ROOT / "tmp" / "artifacts" / "developer-tooling" / "editor-surface"
    summary_paths = require_object(inputs.summary.get("paths"), f"{family_id}.compile_summary.paths")
    if normalize_path(str(inputs.summary.get("out_dir", ""))) != repo_rel(paths.artifact_dir):
        raise RuntimeError(f"{family_id}.production frontend summary out_dir drifted from fresh artifact dir")
    if normalize_path(str(summary_paths.get("summary", ""))) != repo_rel(paths.compile_summary):
        raise RuntimeError(f"{family_id}.production frontend summary path drifted from fresh report dir")

    manifest_path = _require_generated_artifact_under(
        str(inputs.manifest_path_text or ""),
        artifact_root,
        f"{family_id}.production manifest path",
    )
    for key in ("diagnostics", "object"):
        _require_generated_artifact_under(
            str(summary_paths.get(key, "")),
            artifact_root,
            f"{family_id}.production {key} path",
        )
    compile_summary_path = _require_generated_artifact_under(
        repo_rel(paths.compile_summary),
        report_root,
        f"{family_id}.production compile summary path",
    )
    source_graph_path = _require_generated_artifact_under(
        str(published.source_graph_path),
        report_root,
        f"{family_id}.production source graph path",
    )
    debug_map_path = _require_generated_artifact_under(
        str(published.debug_path),
        report_root,
        f"{family_id}.production debug map path",
    )

    evidence = require_object(graph.get("evidence"), f"{family_id}.source_graph.evidence")
    if normalize_path(str(evidence.get("manifest_path", ""))) != manifest_path:
        raise RuntimeError(f"{family_id}.source graph evidence manifest path drifted from fresh compile output")
    if "manifest-source-graph-fields" not in require_string_list(
        evidence.get("source_truth_inputs"),
        f"{family_id}.source_graph.evidence.source_truth_inputs",
    ):
        raise RuntimeError(f"{family_id}.source graph must consume fresh compiler manifest source-graph fields")
    source_graph_digest = require_nonempty_string(
        graph.get("source_graph_digest"),
        f"{family_id}.source_graph.source_graph_digest",
    )
    if load_json(ROOT / source_graph_path).get("source_graph_digest") != source_graph_digest:
        raise RuntimeError(f"{family_id}.published source graph digest drifted from in-memory model")

    return {
        "compile_summary": compile_summary_path,
        "manifest": manifest_path,
        "source_graph": source_graph_path,
        "debug_map": debug_map_path,
    }


def _require_no_direct_import_statement(family_id: str, source_text: str) -> None:
    for line_number, line in enumerate(source_text.splitlines(), start=1):
        if line.strip().startswith("@import "):
            raise RuntimeError(
                f"{family_id} must not promote direct @import syntax on line {line_number}"
            )


def _require_source_text_identity(
    *,
    family_id: str,
    source_path: Path,
    expected_module_name: str,
    identity: dict[str, Any],
    field: str,
) -> dict[str, Any]:
    declared_source = require_source_owned_path(str(identity.get("source", "")), f"{field}.source")
    if repo_rel(declared_source) != repo_rel(source_path):
        raise RuntimeError(f"{field}.source drifted from workspace source")
    module_name = require_nonempty_string(identity.get("module_name"), f"{field}.module_name")
    if module_name != expected_module_name:
        raise RuntimeError(f"{field}.module_name drifted from workspace module name")
    expected_digest = require_nonempty_string(identity.get("source_sha256"), f"{field}.source_sha256").lower()
    actual_digest = sha256_file(source_path).lower()
    if expected_digest != actual_digest:
        raise RuntimeError(
            f"{field}.source_sha256 drifted for {repo_rel(source_path)}: expected {expected_digest}, got {actual_digest}"
        )
    source_text = source_path.read_text(encoding="utf-8")
    module_declaration = require_nonempty_string(
        identity.get("module_declaration"),
        f"{field}.module_declaration",
    )
    if module_declaration not in source_text:
        raise RuntimeError(f"{field}.module_declaration is missing from source text")
    required_tokens = [
        require_nonempty_string(token, f"{field}.required_tokens[{index}]")
        for index, token in enumerate(require_list(identity.get("required_tokens"), f"{field}.required_tokens"))
    ]
    missing_tokens = [token for token in required_tokens if token not in source_text]
    if missing_tokens:
        raise RuntimeError(f"{field} source text is missing required tokens: " + ", ".join(missing_tokens))
    return {
        "source": repo_rel(source_path),
        "module_name": module_name,
        "source_sha256": actual_digest,
        "module_declaration": module_declaration,
        "required_token_count": len(required_tokens),
    }


def _validate_workspace_source_text_identities(
    family_id: str,
    workspace: dict[str, Any],
    source_path: Path,
) -> list[dict[str, Any]]:
    identities: list[dict[str, Any]] = []
    module = require_object(workspace.get("module"), f"{family_id}.workspace.module")
    module_name = require_nonempty_string(module.get("module_name"), f"{family_id}.workspace.module.module_name")
    identities.append(
        _require_source_text_identity(
            family_id=family_id,
            source_path=source_path,
            expected_module_name=module_name,
            identity=require_object(
                module.get("source_text_identity"),
                f"{family_id}.workspace.module.source_text_identity",
            ),
            field=f"{family_id}.workspace.module.source_text_identity",
        )
    )

    for index, raw_module in enumerate(
        require_list(workspace.get("package_modules"), f"{family_id}.workspace.package_modules")
    ):
        package_module = require_object(raw_module, f"{family_id}.workspace.package_modules[{index}]")
        package_source = require_source_owned_path(
            str(package_module.get("source", "")),
            f"{family_id}.workspace.package_modules[{index}].source",
        )
        package_module_name = require_nonempty_string(
            package_module.get("module_name"),
            f"{family_id}.workspace.package_modules[{index}].module_name",
        )
        identities.append(
            _require_source_text_identity(
                family_id=family_id,
                source_path=package_source,
                expected_module_name=package_module_name,
                identity=require_object(
                    package_module.get("source_text_identity"),
                    f"{family_id}.workspace.package_modules[{index}].source_text_identity",
                ),
                field=f"{family_id}.workspace.package_modules[{index}].source_text_identity",
            )
        )
    return identities


def _first_matching_row(rows: Any, *, field_name: str, expected_value: str, field: str) -> dict[str, Any]:
    for index, raw_row in enumerate(require_list(rows, field)):
        row = require_object(raw_row, f"{field}[{index}]")
        if row.get(field_name) == expected_value:
            return row
    raise RuntimeError(f"{field} missing {field_name}={expected_value}")


def _require_source_owned_evidence_paths(paths: list[Any], field: str) -> list[str]:
    normalized: list[str] = []
    for index, raw_path in enumerate(paths):
        path_text = require_nonempty_string(raw_path, f"{field}[{index}]")
        normalized.append(repo_rel(require_source_owned_path(path_text, f"{field}[{index}]")))
    return normalized


def _validate_direct_import_source_truth(
    family_id: str,
    proof: dict[str, Any],
) -> dict[str, Any]:
    direct_import = require_object(
        proof.get("direct_import_source_truth"),
        f"{family_id}.text_collections_package_proof.direct_import_source_truth",
    )
    if direct_import.get("status") != "expected-pass":
        raise RuntimeError(f"{family_id}.direct_import_source_truth.status must be expected-pass")
    if direct_import.get("capability_id") != DIRECT_IMPORT_CAPABILITY_ID:
        raise RuntimeError(f"{family_id}.direct_import_source_truth.capability_id drifted")
    if direct_import.get("support_claim") != DIRECT_IMPORT_SUPPORT_CLAIM:
        raise RuntimeError(f"{family_id}.direct_import_source_truth.support_claim drifted")
    if direct_import.get("owner_phase") != DIRECT_IMPORT_OWNER_PHASE:
        raise RuntimeError(
            f"{family_id}.direct_import_source_truth.owner_phase must stay "
            f"{DIRECT_IMPORT_OWNER_PHASE}-owned"
        )
    if direct_import.get("public_replay_command") != DIRECT_IMPORT_PUBLIC_COMMAND:
        raise RuntimeError(f"{family_id}.direct_import_source_truth.public command drifted")

    consumer_fixture = require_source_owned_path(
        str(direct_import.get("consumer_fixture", "")),
        f"{family_id}.direct_import_source_truth.consumer_fixture",
    )
    provider_fixture = require_source_owned_path(
        str(direct_import.get("provider_fixture", "")),
        f"{family_id}.direct_import_source_truth.provider_fixture",
    )
    conformance_fixture = require_source_owned_path(
        str(direct_import.get("conformance_fixture", "")),
        f"{family_id}.direct_import_source_truth.conformance_fixture",
    )
    traceability_fixture = require_source_owned_path(
        str(direct_import.get("traceability_fixture", "")),
        f"{family_id}.direct_import_source_truth.traceability_fixture",
    )

    consumer_text = consumer_fixture.read_text(encoding="utf-8")
    provider_text = provider_fixture.read_text(encoding="utf-8")
    required_consumer_tokens = [
        require_nonempty_string(token, f"{family_id}.direct_import_source_truth.required_consumer_tokens[{index}]")
        for index, token in enumerate(
            require_list(
                direct_import.get("required_consumer_tokens"),
                f"{family_id}.direct_import_source_truth.required_consumer_tokens",
            )
        )
    ]
    missing_consumer_tokens = [token for token in required_consumer_tokens if token not in consumer_text]
    if missing_consumer_tokens:
        raise RuntimeError(
            f"{family_id}.direct_import_source_truth.consumer fixture missing tokens: "
            + ", ".join(missing_consumer_tokens)
        )
    if "@import " not in consumer_text:
        raise RuntimeError(f"{family_id}.direct_import_source_truth consumer fixture must contain direct @import")
    if "module moduleImportLookupProvider;" not in provider_text:
        raise RuntimeError(f"{family_id}.direct_import_source_truth provider module declaration drifted")

    canonical_manifest = load_json(CANONICAL_MANIFEST_PATH)
    support_claim = _first_matching_row(
        canonical_manifest.get("support_claims"),
        field_name="claim_id",
        expected_value=DIRECT_IMPORT_SUPPORT_CLAIM,
        field="canonical_manifest.support_claims",
    )
    if support_claim.get("owner_phase") != direct_import.get("owner_phase"):
        raise RuntimeError(f"{family_id}.direct_import_source_truth owner phase drifted from canonical manifest")
    if normalize_path(str(support_claim.get("behavior_fixture", ""))) != repo_rel(conformance_fixture):
        raise RuntimeError(f"{family_id}.direct_import_source_truth behavior fixture drifted from canonical manifest")
    if support_claim.get("executable_command") != DIRECT_IMPORT_PUBLIC_COMMAND:
        raise RuntimeError(f"{family_id}.direct_import_source_truth executable command drifted from canonical manifest")

    evidence_catalog = load_json(SUPPORT_CLAIM_EVIDENCE_CATALOG_PATH)
    evidence_row = _first_matching_row(
        evidence_catalog.get("rows"),
        field_name="support_claim",
        expected_value=DIRECT_IMPORT_SUPPORT_CLAIM,
        field="support_claim_evidence_catalog.rows",
    )
    if evidence_row.get("capability_id") != DIRECT_IMPORT_CAPABILITY_ID:
        raise RuntimeError(f"{family_id}.direct_import_source_truth evidence row capability drifted")
    if evidence_row.get("owner_phase") != direct_import.get("owner_phase"):
        raise RuntimeError(f"{family_id}.direct_import_source_truth evidence row owner drifted")
    if evidence_row.get("runnable_command") != DIRECT_IMPORT_PUBLIC_COMMAND:
        raise RuntimeError(f"{family_id}.direct_import_source_truth evidence command drifted")
    if normalize_path(str(evidence_row.get("conformance_fixture", ""))) != repo_rel(conformance_fixture):
        raise RuntimeError(f"{family_id}.direct_import_source_truth conformance fixture drifted")
    if normalize_path(str(evidence_row.get("traceability_fixture", ""))) != repo_rel(traceability_fixture):
        raise RuntimeError(f"{family_id}.direct_import_source_truth traceability fixture drifted")

    required_positive = _require_source_owned_evidence_paths(
        require_list(
            direct_import.get("required_positive_evidence"),
            f"{family_id}.direct_import_source_truth.required_positive_evidence",
        ),
        f"{family_id}.direct_import_source_truth.required_positive_evidence",
    )
    required_negative = _require_source_owned_evidence_paths(
        require_list(
            direct_import.get("required_negative_evidence"),
            f"{family_id}.direct_import_source_truth.required_negative_evidence",
        ),
        f"{family_id}.direct_import_source_truth.required_negative_evidence",
    )
    positive_evidence = {
        normalize_path(require_nonempty_string(path, "positive_evidence[]"))
        for path in require_list(evidence_row.get("positive_evidence"), "support_claim_evidence_catalog.positive_evidence")
    }
    negative_evidence = {
        normalize_path(require_nonempty_string(path, "negative_evidence[]"))
        for path in require_list(evidence_row.get("negative_evidence"), "support_claim_evidence_catalog.negative_evidence")
    }
    missing_positive = sorted(set(required_positive).difference(positive_evidence))
    missing_negative = sorted(set(required_negative).difference(negative_evidence))
    if missing_positive:
        raise RuntimeError(f"{family_id}.direct_import_source_truth missing positive evidence: " + ", ".join(missing_positive))
    if missing_negative:
        raise RuntimeError(f"{family_id}.direct_import_source_truth missing negative evidence: " + ", ".join(missing_negative))
    required_codes = set(
        require_string_list(
            direct_import.get("required_diagnostic_codes"),
            f"{family_id}.direct_import_source_truth.required_diagnostic_codes",
        )
    )
    actual_codes = {
        require_nonempty_string(code, "support_claim_evidence_catalog.required_diagnostic_codes[]")
        for code in require_list(
            evidence_row.get("required_diagnostic_codes"),
            "support_claim_evidence_catalog.required_diagnostic_codes",
        )
    }
    missing_codes = sorted(required_codes.difference(actual_codes))
    if missing_codes:
        raise RuntimeError(f"{family_id}.direct_import_source_truth missing diagnostic codes: " + ", ".join(missing_codes))
    source_truth_requirements = require_string_list(
        evidence_row.get("source_truth_requirements"),
        "support_claim_evidence_catalog.source_truth_requirements",
    )
    if not source_truth_requirements:
        raise RuntimeError(f"{family_id}.direct_import_source_truth requires source-truth requirements")

    return {
        "status": "PASS",
        "capability_id": DIRECT_IMPORT_CAPABILITY_ID,
        "support_claim": DIRECT_IMPORT_SUPPORT_CLAIM,
        "public_replay_command": DIRECT_IMPORT_PUBLIC_COMMAND,
        "consumer_fixture": repo_rel(consumer_fixture),
        "provider_fixture": repo_rel(provider_fixture),
        "conformance_fixture": repo_rel(conformance_fixture),
        "traceability_fixture": repo_rel(traceability_fixture),
        "positive_evidence_count": len(positive_evidence),
        "negative_evidence_count": len(negative_evidence),
        "source_truth_requirement_count": len(source_truth_requirements),
    }


def _validate_source_reference_closure_boundary(
    family_id: str,
    source_graph: dict[str, Any],
    graph: dict[str, Any],
) -> dict[str, Any]:
    expected_supported = source_graph.get("semantic_reference_closure_supported")
    if not isinstance(expected_supported, bool):
        raise RuntimeError(f"{family_id}.source_graph.semantic_reference_closure_supported must be boolean")
    evidence = require_object(graph.get("evidence"), f"{family_id}.source_graph.evidence")
    actual_supported = evidence.get("semantic_reference_closure") is True
    if actual_supported != expected_supported:
        raise RuntimeError(
            f"{family_id}.source graph semantic reference closure drifted: "
            f"expected {expected_supported}, got {actual_supported}"
        )

    required_consumers = require_string_list(
        source_graph.get("required_fail_closed_consumers"),
        f"{family_id}.source_graph.required_fail_closed_consumers",
    )
    required_diagnostics = require_string_list(
        source_graph.get("required_fail_closed_diagnostics"),
        f"{family_id}.source_graph.required_fail_closed_diagnostics",
    )
    required_remaining_work = require_string_list(
        source_graph.get("required_remaining_native_compiler_work"),
        f"{family_id}.source_graph.required_remaining_native_compiler_work",
    )
    minimum_reference_candidates = source_graph.get("minimum_reference_candidates")
    if not isinstance(minimum_reference_candidates, int) or isinstance(minimum_reference_candidates, bool):
        raise RuntimeError(f"{family_id}.source_graph.minimum_reference_candidates must be an integer")

    if expected_supported:
        if graph.get("fail_closed") is True:
            raise RuntimeError(f"{family_id}.source graph must not fail closed when semantic closure is supported")
        return {
            "status": "PASS",
            "semantic_reference_closure_supported": True,
            "fail_closed": False,
        }

    if graph.get("fail_closed") is not True:
        raise RuntimeError(f"{family_id}.source graph must fail closed without semantic reference closure")
    if evidence.get("lexical_candidates_authoritative") is not False:
        raise RuntimeError(f"{family_id}.source graph lexical candidates must remain non-authoritative")
    if int(graph.get("reference_candidate_count", 0) or 0) < minimum_reference_candidates:
        raise RuntimeError(
            f"{family_id}.source graph expected at least {minimum_reference_candidates} reference candidates"
        )
    diagnostics = require_list(graph.get("diagnostics"), f"{family_id}.source_graph.diagnostics")
    diagnostic_codes = {
        require_nonempty_string(
            require_object(entry, f"{family_id}.source_graph.diagnostics[]").get("code"),
            f"{family_id}.source_graph.diagnostics[].code",
        )
        for entry in diagnostics
    }
    missing_diagnostics = sorted(set(required_diagnostics).difference(diagnostic_codes))
    if missing_diagnostics:
        raise RuntimeError(
            f"{family_id}.source graph missing fail-closed diagnostics: " + ", ".join(missing_diagnostics)
        )
    consumer_capabilities = require_object(
        graph.get("consumer_capabilities"),
        f"{family_id}.source_graph.consumer_capabilities",
    )
    fail_closed_consumers: list[str] = []
    for consumer_name in required_consumers:
        consumer = require_object(
            consumer_capabilities.get(consumer_name),
            f"{family_id}.source_graph.consumer_capabilities.{consumer_name}",
        )
        if consumer.get("supported") is not False or consumer.get("fail_closed") is not True:
            raise RuntimeError(f"{family_id}.source graph consumer {consumer_name} must fail closed")
        fail_closed_consumers.append(consumer_name)
    remaining_work = set(
        require_string_list(
            graph.get("remaining_native_compiler_work"),
            f"{family_id}.source_graph.remaining_native_compiler_work",
        )
    )
    missing_remaining_work = sorted(set(required_remaining_work).difference(remaining_work))
    if missing_remaining_work:
        raise RuntimeError(
            f"{family_id}.source graph missing native work blockers: " + ", ".join(missing_remaining_work)
        )
    return {
        "status": "PASS",
        "semantic_reference_closure_supported": False,
        "fail_closed": True,
        "reference_candidate_count": int(graph.get("reference_candidate_count", 0) or 0),
        "fail_closed_consumers": fail_closed_consumers,
        "fail_closed_diagnostics": sorted(required_diagnostics),
        "remaining_native_compiler_work": sorted(required_remaining_work),
    }


def _validate_missing_provider_link_failure(
    family_id: str,
    executable_proof: dict[str, Any],
    artifact_dir: Path,
) -> dict[str, Any]:
    compile_dir = ROOT / normalize_path(
        require_nonempty_string(executable_proof.get("compile_dir"), f"{family_id}.compile_dir")
    )
    obj_path = compile_dir / "module.obj"
    require_artifact(obj_path, "text package missing-provider object")
    runtime_library, driver_flags = load_runtime_launch_inputs(compile_dir)
    missing_provider_exe = artifact_dir / "missing-provider.exe"
    missing_provider_log = artifact_dir / "missing-provider-link.log"
    result = run_checked(
        [
            resolve_clangxx(),
            *link_driver_args(),
            str(obj_path),
            str(runtime_library),
            *driver_flags,
            "-o",
            str(missing_provider_exe),
            "-fno-color-diagnostics",
        ],
        cwd=ROOT,
        log_path=missing_provider_log,
        domain=f"{family_id}.package trust diagnostic",
        check=False,
    )
    if result.returncode == 0:
        raise RuntimeError(f"{family_id}.package trust diagnostic linked without provider package")
    return {
        "status": "PASS",
        "expected_failure_stage": "link",
        "actual_exit_code": result.returncode,
        "log": repo_rel(missing_provider_log),
    }


def _validate_text_runtime_package_execution_boundary(
    family_id: str,
    expectation: dict[str, Any],
    proof: dict[str, Any],
    executable_proof: dict[str, Any] | None,
) -> dict[str, Any]:
    runtime_package = require_object(
        proof.get("runtime_package_execution"),
        f"{family_id}.text_collections_package_proof.runtime_package_execution",
    )
    status = require_nonempty_string(
        runtime_package.get("status"),
        f"{family_id}.runtime_package_execution.status",
    )
    if status not in ALLOWED_SECTION_STATES:
        raise RuntimeError(f"{family_id}.runtime_package_execution.status is unknown: {status}")

    runtime = require_object(expectation["runtime"], f"{family_id}.runtime")
    compile_manifest = require_object(expectation["compile_manifest"], f"{family_id}.compile_manifest")
    if executable_proof is not None:
        require_expected_pass_section(runtime, f"{family_id}.runtime")
        require_expected_pass_section(compile_manifest, f"{family_id}.compile_manifest")
        if status != "expected-pass":
            raise RuntimeError(f"{family_id}.runtime_package_execution.status must be expected-pass with executable proof")
        return {
            "status": "PASS",
            "runtime_status": "expected-pass",
            "compile_manifest_status": "expected-pass",
        }

    require_blocked_section(runtime, f"{family_id}.runtime")
    require_blocked_section(compile_manifest, f"{family_id}.compile_manifest")
    for section_name, section in (("runtime", runtime), ("compile_manifest", compile_manifest)):
        if section.get("blocker_id") != TEXT_PACKAGE_RUNTIME_FFI_BLOCKER_ID:
            raise RuntimeError(f"{family_id}.{section_name}.blocker_id must name the FFI preservation blocker")
        concrete_blocker = require_nonempty_string(
            section.get("concrete_blocker"),
            f"{family_id}.{section_name}.concrete_blocker",
        )
        if TEXT_PACKAGE_RUNTIME_FFI_BLOCKER_TEXT not in concrete_blocker:
            raise RuntimeError(f"{family_id}.{section_name}.concrete_blocker drifted from the known compiler blocker")

    if status != "reserved":
        raise RuntimeError(f"{family_id}.runtime_package_execution.status must stay reserved without executable proof")
    if runtime_package.get("blocker_id") != TEXT_PACKAGE_RUNTIME_FFI_BLOCKER_ID:
        raise RuntimeError(f"{family_id}.runtime_package_execution.blocker_id drifted")
    if runtime_package.get("blocked_provider_module") != "CrossLaneFixtureProvider":
        raise RuntimeError(f"{family_id}.runtime_package_execution.blocked_provider_module drifted")
    failing_stage = require_nonempty_string(
        runtime_package.get("failing_stage"),
        f"{family_id}.runtime_package_execution.failing_stage",
    )
    concrete_blocker = require_nonempty_string(
        runtime_package.get("concrete_blocker"),
        f"{family_id}.runtime_package_execution.concrete_blocker",
    )
    if TEXT_PACKAGE_RUNTIME_FFI_BLOCKER_TEXT not in concrete_blocker:
        raise RuntimeError(f"{family_id}.runtime_package_execution.concrete_blocker drifted")
    remaining_work = require_string_list(
        runtime_package.get("required_remaining_native_compiler_work"),
        f"{family_id}.runtime_package_execution.required_remaining_native_compiler_work",
    )
    if not any("ffi" in item.lower() and "preservation" in item.lower() for item in remaining_work):
        raise RuntimeError(f"{family_id}.runtime_package_execution must keep FFI preservation native work explicit")

    negative_case = _first_matching_row(
        expectation.get("negative_cases"),
        field_name="case_id",
        expected_value="text-collection-package-runtime-ffi-preservation-reserved",
        field=f"{family_id}.negative_cases",
    )
    if negative_case.get("status") != "reserved":
        raise RuntimeError(f"{family_id}.runtime FFI preservation negative case must stay reserved")
    if negative_case.get("blocker_id") != TEXT_PACKAGE_RUNTIME_FFI_BLOCKER_ID:
        raise RuntimeError(f"{family_id}.runtime FFI preservation negative case blocker drifted")

    return {
        "status": "RESERVED",
        "runtime_status": runtime.get("status"),
        "compile_manifest_status": compile_manifest.get("status"),
        "blocker_id": TEXT_PACKAGE_RUNTIME_FFI_BLOCKER_ID,
        "blocked_provider_module": "CrossLaneFixtureProvider",
        "failing_stage": failing_stage,
        "concrete_blocker": concrete_blocker,
        "required_remaining_native_compiler_work": remaining_work,
        "negative_case": negative_case["case_id"],
    }


def validate_text_collections_package_proof(
    family_id: str,
    source_path: Path,
    expectation: dict[str, Any],
    workspace: dict[str, Any],
    executable_proof: dict[str, Any] | None,
) -> dict[str, Any] | None:
    if family_id != "text_collections_package":
        return None

    proof = require_object(
        expectation.get("text_collections_package_proof"),
        f"{family_id}.text_collections_package_proof",
    )
    if proof.get("contract_id") != TEXT_COLLECTIONS_PACKAGE_PROOF_CONTRACT_ID:
        raise RuntimeError(f"{family_id}.text_collections_package_proof contract_id drifted")
    proof_status = require_nonempty_string(proof.get("status"), f"{family_id}.text_collections_package_proof.status")
    if proof_status not in ALLOWED_SECTION_STATES:
        raise RuntimeError(f"{family_id}.text_collections_package_proof.status is unknown: {proof_status}")
    if executable_proof is None and proof_status not in BLOCKED_STATES:
        raise RuntimeError(f"{family_id}.text collections package proof must stay blocker-explicit without executable proof")
    if executable_proof is not None and proof_status != "expected-pass":
        raise RuntimeError(f"{family_id}.text collections package proof must be expected-pass with executable proof")
    if proof.get("runs_canonical_frontend") is not True:
        raise RuntimeError(f"{family_id}.text package proof must run the canonical frontend")
    if proof.get("direct_module_import_syntax_promoted") is not True:
        raise RuntimeError(f"{family_id}.text package proof must promote direct @import only through source truth")
    if proof.get("text_fixture_uses_direct_module_import_syntax") is not False:
        raise RuntimeError(f"{family_id}.text executable fixture must keep package import on objc_import_module metadata")

    source_text = source_path.read_text(encoding="utf-8")
    _require_no_direct_import_statement(family_id, source_text)
    if 'objc_import_module(named("CrossLaneFixtureProvider"))' not in source_text:
        raise RuntimeError(f"{family_id} source must use the checked metadata import surface")
    source_text_identities = _validate_workspace_source_text_identities(family_id, workspace, source_path)
    direct_import_source_truth = _validate_direct_import_source_truth(family_id, proof)
    runtime_package_execution = _validate_text_runtime_package_execution_boundary(
        family_id,
        expectation,
        proof,
        executable_proof,
    )

    source_graph = require_object(expectation["source_graph"], f"{family_id}.source_graph")
    debug_source_map = require_object(expectation["debug_source_map"], f"{family_id}.debug_source_map")
    require_expected_pass_section(source_graph, f"{family_id}.source_graph")
    require_expected_pass_section(debug_source_map, f"{family_id}.debug_source_map")

    expected_workspace_edges = _require_expected_string_members(
        actual_values=_workspace_edge_labels(workspace, family_id),
        expected_values=require_list(source_graph.get("expected_workspace_edges"), f"{family_id}.source_graph.expected_workspace_edges"),
        field=f"{family_id}.source_graph.expected_workspace_edges",
    )

    artifact_dir = ARTIFACT_ROOT / slug_from_family_id(family_id)
    compile_dir: Path | None = None
    link_plan_path: Path | None = None
    provider_surface_path: Path | None = None
    if executable_proof is not None:
        compile_dir = ROOT / normalize_path(
            require_nonempty_string(executable_proof.get("compile_dir"), f"{family_id}.compile_dir")
        )
        link_plan_path = compile_dir / "module.cross-module-runtime-link-plan.json"
        link_plan = _require_json_artifact(link_plan_path, "text package cross-module runtime link plan")
        if link_plan.get("ready") is not True:
            raise RuntimeError(f"{family_id}.package trust diagnostic link plan must be ready")
        imported_modules = require_list(link_plan.get("imported_modules"), f"{family_id}.link_plan.imported_modules")
        imported_module_names = {
            require_nonempty_string(
                require_object(module, f"{family_id}.link_plan.imported_modules[]").get("module_name"),
                f"{family_id}.link_plan.imported_modules[].module_name",
            )
            for module in imported_modules
        }
        if "CrossLaneFixtureProvider" not in imported_module_names:
            raise RuntimeError(f"{family_id}.package trust diagnostic did not import CrossLaneFixtureProvider")

        package_module_proofs = require_list(
            executable_proof.get("package_module_proofs"),
            f"{family_id}.executable_proof.package_module_proofs",
        )
        provider_proof = None
        for raw_provider in package_module_proofs:
            provider = require_object(raw_provider, f"{family_id}.package_module_proofs[]")
            if provider.get("module_name") == "CrossLaneFixtureProvider":
                provider_proof = provider
                break
        if provider_proof is None:
            raise RuntimeError(f"{family_id}.package trust diagnostic missing provider proof")
        provider_surface_path = ROOT / normalize_path(
            require_nonempty_string(
                provider_proof.get("runtime_import_surface"),
                f"{family_id}.provider.runtime_import_surface",
            )
        )
        provider_surface = _require_json_artifact(provider_surface_path, "text package provider import surface")
        if provider_surface.get("module_name") != "CrossLaneFixtureProvider":
            raise RuntimeError(f"{family_id}.provider import surface module drifted")
        if provider_surface.get("ready_for_import_artifact_emission") is not True:
            raise RuntimeError(f"{family_id}.provider import surface is not artifact-emission ready")
        if provider_surface.get("ready_for_frontend_module_consumption") is not True:
            raise RuntimeError(f"{family_id}.provider import surface is not frontend-consumption ready")
        if int(provider_surface.get("function_decl_count", 0) or 0) < 1:
            raise RuntimeError(f"{family_id}.provider import surface must export at least one function")

    paths = paths_for_source(resolve_source(repo_rel(source_path)))
    _reset_editor_tooling_probe_outputs(paths)
    compile_result = run_frontend_compile(paths)
    if compile_result.returncode != 0 or not compile_result.summary_available:
        raise RuntimeError(
            f"{family_id}.source-map/debug diagnostic production frontend probe failed "
            f"with exit {compile_result.returncode}"
        )
    inputs = load_editor_tooling_inputs(paths)
    model = build_editor_tooling_model(paths, inputs)
    published = publish_editor_tooling_surface(paths=paths, inputs=inputs, model=model)
    if inputs.summary.get("success") is not True or inputs.summary.get("status") != 0:
        raise RuntimeError(f"{family_id}.production frontend summary must report success")
    if normalize_path(str(inputs.summary.get("input_path", ""))) != repo_rel(source_path):
        raise RuntimeError(f"{family_id}.production frontend summary input path drifted")

    summary_paths = require_object(inputs.summary.get("paths"), f"{family_id}.compile_summary.paths")
    artifact_path_keys = {
        "manifest": "manifest",
        "ir": "ir",
        "object": "object",
        "runtime-metadata-binary": "runtime_metadata_binary",
    }
    emitted_artifacts: dict[str, str] = {}
    for index, raw_kind in enumerate(
        require_list(proof.get("required_artifact_kinds"), f"{family_id}.required_artifact_kinds")
    ):
        kind = require_nonempty_string(raw_kind, f"{family_id}.required_artifact_kinds[{index}]")
        if kind == "summary":
            candidate = paths.compile_summary
        elif kind == "source-graph":
            candidate = paths.source_graph
        elif kind == "artifact-inspector":
            candidate = paths.artifact_inspector
        elif kind == "debug-map":
            candidate = paths.debug_map
        elif kind == "runtime-import-surface":
            if compile_dir is None:
                raise RuntimeError(f"{family_id}.runtime-import-surface artifact must stay reserved without executable proof")
            candidate = compile_dir / "module.runtime-import-surface.json"
        elif kind == "cross-module-runtime-link-plan":
            if link_plan_path is None:
                raise RuntimeError(f"{family_id}.cross-module-runtime-link-plan artifact must stay reserved without executable proof")
            candidate = link_plan_path
        else:
            path_key = artifact_path_keys.get(kind)
            if path_key is None:
                raise RuntimeError(f"{family_id}.text package proof unknown artifact kind: {kind}")
            candidate = ROOT / normalize_path(
                require_nonempty_string(summary_paths.get(path_key), f"{family_id}.summary.paths.{path_key}")
            )
        require_artifact(candidate, f"text package {kind}")
        emitted_artifacts[kind] = repo_rel(candidate)

    _require_expected_generated_path(
        actual_path=published.source_graph_path,
        expected_path=source_graph.get("expected_path"),
        field=f"{family_id}.source_graph.expected_path",
    )
    _require_expected_generated_path(
        actual_path=published.debug_path,
        expected_path=debug_source_map.get("expected_path"),
        field=f"{family_id}.debug_source_map.expected_path",
    )

    graph = require_object(model.source_graph, f"{family_id}.source_graph.payload")
    if graph.get("available") is not True:
        raise RuntimeError(f"{family_id}.source graph must be available on the production artifact path")
    if graph.get("fail_closed") is not True:
        raise RuntimeError(f"{family_id}.source graph must keep unsupported reference consumers fail-closed")
    evidence = require_object(graph.get("evidence"), f"{family_id}.source_graph.evidence")
    if evidence.get("native_compiler_source_graph_present") is not True:
        raise RuntimeError(f"{family_id}.source graph must expose native compiler source graph fields")
    editor_tooling_freshness = _validate_editor_tooling_probe_freshness(
        family_id=family_id,
        paths=paths,
        inputs=inputs,
        published=published,
        graph=graph,
    )
    source_reference_closure = _validate_source_reference_closure_boundary(family_id, source_graph, graph)
    graph_nodes = require_list(graph.get("nodes"), f"{family_id}.source_graph.nodes")
    package_node_ids = {
        str(require_object(node, f"{family_id}.source_graph.nodes[]").get("owning_package", ""))
        for node in graph_nodes
        if isinstance(node, dict) and node.get("symbol_kind") == "package"
    }
    expected_package_nodes = _require_expected_string_members(
        actual_values=package_node_ids,
        expected_values=require_list(
            source_graph.get("expected_package_nodes"),
            f"{family_id}.source_graph.expected_package_nodes",
        ),
        field=f"{family_id}.source_graph.expected_package_nodes",
    )
    minimums = require_object(proof.get("minimums"), f"{family_id}.text_collections_package_proof.minimums")
    source_graph_nodes = validate_minimum_count(
        family_id=family_id,
        actual=graph.get("node_count"),
        minimums=minimums,
        minimum_key="source_graph_nodes",
        domain="source graph",
    )
    package_nodes = validate_minimum_count(
        family_id=family_id,
        actual=graph.get("package_node_count"),
        minimums=minimums,
        minimum_key="package_nodes",
        domain="source graph",
    )

    debug_map = require_object(model.debug, f"{family_id}.debug_source_map.payload")
    if debug_map.get("supported") is not True or debug_map.get("object_artifact_present") is not True:
        raise RuntimeError(f"{family_id}.debug map must be tied to the emitted object artifact")
    if debug_map.get("source_map_supported") is not False:
        raise RuntimeError(f"{family_id}.debug map must keep full source maps fail-closed")
    if debug_map.get("statement_level_stepping") is not False:
        raise RuntimeError(f"{family_id}.debug map must keep statement stepping fail-closed")
    declaration_breakpoint_anchors = validate_minimum_count(
        family_id=family_id,
        actual=debug_map.get("declaration_breakpoint_anchor_count"),
        minimums=minimums,
        minimum_key="declaration_breakpoint_anchors",
        domain="debug map",
    )
    actual_debug_anchors = {
        str(require_object(anchor, f"{family_id}.debug_map.declaration_breakpoints[]").get("symbol", ""))
        for anchor in require_list(
            debug_map.get("declaration_breakpoints"),
            f"{family_id}.debug_map.declaration_breakpoints",
        )
    }
    required_debug_anchors = _require_expected_string_members(
        actual_values=actual_debug_anchors,
        expected_values=require_list(
            debug_source_map.get("required_declaration_breakpoints"),
            f"{family_id}.debug_source_map.required_declaration_breakpoints",
        ),
        field=f"{family_id}.debug_source_map.required_declaration_breakpoints",
    )

    reserved_rows = require_list(
        proof.get("reserved_rows_not_promoted"),
        f"{family_id}.text_collections_package_proof.reserved_rows_not_promoted",
    )
    for required_row in (
        "runtime.debug-trace.statement-stepping",
        "runtime.debug-trace.full-source-map-publication",
        "developer-tooling.source-graph.semantic-reference-closure",
    ):
        if required_row not in reserved_rows:
            raise RuntimeError(f"{family_id}.text package proof must keep {required_row} unpromoted")
    if executable_proof is None and "runtime.interop.package-loader-bridge" not in reserved_rows:
        raise RuntimeError(f"{family_id}.text package proof must keep package-loader bridge reserved without runtime proof")
    if executable_proof is not None and "runtime.interop.package-loader-bridge" in reserved_rows:
        raise RuntimeError(f"{family_id}.text package proof must not reserve package-loader bridge with runtime proof")
    if DIRECT_IMPORT_CAPABILITY_ID in reserved_rows:
        raise RuntimeError(f"{family_id}.text package proof must use direct import source truth, not a reserved row")

    payload = {
        "status": "PASS",
        "compile_summary": repo_rel(paths.compile_summary),
        "editor_tooling_freshness": editor_tooling_freshness,
        "source_graph": published.source_graph_path,
        "debug_map": published.debug_path,
        "emitted_artifacts": emitted_artifacts,
        "source_text_identities": source_text_identities,
        "direct_import_source_truth": direct_import_source_truth,
        "runtime_package_execution": runtime_package_execution,
        "workspace_edges": expected_workspace_edges,
        "source_reference_closure": source_reference_closure,
        "source_graph_nodes": source_graph_nodes,
        "package_nodes": package_nodes,
        "expected_package_nodes": expected_package_nodes,
        "declaration_breakpoint_anchors": declaration_breakpoint_anchors,
        "required_debug_anchors": required_debug_anchors,
        "reserved_rows_not_promoted": reserved_rows,
    }
    if executable_proof is not None:
        missing_provider = _validate_missing_provider_link_failure(
            family_id,
            executable_proof,
            artifact_dir,
        )
        missing_provider_case = _first_matching_row(
            expectation.get("negative_cases"),
            field_name="case_id",
            expected_value="text-collection-package-missing-provider-link-fail-closed",
            field=f"{family_id}.negative_cases",
        )
        if missing_provider_case.get("status") != "expected-pass":
            raise RuntimeError(f"{family_id}.missing-provider negative case must be expected-pass")
        if missing_provider_case.get("responsible_domain") != "package trust diagnostic":
            raise RuntimeError(f"{family_id}.missing-provider negative case must be package-trust owned")
        if provider_surface_path is not None:
            payload["provider_import_surface"] = repo_rel(provider_surface_path)
        if link_plan_path is not None:
            payload["cross_module_runtime_link_plan"] = repo_rel(link_plan_path)
        payload["missing_provider_negative_case"] = missing_provider
        payload["missing_provider_negative_case"]["case_id"] = missing_provider_case["case_id"]
    return payload


def run_public_workflow_action(
    action: str,
    *,
    args: list[str] | None = None,
    log_path: Path,
    domain: str,
) -> None:
    run_checked(
        [sys.executable, "-m", "scripts.objc3c_workflow", action, *(args or [])],
        cwd=ROOT,
        log_path=log_path,
        domain=domain,
    )


def _require_json_artifact(path: Path, label: str) -> dict[str, Any]:
    require_artifact(path, label)
    return load_json(path)


def _require_receipt_operation(receipt: dict[str, Any], operation: str, expected_order: list[str]) -> None:
    if receipt.get("operation") != operation:
        raise RuntimeError(f"distribution lifecycle {operation} receipt operation drifted")
    if receipt.get("network_policy") != "no-network-during-validation":
        raise RuntimeError(f"distribution lifecycle {operation} receipt network policy drifted")
    if receipt.get("hosted_registry_support") != "unsupported-fail-closed-if-claimed":
        raise RuntimeError(f"distribution lifecycle {operation} receipt hosted-registry boundary drifted")
    if receipt.get("package_order") != expected_order:
        raise RuntimeError(f"distribution lifecycle {operation} receipt package order drifted")


def _lock_package_trust_signatures(lock: dict[str, Any]) -> dict[str, str]:
    signatures: dict[str, str] = {}
    for raw_package in lock.get("packages", []):
        if not isinstance(raw_package, dict):
            continue
        package_id = str(raw_package.get("package_id", ""))
        trust = raw_package.get("trust", {})
        if package_id and isinstance(trust, dict):
            signatures[package_id] = str(trust.get("signature", ""))
    return signatures


def _channel_manifest_entries_by_id(
    family_id: str,
    channel_manifest: dict[str, Any],
) -> dict[str, dict[str, Any]]:
    entries: dict[str, dict[str, Any]] = {}
    for index, raw_entry in enumerate(
        require_list(
            channel_manifest.get("channel_manifests"),
            f"{family_id}.release_operations.channel_manifest.channel_manifests",
        )
    ):
        entry = require_object(
            raw_entry,
            f"{family_id}.release_operations.channel_manifest.channel_manifests[{index}]",
        )
        channel_id = require_nonempty_string(
            entry.get("channel_id"),
            f"{family_id}.release_operations.channel_manifest.channel_manifests[{index}].channel_id",
        )
        entries[channel_id] = entry
    return entries


def validate_distribution_release_operations_model(family_id: str) -> dict[str, Any]:
    model = _require_json_artifact(
        RELEASE_CHANNEL_OPERATIONS_MODEL_PATH,
        "release operations channel model",
    )
    if model.get("contract_id") != "objc3c.release.operations.channel.operations.model.v1":
        raise RuntimeError(f"{family_id}.release operations channel model contract drifted")
    channels = require_list(model.get("channels"), f"{family_id}.release_operations.channels")
    rollback_channels: dict[str, str] = {}
    clean_install_channels: list[str] = []
    gate_actions: dict[str, list[str]] = {}
    for index, raw_channel in enumerate(channels):
        channel = require_object(raw_channel, f"{family_id}.release_operations.channels[{index}]")
        channel_id = require_nonempty_string(
            channel.get("channel_id"),
            f"{family_id}.release_operations.channels[{index}].channel_id",
        )
        prerequisite = require_object(
            channel.get("clean_install_prerequisite"),
            f"{family_id}.{channel_id}.clean_install_prerequisite",
        )
        if prerequisite.get("required_action") != "validate-package-install-distribution":
            raise RuntimeError(f"{family_id}.{channel_id} clean install prerequisite action drifted")
        if prerequisite.get("required_flag") != "--from-nothing":
            raise RuntimeError(f"{family_id}.{channel_id} clean install prerequisite flag drifted")
        if (
            prerequisite.get("required_summary")
            != "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
        ):
            raise RuntimeError(f"{family_id}.{channel_id} clean install prerequisite summary drifted")
        if prerequisite.get("blocks_publication_on_failure") is not True:
            raise RuntimeError(f"{family_id}.{channel_id} clean install prerequisite must block publication")
        clean_install_channels.append(channel_id)

        rollback = require_object(channel.get("rollback_safety"), f"{family_id}.{channel_id}.rollback_safety")
        rollback_channel = require_nonempty_string(
            rollback.get("rollback_channel"),
            f"{family_id}.{channel_id}.rollback_channel",
        )
        if rollback.get("blocks_publication_on_failure") is not True:
            raise RuntimeError(f"{family_id}.{channel_id} rollback safety must block publication")
        operator_command = require_nonempty_string(
            rollback.get("operator_command"),
            f"{family_id}.{channel_id}.rollback_operator_command",
        )
        if not operator_command.startswith("npm run objc3c -- "):
            raise RuntimeError(f"{family_id}.{channel_id} rollback command must use the public objc3c surface")
        rollback_channels[channel_id] = rollback_channel

        actions = [
            require_nonempty_string(action, f"{family_id}.{channel_id}.release_gate_actions[]")
            for action in require_list(channel.get("release_gate_actions"), f"{family_id}.{channel_id}.release_gate_actions")
        ]
        if not actions:
            raise RuntimeError(f"{family_id}.{channel_id} release gate actions cannot be empty")
        gate_actions[channel_id] = actions

    for required_channel in ("stable", "candidate", "nightly", "preview"):
        if required_channel not in rollback_channels:
            raise RuntimeError(f"{family_id}.release operations missing channel {required_channel}")
        if required_channel not in clean_install_channels:
            raise RuntimeError(f"{family_id}.release operations missing clean install prerequisite for {required_channel}")
    if rollback_channels.get("stable") != "local-installer":
        raise RuntimeError(f"{family_id}.stable rollback channel drifted")
    if rollback_channels.get("nightly") != "offline-bundle":
        raise RuntimeError(f"{family_id}.nightly rollback channel drifted")

    fail_closed_rules = require_list(model.get("fail_closed_rules"), f"{family_id}.release_operations.fail_closed_rules")
    if not any(
        isinstance(rule, dict)
        and rule.get("rule_id") == "missing-rollback-proof"
        and rule.get("blocks_publication") is True
        for rule in fail_closed_rules
    ):
        raise RuntimeError(f"{family_id}.release operations must fail closed on missing rollback proof")
    if "validate-release-operations-end-to-end" not in gate_actions.get("stable", []):
        raise RuntimeError(f"{family_id}.stable release gate must include release operations end-to-end validation")

    return {
        "model": repo_rel(RELEASE_CHANNEL_OPERATIONS_MODEL_PATH),
        "clean_install_prerequisite_channels": sorted(clean_install_channels),
        "rollback_channels": rollback_channels,
        "release_gate_actions": gate_actions,
        "fail_closed_rule_count": len(fail_closed_rules),
    }


def validate_distribution_release_operations_lifecycle_proof(
    family_id: str,
    release_model: dict[str, Any],
    release_summary: dict[str, Any],
    update_manifest: dict[str, Any],
    channel_manifest: dict[str, Any],
) -> dict[str, Any]:
    if release_summary.get("contract_id") != "objc3c.release.operations.end-to-end.summary.v1":
        raise RuntimeError(f"{family_id}.release operations end-to-end summary contract drifted")
    if release_summary.get("status") != "PASS":
        raise RuntimeError(f"{family_id}.release operations end-to-end summary did not pass")
    if update_manifest.get("contract_id") != "objc3c.release.operations.update-manifest.v1":
        raise RuntimeError(f"{family_id}.release operations update manifest contract drifted")
    if channel_manifest.get("contract_id") != "objc3c.release.operations.channel-manifest.v1":
        raise RuntimeError(f"{family_id}.release operations channel manifest contract drifted")

    if release_summary.get("update_manifest") != repo_rel(RELEASE_UPDATE_MANIFEST_PATH):
        raise RuntimeError(f"{family_id}.release operations summary update manifest link drifted")
    if release_summary.get("release_channel_manifest") != repo_rel(RELEASE_CHANNEL_MANIFEST_PATH):
        raise RuntimeError(f"{family_id}.release operations summary channel manifest link drifted")
    if update_manifest.get("release_channel_manifest") != repo_rel(RELEASE_CHANNEL_MANIFEST_PATH):
        raise RuntimeError(f"{family_id}.release operations update manifest channel link drifted")

    expected_channels = ["stable", "candidate", "nightly", "preview"]
    summary_channels = require_string_list(release_summary.get("channels"), f"{family_id}.release_operations.summary.channels")
    if summary_channels != expected_channels:
        raise RuntimeError(f"{family_id}.release operations summary channel order drifted")
    manifest_channels = [
        require_nonempty_string(
            require_object(raw_channel, f"{family_id}.release_operations.update_manifest.channels[{index}]").get("channel_id"),
            f"{family_id}.release_operations.update_manifest.channels[{index}].channel_id",
        )
        for index, raw_channel in enumerate(
            require_list(update_manifest.get("channels"), f"{family_id}.release_operations.update_manifest.channels")
        )
    ]
    if manifest_channels != expected_channels:
        raise RuntimeError(f"{family_id}.release operations update manifest channel order drifted")

    summary_clean_channels = sorted(
        require_string_list(
            release_summary.get("clean_install_prerequisite_channels"),
            f"{family_id}.release_operations.summary.clean_install_prerequisite_channels",
        )
    )
    if summary_clean_channels != release_model["clean_install_prerequisite_channels"]:
        raise RuntimeError(f"{family_id}.release operations clean-install prerequisite proof drifted")

    channel_entries = _channel_manifest_entries_by_id(family_id, channel_manifest)
    stable_entry = require_object(channel_entries.get("stable"), f"{family_id}.release_operations.stable_channel")
    nightly_entry = require_object(channel_entries.get("nightly"), f"{family_id}.release_operations.nightly_channel")
    stable_gates = require_string_list(
        stable_entry.get("release_gate_actions"),
        f"{family_id}.release_operations.stable.release_gate_actions",
    )
    nightly_gates = require_string_list(
        nightly_entry.get("release_gate_actions"),
        f"{family_id}.release_operations.nightly.release_gate_actions",
    )
    summary_stable_gates = require_string_list(
        release_summary.get("stable_gate_actions"),
        f"{family_id}.release_operations.summary.stable_gate_actions",
    )
    summary_nightly_gates = require_string_list(
        release_summary.get("nightly_gate_actions"),
        f"{family_id}.release_operations.summary.nightly_gate_actions",
    )
    if stable_gates != summary_stable_gates or stable_gates != release_model["release_gate_actions"]["stable"]:
        raise RuntimeError(f"{family_id}.release operations stable gate proof drifted")
    if nightly_gates != summary_nightly_gates or nightly_gates != release_model["release_gate_actions"]["nightly"]:
        raise RuntimeError(f"{family_id}.release operations nightly gate proof drifted")
    if "validate-release-operations-end-to-end" not in stable_gates:
        raise RuntimeError(f"{family_id}.release operations stable channel lost end-to-end validation")
    if "test-nightly" not in nightly_gates:
        raise RuntimeError(f"{family_id}.release operations nightly channel lost nightly validation")

    stable_rollback = require_object(
        stable_entry.get("rollback_safety"),
        f"{family_id}.release_operations.stable.rollback_safety",
    )
    nightly_rollback = require_object(
        nightly_entry.get("rollback_safety"),
        f"{family_id}.release_operations.nightly.rollback_safety",
    )
    if stable_rollback.get("rollback_channel") != "local-installer":
        raise RuntimeError(f"{family_id}.release operations stable rollback channel drifted")
    if nightly_rollback.get("rollback_channel") != "offline-bundle":
        raise RuntimeError(f"{family_id}.release operations nightly rollback channel drifted")

    for field in ("rollback_diagnostic_count", "fail_closed_diagnostic_count"):
        value = release_summary.get(field)
        if not isinstance(value, int) or value <= 0:
            raise RuntimeError(f"{family_id}.release operations summary {field} must be positive")

    stable_artifacts = require_object(
        release_summary.get("stable_artifacts"),
        f"{family_id}.release_operations.summary.stable_artifacts",
    )
    installer_signature = require_object(
        stable_artifacts.get("installer_signature"),
        f"{family_id}.release_operations.summary.stable_artifacts.installer_signature",
    )
    if installer_signature.get("artifact") != stable_artifacts.get("installer_archive"):
        raise RuntimeError(f"{family_id}.release operations installer signature artifact drifted")
    require_nonempty_string(
        installer_signature.get("sha256"),
        f"{family_id}.release_operations.summary.stable_artifacts.installer_signature.sha256",
    )

    release_evidence = require_object(
        channel_manifest.get("release_evidence"),
        f"{family_id}.release_operations.channel_manifest.release_evidence",
    )
    evidence_artifacts = require_string_list(
        release_evidence.get("evidence_artifacts"),
        f"{family_id}.release_operations.channel_manifest.release_evidence.evidence_artifacts",
    )
    for required_artifact in (
        repo_rel(RELEASE_UPDATE_MANIFEST_PATH),
        repo_rel(RELEASE_CHANNEL_MANIFEST_PATH),
    ):
        if required_artifact not in evidence_artifacts:
            raise RuntimeError(f"{family_id}.release operations evidence omitted {required_artifact}")
    release_source_truth_paths = require_string_list(
        release_summary.get("release_source_truth_paths"),
        f"{family_id}.release_operations.summary.release_source_truth_paths",
    )
    generated_source_truth = [
        path
        for path in release_source_truth_paths
        if path.startswith("tmp/") or path.startswith("artifacts/")
    ]
    if generated_source_truth:
        raise RuntimeError(
            f"{family_id}.release operations source truth used generated outputs: "
            + ", ".join(generated_source_truth)
        )

    return {
        "status": "PASS",
        "summary": repo_rel(RELEASE_OPERATIONS_SUMMARY_PATH),
        "update_manifest": repo_rel(RELEASE_UPDATE_MANIFEST_PATH),
        "release_channel_manifest": repo_rel(RELEASE_CHANNEL_MANIFEST_PATH),
        "channels": summary_channels,
        "stable_gate_actions": stable_gates,
        "nightly_gate_actions": nightly_gates,
        "stable_rollback_channel": stable_rollback["rollback_channel"],
        "nightly_rollback_channel": nightly_rollback["rollback_channel"],
        "rollback_diagnostic_count": release_summary["rollback_diagnostic_count"],
        "fail_closed_diagnostic_count": release_summary["fail_closed_diagnostic_count"],
        "release_source_truth_paths": release_source_truth_paths,
    }


def validate_tampered_package_rejection(
    family_id: str,
    artifact_dir: Path,
    *,
    contract: dict[str, Any],
    lock: dict[str, Any],
    mirror: dict[str, Any],
    registry: dict[str, Any],
    publication: dict[str, Any],
    restore_receipt: dict[str, Any],
    verification: dict[str, Any],
) -> dict[str, Any]:
    tampered_verification = deepcopy(verification)
    installed_records = require_list(
        tampered_verification.get("installed_packages"),
        f"{family_id}.tampered_package.installed_packages",
    )
    if not installed_records:
        raise RuntimeError(f"{family_id}.tampered package rejection has no installed packages")
    target_record = require_object(
        installed_records[0],
        f"{family_id}.tampered_package.installed_packages[0]",
    )
    package_id = require_nonempty_string(
        target_record.get("package_id"),
        f"{family_id}.tampered_package.package_id",
    )
    original_signature = require_nonempty_string(
        target_record.get("trust_signature"),
        f"{family_id}.tampered_package.original_trust_signature",
    )
    target_record["trust_signature"] = original_signature + ".tampered"

    observed_failures = collect_install_distribution_failures(
        root=ROOT,
        contract=contract,
        lock=lock,
        mirror=mirror,
        registry=registry,
        publication=publication,
        restore_receipt=restore_receipt,
        verification=tampered_verification,
    )
    expected_diagnostic = f"{PACKAGE_MANAGER_TAMPER_CODE}: installed trust signature drifted for {package_id}"
    if expected_diagnostic not in observed_failures:
        raise RuntimeError(
            f"{family_id}.tampered package rejection did not report expected diagnostic: "
            + expected_diagnostic
        )

    payload = {
        "contract_id": "objc3c.cross_lane_e2e.tampered_package_rejection.v1",
        "schema_version": 1,
        "issue": 8200,
        "family_id": family_id,
        "status": "PASS",
        "rejection_engine": "collect_install_distribution_failures",
        "source_verification": repo_rel(PACKAGE_INSTALL_VERIFICATION_PATH),
        "package_id": package_id,
        "tampered_field": "installed_packages[0].trust_signature",
        "expected_diagnostic": expected_diagnostic,
        "observed_diagnostics": observed_failures,
    }
    trace_path = artifact_dir / "tampered-package-rejection.json"
    write_json_file(trace_path, payload)
    return {
        "status": "PASS",
        "trace": repo_rel(trace_path),
        "package_id": package_id,
        "expected_diagnostic": expected_diagnostic,
        "observed_diagnostic_count": len(observed_failures),
    }


def _require_owned_tmp_output(path: Path, allowed_roots: tuple[Path, ...]) -> None:
    resolved = path.resolve()
    resolved_allowed = tuple(root.resolve() for root in allowed_roots)
    if not any(resolved == root or resolved.is_relative_to(root) for root in resolved_allowed):
        raise RuntimeError(f"refusing to remove non-cross-lane preflight output: {repo_rel(path)}")


def clean_release_operations_preflight_outputs() -> dict[str, Any]:
    output_roots = (
        RELEASE_OPERATIONS_ARTIFACT_ROOT,
        RELEASE_OPERATIONS_REPORT_ROOT,
        PACKAGE_CHANNELS_ARTIFACT_ROOT,
        PACKAGE_CHANNELS_REPORT_ROOT,
        RELEASE_FOUNDATION_ARTIFACT_ROOT,
        RELEASE_FOUNDATION_REPORT_ROOT,
        RELEASE_EVIDENCE_REPORT_ROOT,
        PLATFORM_HARDENING_ARTIFACT_ROOT,
        PLATFORM_HARDENING_REPORT_ROOT,
        RELEASE_OPERATIONS_PREFLIGHT_ARTIFACT_DIR,
    )
    allowed_roots = output_roots
    preexisting = {repo_rel(path): path.exists() for path in output_roots}
    removed: list[str] = []
    for path in output_roots:
        _require_owned_tmp_output(path, allowed_roots)
        if not path.exists():
            continue
        if path.is_dir():
            shutil.rmtree(path)
        else:
            path.unlink()
        removed.append(repo_rel(path))
    after_clean = {repo_rel(path): path.exists() for path in output_roots}
    return {
        "requested": True,
        "owned_roots": [repo_rel(path) for path in output_roots],
        "preexisting_owned_outputs": preexisting,
        "removed_owned_outputs": sorted(removed),
        "owned_outputs_exist_after_clean": after_clean,
        "generated_from_clean_owned_outputs": not any(after_clean.values()),
    }


def release_operations_artifact_digests() -> dict[str, str]:
    artifacts = (
        RELEASE_OPERATIONS_SUMMARY_PATH,
        RELEASE_UPDATE_MANIFEST_PATH,
        RELEASE_CHANNEL_MANIFEST_PATH,
    )
    return {repo_rel(path): sha256_file(path) for path in artifacts}


def require_release_operations_preflight(
    family_id: str,
    release_operations_preflight: dict[str, Any],
) -> dict[str, Any]:
    if release_operations_preflight.get("status") != "PASS":
        raise RuntimeError(f"{family_id}.release operations preflight must pass before distribution proof")
    if release_operations_preflight.get("consumer_family_id") != family_id:
        raise RuntimeError(f"{family_id}.release operations preflight consumer drifted")
    if release_operations_preflight.get("action") != "validate-release-operations":
        raise RuntimeError(f"{family_id}.release operations preflight action drifted")
    if release_operations_preflight.get("args") != ["--skip-upstream"]:
        raise RuntimeError(f"{family_id}.release operations preflight must skip upstream after deterministic generation")

    artifact_generation = require_object(
        release_operations_preflight.get("deterministic_artifact_generation"),
        f"{family_id}.release_operations_preflight.deterministic_artifact_generation",
    )
    if artifact_generation.get("action") != "build-package-channels":
        raise RuntimeError(f"{family_id}.release operations preflight generator action drifted")

    clean_start = require_object(
        release_operations_preflight.get("clean_start"),
        f"{family_id}.release_operations_preflight.clean_start",
    )
    if clean_start.get("generated_from_clean_owned_outputs") is not True:
        raise RuntimeError(f"{family_id}.release operations preflight did not start from clean owned outputs")

    prerequisite = require_object(
        release_operations_preflight.get("from_nothing_distribution_prerequisite"),
        f"{family_id}.release_operations_preflight.from_nothing_distribution_prerequisite",
    )
    if prerequisite.get("required_action") != "validate-package-install-distribution":
        raise RuntimeError(f"{family_id}.release operations preflight prerequisite action drifted")
    if prerequisite.get("required_flag") != "--from-nothing":
        raise RuntimeError(f"{family_id}.release operations preflight prerequisite flag drifted")
    if (
        prerequisite.get("required_summary")
        != "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
    ):
        raise RuntimeError(f"{family_id}.release operations preflight prerequisite summary drifted")

    preflight_digests = require_object(
        release_operations_preflight.get("generated_artifact_sha256"),
        f"{family_id}.release_operations_preflight.generated_artifact_sha256",
    )
    for path in (
        RELEASE_OPERATIONS_SUMMARY_PATH,
        RELEASE_UPDATE_MANIFEST_PATH,
        RELEASE_CHANNEL_MANIFEST_PATH,
    ):
        rel = repo_rel(path)
        expected = require_nonempty_string(
            preflight_digests.get(rel),
            f"{family_id}.release_operations_preflight.generated_artifact_sha256.{rel}",
        )
        actual = sha256_file(path)
        if actual != expected:
            raise RuntimeError(f"{family_id}.release operations artifact changed after preflight: {rel}")

    return require_object(
        release_operations_preflight.get("release_model"),
        f"{family_id}.release_operations_preflight.release_model",
    )


def prepare_release_operations_preflight(families: list[Any]) -> dict[str, Any]:
    for index, raw_row in enumerate(families):
        family = manifest_family(raw_row, f"families[{index}]")
        family_id = require_nonempty_string(
            family.get("family_id"),
            f"families[{index}].family_id",
        )
        if family_id != "distribution_package_lifecycle":
            continue
        expectation_path = require_source_owned_path(
            str(family.get("expectation", "")),
            f"{family_id}.expectation",
        )
        expectation = load_json(expectation_path)
        lifecycle = expectation.get("distribution_lifecycle")
        if not isinstance(lifecycle, dict) or lifecycle.get("status") != "expected-pass":
            continue

        release_model = validate_distribution_release_operations_model(family_id)
        clean_start = clean_release_operations_preflight_outputs()
        run_public_workflow_action(
            "build-package-channels",
            log_path=RELEASE_OPERATIONS_PREFLIGHT_ARTIFACT_DIR / "deterministic-release-artifact-generation.log",
            domain=f"{family_id}.deterministic release artifact generation",
        )
        run_public_workflow_action(
            "validate-release-operations",
            args=["--skip-upstream"],
            log_path=RELEASE_OPERATIONS_PREFLIGHT_ARTIFACT_DIR / "release-operations-preflight.log",
            domain=f"{family_id}.release operations preflight",
        )
        require_artifact(RELEASE_OPERATIONS_SUMMARY_PATH, "release operations summary")
        require_artifact(RELEASE_UPDATE_MANIFEST_PATH, "release operations update manifest")
        require_artifact(RELEASE_CHANNEL_MANIFEST_PATH, "release operations channel manifest")
        generated_artifacts = {
            "summary": repo_rel(RELEASE_OPERATIONS_SUMMARY_PATH),
            "update_manifest": repo_rel(RELEASE_UPDATE_MANIFEST_PATH),
            "release_channel_manifest": repo_rel(RELEASE_CHANNEL_MANIFEST_PATH),
        }
        return {
            "status": "PASS",
            "consumer_family_id": family_id,
            "action": "validate-release-operations",
            "args": ["--skip-upstream"],
            "command": "npm run objc3c -- validate-release-operations --skip-upstream",
            "deterministic_artifact_generation": {
                "action": "build-package-channels",
                "command": "npm run objc3c -- build-package-channels",
                "log": repo_rel(
                    RELEASE_OPERATIONS_PREFLIGHT_ARTIFACT_DIR
                    / "deterministic-release-artifact-generation.log"
                ),
            },
            "log": repo_rel(RELEASE_OPERATIONS_PREFLIGHT_ARTIFACT_DIR / "release-operations-preflight.log"),
            "clean_start": clean_start,
            "generated_from_clean_owned_outputs": clean_start["generated_from_clean_owned_outputs"],
            "generated_artifacts": generated_artifacts,
            "generated_artifact_sha256": release_operations_artifact_digests(),
            "release_model": release_model,
            "from_nothing_distribution_prerequisite": {
                "required_action": "validate-package-install-distribution",
                "required_flag": "--from-nothing",
                "required_summary": "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json",
                "enforced_for_channels": release_model["clean_install_prerequisite_channels"],
            },
        }

    return {
        "status": "SKIPPED",
        "reason": "no expected-pass distribution lifecycle family requires release-operation artifacts",
    }


def validate_distribution_package_lifecycle_proof(
    family_id: str,
    expectation: dict[str, Any],
    executable_proof: dict[str, Any],
    release_operations_preflight: dict[str, Any],
) -> dict[str, Any]:
    lifecycle = require_object(expectation.get("distribution_lifecycle"), f"{family_id}.distribution_lifecycle")
    expected_path = require_nonempty_string(
        lifecycle.get("expected_path"),
        f"{family_id}.distribution_lifecycle.expected_path",
    )
    trace_path = ROOT / normalize_path(expected_path)
    artifact_dir = ARTIFACT_ROOT / slug_from_family_id(family_id)

    run_public_workflow_action(
        "validate-package-install-distribution",
        args=["--from-nothing"],
        log_path=artifact_dir / "package-install-distribution.log",
        domain=f"{family_id}.package trust diagnostic",
    )
    release_model = require_release_operations_preflight(family_id, release_operations_preflight)

    package_summary = _require_json_artifact(PACKAGE_INSTALL_SUMMARY_PATH, "package install summary")
    verification = _require_json_artifact(PACKAGE_INSTALL_VERIFICATION_PATH, "package install verification")
    install_proof = _require_json_artifact(PACKAGE_INSTALL_PROOF_PATH, "package install proof manifest")
    lock = _require_json_artifact(PACKAGE_LOCK_PATH, "package lock")
    update_receipt = _require_json_artifact(PACKAGE_UPDATE_RECEIPT_PATH, "package update receipt")
    uninstall_receipt = _require_json_artifact(PACKAGE_UNINSTALL_RECEIPT_PATH, "package uninstall receipt")
    package_contract = _require_json_artifact(PACKAGE_INSTALL_CONTRACT_PATH, "package install contract")
    mirror = _require_json_artifact(PACKAGE_INSTALL_MIRROR_PATH, "package install mirror")
    registry = _require_json_artifact(PACKAGE_INSTALL_REGISTRY_PATH, "package install registry")
    publication = _require_json_artifact(PACKAGE_INSTALL_PUBLICATION_PATH, "package install publication metadata")
    restore_receipt = _require_json_artifact(PACKAGE_INSTALL_RESTORE_RECEIPT_PATH, "package install restore receipt")
    release_summary = _require_json_artifact(RELEASE_OPERATIONS_SUMMARY_PATH, "release operations summary")
    release_update_manifest = _require_json_artifact(RELEASE_UPDATE_MANIFEST_PATH, "release operations update manifest")
    release_channel_manifest = _require_json_artifact(RELEASE_CHANNEL_MANIFEST_PATH, "release operations channel manifest")

    if package_summary.get("status") != "PASS":
        raise RuntimeError(f"{family_id}.package trust diagnostic did not pass")
    from_nothing = require_object(package_summary.get("from_nothing_probe"), f"{family_id}.from_nothing_probe")
    if from_nothing.get("requested") is not True or from_nothing.get("generated_from_clean_owned_outputs") is not True:
        raise RuntimeError(f"{family_id}.package trust diagnostic must start from clean owned package outputs")
    if verification.get("network_policy") != "no-network-during-validation":
        raise RuntimeError(f"{family_id}.package trust diagnostic network policy drifted")
    if verification.get("hosted_registry_support") != "unsupported-fail-closed-if-claimed":
        raise RuntimeError(f"{family_id}.package trust diagnostic hosted-registry boundary drifted")
    if verification.get("manifest_count") != package_summary.get("installed_package_count"):
        raise RuntimeError(f"{family_id}.package trust diagnostic installed package count drifted")

    install_order = [str(value) for value in require_list(verification.get("install_order"), f"{family_id}.install_order")]
    if not install_order:
        raise RuntimeError(f"{family_id}.package trust diagnostic has no install order")
    _require_receipt_operation(update_receipt, "update", install_order)
    _require_receipt_operation(uninstall_receipt, "uninstall", list(reversed(install_order)))

    trust_signatures = _lock_package_trust_signatures(lock)
    installed_records = require_list(verification.get("installed_packages"), f"{family_id}.installed_packages")
    first_installed: dict[str, Any] | None = None
    for index, raw_record in enumerate(installed_records):
        record = require_object(raw_record, f"{family_id}.installed_packages[{index}]")
        package_id = require_nonempty_string(record.get("package_id"), f"{family_id}.installed_packages[{index}].package_id")
        signature = require_nonempty_string(
            record.get("trust_signature"),
            f"{family_id}.installed_packages[{index}].trust_signature",
        )
        if trust_signatures.get(package_id) != signature:
            raise RuntimeError(f"{family_id}.package signing proof drifted for {package_id}")
        require_nonempty_string(
            record.get("local_install_artifact_digest"),
            f"{family_id}.installed_packages[{index}].local_install_artifact_digest",
        )
        first_installed = first_installed or record

    if first_installed is None:
        raise RuntimeError(f"{family_id}.package trust diagnostic installed no packages")
    release_lifecycle_proof = validate_distribution_release_operations_lifecycle_proof(
        family_id,
        release_model,
        release_summary,
        release_update_manifest,
        release_channel_manifest,
    )
    tampered_rejection = validate_tampered_package_rejection(
        family_id,
        artifact_dir,
        contract=package_contract,
        lock=lock,
        mirror=mirror,
        registry=registry,
        publication=publication,
        restore_receipt=restore_receipt,
        verification=verification,
    )

    payload = {
        "contract_id": "objc3c.cross_lane_e2e.distribution_lifecycle.v1",
        "schema_version": 1,
        "issue": 8200,
        "family_id": family_id,
        "status": "PASS",
        "source_truth": False,
        "source": expectation["source"],
        "compile_run": {
            "status": executable_proof["status"],
            "expected_exit_code": executable_proof["expected_exit_code"],
            "actual_exit_code": executable_proof["actual_exit_code"],
            "executable": executable_proof["executable"],
            "compile_dir": executable_proof["compile_dir"],
            "package_module_proofs": executable_proof["package_module_proofs"],
        },
        "package_install": {
            "summary": repo_rel(PACKAGE_INSTALL_SUMMARY_PATH),
            "verification": repo_rel(PACKAGE_INSTALL_VERIFICATION_PATH),
            "install_proof_manifest": repo_rel(PACKAGE_INSTALL_PROOF_PATH),
            "package_count": package_summary["package_count"],
            "installed_package_count": package_summary["installed_package_count"],
            "from_nothing_clean_start": from_nothing,
            "install_receipt": package_summary["install_receipt"],
            "update_receipt": package_summary["update_receipt"],
            "uninstall_receipt": package_summary["uninstall_receipt"],
            "network_policy": verification["network_policy"],
            "hosted_registry_support": verification["hosted_registry_support"],
        },
        "release_operations": {
            "model": release_model["model"],
            "preflight": {
                "action": release_operations_preflight["action"],
                "args": release_operations_preflight["args"],
                "command": release_operations_preflight["command"],
                "deterministic_artifact_generation": release_operations_preflight[
                    "deterministic_artifact_generation"
                ],
                "log": release_operations_preflight["log"],
                "clean_start": release_operations_preflight["clean_start"],
                "generated_artifacts": release_operations_preflight["generated_artifacts"],
                "generated_artifact_sha256": release_operations_preflight["generated_artifact_sha256"],
                "from_nothing_distribution_prerequisite": release_operations_preflight[
                    "from_nothing_distribution_prerequisite"
                ],
            },
            "end_to_end_summary": repo_rel(RELEASE_OPERATIONS_SUMMARY_PATH),
            "update_manifest": repo_rel(RELEASE_UPDATE_MANIFEST_PATH),
            "release_channel_manifest": repo_rel(RELEASE_CHANNEL_MANIFEST_PATH),
            "lifecycle_proof": release_lifecycle_proof,
            "clean_install_prerequisite_channels": release_model["clean_install_prerequisite_channels"],
            "rollback_channels": release_model["rollback_channels"],
            "release_gate_actions": release_model["release_gate_actions"],
            "fail_closed_rule_count": release_model["fail_closed_rule_count"],
        },
        "negative_cases": {
            "tampered_package_signature": {
                **tampered_rejection,
            }
        },
        "reserved_or_external_rows_not_promoted": [
            "hosted package registry",
            "network dependency resolution",
            "public production release publication",
            "background update service",
        ],
    }
    write_json_file(trace_path, payload)
    return {
        "status": "PASS",
        "trace": repo_rel(trace_path),
        "package_install_summary": repo_rel(PACKAGE_INSTALL_SUMMARY_PATH),
        "release_operations_model": release_model["model"],
        "release_operations_preflight_log": release_operations_preflight["log"],
        "from_nothing_distribution_prerequisite": release_operations_preflight[
            "from_nothing_distribution_prerequisite"
        ],
        "package_from_nothing_probe": from_nothing,
        "package_count": package_summary["package_count"],
        "installed_package_count": package_summary["installed_package_count"],
        "rollback_channels": release_model["rollback_channels"],
        "release_operations_summary": repo_rel(RELEASE_OPERATIONS_SUMMARY_PATH),
        "tampered_signature_negative_case": tampered_rejection["expected_diagnostic"],
        "tampered_package_rejection_probe": tampered_rejection["trace"],
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
    if evidence.get("umbrella_support_promoted") is not True:
        raise RuntimeError(f"{family_id}.workspace must promote advanced-runtime umbrella support")
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
    if compile_manifest.get("status") == "expected-pass":
        expected_compile_manifest = normalize_path(
            require_nonempty_string(
                compile_manifest.get("expected_path"),
                f"{family_id}.compile_manifest.expected_path",
            )
        )
        if expected_compile_manifest != "tmp/artifacts/cross-lane-e2e/advanced-runtime-closure/compile/module.manifest.json":
            raise RuntimeError(f"{family_id}.compile_manifest.expected_path drifted")
    else:
        require_blocked_section(compile_manifest, f"{family_id}.compile_manifest")
    require_blocked_section(optimization_trace, f"{family_id}.optimization_trace")

    if runtime.get("proof_kind") != "advanced-runtime-combined-runtime-state":
        raise RuntimeError(f"{family_id}.runtime.proof_kind drifted")
    if source_graph.get("proof_kind") != "advanced-runtime-compiler-owned-source-graph":
        raise RuntimeError(f"{family_id}.source_graph.proof_kind drifted")
    if debug_source_map.get("proof_kind") != "advanced-runtime-canonical-source-debug-map":
        raise RuntimeError(f"{family_id}.debug_source_map.proof_kind drifted")
    if runtime.get("umbrella_support_promoted") is not True:
        raise RuntimeError(f"{family_id}.runtime must promote advanced-runtime umbrella support")
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
        "umbrella_support_promoted": True,
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
        require_failure_domain(blocker.get("domain"), f"{field}[{index}].domain")
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


def validate_text_package_native_meta_boundary(family_id: str, meta: dict[str, Any]) -> None:
    if family_id != "text_collections_package":
        return
    if meta.get("fixture_kind") != "executable-package-runtime-proof-with-reserved-debugger-rows":
        raise RuntimeError(f"{family_id}.native_meta.fixture_kind drifted from executable package runtime boundary")

    boundary = require_object(meta.get("boundary"), f"{family_id}.native_meta.boundary")
    required_true = (
        "canonical_behavior_source",
        "runtime_support_claimed",
        "source_collection_literals_claimed",
        "source_for_in_claimed",
        "package_import_metadata_claimed",
        "source_graph_package_nodes_claimed",
        "debug_map_declaration_anchors_claimed",
        "missing_provider_link_rejection_claimed",
        "package_text_identity_claimed",
        "direct_module_import_source_truth_claimed",
        "runtime_package_execution_claimed",
        "direct_module_import_syntax_promoted",
    )
    for key in required_true:
        if boundary.get(key) is not True:
            raise RuntimeError(f"{family_id}.native_meta.boundary.{key} must be true")

    required_false = (
        "reserved_support_promoted",
        "text_fixture_uses_direct_module_import_syntax",
        "runtime_package_execution_reserved",
        "semantic_reference_closure_promoted",
        "full_source_map_publication_promoted",
        "statement_stepping_promoted",
    )
    for key in required_false:
        if boundary.get(key) is not False:
            raise RuntimeError(f"{family_id}.native_meta.boundary.{key} must be false")

    execution = require_object(meta.get("execution"), f"{family_id}.native_meta.execution")
    if execution.get("expected_exit_code") != 0:
        raise RuntimeError(f"{family_id}.native_meta.execution.expected_exit_code must be 0")
    if execution.get("requires_live_runtime_dispatch") is not True:
        raise RuntimeError(f"{family_id}.native_meta.execution.requires_live_runtime_dispatch must stay true")
    if execution.get("requires_provider_runtime_import_surface") is not True:
        raise RuntimeError(
            f"{family_id}.native_meta.execution.requires_provider_runtime_import_surface must stay true"
        )


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
    validate_text_package_native_meta_boundary(family_id, meta)
    return meta


def validate_section_status(section: dict[str, Any], field: str, expectation_state: str) -> None:
    status = require_nonempty_string(section.get("status"), f"{field}.status")
    if status not in ALLOWED_SECTION_STATES:
        raise RuntimeError(f"{field}.status is unknown: {status}")
    if "responsible_domain" in section:
        require_failure_domain(section.get("responsible_domain"), f"{field}.responsible_domain")
    if status in BLOCKED_STATES:
        require_nonempty_string(section.get("blocker_id"), f"{field}.blocker_id")
        require_failure_domain(section.get("responsible_domain"), f"{field}.responsible_domain")
        require_nonempty_string(section.get("reason"), f"{field}.reason")
    if expectation_state in BLOCKED_STATES and status == "expected-pass":
        require_failure_domain(section.get("responsible_domain"), f"{field}.responsible_domain")
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
        require_failure_domain(domain, f"{family_id}.responsible_domains[{index}]")
        for index, domain in enumerate(
            require_list(
                expectation.get("responsible_domains"),
                f"{family_id}.responsible_domains",
            )
        )
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
    expected_no_reserved_promotion = family_id != "object_reflection_debugger"
    if boundary.get("cannot_promote_reserved_rows") is not expected_no_reserved_promotion:
        raise RuntimeError(f"{family_id}.capability_boundary must forbid reserved-row promotion")
    require_list(boundary.get("referenced_capability_rows"), f"{family_id}.capability_boundary.rows")
    return expectation


def validate_source_tokens(family_id: str, source_path: Path, tokens: list[Any]) -> None:
    source_text = source_path.read_text(encoding="utf-8")
    for token in tokens:
        token_text = require_nonempty_string(token, f"{family_id}.required_source_tokens[]")
        if token_text not in source_text:
            raise RuntimeError(f"{family_id} source is missing required token: {token_text}")


def validate_family(
    family: dict[str, Any],
    *,
    release_operations_preflight: dict[str, Any],
) -> dict[str, Any]:
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
    text_collections_package_proof: dict[str, Any] | None = None
    object_reflection_debugger_proof: dict[str, Any] | None = None
    advanced_runtime_contract_proof: dict[str, Any] | None = None
    optimization_trace_proof: dict[str, Any] | None = None
    distribution_lifecycle_proof: dict[str, Any] | None = None
    if require_object(expectation["runtime"], f"{family_id}.runtime").get("status") == "expected-pass":
        if family_id == ADVANCED_RUNTIME_FAMILY_ID:
            advanced_runtime_contract_proof = validate_advanced_runtime_contract_backed_proof(
                family_id,
                expectation,
                workspace,
            )
        executable_proof = validate_executable_runtime_proof(family_id, source_path, expectation, workspace)
    object_reflection_debugger_proof = validate_object_reflection_debugger_proof(
        family_id,
        source_path,
        expectation,
        executable_proof,
    )
    text_collections_package_proof = validate_text_collections_package_proof(
        family_id,
        source_path,
        expectation,
        workspace,
        executable_proof,
    )
    if require_object(expectation["optimization_trace"], f"{family_id}.optimization_trace").get("status") == "expected-pass":
        if executable_proof is None:
            raise RuntimeError(f"{family_id}.optimization proof diagnostic requires executable runtime proof")
        optimization_trace_proof = validate_optimization_trace_proof(family_id, expectation, executable_proof)
    distribution_lifecycle = expectation.get("distribution_lifecycle")
    if isinstance(distribution_lifecycle, dict) and distribution_lifecycle.get("status") == "expected-pass":
        if executable_proof is None:
            raise RuntimeError(f"{family_id}.distribution lifecycle proof requires executable runtime proof")
        distribution_lifecycle_proof = validate_distribution_package_lifecycle_proof(
            family_id,
            expectation,
            executable_proof,
            release_operations_preflight,
        )

    capability_rows = require_list(family.get("capability_rows"), f"{family_id}.capability_rows")
    support_claims = require_list(family.get("support_claims"), f"{family_id}.support_claims")
    evidence_refs = require_list(family.get("evidence_map_refs"), f"{family_id}.evidence_map_refs")
    if not capability_rows or not support_claims or not evidence_refs:
        raise RuntimeError(f"{family_id} must reference capability rows, support claims, and evidence-map entries")
    declared_support_claims = {
        require_nonempty_string(claim, f"{family_id}.support_claims[]")
        for claim in support_claims
    }

    for index, raw_ref in enumerate(evidence_refs):
        ref = require_object(raw_ref, f"{family_id}.evidence_map_refs[{index}]")
        require_nonempty_string(ref.get("capability_id"), f"{family_id}.evidence_map_refs[{index}].capability_id")
        status = require_nonempty_string(ref.get("status"), f"{family_id}.evidence_map_refs[{index}].status")
        if status not in ("implemented", "blocked", "reserved"):
            raise RuntimeError(f"{family_id}.evidence_map_refs[{index}].status is unknown: {status}")
        support_claim = str(ref.get("support_claim", ""))
        if status == "implemented":
            support_claim = require_nonempty_string(
                support_claim,
                f"{family_id}.evidence_map_refs[{index}].support_claim",
            )
            if support_claim not in declared_support_claims:
                raise RuntimeError(
                    f"{family_id}.evidence_map_refs[{index}].support_claim "
                    "must be listed in family.support_claims"
                )
        elif support_claim:
            raise RuntimeError(
                f"{family_id}.evidence_map_refs[{index}].support_claim "
                "must be empty unless the referenced capability is implemented"
            )
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
        "text_collections_package_proof": text_collections_package_proof,
        "object_reflection_debugger_proof": object_reflection_debugger_proof,
        "advanced_runtime_contract_proof": advanced_runtime_contract_proof,
        "optimization_trace_proof": optimization_trace_proof,
        "distribution_lifecycle_proof": distribution_lifecycle_proof,
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


def validate_required_public_commands(manifest: dict[str, Any]) -> dict[str, Any]:
    commands = tuple(require_string_list(manifest.get("required_public_commands"), "required_public_commands"))
    if commands != REQUIRED_PUBLIC_COMMANDS:
        raise RuntimeError(
            "cross-lane E2E required_public_commands must stay in #8200 order: "
            + ", ".join(REQUIRED_PUBLIC_COMMANDS)
        )
    return {
        "status": "wired",
        "commands": list(commands),
    }


def collect_public_suite_references(public_suite_manifest: dict[str, Any]) -> tuple[set[str], set[str]]:
    references: set[str] = set()
    public_commands: set[str] = set()
    for case_index, raw_case in enumerate(require_list(public_suite_manifest.get("suite_cases"), "public_suite.suite_cases")):
        case = require_object(raw_case, f"public_suite.suite_cases[{case_index}]")
        command = case.get("runnable_command")
        if isinstance(command, str) and command:
            public_commands.add(command)
        for field_name in ("source_manifest", "conformance_fixture", "traceability_fixture"):
            value = case.get(field_name)
            if isinstance(value, str) and value:
                references.add(normalize_path(value))
        for field_name in ("positive_evidence", "negative_evidence"):
            values = case.get(field_name)
            if isinstance(values, list):
                references.update(normalize_path(value) for value in values if isinstance(value, str) and value)
    return references, public_commands


def validate_public_conformance_suite_linkage(family_summaries: list[dict[str, Any]]) -> dict[str, Any]:
    public_suite_manifest = load_json(PUBLIC_CONFORMANCE_SUITE_MANIFEST_PATH)
    references, public_commands = collect_public_suite_references(public_suite_manifest)
    if PUBLIC_COMMAND not in public_commands:
        raise RuntimeError("public conformance suite must expose the cross-lane E2E replay command")
    if repo_rel(MANIFEST_PATH) not in references:
        raise RuntimeError("public conformance suite must reference the #8200 cross-lane manifest")

    missing_families: list[str] = []
    linked_families: list[str] = []
    for family in family_summaries:
        source = normalize_path(str(family["source"]))
        expectation = normalize_path(str(family["expectation"]))
        if source in references or expectation in references:
            linked_families.append(str(family["family_id"]))
        else:
            missing_families.append(str(family["family_id"]))
    if missing_families:
        raise RuntimeError(
            "public conformance suite missing cross-lane family linkage: "
            + ", ".join(missing_families)
        )

    return {
        "status": "linked",
        "manifest": repo_rel(PUBLIC_CONFORMANCE_SUITE_MANIFEST_PATH),
        "public_command": PUBLIC_CONFORMANCE_SUITE_COMMAND,
        "linked_family_ids": linked_families,
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
    required_public_commands = validate_required_public_commands(manifest)

    policy = require_object(manifest.get("claim_policy"), "claim_policy")
    if policy.get("integrated_success_promotes_reserved_rows") is not False:
        raise RuntimeError("claim_policy must not promote reserved rows")
    if policy.get("tmp_source_truth_allowed") is not False:
        raise RuntimeError("claim_policy must reject tmp source truth")

    families = require_list(manifest.get("families"), "families")
    family_rows = [manifest_family(row, f"families[{index}]") for index, row in enumerate(families)]
    family_ids = [
        require_nonempty_string(family.get("family_id"), f"families[{index}].family_id")
        for index, family in enumerate(family_rows)
    ]
    if tuple(family_ids) != REQUIRED_FAMILY_IDS:
        raise RuntimeError(
            "cross-lane E2E families must stay in required #8200 order: "
            + ", ".join(REQUIRED_FAMILY_IDS)
        )

    release_operations_preflight = prepare_release_operations_preflight(family_rows)
    family_summaries = [
        validate_family(
            family,
            release_operations_preflight=release_operations_preflight,
        )
        for family in family_rows
    ]
    workflow_action_glue = validate_workflow_action_glue()
    public_conformance_suite_linkage = validate_public_conformance_suite_linkage(family_summaries)
    blocked_count = sum(1 for row in family_summaries if row["expected_state"] in BLOCKED_STATES)
    reserved_ref_count = sum(
        1
        for family in family_rows
        for raw_ref in require_list(family.get("evidence_map_refs"), f"{family['family_id']}.evidence_map_refs")
        if require_object(raw_ref, "evidence_map_ref").get("status") == "reserved"
    )

    return {
        "manifest_path": repo_rel(MANIFEST_PATH),
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_manifest_contract": MANIFEST_CONTRACT_ID,
        "issue": 8200,
        "public_action": PUBLIC_ACTION,
        "public_replay_command": PUBLIC_COMMAND,
        "required_public_commands": required_public_commands,
        "workflow_action_glue": workflow_action_glue,
        "public_conformance_suite_linkage": public_conformance_suite_linkage,
        "release_operations_preflight": release_operations_preflight,
        "family_count": len(family_summaries),
        "blocked_or_reserved_family_count": blocked_count,
        "reserved_evidence_ref_count": reserved_ref_count,
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
