#!/usr/bin/env python3
"""Unified public workflow runner for the live objc3c command surface."""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import sys
from dataclasses import asdict, dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Callable, Sequence

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
NPX = shutil.which("npx.cmd") or shutil.which("npx") or "npx"

BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_PS1 = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
RECOVERY_PS1 = ROOT / "scripts" / "check_objc3c_native_recovery_contract.ps1"
MATRIX_PS1 = ROOT / "scripts" / "run_objc3c_native_fixture_matrix.ps1"
NEGATIVE_EXPECTATIONS_PS1 = ROOT / "scripts" / "check_objc3c_negative_fixture_expectations.ps1"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PROOF_PS1 = ROOT / "scripts" / "run_objc3c_native_compile_proof.ps1"
SITE_PY = ROOT / "scripts" / "build_site_index.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"
RUNTIME_ARCHITECTURE_PROOF_PACKET_PY = ROOT / "scripts" / "check_objc3c_runtime_architecture_proof_packet.py"
RUNTIME_ARCHITECTURE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
RUNNABLE_BOOTSTRAP_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_bootstrap_end_to_end.py"
RUNNABLE_BLOCK_ARC_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_block_arc_conformance.py"
RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_object_model_conformance.py"
RUNNABLE_OBJECT_MODEL_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_object_model_end_to_end.py"
RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_conformance.py"
RUNNABLE_STORAGE_REFLECTION_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_end_to_end.py"
PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"


@dataclass(frozen=True)
class ActionSpec:
    action: str
    summary: str
    backend: str
    public_scripts: tuple[str, ...]
    validation_tier: str = ""
    guarantee_owner: str = ""
    pass_through_args: bool = False


ActionHandler = Callable[[list[str]], int]


def run(command: Sequence[str]) -> int:
    return subprocess.run(list(command), cwd=ROOT, check=False).returncode


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


def pwsh_file(script: Path, *args: str) -> int:
    return run([PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(script), *args])


def run_steps(actions: Sequence[str]) -> int:
    for action in actions:
        rc = execute_registered_action(action, [])
        if rc != 0:
            return rc
    return 0


def action_build_default(_: list[str]) -> int:
    return run_steps(["build-native-binaries"])


def action_build_native_binaries(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only")


def action_build_native_contracts(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "packets-binary")


def action_build_native_full(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "full")


def action_build_native_reconfigure(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only", "-ForceReconfigure")


def action_build_spec(_: list[str]) -> int:
    rc = run([sys.executable, str(SITE_PY)])
    if rc != 0:
        return rc
    return run([NPX, "prettier", "--write", "site/index.md"])


def action_compile_objc3c(rest: list[str]) -> int:
    return pwsh_file(COMPILE_PS1, *rest)


def action_lint_spec(_: list[str]) -> int:
    return run([sys.executable, str(SPEC_LINT_PY)])


def action_test_default(_: list[str]) -> int:
    return run_steps(["test-fast"])


def to_repo_relative(raw_path: str) -> str:
    candidate = Path(raw_path.strip())
    try:
        if candidate.is_absolute():
            return str(candidate.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return raw_path.strip()
    return raw_path.strip().replace("\\", "/")


def extract_report_paths(stdout: str) -> list[str]:
    report_paths: list[str] = []
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith("summary_path:"):
            report_paths.append(to_repo_relative(line.split(":", 1)[1]))
            continue
        match = re.search(r"runtime-acceptance:\s+PASS\s+\((.+)\)", line)
        if match:
            report_paths.append(to_repo_relative(match.group(1)))
            continue
        if line.startswith("out_dir:"):
            report_paths.append(to_repo_relative(line.split(":", 1)[1]))
    return report_paths


def load_surface_from_report(
    steps: Sequence[dict[str, object]], surface_key: str
) -> dict[str, object] | None:
    for step in steps:
        report_paths = step.get("report_paths", [])
        if not isinstance(report_paths, list):
            continue
        for raw_path in report_paths:
            if not isinstance(raw_path, str):
                continue
            candidate = ROOT / raw_path
            if not candidate.is_file():
                continue
            try:
                payload = json.loads(candidate.read_text(encoding="utf-8"))
            except json.JSONDecodeError:
                continue
            surface = payload.get(surface_key)
            if isinstance(surface, dict):
                return surface
    return None


def write_composite_validation_report(
    action: str,
    steps: list[dict[str, object]],
    *,
    status: str,
) -> Path:
    PUBLIC_WORKFLOW_REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    report_path = PUBLIC_WORKFLOW_REPORT_ROOT / f"{action}.json"
    payload = {
        "action": action,
        "status": status,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": "scripts/objc3c_public_workflow_runner.py",
        "claim_boundary": {
            "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
            "reports_are_authoritative_only_when_child_steps_are_compile-coupled": True,
            "authoritative_child_surfaces": [
                "scripts/check_objc3c_runtime_acceptance.py",
                "scripts/check_objc3c_execution_replay_proof.ps1",
                "scripts/check_objc3c_native_execution_smoke.ps1",
            ],
            "non_authoritative_inputs": [
                "integrated report paths by themselves",
                "sidecar-only summaries with no matching emitted object/probe path",
                "synthetic or hand-authored llvm ir used without coupled compile output",
            ],
        },
        "steps": steps,
    }
    runtime_state_publication_surface = load_surface_from_report(
        steps, "runtime_state_publication_surface"
    )
    if runtime_state_publication_surface is not None:
        payload["runtime_state_publication_surface"] = runtime_state_publication_surface
    acceptance_suite_surface = load_surface_from_report(steps, "acceptance_suite_surface")
    if acceptance_suite_surface is not None:
        payload["acceptance_suite_surface"] = acceptance_suite_surface
    runtime_installation_abi_surface = load_surface_from_report(
        steps, "runtime_installation_abi_surface"
    )
    if runtime_installation_abi_surface is not None:
        payload["runtime_installation_abi_surface"] = runtime_installation_abi_surface
    runtime_loader_lifecycle_surface = load_surface_from_report(
        steps, "runtime_loader_lifecycle_surface"
    )
    if runtime_loader_lifecycle_surface is not None:
        payload["runtime_loader_lifecycle_surface"] = runtime_loader_lifecycle_surface
    runtime_object_model_realization_source_surface = load_surface_from_report(
        steps, "runtime_object_model_realization_source_surface"
    )
    if runtime_object_model_realization_source_surface is not None:
        payload["runtime_object_model_realization_source_surface"] = (
            runtime_object_model_realization_source_surface
        )
    runtime_block_arc_unified_source_surface = load_surface_from_report(
        steps, "runtime_block_arc_unified_source_surface"
    )
    if runtime_block_arc_unified_source_surface is not None:
        payload["runtime_block_arc_unified_source_surface"] = (
            runtime_block_arc_unified_source_surface
        )
    runtime_ownership_transfer_capture_family_source_surface = load_surface_from_report(
        steps, "runtime_ownership_transfer_capture_family_source_surface"
    )
    if runtime_ownership_transfer_capture_family_source_surface is not None:
        payload["runtime_ownership_transfer_capture_family_source_surface"] = (
            runtime_ownership_transfer_capture_family_source_surface
        )
    runtime_block_arc_lowering_helper_surface = load_surface_from_report(
        steps, "runtime_block_arc_lowering_helper_surface"
    )
    if runtime_block_arc_lowering_helper_surface is not None:
        payload["runtime_block_arc_lowering_helper_surface"] = (
            runtime_block_arc_lowering_helper_surface
        )
    runtime_block_arc_runtime_abi_surface = load_surface_from_report(
        steps, "runtime_block_arc_runtime_abi_surface"
    )
    if runtime_block_arc_runtime_abi_surface is not None:
        payload["runtime_block_arc_runtime_abi_surface"] = (
            runtime_block_arc_runtime_abi_surface
        )
    runtime_property_ivar_storage_accessor_source_surface = load_surface_from_report(
        steps, "runtime_property_ivar_storage_accessor_source_surface"
    )
    if runtime_property_ivar_storage_accessor_source_surface is not None:
        payload["runtime_property_ivar_storage_accessor_source_surface"] = (
            runtime_property_ivar_storage_accessor_source_surface
        )
    storage_accessor_runtime_abi_surface = load_surface_from_report(
        steps, "storage_accessor_runtime_abi_surface"
    )
    if storage_accessor_runtime_abi_surface is not None:
        payload["storage_accessor_runtime_abi_surface"] = (
            storage_accessor_runtime_abi_surface
        )
    runtime_property_ivar_accessor_reflection_implementation_surface = (
        load_surface_from_report(
            steps,
            "runtime_property_ivar_accessor_reflection_implementation_surface",
        )
    )
    if runtime_property_ivar_accessor_reflection_implementation_surface is not None:
        payload["runtime_property_ivar_accessor_reflection_implementation_surface"] = (
            runtime_property_ivar_accessor_reflection_implementation_surface
        )
    runtime_property_atomicity_synthesis_reflection_source_surface = load_surface_from_report(
        steps, "runtime_property_atomicity_synthesis_reflection_source_surface"
    )
    if runtime_property_atomicity_synthesis_reflection_source_surface is not None:
        payload["runtime_property_atomicity_synthesis_reflection_source_surface"] = (
            runtime_property_atomicity_synthesis_reflection_source_surface
        )
    runtime_realization_lowering_reflection_artifact_surface = load_surface_from_report(
        steps, "runtime_realization_lowering_reflection_artifact_surface"
    )
    if runtime_realization_lowering_reflection_artifact_surface is not None:
        payload["runtime_realization_lowering_reflection_artifact_surface"] = (
            runtime_realization_lowering_reflection_artifact_surface
        )
    runtime_dispatch_table_reflection_record_lowering_surface = load_surface_from_report(
        steps, "runtime_dispatch_table_reflection_record_lowering_surface"
    )
    if runtime_dispatch_table_reflection_record_lowering_surface is not None:
        payload["runtime_dispatch_table_reflection_record_lowering_surface"] = (
            runtime_dispatch_table_reflection_record_lowering_surface
        )
    runtime_cross_module_realized_metadata_replay_preservation_surface = load_surface_from_report(
        steps, "runtime_cross_module_realized_metadata_replay_preservation_surface"
    )
    if runtime_cross_module_realized_metadata_replay_preservation_surface is not None:
        payload["runtime_cross_module_realized_metadata_replay_preservation_surface"] = (
            runtime_cross_module_realized_metadata_replay_preservation_surface
        )
    runtime_object_model_abi_query_surface = load_surface_from_report(
        steps, "runtime_object_model_abi_query_surface"
    )
    if runtime_object_model_abi_query_surface is not None:
        payload["runtime_object_model_abi_query_surface"] = (
            runtime_object_model_abi_query_surface
        )
    runtime_realization_lookup_reflection_implementation_surface = load_surface_from_report(
        steps, "runtime_realization_lookup_reflection_implementation_surface"
    )
    if runtime_realization_lookup_reflection_implementation_surface is not None:
        payload["runtime_realization_lookup_reflection_implementation_surface"] = (
            runtime_realization_lookup_reflection_implementation_surface
        )
    runtime_reflection_query_surface = load_surface_from_report(
        steps, "runtime_reflection_query_surface"
    )
    if runtime_reflection_query_surface is not None:
        payload["runtime_reflection_query_surface"] = runtime_reflection_query_surface
    runtime_realization_lookup_semantics_surface = load_surface_from_report(
        steps, "runtime_realization_lookup_semantics_surface"
    )
    if runtime_realization_lookup_semantics_surface is not None:
        payload["runtime_realization_lookup_semantics_surface"] = (
            runtime_realization_lookup_semantics_surface
        )
    runtime_class_metaclass_protocol_realization_surface = load_surface_from_report(
        steps, "runtime_class_metaclass_protocol_realization_surface"
    )
    if runtime_class_metaclass_protocol_realization_surface is not None:
        payload["runtime_class_metaclass_protocol_realization_surface"] = (
            runtime_class_metaclass_protocol_realization_surface
        )
    runtime_category_attachment_merged_dispatch_surface = load_surface_from_report(
        steps, "runtime_category_attachment_merged_dispatch_surface"
    )
    if runtime_category_attachment_merged_dispatch_surface is not None:
        payload["runtime_category_attachment_merged_dispatch_surface"] = (
            runtime_category_attachment_merged_dispatch_surface
        )
    runtime_reflection_visibility_coherence_diagnostics_surface = load_surface_from_report(
        steps, "runtime_reflection_visibility_coherence_diagnostics_surface"
    )
    if runtime_reflection_visibility_coherence_diagnostics_surface is not None:
        payload["runtime_reflection_visibility_coherence_diagnostics_surface"] = (
            runtime_reflection_visibility_coherence_diagnostics_surface
        )
    report_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return report_path


def run_composite_step(action: str, command: Sequence[str]) -> dict[str, object]:
    result = run_capture(command)
    return {
        "action": action,
        "command": [str(token) for token in command],
        "exit_code": result.returncode,
        "report_paths": extract_report_paths(result.stdout),
    }


def run_composite_validation(action: str, steps: list[tuple[str, Sequence[str]]]) -> int:
    results: list[dict[str, object]] = []
    for step_action, command in steps:
        step = run_composite_step(step_action, command)
        results.append(step)
        if step["exit_code"] != 0:
            report_path = write_composite_validation_report(action, results, status="FAIL")
            print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
            return int(step["exit_code"])
    report_path = write_composite_validation_report(action, results, status="PASS")
    print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
    return 0


def action_test_fast(_: list[str]) -> int:
    return run_composite_validation(
        "test-fast",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1), "-Limit", "12"]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
        ],
    )


def action_test_smoke(_: list[str]) -> int:
    return run_steps(["test-execution-smoke"])


def action_test_ci(_: list[str]) -> int:
    return run_composite_validation(
        "test-ci",
        [
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
        ],
    )


def action_test_recovery(rest: list[str]) -> int:
    return pwsh_file(RECOVERY_PS1, *rest)


def action_test_execution_smoke(rest: list[str]) -> int:
    return pwsh_file(SMOKE_PS1, *rest)


def action_test_execution_replay(rest: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, *rest)


def action_test_runtime_acceptance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY)])


def action_proof_runtime_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ARCHITECTURE_PROOF_PACKET_PY)])


def action_validate_runtime_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ARCHITECTURE_INTEGRATION_PY)])


def action_validate_runnable_bootstrap(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BOOTSTRAP_E2E_PY)])


def action_validate_block_arc_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BLOCK_ARC_CONFORMANCE_PY)])


def action_validate_object_model_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY)])


def action_validate_runnable_object_model(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_OBJECT_MODEL_E2E_PY)])


def action_validate_storage_reflection_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY)])


def action_validate_runnable_storage_reflection(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STORAGE_REFLECTION_E2E_PY)])


def action_test_fixture_matrix(rest: list[str]) -> int:
    return pwsh_file(MATRIX_PS1, *rest)


def action_test_negative_expectations(rest: list[str]) -> int:
    return pwsh_file(NEGATIVE_EXPECTATIONS_PS1, *rest)


def action_test_full(_: list[str]) -> int:
    return run_composite_validation(
        "test-full",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
        ],
    )


def action_test_nightly(_: list[str]) -> int:
    return run_composite_validation(
        "test-nightly",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
            ("test-recovery", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(RECOVERY_PS1)]),
            ("test-fixture-matrix", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(MATRIX_PS1)]),
            ("test-negative-expectations", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(NEGATIVE_EXPECTATIONS_PS1)]),
        ],
    )


def action_package_runnable_toolchain(_: list[str]) -> int:
    return pwsh_file(PACKAGE_PS1)


def action_proof_objc3c(_: list[str]) -> int:
    return pwsh_file(PROOF_PS1)


ACTION_SPECS: dict[str, ActionSpec] = {
    "build-default": ActionSpec("build-default", "default public build entrypoint", "runner-internal", ("build",)),
    "build-native-binaries": ActionSpec("build-native-binaries", "build native binaries", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native",)),
    "build-native-contracts": ActionSpec("build-native-contracts", "build native contracts/binaries packet surface", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:contracts",)),
    "build-native-full": ActionSpec("build-native-full", "run full native build", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:full",)),
    "build-native-reconfigure": ActionSpec("build-native-reconfigure", "force native reconfigure build", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:reconfigure",)),
    "build-spec": ActionSpec("build-spec", "build site/spec output and format it", "python:scripts/build_site_index.py + npx prettier", ("build:spec",)),
    "compile-objc3c": ActionSpec("compile-objc3c", "compile one Objective-C 3 fixture through the native compiler", "pwsh:scripts/objc3c_native_compile.ps1", ("compile:objc3c",), pass_through_args=True),
    "lint-spec": ActionSpec("lint-spec", "run spec lint", "python:scripts/spec_lint.py", ("lint:spec",)),
    "test-default": ActionSpec("test-default", "default public test entrypoint", "runner-internal", ("test",)),
    "test-fast": ActionSpec("test-fast", "fast public validation entrypoint", "runner-internal + targeted smoke slice", ("test:fast",), validation_tier="fast", guarantee_owner="runtime acceptance, canonical replay, and a bounded smoke slice"),
    "test-smoke": ActionSpec("test-smoke", "developer smoke validation entrypoint", "runner-internal", ("test:smoke",), validation_tier="smoke", guarantee_owner="full execution smoke corpus"),
    "test-ci": ActionSpec("test-ci", "CI-oriented public validation entrypoint", "runner-internal + direct task hygiene", ("test:ci",), validation_tier="ci", guarantee_owner="task hygiene plus full developer validation"),
    "test-recovery": ActionSpec("test-recovery", "native recovery contract suite", "pwsh:scripts/check_objc3c_native_recovery_contract.ps1", ("test:objc3c",), validation_tier="recovery", guarantee_owner="recovery compile success and deterministic recovery diagnostics", pass_through_args=True),
    "test-execution-smoke": ActionSpec("test-execution-smoke", "native execution smoke suite", "pwsh:scripts/check_objc3c_native_execution_smoke.ps1", ("test:objc3c:execution-smoke",), validation_tier="smoke", guarantee_owner="compile/link/run execution behavior", pass_through_args=True),
    "test-execution-replay": ActionSpec("test-execution-replay", "native execution replay proof suite", "pwsh:scripts/check_objc3c_execution_replay_proof.ps1", ("test:objc3c:execution-replay-proof",), validation_tier="full", guarantee_owner="replay and native-output truth", pass_through_args=True),
    "test-runtime-acceptance": ActionSpec("test-runtime-acceptance", "runtime acceptance suite", "python:scripts/check_objc3c_runtime_acceptance.py", ("test:objc3c:runtime-acceptance",), validation_tier="fast", guarantee_owner="runtime acceptance and ABI/accessor proof"),
    "proof-runtime-architecture": ActionSpec("proof-runtime-architecture", "emit the integrated runtime architecture proof packet", "python:scripts/check_objc3c_runtime_architecture_proof_packet.py", ("proof:objc3c:runtime-architecture",)),
    "validate-runtime-architecture": ActionSpec("validate-runtime-architecture", "validate runtime architecture across the full public workflow and proof packet", "python:scripts/check_objc3c_runtime_architecture_integration.py", ("test:objc3c:runtime-architecture",), validation_tier="full", guarantee_owner="full public workflow and runtime architecture proof packet alignment"),
    "validate-runnable-bootstrap": ActionSpec("validate-runnable-bootstrap", "validate the staged runnable toolchain end to end from the package root", "python:scripts/check_objc3c_runnable_bootstrap_end_to_end.py", ("test:objc3c:runnable-bootstrap",), validation_tier="full", guarantee_owner="packaged compile, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-block-arc-conformance": ActionSpec("validate-block-arc-conformance", "validate runnable block/ARC conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_block_arc_conformance.py", ("test:objc3c:block-arc-conformance",), validation_tier="full", guarantee_owner="integrated block/ARC conformance over the live runtime architecture workflow"),
    "validate-object-model-conformance": ActionSpec("validate-object-model-conformance", "validate runnable object-model conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_object_model_conformance.py", ("test:objc3c:object-model-conformance",), validation_tier="full", guarantee_owner="integrated object-model conformance over the live runtime architecture workflow"),
    "validate-storage-reflection-conformance": ActionSpec("validate-storage-reflection-conformance", "validate runnable storage/reflection conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_storage_reflection_conformance.py", ("test:objc3c:storage-reflection-conformance",), validation_tier="full", guarantee_owner="integrated storage/accessor/reflection conformance over the live runtime architecture workflow"),
    "validate-runnable-object-model": ActionSpec("validate-runnable-object-model", "validate runnable object-model execution end to end from the package root", "python:scripts/check_objc3c_runnable_object_model_end_to_end.py", ("test:objc3c:runnable-object-model",), validation_tier="full", guarantee_owner="packaged compile, object-model probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-storage-reflection": ActionSpec("validate-runnable-storage-reflection", "validate runnable storage/reflection execution end to end from the package root", "python:scripts/check_objc3c_runnable_storage_reflection_end_to_end.py", ("test:objc3c:runnable-storage-reflection",), validation_tier="full", guarantee_owner="packaged compile, storage/reflection probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "test-fixture-matrix": ActionSpec("test-fixture-matrix", "broad positive recovery fixture matrix sweep", "pwsh:scripts/run_objc3c_native_fixture_matrix.ps1", ("test:objc3c:fixture-matrix",), validation_tier="nightly", guarantee_owner="broad positive corpus artifact sanity", pass_through_args=True),
    "test-negative-expectations": ActionSpec("test-negative-expectations", "static negative fixture expectation enforcement", "pwsh:scripts/check_objc3c_negative_fixture_expectations.ps1", ("test:objc3c:negative-expectations",), validation_tier="nightly", guarantee_owner="negative expectation header and token enforcement", pass_through_args=True),
    "test-full": ActionSpec("test-full", "full developer validation entrypoint", "runner-internal + direct PowerShell suites", ("test:objc3c:full",), validation_tier="full", guarantee_owner="smoke, runtime acceptance, and replay without full recovery fan-out"),
    "test-nightly": ActionSpec("test-nightly", "exhaustive validation entrypoint", "runner-internal + direct PowerShell suites", ("test:objc3c:nightly",), validation_tier="nightly", guarantee_owner="full validation plus recovery and broad corpus sweeps"),
    "package-runnable-toolchain": ActionSpec("package-runnable-toolchain", "package the runnable native toolchain", "pwsh:scripts/package_objc3c_runnable_toolchain.ps1", ("package:objc3c-native:runnable-toolchain",)),
    "proof-objc3c": ActionSpec("proof-objc3c", "run the native compile proof workflow", "pwsh:scripts/run_objc3c_native_compile_proof.ps1", ("proof:objc3c",)),
}


ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-default": action_build_default,
    "build-native-binaries": action_build_native_binaries,
    "build-native-contracts": action_build_native_contracts,
    "build-native-full": action_build_native_full,
    "build-native-reconfigure": action_build_native_reconfigure,
    "build-spec": action_build_spec,
    "compile-objc3c": action_compile_objc3c,
    "lint-spec": action_lint_spec,
    "test-default": action_test_default,
    "test-fast": action_test_fast,
    "test-smoke": action_test_smoke,
    "test-ci": action_test_ci,
    "test-recovery": action_test_recovery,
    "test-execution-smoke": action_test_execution_smoke,
    "test-execution-replay": action_test_execution_replay,
    "test-runtime-acceptance": action_test_runtime_acceptance,
    "proof-runtime-architecture": action_proof_runtime_architecture,
    "validate-runtime-architecture": action_validate_runtime_architecture,
    "validate-runnable-bootstrap": action_validate_runnable_bootstrap,
    "validate-block-arc-conformance": action_validate_block_arc_conformance,
    "validate-object-model-conformance": action_validate_object_model_conformance,
    "validate-storage-reflection-conformance": action_validate_storage_reflection_conformance,
    "validate-runnable-object-model": action_validate_runnable_object_model,
    "validate-runnable-storage-reflection": action_validate_runnable_storage_reflection,
    "test-fixture-matrix": action_test_fixture_matrix,
    "test-negative-expectations": action_test_negative_expectations,
    "test-full": action_test_full,
    "test-nightly": action_test_nightly,
    "package-runnable-toolchain": action_package_runnable_toolchain,
    "proof-objc3c": action_proof_objc3c,
}


def emit_json(payload: object) -> int:
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


def list_actions_payload() -> dict[str, object]:
    return {
        "mode": "m314-c002-parameterized-task-runner-v1",
        "runner_path": "scripts/objc3c_public_workflow_runner.py",
        "action_count": len(ACTION_SPECS),
        "actions": [asdict(spec) for spec in ACTION_SPECS.values()],
    }


def describe_action_payload(action: str) -> dict[str, object]:
    spec = ACTION_SPECS[action]
    payload = asdict(spec)
    payload["mode"] = "m314-c002-parameterized-task-runner-v1"
    payload["runner_path"] = "scripts/objc3c_public_workflow_runner.py"
    return payload


def execute_registered_action(action: str, rest: list[str]) -> int:
    spec = ACTION_SPECS.get(action)
    handler = ACTION_HANDLERS.get(action)
    if spec is None or handler is None:
        print(f"unknown action: {action}", file=sys.stderr)
        return 2
    if rest and not spec.pass_through_args:
        print(f"action does not accept extra arguments: {action}", file=sys.stderr)
        return 2
    return handler(rest)


def main(argv: Sequence[str]) -> int:
    if not argv:
        print(
            "usage: objc3c_public_workflow_runner.py <action> [args...]\n"
            "       objc3c_public_workflow_runner.py --list-json\n"
            "       objc3c_public_workflow_runner.py --describe <action>",
            file=sys.stderr,
        )
        return 2

    action, *rest = argv
    if action == "--list-json":
        return emit_json(list_actions_payload())
    if action == "--describe":
        if len(rest) != 1:
            print("usage: objc3c_public_workflow_runner.py --describe <action>", file=sys.stderr)
            return 2
        describe_action = rest[0]
        if describe_action not in ACTION_SPECS:
            print(f"unknown action: {describe_action}", file=sys.stderr)
            return 2
        return emit_json(describe_action_payload(describe_action))
    return execute_registered_action(action, rest)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
