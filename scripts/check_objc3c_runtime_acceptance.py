#!/usr/bin/env python3
"""Compile and run the live objc3 runtime acceptance workload."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from time import perf_counter
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-runtime-acceptance"
REPORT_ROOT = ROOT / "tmp" / "reports" / "runtime" / "acceptance"
BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PWSH = shutil.which("pwsh") or shutil.which("powershell") or "pwsh"
RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID = "objc3c.runtime.state.publication.surface.v1"
RUNTIME_STATE_PUBLICATION_SURFACE_KIND = "compile-manifest-plus-registration-manifest"
RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID = "objc3c.runtime.bootstrap.registration.source.surface.v1"
RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.bootstrap.lowering.registration.artifact.surface.v1"
)
RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.multi.image.startup.ordering.source.surface.v1"
)
RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.object.model.realization.source.surface.v1"
)
RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.property.ivar.storage.accessor.source.surface.v1"
)
RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.block.arc.unified.source.surface.v1"
)
RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.ownership.transfer.capture.family.source.surface.v1"
)
RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.block.arc.lowering.helper.surface.v1"
)
RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.block.arc.runtime.abi.surface.v1"
)
RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.execution.cleanup.source.surface.v1"
)
RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.catch.filter.finalization.source.surface.v1"
)
RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.propagation.cleanup.semantics.surface.v1"
)
RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.bridging.filter.unwind.diagnostics.surface.v1"
)
RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.lowering.unwind.bridge.helper.surface.v1"
)
RUNTIME_CROSS_MODULE_ERROR_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.error.metadata.replay.preservation.surface.v1"
)
RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.runtime.abi.cleanup.surface.v1"
)
RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.propagation.catch.cleanup.runtime.implementation.surface.v1"
)
DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
)
EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.executable.property.accessor.layout.lowering.v1"
)
EXECUTABLE_IVAR_LAYOUT_EMISSION_SURFACE_CONTRACT_ID = (
    "objc3c.executable.ivar.layout.emission.v1"
)
EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.executable.synthesized.accessor.property.lowering.v1"
)
RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.storage.accessor.abi.surface.v1"
)
RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.property.ivar.accessor.reflection.implementation.surface.v1"
)
RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.property.atomicity.synthesis.reflection.source.surface.v1"
)
RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1"
)
RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1"
)
RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1"
)
RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.storage.reflection.artifact.preservation.v1"
)
RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.block.ownership.artifact.preservation.v1"
)
RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.object.model.abi.query.surface.v1"
)
RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lookup.reflection.implementation.surface.v1"
)
RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.reflection.query.surface.v1"
)
RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lookup.semantics.v1"
)
RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.class.metaclass.protocol.realization.v1"
)
RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.category.attachment.merged.dispatch.surface.v1"
)
RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1"
)
RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.unified.concurrency.source.surface.v1"
)
RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.async.task.actor.normalization.completion.surface.v1"
)
RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.unified.concurrency.lowering.metadata.surface.v1"
)
RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.unified.concurrency.runtime.abi.surface.v1"
)
RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.source.surface.v1"
)
RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.package.provenance.source.surface.v1"
)
RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.semantics.surface.v1"
)
RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID = "objc3c.runtime.acceptance.suite.surface.v1"
RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID = "objc3c.runtime.installation.abi.surface.v1"
RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID = "objc3c.runtime.loader.lifecycle.surface.v1"
RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL = (
    "duplicate-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state"
)
RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL = (
    "out-of-order-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state"
)
RUNTIME_PUBLIC_HEADER_PATH = "native/objc3c/src/runtime/objc3_runtime.h"
RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH = (
    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
)
INSTALLATION_LIFECYCLE_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3"
)
INSTALLATION_LIFECYCLE_PROBE = (
    "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp"
)
IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_packaging_provider.objc3"
)
IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3"
)
IMPORTED_RUNTIME_PACKAGING_PROBE = (
    "tests/tooling/runtime/import_module_execution_matrix_probe.cpp"
)
CONTINUATION_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/continuation_runtime_helper_probe.cpp"
)
TASK_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/task_runtime_abi_completion_probe.cpp"
)
ACTOR_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp"
)
LIVE_CONTINUATION_RUNTIME_FIXTURE = (
    "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3"
)
LIVE_CONTINUATION_RUNTIME_PROBE = (
    "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp"
)
LIVE_TASK_RUNTIME_FIXTURE = (
    "tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3"
)
LIVE_TASK_RUNTIME_PROBE = (
    "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp"
)
LIVE_ACTOR_RUNTIME_FIXTURE = (
    "tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3"
)
LIVE_ACTOR_RUNTIME_PROBE = (
    "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp"
)
CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/cross_module_actor_isolation_provider.objc3"
)
CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/cross_module_actor_isolation_consumer.objc3"
)
STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3"
)
STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3"
)
BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3"
)
BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3"
)
BLOCK_ARC_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp"
)
REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE = (
    "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp"
)
RUNTIME_ACCEPTANCE_COMMAND = "python scripts/check_objc3c_runtime_acceptance.py"
VALIDATE_RUNTIME_ARCHITECTURE_COMMAND = (
    "python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture"
)
PUBLIC_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_register_image",
    "objc3_runtime_lookup_selector",
    "objc3_runtime_dispatch_i32",
    "objc3_runtime_reset_for_testing",
]
RUNTIME_INSTALLATION_ABI_BOUNDARY = [
    "objc3_runtime_register_image",
    "objc3_runtime_copy_registration_state_for_testing",
    "objc3_runtime_reset_for_testing",
]
RUNTIME_LOADER_TESTING_BOUNDARY = [
    "objc3_runtime_stage_registration_table_for_bootstrap",
    "objc3_runtime_copy_image_walk_state_for_testing",
    "objc3_runtime_replay_registered_images_for_testing",
    "objc3_runtime_copy_reset_replay_state_for_testing",
]
PRIVATE_BLOCK_ARC_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_promote_block_i32",
    "objc3_runtime_invoke_block_i32",
    "objc3_runtime_retain_i32",
    "objc3_runtime_release_i32",
    "objc3_runtime_autorelease_i32",
    "objc3_runtime_push_autoreleasepool_scope",
    "objc3_runtime_pop_autoreleasepool_scope",
    "objc3_runtime_read_current_property_i32",
    "objc3_runtime_write_current_property_i32",
    "objc3_runtime_exchange_current_property_i32",
    "objc3_runtime_bind_current_property_context_for_testing",
    "objc3_runtime_clear_current_property_context_for_testing",
    "objc3_runtime_load_weak_current_property_i32",
    "objc3_runtime_store_weak_current_property_i32",
    "objc3_runtime_copy_arc_debug_state_for_testing",
    "objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
]
PRIVATE_ERROR_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_store_thrown_error_i32",
    "objc3_runtime_load_thrown_error_i32",
    "objc3_runtime_bridge_status_error_i32",
    "objc3_runtime_bridge_nserror_error_i32",
    "objc3_runtime_catch_matches_error_i32",
    "objc3_runtime_copy_error_bridge_state_for_testing",
]
PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_allocate_async_continuation_i32",
    "objc3_runtime_handoff_async_continuation_to_executor_i32",
    "objc3_runtime_resume_async_continuation_i32",
    "objc3_runtime_spawn_task_i32",
    "objc3_runtime_enter_task_group_scope_i32",
    "objc3_runtime_add_task_group_task_i32",
    "objc3_runtime_wait_task_group_next_i32",
    "objc3_runtime_cancel_task_group_i32",
    "objc3_runtime_task_is_cancelled_i32",
    "objc3_runtime_task_on_cancel_i32",
    "objc3_runtime_executor_hop_i32",
    "objc3_runtime_actor_enter_isolation_thunk_i32",
    "objc3_runtime_actor_enter_nonisolated_i32",
    "objc3_runtime_actor_hop_to_executor_i32",
    "objc3_runtime_actor_record_replay_proof_i32",
    "objc3_runtime_actor_record_race_guard_i32",
    "objc3_runtime_actor_bind_executor_i32",
    "objc3_runtime_actor_mailbox_enqueue_i32",
    "objc3_runtime_actor_mailbox_drain_next_i32",
    "objc3_runtime_copy_async_continuation_state_for_testing",
    "objc3_runtime_copy_task_runtime_state_for_testing",
    "objc3_runtime_copy_actor_runtime_state_for_testing",
]
UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY_MODEL = (
    "private-async-task-and-actor-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header"
)
UNIFIED_CONCURRENCY_CONTINUATION_RUNTIME_MODEL = (
    "continuation-allocation-handoff-resume-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
)
UNIFIED_CONCURRENCY_TASK_RUNTIME_MODEL = (
    "task-spawn-group-cancellation-executor-hop-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
)
UNIFIED_CONCURRENCY_ACTOR_RUNTIME_MODEL = (
    "actor-isolation-nonisolated-hop-replay-race-guard-mailbox-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
)
UNIFIED_CONCURRENCY_RUNTIME_FAIL_CLOSED_MODEL = (
    "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-concurrency-runtime-abi-widening"
)
BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL = (
    "private-block-and-arc-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header"
)
BLOCK_ARC_RUNTIME_BLOCK_MODEL = (
    "promote-invoke-and-handle-lifetime-for-supported-block-records-stay-on-bootstrap-internal-runtime-entrypoints"
)
BLOCK_ARC_RUNTIME_ARC_MODEL = (
    "retain-release-autorelease-autoreleasepool-and-current-property-weak-helper-traffic-stays-on-bootstrap-internal-runtime-entrypoints"
)
BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL = (
    "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-runtime-abi-widening"
)
COMPILE_PROVENANCE_CONTRACT_ID = "objc3c.native.compile.output.provenance.v1"
COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID = "objc3c.native.compile.output.truthfulness.v1"


def run(command: list[str], *, cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=str(cwd or ROOT),
        text=True,
        capture_output=True,
        check=False,
    )


def ensure_native_binaries() -> None:
    if NATIVE_EXE.is_file() and RUNTIME_LIB.is_file():
        return
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(BUILD_PS1),
            "-ExecutionMode",
            "binaries-only",
        ]
    )
    if result.returncode != 0:
        raise RuntimeError(
            "native build failed:\nSTDOUT:\n"
            + result.stdout
            + "\nSTDERR:\n"
            + result.stderr
        )
    if not NATIVE_EXE.is_file() or not RUNTIME_LIB.is_file():
        raise RuntimeError("native build completed without publishing the runtime executable/library")


def find_clangxx() -> str:
    llvm_root = os.environ.get("LLVM_ROOT")
    if llvm_root:
        candidate = Path(llvm_root) / "bin" / "clang++.exe"
        if candidate.is_file():
            return str(candidate)
    candidate = shutil.which("clang++")
    if candidate:
        return candidate
    raise RuntimeError("clang++ not found; set LLVM_ROOT or ensure clang++ is on PATH")


def compile_fixture_with_args(
    fixture: Path, out_dir: Path, extra_args: list[str] | None = None
) -> Path:
    out_dir.mkdir(parents=True, exist_ok=True)
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(COMPILE_PS1),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            *(extra_args or []),
        ]
    )
    if result.returncode != 0:
        raise RuntimeError(
            f"fixture compile failed for {fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    obj_path = out_dir / "module.obj"
    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    registration_descriptor_path = out_dir / "module.runtime-registration-descriptor.json"
    ll_path = out_dir / "module.ll"
    provenance_path = out_dir / "module.compile-provenance.json"
    if not obj_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {obj_path}")
    if not manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {manifest_path}")
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")
    if not registration_descriptor_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_descriptor_path}")
    if not ll_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {ll_path}")
    if not provenance_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {provenance_path}")

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    provenance = json.loads(provenance_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    registration_descriptor = json.loads(registration_descriptor_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    bootstrap_source_surface = manifest.get("runtime_bootstrap_registration_source_surface")
    bootstrap_lowering_registration_artifact_surface = manifest.get(
        "runtime_bootstrap_lowering_registration_artifact_surface"
    )
    multi_image_startup_ordering_source_surface = manifest.get(
        "runtime_multi_image_startup_ordering_source_surface"
    )
    object_model_realization_source_surface = manifest.get(
        "runtime_object_model_realization_source_surface"
    )
    block_arc_unified_source_surface = manifest.get(
        "runtime_block_arc_unified_source_surface"
    )
    ownership_transfer_capture_family_source_surface = manifest.get(
        "runtime_ownership_transfer_capture_family_source_surface"
    )
    block_arc_lowering_helper_surface = manifest.get(
        "runtime_block_arc_lowering_helper_surface"
    )
    block_arc_runtime_abi_surface = manifest.get(
        "runtime_block_arc_runtime_abi_surface"
    )
    dispatch_and_synthesized_accessor_lowering_surface = manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface"
    )
    executable_property_accessor_layout_lowering_surface = manifest.get(
        "executable_property_accessor_layout_lowering_surface"
    )
    executable_ivar_layout_emission_surface = manifest.get(
        "executable_ivar_layout_emission_surface"
    )
    executable_synthesized_accessor_property_lowering_surface = manifest.get(
        "executable_synthesized_accessor_property_lowering_surface"
    )
    property_ivar_storage_accessor_source_surface = manifest.get(
        "runtime_property_ivar_storage_accessor_source_surface"
    )
    property_atomicity_synthesis_reflection_source_surface = manifest.get(
        "runtime_property_atomicity_synthesis_reflection_source_surface"
    )
    realization_lowering_reflection_artifact_surface = manifest.get(
        "runtime_realization_lowering_reflection_artifact_surface"
    )
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface"
    )
    object_model_abi_query_surface = manifest.get("runtime_object_model_abi_query_surface")
    realization_lookup_reflection_implementation_surface = manifest.get(
        "runtime_realization_lookup_reflection_implementation_surface"
    )
    reflection_query_surface = manifest.get("runtime_reflection_query_surface")
    realization_lookup_semantics_surface = manifest.get(
        "runtime_realization_lookup_semantics_surface"
    )
    class_metaclass_protocol_realization_surface = manifest.get(
        "runtime_class_metaclass_protocol_realization_surface"
    )
    category_attachment_merged_dispatch_surface = manifest.get(
        "runtime_category_attachment_merged_dispatch_surface"
    )
    reflection_visibility_coherence_diagnostics_surface = manifest.get(
        "runtime_reflection_visibility_coherence_diagnostics_surface"
    )
    unified_concurrency_source_surface = manifest.get(
        "runtime_unified_concurrency_source_surface"
    )
    async_task_actor_normalization_completion_surface = manifest.get(
        "runtime_async_task_actor_normalization_completion_surface"
    )
    unified_concurrency_lowering_metadata_surface = manifest.get(
        "runtime_unified_concurrency_lowering_metadata_surface"
    )
    unified_concurrency_runtime_abi_surface = manifest.get(
        "runtime_unified_concurrency_runtime_abi_surface"
    )
    registration_descriptor_frontend_closure = semantic_surface.get(
        "objc_runtime_registration_descriptor_frontend_closure",
        manifest.get("objc_runtime_registration_descriptor_frontend_closure", {}),
    )
    registration_descriptor_image_root_source_surface = semantic_surface.get(
        "objc_runtime_registration_descriptor_image_root_source_surface",
        manifest.get("objc_runtime_registration_descriptor_image_root_source_surface", {}),
    )
    translation_unit_registration_manifest = semantic_surface.get(
        "objc_runtime_translation_unit_registration_manifest",
        manifest.get("objc_runtime_translation_unit_registration_manifest", {}),
    )
    runtime_bootstrap_lowering = semantic_surface.get(
        "objc_runtime_bootstrap_lowering_contract",
        manifest.get(
            "objc_runtime_bootstrap_lowering_contract",
            manifest.get("objc_runtime_bootstrap_lowering", {}),
        ),
    )
    bootstrap_legality_semantics = semantic_surface.get(
        "objc_runtime_bootstrap_legality_semantics",
        manifest.get("objc_runtime_bootstrap_legality_semantics", {}),
    )
    bootstrap_failure_restart_semantics = semantic_surface.get(
        "objc_runtime_bootstrap_failure_restart_semantics",
        manifest.get("objc_runtime_bootstrap_failure_restart_semantics", {}),
    )
    runtime_bootstrap_semantics = semantic_surface.get(
        "objc_runtime_startup_bootstrap_semantics",
        manifest.get("objc_runtime_startup_bootstrap_semantics", {}),
    )
    bootstrap_api_contract = semantic_surface.get(
        "objc_runtime_bootstrap_api_contract",
        manifest.get("objc_runtime_bootstrap_api_contract", {}),
    )
    bootstrap_reset_contract = semantic_surface.get(
        "objc_runtime_bootstrap_reset_contract",
        manifest.get("objc_runtime_bootstrap_reset_contract", {}),
    )
    bootstrap_registrar_contract = semantic_surface.get(
        "objc_runtime_bootstrap_registrar_contract",
        manifest.get("objc_runtime_bootstrap_registrar_contract", {}),
    )
    bootstrap_archive_static_link_replay_corpus = semantic_surface.get(
        "objc_runtime_bootstrap_archive_static_link_replay_corpus",
        manifest.get("objc_runtime_bootstrap_archive_static_link_replay_corpus", {}),
    )
    publication_surface = manifest.get("runtime_state_publication_surface")
    runtime_installation_abi_surface = manifest.get("runtime_installation_abi_surface")
    runtime_loader_lifecycle_surface = manifest.get("runtime_loader_lifecycle_surface")
    if not isinstance(publication_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_state_publication_surface")
    if publication_surface.get("contract_id") != RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID:
        raise RuntimeError("compiled fixture manifest published the wrong runtime_state_publication_surface contract")
    if publication_surface.get("publication_surface_kind") != RUNTIME_STATE_PUBLICATION_SURFACE_KIND:
        raise RuntimeError("compiled fixture manifest published the wrong runtime_state_publication_surface kind")
    if publication_surface.get("compile_manifest_artifact") != "module.manifest.json":
        raise RuntimeError("runtime_state_publication_surface drifted from the compile manifest artifact path")
    if publication_surface.get("registration_manifest_artifact") != "module.runtime-registration-manifest.json":
        raise RuntimeError("runtime_state_publication_surface drifted from the runtime registration manifest artifact path")
    if publication_surface.get("object_artifact") != "module.obj":
        raise RuntimeError("runtime_state_publication_surface drifted from the emitted object artifact path")
    if publication_surface.get("backend_artifact") != "module.ll":
        raise RuntimeError("runtime_state_publication_surface drifted from the emitted LLVM IR artifact path")
    if publication_surface.get("runtime_support_library_archive_relative_path") != registration_manifest.get("runtime_support_library_archive_relative_path"):
        raise RuntimeError("runtime_state_publication_surface drifted from the runtime registration manifest archive path")
    if publication_surface.get("registration_entrypoint_symbol") != registration_manifest.get("registration_entrypoint_symbol"):
        raise RuntimeError("runtime_state_publication_surface drifted from the runtime registration entrypoint symbol")
    if publication_surface.get("runtime_state_snapshot_symbol") != registration_manifest.get("runtime_state_snapshot_symbol"):
        raise RuntimeError("runtime_state_publication_surface drifted from the runtime state snapshot symbol")
    if publication_surface.get("public_runtime_abi_boundary") != PUBLIC_RUNTIME_ABI_BOUNDARY:
        raise RuntimeError("runtime_state_publication_surface drifted from the public runtime ABI boundary")
    for field in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        if publication_surface.get(field) != registration_manifest.get(field):
            raise RuntimeError(f"runtime_state_publication_surface drifted from registration manifest field {field}")
    if publication_surface.get("publication_requires_coupled_registration_manifest") is not True:
        raise RuntimeError("runtime_state_publication_surface must require the coupled runtime registration manifest")
    if publication_surface.get("publication_requires_real_compile_output") is not True:
        raise RuntimeError("runtime_state_publication_surface must require real compile output")
    if not isinstance(bootstrap_source_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_bootstrap_registration_source_surface")
    if bootstrap_source_surface.get("contract_id") != RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID:
        raise RuntimeError("compiled fixture manifest published the wrong runtime_bootstrap_registration_source_surface contract")
    if bootstrap_source_surface.get("compile_manifest_artifact") != "module.manifest.json":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the compile manifest artifact path")
    if bootstrap_source_surface.get("registration_manifest_artifact") != "module.runtime-registration-manifest.json":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the runtime registration manifest artifact path")
    if bootstrap_source_surface.get("registration_descriptor_artifact") != "module.runtime-registration-descriptor.json":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the runtime registration descriptor artifact path")
    if bootstrap_source_surface.get("object_artifact") != "module.obj":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the emitted object artifact path")
    if bootstrap_source_surface.get("backend_artifact") != "module.ll":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the emitted LLVM IR artifact path")
    if bootstrap_source_surface.get("source_surface_contract_id") != registration_descriptor_image_root_source_surface.get("contract_id"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration-descriptor image-root source contract")
    if bootstrap_source_surface.get("frontend_closure_contract_id") != registration_descriptor_frontend_closure.get("contract_id"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration-descriptor frontend closure contract")
    if bootstrap_source_surface.get("registration_manifest_contract_id") != translation_unit_registration_manifest.get("contract_id"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration manifest contract")
    if bootstrap_source_surface.get("bootstrap_lowering_contract_id") != runtime_bootstrap_lowering.get("contract_id"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the bootstrap lowering contract")
    if bootstrap_source_surface.get("runtime_support_library_archive_relative_path") != registration_manifest.get("runtime_support_library_archive_relative_path"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the runtime support library archive path")
    if bootstrap_source_surface.get("registration_descriptor_identifier") != registration_descriptor_frontend_closure.get("registration_descriptor_identifier"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration descriptor identifier")
    if bootstrap_source_surface.get("image_root_identifier") != registration_descriptor_frontend_closure.get("image_root_identifier"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the image root identifier")
    if bootstrap_source_surface.get("registration_entrypoint_symbol") != registration_manifest.get("registration_entrypoint_symbol"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration entrypoint symbol")
    if bootstrap_source_surface.get("constructor_root_symbol") != registration_manifest.get("constructor_root_symbol"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the constructor root symbol")
    if bootstrap_source_surface.get("translation_unit_identity_key") != registration_manifest.get("translation_unit_identity_key"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the translation unit identity key")
    if (
        bootstrap_source_surface.get("translation_unit_registration_order_ordinal")
        != registration_manifest.get("translation_unit_registration_order_ordinal")
    ):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the translation unit registration order ordinal")
    if bootstrap_source_surface.get("requires_coupled_registration_descriptor_artifact") is not True:
        raise RuntimeError("runtime_bootstrap_registration_source_surface must require the coupled runtime registration descriptor artifact")
    if bootstrap_source_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError("runtime_bootstrap_registration_source_surface must require the coupled runtime registration manifest")
    if bootstrap_source_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError("runtime_bootstrap_registration_source_surface must require real compile output")
    if not isinstance(bootstrap_lowering_registration_artifact_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_bootstrap_lowering_registration_artifact_surface"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("contract_id")
        != RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_bootstrap_lowering_registration_artifact_surface contract"
        )
    if bootstrap_lowering_registration_artifact_surface.get("compile_manifest_artifact") != "module.manifest.json":
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the compile manifest artifact path"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("registration_manifest_artifact")
        != "module.runtime-registration-manifest.json"
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("registration_descriptor_artifact")
        != "module.runtime-registration-descriptor.json"
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the runtime registration descriptor artifact path"
        )
    if bootstrap_lowering_registration_artifact_surface.get("object_artifact") != "module.obj":
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the emitted object artifact path"
        )
    if bootstrap_lowering_registration_artifact_surface.get("backend_artifact") != "module.ll":
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the emitted LLVM IR artifact path"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("bootstrap_lowering_contract_id")
        != runtime_bootstrap_lowering.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the bootstrap lowering contract"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("registration_manifest_contract_id")
        != translation_unit_registration_manifest.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the registration manifest contract"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("bootstrap_semantics_contract_id")
        != runtime_bootstrap_semantics.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the bootstrap semantics contract"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "registration_descriptor_frontend_closure_contract_id"
        )
        != registration_descriptor_frontend_closure.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the registration-descriptor frontend closure contract"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "runtime_support_library_archive_relative_path"
        )
        != registration_manifest.get("runtime_support_library_archive_relative_path")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the runtime support library archive path"
        )
    for surface_field, lowering_field in (
        ("constructor_root_symbol", "constructor_root_symbol"),
        ("init_stub_symbol_prefix", "constructor_init_stub_symbol_prefix"),
        ("registration_table_symbol_prefix", "registration_table_symbol_prefix"),
        ("image_local_init_state_symbol_prefix", "image_local_init_state_symbol_prefix"),
        ("registration_entrypoint_symbol", "registration_entrypoint_symbol"),
        ("global_ctor_list_model", "global_ctor_list_model"),
        ("registration_table_layout_model", "registration_table_layout_model"),
        ("image_local_initialization_model", "image_local_initialization_model"),
        ("registration_table_abi_version", "registration_table_abi_version"),
        ("registration_table_pointer_field_count", "registration_table_pointer_field_count"),
        ("constructor_root_emission_state", "constructor_root_emission_state"),
        ("init_stub_emission_state", "init_stub_emission_state"),
        ("registration_table_emission_state", "registration_table_emission_state"),
        ("bootstrap_ir_materialization_landed", "bootstrap_ir_materialization_landed"),
        ("image_local_initialization_landed", "image_local_initialization_landed"),
    ):
        if (
            bootstrap_lowering_registration_artifact_surface.get(surface_field)
            != runtime_bootstrap_lowering.get(lowering_field)
        ):
            raise RuntimeError(
                "runtime_bootstrap_lowering_registration_artifact_surface drifted "
                f"from bootstrap lowering field {lowering_field}"
            )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "requires_coupled_registration_descriptor_artifact"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface must require the coupled runtime registration descriptor artifact"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface must require the coupled runtime registration manifest"
        )
    if bootstrap_lowering_registration_artifact_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface must require real compile output"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "lowered_registration_descriptor_fields"
        )
        != [
            "constructor_init_stub_symbol",
            "bootstrap_registration_table_symbol",
            "bootstrap_image_local_init_state_symbol",
            "bootstrap_registration_table_layout_model",
            "bootstrap_image_local_initialization_model",
            "bootstrap_registration_table_abi_version",
            "bootstrap_registration_table_pointer_field_count",
            "translation_unit_registration_order_ordinal",
        ]
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the lowered registration descriptor field set"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "loader_table_ir_proof_fields"
        )
        != [
            "constructor_root_symbol",
            "constructor_init_stub_symbol",
            "bootstrap_registration_table_symbol",
            "bootstrap_image_local_init_state_symbol",
            "translation_unit_registration_order_ordinal",
        ]
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the loader-table IR proof field set"
        )
    if bootstrap_lowering_registration_artifact_surface.get("requires_emitted_loader_table_ir") is not True:
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface must require emitted loader-table IR"
        )
    if registration_descriptor.get("bootstrap_lowering_contract_id") != runtime_bootstrap_lowering.get(
        "contract_id"
    ):
        raise RuntimeError(
            "runtime registration descriptor drifted from the bootstrap lowering contract"
        )
    for descriptor_field, lowering_field in (
        ("bootstrap_registration_table_layout_model", "registration_table_layout_model"),
        ("bootstrap_image_local_initialization_model", "image_local_initialization_model"),
        ("bootstrap_constructor_root_emission_state", "constructor_root_emission_state"),
        ("bootstrap_init_stub_emission_state", "init_stub_emission_state"),
        ("bootstrap_registration_table_emission_state", "registration_table_emission_state"),
        ("bootstrap_registration_table_abi_version", "registration_table_abi_version"),
        ("bootstrap_registration_table_pointer_field_count", "registration_table_pointer_field_count"),
    ):
        if registration_descriptor.get(descriptor_field) != runtime_bootstrap_lowering.get(
            lowering_field
        ):
            raise RuntimeError(
                f"runtime registration descriptor drifted from bootstrap lowering field {lowering_field}"
            )
    for descriptor_field, manifest_field in (
        ("constructor_root_symbol", "constructor_root_symbol"),
        ("registration_entrypoint_symbol", "registration_entrypoint_symbol"),
        ("constructor_init_stub_symbol", "constructor_init_stub_symbol"),
        ("bootstrap_registration_table_symbol", "bootstrap_registration_table_symbol"),
        ("bootstrap_image_local_init_state_symbol", "bootstrap_image_local_init_state_symbol"),
        ("translation_unit_registration_order_ordinal", "translation_unit_registration_order_ordinal"),
    ):
        if registration_descriptor.get(descriptor_field) != registration_manifest.get(manifest_field):
            raise RuntimeError(
                f"runtime registration descriptor drifted from registration manifest field {manifest_field}"
            )
    if registration_descriptor.get("ready_for_loader_table_lowering") is not True:
        raise RuntimeError(
            "runtime registration descriptor must be ready for loader-table lowering"
        )
    for symbol_field in (
        "constructor_root_symbol",
        "constructor_init_stub_symbol",
        "bootstrap_registration_table_symbol",
        "bootstrap_image_local_init_state_symbol",
    ):
        symbol = registration_descriptor.get(symbol_field)
        if not isinstance(symbol, str) or not symbol:
            raise RuntimeError(f"runtime registration descriptor omitted {symbol_field}")
        if f"@{symbol}" not in ll_text:
            raise RuntimeError(
                f"emitted LLVM IR did not lower runtime bootstrap symbol {symbol_field}"
            )
    ctor_loader_table_edge = (
        f"ptr @{registration_descriptor['constructor_root_symbol']}, ptr "
        f"@{registration_descriptor['bootstrap_registration_table_symbol']}"
    )
    if ctor_loader_table_edge not in ll_text:
        raise RuntimeError(
            "emitted LLVM IR did not lower the constructor-root to loader-table global ctor edge"
        )
    if not isinstance(multi_image_startup_ordering_source_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_multi_image_startup_ordering_source_surface")
    if (
        multi_image_startup_ordering_source_surface.get("contract_id")
        != RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_multi_image_startup_ordering_source_surface contract"
        )
    if multi_image_startup_ordering_source_surface.get("compile_manifest_artifact") != "module.manifest.json":
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the compile manifest artifact path"
        )
    if multi_image_startup_ordering_source_surface.get("registration_manifest_artifact") != "module.runtime-registration-manifest.json":
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the runtime registration manifest artifact path"
        )
    if multi_image_startup_ordering_source_surface.get("registration_descriptor_artifact") != "module.runtime-registration-descriptor.json":
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_legality_semantics_contract_id")
        != bootstrap_legality_semantics.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap legality semantics contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_failure_restart_contract_id")
        != bootstrap_failure_restart_semantics.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap failure restart contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_api_contract_id")
        != bootstrap_api_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap API contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_reset_contract_id")
        != bootstrap_reset_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap reset contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_registrar_contract_id")
        != bootstrap_registrar_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap registrar contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("archive_static_link_replay_corpus_contract_id")
        != bootstrap_archive_static_link_replay_corpus.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the archive static-link replay corpus contract"
        )
    if multi_image_startup_ordering_source_surface.get("public_header_path") != RUNTIME_PUBLIC_HEADER_PATH:
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the public runtime header path"
        )
    if multi_image_startup_ordering_source_surface.get("internal_header_path") != RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH:
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap internal header path"
        )
    if (
        multi_image_startup_ordering_source_surface.get("registration_entrypoint_symbol")
        != registration_manifest.get("registration_entrypoint_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the registration entrypoint symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("state_snapshot_symbol")
        != bootstrap_api_contract.get("state_snapshot_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the runtime state snapshot symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("reset_for_testing_symbol")
        != bootstrap_api_contract.get("reset_for_testing_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the reset-for-testing symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("replay_registered_images_symbol")
        != bootstrap_failure_restart_semantics.get("replay_registered_images_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the replay-registered-images symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("reset_replay_state_snapshot_symbol")
        != bootstrap_failure_restart_semantics.get("reset_replay_state_snapshot_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the reset replay state snapshot symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("translation_unit_identity_key")
        != registration_manifest.get("translation_unit_identity_key")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the translation unit identity key"
        )
    if (
        multi_image_startup_ordering_source_surface.get("translation_unit_registration_order_ordinal")
        != registration_manifest.get("translation_unit_registration_order_ordinal")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the translation unit registration order ordinal"
        )
    if (
        multi_image_startup_ordering_source_surface.get("duplicate_registration_status_code")
        != runtime_bootstrap_semantics.get("duplicate_registration_status_code")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the duplicate registration status code"
        )
    if (
        multi_image_startup_ordering_source_surface.get("out_of_order_status_code")
        != runtime_bootstrap_semantics.get("out_of_order_status_code")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the out-of-order registration status code"
        )
    if (
        multi_image_startup_ordering_source_surface.get("duplicate_install_diagnostic_model")
        != RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the duplicate-install diagnostic model"
        )
    if (
        multi_image_startup_ordering_source_surface.get("out_of_order_install_diagnostic_model")
        != RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the out-of-order-install diagnostic model"
        )
    if (
        multi_image_startup_ordering_source_surface.get("last_rejected_module_name_field")
        != "last_rejected_module_name"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the rejected module-name field"
        )
    if (
        multi_image_startup_ordering_source_surface.get("last_rejected_translation_unit_identity_key_field")
        != "last_rejected_translation_unit_identity_key"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the rejected translation-unit identity field"
        )
    if runtime_bootstrap_semantics.get("invalid_registration_roots_status_code") != -4:
        raise RuntimeError(
            "objc_runtime_startup_bootstrap_semantics drifted from the invalid registration roots status code"
        )
    if bootstrap_failure_restart_semantics.get("invalid_registration_roots_status_code") != -4:
        raise RuntimeError(
            "objc_runtime_bootstrap_failure_restart_semantics drifted from the invalid registration roots status code"
        )
    if (
        multi_image_startup_ordering_source_surface.get("next_expected_registration_order_field")
        != "next_expected_registration_order_ordinal"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the next-expected registration order field"
        )
    if (
        multi_image_startup_ordering_source_surface.get("last_successful_registration_order_field")
        != "last_successful_registration_order_ordinal"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the last-successful registration order field"
        )
    if (
        multi_image_startup_ordering_source_surface.get("last_rejected_registration_order_field")
        != "last_rejected_registration_order_ordinal"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the last-rejected registration order field"
        )
    if multi_image_startup_ordering_source_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface must require a linked runtime probe"
        )
    if multi_image_startup_ordering_source_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface must require real compile output"
        )
    if not isinstance(object_model_realization_source_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_object_model_realization_source_surface")
    if (
        object_model_realization_source_surface.get("contract_id")
        != RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_object_model_realization_source_surface contract"
        )
    if object_model_realization_source_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the compile manifest artifact path"
        )
    if (
        object_model_realization_source_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        object_model_realization_source_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if object_model_realization_source_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the emitted object artifact path"
        )
    if object_model_realization_source_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_object_model_contract_fields = {
        "executable_realization_records_contract_id": "objc3c.executable.realization.records.v1",
        "runtime_class_realization_contract_id": "objc3c.runtime.class.realization.freeze.v1",
        "runtime_metaclass_graph_contract_id": "objc3c.runtime.metaclass.graph.root.class.baseline.v1",
        "runtime_category_attachment_protocol_conformance_contract_id": (
            "objc3c.runtime.category.attachment.protocol.conformance.v1"
        ),
        "canonical_runnable_object_support_contract_id": (
            "objc3c.runtime.canonical.runnable.object.sample.support.v1"
        ),
        "runtime_support_library_archive_relative_path": (
            registration_manifest.get("runtime_support_library_archive_relative_path")
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "realized_class_graph_snapshot_symbol": "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
    }
    for field, expected_value in expected_object_model_contract_fields.items():
        if object_model_realization_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_object_model_realization_source_surface drifted from {field}"
            )
    if object_model_realization_source_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface must require the coupled runtime registration manifest"
        )
    if object_model_realization_source_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface must require real compile output"
        )
    if object_model_realization_source_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface must require a linked runtime probe"
        )
    if not isinstance(unified_concurrency_source_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_unified_concurrency_source_surface"
        )
    if (
        unified_concurrency_source_surface.get("contract_id")
        != RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_unified_concurrency_source_surface contract"
        )
    expected_unified_concurrency_source_surface_fields = {
        "compile_manifest_artifact": manifest_path.name,
        "registration_manifest_artifact": registration_manifest_path.name,
        "registration_descriptor_artifact": registration_descriptor_path.name,
        "object_artifact": obj_path.name,
        "backend_artifact": ll_path.name,
        "source_surface_model": (
            "unified-concurrency-source-surface-freezes-live-async-actor-task-source-and-sema-boundaries-before-lowering-runtime-and-public-abi-expansion"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/runtime/objc3_runtime.h",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_async_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_member_and_isolation_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_cancellation_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model",
        ],
        "source_contract_ids": [
            "objc3c.concurrency.async.source.closure.v1",
            "objc3c.concurrency.actor.member.isolation.source.closure.v1",
            "objc3c.concurrency.task.group.cancellation.source.closure.v1",
            "objc3c.concurrency.async.effect.suspension.semantic.model.v1",
            "objc3c.concurrency.task.executor.cancellation.semantic.model.v1",
            "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_await_executor_source_closure_positive.objc3",
            "tests/tooling/fixtures/native/actor_member_isolation_surface_positive.objc3",
            "tests/tooling/fixtures/native/task_executor_cancellation_source_closure_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-concurrency-proof",
        ],
    }
    for field, expected_value in expected_unified_concurrency_source_surface_fields.items():
        if unified_concurrency_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_unified_concurrency_source_surface drifted from {field}"
            )
    if (
        unified_concurrency_source_surface.get("private_concurrency_runtime_boundary")
        != [
            "objc3_runtime_allocate_async_continuation_i32",
            "objc3_runtime_handoff_async_continuation_to_executor_i32",
            "objc3_runtime_resume_async_continuation_i32",
            "objc3_runtime_spawn_task_i32",
            "objc3_runtime_enter_task_group_scope_i32",
            "objc3_runtime_add_task_group_task_i32",
            "objc3_runtime_wait_task_group_next_i32",
            "objc3_runtime_cancel_task_group_i32",
            "objc3_runtime_task_is_cancelled_i32",
            "objc3_runtime_task_on_cancel_i32",
            "objc3_runtime_actor_enter_isolation_thunk_i32",
            "objc3_runtime_actor_enter_nonisolated_i32",
            "objc3_runtime_actor_hop_to_executor_i32",
            "objc3_runtime_actor_record_replay_proof_i32",
            "objc3_runtime_actor_record_race_guard_i32",
            "objc3_runtime_actor_bind_executor_i32",
            "objc3_runtime_actor_mailbox_enqueue_i32",
            "objc3_runtime_actor_mailbox_drain_next_i32",
            "objc3_runtime_copy_async_continuation_state_for_testing",
            "objc3_runtime_copy_task_runtime_state_for_testing",
            "objc3_runtime_copy_actor_runtime_state_for_testing",
        ]
    ):
        raise RuntimeError(
            "runtime_unified_concurrency_source_surface drifted from the private concurrency runtime boundary"
        )
    if (
        unified_concurrency_source_surface.get("requires_coupled_registration_manifest")
        is not True
        or unified_concurrency_source_surface.get("requires_real_compile_output")
        is not True
        or unified_concurrency_source_surface.get("requires_linked_runtime_probe")
        is not True
    ):
        raise RuntimeError(
            "runtime_unified_concurrency_source_surface must require the coupled registration manifest, real compile output, and linked runtime probes"
        )
    if not isinstance(async_task_actor_normalization_completion_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_async_task_actor_normalization_completion_surface"
        )
    if (
        async_task_actor_normalization_completion_surface.get("contract_id")
        != RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_async_task_actor_normalization_completion_surface contract"
        )
    expected_async_task_actor_normalization_completion_surface_fields = {
        "compile_manifest_artifact": manifest_path.name,
        "registration_manifest_artifact": registration_manifest_path.name,
        "registration_descriptor_artifact": registration_descriptor_path.name,
        "object_artifact": obj_path.name,
        "backend_artifact": ll_path.name,
        "source_surface_contract_id": RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
        "normalized_semantic_contract_ids": [
            "objc3c.concurrency.async.effect.suspension.semantic.model.v1",
            "objc3c.concurrency.task.executor.cancellation.semantic.model.v1",
            "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1",
        ],
        "lowering_contract_ids": [
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ],
        "lowering_lane_contract_ids": [
            "objc3c.async.continuation.lowering.v1",
            "objc3c.await.lowering.suspension.state.lowering.v1",
            "objc3c.task.runtime.interop.cancellation.lowering.v1",
            "objc3c.concurrency.replay.race.guard.lowering.v1",
            "objc3c.actor.lowering.metadata.contract.v1",
            "objc3c.actor.isolation.sendability.lowering.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_surface_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract",
        ],
        "normalization_completion_model": (
            "normalized-async-task-actor-sema-and-lowering-packets-freeze-the-live-boundary-before-runtime-abi-and-runnable-execution-closure"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_lowering_positive.objc3",
            "tests/tooling/fixtures/native/actor_isolation_sendable_semantic_model_positive.objc3",
            "tests/tooling/fixtures/native/task_executor_cancellation_semantic_model_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-runnable-task-or-actor-execution-claim",
            "no-milestone-specific-scaffolding",
        ],
    }
    for field, expected_value in (
        expected_async_task_actor_normalization_completion_surface_fields.items()
    ):
        if async_task_actor_normalization_completion_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_async_task_actor_normalization_completion_surface drifted from {field}"
            )
    if (
        async_task_actor_normalization_completion_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
        or async_task_actor_normalization_completion_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_async_task_actor_normalization_completion_surface must require the coupled registration manifest and real compile output"
        )
    if not isinstance(unified_concurrency_lowering_metadata_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_unified_concurrency_lowering_metadata_surface"
        )
    if (
        unified_concurrency_lowering_metadata_surface.get("contract_id")
        != RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_unified_concurrency_lowering_metadata_surface contract"
        )
    expected_unified_concurrency_lowering_metadata_surface_fields = {
        "compile_manifest_artifact": manifest_path.name,
        "registration_manifest_artifact": registration_manifest_path.name,
        "registration_descriptor_artifact": registration_descriptor_path.name,
        "object_artifact": obj_path.name,
        "backend_artifact": ll_path.name,
        "source_surface_contract_id": RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
        "normalization_completion_surface_contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "lowering_contract_ids": [
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ],
        "lowering_detail_contract_ids": [
            "objc3c.concurrency.async.direct.call.lowering.v1",
            "objc3c.concurrency.task.runtime.abi.completion.v1",
            "objc3c.concurrency.actor.isolation.sendability.enforcement.v1",
        ],
        "lowering_lane_contract_ids": [
            "objc3c.async.continuation.lowering.v1",
            "objc3c.await.lowering.suspension.state.lowering.v1",
            "objc3c.task.runtime.interop.cancellation.lowering.v1",
            "objc3c.concurrency.replay.race.guard.lowering.v1",
            "objc3c.actor.lowering.metadata.contract.v1",
            "objc3c.actor.isolation.sendability.lowering.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        ],
        "authoritative_surface_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_async_function_await_and_continuation_lowering",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_runtime_abi_completion",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement",
        ],
        "lowering_metadata_surface_model": (
            "unified-concurrency-lowering-and-metadata-surface-freezes-live-async-task-actor-lowering-packets-and-emitted-metadata-boundaries-before-runtime-abi-and-runnable-execution-closure"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_lowering_positive.objc3",
            "tests/tooling/fixtures/native/task_runtime_async_entry_lowering_positive.objc3",
            "tests/tooling/fixtures/native/actor_lowering_metadata_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-runnable-task-or-actor-execution-claim",
            "no-milestone-specific-scaffolding",
        ],
    }
    for field, expected_value in (
        expected_unified_concurrency_lowering_metadata_surface_fields.items()
    ):
        if unified_concurrency_lowering_metadata_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_unified_concurrency_lowering_metadata_surface drifted from {field}"
            )
    if (
        unified_concurrency_lowering_metadata_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
        or unified_concurrency_lowering_metadata_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_unified_concurrency_lowering_metadata_surface must require the coupled registration manifest and real compile output"
        )
    if not isinstance(unified_concurrency_runtime_abi_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_unified_concurrency_runtime_abi_surface"
        )
    if (
        unified_concurrency_runtime_abi_surface.get("contract_id")
        != RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_unified_concurrency_runtime_abi_surface contract"
        )
    expected_unified_concurrency_runtime_abi_surface_fields = {
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "unified_concurrency_source_surface_contract_id": (
            RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "async_task_actor_normalization_completion_surface_contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "unified_concurrency_lowering_metadata_surface_contract_id": (
            RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID
        ),
        "async_continuation_state_snapshot_symbol": (
            "objc3_runtime_copy_async_continuation_state_for_testing"
        ),
        "task_runtime_state_snapshot_symbol": (
            "objc3_runtime_copy_task_runtime_state_for_testing"
        ),
        "actor_runtime_state_snapshot_symbol": (
            "objc3_runtime_copy_actor_runtime_state_for_testing"
        ),
        "runtime_abi_boundary_model": UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY_MODEL,
        "continuation_runtime_model": UNIFIED_CONCURRENCY_CONTINUATION_RUNTIME_MODEL,
        "task_runtime_model": UNIFIED_CONCURRENCY_TASK_RUNTIME_MODEL,
        "actor_runtime_model": UNIFIED_CONCURRENCY_ACTOR_RUNTIME_MODEL,
        "fail_closed_model": UNIFIED_CONCURRENCY_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_probe_paths": [
            CONTINUATION_RUNTIME_ABI_PROBE,
            TASK_RUNTIME_ABI_PROBE,
            ACTOR_RUNTIME_ABI_PROBE,
        ],
    }
    for field, expected_value in (
        expected_unified_concurrency_runtime_abi_surface_fields.items()
    ):
        if unified_concurrency_runtime_abi_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_unified_concurrency_runtime_abi_surface drifted from {field}"
            )
    if (
        unified_concurrency_runtime_abi_surface.get("public_runtime_abi_boundary")
        != PUBLIC_RUNTIME_ABI_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_unified_concurrency_runtime_abi_surface drifted from the public runtime ABI boundary"
        )
    if (
        unified_concurrency_runtime_abi_surface.get(
            "private_unified_concurrency_runtime_abi_boundary"
        )
        != PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_unified_concurrency_runtime_abi_surface drifted from the private concurrency runtime ABI boundary"
        )
    if (
        unified_concurrency_runtime_abi_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_unified_concurrency_runtime_abi_surface must require the coupled runtime registration manifest"
        )
    if (
        unified_concurrency_runtime_abi_surface.get("requires_real_compile_output")
        is not True
    ):
        raise RuntimeError(
            "runtime_unified_concurrency_runtime_abi_surface must require real compile output"
        )
    if (
        unified_concurrency_runtime_abi_surface.get("requires_linked_runtime_probe")
        is not True
    ):
        raise RuntimeError(
            "runtime_unified_concurrency_runtime_abi_surface must require a linked runtime probe"
        )
    if not isinstance(dispatch_and_synthesized_accessor_lowering_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish dispatch_and_synthesized_accessor_lowering_surface"
        )
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get("contract_id")
        != DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong dispatch_and_synthesized_accessor_lowering_surface contract"
        )
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface drifted from the compile manifest artifact path"
        )
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get("object_artifact")
        != obj_path.name
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface drifted from the emitted object artifact path"
        )
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get("backend_artifact")
        != ll_path.name
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_dispatch_and_synthesized_accessor_lowering_fields = {
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
        ),
        "storage_accessor_runtime_abi_surface_contract_id": (
            RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "accessor_storage_lowering_metadata_model": (
            "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path"
        ),
        "accessor_storage_lowering_helper_selection_model": (
            "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers"
        ),
        "lowering_contract_source_path": (
            "native/objc3c/src/lower/objc3_lowering_contract.h"
        ),
        "ir_emitter_source_path": "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "frontend_artifacts_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
        ),
        "runtime_source_path": "native/objc3c/src/runtime/objc3_runtime.cpp",
    }
    for field, expected_value in (
        expected_dispatch_and_synthesized_accessor_lowering_fields.items()
    ):
        if dispatch_and_synthesized_accessor_lowering_surface.get(field) != expected_value:
            raise RuntimeError(
                f"dispatch_and_synthesized_accessor_lowering_surface drifted from {field}"
            )
    expected_dispatch_and_synthesized_accessor_fixture_paths = [
        "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
        "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
        "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
    ]
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get(
            "authoritative_fixture_paths"
        )
        != expected_dispatch_and_synthesized_accessor_fixture_paths
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface drifted from authoritative_fixture_paths"
        )
    expected_dispatch_and_synthesized_accessor_probe_paths = [
        "tests/tooling/runtime/synthesized_accessor_probe.cpp",
        "tests/tooling/runtime/property_layout_runtime_probe.cpp",
        "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
    ]
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get(
            "authoritative_probe_paths"
        )
        != expected_dispatch_and_synthesized_accessor_probe_paths
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface drifted from authoritative_probe_paths"
        )
    expected_dispatch_and_synthesized_accessor_non_goals = [
        "no-public-runtime-abi-widening",
        "no-milestone-specific-scaffolding",
        "no-sidecar-only-lowering-proof",
    ]
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get("explicit_non_goals")
        != expected_dispatch_and_synthesized_accessor_non_goals
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface drifted from explicit_non_goals"
        )
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface must require the coupled runtime registration manifest"
        )
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface must require real compile output"
        )
    if (
        dispatch_and_synthesized_accessor_lowering_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "dispatch_and_synthesized_accessor_lowering_surface must require a linked runtime probe"
        )
    if not isinstance(executable_property_accessor_layout_lowering_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish executable_property_accessor_layout_lowering_surface"
        )
    expected_executable_property_accessor_layout_lowering_fields = {
        "contract_id": EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": manifest_path.name,
        "registration_manifest_artifact": registration_manifest_path.name,
        "object_artifact": obj_path.name,
        "backend_artifact": ll_path.name,
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID
        ),
        "property_table_model": (
            "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records"
        ),
        "ivar_layout_model": (
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-slot-size-alignment-records"
        ),
        "accessor_binding_model": (
            "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis"
        ),
        "scope_model": (
            "ast-sema-property-layout-handoff-ir-object-metadata-publication"
        ),
        "fail_closed_model": (
            "no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation"
        ),
        "lowering_contract_source_path": (
            "native/objc3c/src/lower/objc3_lowering_contract.h"
        ),
        "ir_emitter_source_path": "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "frontend_artifacts_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
        ),
    }
    for field, expected_value in (
        expected_executable_property_accessor_layout_lowering_fields.items()
    ):
        if (
            executable_property_accessor_layout_lowering_surface.get(field)
            != expected_value
        ):
            raise RuntimeError(
                f"executable_property_accessor_layout_lowering_surface drifted from {field}"
            )
    expected_executable_accessor_layout_fixture_paths = [
        "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
        "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
    ]
    if (
        executable_property_accessor_layout_lowering_surface.get(
            "authoritative_fixture_paths"
        )
        != expected_executable_accessor_layout_fixture_paths
    ):
        raise RuntimeError(
            "executable_property_accessor_layout_lowering_surface drifted from authoritative_fixture_paths"
        )
    expected_executable_accessor_layout_probe_paths = [
        "tests/tooling/runtime/synthesized_accessor_probe.cpp",
        "tests/tooling/runtime/property_layout_runtime_probe.cpp",
        "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
    ]
    if (
        executable_property_accessor_layout_lowering_surface.get(
            "authoritative_probe_paths"
        )
        != expected_executable_accessor_layout_probe_paths
    ):
        raise RuntimeError(
            "executable_property_accessor_layout_lowering_surface drifted from authoritative_probe_paths"
        )
    if (
        executable_property_accessor_layout_lowering_surface.get("explicit_non_goals")
        != [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-layout-or-accessor-body-rederivation-outside-the-live-lowering-path",
        ]
    ):
        raise RuntimeError(
            "executable_property_accessor_layout_lowering_surface drifted from explicit_non_goals"
        )
    if (
        executable_property_accessor_layout_lowering_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
        or executable_property_accessor_layout_lowering_surface.get(
            "requires_real_compile_output"
        )
        is not True
        or executable_property_accessor_layout_lowering_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "executable_property_accessor_layout_lowering_surface must require the coupled registration manifest, real compile output, and linked runtime probes"
        )
    if not isinstance(executable_ivar_layout_emission_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish executable_ivar_layout_emission_surface"
        )
    expected_executable_ivar_layout_emission_fields = {
        "contract_id": EXECUTABLE_IVAR_LAYOUT_EMISSION_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": manifest_path.name,
        "registration_manifest_artifact": registration_manifest_path.name,
        "object_artifact": obj_path.name,
        "backend_artifact": ll_path.name,
        "executable_property_accessor_layout_lowering_surface_contract_id": (
            EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID
        ),
        "descriptor_model": (
            "ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment"
        ),
        "offset_global_model": (
            "one-retained-i64-offset-global-per-emitted-ivar-binding"
        ),
        "layout_table_model": (
            "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size"
        ),
        "scope_model": (
            "sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation"
        ),
        "fail_closed_model": (
            "no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis"
        ),
        "lowering_contract_source_path": (
            "native/objc3c/src/lower/objc3_lowering_contract.h"
        ),
        "ir_emitter_source_path": "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "frontend_artifacts_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
        ),
    }
    for field, expected_value in (
        expected_executable_ivar_layout_emission_fields.items()
    ):
        if executable_ivar_layout_emission_surface.get(field) != expected_value:
            raise RuntimeError(
                f"executable_ivar_layout_emission_surface drifted from {field}"
            )
    if (
        executable_ivar_layout_emission_surface.get("authoritative_fixture_paths")
        != [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        ]
    ):
        raise RuntimeError(
            "executable_ivar_layout_emission_surface drifted from authoritative_fixture_paths"
        )
    if (
        executable_ivar_layout_emission_surface.get("authoritative_probe_paths")
        != [
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        ]
    ):
        raise RuntimeError(
            "executable_ivar_layout_emission_surface drifted from authoritative_probe_paths"
        )
    if (
        executable_ivar_layout_emission_surface.get("explicit_non_goals")
        != [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-runtime-layout-rederivation",
        ]
    ):
        raise RuntimeError(
            "executable_ivar_layout_emission_surface drifted from explicit_non_goals"
        )
    if (
        executable_ivar_layout_emission_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
        or executable_ivar_layout_emission_surface.get("requires_real_compile_output")
        is not True
        or executable_ivar_layout_emission_surface.get("requires_linked_runtime_probe")
        is not True
    ):
        raise RuntimeError(
            "executable_ivar_layout_emission_surface must require the coupled registration manifest, real compile output, and linked runtime probes"
        )
    if not isinstance(
        executable_synthesized_accessor_property_lowering_surface, dict
    ):
        raise RuntimeError(
            "compiled fixture manifest did not publish executable_synthesized_accessor_property_lowering_surface"
        )
    expected_executable_synthesized_accessor_property_lowering_fields = {
        "contract_id": (
            EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_LOWERING_SURFACE_CONTRACT_ID
        ),
        "compile_manifest_artifact": manifest_path.name,
        "registration_manifest_artifact": registration_manifest_path.name,
        "object_artifact": obj_path.name,
        "backend_artifact": ll_path.name,
        "executable_property_accessor_layout_lowering_surface_contract_id": (
            EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID
        ),
        "source_model": (
            "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists"
        ),
        "storage_model": (
            "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals"
        ),
        "property_descriptor_model": (
            "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers"
        ),
        "fail_closed_model": (
            "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-storage-global-fallbacks"
        ),
        "lowering_contract_source_path": (
            "native/objc3c/src/lower/objc3_lowering_contract.h"
        ),
        "ir_emitter_source_path": "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "frontend_artifacts_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
        ),
        "runtime_source_path": "native/objc3c/src/runtime/objc3_runtime.cpp",
    }
    for field, expected_value in (
        expected_executable_synthesized_accessor_property_lowering_fields.items()
    ):
        if (
            executable_synthesized_accessor_property_lowering_surface.get(field)
            != expected_value
        ):
            raise RuntimeError(
                f"executable_synthesized_accessor_property_lowering_surface drifted from {field}"
            )
    if (
        executable_synthesized_accessor_property_lowering_surface.get(
            "authoritative_fixture_paths"
        )
        != expected_executable_accessor_layout_fixture_paths
    ):
        raise RuntimeError(
            "executable_synthesized_accessor_property_lowering_surface drifted from authoritative_fixture_paths"
        )
    if (
        executable_synthesized_accessor_property_lowering_surface.get(
            "authoritative_probe_paths"
        )
        != expected_executable_accessor_layout_probe_paths
    ):
        raise RuntimeError(
            "executable_synthesized_accessor_property_lowering_surface drifted from authoritative_probe_paths"
        )
    if (
        executable_synthesized_accessor_property_lowering_surface.get(
            "explicit_non_goals"
        )
        != [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-storage-global-fallbacks-or-sidecar-body-proof",
        ]
    ):
        raise RuntimeError(
            "executable_synthesized_accessor_property_lowering_surface drifted from explicit_non_goals"
        )
    if (
        executable_synthesized_accessor_property_lowering_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
        or executable_synthesized_accessor_property_lowering_surface.get(
            "requires_real_compile_output"
        )
        is not True
        or executable_synthesized_accessor_property_lowering_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "executable_synthesized_accessor_property_lowering_surface must require the coupled registration manifest, real compile output, and linked runtime probes"
        )
    if not isinstance(property_ivar_storage_accessor_source_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_property_ivar_storage_accessor_source_surface"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("contract_id")
        != RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_property_ivar_storage_accessor_source_surface contract"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the compile manifest artifact path"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("object_artifact")
        != obj_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the emitted object artifact path"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("backend_artifact")
        != ll_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_property_ivar_storage_accessor_source_fields = {
        "property_ast_anchor": "Objc3PropertyDecl",
        "ivar_ast_anchor": "Objc3PropertyDecl.ivar_binding_symbol",
        "source_closure_contract_id": "objc3c.executable.property.ivar.source.closure.v1",
        "source_model_completion_contract_id": (
            "objc3c.executable.property.ivar.source.model.completion.v1"
        ),
        "source_semantics_contract_id": "objc3c.executable.property.ivar.semantics.v1",
        "source_surface_model": (
            "property-ivar-storage-accessor-runtime-source-surface-freezes-ast-sema-ir-pipeline-and-runtime-codepaths-before-lowering-or-runtime-semantic-expansion"
        ),
        "layout_model": (
            "property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization"
        ),
        "attribute_model": (
            "property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles"
        ),
        "synthesis_semantics_model": (
            "non-category-class-interface-properties-own-authoritative-default-ivar-and-synthesized-binding-identities-across-implementation-redeclaration-boundaries"
        ),
        "default_ivar_binding_resolution_model": (
            "matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration"
        ),
        "accessor_semantics_model": (
            "readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission"
        ),
        "accessor_selector_uniqueness_model": (
            "effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding"
        ),
        "ownership_atomicity_interaction_model": (
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land"
        ),
        "storage_semantics_model": (
            "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation"
        ),
        "layout_init_order_field": "Objc3PropertyDecl.executable_ivar_init_order_index",
        "layout_destroy_order_field": "Objc3PropertyDecl.executable_ivar_destroy_order_index",
        "synthesizes_accessors_field": (
            "Objc3RuntimeMetadataPropertySourceRecord.synthesizes_executable_accessors"
        ),
        "getter_runtime_helper_field": (
            "Objc3RuntimeMetadataPropertySourceRecord.getter_storage_runtime_helper_symbol"
        ),
        "setter_runtime_helper_field": (
            "Objc3RuntimeMetadataPropertySourceRecord.setter_storage_runtime_helper_symbol"
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "executable_property_accessor_layout_lowering_contract_id": (
            "objc3c.executable.property.accessor.layout.lowering.v1"
        ),
        "executable_ivar_layout_emission_contract_id": (
            "objc3c.executable.ivar.layout.emission.v1"
        ),
        "executable_synthesized_accessor_property_lowering_contract_id": (
            "objc3c.executable.synthesized.accessor.property.lowering.v1"
        ),
        "storage_accessor_runtime_abi_surface_contract_id": (
            RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "accessor_storage_lowering_metadata_model": (
            "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path"
        ),
        "accessor_storage_lowering_helper_selection_model": (
            "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers"
        ),
        "compatibility_semantics_model": (
            "protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols"
        ),
        "ast_source_path": "native/objc3c/src/ast/objc3_ast.h",
        "lowering_contract_source_path": (
            "native/objc3c/src/lower/objc3_lowering_contract.h"
        ),
        "sema_source_path": "native/objc3c/src/sema/objc3_semantic_passes.cpp",
        "frontend_pipeline_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
        ),
        "ir_emitter_source_path": "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "frontend_artifacts_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
        ),
        "runtime_source_path": "native/objc3c/src/runtime/objc3_runtime.cpp",
    }
    for field, expected_value in expected_property_ivar_storage_accessor_source_fields.items():
        if property_ivar_storage_accessor_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_property_ivar_storage_accessor_source_surface drifted from {field}"
            )
    expected_authoritative_fixture_paths = [
        "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
        "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
        "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
        "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
    ]
    if (
        property_ivar_storage_accessor_source_surface.get("authoritative_fixture_paths")
        != expected_authoritative_fixture_paths
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from authoritative_fixture_paths"
        )
    expected_authoritative_probe_paths = [
        "tests/tooling/runtime/synthesized_accessor_probe.cpp",
        "tests/tooling/runtime/property_layout_runtime_probe.cpp",
        "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
    ]
    if (
        property_ivar_storage_accessor_source_surface.get("authoritative_probe_paths")
        != expected_authoritative_probe_paths
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from authoritative_probe_paths"
        )
    expected_explicit_non_goals = [
        "no-public-runtime-abi-widening",
        "no-milestone-specific-scaffolding",
        "no-lowering-owned-storage-or-accessor-semantics-invention",
    ]
    if (
        property_ivar_storage_accessor_source_surface.get("explicit_non_goals")
        != expected_explicit_non_goals
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from explicit_non_goals"
        )
    if (
        property_ivar_storage_accessor_source_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface must require the coupled runtime registration manifest"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("requires_real_compile_output")
        is not True
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface must require real compile output"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("requires_linked_runtime_probe")
        is not True
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface must require a linked runtime probe"
        )
    if not isinstance(block_arc_unified_source_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_block_arc_unified_source_surface"
        )
    if (
        block_arc_unified_source_surface.get("contract_id")
        != RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_block_arc_unified_source_surface contract"
        )
    if (
        block_arc_unified_source_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from the compile manifest artifact path"
        )
    if (
        block_arc_unified_source_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        block_arc_unified_source_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if block_arc_unified_source_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from the emitted object artifact path"
        )
    if block_arc_unified_source_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_block_arc_unified_source_fields = {
        "source_surface_model": (
            "block-arc-unified-source-surface-freezes-live-frontend-sema-ir-and-runtime-entrypoints-before-generalized-ownership-automation-or-public-abi-widening"
        ),
        "block_runtime_boundary_model": (
            "source-only-sema-rejects-escaping-byref-and-owned-object-captures-before-runnable-block-ownership-lowering"
        ),
        "arc_runtime_boundary_model": (
            "weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-stay-profiled-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc"
        ),
    }
    for field, expected_value in expected_block_arc_unified_source_fields.items():
        if block_arc_unified_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_block_arc_unified_source_surface drifted from {field}"
            )
    expected_block_arc_source_contract_ids = [
        "objc3c.executable.block.source.closure.v1",
        "objc3c.executable.block.source.model.completion.v1",
        "objc3c.executable.block.source.storage.annotation.v1",
        "objc3c.executable.block.runtime.semantic.rules.v1",
        "objc3c.executable.block.capture.legality.escape.and.invocation.v1",
        "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1",
        "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
        "objc3c.executable.block.byref.helper.lowering.v1",
        "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        "objc3c.arc.source.mode.boundary.freeze.v1",
        "objc3c.arc.mode.handling.v1",
        "objc3c.arc.semantic.rules.v1",
        "objc3c.arc.inference.lifetime.v1",
        "objc3c.arc.interaction.semantics.v1",
    ]
    if (
        block_arc_unified_source_surface.get("source_contract_ids")
        != expected_block_arc_source_contract_ids
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from source_contract_ids"
        )
    expected_block_arc_authoritative_code_paths = [
        "native/objc3c/src/ast/objc3_ast.h",
        "native/objc3c/src/sema/objc3_semantic_passes.cpp",
        "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
        "native/objc3c/src/runtime/objc3_runtime.cpp",
    ]
    if (
        block_arc_unified_source_surface.get("authoritative_code_paths")
        != expected_block_arc_authoritative_code_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from authoritative_code_paths"
        )
    expected_block_arc_authoritative_source_fields = [
        "frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface",
        "frontend.pipeline.semantic_surface.objc_block_literal_capture_lowering_surface",
        "frontend.pipeline.semantic_surface.objc_block_source_model_completion_surface",
        "frontend.pipeline.semantic_surface.objc_block_source_storage_annotation_surface",
        "frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface",
        "frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface",
        "frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface",
        "llvm_ir_summary.executable_block_object_invoke_thunk_lowering",
        "llvm_ir_summary.executable_block_byref_helper_lowering",
        "llvm_ir_summary.executable_block_escape_runtime_hook_lowering",
        "llvm_ir_summary.runtime_block_api_object_layout",
        "llvm_ir_summary.runtime_block_allocation_copy_dispose_invoke_support",
        "llvm_ir_summary.runtime_block_byref_forwarding_heap_promotion_ownership_interop",
        "runtime_api.objc3_runtime_promote_block_i32",
        "runtime_api.objc3_runtime_invoke_block_i32",
        "runtime_api.objc3_runtime_copy_arc_debug_state_for_testing",
        "runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
    ]
    if (
        block_arc_unified_source_surface.get("authoritative_source_fields")
        != expected_block_arc_authoritative_source_fields
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from authoritative_source_fields"
        )
    expected_block_arc_authoritative_fixture_paths = [
        "tests/tooling/fixtures/native/block_source_model_completion_positive.objc3",
        "tests/tooling/fixtures/native/block_source_storage_annotations_positive.objc3",
        "tests/tooling/fixtures/native/capture_legality_escape_invocation_bad_call.objc3",
        "tests/tooling/fixtures/native/capture_legality_escape_invocation_missing_capture.objc3",
        "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
        "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
        "tests/tooling/fixtures/native/arc_mode_handling_positive.objc3",
        "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
        "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
        "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
        "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
        "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
    ]
    if (
        block_arc_unified_source_surface.get("authoritative_fixture_paths")
        != expected_block_arc_authoritative_fixture_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from authoritative_fixture_paths"
        )
    expected_block_arc_authoritative_probe_paths = [
        "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
        "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
    ]
    if (
        block_arc_unified_source_surface.get("authoritative_probe_paths")
        != expected_block_arc_authoritative_probe_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from authoritative_probe_paths"
        )
    expected_block_arc_explicit_non_goals = [
        "no-public-block-object-abi-widening",
        "no-milestone-specific-scaffolding",
        "no-sidecar-only-block-or-arc-proof",
    ]
    if (
        block_arc_unified_source_surface.get("explicit_non_goals")
        != expected_block_arc_explicit_non_goals
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface drifted from explicit_non_goals"
        )
    if (
        block_arc_unified_source_surface.get("requires_coupled_registration_manifest")
        is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface must require the coupled runtime registration manifest"
        )
    if (
        block_arc_unified_source_surface.get("requires_real_compile_output") is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface must require real compile output"
        )
    if (
        block_arc_unified_source_surface.get("requires_linked_runtime_probe") is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_unified_source_surface must require a linked runtime probe"
        )
    if not isinstance(ownership_transfer_capture_family_source_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_ownership_transfer_capture_family_source_surface"
        )
    if (
        ownership_transfer_capture_family_source_surface.get("contract_id")
        != RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_ownership_transfer_capture_family_source_surface contract"
        )
    if (
        ownership_transfer_capture_family_source_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from the compile manifest artifact path"
        )
    if (
        ownership_transfer_capture_family_source_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        ownership_transfer_capture_family_source_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        ownership_transfer_capture_family_source_surface.get("object_artifact")
        != obj_path.name
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from the emitted object artifact path"
        )
    if (
        ownership_transfer_capture_family_source_surface.get("backend_artifact")
        != ll_path.name
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_ownership_transfer_capture_family_fields = {
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "source_surface_model": (
            "ownership-transfer-and-capture-family-source-surface-freezes-sema-level-move-capture-explicit-capture-mode-and-retainable-family-truth-before-lowering-or-runtime-lifetime-expansion"
        ),
        "ownership_resource_move_use_after_move_surface_path": (
            "frontend.pipeline.semantic_surface.objc_ownership_resource_move_and_use_after_move_semantics"
        ),
        "ownership_capture_list_retainable_family_surface_path": (
            "frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion"
        ),
        "block_capture_ownership_contract_id": (
            "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1"
        ),
        "arc_inference_lifetime_contract_id": "objc3c.arc.inference.lifetime.v1",
        "arc_interaction_semantics_contract_id": "objc3c.arc.interaction.semantics.v1",
        "block_capture_ownership_profile_field": (
            "Expr.block_runtime_capture_ownership_profile"
        ),
        "block_capture_owned_count_field": (
            "Expr.block_runtime_owned_object_capture_count"
        ),
        "block_capture_weak_count_field": (
            "Expr.block_runtime_weak_object_capture_count"
        ),
        "block_capture_unowned_count_field": (
            "Expr.block_runtime_unowned_object_capture_count"
        ),
        "cleanup_ownership_transfer_field": "cleanup_ownership_transfer_enforced",
        "explicit_capture_ownership_mode_field": (
            "explicit_capture_ownership_mode_enforced"
        ),
        "retainable_family_conflict_field": "retainable_family_conflict_enforced",
    }
    for field, expected_value in expected_ownership_transfer_capture_family_fields.items():
        if ownership_transfer_capture_family_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_ownership_transfer_capture_family_source_surface drifted from {field}"
            )
    expected_ownership_transfer_capture_family_code_paths = [
        "native/objc3c/src/ast/objc3_ast.h",
        "native/objc3c/src/sema/objc3_semantic_passes.cpp",
        "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        "native/objc3c/src/ir/objc3_ir_emitter.cpp",
    ]
    if (
        ownership_transfer_capture_family_source_surface.get("authoritative_code_paths")
        != expected_ownership_transfer_capture_family_code_paths
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from authoritative_code_paths"
        )
    expected_ownership_transfer_capture_family_fixture_paths = [
        "tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3",
        "tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3",
        "tests/tooling/fixtures/native/nonowning_object_capture_helper_elided_positive.objc3",
        "tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3",
        "tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3",
        "tests/tooling/fixtures/native/unowned_object_capture_mutation_negative.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
        "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
        "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
        "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
        "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
        "tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
    ]
    if (
        ownership_transfer_capture_family_source_surface.get(
            "authoritative_fixture_paths"
        )
        != expected_ownership_transfer_capture_family_fixture_paths
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from authoritative_fixture_paths"
        )
    expected_ownership_transfer_capture_family_probe_paths = [
        "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
        "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
    ]
    if (
        ownership_transfer_capture_family_source_surface.get("authoritative_probe_paths")
        != expected_ownership_transfer_capture_family_probe_paths
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from authoritative_probe_paths"
        )
    expected_ownership_transfer_capture_family_non_goals = [
        "no-parallel-semantics-path",
        "no-milestone-specific-scaffolding",
        "no-lowering-owned-reinterpretation-of-capture-family-truth",
    ]
    if (
        ownership_transfer_capture_family_source_surface.get("explicit_non_goals")
        != expected_ownership_transfer_capture_family_non_goals
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface drifted from explicit_non_goals"
        )
    if (
        ownership_transfer_capture_family_source_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface must require the coupled runtime registration manifest"
        )
    if (
        ownership_transfer_capture_family_source_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface must require real compile output"
        )
    if (
        ownership_transfer_capture_family_source_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_ownership_transfer_capture_family_source_surface must require a linked runtime probe"
        )
    if not isinstance(block_arc_lowering_helper_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_block_arc_lowering_helper_surface"
        )
    if (
        block_arc_lowering_helper_surface.get("contract_id")
        != RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_block_arc_lowering_helper_surface contract"
        )
    if (
        block_arc_lowering_helper_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from the compile manifest artifact path"
        )
    if (
        block_arc_lowering_helper_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        block_arc_lowering_helper_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from the runtime registration descriptor artifact path"
        )
    if block_arc_lowering_helper_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from the emitted object artifact path"
        )
    if block_arc_lowering_helper_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_block_arc_lowering_helper_fields = {
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_block_arc_runtime_abi_surface_contract_id": (
            RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "ownership_transfer_capture_family_source_surface_contract_id": (
            RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "block_object_invoke_thunk_lowering_contract_id": (
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1"
        ),
        "block_byref_helper_lowering_contract_id": (
            "objc3c.executable.block.byref.helper.lowering.v1"
        ),
        "block_escape_runtime_hook_lowering_contract_id": (
            "objc3c.executable.block.escape.runtime.hook.lowering.v1"
        ),
        "arc_mode_handling_contract_id": "objc3c.arc.mode.handling.v1",
        "arc_semantic_rules_contract_id": "objc3c.arc.semantic.rules.v1",
        "arc_inference_lifetime_contract_id": "objc3c.arc.inference.lifetime.v1",
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "lowering_helper_surface_model": (
            "block-arc-lowering-helper-surface-freezes-live-semantic-lowering-packets-manifest-replay-keys-llvm-helper-summaries-and-private-runtime-hooks-before-cross-module-or-public-abi-expansion"
        ),
    }
    for field, expected_value in expected_block_arc_lowering_helper_fields.items():
        if block_arc_lowering_helper_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_block_arc_lowering_helper_surface drifted from {field}"
            )
    expected_block_arc_lowering_helper_semantic_surface_paths = [
        "frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface",
        "frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface",
        "frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface",
        "frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface",
    ]
    if (
        block_arc_lowering_helper_surface.get("semantic_surface_paths")
        != expected_block_arc_lowering_helper_semantic_surface_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from semantic_surface_paths"
        )
    expected_block_arc_lowering_helper_manifest_lowering_paths = [
        "lowering_block_abi_invoke_trampoline",
        "lowering_block_storage_escape",
        "lowering_block_copy_dispose",
    ]
    if (
        block_arc_lowering_helper_surface.get("manifest_lowering_paths")
        != expected_block_arc_lowering_helper_manifest_lowering_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from manifest_lowering_paths"
        )
    expected_block_arc_lowering_helper_llvm_ir_summary_paths = [
        "llvm_ir_summary.executable_block_object_invoke_thunk_lowering",
        "llvm_ir_summary.executable_block_byref_helper_lowering",
        "llvm_ir_summary.executable_block_escape_runtime_hook_lowering",
        "llvm_ir_summary.arc_cleanup_weak_lifetime_hooks",
        "llvm_ir_summary.arc_block_autorelease_return_lowering",
    ]
    if (
        block_arc_lowering_helper_surface.get("llvm_ir_summary_paths")
        != expected_block_arc_lowering_helper_llvm_ir_summary_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from llvm_ir_summary_paths"
        )
    expected_block_arc_lowering_helper_runtime_api_paths = [
        "runtime_api.objc3_runtime_promote_block_i32",
        "runtime_api.objc3_runtime_invoke_block_i32",
        "runtime_api.objc3_runtime_copy_arc_debug_state_for_testing",
        "runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
    ]
    if (
        block_arc_lowering_helper_surface.get("runtime_api_paths")
        != expected_block_arc_lowering_helper_runtime_api_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from runtime_api_paths"
        )
    expected_block_arc_lowering_helper_code_paths = [
        "native/objc3c/src/ast/objc3_ast.h",
        "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
        "native/objc3c/src/runtime/objc3_runtime.cpp",
    ]
    if (
        block_arc_lowering_helper_surface.get("authoritative_code_paths")
        != expected_block_arc_lowering_helper_code_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from authoritative_code_paths"
        )
    expected_block_arc_lowering_helper_fixture_paths = [
        "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
        "tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3",
        "tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
        "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
        "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
        "tests/tooling/fixtures/native/arc_mode_handling_positive.objc3",
        "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
        "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
        "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
        "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
    ]
    if (
        block_arc_lowering_helper_surface.get("authoritative_fixture_paths")
        != expected_block_arc_lowering_helper_fixture_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from authoritative_fixture_paths"
        )
    expected_block_arc_lowering_helper_probe_paths = [
        "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
        "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
    ]
    if (
        block_arc_lowering_helper_surface.get("authoritative_probe_paths")
        != expected_block_arc_lowering_helper_probe_paths
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from authoritative_probe_paths"
        )
    expected_block_arc_lowering_helper_non_goals = [
        "no-cross-module-packaging-claims",
        "no-public-block-abi-widening",
        "no-milestone-specific-scaffolding",
    ]
    if (
        block_arc_lowering_helper_surface.get("explicit_non_goals")
        != expected_block_arc_lowering_helper_non_goals
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface drifted from explicit_non_goals"
        )
    if (
        block_arc_lowering_helper_surface.get("requires_coupled_registration_manifest")
        is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface must require the coupled runtime registration manifest"
        )
    if (
        block_arc_lowering_helper_surface.get("requires_real_compile_output")
        is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface must require real compile output"
        )
    if (
        block_arc_lowering_helper_surface.get("requires_linked_runtime_probe")
        is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_lowering_helper_surface must require a linked runtime probe"
        )
    if not isinstance(block_arc_runtime_abi_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_block_arc_runtime_abi_surface"
        )
    if (
        block_arc_runtime_abi_surface.get("contract_id")
        != RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_block_arc_runtime_abi_surface contract"
        )
    expected_block_arc_runtime_abi_surface_fields = {
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "block_arc_lowering_helper_surface_contract_id": (
            RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID
        ),
        "block_arc_runtime_abi_snapshot_symbol": (
            "objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing"
        ),
        "arc_debug_state_snapshot_symbol": (
            "objc3_runtime_copy_arc_debug_state_for_testing"
        ),
        "runtime_abi_boundary_model": BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
        "block_runtime_model": BLOCK_ARC_RUNTIME_BLOCK_MODEL,
        "arc_runtime_model": BLOCK_ARC_RUNTIME_ARC_MODEL,
        "fail_closed_model": BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_probe_path": BLOCK_ARC_RUNTIME_ABI_PROBE,
    }
    for field, expected_value in expected_block_arc_runtime_abi_surface_fields.items():
        if block_arc_runtime_abi_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_block_arc_runtime_abi_surface drifted from {field}"
            )
    if (
        block_arc_runtime_abi_surface.get("public_runtime_abi_boundary")
        != PUBLIC_RUNTIME_ABI_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_block_arc_runtime_abi_surface drifted from the public runtime ABI boundary"
        )
    if (
        block_arc_runtime_abi_surface.get("private_block_arc_runtime_abi_boundary")
        != PRIVATE_BLOCK_ARC_RUNTIME_ABI_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_block_arc_runtime_abi_surface drifted from the private block ARC runtime ABI boundary"
        )
    if (
        block_arc_runtime_abi_surface.get("requires_coupled_registration_manifest")
        is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_runtime_abi_surface must require the coupled runtime registration manifest"
        )
    if (
        block_arc_runtime_abi_surface.get("requires_real_compile_output")
        is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_runtime_abi_surface must require real compile output"
        )
    if (
        block_arc_runtime_abi_surface.get("requires_linked_runtime_probe")
        is not True
    ):
        raise RuntimeError(
            "runtime_block_arc_runtime_abi_surface must require a linked runtime probe"
        )
    if not isinstance(property_atomicity_synthesis_reflection_source_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_property_atomicity_synthesis_reflection_source_surface"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("contract_id")
        != RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_property_atomicity_synthesis_reflection_source_surface contract"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the compile manifest artifact path"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("object_artifact")
        != obj_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the emitted object artifact path"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("backend_artifact")
        != ll_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_property_atomicity_synthesis_reflection_fields = {
        "property_storage_source_surface_contract_id": (
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
        ),
        "property_metadata_reflection_contract_id": "objc3c.runtime.property.metadata.reflection.v1",
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "atomic_modifier_field": "Objc3PropertyDecl.is_atomic",
        "nonatomic_modifier_field": "Objc3PropertyDecl.is_nonatomic",
        "atomicity_conflict_field": "Objc3PropertyDecl.has_atomicity_conflict",
        "property_attribute_profile_field": "Objc3PropertyDecl.property_attribute_profile",
        "reflection_attribute_profile_field": (
            "objc3_runtime_property_entry_snapshot.property_attribute_profile"
        ),
        "ast_source_path": "native/objc3c/src/ast/objc3_ast.h",
        "sema_source_path": "native/objc3c/src/sema/objc3_semantic_passes.cpp",
        "sema_pass_manager_source_path": "native/objc3c/src/sema/objc3_sema_pass_manager.cpp",
        "frontend_pipeline_source_path": "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        "frontend_artifacts_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
        ),
        "runtime_internal_header_path": (
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
        ),
        "runtime_source_path": "native/objc3c/src/runtime/objc3_runtime.cpp",
        "source_surface_model": (
            "property-atomicity-synthesis-reflection-source-surface-freezes-atomicity-flags-conflict-state-attribute-profiles-and-private-reflection-codepaths-before-runtime-managed-atomic-storage-semantics-land"
        ),
        "atomicity_fail_closed_model": (
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land"
        ),
        "reflection_boundary_model": (
            "property-attribute-profiles-remain-the-authoritative-reflection-carrier-for-atomicity-and-synthesis-state-on-the-private-property-query-boundary"
        ),
    }
    for field, expected_value in expected_property_atomicity_synthesis_reflection_fields.items():
        if property_atomicity_synthesis_reflection_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_property_atomicity_synthesis_reflection_source_surface drifted from {field}"
            )
    expected_atomicity_authoritative_fixture_paths = [
        "tests/tooling/fixtures/native/property_atomic_ownership_negative.objc3",
        "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
        "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
        "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
        "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
    ]
    if (
        property_atomicity_synthesis_reflection_source_surface.get(
            "authoritative_fixture_paths"
        )
        != expected_atomicity_authoritative_fixture_paths
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from authoritative_fixture_paths"
        )
    expected_atomicity_authoritative_probe_paths = [
        "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
    ]
    if (
        property_atomicity_synthesis_reflection_source_surface.get(
            "authoritative_probe_paths"
        )
        != expected_atomicity_authoritative_probe_paths
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from authoritative_probe_paths"
        )
    expected_atomicity_explicit_non_goals = [
        "no-public-atomic-property-runtime-abi-widening",
        "no-runtime-managed-atomic-storage-semantics-before-lane-b-and-lane-d-implementation",
        "no-milestone-specific-scaffolding",
    ]
    if (
        property_atomicity_synthesis_reflection_source_surface.get("explicit_non_goals")
        != expected_atomicity_explicit_non_goals
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from explicit_non_goals"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface must require the coupled runtime registration manifest"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface must require real compile output"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface must require a linked runtime probe"
        )
    if not isinstance(realization_lowering_reflection_artifact_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_realization_lowering_reflection_artifact_surface"
        )
    if (
        realization_lowering_reflection_artifact_surface.get("contract_id")
        != RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_realization_lowering_reflection_artifact_surface contract"
        )
    if (
        realization_lowering_reflection_artifact_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the compile manifest artifact path"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "registration_manifest_artifact"
        )
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "registration_descriptor_artifact"
        )
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the runtime registration descriptor artifact path"
        )
    if realization_lowering_reflection_artifact_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the emitted object artifact path"
        )
    if realization_lowering_reflection_artifact_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_realization_lowering_reflection_artifact_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "executable_realization_records_contract_id": (
            "objc3c.executable.realization.records.v1"
        ),
        "property_metadata_reflection_contract_id": (
            "objc3c.runtime.property.metadata.reflection.v1"
        ),
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "runtime_support_library_archive_relative_path": (
            registration_manifest.get("runtime_support_library_archive_relative_path")
        ),
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "lowering_artifact_boundary_model": (
            "compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts"
        ),
        "reflection_artifact_handoff_model": (
            "property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs"
        ),
    }
    for field, expected_value in expected_realization_lowering_reflection_artifact_fields.items():
        if realization_lowering_reflection_artifact_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_realization_lowering_reflection_artifact_surface drifted from {field}"
            )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "requires_coupled_registration_descriptor_artifact"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface must require the coupled runtime registration descriptor artifact"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface must require the coupled runtime registration manifest"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface must require real compile output"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "requires_compile_output_truthfulness"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface must require compile output truthfulness"
        )
    if not isinstance(dispatch_table_reflection_record_lowering_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_dispatch_table_reflection_record_lowering_surface"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        != RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_dispatch_table_reflection_record_lowering_surface contract"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the compile manifest artifact path"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the runtime registration descriptor artifact path"
        )
    if dispatch_table_reflection_record_lowering_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the emitted object artifact path"
        )
    if dispatch_table_reflection_record_lowering_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the emitted LLVM IR artifact path"
        )
    dispatch_lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expected_dispatch_table_reflection_record_lowering_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_state_publication_surface_contract_id": (
            RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "method_dispatch_and_selector_thunk_lowering_contract_id": (
            "objc3c.method.dispatch.selector.thunk.lowering.v1"
        ),
        "executable_realization_records_contract_id": (
            "objc3c.executable.realization.records.v1"
        ),
        "runtime_support_library_archive_relative_path": (
            registration_manifest.get("runtime_support_library_archive_relative_path")
        ),
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "selector_pool_section_root_symbol": "@__objc3_sec_selector_pool",
        "class_aggregate_symbol": "__objc3_sec_class_descriptors",
        "protocol_aggregate_symbol": "__objc3_sec_protocol_descriptors",
        "category_aggregate_symbol": "__objc3_sec_category_descriptors",
        "property_aggregate_symbol": "__objc3_sec_property_descriptors",
        "ivar_aggregate_symbol": "__objc3_sec_ivar_descriptors",
        "message_send_sites": dispatch_lowering_surface.get("message_send_sites"),
        "class_descriptor_count": registration_manifest.get("class_descriptor_count"),
        "protocol_descriptor_count": registration_manifest.get("protocol_descriptor_count"),
        "category_descriptor_count": registration_manifest.get("category_descriptor_count"),
        "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
        "dispatch_table_lowering_model": (
            "selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts"
        ),
        "reflection_record_lowering_model": (
            "realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts"
        ),
    }
    for field, expected_value in expected_dispatch_table_reflection_record_lowering_fields.items():
        if dispatch_table_reflection_record_lowering_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_dispatch_table_reflection_record_lowering_surface drifted from {field}"
            )
    if (
        dispatch_table_reflection_record_lowering_surface.get(
            "requires_coupled_registration_descriptor_artifact"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface must require the coupled runtime registration descriptor artifact"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface must require the coupled runtime registration manifest"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("requires_real_compile_output")
        is not True
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface must require real compile output"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get(
            "requires_compile_output_truthfulness"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface must require compile output truthfulness"
        )
    if not isinstance(object_model_abi_query_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_object_model_abi_query_surface"
        )
    if (
        object_model_abi_query_surface.get("contract_id")
        != RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_object_model_abi_query_surface contract"
        )
    if object_model_abi_query_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the compile manifest artifact path"
        )
    if (
        object_model_abi_query_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        object_model_abi_query_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the runtime registration descriptor artifact path"
        )
    if object_model_abi_query_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the emitted object artifact path"
        )
    if object_model_abi_query_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_object_model_abi_query_surface_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_realization_lowering_reflection_artifact_surface_contract_id": (
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID
        ),
        "runtime_dispatch_table_reflection_record_lowering_surface_contract_id": (
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID
        ),
        "runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id": (
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_realization_lookup_semantics_surface_contract_id": (
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "runtime_class_metaclass_protocol_realization_surface_contract_id": (
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
        ),
        "runtime_category_attachment_merged_dispatch_surface_contract_id": (
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface_contract_id": (
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "object_model_query_boundary_model": (
            "public-runtime-header-plus-private-testing-snapshots-freeze-the-object-model-lookup-and-reflection-query-surface-without-widening-the-public-abi"
        ),
    }
    for field, expected_value in expected_object_model_abi_query_surface_fields.items():
        if object_model_abi_query_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_object_model_abi_query_surface drifted from {field}"
            )
    if object_model_abi_query_surface.get("public_runtime_abi_boundary") != PUBLIC_RUNTIME_ABI_BOUNDARY:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the public runtime ABI boundary"
        )
    if object_model_abi_query_surface.get("private_object_model_query_boundary") != [
        "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "objc3_runtime_copy_realized_class_entry_for_testing",
        "objc3_runtime_copy_property_registry_state_for_testing",
        "objc3_runtime_copy_property_entry_for_testing",
        "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "objc3_runtime_copy_selector_lookup_table_state_for_testing",
        "objc3_runtime_copy_selector_lookup_entry_for_testing",
        "objc3_runtime_copy_method_cache_state_for_testing",
        "objc3_runtime_copy_method_cache_entry_for_testing",
        "objc3_runtime_copy_dispatch_state_for_testing",
    ]:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the private object-model query boundary"
        )
    if object_model_abi_query_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface must require the coupled runtime registration manifest"
        )
    if object_model_abi_query_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface must require real compile output"
        )
    if object_model_abi_query_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface must require a linked runtime probe"
        )
    if not isinstance(realization_lookup_reflection_implementation_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_realization_lookup_reflection_implementation_surface"
        )
    if (
        realization_lookup_reflection_implementation_surface.get("contract_id")
        != RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_realization_lookup_reflection_implementation_surface contract"
        )
    if (
        realization_lookup_reflection_implementation_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the compile manifest artifact path"
        )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "registration_manifest_artifact"
        )
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "registration_descriptor_artifact"
        )
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        realization_lookup_reflection_implementation_surface.get("object_artifact")
        != obj_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the emitted object artifact path"
        )
    if (
        realization_lookup_reflection_implementation_surface.get("backend_artifact")
        != ll_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_realization_lookup_reflection_implementation_fields = {
        "runtime_object_model_abi_query_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_realization_lookup_semantics_surface_contract_id": (
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "runtime_class_metaclass_protocol_realization_surface_contract_id": (
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
        ),
        "runtime_category_attachment_merged_dispatch_surface_contract_id": (
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface_contract_id": (
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID
        ),
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "object_model_query_state_snapshot_symbol": (
            "objc3_runtime_copy_object_model_query_state_for_testing"
        ),
        "realized_class_entry_snapshot_symbol": (
            "objc3_runtime_copy_realized_class_entry_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "protocol_conformance_query_symbol": (
            "objc3_runtime_copy_protocol_conformance_query_for_testing"
        ),
        "selector_lookup_table_state_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_table_state_for_testing"
        ),
        "selector_lookup_entry_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_entry_for_testing"
        ),
        "method_cache_state_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_state_for_testing"
        ),
        "method_cache_entry_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_entry_for_testing"
        ),
        "dispatch_state_snapshot_symbol": "objc3_runtime_copy_dispatch_state_for_testing",
        "realization_lookup_reflection_implementation_model": (
            "runtime-owned-class-property-protocol-and-method-cache-query-snapshots-publish-coherent-last-query-state-and-live-object-model-counts"
        ),
    }
    for field, expected_value in (
        expected_realization_lookup_reflection_implementation_fields.items()
    ):
        if realization_lookup_reflection_implementation_surface.get(field) != expected_value:
            raise RuntimeError(
                "runtime_realization_lookup_reflection_implementation_surface drifted "
                f"from {field}"
            )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface must require the coupled runtime registration manifest"
        )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface must require real compile output"
        )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface must require a linked runtime probe"
        )
    if not isinstance(reflection_query_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_reflection_query_surface")
    if (
        reflection_query_surface.get("contract_id")
        != RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_reflection_query_surface contract"
        )
    if reflection_query_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_reflection_query_surface drifted from the compile manifest artifact path"
        )
    if reflection_query_surface.get("registration_manifest_artifact") != registration_manifest_path.name:
        raise RuntimeError(
            "runtime_reflection_query_surface drifted from the runtime registration manifest artifact path"
        )
    if reflection_query_surface.get("registration_descriptor_artifact") != registration_descriptor_path.name:
        raise RuntimeError(
            "runtime_reflection_query_surface drifted from the runtime registration descriptor artifact path"
        )
    if reflection_query_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError("runtime_reflection_query_surface drifted from the emitted object artifact path")
    if reflection_query_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError("runtime_reflection_query_surface drifted from the emitted LLVM IR artifact path")
    expected_reflection_surface_fields = {
        "object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "dispatch_accessor_runtime_abi_surface_contract_id": "objc3c.runtime.dispatch_accessor.abi.surface.v1",
        "property_metadata_reflection_contract_id": "objc3c.runtime.property.metadata.reflection.v1",
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "query_api_boundary_model": (
            "private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi"
        ),
    }
    for field, expected_value in expected_reflection_surface_fields.items():
        if reflection_query_surface.get(field) != expected_value:
            raise RuntimeError(f"runtime_reflection_query_surface drifted from {field}")
    if reflection_query_surface.get("private_query_symbols") != [
        "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "objc3_runtime_copy_realized_class_entry_for_testing",
        "objc3_runtime_copy_property_registry_state_for_testing",
        "objc3_runtime_copy_property_entry_for_testing",
        "objc3_runtime_copy_protocol_conformance_query_for_testing",
    ]:
        raise RuntimeError("runtime_reflection_query_surface drifted from the private query symbol boundary")
    if reflection_query_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_reflection_query_surface must require the coupled runtime registration manifest"
        )
    if reflection_query_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_reflection_query_surface must require real compile output"
        )
    if reflection_query_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_reflection_query_surface must require a linked runtime probe"
        )
    if not isinstance(realization_lookup_semantics_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_realization_lookup_semantics_surface"
        )
    if (
        realization_lookup_semantics_surface.get("contract_id")
        != RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_realization_lookup_semantics_surface contract"
        )
    if realization_lookup_semantics_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the compile manifest artifact path"
        )
    if (
        realization_lookup_semantics_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        realization_lookup_semantics_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the runtime registration descriptor artifact path"
        )
    if realization_lookup_semantics_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the emitted object artifact path"
        )
    if realization_lookup_semantics_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_realization_lookup_semantics_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "dispatch_accessor_runtime_abi_surface_contract_id": (
            "objc3c.runtime.dispatch_accessor.abi.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "selector_lookup_table_state_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_table_state_for_testing"
        ),
        "selector_lookup_entry_snapshot_symbol": "objc3_runtime_copy_selector_lookup_entry_for_testing",
        "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
        "method_cache_entry_snapshot_symbol": "objc3_runtime_copy_method_cache_entry_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "lookup_resolution_order_model": (
            "seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-deterministic-fallback"
        ),
        "selector_materialization_model": (
            "metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup"
        ),
        "unresolved_selector_behavior_model": (
            "negative-cache-entry-preserved-and-deterministic-fallback-returned"
        ),
    }
    for field, expected_value in expected_realization_lookup_semantics_fields.items():
        if realization_lookup_semantics_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_realization_lookup_semantics_surface drifted from {field}"
            )
    if realization_lookup_semantics_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface must require the coupled runtime registration manifest"
        )
    if realization_lookup_semantics_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface must require real compile output"
        )
    if realization_lookup_semantics_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface must require a linked runtime probe"
        )
    if not isinstance(class_metaclass_protocol_realization_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_class_metaclass_protocol_realization_surface"
        )
    if (
        class_metaclass_protocol_realization_surface.get("contract_id")
        != RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_class_metaclass_protocol_realization_surface contract"
        )
    if class_metaclass_protocol_realization_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the compile manifest artifact path"
        )
    if (
        class_metaclass_protocol_realization_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        class_metaclass_protocol_realization_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the runtime registration descriptor artifact path"
        )
    if class_metaclass_protocol_realization_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the emitted object artifact path"
        )
    if class_metaclass_protocol_realization_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_class_metaclass_protocol_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_realization_lookup_semantics_surface_contract_id": (
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "realized_class_graph_snapshot_symbol": "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "class_realization_model": (
            "registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection"
        ),
        "metaclass_lineage_model": (
            "realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities"
        ),
        "protocol_conformance_model": (
            "realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance"
        ),
    }
    for field, expected_value in expected_class_metaclass_protocol_fields.items():
        if class_metaclass_protocol_realization_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_class_metaclass_protocol_realization_surface drifted from {field}"
            )
    if class_metaclass_protocol_realization_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface must require the coupled runtime registration manifest"
        )
    if class_metaclass_protocol_realization_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface must require real compile output"
        )
    if class_metaclass_protocol_realization_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface must require a linked runtime probe"
        )
    if not isinstance(category_attachment_merged_dispatch_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_category_attachment_merged_dispatch_surface"
        )
    if (
        category_attachment_merged_dispatch_surface.get("contract_id")
        != RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_category_attachment_merged_dispatch_surface contract"
        )
    if category_attachment_merged_dispatch_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the compile manifest artifact path"
        )
    if (
        category_attachment_merged_dispatch_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        category_attachment_merged_dispatch_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the runtime registration descriptor artifact path"
        )
    if category_attachment_merged_dispatch_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the emitted object artifact path"
        )
    if category_attachment_merged_dispatch_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_category_attachment_merged_dispatch_fields = {
        "runtime_category_attachment_protocol_conformance_contract_id": (
            "objc3c.runtime.category.attachment.protocol.conformance.v1"
        ),
        "runtime_realization_lookup_semantics_surface_contract_id": (
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "runtime_class_metaclass_protocol_realization_surface_contract_id": (
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "realized_class_graph_snapshot_symbol": "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
        "method_cache_entry_snapshot_symbol": "objc3_runtime_copy_method_cache_entry_for_testing",
        "category_attachment_model": (
            "registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch"
        ),
        "merged_dispatch_resolution_model": (
            "attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-fallback"
        ),
        "attached_protocol_visibility_model": (
            "attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries"
        ),
    }
    for field, expected_value in expected_category_attachment_merged_dispatch_fields.items():
        if category_attachment_merged_dispatch_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_category_attachment_merged_dispatch_surface drifted from {field}"
            )
    if category_attachment_merged_dispatch_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface must require the coupled runtime registration manifest"
        )
    if category_attachment_merged_dispatch_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface must require real compile output"
        )
    if category_attachment_merged_dispatch_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface must require a linked runtime probe"
        )
    if not isinstance(reflection_visibility_coherence_diagnostics_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_reflection_visibility_coherence_diagnostics_surface"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get("contract_id")
        != RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_reflection_visibility_coherence_diagnostics_surface contract"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the compile manifest artifact path"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the runtime registration descriptor artifact path"
        )
    if reflection_visibility_coherence_diagnostics_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the emitted object artifact path"
        )
    if reflection_visibility_coherence_diagnostics_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_reflection_visibility_coherence_diagnostics_fields = {
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_category_attachment_merged_dispatch_surface_contract_id": (
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID
        ),
        "dispatch_accessor_runtime_abi_surface_contract_id": (
            "objc3c.runtime.dispatch_accessor.abi.surface.v1"
        ),
        "property_metadata_reflection_contract_id": (
            "objc3c.runtime.property.metadata.reflection.v1"
        ),
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
        "reflection_visibility_boundary_model": (
            "private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state"
        ),
        "fail_closed_lookup_diagnostic_model": (
            "missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state"
        ),
        "runtime_coherence_diagnostic_model": (
            "reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state"
        ),
    }
    for field, expected_value in expected_reflection_visibility_coherence_diagnostics_fields.items():
        if reflection_visibility_coherence_diagnostics_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_reflection_visibility_coherence_diagnostics_surface drifted from {field}"
            )
    if (
        reflection_visibility_coherence_diagnostics_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface must require the coupled runtime registration manifest"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface must require real compile output"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface must require a linked runtime probe"
        )
    if not isinstance(runtime_installation_abi_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_installation_abi_surface")
    if (
        runtime_installation_abi_surface.get("contract_id")
        != RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_installation_abi_surface contract"
        )
    if runtime_installation_abi_surface.get("public_header_path") != RUNTIME_PUBLIC_HEADER_PATH:
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the public runtime header path"
        )
    if (
        runtime_installation_abi_surface.get("internal_header_path")
        != RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the bootstrap internal header path"
        )
    if (
        runtime_installation_abi_surface.get("bootstrap_api_contract_id")
        != bootstrap_api_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the bootstrap API contract"
        )
    if (
        runtime_installation_abi_surface.get("bootstrap_reset_contract_id")
        != bootstrap_reset_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the bootstrap reset contract"
        )
    if (
        runtime_installation_abi_surface.get("bootstrap_registrar_contract_id")
        != bootstrap_registrar_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the bootstrap registrar contract"
        )
    if (
        runtime_installation_abi_surface.get("public_installation_abi_boundary")
        != RUNTIME_INSTALLATION_ABI_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the public installation ABI boundary"
        )
    if (
        runtime_installation_abi_surface.get("private_loader_testing_boundary")
        != RUNTIME_LOADER_TESTING_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the private loader testing boundary"
        )
    if (
        runtime_installation_abi_surface.get(
            "installation_requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface must require the coupled registration manifest"
        )
    if (
        runtime_installation_abi_surface.get(
            "register_image_consumes_staged_registration_table_once"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface must preserve single-consumption staged registration"
        )
    if (
        runtime_installation_abi_surface.get("deterministic_reset_replay_supported")
        is not True
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface must preserve deterministic reset/replay support"
        )
    if not isinstance(runtime_loader_lifecycle_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_loader_lifecycle_surface")
    if (
        runtime_loader_lifecycle_surface.get("contract_id")
        != RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_loader_lifecycle_surface contract"
        )
    if (
        runtime_loader_lifecycle_surface.get(
            "runtime_installation_abi_surface_contract_id"
        )
        != RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the installation ABI contract id"
        )
    if (
        runtime_loader_lifecycle_surface.get("bootstrap_semantics_contract_id")
        != runtime_bootstrap_semantics.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the bootstrap semantics contract"
        )
    if (
        runtime_loader_lifecycle_surface.get("bootstrap_reset_contract_id")
        != bootstrap_reset_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the bootstrap reset contract"
        )
    if (
        runtime_loader_lifecycle_surface.get("bootstrap_registrar_contract_id")
        != bootstrap_registrar_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the bootstrap registrar contract"
        )
    if (
        runtime_loader_lifecycle_surface.get("authoritative_probe_path")
        != INSTALLATION_LIFECYCLE_PROBE
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the authoritative lifecycle probe path"
        )
    if (
        runtime_loader_lifecycle_surface.get("loader_testing_boundary_symbols")
        != RUNTIME_LOADER_TESTING_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the loader testing boundary symbols"
        )
    if runtime_loader_lifecycle_surface.get("lifecycle_phases") != [
        "startup-installed-runtime-state",
        "duplicate-registration-rejected-without-state-advance",
        "out-of-order-registration-rejected-without-state-advance",
        "invalid-anchor-root-rejected-without-state-advance",
        "invalid-discovery-root-rejected-without-state-advance",
        "reset-retained-bootstrap-catalog",
        "replay-restored-installed-runtime-state",
    ]:
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the lifecycle phase set"
        )
    if runtime_loader_lifecycle_surface.get("rejected_registration_status_codes") != {
        "duplicate_translation_unit_identity_key": -2,
        "out_of_order_registration": -3,
        "invalid_registration_roots": -4,
    }:
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the rejected-registration status set"
        )
    if (
        runtime_loader_lifecycle_surface.get("retained_bootstrap_catalog_required")
        is not True
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface must require retained bootstrap catalog support"
        )
    if (
        runtime_loader_lifecycle_surface.get("deterministic_replay_required")
        is not True
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface must require deterministic replay"
        )
    if (
        runtime_loader_lifecycle_surface.get(
            "requires_linked_fixture_or_loader_retained_roots"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface must require linked fixture or loader-retained roots"
        )

    if provenance.get("contract_id") != COMPILE_PROVENANCE_CONTRACT_ID:
        raise RuntimeError("compiled fixture did not publish the native compile provenance contract")
    truthfulness = provenance.get("compile_output_truthfulness")
    if not isinstance(truthfulness, dict) or truthfulness.get("contract_id") != COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID:
        raise RuntimeError("compiled fixture did not publish the compile output truthfulness contract")
    if truthfulness.get("truthful") is not True:
        raise RuntimeError("compiled fixture did not certify truthful compile output")
    if registration_manifest.get("compile_output_provenance_artifact") != "module.compile-provenance.json":
        raise RuntimeError("runtime registration manifest did not bind compile provenance artifact")
    if registration_manifest.get("compile_output_truthful") is not True:
        raise RuntimeError("runtime registration manifest did not certify truthful compile output")
    if registration_manifest.get("compile_output_artifact_set_digest_sha256") != provenance.get("artifact_set_digest_sha256"):
        raise RuntimeError("runtime registration manifest compile output digest drifted from compile provenance")
    return obj_path


def compile_fixture(fixture: Path, out_dir: Path) -> Path:
    return compile_fixture_with_args(fixture, out_dir)


def compile_fixture_manifest_only(
    fixture: Path, out_dir: Path, extra_args: list[str] | None = None
) -> tuple[Path, subprocess.CompletedProcess[str]]:
    out_dir.mkdir(parents=True, exist_ok=True)
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(COMPILE_PS1),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            *(extra_args or []),
        ]
    )
    manifest_path = out_dir / "module.manifest.json"
    if not manifest_path.is_file():
        raise RuntimeError(
            f"fixture compile did not publish {manifest_path} for {fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    return manifest_path, result


def compile_fixture_outputs(fixture: Path, out_dir: Path) -> tuple[Path, Path, Path]:
    obj_path = compile_fixture(fixture, out_dir)
    ll_path = out_dir / "module.ll"
    manifest_path = out_dir / "module.manifest.json"
    if not ll_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {ll_path}")
    return obj_path, ll_path, manifest_path


def compile_fixture_outputs_with_args(
    fixture: Path, out_dir: Path, extra_args: list[str] | None = None
) -> tuple[Path, Path, Path]:
    obj_path = compile_fixture_with_args(fixture, out_dir, extra_args=extra_args)
    ll_path = out_dir / "module.ll"
    manifest_path = out_dir / "module.manifest.json"
    if not ll_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {ll_path}")
    return obj_path, ll_path, manifest_path


def compile_fixture_expect_failure(
    fixture: Path,
    out_dir: Path,
    *,
    expected_snippets: list[str],
    expected_codes: list[str],
) -> dict[str, Any]:
    out_dir.mkdir(parents=True, exist_ok=True)
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(COMPILE_PS1),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
    )
    if result.returncode == 0:
        raise RuntimeError(f"fixture compile unexpectedly succeeded for {fixture}")
    diagnostics_txt_path = out_dir / "module.diagnostics.txt"
    diagnostics_json_path = out_dir / "module.diagnostics.json"
    if not diagnostics_txt_path.is_file():
        raise RuntimeError(f"failed compile for {fixture} did not publish {diagnostics_txt_path}")
    if not diagnostics_json_path.is_file():
        raise RuntimeError(f"failed compile for {fixture} did not publish {diagnostics_json_path}")
    diagnostics_text = diagnostics_txt_path.read_text(encoding="utf-8")
    diagnostics_payload = json.loads(diagnostics_json_path.read_text(encoding="utf-8"))
    diagnostics = diagnostics_payload.get("diagnostics", [])
    expect(
        isinstance(diagnostics, list) and diagnostics,
        f"failed compile for {fixture} did not publish structured diagnostics",
    )
    for snippet in expected_snippets:
        expect(
            snippet in diagnostics_text,
            f"failed compile for {fixture} did not publish expected diagnostic snippet: {snippet}",
        )
    observed_codes = {
        diagnostic.get("code")
        for diagnostic in diagnostics
        if isinstance(diagnostic, dict) and isinstance(diagnostic.get("code"), str)
    }
    for expected_code in expected_codes:
        expect(
            expected_code in observed_codes,
            f"failed compile for {fixture} did not publish expected diagnostic code {expected_code}",
        )
    return {
        "returncode": result.returncode,
        "diagnostic_count": len(diagnostics),
        "diagnostic_codes": sorted(observed_codes),
        "diagnostics_path": str(diagnostics_json_path.relative_to(ROOT)).replace("\\", "/"),
    }


def compile_probe(clangxx: str, probe: Path, exe_path: Path, extra_objects: list[Path]) -> None:
    compile_probe_with_args(clangxx, probe, exe_path, extra_objects, [])


def compile_probe_with_args(
    clangxx: str,
    probe: Path,
    exe_path: Path,
    extra_objects: list[Path],
    extra_args: list[str],
) -> None:
    exe_path.parent.mkdir(parents=True, exist_ok=True)
    command = [
        clangxx,
        "-std=c++20",
        "-fms-runtime-lib=dll",
        "-I",
        str(ROOT / "native" / "objc3c" / "src"),
        *extra_args,
        str(probe),
        *[str(path) for path in extra_objects],
        str(RUNTIME_LIB),
        "-o",
        str(exe_path),
    ]
    result = run(command)
    if result.returncode != 0:
        raise RuntimeError(
            f"probe link failed for {probe}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )


def link_fixture_executable(clangxx: str, obj_path: Path, exe_path: Path) -> None:
    exe_path.parent.mkdir(parents=True, exist_ok=True)
    command = [
        clangxx,
        "-std=c++20",
        "-fms-runtime-lib=dll",
        str(obj_path),
        str(RUNTIME_LIB),
        "-o",
        str(exe_path),
    ]
    result = run(command)
    if result.returncode != 0:
        raise RuntimeError(
            f"fixture link failed for {obj_path}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )


def run_probe(exe_path: Path) -> subprocess.CompletedProcess[str]:
    result = run([str(exe_path)])
    if result.returncode != 0:
        raise RuntimeError(
            f"probe execution failed for {exe_path} (exit={result.returncode}):\n"
            f"STDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    return result


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def parse_json_output(result: subprocess.CompletedProcess[str], label: str) -> dict[str, Any]:
    stdout = result.stdout.strip()
    if not stdout:
        raise RuntimeError(f"{label} produced no JSON output")
    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"{label} produced invalid JSON: {exc}\nstdout:\n{stdout}") from exc
    if not isinstance(payload, dict):
        raise RuntimeError(f"{label} did not produce a JSON object")
    return payload


def parse_key_value_output(
    result: subprocess.CompletedProcess[str], label: str
) -> dict[str, Any]:
    stdout = result.stdout.strip()
    if not stdout:
        raise RuntimeError(f"{label} produced no key/value output")
    payload: dict[str, Any] = {}
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if "=" not in line:
            raise RuntimeError(f"{label} produced malformed line: {line}")
        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()
        if value and (value.lstrip("-").isdigit()):
            payload[key] = int(value)
        else:
            payload[key] = value
    if not payload:
        raise RuntimeError(f"{label} produced no parseable key/value output")
    return payload


@dataclass(frozen=True)
class CaseResult:
    case_id: str
    probe: str
    fixture: str | None
    claim_class: str
    passed: bool
    summary: dict[str, Any]


def build_claim_boundary() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
        "authoritative_claim_classes": {
            "linked-runtime-probe": {
                "requires_runtime_library_or_emitted_object": True,
                "requires_executable_probe": True,
                "requires_runtime_backed_execution_or_snapshot": True,
            },
            "compile-coupled-inspection": {
                "requires_real_compile": True,
                "requires_compile_output_truthfulness": True,
                "requires_coupled_registration_manifest": True,
            },
        },
        "non_authoritative_inputs": [
            "hand-authored llvm ir without matching compile output",
            "sidecar-only manifests or reports with no coupled object/probe path",
            "compatibility shims without a coupled emitted object and runtime probe",
            "comment-only or placeholder-only capability claims",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
    }


def build_runtime_state_publication_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
        "publication_surface_kind": RUNTIME_STATE_PUBLICATION_SURFACE_KIND,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "publication_requires_coupled_registration_manifest": True,
        "publication_requires_real_compile_output": True,
    }


def build_runtime_bootstrap_registration_source_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_bootstrap_lowering_registration_artifact_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "composed_source_inputs": [
            "objc_runtime_bootstrap_lowering_contract",
            "objc_runtime_translation_unit_registration_manifest",
            "objc_runtime_startup_bootstrap_semantics",
            "objc_runtime_registration_descriptor_frontend_closure",
        ],
        "emitted_symbol_fields": [
            "constructor_root_symbol",
            "init_stub_symbol_prefix",
            "registration_table_symbol_prefix",
            "image_local_init_state_symbol_prefix",
            "registration_entrypoint_symbol",
        ],
        "emitted_table_fields": [
            "registration_table_layout_model",
            "registration_table_abi_version",
            "registration_table_pointer_field_count",
        ],
        "emission_state_fields": [
            "constructor_root_emission_state",
            "init_stub_emission_state",
            "registration_table_emission_state",
            "bootstrap_ir_materialization_landed",
            "image_local_initialization_landed",
        ],
        "lowered_registration_descriptor_fields": [
            "constructor_init_stub_symbol",
            "bootstrap_registration_table_symbol",
            "bootstrap_image_local_init_state_symbol",
            "bootstrap_registration_table_layout_model",
            "bootstrap_image_local_initialization_model",
            "bootstrap_registration_table_abi_version",
            "bootstrap_registration_table_pointer_field_count",
            "translation_unit_registration_order_ordinal",
        ],
        "loader_table_ir_proof_fields": [
            "constructor_root_symbol",
            "constructor_init_stub_symbol",
            "bootstrap_registration_table_symbol",
            "bootstrap_image_local_init_state_symbol",
            "translation_unit_registration_order_ordinal",
        ],
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_emitted_loader_table_ir": True,
    }


def build_runtime_multi_image_startup_ordering_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    installation_lifecycle = next(
        (result for result in results if result.case_id == "installation-lifecycle"),
        None,
    )
    latest_summary = installation_lifecycle.summary if installation_lifecycle is not None else {}
    return {
        "contract_id": RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID,
        "runtime_installation_abi_surface_contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "runtime_loader_lifecycle_surface_contract_id": RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID,
        "authoritative_case_ids": (
            ["installation-lifecycle"] if installation_lifecycle is not None else []
        ),
        "fixture_path": INSTALLATION_LIFECYCLE_FIXTURE,
        "probe_path": INSTALLATION_LIFECYCLE_PROBE,
        "runtime_public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "runtime_bootstrap_internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "validation_commands": [
            RUNTIME_ACCEPTANCE_COMMAND,
            VALIDATE_RUNTIME_ARCHITECTURE_COMMAND,
        ],
        "runtime_symbols": [
            "objc3_runtime_register_image",
            "objc3_runtime_copy_registration_state_for_testing",
            "objc3_runtime_reset_for_testing",
            "objc3_runtime_replay_registered_images_for_testing",
            "objc3_runtime_copy_reset_replay_state_for_testing",
        ],
        "diagnostic_status_codes": {
            "duplicate_registration": -2,
            "out_of_order_registration": -3,
        },
        "diagnostic_models": {
            "duplicate_install": RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL,
            "out_of_order_install": RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL,
        },
        "rejected_registration_fields": [
            "last_rejected_module_name",
            "last_rejected_translation_unit_identity_key",
            "last_rejected_registration_order_ordinal",
        ],
        "ordering_snapshot_fields": [
            "next_expected_registration_order_ordinal",
            "last_successful_registration_order_ordinal",
            "last_rejected_registration_order_ordinal",
        ],
        "measured_summary_fields": [
            "fixture_compile_ms",
            "probe_link_ms",
            "probe_run_ms",
            "case_total_ms",
        ],
        "latest_installation_lifecycle_measurements": latest_summary,
        "latest_duplicate_install_diagnostic": {
            "status": latest_summary.get("duplicate_status"),
            "rejected_module_name": latest_summary.get("duplicate_rejected_module_name"),
            "rejected_translation_unit_identity_key": latest_summary.get(
                "duplicate_rejected_translation_unit_identity_key"
            ),
            "rejected_registration_order_ordinal": latest_summary.get(
                "duplicate_rejected_registration_order_ordinal"
            ),
        },
        "latest_out_of_order_install_diagnostic": {
            "status": latest_summary.get("out_of_order_status"),
            "rejected_module_name": latest_summary.get("out_of_order_rejected_module_name"),
            "rejected_translation_unit_identity_key": latest_summary.get(
                "out_of_order_rejected_translation_unit_identity_key"
            ),
            "rejected_registration_order_ordinal": latest_summary.get(
                "out_of_order_rejected_registration_order_ordinal"
            ),
        },
    }


def build_runtime_error_execution_cleanup_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "error-execution-cleanup-source"
    ]
    return {
        "contract_id": RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.error.source.closure.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/parse/objc3_parser.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/error_source_closure_positive.objc3",
            "tests/tooling/fixtures/native/try_expression_fail_closed_negative.objc3",
            "tests/tooling/fixtures/native/throw_statement_fail_closed_negative.objc3",
            "tests/tooling/fixtures/native/do_catch_fail_closed_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-public-runtime-abi-widening",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_catch_filter_finalization_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "catch-filter-finalization-source"
    ]
    return {
        "contract_id": RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.try.throw.do.catch.semantics.v1",
            "objc3c.error_handling.error.bridge.legality.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/parse/objc3_parser.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
            "tests/tooling/fixtures/native/bridge_legality_positive.objc3",
            "tests/tooling/fixtures/native/try_requires_throwing_context_negative.objc3",
            "tests/tooling/fixtures/native/throw_requires_throws_or_catch_negative.objc3",
            "tests/tooling/fixtures/native/catch_after_catch_all_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_throws_conflict_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-lowering-or-runtime-abi-claims",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_error_propagation_cleanup_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "error-propagation-cleanup-semantics"
    ]
    return {
        "contract_id": RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.error.semantic.model.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/sema/objc3_sema_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.h",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/error_source_closure_positive.objc3",
            "tests/tooling/fixtures/native/error_bridge_marker_surface_positive.objc3",
            "tests/tooling/fixtures/native/status_code_attribute_missing_mapping_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-runtime-abi-claims-before-lane-d",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_bridging_filter_unwind_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "bridging-filter-unwind-compatibility-diagnostics"
    ]
    return {
        "contract_id": RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.error.bridge.legality.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/sema/objc3_sema_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.h",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/bridge_legality_positive.objc3",
            "tests/tooling/fixtures/native/bridge_legality_native_fail_closed.objc3",
            "tests/tooling/fixtures/native/bridge_legality_nserror_missing_out_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_nserror_bad_return_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_throws_conflict_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_marker_conflict_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_error_type_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_missing_mapping_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_mapping_signature_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_status_return_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-runtime-abi-claims-before-lane-d",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_error_lowering_unwind_bridge_helper_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "error-lowering-unwind-bridge-helper-surface"
    ]
    return {
        "contract_id": RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.ns.error.bridging.lowering.v1",
            "objc3c.unwind.cleanup.lowering.v1",
            "objc3c.error_handling.throws.abi.propagation.lowering.v1",
            "objc3c.error_handling.result.and.bridging.artifact.replay.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/error_out_abi_positive.objc3",
            "tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3",
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-runtime-helper-abi-claims-before-lane-d",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_error_runtime_abi_cleanup_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"error-runtime-abi-cleanup"}
    ]
    return {
        "contract_id": RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "error_lowering_unwind_bridge_helper_surface_contract_id": (
            RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_error_runtime_abi_boundary": PRIVATE_ERROR_RUNTIME_ABI_BOUNDARY,
        "error_store_symbol": "objc3_runtime_store_thrown_error_i32",
        "error_load_symbol": "objc3_runtime_load_thrown_error_i32",
        "error_status_bridge_symbol": "objc3_runtime_bridge_status_error_i32",
        "error_nserror_bridge_symbol": "objc3_runtime_bridge_nserror_error_i32",
        "error_catch_match_symbol": "objc3_runtime_catch_matches_error_i32",
        "error_bridge_state_snapshot_symbol": (
            "objc3_runtime_copy_error_bridge_state_for_testing"
        ),
        "runtime_abi_boundary_model": (
            "private-runtime-abi-exposes-thrown-error-storage-status-bridge-"
            "nserror-bridge-and-catch-match-helpers-through-stable-testable-"
            "bootstrap-internal-entrypoints"
        ),
        "cleanup_runtime_model": (
            "lowered-throw-and-catch-paths-share-one-runtime-error-bridge-state-"
            "snapshot-surface-for-store-load-bridge-and-catch-match-observation"
        ),
        "fail_closed_model": (
            "public-header-surface-stays-unchanged-while-private-error-runtime-abi-"
            "remains-test-only-and-explicitly-versioned-through-the-runtime-probe"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_path": (
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp"
        ),
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {"error-runtime-abi-cleanup", "live-error-runtime-integration"}
    ]
    return {
        "contract_id": (
            RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        "error_runtime_abi_cleanup_surface_contract_id": (
            RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID
        ),
        "error_lowering_unwind_bridge_helper_surface_contract_id": (
            RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_error_runtime_abi_boundary": PRIVATE_ERROR_RUNTIME_ABI_BOUNDARY,
        "authoritative_code_paths": [
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
            "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
        ],
        "runtime_implementation_model": (
            "lowered-throw-catch-and-status-bridge-paths-execute-through-the-live-"
            "error-runtime-helpers-and-publish-observable-bridge-state-snapshots"
        ),
        "fail_closed_model": (
            "runtime-integration-remains-private-and-testable-through-runtime-probes-"
            "until-a-public-error-abi-is-explicitly-claimed"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_unified_concurrency_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"unified-concurrency-runtime-architecture"}
    ]
    return {
        "contract_id": RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "source_surface_model": (
            "unified-concurrency-source-surface-freezes-live-async-actor-task-source-and-sema-boundaries-before-lowering-runtime-and-public-abi-expansion"
        ),
        "source_contract_ids": [
            "objc3c.concurrency.async.source.closure.v1",
            "objc3c.concurrency.actor.member.isolation.source.closure.v1",
            "objc3c.concurrency.task.group.cancellation.source.closure.v1",
            "objc3c.concurrency.async.effect.suspension.semantic.model.v1",
            "objc3c.concurrency.task.executor.cancellation.semantic.model.v1",
            "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/runtime/objc3_runtime.h",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_async_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_member_and_isolation_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_cancellation_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_concurrency_runtime_boundary": [
            "objc3_runtime_allocate_async_continuation_i32",
            "objc3_runtime_handoff_async_continuation_to_executor_i32",
            "objc3_runtime_resume_async_continuation_i32",
            "objc3_runtime_spawn_task_i32",
            "objc3_runtime_enter_task_group_scope_i32",
            "objc3_runtime_add_task_group_task_i32",
            "objc3_runtime_wait_task_group_next_i32",
            "objc3_runtime_cancel_task_group_i32",
            "objc3_runtime_task_is_cancelled_i32",
            "objc3_runtime_task_on_cancel_i32",
            "objc3_runtime_actor_enter_isolation_thunk_i32",
            "objc3_runtime_actor_enter_nonisolated_i32",
            "objc3_runtime_actor_hop_to_executor_i32",
            "objc3_runtime_actor_record_replay_proof_i32",
            "objc3_runtime_actor_record_race_guard_i32",
            "objc3_runtime_actor_bind_executor_i32",
            "objc3_runtime_actor_mailbox_enqueue_i32",
            "objc3_runtime_actor_mailbox_drain_next_i32",
            "objc3_runtime_copy_async_continuation_state_for_testing",
            "objc3_runtime_copy_task_runtime_state_for_testing",
            "objc3_runtime_copy_actor_runtime_state_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_await_executor_source_closure_positive.objc3",
            "tests/tooling/fixtures/native/actor_member_isolation_surface_positive.objc3",
            "tests/tooling/fixtures/native/task_executor_cancellation_source_closure_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-concurrency-proof",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_async_task_actor_normalization_completion_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"async-task-actor-normalization-completion"}
    ]
    return {
        "contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "source_surface_contract_id": RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
        "normalized_semantic_contract_ids": [
            "objc3c.concurrency.async.effect.suspension.semantic.model.v1",
            "objc3c.concurrency.task.executor.cancellation.semantic.model.v1",
            "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1",
        ],
        "lowering_contract_ids": [
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ],
        "lowering_lane_contract_ids": [
            "objc3c.async.continuation.lowering.v1",
            "objc3c.await.lowering.suspension.state.lowering.v1",
            "objc3c.task.runtime.interop.cancellation.lowering.v1",
            "objc3c.concurrency.replay.race.guard.lowering.v1",
            "objc3c.actor.lowering.metadata.contract.v1",
            "objc3c.actor.isolation.sendability.lowering.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_surface_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract",
        ],
        "normalization_completion_model": (
            "normalized-async-task-actor-sema-and-lowering-packets-freeze-the-live-boundary-before-runtime-abi-and-runnable-execution-closure"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_lowering_positive.objc3",
            "tests/tooling/fixtures/native/actor_isolation_sendable_semantic_model_positive.objc3",
            "tests/tooling/fixtures/native/task_executor_cancellation_semantic_model_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-runnable-task-or-actor-execution-claim",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_unified_concurrency_lowering_metadata_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"unified-concurrency-lowering-metadata-surface"}
    ]
    return {
        "contract_id": RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "source_surface_contract_id": RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
        "normalization_completion_surface_contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "lowering_contract_ids": [
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ],
        "lowering_detail_contract_ids": [
            "objc3c.concurrency.async.direct.call.lowering.v1",
            "objc3c.concurrency.task.runtime.abi.completion.v1",
            "objc3c.concurrency.actor.isolation.sendability.enforcement.v1",
        ],
        "lowering_lane_contract_ids": [
            "objc3c.async.continuation.lowering.v1",
            "objc3c.await.lowering.suspension.state.lowering.v1",
            "objc3c.task.runtime.interop.cancellation.lowering.v1",
            "objc3c.concurrency.replay.race.guard.lowering.v1",
            "objc3c.actor.lowering.metadata.contract.v1",
            "objc3c.actor.isolation.sendability.lowering.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        ],
        "authoritative_surface_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_async_function_await_and_continuation_lowering",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_runtime_abi_completion",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement",
        ],
        "lowering_metadata_surface_model": (
            "unified-concurrency-lowering-and-metadata-surface-freezes-live-async-task-actor-lowering-packets-and-emitted-metadata-boundaries-before-runtime-abi-and-runnable-execution-closure"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_lowering_positive.objc3",
            "tests/tooling/fixtures/native/task_runtime_async_entry_lowering_positive.objc3",
            "tests/tooling/fixtures/native/actor_lowering_metadata_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-runnable-task-or-actor-execution-claim",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_unified_concurrency_runtime_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"unified-concurrency-runtime-abi"}
    ]
    return {
        "contract_id": RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "unified_concurrency_source_surface_contract_id": (
            RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "async_task_actor_normalization_completion_surface_contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "unified_concurrency_lowering_metadata_surface_contract_id": (
            RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_unified_concurrency_runtime_abi_boundary": (
            PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY
        ),
        "async_continuation_state_snapshot_symbol": (
            "objc3_runtime_copy_async_continuation_state_for_testing"
        ),
        "task_runtime_state_snapshot_symbol": (
            "objc3_runtime_copy_task_runtime_state_for_testing"
        ),
        "actor_runtime_state_snapshot_symbol": (
            "objc3_runtime_copy_actor_runtime_state_for_testing"
        ),
        "runtime_abi_boundary_model": UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY_MODEL,
        "continuation_runtime_model": UNIFIED_CONCURRENCY_CONTINUATION_RUNTIME_MODEL,
        "task_runtime_model": UNIFIED_CONCURRENCY_TASK_RUNTIME_MODEL,
        "actor_runtime_model": UNIFIED_CONCURRENCY_ACTOR_RUNTIME_MODEL,
        "fail_closed_model": UNIFIED_CONCURRENCY_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_paths": [
            CONTINUATION_RUNTIME_ABI_PROBE,
            TASK_RUNTIME_ABI_PROBE,
            ACTOR_RUNTIME_ABI_PROBE,
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_metaprogramming_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"metaprogramming-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.metaprogramming.metaprogramming.source.closure.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/token/objc3_token_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3InterfaceDecl.objc_derive_declared",
            "Objc3InterfaceDecl.objc_derive_name",
            "Objc3FunctionDecl.objc_macro_declared",
            "Objc3FunctionDecl.objc_macro_name",
            "Objc3PropertyDecl.property_behavior_name",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_derive_macro_property_behavior_source_closure",
        ],
        "source_surface_model": (
            "derive-markers-macro-markers-and-property-behavior-markers-are-live-parser-owned-source-surfaces-before-semantic-expansion-lowering-or-runtime-materialization"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        ],
        "explicit_non_goals": [
            "no-runnable-macro-execution-claim",
            "no-derived-method-body-materialization-claim",
            "no-property-behavior-runtime-hook-claim",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_metaprogramming_package_provenance_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"metaprogramming-package-provenance-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/token/objc3_token_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3FunctionDecl.objc_macro_package_declared",
            "Objc3FunctionDecl.objc_macro_package_name",
            "Objc3FunctionDecl.objc_macro_provenance_declared",
            "Objc3FunctionDecl.objc_macro_provenance_name",
            "Objc3PropertyDecl.property_behavior_name",
            "Objc3PropertyDecl.executable_synthesized_binding_symbol",
            "Objc3PropertyDecl.effective_getter_selector",
            "Objc3PropertyDecl.effective_setter_selector",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_package_and_provenance_source_completion",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion",
        ],
        "source_surface_model": (
            "macro-package-macro-provenance-and-property-behavior-source-completion-freezes-expansion-visible-and-synthesized-declaration-state-before-semantic-expansion-lowering-or-runtime-materialization"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_behavior_source_completion_positive.objc3",
        ],
        "explicit_non_goals": [
            "no-macro-sandbox-execution-claim",
            "no-property-behavior-runtime-hook-claim",
            "no-executable-synthesized-declaration-materialization-claim",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_metaprogramming_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "metaprogramming-semantics",
            "metaprogramming-derive-property-behavior-semantics",
        }
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID,
        "semantic_contract_ids": [
            "objc3c.metaprogramming.expansion.behavior.semantic.model.v1",
        ],
        "source_dependency_contract_ids": [
            "objc3c.metaprogramming.metaprogramming.source.closure.v1",
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        ],
        "semantic_surface_model": (
            "derive-macro-package-provenance-and-property-behavior-source-packets-share-one-deterministic-sema-boundary-before-lowering-cache-integration-or-runtime-hooks"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_behavior_semantic_model_positive.objc3",
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
            "tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
            "tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
        ],
        "requires_real_compile_output": True,
    }


def build_runtime_object_model_realization_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.executable.realization.records.v1",
            "objc3c.runtime.class.realization.freeze.v1",
            "objc3c.runtime.metaclass.graph.root.class.baseline.v1",
            "objc3c.runtime.category.attachment.protocol.conformance.v1",
            "objc3c.runtime.canonical.runnable.object.sample.support.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_object_model_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_property_ivar_storage_accessor_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "accessor-storage-lowering-metadata-surface",
            "property-ivar-ordering-semantics",
            "property-reflection-accessor-compatibility-diagnostics",
            "property-synthesis-storage-binding-semantics",
            "storage-legality-semantics",
            "synthesized-accessor-codegen",
            "synthesized-accessor-runtime",
            "property-layout",
            "property-execution",
            "property-reflection",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.executable.property.ivar.source.closure.v1",
            "objc3c.executable.property.ivar.source.model.completion.v1",
            "objc3c.executable.property.ivar.semantics.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3PropertyDecl.ivar_binding_symbol",
            "Objc3PropertyDecl.executable_synthesized_binding_kind",
            "Objc3PropertyDecl.executable_synthesized_binding_symbol",
            "Objc3PropertyDecl.property_attribute_profile",
            "Objc3PropertyDecl.effective_getter_selector",
            "Objc3PropertyDecl.effective_setter_available",
            "Objc3PropertyDecl.effective_setter_selector",
            "Objc3PropertyDecl.accessor_ownership_profile",
            "Objc3PropertyDecl.executable_ivar_layout_symbol",
            "Objc3PropertyDecl.executable_ivar_layout_slot_index",
            "Objc3PropertyDecl.executable_ivar_layout_size_bytes",
            "Objc3PropertyDecl.executable_ivar_layout_alignment_bytes",
            "Objc3PropertyDecl.executable_ivar_init_order_index",
            "Objc3PropertyDecl.executable_ivar_destroy_order_index",
            "Objc3RuntimeMetadataPropertySourceRecord.synthesizes_executable_accessors",
            "Objc3RuntimeMetadataPropertySourceRecord.getter_storage_runtime_helper_symbol",
            "Objc3RuntimeMetadataPropertySourceRecord.setter_storage_runtime_helper_symbol",
            "Objc3ExecutableMetadataPropertyGraphNode.synthesizes_executable_accessors",
            "Objc3ExecutableMetadataPropertyGraphNode.getter_storage_runtime_helper_symbol",
            "Objc3ExecutableMetadataPropertyGraphNode.setter_storage_runtime_helper_symbol",
        ],
        "semantic_boundary_model": (
            "property-ivar-storage-accessor-source-surface-freezes-ast-sema-ir-pipeline-and-runtime-codepaths-before-lowering-or-runtime-semantic-expansion"
        ),
        "source_models": [
            "property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization",
            "property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles",
            "non-category-class-interface-properties-own-authoritative-default-ivar-and-synthesized-binding-identities-across-implementation-redeclaration-boundaries",
            "matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration",
            "readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission",
            "effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding",
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land",
            "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
            "protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols",
            "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path",
            "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-lowering-owned-storage-or-accessor-semantics-invention",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_block_arc_unified_source_surface(results: list[CaseResult]) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "block-arc-runtime-abi",
            "escaping-block-capture-legality",
            "block-storage-arc-automation-semantics",
            "block-helper-runtime-execution",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "source_surface_model": (
            "block-arc-unified-source-surface-freezes-live-frontend-sema-ir-and-runtime-entrypoints-before-generalized-ownership-automation-or-public-abi-widening"
        ),
        "source_contract_ids": [
            "objc3c.executable.block.source.closure.v1",
            "objc3c.executable.block.source.model.completion.v1",
            "objc3c.executable.block.source.storage.annotation.v1",
            "objc3c.executable.block.runtime.semantic.rules.v1",
            "objc3c.executable.block.capture.legality.escape.and.invocation.v1",
            "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1",
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
            "objc3c.executable.block.byref.helper.lowering.v1",
            "objc3c.executable.block.escape.runtime.hook.lowering.v1",
            "objc3c.arc.source.mode.boundary.freeze.v1",
            "objc3c.arc.mode.handling.v1",
            "objc3c.arc.semantic.rules.v1",
            "objc3c.arc.inference.lifetime.v1",
            "objc3c.arc.interaction.semantics.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_literal_capture_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_source_model_completion_surface",
            "frontend.pipeline.semantic_surface.objc_block_source_storage_annotation_surface",
            "frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface",
            "llvm_ir_summary.executable_block_object_invoke_thunk_lowering",
            "llvm_ir_summary.executable_block_byref_helper_lowering",
            "llvm_ir_summary.executable_block_escape_runtime_hook_lowering",
            "llvm_ir_summary.runtime_block_api_object_layout",
            "llvm_ir_summary.runtime_block_allocation_copy_dispose_invoke_support",
            "llvm_ir_summary.runtime_block_byref_forwarding_heap_promotion_ownership_interop",
            "runtime_api.objc3_runtime_promote_block_i32",
            "runtime_api.objc3_runtime_invoke_block_i32",
            "runtime_api.objc3_runtime_copy_arc_debug_state_for_testing",
            "runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
        ],
        "block_runtime_boundary_model": (
            "source-only-sema-rejects-escaping-byref-and-owned-object-captures-before-runnable-block-ownership-lowering"
        ),
        "arc_runtime_boundary_model": (
            "weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-stay-profiled-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/block_source_model_completion_positive.objc3",
            "tests/tooling/fixtures/native/block_source_storage_annotations_positive.objc3",
            "tests/tooling/fixtures/native/capture_legality_escape_invocation_bad_call.objc3",
            "tests/tooling/fixtures/native/capture_legality_escape_invocation_missing_capture.objc3",
            "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
            "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
            "tests/tooling/fixtures/native/arc_mode_handling_positive.objc3",
            "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
            "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
            "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
            "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
            BLOCK_ARC_RUNTIME_ABI_PROBE,
        ],
        "explicit_non_goals": [
            "no-public-block-object-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-block-or-arc-proof",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_ownership_transfer_capture_family_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "escaping-block-capture-legality",
            "block-storage-arc-automation-semantics",
            "block-helper-runtime-execution",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": (
            RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "source_surface_model": (
            "ownership-transfer-and-capture-family-source-surface-freezes-sema-level-move-capture-explicit-capture-mode-and-retainable-family-truth-before-lowering-or-runtime-lifetime-expansion"
        ),
        "ownership_resource_move_use_after_move_surface_path": (
            "frontend.pipeline.semantic_surface.objc_ownership_resource_move_and_use_after_move_semantics"
        ),
        "ownership_capture_list_retainable_family_surface_path": (
            "frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion"
        ),
        "block_capture_ownership_contract_id": (
            "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1"
        ),
        "arc_inference_lifetime_contract_id": "objc3c.arc.inference.lifetime.v1",
        "arc_interaction_semantics_contract_id": "objc3c.arc.interaction.semantics.v1",
        "block_capture_ownership_profile_field": (
            "Expr.block_runtime_capture_ownership_profile"
        ),
        "block_capture_owned_count_field": (
            "Expr.block_runtime_owned_object_capture_count"
        ),
        "block_capture_weak_count_field": (
            "Expr.block_runtime_weak_object_capture_count"
        ),
        "block_capture_unowned_count_field": (
            "Expr.block_runtime_unowned_object_capture_count"
        ),
        "cleanup_ownership_transfer_field": "cleanup_ownership_transfer_enforced",
        "explicit_capture_ownership_mode_field": (
            "explicit_capture_ownership_mode_enforced"
        ),
        "retainable_family_conflict_field": "retainable_family_conflict_enforced",
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3",
            "tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3",
            "tests/tooling/fixtures/native/nonowning_object_capture_helper_elided_positive.objc3",
            "tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3",
            "tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3",
            "tests/tooling/fixtures/native/unowned_object_capture_mutation_negative.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
            "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
            "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
            "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
            "tests/tooling/fixtures/native/arc_block_autorelease_return_positive.objc3",
            "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
            "tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-parallel-semantics-path",
            "no-milestone-specific-scaffolding",
            "no-lowering-owned-reinterpretation-of-capture-family-truth",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_block_arc_lowering_helper_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "block-arc-runtime-abi",
            "escaping-block-capture-legality",
            "block-storage-arc-automation-semantics",
            "block-helper-runtime-execution",
        }
    ]
    return {
        "contract_id": RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_block_arc_runtime_abi_surface_contract_id": (
            RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "ownership_transfer_capture_family_source_surface_contract_id": (
            RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "block_object_invoke_thunk_lowering_contract_id": (
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1"
        ),
        "block_byref_helper_lowering_contract_id": (
            "objc3c.executable.block.byref.helper.lowering.v1"
        ),
        "block_escape_runtime_hook_lowering_contract_id": (
            "objc3c.executable.block.escape.runtime.hook.lowering.v1"
        ),
        "arc_mode_handling_contract_id": "objc3c.arc.mode.handling.v1",
        "arc_semantic_rules_contract_id": "objc3c.arc.semantic.rules.v1",
        "arc_inference_lifetime_contract_id": "objc3c.arc.inference.lifetime.v1",
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "lowering_helper_surface_model": (
            "block-arc-lowering-helper-surface-freezes-live-semantic-lowering-packets-manifest-replay-keys-llvm-helper-summaries-and-private-runtime-hooks-before-cross-module-or-public-abi-expansion"
        ),
        "semantic_surface_paths": [
            "frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface",
        ],
        "manifest_lowering_paths": [
            "lowering_block_abi_invoke_trampoline",
            "lowering_block_storage_escape",
            "lowering_block_copy_dispose",
        ],
        "llvm_ir_summary_paths": [
            "llvm_ir_summary.executable_block_object_invoke_thunk_lowering",
            "llvm_ir_summary.executable_block_byref_helper_lowering",
            "llvm_ir_summary.executable_block_escape_runtime_hook_lowering",
            "llvm_ir_summary.arc_cleanup_weak_lifetime_hooks",
            "llvm_ir_summary.arc_block_autorelease_return_lowering",
        ],
        "runtime_api_paths": [
            "runtime_api.objc3_runtime_promote_block_i32",
            "runtime_api.objc3_runtime_invoke_block_i32",
            "runtime_api.objc3_runtime_copy_arc_debug_state_for_testing",
            "runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
            "tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3",
            "tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
            "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
            "tests/tooling/fixtures/native/arc_mode_handling_positive.objc3",
            "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
            "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
            "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
            "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
            BLOCK_ARC_RUNTIME_ABI_PROBE,
        ],
        "explicit_non_goals": [
            "no-cross-module-packaging-claims",
            "no-public-block-abi-widening",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_block_arc_runtime_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "block-arc-runtime-abi",
            "block-helper-runtime-execution",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "block_arc_lowering_helper_surface_contract_id": (
            RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_block_arc_runtime_abi_boundary": PRIVATE_BLOCK_ARC_RUNTIME_ABI_BOUNDARY,
        "block_arc_runtime_abi_snapshot_symbol": (
            "objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing"
        ),
        "arc_debug_state_snapshot_symbol": (
            "objc3_runtime_copy_arc_debug_state_for_testing"
        ),
        "runtime_abi_boundary_model": BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
        "block_runtime_model": BLOCK_ARC_RUNTIME_BLOCK_MODEL,
        "arc_runtime_model": BLOCK_ARC_RUNTIME_ARC_MODEL,
        "fail_closed_model": BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_path": BLOCK_ARC_RUNTIME_ABI_PROBE,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_property_atomicity_synthesis_reflection_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-reflection-accessor-compatibility-diagnostics",
            "storage-legality-semantics",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": (
            RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/sema/objc3_sema_pass_manager.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3PropertyDecl.is_atomic",
            "Objc3PropertyDecl.is_nonatomic",
            "Objc3PropertyDecl.has_atomicity_conflict",
            "Objc3PropertyDecl.property_attribute_profile",
            "objc3_runtime_property_entry_snapshot.property_attribute_profile",
        ],
        "source_surface_model": (
            "property-atomicity-synthesis-reflection-source-surface-freezes-atomicity-flags-conflict-state-attribute-profiles-and-private-reflection-codepaths-before-runtime-managed-atomic-storage-semantics-land"
        ),
        "atomicity_fail_closed_model": (
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land"
        ),
        "reflection_boundary_model": (
            "property-attribute-profiles-remain-the-authoritative-reflection-carrier-for-atomicity-and-synthesis-state-on-the-private-property-query-boundary"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/property_atomic_ownership_negative.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-atomic-property-runtime-abi-widening",
            "no-runtime-managed-atomic-storage-semantics-before-lane-b-and-lane-d-implementation",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_dispatch_and_synthesized_accessor_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "accessor-storage-lowering-metadata-surface",
            "property-synthesis-storage-binding-semantics",
            "synthesized-accessor-codegen",
            "synthesized-accessor-runtime",
            "property-layout",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.executable.property.accessor.layout.lowering.v1",
            "objc3c.executable.ivar.layout.emission.v1",
            "objc3c.executable.synthesized.accessor.property.lowering.v1",
            RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "lowering_metadata_model": (
            "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path"
        ),
        "helper_selection_model": (
            "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-lowering-proof",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_executable_property_accessor_layout_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-accessor-layout-lowering",
            "property-synthesis-storage-binding-semantics",
            "property-layout",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
            DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        ],
        "property_table_model": (
            "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records"
        ),
        "ivar_layout_model": (
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-slot-size-alignment-records"
        ),
        "accessor_binding_model": (
            "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-layout-or-accessor-body-rederivation-outside-the-live-lowering-path",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_executable_ivar_layout_emission_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-accessor-layout-lowering",
            "property-ivar-ordering-semantics",
            "property-layout",
            "property-execution",
        }
    ]
    return {
        "contract_id": EXECUTABLE_IVAR_LAYOUT_EMISSION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        ],
        "descriptor_model": (
            "ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment"
        ),
        "offset_global_model": (
            "one-retained-i64-offset-global-per-emitted-ivar-binding"
        ),
        "layout_table_model": (
            "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-runtime-layout-rederivation",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_executable_synthesized_accessor_property_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-accessor-layout-lowering",
            "property-synthesis-storage-binding-semantics",
            "synthesized-accessor-codegen",
            "synthesized-accessor-runtime",
            "property-layout",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": (
            EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_LOWERING_SURFACE_CONTRACT_ID
        ),
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
            DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "source_model": (
            "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists"
        ),
        "storage_model": (
            "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals"
        ),
        "property_descriptor_model": (
            "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-storage-global-fallbacks-or-sidecar-body-proof",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lowering_reflection_artifact_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.compile-provenance.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
            "objc3c.executable.realization.records.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "lowering_artifact_boundary_model": (
            "compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts"
        ),
        "reflection_artifact_handoff_model": (
            "property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_reflection_artifact_query_boundary": [
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_compile_output_truthfulness": True,
    }


def build_runtime_dispatch_table_reflection_record_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "dispatch-fast-path",
            "property-execution",
        }
    ]
    return {
        "contract_id": RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.compile-provenance.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
            "objc3c.method.dispatch.selector.thunk.lowering.v1",
            "objc3c.executable.realization.records.v1",
        ],
        "dispatch_table_lowering_model": (
            "selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts"
        ),
        "reflection_record_lowering_model": (
            "realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        ],
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_compile_output_truthfulness": True,
    }


def build_runtime_cross_module_realized_metadata_replay_preservation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "imported-runtime-packaging-replay"
    ]
    return {
        "contract_id": RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.cross-module-runtime-linker-options.rsp",
        ],
        "source_contract_ids": [
            "objc3c.cross.module.runtime.packaging.link.plan.v1",
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "realized_metadata_replay_preservation_model": (
            "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        ],
        "authoritative_probe_paths": [IMPORTED_RUNTIME_PACKAGING_PROBE],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_object_model_abi_query_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "canonical-sample-set",
            "realization-lookup-reflection-runtime",
            "dispatch-fast-path",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ],
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_object_model_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_selector_lookup_table_state_for_testing",
            "objc3_runtime_copy_selector_lookup_entry_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
            "objc3_runtime_copy_dispatch_state_for_testing",
        ],
        "object_model_query_boundary_model": (
            "public-runtime-header-plus-private-testing-snapshots-freeze-the-object-model-lookup-and-reflection-query-surface-without-widening-the-public-abi"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lookup_reflection_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "realization-lookup-reflection-runtime",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ],
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "object_model_query_state_snapshot_symbol": (
            "objc3_runtime_copy_object_model_query_state_for_testing"
        ),
        "realized_class_entry_snapshot_symbol": (
            "objc3_runtime_copy_realized_class_entry_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "protocol_conformance_query_symbol": (
            "objc3_runtime_copy_protocol_conformance_query_for_testing"
        ),
        "selector_lookup_table_state_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_table_state_for_testing"
        ),
        "selector_lookup_entry_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_entry_for_testing"
        ),
        "method_cache_state_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_state_for_testing"
        ),
        "method_cache_entry_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_entry_for_testing"
        ),
        "dispatch_state_snapshot_symbol": "objc3_runtime_copy_dispatch_state_for_testing",
        "realization_lookup_reflection_implementation_model": (
            "runtime-owned-class-property-protocol-and-method-cache-query-snapshots-publish-coherent-last-query-state-and-live-object-model-counts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_reflection_query_surface(results: list[CaseResult]) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-sample-set",
            "property-reflection",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "query_api_boundary_model": (
            "private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_query_symbols": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "no_public_reflection_abi": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lookup_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_lookup_query_boundary": [
            "objc3_runtime_copy_selector_lookup_table_state_for_testing",
            "objc3_runtime_copy_selector_lookup_entry_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "lookup_resolution_order_model": (
            "seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-deterministic-fallback"
        ),
        "selector_materialization_model": (
            "metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup"
        ),
        "unresolved_selector_behavior_model": (
            "negative-cache-entry-preserved-and-deterministic-fallback-returned"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_class_metaclass_protocol_realization_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
        }
    ]
    return {
        "contract_id": RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_realization_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "class_realization_model": (
            "registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection"
        ),
        "metaclass_lineage_model": (
            "realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities"
        ),
        "protocol_conformance_model": (
            "realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_category_attachment_merged_dispatch_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "canonical-sample-set",
        }
    ]
    return {
        "contract_id": RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.runtime.category.attachment.protocol.conformance.v1",
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_category_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
        ],
        "category_attachment_model": (
            "registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch"
        ),
        "merged_dispatch_resolution_model": (
            "attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-fallback"
        ),
        "attached_protocol_visibility_model": (
            "attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_reflection_visibility_coherence_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-sample-set",
            "property-reflection-accessor-compatibility-diagnostics",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_coherence_query_boundary": [
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
        ],
        "reflection_visibility_boundary_model": (
            "private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state"
        ),
        "fail_closed_lookup_diagnostic_model": (
            "missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state"
        ),
        "runtime_coherence_diagnostic_model": (
            "reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_acceptance_suite_surface(results: list[CaseResult], report_path: Path) -> dict[str, Any]:
    compile_coupled_case_ids = [result.case_id for result in results if result.fixture is not None]
    linked_runtime_probe_case_ids = [
        result.case_id for result in results if result.claim_class == "linked-runtime-probe"
    ]
    compile_coupled_inspection_case_ids = [
        result.case_id for result in results if result.claim_class == "compile-coupled-inspection"
    ]
    return {
        "contract_id": RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
        "suite_path": "scripts/check_objc3c_runtime_acceptance.py",
        "report_path": str(report_path.relative_to(ROOT)).replace("\\", "/"),
        "consumes_runtime_state_publication_surface_contract_id": RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
        "authoritative_claim_classes": [
            "linked-runtime-probe",
            "compile-coupled-inspection",
        ],
        "linked_runtime_probe_case_ids": linked_runtime_probe_case_ids,
        "compile_coupled_case_ids": compile_coupled_case_ids,
        "compile_coupled_inspection_case_ids": compile_coupled_inspection_case_ids,
        "compile_output_provenance_contract_id": COMPILE_PROVENANCE_CONTRACT_ID,
        "compile_output_truthfulness_contract_id": COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
        "coupled_artifact_requirements": [
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.compile-provenance.json",
        ],
    }


def build_runtime_installation_abi_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "bootstrap_api_contract_id": "objc3c.runtime.bootstrap.api.freeze.v1",
        "bootstrap_reset_contract_id": "objc3c.runtime.bootstrap.reset.replay.v1",
        "bootstrap_registrar_contract_id": "objc3c.runtime.bootstrap.registrar.image.walk.v1",
        "public_installation_abi_boundary": RUNTIME_INSTALLATION_ABI_BOUNDARY,
        "private_loader_testing_boundary": RUNTIME_LOADER_TESTING_BOUNDARY,
        "installation_requires_coupled_registration_manifest": True,
        "register_image_consumes_staged_registration_table_once": True,
        "deterministic_reset_replay_supported": True,
    }


def build_runtime_loader_lifecycle_surface(results: list[CaseResult]) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id for result in results if result.case_id == "installation-lifecycle"
    ]
    return {
        "contract_id": RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID,
        "runtime_installation_abi_surface_contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "bootstrap_semantics_contract_id": "objc3c.runtime.startup.bootstrap.semantics.v1",
        "bootstrap_reset_contract_id": "objc3c.runtime.bootstrap.reset.replay.v1",
        "bootstrap_registrar_contract_id": "objc3c.runtime.bootstrap.registrar.image.walk.v1",
        "authoritative_probe_path": INSTALLATION_LIFECYCLE_PROBE,
        "authoritative_case_ids": authoritative_case_ids,
        "loader_testing_boundary_symbols": RUNTIME_LOADER_TESTING_BOUNDARY,
        "lifecycle_phases": [
            "startup-installed-runtime-state",
            "duplicate-registration-rejected-without-state-advance",
            "out-of-order-registration-rejected-without-state-advance",
            "invalid-anchor-root-rejected-without-state-advance",
            "invalid-discovery-root-rejected-without-state-advance",
            "reset-retained-bootstrap-catalog",
            "replay-restored-installed-runtime-state",
        ],
        "rejected_registration_status_codes": {
            "duplicate_translation_unit_identity_key": -2,
            "out_of_order_registration": -3,
            "invalid_registration_roots": -4,
        },
        "retained_bootstrap_catalog_required": True,
        "deterministic_replay_required": True,
        "requires_linked_fixture_or_loader_retained_roots": True,
    }


def check_runtime_library_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "runtime-library"
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_library_probe.cpp"
    exe_path = case_dir / "runtime_library_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    run_probe(exe_path)
    return CaseResult(
        case_id="runtime-library",
        probe="tests/tooling/runtime/runtime_library_probe.cpp",
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={"kind": "standalone-runtime-probe"},
    )


def check_installation_lifecycle_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "installation-lifecycle"
    fixture = ROOT / Path(INSTALLATION_LIFECYCLE_FIXTURE)
    probe = ROOT / Path(INSTALLATION_LIFECYCLE_PROBE)
    compile_started = perf_counter()
    obj_path = compile_fixture(fixture, case_dir / "compile")
    fixture_compile_ms = int((perf_counter() - compile_started) * 1000)
    registration_descriptor = json.loads(
        (case_dir / "compile" / "module.runtime-registration-descriptor.json").read_text(
            encoding="utf-8"
        )
    )
    probe_fixture_config = case_dir / "runtime_installation_loader_lifecycle_probe_fixture_config.h"
    probe_fixture_config.write_text(
        "\n".join(
            [
                "#pragma once",
                f"#define OBJC3_RUNTIME_FIXTURE_MODULE_NAME {json.dumps(registration_descriptor['registration_descriptor_identifier'].removesuffix('_registration_descriptor'))}",
                f"#define OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY {json.dumps(registration_descriptor['translation_unit_identity_key'])}",
                f"#define OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL {registration_descriptor['translation_unit_registration_order_ordinal']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT {registration_descriptor['class_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT {registration_descriptor['protocol_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT {registration_descriptor['category_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT {registration_descriptor['property_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT {registration_descriptor['ivar_descriptor_count']}ULL",
                "",
            ]
        ),
        encoding="utf-8",
    )
    exe_path = case_dir / "runtime_installation_loader_lifecycle_probe.exe"
    probe_link_started = perf_counter()
    compile_probe_with_args(
        clangxx,
        probe,
        exe_path,
        [obj_path],
        [
            "-include",
            str(probe_fixture_config),
        ],
    )
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)
    probe_run_started = perf_counter()
    payload = parse_json_output(run_probe(exe_path), "runtime installation loader lifecycle probe")
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    expect(payload.get("startup_registration_copy_status") == 0, "expected startup registration snapshot copy to succeed")
    expect(payload.get("startup_image_walk_copy_status") == 0, "expected startup image walk snapshot copy to succeed")
    expect(payload.get("startup_reset_replay_copy_status") == 0, "expected startup reset/replay snapshot copy to succeed")
    expect(payload.get("startup_registered_image_count") == 1, "expected startup runtime installation state to contain one registered image")
    expect(payload.get("startup_next_expected_registration_order_ordinal") == 2, "expected startup installation state to advance the next registration ordinal")
    expect(payload.get("startup_walked_image_count") == 1, "expected startup image walk state to publish one walked image")
    expect(payload.get("startup_last_discovery_root_entry_count", 0) > 0, "expected startup image walk state to publish a non-empty discovery root")
    expect(payload.get("startup_last_registration_used_staged_table") == 1, "expected startup installation state to consume the staged registration table")
    expect(payload.get("startup_retained_bootstrap_image_count") == 1, "expected startup reset/replay state to retain one bootstrap image")

    startup_module_name = payload.get("startup_last_registered_module_name")
    startup_identity_key = payload.get("startup_last_registered_translation_unit_identity_key")
    expect(isinstance(startup_module_name, str) and startup_module_name != "", "expected startup installation state to publish a registered module name")
    expect(isinstance(startup_identity_key, str) and startup_identity_key != "", "expected startup installation state to publish a registered translation unit identity key")
    expect(payload.get("duplicate_status") == -2, "expected duplicate registration to fail with duplicate translation-unit identity status")
    expect(payload.get("after_duplicate_registration_copy_status") == 0, "expected duplicate rejection registration snapshot copy to succeed")
    expect(payload.get("after_duplicate_image_walk_copy_status") == 0, "expected duplicate rejection image walk snapshot copy to succeed")
    expect(payload.get("after_duplicate_registered_image_count") == 1, "expected duplicate rejection to leave installed image count unchanged")
    expect(payload.get("after_duplicate_next_expected_registration_order_ordinal") == 2, "expected duplicate rejection to preserve the next expected registration ordinal")
    expect(payload.get("after_duplicate_last_successful_registration_order_ordinal") == 1, "expected duplicate rejection to preserve the last successful registration ordinal")
    expect(payload.get("after_duplicate_last_registration_status") == -2, "expected duplicate rejection snapshot to publish duplicate registration status")
    expect(payload.get("after_duplicate_last_rejected_module_name") == startup_module_name, "expected duplicate rejection snapshot to publish the rejected module name")
    expect(payload.get("after_duplicate_last_rejected_translation_unit_identity_key") == startup_identity_key, "expected duplicate rejection snapshot to publish the rejected translation unit identity key")
    expect(payload.get("after_duplicate_last_rejected_registration_order_ordinal") == 1, "expected duplicate rejection snapshot to publish the rejected registration ordinal")
    expect(payload.get("after_duplicate_walked_image_count") == 1, "expected duplicate rejection to leave image walk state unchanged")
    expect(payload.get("out_of_order_status") == -3, "expected out-of-order registration to fail with out-of-order status")
    expect(payload.get("after_out_of_order_registration_copy_status") == 0, "expected out-of-order rejection registration snapshot copy to succeed")
    expect(payload.get("after_out_of_order_image_walk_copy_status") == 0, "expected out-of-order rejection image walk snapshot copy to succeed")
    expect(payload.get("after_out_of_order_registered_image_count") == 1, "expected out-of-order rejection to leave installed image count unchanged")
    expect(payload.get("after_out_of_order_next_expected_registration_order_ordinal") == 2, "expected out-of-order rejection to preserve the next expected registration ordinal")
    expect(payload.get("after_out_of_order_last_successful_registration_order_ordinal") == 1, "expected out-of-order rejection to preserve the last successful registration ordinal")
    expect(payload.get("after_out_of_order_last_registration_status") == -3, "expected out-of-order rejection snapshot to publish out-of-order status")
    expect(payload.get("after_out_of_order_last_rejected_module_name") == "out-of-order-module", "expected out-of-order rejection snapshot to publish the rejected module name")
    expect(
        payload.get("after_out_of_order_last_rejected_translation_unit_identity_key")
        == startup_identity_key + "-out-of-order",
        "expected out-of-order rejection snapshot to publish the rejected translation unit identity key",
    )
    expect(payload.get("after_out_of_order_last_rejected_registration_order_ordinal") == 3, "expected out-of-order rejection snapshot to publish the rejected registration ordinal")
    expect(payload.get("after_out_of_order_walked_image_count") == 1, "expected out-of-order rejection to leave image walk state unchanged")

    expect(payload.get("post_reset_registration_copy_status") == 0, "expected post-reset registration snapshot copy to succeed")
    expect(payload.get("post_reset_reset_replay_copy_status") == 0, "expected post-reset reset/replay snapshot copy to succeed")
    expect(payload.get("post_reset_registered_image_count") == 0, "expected reset to clear installed runtime images")
    expect(payload.get("post_reset_next_expected_registration_order_ordinal") == 1, "expected reset to restore the initial registration ordinal")
    expect(payload.get("post_reset_retained_bootstrap_image_count") == 1, "expected reset to retain one bootstrap image for replay")
    expect(payload.get("post_reset_last_reset_cleared_image_local_init_state_count") == 1, "expected reset to clear one image-local initialization state record")
    expect(payload.get("post_reset_invalid_anchor_status") == -4, "expected mismatched linker-anchor registration to fail with invalid registration roots status")
    expect(payload.get("after_invalid_anchor_registration_copy_status") == 0, "expected invalid-anchor rejection registration snapshot copy to succeed")
    expect(payload.get("after_invalid_anchor_image_walk_copy_status") == 0, "expected invalid-anchor rejection image walk snapshot copy to succeed")
    expect(payload.get("after_invalid_anchor_registered_image_count") == 0, "expected invalid-anchor rejection to preserve the cleared installed image count")
    expect(payload.get("after_invalid_anchor_next_expected_registration_order_ordinal") == 1, "expected invalid-anchor rejection to preserve the reset registration ordinal")
    expect(payload.get("after_invalid_anchor_last_registration_status") == -4, "expected invalid-anchor rejection snapshot to publish invalid registration roots status")
    expect(payload.get("after_invalid_anchor_walked_image_count") == 0, "expected invalid-anchor rejection to leave image walk state empty")
    expect(payload.get("after_invalid_anchor_last_linker_anchor_matches_discovery_root") == 0, "expected invalid-anchor rejection to leave linker-anchor/discovery-root proof unset")
    expect(payload.get("post_reset_invalid_discovery_root_status") == -4, "expected malformed discovery-root registration to fail with invalid registration roots status")
    expect(payload.get("after_invalid_discovery_root_registration_copy_status") == 0, "expected invalid-discovery-root rejection registration snapshot copy to succeed")
    expect(payload.get("after_invalid_discovery_root_image_walk_copy_status") == 0, "expected invalid-discovery-root rejection image walk snapshot copy to succeed")
    expect(payload.get("after_invalid_discovery_root_registered_image_count") == 0, "expected invalid-discovery-root rejection to preserve the cleared installed image count")
    expect(payload.get("after_invalid_discovery_root_next_expected_registration_order_ordinal") == 1, "expected invalid-discovery-root rejection to preserve the reset registration ordinal")
    expect(payload.get("after_invalid_discovery_root_last_registration_status") == -4, "expected invalid-discovery-root rejection snapshot to publish invalid registration roots status")
    expect(payload.get("after_invalid_discovery_root_walked_image_count") == 0, "expected invalid-discovery-root rejection to leave image walk state empty")
    expect(payload.get("after_invalid_discovery_root_last_linker_anchor_matches_discovery_root") == 0, "expected invalid-discovery-root rejection to leave linker-anchor/discovery-root proof unset")

    expect(payload.get("replay_status") == 0, "expected replay_registered_images_for_testing to succeed")
    expect(payload.get("post_replay_registration_copy_status") == 0, "expected post-replay registration snapshot copy to succeed")
    expect(payload.get("post_replay_image_walk_copy_status") == 0, "expected post-replay image walk snapshot copy to succeed")
    expect(payload.get("post_replay_reset_replay_copy_status") == 0, "expected post-replay reset/replay snapshot copy to succeed")
    expect(payload.get("post_replay_registered_image_count") == 1, "expected replay to restore one registered runtime image")
    expect(payload.get("post_replay_next_expected_registration_order_ordinal") == 2, "expected replay to restore the next registration ordinal")
    expect(payload.get("post_replay_walked_image_count") == 1, "expected replay to restore one walked image")
    expect(payload.get("post_replay_last_discovery_root_entry_count") == payload.get("startup_last_discovery_root_entry_count"), "expected replay to preserve discovery root entry count")
    expect(payload.get("post_replay_last_registration_used_staged_table") == 1, "expected replay registration to consume the staged registration table")
    expect(payload.get("post_replay_retained_bootstrap_image_count") == 1, "expected replay to preserve the retained bootstrap catalog")
    expect(payload.get("post_replay_last_replayed_image_count") == 1, "expected replay state to publish one replayed image")
    expect(payload.get("post_replay_replay_generation", 0) >= 1, "expected replay state to advance the replay generation")
    expect(payload.get("post_replay_last_replay_status") == 0, "expected replay state to publish a successful replay status")
    expect(payload.get("post_replay_last_registered_module_name") == startup_module_name, "expected replay to restore the registered module name")
    expect(payload.get("post_replay_last_walked_module_name") == startup_module_name, "expected replay image walk state to publish the registered module name")
    expect(payload.get("post_replay_last_replayed_module_name") == startup_module_name, "expected replay state to publish the replayed module name")
    expect(payload.get("post_replay_last_registered_translation_unit_identity_key") == startup_identity_key, "expected replay to restore the registered translation unit identity key")
    expect(payload.get("post_replay_last_walked_translation_unit_identity_key") == startup_identity_key, "expected replay image walk state to publish the translation unit identity key")
    expect(payload.get("post_replay_last_replayed_translation_unit_identity_key") == startup_identity_key, "expected replay state to publish the replayed translation unit identity key")

    return CaseResult(
        case_id="installation-lifecycle",
        probe="tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "fixture_compile_ms": fixture_compile_ms,
            "probe_link_ms": probe_link_ms,
            "probe_run_ms": probe_run_ms,
            "case_total_ms": case_total_ms,
            "duplicate_status": payload["duplicate_status"],
            "duplicate_rejected_module_name": payload["after_duplicate_last_rejected_module_name"],
            "duplicate_rejected_translation_unit_identity_key": payload[
                "after_duplicate_last_rejected_translation_unit_identity_key"
            ],
            "duplicate_rejected_registration_order_ordinal": payload[
                "after_duplicate_last_rejected_registration_order_ordinal"
            ],
            "out_of_order_status": payload["out_of_order_status"],
            "out_of_order_rejected_module_name": payload[
                "after_out_of_order_last_rejected_module_name"
            ],
            "out_of_order_rejected_translation_unit_identity_key": payload[
                "after_out_of_order_last_rejected_translation_unit_identity_key"
            ],
            "out_of_order_rejected_registration_order_ordinal": payload[
                "after_out_of_order_last_rejected_registration_order_ordinal"
            ],
            "invalid_anchor_status": payload["post_reset_invalid_anchor_status"],
            "invalid_discovery_root_status": payload["post_reset_invalid_discovery_root_status"],
            "startup_registered_image_count": payload["startup_registered_image_count"],
            "post_reset_registered_image_count": payload["post_reset_registered_image_count"],
            "post_replay_registered_image_count": payload["post_replay_registered_image_count"],
            "retained_bootstrap_image_count": payload["post_replay_retained_bootstrap_image_count"],
            "replay_generation": payload["post_replay_replay_generation"],
        },
    )


def check_metaprogramming_source_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "metaprogramming-source-surface"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "expansion_lowering_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    semantic_surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_derive_macro_property_behavior_source_closure", {})
    )
    expect(
        isinstance(semantic_surface, dict),
        "expected metaprogramming source fixture to publish objc_metaprogramming_derive_macro_property_behavior_source_closure",
    )
    expected_fields = {
        "contract_id": "objc3c.metaprogramming.metaprogramming.source.closure.v1",
        "frontend_surface_path": (
            "frontend.pipeline.semantic_surface."
            "objc_metaprogramming_derive_macro_property_behavior_source_closure"
        ),
        "source_model": (
            "derive-markers-macro-markers-and-property-behavior-markers-are-live-parser-owned-source-surfaces-while-expansion-synthesis-and-runtime-behavior-remain-deferred"
        ),
        "failure_model": (
            "metaprogramming-stays-source-closure-only-with-no-macro-expansion-derived-conformance-synthesis-or-property-runtime-claims-yet"
        ),
    }
    for field, expected_value in expected_fields.items():
        expect(
            semantic_surface.get(field) == expected_value,
            f"expected metaprogramming source surface to preserve {field}",
        )
    expect(
        semantic_surface.get("source_only_claim_ids")
        == [
            "source-only:derive-markers",
            "source-only:macro-markers",
            "source-only:property-behavior-markers",
        ],
        "expected metaprogramming source surface to preserve source_only_claim_ids",
    )
    expect(
        semantic_surface.get("derive_marker_sites") == 1,
        "expected metaprogramming source surface to publish one derive marker site",
    )
    expect(
        semantic_surface.get("macro_marker_sites") == 1,
        "expected metaprogramming source surface to publish one macro marker site",
    )
    expect(
        semantic_surface.get("property_behavior_sites") == 2,
        "expected metaprogramming source surface to publish two property behavior sites",
    )
    expect(
        semantic_surface.get("derive_marker_source_supported") is True
        and semantic_surface.get("macro_marker_source_supported") is True
        and semantic_surface.get("property_behavior_source_supported") is True,
        "expected metaprogramming source surface to preserve source support flags",
    )
    expect(
        semantic_surface.get("deterministic_handoff") is True
        and semantic_surface.get("ready_for_semantic_expansion") is True,
        "expected metaprogramming source surface to preserve deterministic source handoff",
    )

    return CaseResult(
        case_id="metaprogramming-source-surface",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "surface": "objc_metaprogramming_derive_macro_property_behavior_source_closure",
            "contract_id": semantic_surface.get("contract_id"),
            "derive_marker_sites": semantic_surface.get("derive_marker_sites"),
            "macro_marker_sites": semantic_surface.get("macro_marker_sites"),
            "property_behavior_sites": semantic_surface.get("property_behavior_sites"),
        },
    )


def check_metaprogramming_package_provenance_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-package-provenance-source-surface"
    fixtures: dict[str, tuple[Path, str, str]] = {
        "macro_package_provenance": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_lowering_positive.objc3",
            "objc_metaprogramming_macro_package_and_provenance_source_completion",
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
        ),
        "property_behavior_source_completion": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_source_completion_positive.objc3",
            "objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        semantic_surface_name,
        expected_contract_id,
    ) in fixtures.items():
        _, _, manifest_path = compile_fixture_outputs(fixture_path, case_dir / fixture_key / "compile")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(semantic_surface_name, {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish {semantic_surface_name}",
        )
        expect(
            semantic_surface.get("contract_id") == expected_contract_id,
            f"expected {fixture_key} fixture to preserve {expected_contract_id}",
        )
        expect(
            semantic_surface.get("deterministic_handoff") is True
            and semantic_surface.get("ready_for_semantic_expansion") is True,
            f"expected {fixture_key} fixture to preserve deterministic source completion handoff",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "surface": semantic_surface_name,
            "contract_id": semantic_surface.get("contract_id"),
        }
        if fixture_key == "macro_package_provenance":
            expect(
                semantic_surface.get("macro_marker_sites") == 1
                and semantic_surface.get("macro_package_sites") == 1
                and semantic_surface.get("macro_provenance_sites") == 1
                and semantic_surface.get("expansion_visible_macro_sites") == 1,
                "expected macro package/provenance source completion to preserve marker and visibility counts",
            )
            expect(
                semantic_surface.get("macro_package_source_supported") is True
                and semantic_surface.get("macro_provenance_source_supported") is True
                and semantic_surface.get("expansion_visible_source_supported") is True,
                "expected macro package/provenance source completion to preserve support flags",
            )
        else:
            expect(
                semantic_surface.get("property_behavior_sites") == 5
                and semantic_surface.get("interface_property_behavior_sites") == 2
                and semantic_surface.get("implementation_property_behavior_sites") == 2
                and semantic_surface.get("protocol_property_behavior_sites") == 1,
                "expected property-behavior source completion to preserve property behavior counts",
            )
            expect(
                semantic_surface.get("synthesized_binding_visible_sites") == 4
                and semantic_surface.get("synthesized_getter_visible_sites") == 5
                and semantic_surface.get("synthesized_setter_visible_sites") == 2,
                "expected property-behavior source completion to preserve synthesized declaration visibility counts",
            )
            expect(
                semantic_surface.get("property_behavior_source_supported") is True
                and semantic_surface.get("synthesized_declaration_visibility_supported")
                is True,
                "expected property-behavior source completion to preserve support flags",
            )

    return CaseResult(
        case_id="metaprogramming-package-provenance-source-surface",
        probe="compile-manifest-source-completion-surface",
        fixture="tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )


def check_metaprogramming_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "metaprogramming-semantics"
    fixtures: dict[str, tuple[Path, int, int, int]] = {
        "semantic_model": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_behavior_semantic_model_positive.objc3",
            0,
            1,
            2,
        ),
        "lowering_ready": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_lowering_positive.objc3",
            1,
            1,
            2,
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        expected_derive_marker_sites,
        expected_macro_marker_sites,
        expected_property_behavior_sites,
    ) in fixtures.items():
        _, _, manifest_path = compile_fixture_outputs(fixture_path, case_dir / fixture_key / "compile")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get("objc_metaprogramming_expansion_and_behavior_semantic_model", {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish objc_metaprogramming_expansion_and_behavior_semantic_model",
        )
        expect(
            semantic_surface.get("contract_id")
            == "objc3c.metaprogramming.expansion.behavior.semantic.model.v1",
            f"expected {fixture_key} fixture to preserve the metaprogramming expansion behavior semantic contract",
        )
        expect(
            semantic_surface.get("frontend_dependency_contract_id")
            == "objc3c.metaprogramming.property.behavior.source.completion.v1",
            f"expected {fixture_key} fixture to preserve the property behavior source dependency contract",
        )
        expect(
            semantic_surface.get("derive_marker_sites") == expected_derive_marker_sites
            and semantic_surface.get("macro_marker_sites") == expected_macro_marker_sites
            and semantic_surface.get("property_behavior_sites")
            == expected_property_behavior_sites,
            f"expected {fixture_key} fixture to preserve semantic site counts",
        )
        expect(
            semantic_surface.get("macro_package_provenance_surface_reused") is True
            and semantic_surface.get("property_behavior_source_supported") is True
            and semantic_surface.get("synthesized_visibility_surface_reused")
            is True,
            f"expected {fixture_key} fixture to preserve semantic surface reuse flags",
        )
        expect(
            semantic_surface.get("derive_synthesis_deferred") is True
            and semantic_surface.get("macro_execution_deferred") is True
            and semantic_surface.get("property_behavior_runtime_deferred") is True,
            f"expected {fixture_key} fixture to preserve deferred runtime semantics",
        )
        expect(
            semantic_surface.get("deterministic") is True
            and semantic_surface.get("ready_for_core_implementation") is True,
            f"expected {fixture_key} fixture to preserve deterministic semantic readiness",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "contract_id": semantic_surface.get("contract_id"),
            "derive_marker_sites": semantic_surface.get("derive_marker_sites"),
            "macro_marker_sites": semantic_surface.get("macro_marker_sites"),
            "property_behavior_sites": semantic_surface.get("property_behavior_sites"),
        }

    return CaseResult(
        case_id="metaprogramming-semantics",
        probe="compile-manifest-semantic-surface",
        fixture="tests/tooling/fixtures/native/expansion_behavior_semantic_model_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )


def check_metaprogramming_derive_property_behavior_semantics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-derive-property-behavior-semantics"
    derive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "derive_expansion_inventory_positive.objc3"
    )
    property_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_behavior_legality_positive.objc3"
    )
    _, _, derive_manifest_path = compile_fixture_outputs(
        derive_fixture, case_dir / "derive-positive" / "compile"
    )
    _, _, property_manifest_path = compile_fixture_outputs(
        property_fixture, case_dir / "property-positive" / "compile"
    )
    derive_manifest = json.loads(derive_manifest_path.read_text(encoding="utf-8"))
    property_manifest = json.loads(property_manifest_path.read_text(encoding="utf-8"))
    derive_surface = (
        derive_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_derive_expansion_inventory", {})
    )
    property_surface = (
        property_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get(
            "objc_metaprogramming_property_behavior_legality_and_interaction_completion",
            {},
        )
    )
    expect(
        derive_surface.get("contract_id")
        == "objc3c.metaprogramming.derive.expansion.inventory.v1",
        "expected derive expansion inventory fixture to preserve the derive inventory contract",
    )
    expect(
        derive_surface.get("derive_request_sites") == 4
        and derive_surface.get("supported_derive_request_sites") == 4
        and derive_surface.get("generated_method_entry_count") == 4,
        "expected derive expansion inventory fixture to preserve supported derive counts",
    )
    expect(
        derive_surface.get("equatable_alias_sites") == 1
        and derive_surface.get("equality_derive_sites") == 2
        and derive_surface.get("hash_derive_sites") == 1
        and derive_surface.get("debug_description_derive_sites") == 1,
        "expected derive expansion inventory fixture to preserve derive family counts",
    )
    expect(
        derive_surface.get("unsupported_derive_fail_closed") is True
        and derive_surface.get("selector_conflicts_fail_closed") is True
        and derive_surface.get("deterministic") is True
        and derive_surface.get("ready_for_lowering_and_runtime") is True,
        "expected derive expansion inventory fixture to preserve fail-closed readiness",
    )
    expect(
        property_surface.get("contract_id")
        == "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1",
        "expected property behavior legality fixture to preserve the legality contract",
    )
    expect(
        property_surface.get("property_behavior_sites") == 5
        and property_surface.get("supported_behavior_sites") == 5
        and property_surface.get("unsupported_behavior_sites") == 0,
        "expected property behavior legality fixture to preserve behavior counts",
    )
    expect(
        property_surface.get("observed_behavior_sites") == 2
        and property_surface.get("projected_behavior_sites") == 3,
        "expected property behavior legality fixture to preserve observed/projected counts",
    )
    expect(
        property_surface.get("unsupported_behavior_fail_closed") is True
        and property_surface.get("owner_topology_fail_closed") is True
        and property_surface.get("interaction_legality_fail_closed") is True
        and property_surface.get("storage_legality_fail_closed") is True
        and property_surface.get("deterministic") is True
        and property_surface.get("ready_for_lowering_and_runtime") is True,
        "expected property behavior legality fixture to preserve fail-closed readiness",
    )

    negative_fixtures = {
        "unsupported_derive": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "derive_expansion_inventory_negative_unsupported.objc3",
            "O3S317",
            "unsupported derive 'Networked'",
        ),
        "unsupported_behavior": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_unsupported.objc3",
            "O3S326",
            "unsupported property behavior 'Cached'",
        ),
        "nonobject_behavior": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_nonobject.objc3",
            "O3S327",
            "requires an Objective-C object property",
        ),
        "protocol_observed": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_protocol_observed.objc3",
            "O3S328",
            "requires a concrete interface or implementation property",
        ),
        "projected_writable": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_projected_writable.objc3",
            "O3S330",
            "requires a readonly getter-only property",
        ),
    }
    negative_summary: dict[str, Any] = {}
    for negative_key, (fixture_path, expected_code, expected_message) in negative_fixtures.items():
        compile_dir = case_dir / negative_key / "compile"
        compile_dir.mkdir(parents=True, exist_ok=True)
        compile_result = run(
            [
                PWSH,
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(COMPILE_PS1),
                str(fixture_path),
                "--out-dir",
                str(compile_dir),
                "--emit-prefix",
                "module",
            ]
        )
        expect(
            compile_result.returncode != 0,
            f"expected negative metaprogramming fixture {negative_key} to fail compilation",
        )
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        expect(diagnostics_path.is_file(), f"expected diagnostics for negative fixture {negative_key}")
        diagnostics_text = diagnostics_path.read_text(encoding="utf-8")
        expect(
            expected_code in diagnostics_text and expected_message in diagnostics_text,
            f"expected negative metaprogramming fixture {negative_key} to preserve {expected_code}",
        )
        negative_summary[negative_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "diagnostics": str(diagnostics_path.relative_to(ROOT)).replace("\\", "/"),
            "expected_code": expected_code,
        }

    return CaseResult(
        case_id="metaprogramming-derive-property-behavior-semantics",
        probe="compile-manifest-derive-property-behavior-semantics",
        fixture="tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "derive_positive": {
                "fixture": str(derive_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(derive_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "generated_method_entry_count": derive_surface.get(
                    "generated_method_entry_count"
                ),
                "expansion_inventory_rows_lexicographic": derive_surface.get(
                    "expansion_inventory_rows_lexicographic"
                ),
            },
            "property_positive": {
                "fixture": str(property_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(property_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "supported_behavior_sites": property_surface.get(
                    "supported_behavior_sites"
                ),
                "observed_behavior_sites": property_surface.get(
                    "observed_behavior_sites"
                ),
                "projected_behavior_sites": property_surface.get(
                    "projected_behavior_sites"
                ),
            },
            "negative_cases": negative_summary,
        },
    )


def check_unified_concurrency_runtime_architecture_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "unified-concurrency-runtime-architecture"
    fixtures: dict[str, tuple[Path, bool]] = {
        "async_source": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "async_await_executor_source_closure_positive.objc3",
            True,
        ),
        "actor_source": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "actor_member_isolation_surface_positive.objc3",
            True,
        ),
        "task_source": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "task_executor_cancellation_source_closure_positive.objc3",
            True,
        ),
    }
    expected_semantic_surfaces = {
        "async_source": (
            "objc_concurrency_async_source_closure",
            "objc3c.concurrency.async.source.closure.v1",
        ),
        "actor_source": (
            "objc_concurrency_actor_member_and_isolation_source_closure",
            "objc3c.concurrency.actor.member.isolation.source.closure.v1",
        ),
        "task_source": (
            "objc_concurrency_task_group_and_cancellation_source_closure",
            "objc3c.concurrency.task.group.cancellation.source.closure.v1",
        ),
    }
    surface_summaries: dict[str, Any] = {}

    for fixture_key, (fixture_path, requires_real_compile_output) in fixtures.items():
        compile_dir = case_dir / fixture_key / "compile"
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        if requires_real_compile_output:
            _, _, manifest_path = compile_fixture_outputs(fixture_path, compile_dir)
        else:
            manifest_path, compile_result = compile_fixture_manifest_only(
                fixture_path, compile_dir
            )
            expect(
                compile_result.returncode != 0,
                "expected task source closure fixture to remain source-surface-only until later lowering work lands",
            )
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface_name, expected_contract_id = expected_semantic_surfaces[fixture_key]
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(semantic_surface_name, {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish {semantic_surface_name}",
        )
        expect(
            semantic_surface.get("contract_id") == expected_contract_id,
            f"expected {fixture_key} fixture to preserve {expected_contract_id}",
        )
        expect(
            semantic_surface.get("deterministic_handoff") is True,
            f"expected {fixture_key} fixture to preserve deterministic_handoff",
        )
        expect(
            semantic_surface.get("ready_for_semantic_expansion") is True,
            f"expected {fixture_key} fixture to preserve ready_for_semantic_expansion",
        )
        surface_summaries[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "surface": semantic_surface_name,
            "contract_id": semantic_surface.get("contract_id"),
        }
        if not requires_real_compile_output and diagnostics_path.is_file():
            surface_summaries[fixture_key]["diagnostics"] = str(
                diagnostics_path.relative_to(ROOT)
            ).replace("\\", "/")
        if fixture_key == "async_source":
            top_level_surface = manifest.get("runtime_unified_concurrency_source_surface", {})
            expect(
                isinstance(top_level_surface, dict),
                "expected async concurrency fixture to publish runtime_unified_concurrency_source_surface",
            )
            expect(
                top_level_surface.get("contract_id")
                == RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
                "expected concurrency runtime architecture fixture to preserve the unified source surface contract",
            )
            expect(
                top_level_surface.get("source_surface_model")
                == "unified-concurrency-source-surface-freezes-live-async-actor-task-source-and-sema-boundaries-before-lowering-runtime-and-public-abi-expansion",
                "expected concurrency runtime architecture fixture to preserve the unified source surface model",
            )
            expect(
                top_level_surface.get("requires_coupled_registration_manifest") is True
                and top_level_surface.get("requires_real_compile_output") is True
                and top_level_surface.get("requires_linked_runtime_probe") is True,
                "expected unified concurrency source surface to remain compile-coupled and probe-backed",
            )
            surface_summaries["runtime_surface"] = {
                "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": top_level_surface.get("contract_id"),
                "source_contract_ids": top_level_surface.get("source_contract_ids"),
            }

    return CaseResult(
        case_id="unified-concurrency-runtime-architecture",
        probe="compile-manifest-runtime-source-surface",
        fixture="tests/tooling/fixtures/native/async_await_executor_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=surface_summaries,
    )


def check_async_task_actor_normalization_completion_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "async-task-actor-normalization-completion"
    fixtures: dict[str, tuple[Path, bool, str, str]] = {
        "async_normalization": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "async_lowering_positive.objc3",
            True,
            "objc_concurrency_async_effect_and_suspension_semantic_model",
            "objc_concurrency_continuation_abi_and_async_lowering_contract",
        ),
        "actor_normalization": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "actor_isolation_sendable_semantic_model_positive.objc3",
            True,
            "objc_concurrency_actor_isolation_and_sendable_semantic_model",
            "objc_concurrency_actor_lowering_and_metadata_contract",
        ),
        "task_normalization": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "task_executor_cancellation_semantic_model_positive.objc3",
            True,
            "objc_concurrency_task_executor_and_cancellation_semantic_model",
            "objc_concurrency_task_runtime_lowering_contract",
        ),
    }
    expected_contract_ids = {
        "async_normalization": (
            "objc3c.concurrency.async.effect.suspension.semantic.model.v1",
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
        ),
        "actor_normalization": (
            "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ),
        "task_normalization": (
            "objc3c.concurrency.task.executor.cancellation.semantic.model.v1",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        requires_real_compile_output,
        semantic_surface_name,
        lowering_surface_name,
    ) in fixtures.items():
        compile_dir = case_dir / fixture_key / "compile"
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        if requires_real_compile_output:
            _, _, manifest_path = compile_fixture_outputs(fixture_path, compile_dir)
        else:
            manifest_path, compile_result = compile_fixture_manifest_only(
                fixture_path, compile_dir
            )
            expect(
                compile_result.returncode != 0,
                "expected task normalization fixture to remain manifest-backed until task lowering determinism lands",
            )
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(semantic_surface_name, {})
        )
        lowering_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(lowering_surface_name, {})
        )
        expected_semantic_contract_id, expected_lowering_contract_id = (
            expected_contract_ids[fixture_key]
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish {semantic_surface_name}",
        )
        expect(
            isinstance(lowering_surface, dict),
            f"expected {fixture_key} fixture to publish {lowering_surface_name}",
        )
        expect(
            semantic_surface.get("contract_id") == expected_semantic_contract_id,
            f"expected {fixture_key} semantic model to preserve {expected_semantic_contract_id}",
        )
        expect(
            lowering_surface.get("contract_id") == expected_lowering_contract_id,
            f"expected {fixture_key} lowering surface to preserve {expected_lowering_contract_id}",
        )
        expect(
            lowering_surface.get("deterministic_handoff") is True
            and lowering_surface.get("ready_for_ir_emission") is True,
            f"expected {fixture_key} lowering surface to preserve deterministic IR handoff",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "semantic_surface": semantic_surface_name,
            "semantic_contract_id": semantic_surface.get("contract_id"),
            "lowering_surface": lowering_surface_name,
            "lowering_contract_id": lowering_surface.get("contract_id"),
        }
        if not requires_real_compile_output and diagnostics_path.is_file():
            summary[fixture_key]["diagnostics"] = str(
                diagnostics_path.relative_to(ROOT)
            ).replace("\\", "/")
        if fixture_key == "async_normalization":
            top_level_surface = manifest.get(
                "runtime_async_task_actor_normalization_completion_surface", {}
            )
            expect(
                isinstance(top_level_surface, dict),
                "expected async lowering fixture to publish runtime_async_task_actor_normalization_completion_surface",
            )
            expect(
                top_level_surface.get("contract_id")
                == RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID,
                "expected async/task/actor normalization fixture to preserve the normalization completion surface contract",
            )
            expect(
                top_level_surface.get("normalization_completion_model")
                == "normalized-async-task-actor-sema-and-lowering-packets-freeze-the-live-boundary-before-runtime-abi-and-runnable-execution-closure",
                "expected async/task/actor normalization fixture to preserve the normalization completion model",
            )
            summary["runtime_surface"] = {
                "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": top_level_surface.get("contract_id"),
                "normalized_semantic_contract_ids": top_level_surface.get(
                    "normalized_semantic_contract_ids"
                ),
                "lowering_contract_ids": top_level_surface.get(
                    "lowering_contract_ids"
                ),
            }

    return CaseResult(
        case_id="async-task-actor-normalization-completion",
        probe="compile-manifest-normalization-surface",
        fixture="tests/tooling/fixtures/native/async_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )


def check_unified_concurrency_lowering_metadata_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "unified-concurrency-lowering-metadata-surface"
    fixtures: dict[
        str,
        tuple[
            Path,
            str,
            str,
            str,
            str,
            str,
            str,
        ],
    ] = {
        "async_lowering": (
            ROOT / "tests" / "tooling" / "fixtures" / "native" / "async_lowering_positive.objc3",
            "objc_concurrency_continuation_abi_and_async_lowering_contract",
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
            "objc_concurrency_async_function_await_and_continuation_lowering",
            "objc3c.concurrency.async.direct.call.lowering.v1",
            "deterministic_handoff",
            "ready_for_ir_emission",
        ),
        "task_lowering": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "task_runtime_async_entry_lowering_positive.objc3",
            "objc_concurrency_task_runtime_lowering_contract",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
            "objc_concurrency_task_group_and_runtime_abi_completion",
            "objc3c.concurrency.task.runtime.abi.completion.v1",
            "deterministic_handoff",
            "ready_for_ir_emission",
        ),
        "actor_lowering": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "actor_lowering_metadata_positive.objc3",
            "objc_concurrency_actor_lowering_and_metadata_contract",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
            "objc_concurrency_actor_isolation_and_sendability_enforcement",
            "objc3c.concurrency.actor.isolation.sendability.enforcement.v1",
            "deterministic_handoff",
            "ready_for_ir_emission",
        ),
    }
    summary: dict[str, Any] = {}

    for (
        fixture_key,
        (
            fixture_path,
            lowering_surface_name,
            expected_lowering_contract_id,
            detail_surface_name,
            expected_detail_contract_id,
            lowering_determinism_field,
            lowering_readiness_field,
        ),
    ) in fixtures.items():
        compile_dir = case_dir / fixture_key / "compile"
        _, _, manifest_path = compile_fixture_outputs(fixture_path, compile_dir)
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get(
            "semantic_surface", {}
        )
        lowering_surface = semantic_surface.get(lowering_surface_name, {})
        detail_surface = semantic_surface.get(detail_surface_name, {})
        expect(
            isinstance(lowering_surface, dict),
            f"expected {fixture_key} fixture to publish {lowering_surface_name}",
        )
        expect(
            isinstance(detail_surface, dict),
            f"expected {fixture_key} fixture to publish {detail_surface_name}",
        )
        expect(
            lowering_surface.get("contract_id") == expected_lowering_contract_id,
            f"expected {fixture_key} lowering fixture to preserve {expected_lowering_contract_id}",
        )
        expect(
            detail_surface.get("contract_id") == expected_detail_contract_id,
            f"expected {fixture_key} lowering fixture to preserve {expected_detail_contract_id}",
        )
        expect(
            lowering_surface.get(lowering_determinism_field) is True,
            f"expected {fixture_key} lowering surface to preserve {lowering_determinism_field}",
        )
        expect(
            lowering_surface.get(lowering_readiness_field) is True,
            f"expected {fixture_key} lowering surface to preserve {lowering_readiness_field}",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "lowering_surface": lowering_surface_name,
            "lowering_contract_id": lowering_surface.get("contract_id"),
            "detail_surface": detail_surface_name,
            "detail_contract_id": detail_surface.get("contract_id"),
        }
        if fixture_key == "async_lowering":
            top_level_surface = manifest.get(
                "runtime_unified_concurrency_lowering_metadata_surface", {}
            )
            expect(
                isinstance(top_level_surface, dict),
                "expected async lowering fixture to publish runtime_unified_concurrency_lowering_metadata_surface",
            )
            expect(
                top_level_surface.get("contract_id")
                == RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID,
                "expected unified concurrency lowering fixture to preserve the lowering metadata surface contract",
            )
            expect(
                top_level_surface.get("lowering_metadata_surface_model")
                == "unified-concurrency-lowering-and-metadata-surface-freezes-live-async-task-actor-lowering-packets-and-emitted-metadata-boundaries-before-runtime-abi-and-runnable-execution-closure",
                "expected unified concurrency lowering fixture to preserve the lowering metadata model",
            )
            summary["runtime_surface"] = {
                "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": top_level_surface.get("contract_id"),
                "lowering_contract_ids": top_level_surface.get("lowering_contract_ids"),
                "lowering_detail_contract_ids": top_level_surface.get(
                    "lowering_detail_contract_ids"
                ),
            }

    return CaseResult(
        case_id="unified-concurrency-lowering-metadata-surface",
        probe="compile-manifest-lowering-metadata-surface",
        fixture="tests/tooling/fixtures/native/async_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )


def check_unified_concurrency_runtime_abi_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "unified-concurrency-runtime-abi"

    continuation_probe = ROOT / Path(CONTINUATION_RUNTIME_ABI_PROBE)
    continuation_exe = case_dir / "continuation_runtime_abi_probe.exe"
    compile_probe(clangxx, continuation_probe, continuation_exe, [])
    continuation_payload = parse_key_value_output(
        run_probe(continuation_exe), "unified concurrency continuation runtime ABI probe"
    )
    for field_name, expected_value in {
        "handle": 1,
        "handed_off": 1,
        "resumed": 77,
        "copy_status": 0,
        "allocation_call_count": 1,
        "handoff_call_count": 1,
        "resume_call_count": 1,
        "live_continuation_handle_count": 0,
        "last_allocated_continuation_handle": 1,
        "last_allocated_resume_entry_tag": 41,
        "last_allocated_executor_tag": 9,
        "last_handoff_continuation_handle": 1,
        "last_handoff_executor_tag": 17,
        "last_resume_continuation_handle": 1,
        "last_resume_result_value": 77,
        "last_resume_return_value": 77,
    }.items():
        expect(
            continuation_payload.get(field_name) == expected_value,
            f"expected unified concurrency continuation runtime ABI probe to preserve {field_name}",
        )

    task_probe = ROOT / Path(TASK_RUNTIME_ABI_PROBE)
    task_exe = case_dir / "task_runtime_abi_probe.exe"
    compile_probe(clangxx, task_probe, task_exe, [])
    task_payload = parse_key_value_output(
        run_probe(task_exe), "unified concurrency task runtime ABI probe"
    )
    for field_name, expected_value in {
        "spawn_group": 111,
        "scope": 1,
        "add_task": 1,
        "cancelled": 0,
        "wait_next": 23,
        "hop": 23,
        "cancel_all": 31,
        "on_cancel": 41,
        "spawn_detached": 121,
        "copy_status": 0,
        "spawn_call_count": 2,
        "scope_call_count": 1,
        "add_task_call_count": 1,
        "wait_next_call_count": 1,
        "cancel_all_call_count": 1,
        "cancellation_poll_call_count": 1,
        "on_cancel_call_count": 1,
        "executor_hop_call_count": 1,
        "last_spawn_kind": 2,
        "last_spawn_executor_tag": 3,
        "last_wait_next_result": 23,
        "last_executor_hop_executor_tag": 2,
        "last_executor_hop_value": 23,
    }.items():
        expect(
            task_payload.get(field_name) == expected_value,
            f"expected unified concurrency task runtime ABI probe to preserve {field_name}",
        )

    actor_probe = ROOT / Path(ACTOR_RUNTIME_ABI_PROBE)
    actor_exe = case_dir / "actor_runtime_abi_probe.exe"
    compile_probe(clangxx, actor_probe, actor_exe, [])
    actor_payload = parse_key_value_output(
        run_probe(actor_exe), "unified concurrency actor runtime ABI probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "replay": 1,
        "guard": 1,
        "isolation": 1,
        "nonisolated": 5,
        "hopped": 17,
        "replay_proof_call_count": 1,
        "race_guard_call_count": 1,
        "isolation_thunk_call_count": 1,
        "nonisolated_entry_call_count": 1,
        "hop_to_executor_call_count": 1,
        "last_replay_proof_executor_tag": 1,
        "last_race_guard_executor_tag": 1,
        "last_isolation_executor_tag": 1,
        "last_nonisolated_value": 5,
        "last_nonisolated_executor_tag": 0,
        "last_hop_value": 17,
        "last_hop_executor_tag": 1,
        "last_hop_result": 17,
    }.items():
        expect(
            actor_payload.get(field_name) == expected_value,
            f"expected unified concurrency actor runtime ABI probe to preserve {field_name}",
        )

    return CaseResult(
        case_id="unified-concurrency-runtime-abi",
        probe=";".join(
            [
                CONTINUATION_RUNTIME_ABI_PROBE,
                TASK_RUNTIME_ABI_PROBE,
                ACTOR_RUNTIME_ABI_PROBE,
            ]
        ),
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "continuation_probe": CONTINUATION_RUNTIME_ABI_PROBE,
            "task_probe": TASK_RUNTIME_ABI_PROBE,
            "actor_probe": ACTOR_RUNTIME_ABI_PROBE,
            "async_continuation_state_snapshot_symbol": (
                "objc3_runtime_copy_async_continuation_state_for_testing"
            ),
            "task_runtime_state_snapshot_symbol": (
                "objc3_runtime_copy_task_runtime_state_for_testing"
            ),
            "actor_runtime_state_snapshot_symbol": (
                "objc3_runtime_copy_actor_runtime_state_for_testing"
            ),
        },
    )


def check_live_unified_concurrency_runtime_implementation_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "live-unified-concurrency-runtime-implementation"

    continuation_obj, _, continuation_manifest_path = compile_fixture_outputs(
        ROOT / Path(LIVE_CONTINUATION_RUNTIME_FIXTURE), case_dir / "continuation" / "compile"
    )
    continuation_manifest = json.loads(
        continuation_manifest_path.read_text(encoding="utf-8")
    )
    expect(
        continuation_manifest.get("runtime_unified_concurrency_runtime_abi_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "expected live continuation fixture to publish the unified concurrency runtime ABI surface",
    )
    continuation_exe = case_dir / "continuation" / "live_continuation_runtime_probe.exe"
    compile_probe(
        clangxx,
        ROOT / Path(LIVE_CONTINUATION_RUNTIME_PROBE),
        continuation_exe,
        [continuation_obj],
    )
    continuation_payload = parse_key_value_output(
        run_probe(continuation_exe), "live unified concurrency continuation runtime probe"
    )
    for field_name, expected_value in {
        "runTask": 7,
        "loadValue": 7,
        "copy_status": 0,
        "allocation_call_count": 2,
        "handoff_call_count": 2,
        "resume_call_count": 2,
        "live_continuation_handle_count": 0,
        "last_handoff_executor_tag": 1,
        "last_resume_return_value": 7,
    }.items():
        expect(
            continuation_payload.get(field_name) == expected_value,
            f"expected live unified concurrency continuation runtime probe to preserve {field_name}",
        )

    task_obj, _, task_manifest_path = compile_fixture_outputs(
        ROOT / Path(LIVE_TASK_RUNTIME_FIXTURE), case_dir / "task" / "compile"
    )
    task_manifest = json.loads(task_manifest_path.read_text(encoding="utf-8"))
    expect(
        task_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_concurrency_task_runtime_lowering_contract", {})
        .get("contract_id")
        == "objc3c.concurrency.task.runtime.lowering.contract.v1",
        "expected live task fixture to preserve the task runtime lowering contract",
    )
    task_exe = case_dir / "task" / "live_task_runtime_probe.exe"
    compile_probe(
        clangxx,
        ROOT / Path(LIVE_TASK_RUNTIME_PROBE),
        task_exe,
        [task_obj],
    )
    task_payload = parse_key_value_output(
        run_probe(task_exe), "live unified concurrency task runtime probe"
    )
    for field_name, expected_value in {
        "spawn_group": 111,
        "scope": 1,
        "add_task": 1,
        "cancelled": 0,
        "wait_next": 23,
        "hop": 23,
        "cancel_all": 31,
        "on_cancel": 41,
        "spawn_detached": 121,
        "copy_status": 0,
        "spawn_call_count": 2,
        "scope_call_count": 1,
        "add_task_call_count": 1,
        "wait_next_call_count": 1,
        "cancel_all_call_count": 1,
        "cancellation_poll_call_count": 1,
        "on_cancel_call_count": 1,
        "executor_hop_call_count": 1,
        "last_spawn_kind": 2,
        "last_spawn_executor_tag": 3,
        "last_wait_next_result": 23,
        "last_executor_hop_executor_tag": 2,
        "last_executor_hop_value": 23,
    }.items():
        expect(
            task_payload.get(field_name) == expected_value,
            f"expected live unified concurrency task runtime probe to preserve {field_name}",
        )

    actor_obj, _, actor_manifest_path = compile_fixture_outputs(
        ROOT / Path(LIVE_ACTOR_RUNTIME_FIXTURE), case_dir / "actor" / "compile"
    )
    actor_manifest = json.loads(actor_manifest_path.read_text(encoding="utf-8"))
    expect(
        actor_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_concurrency_actor_lowering_and_metadata_contract", {})
        .get("contract_id")
        == "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "expected live actor fixture to preserve the actor lowering metadata contract",
    )
    actor_exe = case_dir / "actor" / "live_actor_runtime_probe.exe"
    compile_probe(
        clangxx,
        ROOT / Path(LIVE_ACTOR_RUNTIME_PROBE),
        actor_exe,
        [actor_obj],
    )
    actor_payload = parse_key_value_output(
        run_probe(actor_exe), "live unified concurrency actor runtime probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "replay": 1,
        "guard": 1,
        "isolation": 1,
        "bound": 1,
        "enqueued": 23,
        "drained": 23,
        "replay_proof_call_count": 1,
        "race_guard_call_count": 1,
        "isolation_thunk_call_count": 1,
        "bind_executor_call_count": 1,
        "mailbox_enqueue_call_count": 1,
        "mailbox_drain_call_count": 1,
        "last_replay_proof_executor_tag": 1,
        "last_race_guard_executor_tag": 1,
        "last_isolation_executor_tag": 1,
        "last_bound_actor_handle": 41,
        "last_bound_executor_tag": 1,
        "last_mailbox_actor_handle": 41,
        "last_mailbox_enqueued_value": 23,
        "last_mailbox_executor_tag": 1,
        "last_mailbox_depth": 0,
        "last_mailbox_drained_value": 23,
    }.items():
        expect(
            actor_payload.get(field_name) == expected_value,
            f"expected live unified concurrency actor runtime probe to preserve {field_name}",
        )

    return CaseResult(
        case_id="live-unified-concurrency-runtime-implementation",
        probe=";".join(
            [
                LIVE_CONTINUATION_RUNTIME_PROBE,
                LIVE_TASK_RUNTIME_PROBE,
                LIVE_ACTOR_RUNTIME_PROBE,
            ]
        ),
        fixture=LIVE_CONTINUATION_RUNTIME_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "continuation_fixture": LIVE_CONTINUATION_RUNTIME_FIXTURE,
            "task_fixture": LIVE_TASK_RUNTIME_FIXTURE,
            "actor_fixture": LIVE_ACTOR_RUNTIME_FIXTURE,
            "continuation_probe": LIVE_CONTINUATION_RUNTIME_PROBE,
            "task_probe": LIVE_TASK_RUNTIME_PROBE,
            "actor_probe": LIVE_ACTOR_RUNTIME_PROBE,
        },
    )


def check_error_execution_cleanup_source_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-execution-cleanup-source"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_source_closure_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_source_closure", {})
    )
    expect(
        isinstance(surface, dict),
        "expected error source closure fixture to publish objc_error_handling_error_source_closure",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.source.closure.v1",
        "frontend_surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_source_closure",
        "throws_declaration_source_supported": True,
        "result_carrier_source_supported": True,
        "ns_error_bridging_source_supported": True,
        "error_bridge_marker_source_supported": True,
        "try_keyword_reserved": True,
        "throw_keyword_reserved": True,
        "catch_keyword_reserved": True,
        "try_fail_closed": True,
        "throw_fail_closed": True,
        "do_catch_fail_closed": True,
        "deterministic_handoff": True,
        "ready_for_semantic_expansion": True,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected error source closure surface to preserve {field_name}",
        )
    expect(
        surface.get("function_throws_declaration_sites") == 1,
        "expected error source closure surface to publish one function throws declaration site",
    )
    expect(
        surface.get("result_like_sites") == 7
        and surface.get("result_success_sites") == 1
        and surface.get("result_failure_sites") == 2
        and surface.get("result_branch_sites") == 4
        and surface.get("result_payload_sites") == 3,
        "expected error source closure surface to preserve the result carrier source counts",
    )
    expect(
        surface.get("ns_error_bridging_sites") == 3
        and surface.get("ns_error_out_parameter_sites") == 1
        and surface.get("ns_error_bridge_path_sites") == 1,
        "expected error source closure surface to preserve the NSError bridge source counts",
    )
    return CaseResult(
        case_id="error-execution-cleanup-source",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/error_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_declaration_sites": surface.get("function_throws_declaration_sites"),
            "result_like_sites": surface.get("result_like_sites"),
            "ns_error_bridging_sites": surface.get("ns_error_bridging_sites"),
            "ready_for_semantic_expansion": surface.get("ready_for_semantic_expansion"),
        },
    )


def check_catch_filter_finalization_source_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "catch-filter-finalization-source"
    try_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_semantics_positive.objc3"
    )
    _, _, try_manifest_path = compile_fixture_outputs(try_fixture, case_dir / "try")
    try_manifest = json.loads(try_manifest_path.read_text(encoding="utf-8"))
    try_surface = (
        try_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(try_surface, dict),
        "expected try/do/catch fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expected_try_fields = {
        "contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "dependency_contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_try_do_catch_semantics",
        "try_surface_landed": True,
        "throw_surface_landed": True,
        "do_catch_surface_landed": True,
        "throwing_context_legality_enforced": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_try_fields.items():
        expect(
            try_surface.get(field_name) == expected_value,
            f"expected try/do/catch source surface to preserve {field_name}",
        )
    expect(
        try_surface.get("try_expression_sites") == 3
        and try_surface.get("throw_statement_sites") == 1
        and try_surface.get("do_catch_sites") == 1
        and try_surface.get("catch_clause_sites") == 2
        and try_surface.get("catch_all_sites") == 1,
        "expected try/do/catch source surface to preserve the catch/finalization source counts",
    )

    bridge_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_positive.objc3"
    )
    _, _, bridge_manifest_path = compile_fixture_outputs(bridge_fixture, case_dir / "bridge")
    bridge_manifest = json.loads(bridge_manifest_path.read_text(encoding="utf-8"))
    bridge_surface = (
        bridge_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(bridge_surface, dict),
        "expected bridge legality fixture to publish objc_error_handling_error_bridge_legality",
    )
    expected_bridge_fields = {
        "contract_id": "objc3c.error_handling.error.bridge.legality.v1",
        "dependency_contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_bridge_legality",
        "bridge_legality_landed": True,
        "try_bridge_filter_landed": True,
        "unsupported_combinations_fail_closed": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_bridge_fields.items():
        expect(
            bridge_surface.get(field_name) == expected_value,
            f"expected bridge legality source surface to preserve {field_name}",
        )
    expect(
        bridge_surface.get("bridge_callable_sites") == 2
        and bridge_surface.get("semantically_valid_bridge_callable_sites") == 2
        and bridge_surface.get("try_eligible_bridge_callable_sites") == 2
        and bridge_surface.get("unsupported_combination_sites") == 0
        and bridge_surface.get("throws_bridge_conflict_sites") == 0,
        "expected bridge legality source surface to preserve the catch-filter eligibility counts",
    )

    return CaseResult(
        case_id="catch-filter-finalization-source",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "try_expression_sites": try_surface.get("try_expression_sites"),
            "catch_clause_sites": try_surface.get("catch_clause_sites"),
            "bridge_callable_sites": bridge_surface.get("bridge_callable_sites"),
            "try_eligible_bridge_callable_sites": bridge_surface.get(
                "try_eligible_bridge_callable_sites"
            ),
        },
    )


def check_error_propagation_cleanup_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-propagation-cleanup-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_source_closure_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_semantic_model", {})
    )
    expect(
        isinstance(surface, dict),
        "expected error semantic model fixture to publish objc_error_handling_error_semantic_model",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "frontend_dependency_contract_id": "objc3c.error_handling.error.source.closure.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_semantic_model",
        "throws_declaration_semantics_landed": True,
        "result_carrier_profile_semantics_landed": True,
        "ns_error_bridging_profile_semantics_landed": True,
        "bridge_marker_semantics_landed": True,
        "parser_fail_closed_boundary_required": True,
        "parser_fail_closed_boundary_preserved": True,
        "propagation_runtime_deferred": True,
        "status_to_error_runtime_deferred": True,
        "native_error_abi_deferred": True,
        "placeholder_throws_summary_carried": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected error semantic model to preserve {field_name}",
        )
    expect(
        surface.get("throws_declaration_sites") == 1
        and surface.get("result_like_sites") == 7
        and surface.get("ns_error_bridging_sites") == 3
        and surface.get("placeholder_throws_propagation_sites") == 0
        and surface.get("placeholder_unwind_cleanup_sites") == 0,
        "expected error semantic model to preserve propagation and cleanup counts",
    )

    return CaseResult(
        case_id="error-propagation-cleanup-semantics",
        probe="compile-manifest-semantic-surface",
        fixture="tests/tooling/fixtures/native/error_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_declaration_sites": surface.get("throws_declaration_sites"),
            "result_like_sites": surface.get("result_like_sites"),
            "ns_error_bridging_sites": surface.get("ns_error_bridging_sites"),
            "propagation_runtime_deferred": surface.get("propagation_runtime_deferred"),
        },
    )


def check_executable_try_throw_do_catch_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "executable-try-throw-do-catch-semantics"
    positive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_semantics_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(positive_fixture, case_dir / "positive")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(surface, dict),
        "expected try/do/catch positive fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "dependency_contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_try_do_catch_semantics",
        "try_surface_landed": True,
        "throw_surface_landed": True,
        "do_catch_surface_landed": True,
        "throwing_context_legality_enforced": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected executable try/do/catch semantics to preserve {field_name}",
        )
    expect(
        surface.get("try_expression_sites") == 3
        and surface.get("try_propagating_sites") == 1
        and surface.get("try_optional_sites") == 1
        and surface.get("try_forced_sites") == 1
        and surface.get("throw_statement_sites") == 1
        and surface.get("do_catch_sites") == 1
        and surface.get("catch_clause_sites") == 2
        and surface.get("catch_binding_sites") == 1
        and surface.get("catch_all_sites") == 1
        and surface.get("throwing_callable_try_sites") == 2
        and surface.get("bridged_callable_try_sites") == 1
        and surface.get("caller_propagation_sites") == 1
        and surface.get("local_handler_sites") == 0
        and surface.get("rethrow_sites") == 0
        and surface.get("contract_violation_sites") == 0,
        "expected executable try/do/catch semantics to preserve the positive semantic counts",
    )

    negatives = [
        (
            "try_requires_throwing_context_negative.objc3",
            ["propagating try requires a throws function or an enclosing do/catch"],
            ["O3S272"],
        ),
        (
            "try_requires_throwing_or_bridged_operand_negative.objc3",
            ["try operand must be a throwing or NSError-bridged call surface"],
            ["O3S271"],
        ),
        (
            "throw_requires_throws_or_catch_negative.objc3",
            ["throw statements require a throws function or a catch body"],
            ["O3S274"],
        ),
        (
            "catch_after_catch_all_negative.objc3",
            ["catch clauses after a catch-all are unreachable"],
            ["O3S269"],
        ),
    ]
    negative_summaries: list[dict[str, Any]] = []
    for fixture_name, expected_snippets, expected_codes in negatives:
        negative_fixture = (
            ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name
        )
        negative_summary = compile_fixture_expect_failure(
            negative_fixture,
            case_dir / negative_fixture.stem,
            expected_snippets=expected_snippets,
            expected_codes=expected_codes,
        )
        negative_summaries.append(
            {
                "fixture": str(negative_fixture.relative_to(ROOT)).replace("\\", "/"),
                "diagnostic_codes": negative_summary["diagnostic_codes"],
            }
        )

    native_fail_closed_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_native_fail_closed.objc3"
    )
    _, _, native_manifest_path = compile_fixture_outputs(
        native_fail_closed_fixture, case_dir / "native-fail-closed"
    )
    native_manifest = json.loads(native_manifest_path.read_text(encoding="utf-8"))
    native_surface = (
        native_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(native_surface, dict),
        "expected native fail-closed fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expect(
        native_surface.get("native_emit_remains_fail_closed") is True
        and native_surface.get("try_expression_sites") == 1
        and native_surface.get("do_catch_sites") == 1
        and native_surface.get("bridged_callable_try_sites") == 1,
        "expected native fail-closed fixture to preserve the semantic fail-closed lowering boundary",
    )

    return CaseResult(
        case_id="executable-try-throw-do-catch-semantics",
        probe="compile-manifest-semantic-surface-plus-fail-closed-diagnostics",
        fixture="tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "try_expression_sites": surface.get("try_expression_sites"),
            "catch_clause_sites": surface.get("catch_clause_sites"),
            "bridged_callable_try_sites": surface.get("bridged_callable_try_sites"),
            "native_fail_closed_fixture": {
                "fixture": str(native_fail_closed_fixture.relative_to(ROOT)).replace("\\", "/"),
                "native_emit_remains_fail_closed": native_surface.get(
                    "native_emit_remains_fail_closed"
                ),
            },
            "negative_fixtures": negative_summaries,
        },
    )


def check_bridging_filter_unwind_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "bridging-filter-unwind-compatibility-diagnostics"
    positive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(positive_fixture, case_dir / "positive")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(surface, dict),
        "expected bridge legality positive fixture to publish objc_error_handling_error_bridge_legality",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.bridge.legality.v1",
        "dependency_contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_bridge_legality",
        "bridge_legality_landed": True,
        "try_bridge_filter_landed": True,
        "unsupported_combinations_fail_closed": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected bridge legality diagnostics to preserve {field_name}",
        )
    expect(
        surface.get("bridge_callable_sites") == 2
        and surface.get("objc_nserror_callable_sites") == 1
        and surface.get("objc_status_code_callable_sites") == 1
        and surface.get("semantically_valid_bridge_callable_sites") == 2
        and surface.get("try_eligible_bridge_callable_sites") == 2
        and surface.get("unsupported_combination_sites") == 0
        and surface.get("contract_violation_sites") == 0,
        "expected bridge legality diagnostics to preserve the positive bridge counts",
    )

    native_fail_closed_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_native_fail_closed.objc3"
    )
    _, _, native_manifest_path = compile_fixture_outputs(
        native_fail_closed_fixture, case_dir / "native-fail-closed"
    )
    native_manifest = json.loads(native_manifest_path.read_text(encoding="utf-8"))
    native_surface = (
        native_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(native_surface, dict),
        "expected bridge native fail-closed fixture to publish objc_error_handling_error_bridge_legality",
    )
    expect(
        native_surface.get("native_emit_remains_fail_closed") is True
        and native_surface.get("bridge_callable_sites") == 1
        and native_surface.get("objc_status_code_callable_sites") == 1
        and native_surface.get("try_eligible_bridge_callable_sites") == 1,
        "expected bridge native fail-closed fixture to preserve the lowered bridge boundary",
    )

    negatives = [
        (
            "bridge_legality_nserror_missing_out_negative.objc3",
            ["objc_nserror requires an NSError out parameter"],
            ["O3S275"],
        ),
        (
            "bridge_legality_nserror_bad_return_negative.objc3",
            ["objc_nserror currently requires a BOOL-like success return"],
            ["O3S276"],
        ),
        (
            "bridge_legality_throws_conflict_negative.objc3",
            ["NSError/status bridge markers cannot currently be combined with throws"],
            ["O3S277"],
        ),
        (
            "bridge_legality_marker_conflict_negative.objc3",
            ["objc_nserror and objc_status_code cannot appear on the same callable"],
            ["O3S278"],
        ),
        (
            "bridge_legality_bad_error_type_negative.objc3",
            ["objc_status_code currently requires error_type: NSError"],
            ["O3S280"],
        ),
        (
            "bridge_legality_missing_mapping_negative.objc3",
            ["objc_status_code mapping symbol must resolve to a declared function"],
            ["O3S282"],
        ),
        (
            "bridge_legality_bad_mapping_signature_negative.objc3",
            ["objc_status_code mapping function must accept one matching status parameter and return NSError"],
            ["O3S283"],
        ),
        (
            "bridge_legality_bad_status_return_negative.objc3",
            ["objc_status_code requires a BOOL-like or integer status return"],
            ["O3S281"],
        ),
    ]
    negative_summaries: list[dict[str, Any]] = []
    for fixture_name, expected_snippets, expected_codes in negatives:
        negative_fixture = (
            ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name
        )
        negative_summary = compile_fixture_expect_failure(
            negative_fixture,
            case_dir / negative_fixture.stem,
            expected_snippets=expected_snippets,
            expected_codes=expected_codes,
        )
        negative_summaries.append(
            {
                "fixture": str(negative_fixture.relative_to(ROOT)).replace("\\", "/"),
                "diagnostic_codes": negative_summary["diagnostic_codes"],
            }
        )

    return CaseResult(
        case_id="bridging-filter-unwind-compatibility-diagnostics",
        probe="compile-manifest-semantic-surface-plus-compatibility-diagnostics",
        fixture="tests/tooling/fixtures/native/bridge_legality_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "bridge_callable_sites": surface.get("bridge_callable_sites"),
            "try_eligible_bridge_callable_sites": surface.get(
                "try_eligible_bridge_callable_sites"
            ),
            "native_fail_closed_fixture": {
                "fixture": str(native_fail_closed_fixture.relative_to(ROOT)).replace("\\", "/"),
                "native_emit_remains_fail_closed": native_surface.get(
                    "native_emit_remains_fail_closed"
                ),
            },
            "negative_fixtures": negative_summaries,
        },
    )


def check_error_lowering_unwind_bridge_helper_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-lowering-unwind-bridge-helper-surface"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_out_abi_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ns_error_bridging = manifest.get("lowering_ns_error_bridging", {})
    unwind_cleanup = manifest.get("lowering_unwind_cleanup", {})
    throws_abi = manifest.get("lowering_error_handling_throws_abi_propagation", {})
    result_replay = manifest.get("lowering_error_handling_result_and_bridging_artifact_replay", {})
    expect(
        isinstance(ns_error_bridging, dict)
        and isinstance(unwind_cleanup, dict)
        and isinstance(throws_abi, dict)
        and isinstance(result_replay, dict),
        "expected error lowering fixture to publish the error_handling lowering and replay surfaces",
    )
    expect(
        ns_error_bridging.get("lane_contract") == "objc3c.ns.error.bridging.lowering.v1"
        and ns_error_bridging.get("deterministic_handoff") is True,
        "expected error lowering fixture to preserve the NSError bridging lowering contract",
    )
    expect(
        unwind_cleanup.get("lane_contract") == "objc3c.unwind.cleanup.lowering.v1"
        and unwind_cleanup.get("deterministic_handoff") is True,
        "expected error lowering fixture to preserve the unwind cleanup lowering contract",
    )
    expect(
        throws_abi.get("contract_id") == "objc3c.error_handling.throws.abi.propagation.lowering.v1"
        and throws_abi.get("deterministic_handoff") is False,
        "expected error lowering fixture to preserve the throws ABI propagation lowering contract",
    )
    expect(
        result_replay.get("contract_id") == "objc3c.error_handling.result.and.bridging.artifact.replay.v1"
        and result_replay.get("deterministic_handoff") is False,
        "expected error lowering fixture to preserve the result and bridge replay lowering contract",
    )
    return CaseResult(
        case_id="error-lowering-unwind-bridge-helper-surface",
        probe="compile-manifest-lowering-surface",
        fixture="tests/tooling/fixtures/native/error_out_abi_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "ns_error_bridging_contract": ns_error_bridging.get("lane_contract"),
            "unwind_cleanup_contract": unwind_cleanup.get("lane_contract"),
            "throws_abi_contract": throws_abi.get("contract_id"),
            "result_replay_contract": result_replay.get("contract_id"),
        },
    )


def check_executable_throw_catch_cleanup_lowering_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "executable-throw-catch-cleanup-lowering"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_out_abi_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    throws_abi = manifest.get("lowering_error_handling_throws_abi_propagation", {})
    result_replay = manifest.get("lowering_error_handling_result_and_bridging_artifact_replay", {})
    ns_error_bridging = manifest.get("lowering_ns_error_bridging", {})
    unwind_cleanup = manifest.get("lowering_unwind_cleanup", {})
    expect(obj_path.is_file(), "expected executable error lowering fixture to emit an object artifact")
    expect(
        "objc3_runtime_store_thrown_error_i32" in ll_text,
        "expected executable error lowering to emit thrown-error store helper calls",
    )
    expect(
        "objc3_runtime_load_thrown_error_i32" in ll_text,
        "expected executable error lowering to emit thrown-error load helper calls",
    )
    expect(
        "objc3_runtime_bridge_status_error_i32" in ll_text,
        "expected executable error lowering to emit status-bridge helper calls",
    )
    expect(
        "objc3_runtime_catch_matches_error_i32" in ll_text,
        "expected executable error lowering to emit catch-match helper calls",
    )
    expect(
        "objc_error_handling_throws_abi_propagation" in ll_text
        and "objc_error_handling_result_and_bridging_artifact_replay" in ll_text,
        "expected executable error lowering to preserve the error_handling lowering metadata packets in emitted LLVM",
    )
    expect(
        isinstance(throws_abi, dict)
        and throws_abi.get("contract_id")
        == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected executable error lowering fixture to preserve the throws ABI propagation contract",
    )
    expect(
        "ready_for_runtime_execution=true"
        in str(throws_abi.get("replay_key", "")),
        "expected executable error lowering fixture to mark the throws ABI replay packet ready for runtime execution",
    )
    expect(
        isinstance(result_replay, dict)
        and result_replay.get("contract_id")
        == "objc3c.error_handling.result.and.bridging.artifact.replay.v1",
        "expected executable error lowering fixture to preserve the error_handling replay contract",
    )
    expect(
        isinstance(ns_error_bridging, dict)
        and ns_error_bridging.get("lane_contract")
        == "objc3c.ns.error.bridging.lowering.v1",
        "expected executable error lowering fixture to preserve the NSError bridging lowering contract",
    )
    expect(
        isinstance(unwind_cleanup, dict)
        and unwind_cleanup.get("lane_contract")
        == "objc3c.unwind.cleanup.lowering.v1",
        "expected executable error lowering fixture to preserve the unwind cleanup lowering contract",
    )
    return CaseResult(
        case_id="executable-throw-catch-cleanup-lowering",
        probe="compile-artifact-llvm-helper-lowering",
        fixture="tests/tooling/fixtures/native/error_out_abi_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_abi_contract": throws_abi.get("contract_id"),
            "result_replay_contract": result_replay.get("contract_id"),
            "ns_error_bridging_contract": ns_error_bridging.get("lane_contract"),
            "unwind_cleanup_contract": unwind_cleanup.get("lane_contract"),
            "helper_calls": {
                "store": "objc3_runtime_store_thrown_error_i32" in ll_text,
                "load": "objc3_runtime_load_thrown_error_i32" in ll_text,
                "status_bridge": "objc3_runtime_bridge_status_error_i32" in ll_text,
                "catch_match": "objc3_runtime_catch_matches_error_i32" in ll_text,
            },
        },
    )


def check_cross_module_error_metadata_replay_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "cross-module-error-metadata-replay-preservation"
    provider_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "artifact_replay_producer.objc3"
    )
    consumer_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "result_bridge_consumer.objc3"
    )
    provider_dir = case_dir / "provider"
    consumer_dir = case_dir / "consumer"
    compile_fixture_outputs_with_args(
        provider_fixture,
        provider_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = provider_dir / "module.runtime-import-surface.json"
    expect(
        provider_import_surface.is_file(),
        "expected cross-module error provider to emit a runtime import surface",
    )
    compile_fixture_outputs_with_args(
        consumer_fixture,
        consumer_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    link_plan_path = consumer_dir / "module.cross-module-runtime-link-plan.json"
    expect(
        link_plan_path.is_file(),
        "expected cross-module error consumer to emit a cross-module runtime link plan",
    )
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))
    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module error consumer to publish one imported module in the link plan",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name") == "m267_c003_error_handling_artifact_replay_producer",
        "expected cross-module error consumer to preserve the imported producer module name",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected cross-module error consumer to preserve the imported producer registration ordinal",
    )
    expect(
        imported_module.get("error_handling_result_and_bridging_artifact_replay_present")
        is True,
        "expected cross-module error consumer to preserve the error_handling replay packet presence flag",
    )
    expect(
        imported_module.get("error_handling_binary_artifact_replay_ready") is True
        and imported_module.get("error_handling_runtime_import_artifact_ready") is True
        and imported_module.get("error_handling_separate_compilation_replay_ready") is True,
        "expected cross-module error consumer to preserve the error_handling replay readiness flags",
    )
    expect(
        imported_module.get("error_handling_contract_id")
        == "objc3c.error_handling.result.and.bridging.artifact.replay.v1",
        "expected cross-module error consumer to preserve the error_handling replay contract id",
    )
    expect(
        imported_module.get("error_handling_source_contract_id")
        == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected cross-module error consumer to preserve the error_handling replay source contract id",
    )
    replay_key = imported_module.get("error_handling_result_and_bridging_artifact_replay_key", "")
    throws_replay_key = imported_module.get("error_handling_replay_key", "")
    expect(
        isinstance(replay_key, str)
        and "runtime_import_artifact_ready=true" in replay_key
        and "separate_compilation_replay_ready=true" in replay_key,
        "expected cross-module error consumer to preserve the error_handling replay packet readiness in the imported replay key",
    )
    expect(
        isinstance(throws_replay_key, str)
        and "ready_for_runtime_execution=true" in throws_replay_key,
        "expected cross-module error consumer to preserve runtime-executable throws replay in the imported error_handling lowering key",
    )
    local_module = link_plan.get("local_module", {})
    expect(
        local_module.get("module_name") == "m267_result_bridge_consumer"
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module error consumer to preserve the local module identity and registration ordinal",
    )
    return CaseResult(
        case_id="cross-module-error-metadata-replay-preservation",
        probe="cross-module-runtime-link-plan",
        fixture="tests/tooling/fixtures/native/result_bridge_consumer.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_import_surface": str(provider_import_surface.relative_to(ROOT)).replace("\\", "/"),
            "consumer_link_plan": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "imported_module_name": imported_module.get("module_name"),
            "imported_module_registration_ordinal": imported_module.get(
                "translation_unit_registration_order_ordinal"
            ),
            "local_module_name": local_module.get("module_name"),
            "local_module_registration_ordinal": local_module.get(
                "translation_unit_registration_order_ordinal"
            ),
        },
    )


def check_error_runtime_abi_cleanup_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-runtime-abi-cleanup"
    probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "error_runtime_bridge_helper_probe.cpp"
    )
    exe_path = case_dir / "error_runtime_bridge_helper_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_key_value_output(run_probe(exe_path), "error runtime ABI cleanup probe")
    expected_integer_fields = {
        "status": 0,
        "loaded": 34,
        "bridged_status": 45,
        "bridged_nserror": 77,
        "match_nserror": 1,
        "match_protocol": 1,
        "match_catch_all": 1,
        "store_call_count": 1,
        "load_call_count": 1,
        "status_bridge_call_count": 1,
        "nserror_bridge_call_count": 1,
        "catch_match_call_count": 3,
        "last_stored_error_value": 34,
        "last_loaded_error_value": 34,
        "last_status_bridge_status_value": 5,
        "last_status_bridge_error_value": 45,
        "last_nserror_bridge_error_value": 77,
        "last_catch_match_error_value": 77,
        "last_catch_match_kind": 0,
        "last_catch_match_is_catch_all": 1,
        "last_catch_match_result": 1,
    }
    for field, expected_value in expected_integer_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected error runtime ABI cleanup probe to preserve {field}",
        )
    expect(
        payload.get("last_catch_kind_name") == "unknown",
        "expected error runtime ABI cleanup probe to preserve the last catch-kind label",
    )
    return CaseResult(
        case_id="error-runtime-abi-cleanup",
        probe="tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
        fixture="tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "bridge_state_snapshot_symbol": "objc3_runtime_copy_error_bridge_state_for_testing",
            "store_symbol": "objc3_runtime_store_thrown_error_i32",
            "load_symbol": "objc3_runtime_load_thrown_error_i32",
            "status_bridge_symbol": "objc3_runtime_bridge_status_error_i32",
            "nserror_bridge_symbol": "objc3_runtime_bridge_nserror_error_i32",
            "catch_match_symbol": "objc3_runtime_catch_matches_error_i32",
        },
    )


def check_live_error_runtime_integration_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "live-error-runtime-integration"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "live_error_runtime_integration_positive.objc3"
    )
    obj_path, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "live_error_runtime_integration_probe.cpp"
    )
    exe_path = case_dir / "live_error_runtime_integration_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "live error runtime integration probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    throws_abi = manifest.get("lowering_error_handling_throws_abi_propagation", {})
    expect(
        payload.get("status") == 0,
        "expected live error runtime integration probe to copy the bridge-state snapshot successfully",
    )
    expected_integer_fields = {
        "rc": 54,
        "store_call_count": 1,
        "load_call_count": 1,
        "status_bridge_call_count": 1,
        "nserror_bridge_call_count": 0,
        "catch_match_call_count": 1,
        "last_stored_error_value": 45,
        "last_loaded_error_value": 45,
        "last_status_bridge_status_value": 5,
        "last_status_bridge_error_value": 45,
        "last_catch_match_kind": 1,
        "last_catch_match_is_catch_all": 0,
        "last_catch_match_result": 1,
    }
    for field, expected_value in expected_integer_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected live error runtime integration probe to preserve {field}",
        )
    expect(
        payload.get("last_catch_kind_name") == "nserror",
        "expected live error runtime integration probe to preserve the NSError catch-kind label",
    )
    expect(
        isinstance(throws_abi, dict)
        and throws_abi.get("contract_id")
        == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected live error runtime integration fixture to preserve the throws ABI propagation contract",
    )
    expect(
        "ready_for_runtime_execution=true"
        in str(throws_abi.get("replay_key", "")),
        "expected live error runtime integration fixture to preserve runtime execution readiness in the throws ABI replay packet",
    )
    return CaseResult(
        case_id="live-error-runtime-integration",
        probe="tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
        fixture="tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "rc": payload.get("rc"),
            "status": payload.get("status"),
            "last_catch_kind_name": payload.get("last_catch_kind_name"),
            "throws_abi_contract": throws_abi.get("contract_id"),
        },
    )


def check_cross_module_storage_reflection_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-storage-reflection-artifact-preservation"
    provider_fixture = ROOT / Path(STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    provider_storage_surface = provider_import_payload.get(
        "objc_runtime_storage_reflection_artifact_preservation", {}
    )
    expect(
        isinstance(provider_storage_surface, dict),
        "expected storage-reflection provider import surface to publish the preservation packet",
    )
    expected_provider_storage_fields = {
        "contract_id": RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        "source_contract_id": RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        "executable_property_accessor_layout_lowering_contract_id": "objc3c.executable.property.accessor.layout.lowering.v1",
        "executable_ivar_layout_emission_contract_id": "objc3c.executable.ivar.layout.emission.v1",
        "executable_synthesized_accessor_property_lowering_contract_id": "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_runtime_storage_reflection_artifact_preservation",
        "import_artifact_member_name": "objc_runtime_storage_reflection_artifact_preservation",
        "source_model": "runtime-metadata-source-records-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-for-separate-compilation",
        "preservation_model": "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission",
        "fail_closed_model": "missing-or-drifted-storage-reflection-preservation-packets-disable-cross-module-storage-reflection-claims",
    }
    for field_name, expected_value in expected_provider_storage_fields.items():
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )
    expect(
        provider_storage_surface.get("local_property_descriptor_count")
        == provider_registration_manifest.get("property_descriptor_count")
        == 6,
        "expected storage-reflection provider import surface to preserve six property descriptors",
    )
    expect(
        provider_storage_surface.get("local_ivar_descriptor_count")
        == provider_registration_manifest.get("ivar_descriptor_count")
        == 3,
        "expected storage-reflection provider import surface to preserve three ivar descriptors",
    )
    for field_name, expected_value in (
        ("implementation_owned_property_entries", 3),
        ("synthesized_accessor_owner_entries", 3),
        ("synthesized_getter_entries", 3),
        ("synthesized_setter_entries", 3),
        ("synthesized_accessor_entries", 6),
        ("current_property_read_entries", 3),
        ("current_property_write_entries", 2),
        ("current_property_exchange_entries", 1),
        ("weak_current_property_load_entries", 0),
        ("weak_current_property_store_entries", 0),
        ("ivar_layout_entries", 3),
        ("ivar_layout_owner_entries", 1),
    ):
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )
    expect(
        provider_storage_surface.get("runtime_import_artifact_ready") is True
        and provider_storage_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_storage_surface.get("deterministic") is True,
        "expected storage-reflection provider import surface to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        isinstance(provider_storage_surface.get("replay_key"), str)
        and provider_storage_surface.get("replay_key") != "",
        "expected storage-reflection provider import surface to publish a replay key",
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    for field_name, expected_value in (
        (
            "runtime_cross_module_storage_reflection_artifact_preservation_surface_contract_id",
            RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "runtime_property_ivar_storage_accessor_source_surface_contract_id",
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        ),
        (
            "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        ),
        (
            "executable_property_accessor_layout_lowering_contract_id",
            "objc3c.executable.property.accessor.layout.lowering.v1",
        ),
        (
            "executable_ivar_layout_emission_contract_id",
            "objc3c.executable.ivar.layout.emission.v1",
        ),
        (
            "executable_synthesized_accessor_property_lowering_contract_id",
            "objc3c.executable.synthesized.accessor.property.lowering.v1",
        ),
        (
            "storage_reflection_artifact_preservation_model",
            "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission",
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("storage_reflection_cross_module_preservation_ready") is True,
        "expected cross-module link plan to mark storage/reflection preservation ready",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected storage-reflection link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == provider_import_payload.get("module_name")
        == "synthesizedAccessorPropertyLowering",
        "expected storage-reflection link plan to preserve the provider module name",
    )
    for field_name, expected_value in (
        ("storage_reflection_artifact_preservation_present", True),
        ("storage_reflection_runtime_import_artifact_ready", True),
        ("storage_reflection_separate_compilation_preservation_ready", True),
        ("storage_reflection_deterministic", True),
        (
            "storage_reflection_contract_id",
            RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "storage_reflection_source_contract_id",
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        ),
        (
            "storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id",
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        ),
        (
            "storage_reflection_executable_property_accessor_layout_lowering_contract_id",
            "objc3c.executable.property.accessor.layout.lowering.v1",
        ),
        (
            "storage_reflection_executable_ivar_layout_emission_contract_id",
            "objc3c.executable.ivar.layout.emission.v1",
        ),
        (
            "storage_reflection_executable_synthesized_accessor_property_lowering_contract_id",
            "objc3c.executable.synthesized.accessor.property.lowering.v1",
        ),
        ("storage_reflection_local_property_descriptor_count", 6),
        ("storage_reflection_local_ivar_descriptor_count", 3),
        ("storage_reflection_implementation_owned_property_entries", 3),
        ("storage_reflection_synthesized_accessor_owner_entries", 3),
        ("storage_reflection_synthesized_getter_entries", 3),
        ("storage_reflection_synthesized_setter_entries", 3),
        ("storage_reflection_synthesized_accessor_entries", 6),
        ("storage_reflection_current_property_read_entries", 3),
        ("storage_reflection_current_property_write_entries", 2),
        ("storage_reflection_current_property_exchange_entries", 1),
        ("storage_reflection_weak_current_property_load_entries", 0),
        ("storage_reflection_weak_current_property_store_entries", 0),
        ("storage_reflection_ivar_layout_entries", 3),
        ("storage_reflection_ivar_layout_owner_entries", 1),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported storage-reflection module to preserve {field_name}",
        )
    expect(
        isinstance(imported_module.get("storage_reflection_replay_key"), str)
        and imported_module.get("storage_reflection_replay_key") != "",
        "expected imported storage-reflection module to preserve a replay key",
    )

    local_expected = {
        "local_storage_reflection_implementation_owned_property_entries": 0,
        "local_storage_reflection_synthesized_accessor_owner_entries": 0,
        "local_storage_reflection_synthesized_getter_entries": 0,
        "local_storage_reflection_synthesized_setter_entries": 0,
        "local_storage_reflection_synthesized_accessor_entries": 0,
        "local_storage_reflection_current_property_read_entries": 0,
        "local_storage_reflection_current_property_write_entries": 0,
        "local_storage_reflection_current_property_exchange_entries": 0,
        "local_storage_reflection_weak_current_property_load_entries": 0,
        "local_storage_reflection_weak_current_property_store_entries": 0,
        "local_storage_reflection_ivar_layout_entries": 0,
        "local_storage_reflection_ivar_layout_owner_entries": 0,
    }
    imported_expected = {
        "imported_storage_reflection_implementation_owned_property_entries": 3,
        "imported_storage_reflection_synthesized_accessor_owner_entries": 3,
        "imported_storage_reflection_synthesized_getter_entries": 3,
        "imported_storage_reflection_synthesized_setter_entries": 3,
        "imported_storage_reflection_synthesized_accessor_entries": 6,
        "imported_storage_reflection_current_property_read_entries": 3,
        "imported_storage_reflection_current_property_write_entries": 2,
        "imported_storage_reflection_current_property_exchange_entries": 1,
        "imported_storage_reflection_weak_current_property_load_entries": 0,
        "imported_storage_reflection_weak_current_property_store_entries": 0,
        "imported_storage_reflection_ivar_layout_entries": 3,
        "imported_storage_reflection_ivar_layout_owner_entries": 1,
    }
    transitive_expected = {
        "transitive_storage_reflection_implementation_owned_property_entries": 3,
        "transitive_storage_reflection_synthesized_accessor_owner_entries": 3,
        "transitive_storage_reflection_synthesized_getter_entries": 3,
        "transitive_storage_reflection_synthesized_setter_entries": 3,
        "transitive_storage_reflection_synthesized_accessor_entries": 6,
        "transitive_storage_reflection_current_property_read_entries": 3,
        "transitive_storage_reflection_current_property_write_entries": 2,
        "transitive_storage_reflection_current_property_exchange_entries": 1,
        "transitive_storage_reflection_weak_current_property_load_entries": 0,
        "transitive_storage_reflection_weak_current_property_store_entries": 0,
        "transitive_storage_reflection_ivar_layout_entries": 3,
        "transitive_storage_reflection_ivar_layout_owner_entries": 1,
    }
    for field_name, expected_value in local_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in imported_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in transitive_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-storage-reflection-artifact-preservation",
        probe=None,
        fixture=STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
            "consumer_fixture": STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": link_plan.get("local_module", {}).get("module_name"),
            "imported_property_descriptor_count": link_plan.get("imported_property_descriptor_count"),
            "imported_ivar_descriptor_count": link_plan.get("imported_ivar_descriptor_count"),
            "imported_synthesized_accessor_entries": link_plan.get(
                "imported_storage_reflection_synthesized_accessor_entries"
            ),
            "imported_ivar_layout_entries": link_plan.get(
                "imported_storage_reflection_ivar_layout_entries"
            ),
        },
    )


def check_cross_module_concurrency_actor_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-concurrency-actor-artifact-preservation"
    provider_fixture = ROOT / Path(CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    provider_actor_surface = provider_import_payload.get(
        "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface", {}
    )
    expect(
        isinstance(provider_actor_surface, dict),
        "expected concurrency actor provider import surface to publish the actor mailbox preservation packet",
    )
    expected_provider_fields = {
        "contract_id": "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        "source_contract_id": "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface",
        "source_model": "runtime-import-surface-preserves-actor-lowering-and-isolation-replay-facts-for-cross-module-runtime-link-planning",
        "fail_closed_model": "missing-or-drifted-actor-mailbox-runtime-import-packets-disable-cross-module-actor-runtime-preservation-claims",
    }
    for field_name, expected_value in expected_provider_fields.items():
        expect(
            provider_actor_surface.get(field_name) == expected_value,
            f"expected concurrency actor provider import surface to preserve {field_name}",
        )
    expect(
        provider_actor_surface.get("actor_mailbox_runtime_ready") is True
        and provider_actor_surface.get("deterministic") is True,
        "expected concurrency actor provider import surface to be runtime-ready and deterministic",
    )
    for field_name in (
        "replay_key",
        "actor_lowering_replay_key",
        "actor_isolation_lowering_replay_key",
    ):
        expect(
            isinstance(provider_actor_surface.get(field_name), str)
            and provider_actor_surface.get(field_name) != "",
            f"expected concurrency actor provider import surface to publish {field_name}",
        )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    expect(
        link_plan.get("concurrency_actor_imported_module_count") == 1,
        "expected cross-module link plan to publish one imported actor module",
    )
    expect(
        link_plan.get("concurrency_actor_imported_module_names_lexicographic")
        == [provider_import_payload.get("module_name")],
        "expected cross-module link plan to preserve the imported actor module names",
    )
    expect(
        link_plan.get("concurrency_actor_cross_module_isolation_ready") is True,
        "expected cross-module link plan to mark actor cross-module isolation ready",
    )
    expect(
        link_plan.get("expected_concurrency_actor_contract_id")
        == "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        "expected cross-module link plan to preserve the actor import contract id",
    )
    expect(
        link_plan.get("expected_concurrency_actor_source_contract_id")
        == "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "expected cross-module link plan to preserve the actor source contract id",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module actor link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name") == provider_import_payload.get("module_name"),
        "expected imported actor module name to match the provider import surface",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected imported actor module registration ordinal to remain one",
    )
    expect(
        imported_module.get("concurrency_actor_mailbox_runtime_import_present") is True
        and imported_module.get("concurrency_actor_mailbox_runtime_ready") is True
        and imported_module.get("concurrency_actor_mailbox_runtime_deterministic")
        is True,
        "expected imported actor module to preserve actor runtime import readiness",
    )
    for field_name, expected_value in (
        (
            "concurrency_actor_contract_id",
            "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        ),
        (
            "concurrency_actor_source_contract_id",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported actor module to preserve {field_name}",
        )
    for field_name in (
        "concurrency_actor_mailbox_runtime_replay_key",
        "concurrency_actor_lowering_replay_key",
        "concurrency_actor_isolation_lowering_replay_key",
    ):
        expect(
            isinstance(imported_module.get(field_name), str)
            and imported_module.get(field_name) != "",
            f"expected imported actor module to preserve {field_name}",
        )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            imported_module.get(field_name) == provider_registration_manifest.get(field_name),
            f"expected imported actor module to preserve {field_name}",
        )

    local_module = link_plan.get("local_module", {})
    expect(
        local_module.get("module_name") == "M270D003Consumer"
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module actor consumer to preserve the local module identity and registration ordinal",
    )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            local_module.get(field_name) == consumer_registration_manifest.get(field_name),
            f"expected local actor module to preserve {field_name}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-concurrency-actor-artifact-preservation",
        probe=None,
        fixture=CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
            "consumer_fixture": CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": local_module.get("module_name"),
            "imported_actor_registration_ordinal": imported_module.get(
                "translation_unit_registration_order_ordinal"
            ),
            "local_actor_registration_ordinal": local_module.get(
                "translation_unit_registration_order_ordinal"
            ),
        },
    )


def check_cross_module_block_ownership_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-block-ownership-artifact-preservation"
    provider_fixture = ROOT / Path(BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_block_surface = provider_import_payload.get(
        "objc_runtime_block_ownership_artifact_preservation", {}
    )
    expect(
        isinstance(provider_block_surface, dict),
        "expected block-ownership provider import surface to publish the preservation packet",
    )
    expected_provider_fields = {
        "contract_id": RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        "source_contract_id": RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        "block_object_invoke_thunk_lowering_contract_id": "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
        "block_byref_helper_lowering_contract_id": "objc3c.executable.block.byref.helper.lowering.v1",
        "block_escape_runtime_hook_lowering_contract_id": "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        "runtime_support_library_link_wiring_contract_id": "objc3c.runtime.support.library.link.wiring.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_runtime_block_ownership_artifact_preservation",
        "import_artifact_member_name": "objc_runtime_block_ownership_artifact_preservation",
        "source_model": "runtime-block-lowering-helper-surfaces-preserve-invoke-thunk-byref-copy-dispose-escape-and-runtime-link-facts-for-separate-compilation",
        "preservation_model": "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission",
        "fail_closed_model": "missing-or-drifted-block-ownership-preservation-packets-disable-cross-module-block-ownership-claims",
    }
    for field_name, expected_value in expected_provider_fields.items():
        expect(
            provider_block_surface.get(field_name) == expected_value,
            f"expected block-ownership provider import surface to preserve {field_name}",
        )
    for field_name, expected_value in (
        ("local_block_literal_sites", 1),
        ("local_invoke_trampoline_symbolized_sites", 1),
        ("local_copy_helper_required_sites", 1),
        ("local_dispose_helper_required_sites", 1),
        ("local_copy_helper_symbolized_sites", 1),
        ("local_dispose_helper_symbolized_sites", 1),
        ("local_escape_to_heap_sites", 1),
        ("local_byref_layout_symbolized_sites", 1),
    ):
        expect(
            provider_block_surface.get(field_name) == expected_value,
            f"expected block-ownership provider import surface to preserve {field_name}",
        )
    expect(
        provider_block_surface.get("runtime_import_artifact_ready") is True
        and provider_block_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_block_surface.get("runtime_support_library_link_wiring_ready")
        is True
        and provider_block_surface.get("deterministic") is True,
        "expected block-ownership provider import surface to be import-ready deterministic and runtime-link ready",
    )
    expect(
        isinstance(provider_block_surface.get("replay_key"), str)
        and provider_block_surface.get("replay_key") != "",
        "expected block-ownership provider import surface to publish a replay key",
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    for field_name, expected_value in (
        (
            "runtime_cross_module_block_ownership_artifact_preservation_surface_contract_id",
            RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "runtime_block_arc_lowering_helper_surface_contract_id",
            RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        ),
        (
            "block_object_invoke_thunk_lowering_contract_id",
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
        ),
        (
            "block_byref_helper_lowering_contract_id",
            "objc3c.executable.block.byref.helper.lowering.v1",
        ),
        (
            "block_escape_runtime_hook_lowering_contract_id",
            "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        ),
        (
            "block_runtime_support_library_link_wiring_contract_id",
            "objc3c.runtime.support.library.link.wiring.v1",
        ),
        (
            "block_ownership_artifact_preservation_model",
            "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission",
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module block-ownership link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("block_ownership_cross_module_preservation_ready") is True,
        "expected cross-module link plan to mark block-ownership preservation ready",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected block-ownership link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == provider_import_payload.get("module_name")
        == "m261_byref_cell_copy_dispose_runtime_positive",
        "expected block-ownership link plan to preserve the provider module name",
    )
    for field_name, expected_value in (
        ("block_ownership_artifact_preservation_present", True),
        ("block_ownership_runtime_import_artifact_ready", True),
        ("block_ownership_separate_compilation_preservation_ready", True),
        ("block_ownership_runtime_support_library_link_wiring_ready", True),
        ("block_ownership_deterministic", True),
        (
            "block_ownership_contract_id",
            RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "block_ownership_source_contract_id",
            RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        ),
        (
            "block_ownership_object_invoke_thunk_lowering_contract_id",
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
        ),
        (
            "block_ownership_byref_helper_lowering_contract_id",
            "objc3c.executable.block.byref.helper.lowering.v1",
        ),
        (
            "block_ownership_escape_runtime_hook_lowering_contract_id",
            "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        ),
        (
            "block_ownership_runtime_support_library_link_wiring_contract_id",
            "objc3c.runtime.support.library.link.wiring.v1",
        ),
        ("block_ownership_local_block_literal_sites", 1),
        ("block_ownership_local_invoke_trampoline_symbolized_sites", 1),
        ("block_ownership_local_copy_helper_required_sites", 1),
        ("block_ownership_local_dispose_helper_required_sites", 1),
        ("block_ownership_local_copy_helper_symbolized_sites", 1),
        ("block_ownership_local_dispose_helper_symbolized_sites", 1),
        ("block_ownership_local_escape_to_heap_sites", 1),
        ("block_ownership_local_byref_layout_symbolized_sites", 1),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported block-ownership module to preserve {field_name}",
        )
    expect(
        isinstance(imported_module.get("block_ownership_replay_key"), str)
        and imported_module.get("block_ownership_replay_key") != "",
        "expected imported block-ownership module to preserve a replay key",
    )

    local_expected = {
        "local_block_ownership_block_literal_sites": 0,
        "local_block_ownership_invoke_trampoline_symbolized_sites": 0,
        "local_block_ownership_copy_helper_required_sites": 0,
        "local_block_ownership_dispose_helper_required_sites": 0,
        "local_block_ownership_copy_helper_symbolized_sites": 0,
        "local_block_ownership_dispose_helper_symbolized_sites": 0,
        "local_block_ownership_escape_to_heap_sites": 0,
        "local_block_ownership_byref_layout_symbolized_sites": 0,
    }
    imported_expected = {
        "imported_block_ownership_block_literal_sites": 1,
        "imported_block_ownership_invoke_trampoline_symbolized_sites": 1,
        "imported_block_ownership_copy_helper_required_sites": 1,
        "imported_block_ownership_dispose_helper_required_sites": 1,
        "imported_block_ownership_copy_helper_symbolized_sites": 1,
        "imported_block_ownership_dispose_helper_symbolized_sites": 1,
        "imported_block_ownership_escape_to_heap_sites": 1,
        "imported_block_ownership_byref_layout_symbolized_sites": 1,
    }
    transitive_expected = {
        "transitive_block_ownership_block_literal_sites": 1,
        "transitive_block_ownership_invoke_trampoline_symbolized_sites": 1,
        "transitive_block_ownership_copy_helper_required_sites": 1,
        "transitive_block_ownership_dispose_helper_required_sites": 1,
        "transitive_block_ownership_copy_helper_symbolized_sites": 1,
        "transitive_block_ownership_dispose_helper_symbolized_sites": 1,
        "transitive_block_ownership_escape_to_heap_sites": 1,
        "transitive_block_ownership_byref_layout_symbolized_sites": 1,
    }
    for field_name, expected_value in local_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in imported_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in transitive_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-block-ownership-artifact-preservation",
        probe=None,
        fixture=BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE,
            "consumer_fixture": BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": link_plan.get("local_module", {}).get("module_name"),
            "imported_block_literal_sites": link_plan.get(
                "imported_block_ownership_block_literal_sites"
            ),
            "imported_copy_helper_required_sites": link_plan.get(
                "imported_block_ownership_copy_helper_required_sites"
            ),
            "imported_byref_layout_symbolized_sites": link_plan.get(
                "imported_block_ownership_byref_layout_symbolized_sites"
            ),
        },
    )


def check_imported_runtime_packaging_replay_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "imported-runtime-packaging-replay"
    provider_fixture = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE)
    probe = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_PROBE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    provider_obj = compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    if not provider_import_surface.is_file():
        raise RuntimeError(
            f"imported runtime provider did not publish {provider_import_surface}"
        )
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    consumer_obj = compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    if not link_plan_path.is_file():
        raise RuntimeError(
            f"imported runtime consumer did not publish {link_plan_path}"
        )
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    expect(
        link_plan.get("bootstrap_live_registration_contract_id")
        == "objc3c.runtime.live.registration.discovery.replay.v1",
        "expected cross-module link plan to preserve the live registration replay contract",
    )
    expect(
        link_plan.get("bootstrap_live_restart_hardening_contract_id")
        == "objc3c.runtime.live.restart.hardening.v1",
        "expected cross-module link plan to preserve the live restart hardening contract",
    )
    expect(
        link_plan.get("bootstrap_replay_registered_images_symbol")
        == "objc3_runtime_replay_registered_images_for_testing",
        "expected cross-module link plan to preserve the replay_registered_images symbol",
    )
    expect(
        link_plan.get("bootstrap_reset_replay_state_snapshot_symbol")
        == "objc3_runtime_copy_reset_replay_state_for_testing",
        "expected cross-module link plan to preserve the reset/replay snapshot symbol",
    )
    expect(
        link_plan.get("bootstrap_reset_for_testing_symbol")
        == "objc3_runtime_reset_for_testing",
        "expected cross-module link plan to preserve the reset_for_testing symbol",
    )
    expect(
        link_plan.get(
            "runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id"
        )
        == RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to publish the realized-metadata replay preservation surface contract",
    )
    expect(
        link_plan.get("runtime_object_model_realization_source_surface_contract_id")
        == RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the object-model realization source contract",
    )
    expect(
        link_plan.get(
            "runtime_realization_lowering_reflection_artifact_surface_contract_id"
        )
        == RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the realization/reflection artifact surface contract",
    )
    expect(
        link_plan.get(
            "runtime_dispatch_table_reflection_record_lowering_surface_contract_id"
        )
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the dispatch/reflection-record lowering surface contract",
    )
    expect(
        link_plan.get("realized_metadata_replay_preservation_model")
        == "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests",
        "expected cross-module link plan to publish the realized-metadata replay preservation model",
    )
    expect(
        link_plan.get("imported_live_registration_replay_ready") is True,
        "expected cross-module link plan to mark imported live registration replay ready",
    )
    expect(
        link_plan.get("imported_live_restart_hardening_ready") is True,
        "expected cross-module link plan to mark imported live restart hardening ready",
    )
    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        provider_import_payload.get("module_name") == imported_module.get("module_name"),
        "expected imported runtime surface module name to match the cross-module link plan",
    )
    expect(
        imported_module.get("module_name") == "runtimePackagingProvider",
        "expected cross-module link plan to preserve the imported provider module name",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected imported module registration ordinal to be preserved in the link plan",
    )
    expect(
        imported_module.get("ready_for_live_registration_discovery_replay") is True,
        "expected imported module to preserve live registration replay readiness",
    )
    expect(
        imported_module.get("ready_for_live_restart_hardening") is True,
        "expected imported module to preserve live restart hardening readiness",
    )
    for field_name, expected_value in (
        ("bootstrap_live_registration_contract_id", "objc3c.runtime.live.registration.discovery.replay.v1"),
        ("bootstrap_live_restart_hardening_contract_id", "objc3c.runtime.live.restart.hardening.v1"),
        ("bootstrap_live_replay_registered_images_symbol", "objc3_runtime_replay_registered_images_for_testing"),
        ("bootstrap_live_reset_replay_state_snapshot_symbol", "objc3_runtime_copy_reset_replay_state_for_testing"),
        ("bootstrap_live_restart_reset_for_testing_symbol", "objc3_runtime_reset_for_testing"),
        ("bootstrap_live_restart_replay_registered_images_symbol", "objc3_runtime_replay_registered_images_for_testing"),
        ("bootstrap_live_restart_reset_replay_state_snapshot_symbol", "objc3_runtime_copy_reset_replay_state_for_testing"),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported module to preserve {field_name}",
        )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            imported_module.get(field_name) == provider_registration_manifest.get(field_name),
            f"expected imported module to preserve {field_name}",
        )
    local_module = link_plan.get("local_module")
    expect(
        isinstance(local_module, dict)
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module link plan to preserve the local registration ordinal",
    )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            local_module.get(field_name) == consumer_registration_manifest.get(field_name),
            f"expected local module to preserve {field_name}",
        )
    expected_imported_counts = {
        "imported_class_descriptor_count": provider_registration_manifest["class_descriptor_count"],
        "imported_protocol_descriptor_count": provider_registration_manifest["protocol_descriptor_count"],
        "imported_category_descriptor_count": provider_registration_manifest["category_descriptor_count"],
        "imported_property_descriptor_count": provider_registration_manifest["property_descriptor_count"],
        "imported_ivar_descriptor_count": provider_registration_manifest["ivar_descriptor_count"],
        "imported_total_descriptor_count": provider_registration_manifest["total_descriptor_count"],
    }
    expected_local_counts = {
        "local_class_descriptor_count": consumer_registration_manifest["class_descriptor_count"],
        "local_protocol_descriptor_count": consumer_registration_manifest["protocol_descriptor_count"],
        "local_category_descriptor_count": consumer_registration_manifest["category_descriptor_count"],
        "local_property_descriptor_count": consumer_registration_manifest["property_descriptor_count"],
        "local_ivar_descriptor_count": consumer_registration_manifest["ivar_descriptor_count"],
        "local_total_descriptor_count": consumer_registration_manifest["total_descriptor_count"],
    }
    for field_name, expected_value in expected_imported_counts.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in expected_local_counts.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("module_image_count") == 2,
        "expected cross-module link plan to preserve a two-image runtime topology",
    )
    expect(
        link_plan.get("direct_import_input_count") == 1,
        "expected cross-module link plan to preserve one direct imported runtime surface",
    )
    for field_name, expected_value in (
        (
            "transitive_class_descriptor_count",
            provider_registration_manifest["class_descriptor_count"]
            + consumer_registration_manifest["class_descriptor_count"],
        ),
        (
            "transitive_protocol_descriptor_count",
            provider_registration_manifest["protocol_descriptor_count"]
            + consumer_registration_manifest["protocol_descriptor_count"],
        ),
        (
            "transitive_category_descriptor_count",
            provider_registration_manifest["category_descriptor_count"]
            + consumer_registration_manifest["category_descriptor_count"],
        ),
        (
            "transitive_property_descriptor_count",
            provider_registration_manifest["property_descriptor_count"]
            + consumer_registration_manifest["property_descriptor_count"],
        ),
        (
            "transitive_ivar_descriptor_count",
            provider_registration_manifest["ivar_descriptor_count"]
            + consumer_registration_manifest["ivar_descriptor_count"],
        ),
        (
            "transitive_total_descriptor_count",
            provider_registration_manifest["total_descriptor_count"]
            + consumer_registration_manifest["total_descriptor_count"],
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    link_object_artifacts = link_plan.get("link_object_artifacts")
    expect(
        isinstance(link_object_artifacts, list) and len(link_object_artifacts) == 2,
        "expected cross-module link plan to publish two ordered link objects",
    )

    exe_path = case_dir / "import_module_execution_matrix_probe.exe"
    probe_link_started = perf_counter()
    compile_probe(clangxx, probe, exe_path, [provider_obj, consumer_obj])
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)
    probe_run_started = perf_counter()
    payload = parse_json_output(
        run_probe(exe_path), "imported runtime cross-module packaging probe"
    )
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    provider_identity = provider_registration_manifest["translation_unit_identity_key"]
    consumer_identity = consumer_registration_manifest["translation_unit_identity_key"]
    expect(payload.get("startup_registration_copy_status") == 0, "expected imported-runtime startup registration snapshot copy to succeed")
    expect(payload.get("startup_registered_image_count") == 2, "expected imported-runtime startup to install two images")
    expect(
        payload.get("startup_registered_image_count") == link_plan.get("module_image_count"),
        "expected imported-runtime startup image count to match the cross-module link plan",
    )
    expect(payload.get("startup_next_expected_registration_order_ordinal") == 3, "expected imported-runtime startup to advance the next registration ordinal to three")
    expect(payload.get("startup_image_walk_status") == 0, "expected imported-runtime startup image-walk snapshot copy to succeed")
    expect(payload.get("startup_walked_image_count") == 2, "expected imported-runtime startup to walk both imported and local images")
    expect(payload.get("startup_last_walked_module_name") == "runtimePackagingConsumer", "expected imported-runtime startup to walk the local image last")
    expect(payload.get("startup_graph_status") == 0, "expected imported-runtime startup realized-class graph snapshot copy to succeed")
    expect(payload.get("startup_realized_class_count") == 2, "expected imported-runtime startup to realize both imported and local classes")
    expect(payload.get("startup_root_class_count") == 2, "expected imported-runtime startup to publish both classes as roots")
    expect(payload.get("startup_metaclass_edge_count") == 0, "expected imported-runtime startup realized-class graph to avoid metaclass edges")
    expect(payload.get("imported_entry_status") == 0 and payload.get("imported_entry_found") == 1, "expected imported provider runtime metadata to be realized at startup")
    expect(payload.get("local_entry_status") == 0 and payload.get("local_entry_found") == 1, "expected local consumer runtime metadata to be realized at startup")
    expect(payload.get("imported_registration_order_ordinal") == 1, "expected imported provider runtime metadata to preserve registration ordinal one")
    expect(payload.get("local_registration_order_ordinal") == 2, "expected local consumer runtime metadata to preserve registration ordinal two")
    expect(payload.get("imported_direct_protocol_count") == 1, "expected imported provider class to publish one direct protocol")
    expect(payload.get("imported_attached_protocol_count") == 0, "expected imported provider class to publish no attached protocols")
    expect(payload.get("imported_runtime_property_accessor_count") == 0, "expected imported provider class to publish no runtime property accessors")
    expect(payload.get("imported_module_name") == "runtimePackagingProvider", "expected imported provider class entry to preserve the provider module name")
    expect(isinstance(payload.get("imported_translation_unit_identity_key"), str) and payload.get("imported_translation_unit_identity_key") != "", "expected imported provider class entry to publish a non-empty translation unit identity key")
    expect(isinstance(payload.get("imported_class_owner_identity"), str) and payload.get("imported_class_owner_identity") != "", "expected imported provider class owner identity to be non-empty")
    expect(payload.get("local_direct_protocol_count") == 0, "expected local consumer class to publish no direct protocols")
    expect(payload.get("local_attached_protocol_count") == 0, "expected local consumer class to publish no attached protocols")
    expect(payload.get("local_runtime_property_accessor_count") == 0, "expected local consumer class to publish no runtime property accessors")
    expect(payload.get("local_module_name") == "runtimePackagingConsumer", "expected local consumer class entry to preserve the consumer module name")
    expect(isinstance(payload.get("local_translation_unit_identity_key"), str) and payload.get("local_translation_unit_identity_key") != "", "expected local consumer class entry to publish a non-empty translation unit identity key")
    expect(isinstance(payload.get("local_class_owner_identity"), str) and payload.get("local_class_owner_identity") != "", "expected local consumer class owner identity to be non-empty")
    expect(payload.get("protocol_query_attached_category_count") == 0, "expected imported-runtime protocol conformance query to publish no attached categories")
    imported_provider_class_value = payload.get("imported_provider_class_value")
    imported_provider_protocol_value = payload.get("imported_provider_protocol_value")
    local_consumer_class_value = payload.get("local_consumer_class_value")
    expect(
        isinstance(imported_provider_class_value, int)
        and imported_provider_class_value != 0,
        "expected imported provider class dispatch to succeed",
    )
    expect(
        isinstance(imported_provider_protocol_value, int)
        and imported_provider_protocol_value != 0,
        "expected imported provider protocol dispatch to succeed",
    )
    expect(
        isinstance(local_consumer_class_value, int)
        and local_consumer_class_value != 0,
        "expected local consumer class dispatch to succeed",
    )
    expect(
        len(
            {
                imported_provider_class_value,
                imported_provider_protocol_value,
                local_consumer_class_value,
            }
        )
        == 3,
        "expected imported and local selector dispatch results to remain distinct",
    )
    expect(payload.get("selector_table_status") == 0, "expected imported-runtime startup selector-table snapshot copy to succeed")
    expect(payload.get("selector_table_entry_count") == 3, "expected imported-runtime startup to publish three selector entries")
    expect(payload.get("selector_metadata_backed_selector_count") == 3, "expected imported-runtime startup to publish three metadata-backed selectors")
    expect(payload.get("selector_dynamic_selector_count") == 0, "expected imported-runtime startup to avoid dynamic selector entries")
    expect(payload.get("provider_selector_status") == 0 and payload.get("provider_selector_found") == 1, "expected provider class selector metadata to be installed at startup")
    expect(payload.get("provider_selector_metadata_backed") == 1, "expected provider class selector metadata to stay metadata-backed")
    expect(payload.get("provider_selector_provider_count") == 1, "expected provider class selector metadata to name one provider")
    expect(payload.get("provider_selector_first_ordinal") == 1, "expected provider class selector metadata to retain the provider registration ordinal")
    expect(payload.get("provider_selector_last_ordinal") == 1, "expected provider class selector metadata to end at the provider registration ordinal")
    expect(payload.get("imported_protocol_selector_status") == 0 and payload.get("imported_protocol_selector_found") == 1, "expected imported protocol selector metadata to be installed at startup")
    expect(payload.get("imported_protocol_selector_metadata_backed") == 1, "expected imported protocol selector metadata to stay metadata-backed")
    expect(payload.get("imported_protocol_selector_provider_count") == 1, "expected imported protocol selector metadata to name one provider")
    expect(payload.get("imported_protocol_selector_first_ordinal") == 1, "expected imported protocol selector metadata to retain the provider registration ordinal")
    expect(payload.get("imported_protocol_selector_last_ordinal") == 1, "expected imported protocol selector metadata to end at the provider registration ordinal")
    expect(payload.get("local_selector_status") == 0 and payload.get("local_selector_found") == 1, "expected local class selector metadata to be installed at startup")
    expect(payload.get("local_selector_metadata_backed") == 1, "expected local class selector metadata to stay metadata-backed")
    expect(payload.get("local_selector_provider_count") == 1, "expected local class selector metadata to name one provider")
    expect(payload.get("local_selector_first_ordinal") == 2, "expected local class selector metadata to retain the local registration ordinal")
    expect(payload.get("local_selector_last_ordinal") == 2, "expected local class selector metadata to end at the local registration ordinal")
    expect(payload.get("method_cache_state_status") == 0, "expected imported-runtime startup method-cache snapshot copy to succeed")
    expect(payload.get("method_cache_entry_count") == 3, "expected imported-runtime startup to publish three method-cache entries")
    expect(payload.get("method_cache_live_dispatch_count") == 0, "expected imported-runtime startup to avoid live dispatch fast-path entries")
    expect(payload.get("method_cache_fallback_dispatch_count") == 3, "expected imported-runtime startup to publish three metadata-backed fallback dispatches")
    expect(payload.get("method_cache_last_selector") == "localClassValue", "expected imported-runtime startup to publish the last resolved selector")
    expect(payload.get("method_cache_last_resolved_class_name") is None, "expected imported-runtime startup to keep the method-cache class name unset for metadata-backed fallback dispatch")
    expect(payload.get("method_cache_last_resolved_owner_identity") is None, "expected imported-runtime startup to keep the method-cache owner identity unset for metadata-backed fallback dispatch")
    expect(payload.get("provider_method_status") == 0 and payload.get("provider_method_found") == 1 and payload.get("provider_method_resolved") == 0, "expected provider class method metadata to remain an unresolved fallback entry at startup")
    expect(payload.get("provider_method_owner_identity") is None, "expected provider class fallback method metadata to avoid a resolved owner identity at startup")
    expect(payload.get("imported_protocol_method_status") == 0 and payload.get("imported_protocol_method_found") == 1 and payload.get("imported_protocol_method_resolved") == 0, "expected imported protocol method metadata to remain an unresolved fallback entry at startup")
    expect(payload.get("imported_protocol_method_owner_identity") is None, "expected imported protocol fallback metadata to avoid a resolved owner identity at startup")
    expect(payload.get("local_method_status") == 0 and payload.get("local_method_found") == 1 and payload.get("local_method_resolved") == 0, "expected local class method metadata to remain an unresolved fallback entry at startup")
    expect(payload.get("local_method_owner_identity") is None, "expected local class fallback metadata to avoid a resolved owner identity at startup")
    expect(payload.get("protocol_query_status") == 0, "expected imported-runtime startup protocol-conformance query snapshot copy to succeed")
    expect(payload.get("protocol_query_class_found") == 1 and payload.get("protocol_query_protocol_found") == 1 and payload.get("protocol_query_conforms") == 1, "expected imported provider protocol conformance to survive cross-module startup")
    expect(payload.get("protocol_query_visited_protocol_count") == 1, "expected imported-runtime startup to visit one protocol during conformance evaluation")
    expect(payload.get("protocol_query_attached_category_count") == 0, "expected imported-runtime startup to avoid category-backed protocol conformance")
    expect(payload.get("protocol_query_matched_protocol_owner_identity") == "", "expected imported-runtime startup protocol conformance to leave the matched protocol owner identity empty")
    expect(payload.get("post_reset_registration_copy_status") == 0, "expected post-reset registration snapshot copy to succeed")
    expect(payload.get("post_reset_replay_copy_status") == 0, "expected post-reset replay snapshot copy to succeed")
    expect(payload.get("post_reset_registered_image_count") == 0, "expected reset to clear installed images before replay")
    expect(payload.get("post_reset_retained_bootstrap_image_count") == 2, "expected reset to retain both imported and local bootstrap images for replay")
    expect(payload.get("post_reset_generation") == 1, "expected reset to advance the reset generation before replay")
    expect(payload.get("replay_status") == 0, "expected imported runtime replay to succeed")
    expect(payload.get("post_replay_registration_copy_status") == 0, "expected post-replay registration snapshot copy to succeed")
    expect(payload.get("post_replay_image_walk_status") == 0, "expected post-replay image-walk snapshot copy to succeed")
    expect(payload.get("post_replay_graph_status") == 0, "expected post-replay realized-class graph snapshot copy to succeed")
    expect(payload.get("post_replay_replay_copy_status") == 0, "expected post-replay replay snapshot copy to succeed")
    expect(payload.get("post_replay_registered_image_count") == 2, "expected replay to restore both imported and local images")
    expect(
        payload.get("post_replay_registered_image_count")
        == link_plan.get("module_image_count"),
        "expected replay image count to match the cross-module link plan",
    )
    expect(payload.get("post_replay_next_expected_registration_order_ordinal") == 3, "expected replay to restore the next registration ordinal to three")
    expect(payload.get("post_replay_walked_image_count") == 2, "expected replay to walk both imported and local images")
    expect(payload.get("post_replay_last_walked_module_name") == "runtimePackagingConsumer", "expected replay to walk the local image last")
    expect(payload.get("post_replay_realized_class_count") == 2, "expected replay to restore both realized classes")
    expect(payload.get("post_replay_replay_generation", 0) >= 1, "expected replay to advance the replay generation")
    expect(payload.get("post_replay_retained_bootstrap_image_count") == 2, "expected replay to preserve both retained bootstrap images")
    expect(payload.get("post_replay_imported_entry_status") == 0 and payload.get("post_replay_imported_entry_found") == 1, "expected imported provider runtime metadata to survive replay")
    expect(payload.get("post_replay_local_entry_status") == 0 and payload.get("post_replay_local_entry_found") == 1, "expected local consumer runtime metadata to survive replay")
    expect(payload.get("post_replay_imported_module_name") == "runtimePackagingProvider", "expected replay to preserve the provider module name")
    expect(payload.get("post_replay_imported_translation_unit_identity_key") == provider_identity, "expected replay to preserve the provider translation unit identity key")
    expect(payload.get("post_replay_local_module_name") == "runtimePackagingConsumer", "expected replay to preserve the consumer module name")
    expect(payload.get("post_replay_local_translation_unit_identity_key") == consumer_identity, "expected replay to preserve the consumer translation unit identity key")
    expect(
        payload.get("post_replay_imported_provider_class_value")
        == imported_provider_class_value,
        "expected imported provider class dispatch to survive replay",
    )
    expect(
        payload.get("post_replay_imported_provider_protocol_value")
        == imported_provider_protocol_value,
        "expected imported provider protocol dispatch to survive replay",
    )
    expect(
        payload.get("post_replay_local_consumer_class_value")
        == local_consumer_class_value,
        "expected local consumer class dispatch to survive replay",
    )
    expect(payload.get("post_replay_selector_table_status") == 0, "expected replay selector-table snapshot copy to succeed")
    expect(payload.get("post_replay_selector_table_entry_count") == 3, "expected replay to restore three selector entries")
    expect(payload.get("post_replay_selector_metadata_backed_selector_count") == 3, "expected replay to restore three metadata-backed selectors")
    expect(payload.get("post_replay_provider_selector_status") == 0 and payload.get("post_replay_provider_selector_found") == 1, "expected provider selector metadata to survive replay")
    expect(payload.get("post_replay_imported_protocol_selector_status") == 0 and payload.get("post_replay_imported_protocol_selector_found") == 1, "expected imported protocol selector metadata to survive replay")
    expect(payload.get("post_replay_local_selector_status") == 0 and payload.get("post_replay_local_selector_found") == 1, "expected local selector metadata to survive replay")
    expect(payload.get("post_replay_method_cache_state_status") == 0, "expected replay method-cache snapshot copy to succeed")
    expect(payload.get("post_replay_method_cache_entry_count") == 3, "expected replay to restore three method-cache entries")
    expect(payload.get("post_replay_method_cache_live_dispatch_count") == 0, "expected replay to keep method-cache entries on the fallback path")
    expect(payload.get("post_replay_method_cache_fallback_dispatch_count") == 3, "expected replay to restore three metadata-backed fallback dispatches")
    expect(payload.get("post_replay_method_cache_last_selector") == "localClassValue", "expected replay to preserve the last resolved selector")
    expect(payload.get("post_replay_method_cache_last_resolved_class_name") is None, "expected replay to keep the method-cache class name unset for metadata-backed fallback dispatch")
    expect(payload.get("post_replay_method_cache_last_resolved_owner_identity") is None, "expected replay to keep the method-cache owner identity unset for metadata-backed fallback dispatch")
    expect(payload.get("post_replay_provider_method_status") == 0 and payload.get("post_replay_provider_method_found") == 1 and payload.get("post_replay_provider_method_resolved") == 0, "expected provider class fallback metadata to survive replay")
    expect(payload.get("post_replay_provider_method_owner_identity") is None, "expected provider class fallback metadata to avoid a resolved owner identity after replay")
    expect(payload.get("post_replay_imported_protocol_method_status") == 0 and payload.get("post_replay_imported_protocol_method_found") == 1 and payload.get("post_replay_imported_protocol_method_resolved") == 0, "expected imported protocol fallback metadata to survive replay")
    expect(payload.get("post_replay_imported_protocol_method_owner_identity") is None, "expected imported protocol fallback metadata to avoid a resolved owner identity after replay")
    expect(payload.get("post_replay_local_method_status") == 0 and payload.get("post_replay_local_method_found") == 1 and payload.get("post_replay_local_method_resolved") == 0, "expected local class fallback metadata to survive replay")
    expect(payload.get("post_replay_local_method_owner_identity") is None, "expected local class fallback metadata to avoid a resolved owner identity after replay")

    return CaseResult(
        case_id="imported-runtime-packaging-replay",
        probe=IMPORTED_RUNTIME_PACKAGING_PROBE,
        fixture=IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "provider_fixture": IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            "provider_import_surface": str(provider_import_surface.relative_to(ROOT)).replace("\\", "/"),
            "link_plan": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "provider_translation_unit_identity_key": provider_identity,
            "consumer_translation_unit_identity_key": consumer_identity,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "probe_link_ms": probe_link_ms,
            "probe_run_ms": probe_run_ms,
            "case_total_ms": case_total_ms,
        },
    )


def check_canonical_dispatch_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "canonical-dispatch"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "runtime_canonical_runnable_object_runtime_library.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_canonical_runnable_object_probe.cpp"
    exe_path = case_dir / "runtime_canonical_runnable_object_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "canonical dispatch probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    expect(payload.get("traced_value") == 13, "expected category-backed tracedValue dispatch to return 13")
    expect(payload.get("inherited_value") == 7, "expected superclass inheritedValue dispatch to return 7")
    expect(payload.get("class_value") == 11, "expected class method dispatch to return 11")
    expect(payload.get("alloc_value", 0) != 0, "expected alloc dispatch to return a realized instance receiver")
    expect(payload.get("init_value") == payload.get("alloc_value"), "expected init to preserve the allocated receiver")
    expect(payload.get("new_value", 0) != 0, "expected new dispatch to materialize an instance receiver")
    expect(payload.get("ignored_value") == payload.get("ignored_expected"),
           "expected unresolved selector dispatch to return the deterministic fallback value")
    expect(payload.get("ignored_cached_value") == payload.get("ignored_expected"),
           "expected cached unresolved selector dispatch to preserve the deterministic fallback value")

    worker_query = payload.get("worker_query", {})
    tracer_query = payload.get("tracer_query", {})
    graph_state = payload.get("graph_state", {})
    widget_entry = payload.get("widget_entry", {})
    method_state = payload.get("method_state", {})
    inherited_state = payload.get("inherited_state", {})
    traced_state = payload.get("traced_state", {})
    class_state = payload.get("class_state", {})
    ignored_state = payload.get("ignored_state", {})
    ignored_cached_state = payload.get("ignored_cached_state", {})
    selector_handles = payload.get("selector_handles", {})
    selector_table_state = payload.get("selector_table_state", {})
    traced_selector_entry = payload.get("traced_selector_entry", {})
    inherited_selector_entry = payload.get("inherited_selector_entry", {})
    class_selector_entry = payload.get("class_selector_entry", {})
    ignored_selector_entry = payload.get("ignored_selector_entry", {})
    traced_entry = payload.get("traced_entry", {})
    inherited_entry = payload.get("inherited_entry", {})
    class_entry = payload.get("class_entry", {})
    ignored_entry = payload.get("ignored_entry", {})
    alloc_entry = payload.get("alloc_entry", {})
    init_entry = payload.get("init_entry", {})
    new_entry = payload.get("new_entry", {})

    expect(worker_query.get("conforms") == 1, "expected Widget to conform to Worker at runtime")
    expect(tracer_query.get("conforms") == 1, "expected Widget category attachment to satisfy Tracer at runtime")
    expect(
        graph_state.get("realized_class_count") == 2
        and graph_state.get("root_class_count") == 1
        and graph_state.get("metaclass_edge_count") == 1
        and graph_state.get("receiver_class_binding_count") == 2
        and graph_state.get("attached_category_count") == 1
        and graph_state.get("protocol_conformance_edge_count") == 2
        and graph_state.get("last_realized_class_name") == "Widget"
        and graph_state.get("last_realized_class_owner_identity") == "class:Widget"
        and graph_state.get("last_realized_metaclass_owner_identity") == "metaclass:Widget"
        and graph_state.get("last_attached_category_owner_identity") == "category:Widget(Tracing)"
        and graph_state.get("last_attached_category_name") == "Tracing",
        "expected canonical dispatch to publish the realized Widget class graph with stable class, metaclass, and attached-category lineage",
    )
    expect(
        widget_entry.get("found") == 1
        and widget_entry.get("is_root_class") == 0
        and widget_entry.get("implementation_backed") == 1
        and widget_entry.get("attached_category_count") == 1
        and widget_entry.get("direct_protocol_count") == 1
        and widget_entry.get("attached_protocol_count") == 1
        and widget_entry.get("class_name") == "Widget"
        and widget_entry.get("class_owner_identity") == "class:Widget"
        and widget_entry.get("metaclass_owner_identity") == "metaclass:Widget"
        and widget_entry.get("super_class_owner_identity") == "class:Base"
        and widget_entry.get("super_metaclass_owner_identity") == "metaclass:Base"
        and widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)"
        and widget_entry.get("last_attached_category_name") == "Tracing",
        "expected canonical dispatch to publish stable Widget class, metaclass, superclass, attached-category, and protocol realization facts",
    )
    expect(
        tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer"
        and tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)",
        "expected Tracer conformance to resolve through the attached Widget(Tracing) category",
    )
    expect(method_state.get("live_dispatch_count", 0) >= 6, "expected live dispatch count to cover alloc/init/new/traced/inherited/class")
    expect(method_state.get("fallback_dispatch_count", 0) == 2, "expected canonical dispatch workload to publish both unresolved fallback calls")
    expect(method_state.get("last_selector_stable_id", 0) == ignored_entry.get("selector_stable_id", 0),
           "expected last dispatch selector stable id to match the negative-cache ignoredValue selector")
    expect(selector_handles.get("alloc", 0) != 0 and selector_handles.get("tracedValue", 0) != 0,
           "expected canonical dispatch selectors to be interned in the runtime selector pool")
    expect(alloc_entry.get("selector_stable_id", 0) == selector_handles.get("alloc", 0),
           "expected alloc cache entry to be keyed by the selector pool stable id")
    expect(init_entry.get("selector_stable_id", 0) == selector_handles.get("init", 0),
           "expected init cache entry to be keyed by the selector pool stable id")
    expect(new_entry.get("selector_stable_id", 0) == selector_handles.get("new", 0),
           "expected new cache entry to be keyed by the selector pool stable id")
    expect(traced_entry.get("selector_stable_id", 0) == selector_handles.get("tracedValue", 0),
           "expected tracedValue cache entry to be keyed by the selector pool stable id")
    expect(inherited_entry.get("selector_stable_id", 0) == selector_handles.get("inheritedValue", 0),
           "expected inheritedValue cache entry to be keyed by the selector pool stable id")
    expect(class_entry.get("selector_stable_id", 0) == selector_handles.get("classValue", 0),
           "expected classValue cache entry to be keyed by the selector pool stable id")
    expect(traced_entry.get("resolved") == 1, "expected tracedValue cache entry to resolve live")
    expect(inherited_entry.get("resolved") == 1, "expected inheritedValue cache entry to resolve live")
    expect(class_entry.get("resolved") == 1, "expected classValue cache entry to resolve live")
    expect(selector_table_state.get("metadata_backed_selector_count", 0) >= 4,
           "expected canonical dispatch selector materialization to keep metadata-backed selectors interned")
    expect(selector_table_state.get("dynamic_selector_count", 0) >= 1,
           "expected unresolved selector dispatch to intern a dynamic selector entry")
    expect(selector_table_state.get("last_materialized_selector") == "ignoredValue",
           "expected ignoredValue to be the last materialized selector after the fallback probe")
    expect(selector_table_state.get("last_materialized_from_metadata") == 0,
           "expected ignoredValue to be recorded as a dynamic selector lookup")
    expect(
        traced_selector_entry.get("found") == 1
        and traced_selector_entry.get("metadata_backed") == 1
        and traced_selector_entry.get("canonical_selector") == "tracedValue",
        "expected tracedValue to remain metadata-backed in the selector table",
    )
    expect(
        inherited_selector_entry.get("found") == 1
        and inherited_selector_entry.get("metadata_backed") == 1
        and inherited_selector_entry.get("canonical_selector") == "inheritedValue",
        "expected inheritedValue to remain metadata-backed in the selector table",
    )
    expect(
        class_selector_entry.get("found") == 1
        and class_selector_entry.get("metadata_backed") == 1
        and class_selector_entry.get("canonical_selector") == "classValue",
        "expected classValue to remain metadata-backed in the selector table",
    )
    expect(
        ignored_selector_entry.get("found") == 1
        and ignored_selector_entry.get("metadata_backed") == 0
        and ignored_selector_entry.get("canonical_selector") == "ignoredValue",
        "expected ignoredValue to materialize as a dynamic selector-table entry",
    )
    expect(
        inherited_state.get("last_dispatch_used_cache") == 0
        and inherited_state.get("last_dispatch_resolved_live_method") == 1
        and inherited_state.get("last_dispatch_fell_back") == 0
        and inherited_state.get("last_selector_stable_id") == selector_handles.get("inheritedValue", 0)
        and inherited_state.get("last_normalized_receiver_identity") == 1042
        and inherited_state.get("last_category_probe_count") == 1
        and inherited_state.get("last_protocol_probe_count") == 3,
        "expected inheritedValue to miss cache first, resolve live, and preserve category/protocol probe counts",
    )
    expect(
        traced_state.get("last_dispatch_used_cache") == 0
        and traced_state.get("last_dispatch_resolved_live_method") == 1
        and traced_state.get("last_dispatch_fell_back") == 0
        and traced_state.get("last_selector_stable_id") == selector_handles.get("tracedValue", 0)
        and traced_state.get("last_normalized_receiver_identity") == 1042
        and traced_state.get("last_category_probe_count") == 1
        and traced_state.get("last_protocol_probe_count") == 0,
        "expected tracedValue to resolve live through the attached category without protocol fallback probes",
    )
    expect(
        class_state.get("last_dispatch_used_cache") == 0
        and class_state.get("last_dispatch_resolved_live_method") == 1
        and class_state.get("last_dispatch_fell_back") == 0
        and class_state.get("last_selector_stable_id") == selector_handles.get("classValue", 0)
        and class_state.get("last_normalized_receiver_identity") == 1043,
        "expected classValue to resolve live through the metaclass path",
    )
    expect(
        ignored_state.get("last_dispatch_used_cache") == 0
        and ignored_state.get("last_dispatch_resolved_live_method") == 0
        and ignored_state.get("last_dispatch_fell_back") == 1
        and ignored_state.get("last_selector_stable_id") == ignored_entry.get("selector_stable_id", 0)
        and ignored_state.get("last_normalized_receiver_identity") == 1042
        and ignored_state.get("last_category_probe_count") == 1
        and ignored_state.get("last_protocol_probe_count") == 3,
        "expected the first ignoredValue dispatch to materialize a negative cache entry and fall back deterministically",
    )
    expect(
        ignored_cached_state.get("last_dispatch_used_cache") == 1
        and ignored_cached_state.get("last_dispatch_resolved_live_method") == 0
        and ignored_cached_state.get("last_dispatch_fell_back") == 1
        and ignored_cached_state.get("last_selector_stable_id") == ignored_entry.get("selector_stable_id", 0)
        and ignored_cached_state.get("last_normalized_receiver_identity") == 1042
        and ignored_cached_state.get("last_category_probe_count") == 1
        and ignored_cached_state.get("last_protocol_probe_count") == 3,
        "expected the second ignoredValue dispatch to reuse the negative cache entry and preserve probe counts",
    )
    expect(
        traced_entry.get("resolved_owner_identity") == "implementation:Widget(Tracing)::instance_method:tracedValue",
        "expected tracedValue cache entry to preserve the category implementation owner",
    )
    expect(
        inherited_entry.get("normalized_receiver_identity") == 1042
        and inherited_entry.get("category_probe_count") == 1
        and inherited_entry.get("protocol_probe_count") == 3
        and inherited_entry.get("resolved_class_name") == "Base"
        and inherited_entry.get("resolved_owner_identity") == "implementation:Base::instance_method:inheritedValue",
        "expected inheritedValue cache entry to preserve the instance-family lookup result through Base",
    )
    expect(
        class_entry.get("dispatch_family_is_class") == 1
        and class_entry.get("normalized_receiver_identity") == 1043
        and class_entry.get("resolved_class_name") == "Widget"
        and class_entry.get("resolved_owner_identity") == "implementation:Widget::class_method:classValue",
        "expected classValue cache entry to preserve the metaclass lookup result",
    )
    expect(
        ignored_entry.get("found") == 1
        and ignored_entry.get("resolved") == 0
        and ignored_entry.get("dispatch_family_is_class") == 0
        and ignored_entry.get("normalized_receiver_identity") == 1042
        and ignored_entry.get("category_probe_count") == 1
        and ignored_entry.get("protocol_probe_count") == 3
        and ignored_entry.get("selector") == "ignoredValue",
        "expected ignoredValue to preserve an unresolved negative cache entry on the canonical instance receiver",
    )
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface", {}
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected canonical dispatch compile manifest to publish dispatch-table/reflection-record lowering surface",
    )
    expect(
        "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
        in ll_text,
        "expected canonical dispatch LLVM IR to publish executable realization records",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("selector_pool_section_root_symbol")
        == "@__objc3_sec_selector_pool",
        "expected canonical dispatch lowering surface to preserve the selector pool section root",
    )

    return CaseResult(
        case_id="canonical-dispatch",
        probe="tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "traced_value": payload["traced_value"],
            "inherited_value": payload["inherited_value"],
            "class_value": payload["class_value"],
            "live_dispatch_count": method_state["live_dispatch_count"],
            "attached_category_count": payload.get("graph_state", {}).get("attached_category_count"),
            "ignored_fallback": payload["ignored_expected"],
        },
    )


def check_canonical_sample_set_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "canonical-sample-set"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "canonical_runnable_sample_set.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    probe = ROOT / "tests" / "tooling" / "runtime" / "canonical_runnable_sample_set_probe.cpp"
    exe_path = case_dir / "canonical_runnable_sample_set_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "canonical runnable sample set probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    widget_entry = payload.get("widget_entry", {})
    worker_query = payload.get("worker_query", {})
    tracer_query = payload.get("tracer_query", {})
    count_property = payload.get("count_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})

    expect(widget_entry.get("found") == 1, "expected Widget to realize successfully for the canonical sample set")
    expect(widget_entry.get("base_identity") == 1041, "expected Widget base identity 1041 for the canonical sample set")
    expect(widget_entry.get("runtime_property_accessor_count") == 4, "expected Widget to expose four runtime property accessors")
    expect(widget_entry.get("runtime_instance_size_bytes") == 24, "expected Widget instance size to remain 24 bytes")
    expect(
        widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)",
        "expected Widget to preserve the attached Tracing category owner",
    )

    expect(registration_manifest.get("class_descriptor_count") == 4, "expected canonical sample set to publish four class descriptors")
    expect(registration_manifest.get("protocol_descriptor_count") == 2, "expected canonical sample set to publish two protocol descriptors")
    expect(registration_manifest.get("category_descriptor_count") == 2, "expected canonical sample set to publish two category descriptors")
    expect(registration_manifest.get("property_descriptor_count") == 8, "expected canonical sample set to publish eight property descriptors")
    expect(registration_manifest.get("ivar_descriptor_count") == 4, "expected canonical sample set to publish four ivar descriptors")
    expect(
        registration_manifest.get("compile_output_truthfulness_property_descriptor_count") == 8,
        "expected compile-output truthfulness to certify eight property descriptors for the canonical sample set",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count") == 4,
        "expected compile-output truthfulness to certify four ivar descriptors for the canonical sample set",
    )

    expect(payload.get("init_value", 0) != 0, "expected alloc/init to return a non-zero canonical sample-set receiver")
    expect(payload.get("traced_value") == 13, "expected tracedValue to return 13 for the canonical sample set")
    expect(payload.get("inherited_value") == 7, "expected inheritedValue to return 7 for the canonical sample set")
    expect(payload.get("class_value") == 11, "expected classValue to return 11 for the canonical sample set")
    expect(payload.get("shared_value") == 19, "expected shared to return 19 for the canonical sample set")
    expect(payload.get("count_value") == 37, "expected count to reload 37 for the canonical sample set")
    expect(payload.get("enabled_value") == 1, "expected enabled to reload 1 for the canonical sample set")
    expect(payload.get("current_value") == 55, "expected currentValue to reload 55 for the canonical sample set")
    expect(payload.get("token_value") == 0, "expected tokenValue to remain 0 for the canonical sample set")

    expect(worker_query.get("conforms") == 1, "expected Widget to conform to Worker in the canonical sample set")
    expect(
        worker_query.get("matched_protocol_owner_identity") in {"protocol:Worker", "protocol:Tracer"},
        "expected Worker query to resolve through Worker or inherited Tracer",
    )
    expect(worker_query.get("matched_attachment_owner_identity") is None, "did not expect Worker query to require an attachment-owner match")
    expect(tracer_query.get("conforms") == 1, "expected Widget to conform to Tracer in the canonical sample set")
    expect(tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer", "expected Tracer query to resolve through protocol:Tracer")
    expect(
        tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)",
        "expected Tracer query to resolve through the attached category owner",
    )

    expect(
        count_property.get("found") == 1
        and count_property.get("slot_index") == 0
        and count_property.get("offset_bytes") == 0
        and count_property.get("size_bytes") == 4
        and count_property.get("getter_owner_identity") == "implementation:Widget::instance_method:count"
        and count_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCount:",
        "expected count property reflection to preserve slot/layout/accessor facts",
    )
    expect(
        value_property.get("found") == 1
        and value_property.get("slot_index") == 2
        and value_property.get("offset_bytes") == 8
        and value_property.get("size_bytes") == 8
        and value_property.get("getter_owner_identity") == "implementation:Widget::instance_method:currentValue"
        and value_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCurrentValue:",
        "expected currentValue property reflection to preserve slot/layout/accessor facts",
    )
    expect(
        token_property.get("found") == 1
        and token_property.get("slot_index") == 3
        and token_property.get("offset_bytes") == 16
        and token_property.get("setter_available") == 0
        and token_property.get("getter_owner_identity") == "implementation:Widget::instance_method:tokenValue"
        and token_property.get("setter_owner_identity") is None,
        "expected tokenValue property reflection to preserve readonly slot/layout/accessor facts",
    )
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface", {}
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected canonical sample set compile manifest to publish dispatch-table/reflection-record lowering surface",
    )
    expect(
        "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
        in ll_text,
        "expected canonical sample set LLVM IR to publish executable realization records",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("class_aggregate_symbol")
        == "__objc3_sec_class_descriptors",
        "expected canonical sample set lowering surface to preserve the class aggregate root symbol",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("property_aggregate_symbol")
        == "__objc3_sec_property_descriptors",
        "expected canonical sample set lowering surface to preserve the property aggregate root symbol",
    )

    return CaseResult(
        case_id="canonical-sample-set",
        probe="tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
        fixture="tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "widget_base_identity": widget_entry.get("base_identity"),
            "traced_value": payload["traced_value"],
            "inherited_value": payload["inherited_value"],
            "class_value": payload["class_value"],
            "shared_value": payload["shared_value"],
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        },
    )


def check_realization_lookup_reflection_runtime_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "realization-lookup-reflection-runtime"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "canonical_runnable_sample_set.objc3"
    )
    obj_path, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = (
        case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )

    probe = ROOT / REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE
    exe_path = case_dir / "object_model_lookup_reflection_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(
        run_probe(exe_path), "realization lookup reflection runtime probe"
    )
    aggregate = payload.get("aggregate", {})
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

    expect(payload.get("widget_found") == 1, "expected Widget class lookup to succeed")
    expect(payload.get("traced_value") == 13, "expected tracedValue to return 13")
    expect(payload.get("count_value") == 41, "expected count to reload the written value")
    expect(
        payload.get("count_property_found") == 1,
        "expected count property reflection lookup to succeed",
    )
    expect(payload.get("tracer_conforms") == 1, "expected Widget to conform to Tracer")
    expect(
        aggregate.get("realized_class_count") == 2,
        "expected aggregate realized class count to report the two live instance-class nodes",
    )
    expect(
        aggregate.get("reflectable_property_count") == 4,
        "expected aggregate reflectable property count to report the four live Widget/Base property accessors",
    )
    expect(
        aggregate.get("attached_category_count") == 1,
        "expected aggregate attached category count to report the single live attached category",
    )
    expect(
        aggregate.get("protocol_conformance_edge_count", 0) >= 2,
        "expected aggregate protocol conformance edge count to stay live",
    )
    expect(
        aggregate.get("method_cache_entry_count", 0) >= 4,
        "expected aggregate method cache entry count to reflect the executed dispatches",
    )
    expect(
        aggregate.get("last_class_query_found") == 1
        and aggregate.get("last_queried_class_name") == "Widget"
        and aggregate.get("last_resolved_class_name") == "Widget",
        "expected aggregate state to preserve the last class lookup",
    )
    expect(
        aggregate.get("last_property_query_found") == 1
        and aggregate.get("last_property_query_inherited") == 0
        and aggregate.get("last_queried_property_name") == "count"
        and aggregate.get("last_resolved_property_class_name") == "Widget",
        "expected aggregate state to preserve the last property lookup",
    )
    expect(
        aggregate.get("last_protocol_query_class_found") == 1
        and aggregate.get("last_protocol_query_protocol_found") == 1
        and aggregate.get("last_protocol_query_conforms") == 1
        and aggregate.get("last_queried_protocol_class_name") == "Widget"
        and aggregate.get("last_queried_protocol_name") == "Tracer",
        "expected aggregate state to preserve the last protocol-conformance query",
    )
    expect(
        bool(aggregate.get("last_resolved_class_owner_identity"))
        and bool(aggregate.get("last_resolved_property_owner_identity"))
        and bool(aggregate.get("last_matched_protocol_owner_identity")),
        "expected aggregate state to preserve the last resolved owner identities",
    )
    expect(
        registration_manifest.get("class_descriptor_count") == 4
        and registration_manifest.get("property_descriptor_count") == 8
        and registration_manifest.get("category_descriptor_count") == 2,
        "expected canonical sample-set registration manifests to keep the broader emitted descriptor counts",
    )
    implementation_surface = manifest.get(
        "runtime_realization_lookup_reflection_implementation_surface", {}
    )
    expect(
        implementation_surface.get("contract_id")
        == RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "expected compile manifest to publish the realization lookup/reflection implementation surface",
    )

    return CaseResult(
        case_id="realization-lookup-reflection-runtime",
        probe=REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
        fixture="tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "realized_class_count": aggregate["realized_class_count"],
            "reflectable_property_count": aggregate["reflectable_property_count"],
            "method_cache_entry_count": aggregate["method_cache_entry_count"],
            "last_queried_protocol_name": aggregate["last_queried_protocol_name"],
        },
    )


def check_live_dispatch_fast_path_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "dispatch-fast-path"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "live_dispatch_fast_path_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "live_dispatch_fast_path_probe.cpp"
    exe_path = case_dir / "live_dispatch_fast_path_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_key_value_output(run_probe(exe_path), "dispatch fast-path probe")
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))

    expect(payload.get("baseline_status") == 0, "expected baseline method-cache snapshot to succeed")
    expect(payload.get("dynamic_entry_status") == 0, "expected dynamic fast-path entry lookup to succeed")
    expect(payload.get("explicit_entry_status") == 0, "expected explicit fast-path entry lookup to succeed")
    expect(payload.get("fallback_entry_status") == 0, "expected fallback method-cache entry lookup to succeed")
    expect(payload.get("implicit_value") == 3, "expected implicit direct call to remain direct")
    expect(payload.get("explicit_value") == 5, "expected explicit direct call to remain direct")
    expect(payload.get("mixed_first") == 12 and payload.get("mixed_second") == 12,
           "expected mixed dispatch fixture to execute through the live runtime")
    expect(payload.get("fallback_first") == payload.get("fallback_expected") == payload.get("fallback_second"),
           "expected fallback dispatch to stay deterministic across cache miss/hit")
    expect(payload.get("baseline_cache_entry_count") == 4,
           "expected realized dispatch runtime to seed four method-cache entries")
    expect(payload.get("baseline_fast_path_seed_count") == 4,
           "expected realized dispatch runtime to publish seeded fast-path entries")
    expect(payload.get("dynamic_entry_found") == 1 and payload.get("dynamic_entry_resolved") == 1,
           "expected dynamicEscape entry to resolve live")
    expect(payload.get("dynamic_entry_fast_path_seeded") == 1,
           "expected dynamicEscape entry to be seeded for fast-path dispatch")
    expect(payload.get("dynamic_entry_effective_direct_dispatch") == 0,
           "expected dynamicEscape entry to stay runtime-dispatched")
    expect(payload.get("dynamic_entry_fast_path_reason") == "class-final",
           "expected dynamicEscape fast-path reason to remain class-final")
    expect(payload.get("explicit_entry_found") == 1 and payload.get("explicit_entry_resolved") == 1,
           "expected explicitDirect entry to resolve live")
    expect(payload.get("explicit_entry_fast_path_seeded") == 1,
           "expected explicitDirect entry to be seeded for direct dispatch")
    expect(payload.get("explicit_entry_effective_direct_dispatch") == 1,
           "expected explicitDirect entry to preserve direct dispatch semantics")
    expect(payload.get("explicit_entry_fast_path_reason") == "direct",
           "expected explicitDirect fast-path reason to remain direct")
    expect(payload.get("mixed_first_state_last_dispatch_used_cache") == 1,
           "expected first mixed dispatch runtime call to hit the seeded cache")
    expect(payload.get("mixed_first_state_last_dispatch_used_fast_path") == 1,
           "expected first mixed dispatch runtime call to use the seeded fast path")
    expect(payload.get("mixed_first_state_last_dispatch_resolved_live_method") == 1,
           "expected first mixed dispatch runtime call to resolve a live method")
    expect(payload.get("mixed_first_state_last_dispatch_fell_back") == 0,
           "did not expect first mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_first_state_last_selector") == "dynamicEscape",
           "expected first mixed dispatch runtime call to target dynamicEscape")
    expect(payload.get("mixed_first_dispatch_state_status") == 0,
           "expected first mixed dispatch runtime call to publish dispatch state")
    expect(payload.get("mixed_first_dispatch_state_last_dispatch_path") == "cache-hit-fast-path",
           "expected first mixed dispatch runtime call to report the cache-hit fast path")
    expect(payload.get("mixed_first_dispatch_state_last_implementation_kind") == "emitted-method-body",
           "expected first mixed dispatch runtime call to execute an emitted method body")
    expect(payload.get("mixed_first_dispatch_state_last_effective_direct_dispatch") == 0,
           "expected first mixed dispatch runtime call to remain runtime-dispatched")
    expect(payload.get("mixed_first_dispatch_state_last_used_builtin") == 0,
           "expected first mixed dispatch runtime call to avoid builtin dispatch")
    expect(payload.get("mixed_second_state_last_dispatch_used_cache") == 1,
           "expected repeated mixed dispatch runtime call to remain cached")
    expect(payload.get("mixed_second_state_last_dispatch_used_fast_path") == 1,
           "expected repeated mixed dispatch runtime call to remain on the fast path")
    expect(payload.get("mixed_second_state_last_dispatch_fell_back") == 0,
           "did not expect repeated mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_second_dispatch_state_status") == 0,
           "expected repeated mixed dispatch runtime call to publish dispatch state")
    expect(payload.get("mixed_second_dispatch_state_last_dispatch_path") == "cache-hit-fast-path",
           "expected repeated mixed dispatch runtime call to stay on the cache-hit fast path")
    expect(payload.get("mixed_second_dispatch_state_last_implementation_kind") == "emitted-method-body",
           "expected repeated mixed dispatch runtime call to execute an emitted method body")
    expect(payload.get("mixed_second_dispatch_state_last_effective_direct_dispatch") == 0,
           "expected repeated mixed dispatch runtime call to remain runtime-dispatched")
    expect(payload.get("mixed_second_dispatch_state_last_used_builtin") == 0,
           "expected repeated mixed dispatch runtime call to avoid builtin dispatch")
    expect(payload.get("fallback_first_state_last_dispatch_used_cache") == 0,
           "expected first missingDispatch: call to miss the cache")
    expect(payload.get("fallback_first_state_last_dispatch_used_fast_path") == 0,
           "expected first missingDispatch: call to avoid the fast path")
    expect(payload.get("fallback_first_state_last_dispatch_resolved_live_method") == 0,
           "did not expect first missingDispatch: call to resolve live")
    expect(payload.get("fallback_first_state_last_dispatch_fell_back") == 1,
           "expected first missingDispatch: call to fall back")
    expect(payload.get("fallback_first_dispatch_state_status") == 0,
           "expected first missingDispatch: call to publish dispatch state")
    expect(payload.get("fallback_first_dispatch_state_last_dispatch_path") == "slow-path-fallback",
           "expected first missingDispatch: call to report slow-path fallback dispatch")
    expect(payload.get("fallback_first_dispatch_state_last_implementation_kind") == "fallback-formula",
           "expected first missingDispatch: call to execute deterministic fallback formula")
    expect(payload.get("fallback_second_state_last_dispatch_used_cache") == 1,
           "expected repeated missingDispatch: call to hit the fallback cache entry")
    expect(payload.get("fallback_second_state_last_dispatch_used_fast_path") == 0,
           "expected repeated missingDispatch: call to stay off the fast path")
    expect(payload.get("fallback_second_state_last_dispatch_fell_back") == 1,
           "expected repeated missingDispatch: call to remain a fallback dispatch")
    expect(payload.get("fallback_second_dispatch_state_status") == 0,
           "expected repeated missingDispatch: call to publish dispatch state")
    expect(payload.get("fallback_second_dispatch_state_last_dispatch_path") == "cache-hit-fallback",
           "expected repeated missingDispatch: call to report cached fallback dispatch")
    expect(payload.get("fallback_second_dispatch_state_last_implementation_kind") == "fallback-formula",
           "expected repeated missingDispatch: call to execute deterministic fallback formula")
    expect(
        "; method_dispatch_and_selector_thunk_lowering_surface = "
        "contract_id=objc3c.method.dispatch.selector.thunk.lowering.v1"
        in ll_text,
        "expected LLVM IR to publish authoritative method dispatch and selector thunk lowering surface",
    )
    expect("direct_dispatch_call_sites=5" in ll_text,
           "expected mixed dispatch fixture to emit five direct dispatch calls")
    expect("runtime_dispatch_call_sites=1" in ll_text,
           "expected mixed dispatch fixture to emit one live runtime dispatch call")
    expect("selector_pool_gep_sites=1" in ll_text,
           "expected mixed dispatch fixture to materialize one selector thunk gep")
    expect("selector_pool_count=4" in ll_text,
           "expected mixed dispatch fixture to publish four pooled selectors")
    expect("dynamic_opt_out_sites=2" in ll_text,
           "expected mixed dispatch fixture to preserve two objc_dynamic opt-out sites")
    expect("call i32 @objc3_method_PolicyBox_class_implicitDirect()" in ll_text,
           "expected implicit direct calls to lower as exact direct LLVM calls")
    expect("call i32 @objc3_method_PolicyBox_class_explicitDirect()" in ll_text,
           "expected explicit direct calls to lower as exact direct LLVM calls")
    expect("call i32 @objc3_method_PolicyBox_class_callers()" in ll_text,
           "expected runFixture to preserve direct class-method dispatch to callers")
    expect("call i32 @objc3_runtime_dispatch_i32(" in ll_text,
           "expected dynamicEscape lowering to retain the live runtime dispatch call")
    expect("@__objc3_sec_selector_pool" in ll_text,
           "expected mixed dispatch fixture to emit the selector pool section root")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(isinstance(lowering_surface, dict),
           "expected compile manifest to publish the live lowering surface")
    expect(lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
           "expected compile manifest lowering surface to keep dispatch symbols aligned")
    expect(lowering_surface.get("message_send_sites") == 6,
           "expected compile manifest lowering surface to publish six message send sites")
    runtime_abi_surface = manifest.get("dispatch_accessor_runtime_abi_surface", {})
    expect(isinstance(runtime_abi_surface, dict),
           "expected compile manifest to publish dispatch/accessor runtime ABI surface")
    expect(runtime_abi_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
           "expected dispatch/accessor runtime ABI surface contract id in compile manifest")
    expect(runtime_abi_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected runtime ABI surface to publish canonical runtime dispatch symbol")
    expect(runtime_abi_surface.get("dispatch_state_snapshot_symbol") == "objc3_runtime_copy_dispatch_state_for_testing",
           "expected runtime ABI surface to publish dispatch state snapshot helper")
    expect(runtime_abi_surface.get("method_cache_state_snapshot_symbol") == "objc3_runtime_copy_method_cache_state_for_testing",
           "expected runtime ABI surface to publish method cache state snapshot helper")
    expect(runtime_abi_surface.get("property_registry_state_snapshot_symbol") == "objc3_runtime_copy_property_registry_state_for_testing",
           "expected runtime ABI surface to publish property registry snapshot helper")
    expect(runtime_abi_surface.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing",
           "expected runtime ABI surface to publish ARC debug snapshot helper")
    expect(runtime_abi_surface.get("bind_current_property_context_symbol") == "objc3_runtime_bind_current_property_context_for_testing",
           "expected runtime ABI surface to publish property context bind helper")
    expect(runtime_abi_surface.get("clear_current_property_context_symbol") == "objc3_runtime_clear_current_property_context_for_testing",
           "expected runtime ABI surface to publish property context clear helper")
    expect(runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime ABI surface to remain on the private testing boundary")
    expect(runtime_abi_surface.get("deterministic") is True,
           "expected runtime ABI surface to report deterministic handoff")
    storage_runtime_abi_surface = manifest.get("storage_accessor_runtime_abi_surface", {})
    expect(isinstance(storage_runtime_abi_surface, dict),
           "expected compile manifest to publish storage/accessor runtime ABI surface")
    expect(storage_runtime_abi_surface.get("contract_id") == RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
           "expected storage/accessor runtime ABI surface contract id in compile manifest")
    expect(storage_runtime_abi_surface.get("abi_boundary_model") == "private-bootstrap-internal-property-helper-and-reflection-snapshot-surface-without-public-header-widening",
           "expected storage/accessor runtime ABI surface to publish the private helper boundary model")
    expect(storage_runtime_abi_surface.get("property_registry_state_snapshot_symbol") == "objc3_runtime_copy_property_registry_state_for_testing",
           "expected storage/accessor runtime ABI surface to publish property registry snapshot helper")
    expect(storage_runtime_abi_surface.get("property_entry_snapshot_symbol") == "objc3_runtime_copy_property_entry_for_testing",
           "expected storage/accessor runtime ABI surface to publish property entry snapshot helper")
    expect(storage_runtime_abi_surface.get("current_property_read_symbol") == "objc3_runtime_read_current_property_i32",
           "expected storage/accessor runtime ABI surface to publish current-property read helper")
    expect(storage_runtime_abi_surface.get("current_property_exchange_symbol") == "objc3_runtime_exchange_current_property_i32",
           "expected storage/accessor runtime ABI surface to publish current-property exchange helper")
    expect(storage_runtime_abi_surface.get("weak_current_property_load_symbol") == "objc3_runtime_load_weak_current_property_i32",
           "expected storage/accessor runtime ABI surface to publish weak current-property load helper")
    expect(storage_runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected storage/accessor runtime ABI surface to remain private-testing only")
    expect(storage_runtime_abi_surface.get("deterministic") is True,
           "expected storage/accessor runtime ABI surface to report deterministic handoff")
    registration_runtime_abi_surface = registration_manifest.get("dispatch_accessor_runtime_abi_surface", {})
    expect(isinstance(registration_runtime_abi_surface, dict),
           "expected runtime registration manifest to publish dispatch/accessor runtime ABI surface")
    expect(registration_runtime_abi_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
           "expected dispatch/accessor runtime ABI surface contract id in runtime registration manifest")
    expect(registration_runtime_abi_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected runtime registration manifest to publish canonical runtime dispatch symbol")
    expect(registration_runtime_abi_surface.get("dispatch_state_snapshot_symbol") == "objc3_runtime_copy_dispatch_state_for_testing",
           "expected runtime registration manifest to publish dispatch state snapshot helper")
    expect(registration_runtime_abi_surface.get("current_property_read_symbol") == "objc3_runtime_read_current_property_i32",
           "expected runtime registration manifest to publish current-property read helper")
    expect(registration_runtime_abi_surface.get("current_property_exchange_symbol") == "objc3_runtime_exchange_current_property_i32",
           "expected runtime registration manifest to publish current-property exchange helper")
    expect(registration_runtime_abi_surface.get("weak_current_property_load_symbol") == "objc3_runtime_load_weak_current_property_i32",
           "expected runtime registration manifest to publish weak current-property load helper")
    expect(registration_runtime_abi_surface.get("autorelease_symbol") == "objc3_runtime_autorelease_i32",
           "expected runtime registration manifest to publish autorelease helper")
    expect(registration_runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime registration manifest ABI surface to remain private-testing only")
    expect(registration_runtime_abi_surface.get("deterministic") is True,
           "expected runtime registration manifest ABI surface to report deterministic handoff")
    registration_storage_runtime_abi_surface = registration_manifest.get("storage_accessor_runtime_abi_surface", {})
    expect(isinstance(registration_storage_runtime_abi_surface, dict),
           "expected runtime registration manifest to publish storage/accessor runtime ABI surface")
    expect(registration_storage_runtime_abi_surface.get("contract_id") == RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
           "expected storage/accessor runtime ABI surface contract id in runtime registration manifest")
    expect(registration_storage_runtime_abi_surface.get("property_registry_state_snapshot_symbol") == "objc3_runtime_copy_property_registry_state_for_testing",
           "expected runtime registration manifest to publish property registry snapshot helper")
    expect(registration_storage_runtime_abi_surface.get("current_property_write_symbol") == "objc3_runtime_write_current_property_i32",
           "expected runtime registration manifest to publish current-property write helper")
    expect(registration_storage_runtime_abi_surface.get("clear_current_property_context_symbol") == "objc3_runtime_clear_current_property_context_for_testing",
           "expected runtime registration manifest to publish property context clear helper")
    expect(registration_storage_runtime_abi_surface.get("weak_current_property_store_symbol") == "objc3_runtime_store_weak_current_property_i32",
           "expected runtime registration manifest to publish weak current-property store helper")
    expect(registration_storage_runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime registration manifest storage/accessor ABI surface to remain private-testing only")
    expect(registration_storage_runtime_abi_surface.get("deterministic") is True,
           "expected runtime registration manifest storage/accessor ABI surface to report deterministic handoff")

    return CaseResult(
        case_id="dispatch-fast-path",
        probe="tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        fixture="tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(registration_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "baseline_cache_entry_count": payload.get("baseline_cache_entry_count"),
            "baseline_fast_path_seed_count": payload.get("baseline_fast_path_seed_count"),
            "mixed_first_dispatch_path": payload.get("mixed_first_dispatch_state_last_dispatch_path"),
            "mixed_first_implementation_kind": payload.get("mixed_first_dispatch_state_last_implementation_kind"),
            "mixed_second_dispatch_path": payload.get("mixed_second_dispatch_state_last_dispatch_path"),
            "mixed_second_implementation_kind": payload.get("mixed_second_dispatch_state_last_implementation_kind"),
            "fallback_first_dispatch_path": payload.get("fallback_first_dispatch_state_last_dispatch_path"),
            "fallback_first_implementation_kind": payload.get("fallback_first_dispatch_state_last_implementation_kind"),
            "fallback_second_dispatch_path": payload.get("fallback_second_dispatch_state_last_dispatch_path"),
            "fallback_second_implementation_kind": payload.get("fallback_second_dispatch_state_last_implementation_kind"),
            "mixed_first_live_dispatch_count": payload.get("mixed_first_state_live_dispatch_count"),
            "mixed_second_live_dispatch_count": payload.get("mixed_second_state_live_dispatch_count"),
            "fallback_first_fallback_dispatch_count": payload.get("fallback_first_state_fallback_dispatch_count"),
            "fallback_second_fallback_dispatch_count": payload.get("fallback_second_state_fallback_dispatch_count"),
        },
    )


def check_property_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-reflection"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "property_metadata_reflection_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_property_metadata_reflection_probe.cpp"
    exe_path = case_dir / "runtime_property_metadata_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property reflection probe")

    widget_entry = payload.get("widget_entry", {})
    token_property = payload.get("token_property", {})
    value_property = payload.get("value_property", {})
    count_property = payload.get("count_property", {})
    missing_property = payload.get("missing_property", {})
    missing_class_property = payload.get("missing_class_property", {})
    registry_after_count = payload.get("registry_state_after_count", {})

    expect(widget_entry.get("found") == 1, "expected Widget realized class entry to be present")
    expect(token_property.get("found") == 1, "expected token property to be reflectable")
    expect(token_property.get("setter_available") == 0, "expected readonly token property to have no setter")
    expect(token_property.get("has_runtime_getter") == 1, "expected token property getter to be runtime-backed")
    expect(value_property.get("found") == 1, "expected value property to be reflectable")
    expect(value_property.get("setter_available") == 1, "expected value property to expose a setter")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property getter/setter to be runtime-backed")
    expect(count_property.get("found") == 1, "expected count property to be reflectable")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property getter/setter to be runtime-backed")
    expect(registry_after_count.get("slot_backed_property_count", 0) >= 3,
           "expected slot-backed property registry to include the three Widget properties")
    expect(missing_property.get("found") == 0, "expected missing property lookup to fail closed")
    expect(missing_class_property.get("found") == 0, "expected missing class property lookup to fail closed")

    return CaseResult(
        case_id="property-reflection",
        probe="tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "reflectable_property_count": registry_after_count.get("reflectable_property_count"),
            "slot_backed_property_count": registry_after_count.get("slot_backed_property_count"),
            "value_property_setter_available": value_property.get("setter_available"),
            "count_property_runtime_setter": count_property.get("has_runtime_setter"),
        },
    )


def check_property_execution_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-execution"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "property_ivar_execution_matrix_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "property_ivar_execution_matrix_probe.cpp"
    exe_path = case_dir / "property_ivar_execution_matrix_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property execution probe")

    widget_entry = payload.get("widget_entry", {})
    registry_state = payload.get("registry_state", {})
    count_property = payload.get("count_property", {})
    enabled_property = payload.get("enabled_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})
    count_method = payload.get("count_method", {})
    enabled_method = payload.get("enabled_method", {})
    value_method = payload.get("value_method", {})
    token_method = payload.get("token_method", {})
    set_count_dispatch = payload.get("set_count_dispatch", {})
    count_dispatch = payload.get("count_dispatch", {})
    set_enabled_dispatch = payload.get("set_enabled_dispatch", {})
    enabled_dispatch = payload.get("enabled_dispatch", {})
    set_value_dispatch = payload.get("set_value_dispatch", {})
    value_dispatch = payload.get("value_dispatch", {})
    token_dispatch = payload.get("token_dispatch", {})

    expect(payload.get("widget_instance", 0) != 0, "expected alloc to materialize a Widget instance")
    expect(payload.get("count_value") == 37, "expected synthesized count getter to return the stored value")
    expect(payload.get("enabled_value") == 1, "expected synthesized enabled getter to return the stored value")
    expect(payload.get("value_result") == 55, "expected synthesized strong property getter to return the stored value")
    expect(widget_entry.get("found") == 1, "expected Widget to be realized during property execution")
    expect(widget_entry.get("runtime_property_accessor_count", 0) >= 4,
           "expected Widget to publish runtime-backed synthesized accessors")
    expect(registry_state.get("slot_backed_property_count", 0) >= 4,
           "expected property execution fixture to register four slot-backed properties")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property to execute through runtime-backed synthesized accessors")
    expect(enabled_property.get("has_runtime_getter") == 1 and enabled_property.get("has_runtime_setter") == 1,
           "expected enabled property to execute through runtime-backed synthesized accessors")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property to execute through runtime-backed synthesized accessors")
    expect(token_property.get("has_runtime_getter") == 1 and token_property.get("setter_available") == 0,
           "expected readonly token property to expose only the synthesized getter")
    expect(count_property.get("property_name") == "count",
           "expected count property reflection to stay coherent")
    expect(count_property.get("effective_getter_selector") == "count",
           "expected count getter selector reflection to stay coherent")
    expect(count_property.get("effective_setter_selector") == "setCount:",
           "expected count setter selector reflection to stay coherent")
    expect(enabled_property.get("effective_getter_selector") == "enabled",
           "expected enabled getter selector reflection to stay coherent")
    expect(enabled_property.get("effective_setter_selector") == "setEnabled:",
           "expected enabled setter selector reflection to stay coherent")
    expect(value_property.get("effective_getter_selector") == "currentValue",
           "expected value getter selector reflection to stay coherent")
    expect(value_property.get("effective_setter_selector") == "setCurrentValue:",
           "expected value setter selector reflection to stay coherent")
    expect(token_property.get("effective_getter_selector") == "tokenValue",
           "expected token getter selector reflection to stay coherent")
    expect(count_property.get("getter_owner_identity"), "expected count getter owner identity to be published")
    expect(count_property.get("setter_owner_identity"), "expected count setter owner identity to be published")
    expect(enabled_property.get("getter_owner_identity"), "expected enabled getter owner identity to be published")
    expect(enabled_property.get("setter_owner_identity"), "expected enabled setter owner identity to be published")
    expect(value_property.get("getter_owner_identity"), "expected value getter owner identity to be published")
    expect(value_property.get("setter_owner_identity"), "expected value setter owner identity to be published")
    expect(token_property.get("getter_owner_identity"), "expected token getter owner identity to be published")
    expect(token_property.get("setter_owner_identity") is None,
           "did not expect readonly token property to publish a setter owner identity")
    expect(count_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected count property base identity to match the realized Widget class")
    expect(enabled_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected enabled property base identity to match the realized Widget class")
    expect(value_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected value property base identity to match the realized Widget class")
    expect(token_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected token property base identity to match the realized Widget class")
    expect(registry_state.get("last_resolved_class_name") == "Widget",
           "expected property registry to resolve Widget during live accessor execution")
    expect(registry_state.get("last_resolved_owner_identity"),
           "expected property registry to publish the resolved owner identity")
    expect(count_method.get("resolved") == 1 and count_method.get("parameter_count") == 0,
           "expected count getter dispatch to resolve live through the runtime cache")
    expect(enabled_method.get("resolved") == 1 and enabled_method.get("parameter_count") == 0,
           "expected enabled getter dispatch to resolve live through the runtime cache")
    expect(value_method.get("resolved") == 1 and value_method.get("parameter_count") == 0,
           "expected currentValue getter dispatch to resolve live through the runtime cache")
    expect(token_method.get("resolved") == 1 and token_method.get("parameter_count") == 0,
           "expected tokenValue getter dispatch to resolve live through the runtime cache")
    expect(count_method.get("resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter cache ownership to match reflected property ownership")
    expect(enabled_method.get("resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter cache ownership to match reflected property ownership")
    expect(value_method.get("resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter cache ownership to match reflected property ownership")
    expect(token_method.get("resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter cache ownership to match reflected property ownership")
    expect(set_count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected setCount: to execute through live synthesized accessor resolution")
    expect(set_count_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCount: to execute through the runtime property-setter builtin")
    expect(set_count_dispatch.get("last_property_name") == count_property.get("property_name"),
           "expected setCount: dispatch property name to match reflected property metadata")
    expect(set_count_dispatch.get("last_property_base_identity") == count_property.get("base_identity"),
           "expected setCount: dispatch base identity to match reflected property metadata")
    expect(set_count_dispatch.get("last_property_slot_index") == count_property.get("slot_index"),
           "expected setCount: dispatch slot index to match reflected property metadata")
    expect(set_count_dispatch.get("last_selector") == count_property.get("effective_setter_selector"),
           "expected setCount: dispatch selector to match reflected property metadata")
    expect(set_count_dispatch.get("last_resolved_owner_identity") == count_property.get("setter_owner_identity"),
           "expected setCount: dispatch ownership to match reflected property metadata")
    expect(set_count_dispatch.get("last_used_builtin") == 1 and set_count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected setCount: to remain builtin-backed and runtime-dispatched")
    expect(set_count_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCount: dispatch to report one setter parameter")
    expect(count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected count getter to execute through live synthesized accessor resolution")
    expect(count_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected count getter to execute through the runtime property-getter builtin")
    expect(count_dispatch.get("last_property_name") == count_property.get("property_name"),
           "expected count getter dispatch property name to match reflected property metadata")
    expect(count_dispatch.get("last_property_base_identity") == count_property.get("base_identity"),
           "expected count getter dispatch base identity to match reflected property metadata")
    expect(count_dispatch.get("last_property_slot_index") == count_property.get("slot_index"),
           "expected count getter dispatch slot index to match reflected property metadata")
    expect(count_dispatch.get("last_selector") == count_property.get("effective_getter_selector"),
           "expected count getter dispatch selector to match reflected property metadata")
    expect(count_dispatch.get("last_resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter dispatch ownership to match reflected property metadata")
    expect(count_dispatch.get("last_used_builtin") == 1 and count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected count getter to remain builtin-backed and runtime-dispatched")
    expect(count_dispatch.get("last_resolved_parameter_count") == 0,
           "expected count getter dispatch to report zero getter parameters")
    expect(set_enabled_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setEnabled: to execute through the runtime property-setter builtin")
    expect(set_enabled_dispatch.get("last_property_name") == enabled_property.get("property_name"),
           "expected setEnabled: dispatch property name to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_property_base_identity") == enabled_property.get("base_identity"),
           "expected setEnabled: dispatch base identity to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_property_slot_index") == enabled_property.get("slot_index"),
           "expected setEnabled: dispatch slot index to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_selector") == enabled_property.get("effective_setter_selector"),
           "expected setEnabled: dispatch selector to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_resolved_owner_identity") == enabled_property.get("setter_owner_identity"),
           "expected setEnabled: dispatch ownership to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_used_builtin") == 1 and set_enabled_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setEnabled: to remain builtin-backed and report one setter parameter")
    expect(enabled_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected enabled getter to execute through the runtime property-getter builtin")
    expect(enabled_dispatch.get("last_property_name") == enabled_property.get("property_name"),
           "expected enabled getter dispatch property name to match reflected property metadata")
    expect(enabled_dispatch.get("last_property_base_identity") == enabled_property.get("base_identity"),
           "expected enabled getter dispatch base identity to match reflected property metadata")
    expect(enabled_dispatch.get("last_property_slot_index") == enabled_property.get("slot_index"),
           "expected enabled getter dispatch slot index to match reflected property metadata")
    expect(enabled_dispatch.get("last_selector") == enabled_property.get("effective_getter_selector"),
           "expected enabled getter dispatch selector to match reflected property metadata")
    expect(enabled_dispatch.get("last_resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter dispatch ownership to match reflected property metadata")
    expect(enabled_dispatch.get("last_used_builtin") == 1 and enabled_dispatch.get("last_resolved_parameter_count") == 0,
           "expected enabled getter to remain builtin-backed and report zero getter parameters")
    expect(set_value_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCurrentValue: to execute through the runtime property-setter builtin")
    expect(set_value_dispatch.get("last_property_name") == value_property.get("property_name"),
           "expected setCurrentValue: dispatch property name to match reflected property metadata")
    expect(set_value_dispatch.get("last_property_base_identity") == value_property.get("base_identity"),
           "expected setCurrentValue: dispatch base identity to match reflected property metadata")
    expect(set_value_dispatch.get("last_property_slot_index") == value_property.get("slot_index"),
           "expected setCurrentValue: dispatch slot index to match reflected property metadata")
    expect(set_value_dispatch.get("last_selector") == value_property.get("effective_setter_selector"),
           "expected setCurrentValue: dispatch selector to match reflected property metadata")
    expect(set_value_dispatch.get("last_resolved_owner_identity") == value_property.get("setter_owner_identity"),
           "expected setCurrentValue: dispatch ownership to match reflected property metadata")
    expect(set_value_dispatch.get("last_used_builtin") == 1 and set_value_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCurrentValue: to remain builtin-backed and report one setter parameter")
    expect(value_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected currentValue getter to execute through the runtime property-getter builtin")
    expect(value_dispatch.get("last_property_name") == value_property.get("property_name"),
           "expected currentValue getter dispatch property name to match reflected property metadata")
    expect(value_dispatch.get("last_property_base_identity") == value_property.get("base_identity"),
           "expected currentValue getter dispatch base identity to match reflected property metadata")
    expect(value_dispatch.get("last_property_slot_index") == value_property.get("slot_index"),
           "expected currentValue getter dispatch slot index to match reflected property metadata")
    expect(value_dispatch.get("last_selector") == value_property.get("effective_getter_selector"),
           "expected currentValue getter dispatch selector to match reflected property metadata")
    expect(value_dispatch.get("last_resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter dispatch ownership to match reflected property metadata")
    expect(value_dispatch.get("last_used_builtin") == 1 and value_dispatch.get("last_resolved_parameter_count") == 0,
           "expected currentValue getter to remain builtin-backed and report zero getter parameters")
    expect(token_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected tokenValue getter to execute through the runtime property-getter builtin")
    expect(token_dispatch.get("last_property_name") == token_property.get("property_name"),
           "expected tokenValue getter dispatch property name to match reflected property metadata")
    expect(token_dispatch.get("last_property_base_identity") == token_property.get("base_identity"),
           "expected tokenValue getter dispatch base identity to match reflected property metadata")
    expect(token_dispatch.get("last_property_slot_index") == token_property.get("slot_index"),
           "expected tokenValue getter dispatch slot index to match reflected property metadata")
    expect(token_dispatch.get("last_selector") == token_property.get("effective_getter_selector"),
           "expected tokenValue getter dispatch selector to match reflected property metadata")
    expect(token_dispatch.get("last_resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter dispatch ownership to match reflected property metadata")
    expect(token_dispatch.get("last_used_builtin") == 1 and token_dispatch.get("last_resolved_parameter_count") == 0,
           "expected tokenValue getter to remain builtin-backed and report zero getter parameters")
    return CaseResult(
        case_id="property-execution",
        probe="tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        fixture="tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "count_value": payload.get("count_value"),
            "enabled_value": payload.get("enabled_value"),
            "value_result": payload.get("value_result"),
            "runtime_property_accessor_count": widget_entry.get("runtime_property_accessor_count"),
            "slot_backed_property_count": registry_state.get("slot_backed_property_count"),
            "count_dispatch_kind": count_dispatch.get("last_implementation_kind"),
            "value_dispatch_kind": value_dispatch.get("last_implementation_kind"),
            "token_dispatch_kind": token_dispatch.get("last_implementation_kind"),
        },
    )


def check_escaping_block_capture_legality_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "escaping-block-capture-legality"

    argument_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_argument_positive.objc3"
    )
    _, _, argument_manifest_path = compile_fixture_outputs(
        argument_fixture, case_dir / "argument-positive"
    )
    argument_manifest = json.loads(argument_manifest_path.read_text(encoding="utf-8"))
    argument_escape_surface = (
        argument_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    argument_copy_dispose_surface = (
        argument_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    return_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_return_positive.objc3"
    )
    _, _, return_manifest_path = compile_fixture_outputs(
        return_fixture, case_dir / "return-positive"
    )
    return_manifest = json.loads(return_manifest_path.read_text(encoding="utf-8"))
    return_escape_surface = (
        return_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    return_copy_dispose_surface = (
        return_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    bad_call_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "capture_legality_escape_invocation_bad_call.objc3",
        case_dir / "bad-call-negative",
        expected_snippets=[
            "type mismatch: expected 'i32' argument for parameter 0 of callable 'closure', got 'bool'"
        ],
        expected_codes=["O3S206"],
    )
    missing_capture_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "capture_legality_escape_invocation_missing_capture.objc3",
        case_dir / "missing-capture-negative",
        expected_snippets=["undefined capture 'seed' in block literal"],
        expected_codes=["O3S202"],
    )
    byref_escape_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_byref_positive.objc3"
    )
    _, _, byref_escape_manifest_path = compile_fixture_outputs(
        byref_escape_fixture, case_dir / "byref-escape-positive"
    )
    byref_escape_manifest = json.loads(byref_escape_manifest_path.read_text(encoding="utf-8"))
    byref_escape_surface = (
        byref_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    byref_copy_dispose_surface = (
        byref_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    owned_escape_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_owned_capture_positive.objc3"
    )
    _, _, owned_escape_manifest_path = compile_fixture_outputs(
        owned_escape_fixture, case_dir / "owned-escape-positive"
    )
    owned_escape_manifest = json.loads(
        owned_escape_manifest_path.read_text(encoding="utf-8")
    )
    owned_escape_surface = (
        owned_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    owned_escape_copy_dispose_surface = (
        owned_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    expect(
        argument_escape_surface.get("escape_to_heap_sites") == 1
        and argument_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping argument fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        argument_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and argument_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected escaping argument fixture to keep copy/dispose helpers elided",
    )
    expect(
        return_escape_surface.get("escape_to_heap_sites") == 1
        and return_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping return fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        return_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and return_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected escaping return fixture to keep copy/dispose helpers elided",
    )
    expect(
        byref_escape_surface.get("escape_to_heap_sites") == 1
        and byref_escape_surface.get("requires_byref_cells_sites") == 1,
        "expected escaping byref fixture to publish one heap-promotion candidate with one byref-cell site",
    )
    expect(
        byref_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and byref_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected escaping byref fixture to require copy/dispose helpers",
    )
    expect(
        owned_escape_surface.get("escape_to_heap_sites") == 1
        and owned_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping owned-capture fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        owned_escape_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_escape_copy_dispose_surface.get("dispose_helper_required_sites")
        == 1,
        "expected escaping owned-capture fixture to require copy/dispose helpers",
    )

    return CaseResult(
        case_id="escaping-block-capture-legality",
        probe="compile-manifest-diagnostics-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "argument_escape_to_heap_sites": argument_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "return_escape_to_heap_sites": return_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "bad_call_diagnostic_count": bad_call_negative["diagnostic_count"],
            "missing_capture_diagnostic_count": missing_capture_negative[
                "diagnostic_count"
            ],
            "byref_escape_to_heap_sites": byref_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "byref_copy_helper_required_sites": byref_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "owned_escape_to_heap_sites": owned_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "owned_copy_helper_required_sites": (
                owned_escape_copy_dispose_surface.get("copy_helper_required_sites")
            ),
        },
    )


def check_block_storage_arc_automation_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "block-storage-arc-automation-semantics"

    owned_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "owned_object_capture_helper_positive.objc3"
    )
    _, owned_ll_path, owned_manifest_path = compile_fixture_outputs(
        owned_fixture, case_dir / "owned-positive"
    )
    owned_manifest = json.loads(owned_manifest_path.read_text(encoding="utf-8"))
    owned_semantic_surface = (
        owned_manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    )
    owned_copy_dispose_surface = owned_semantic_surface.get(
        "objc_block_copy_dispose_lowering_surface", {}
    )
    owned_escape_surface = owned_semantic_surface.get(
        "objc_block_storage_escape_lowering_surface", {}
    )
    owned_ll = owned_ll_path.read_text(encoding="utf-8")

    nonowning_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "nonowning_object_capture_helper_elided_positive.objc3"
    )
    _, nonowning_ll_path, nonowning_manifest_path = compile_fixture_outputs(
        nonowning_fixture, case_dir / "nonowning-positive"
    )
    nonowning_manifest = json.loads(
        nonowning_manifest_path.read_text(encoding="utf-8")
    )
    nonowning_semantic_surface = (
        nonowning_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
    )
    nonowning_copy_dispose_surface = nonowning_semantic_surface.get(
        "objc_block_copy_dispose_lowering_surface", {}
    )
    nonowning_arc_diagnostics_surface = nonowning_semantic_surface.get(
        "objc_arc_diagnostics_fixit_lowering_surface", {}
    )
    nonowning_ll = nonowning_ll_path.read_text(encoding="utf-8")

    arc_mode_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_mode_handling_positive.objc3"
    )
    compile_fixture_with_args(
        arc_mode_fixture, case_dir / "arc-mode-positive", extra_args=["-fobjc-arc"]
    )
    arc_mode_ll_path = case_dir / "arc-mode-positive" / "module.ll"
    arc_mode_manifest_path = case_dir / "arc-mode-positive" / "module.manifest.json"
    arc_mode_manifest = json.loads(arc_mode_manifest_path.read_text(encoding="utf-8"))
    arc_mode_ll = arc_mode_ll_path.read_text(encoding="utf-8")
    arc_mode_sema = arc_mode_manifest.get("frontend", {}).get("pipeline", {}).get(
        "sema_pass_manager", {}
    )
    arc_mode_block_copy_dispose_surface = (
        arc_mode_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    arc_inference_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_inference_lifetime_positive.objc3"
    )
    compile_fixture_with_args(
        arc_inference_fixture,
        case_dir / "arc-inference-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_inference_ll_path = case_dir / "arc-inference-positive" / "module.ll"
    arc_inference_manifest_path = (
        case_dir / "arc-inference-positive" / "module.manifest.json"
    )
    arc_inference_manifest = json.loads(
        arc_inference_manifest_path.read_text(encoding="utf-8")
    )
    arc_inference_ll = arc_inference_ll_path.read_text(encoding="utf-8")
    arc_inference_sema = arc_inference_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})

    arc_cleanup_scope_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_cleanup_scope_positive.objc3"
    )
    compile_fixture_with_args(
        arc_cleanup_scope_fixture,
        case_dir / "arc-cleanup-scope-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_cleanup_scope_manifest_path = (
        case_dir / "arc-cleanup-scope-positive" / "module.manifest.json"
    )
    arc_cleanup_scope_manifest = json.loads(
        arc_cleanup_scope_manifest_path.read_text(encoding="utf-8")
    )
    arc_cleanup_scope_sema = arc_cleanup_scope_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})

    arc_implicit_cleanup_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_implicit_cleanup_void_positive.objc3"
    )
    compile_fixture_with_args(
        arc_implicit_cleanup_fixture,
        case_dir / "arc-implicit-cleanup-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_implicit_cleanup_manifest_path = (
        case_dir / "arc-implicit-cleanup-positive" / "module.manifest.json"
    )
    arc_implicit_cleanup_manifest = json.loads(
        arc_implicit_cleanup_manifest_path.read_text(encoding="utf-8")
    )
    arc_implicit_cleanup_sema = arc_implicit_cleanup_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})

    arc_autorelease_return_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_autorelease_return_positive.objc3"
    )
    compile_fixture_with_args(
        arc_autorelease_return_fixture,
        case_dir / "arc-autorelease-return-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_autorelease_return_ll_path = (
        case_dir / "arc-autorelease-return-positive" / "module.ll"
    )
    arc_autorelease_return_manifest_path = (
        case_dir / "arc-autorelease-return-positive" / "module.manifest.json"
    )
    arc_autorelease_return_manifest = json.loads(
        arc_autorelease_return_manifest_path.read_text(encoding="utf-8")
    )
    arc_autorelease_return_ll = arc_autorelease_return_ll_path.read_text(
        encoding="utf-8"
    )
    arc_autorelease_return_sema = arc_autorelease_return_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})

    weak_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "weak_object_capture_mutation_negative.objc3",
        case_dir / "weak-mutation-negative",
        expected_snippets=[
            "type mismatch: block mutated capture 'weakValue' requires owned runtime-backed storage"
        ],
        expected_codes=["O3S206"],
    )
    unowned_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "unowned_object_capture_mutation_negative.objc3",
        case_dir / "unowned-mutation-negative",
        expected_snippets=[
            "type mismatch: block mutated capture 'borrowedValue' requires owned runtime-backed storage"
        ],
        expected_codes=["O3S206"],
    )

    expect(
        owned_manifest.get("runtime_block_arc_unified_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        "expected owned-capture helper fixture to publish the block/ARC unified source surface",
    )
    expect(
        owned_manifest.get(
            "runtime_ownership_transfer_capture_family_source_surface", {}
        ).get("contract_id")
        == RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
        "expected owned-capture helper fixture to publish the ownership-transfer/capture-family source surface",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected owned object capture fixture to require copy/dispose helpers",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_symbolized_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_symbolized_sites") == 1,
        "expected owned object capture fixture to symbolize copy/dispose helpers",
    )
    expect(
        owned_escape_surface.get("escape_to_heap_sites") == 1
        and owned_escape_surface.get("escape_analysis_enabled_sites") == 1,
        "expected owned object capture fixture to publish one escaping block path",
    )
    expect(
        "; runtime_block_allocation_copy_dispose_invoke_support = "
        "contract=objc3c.runtime.block.allocation.copy.dispose.invoke.support.v1"
        in owned_ll,
        "expected owned object capture fixture LLVM IR to publish the block allocation/copy/dispose/invoke support surface",
    )

    expect(
        nonowning_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected non-owning object capture fixture to elide copy/dispose helpers",
    )
    expect(
        nonowning_copy_dispose_surface.get("copy_helper_symbolized_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_symbolized_sites") == 0,
        "expected non-owning object capture fixture to publish zero helper symbols",
    )
    expect(
        nonowning_arc_diagnostics_surface.get(
            "ownership_arc_diagnostic_candidate_sites"
        )
        == 1
        and nonowning_arc_diagnostics_surface.get("ownership_arc_fixit_available_sites")
        == 1
        and nonowning_arc_diagnostics_surface.get("ownership_arc_profiled_sites")
        == 1,
        "expected non-owning object capture fixture to publish one ARC ownership diagnostic/fixit candidate",
    )
    expect(
        "; block_copy_dispose_lowering = " in nonowning_ll,
        "expected non-owning object capture fixture LLVM IR to publish the block copy/dispose lowering summary",
    )

    expect(
        arc_mode_sema.get("retain_release_operation_lowering_retain_insertion_sites")
        == 8
        and arc_mode_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected arc mode handling fixture to publish eight retain and eight release insertions",
    )
    expect(
        arc_mode_block_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and arc_mode_block_copy_dispose_surface.get("dispose_helper_required_sites")
        == 1,
        "expected arc mode handling fixture to keep block copy/dispose helper lowering enabled",
    )
    expect(
        "; arc_cleanup_weak_lifetime_hooks = "
        "contract=objc3c.arc.cleanup.weak.lifetime.hooks.v1" in arc_mode_ll,
        "expected arc mode handling fixture LLVM IR to publish the ARC cleanup/weak lifetime hooks surface",
    )

    expect(
        arc_inference_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected arc inference fixture to publish canonical retain/release insertion counts without autorelease insertion",
    )
    expect(
        arc_inference_sema.get(
            "weak_unowned_semantics_lowering_ownership_candidate_sites"
        )
        == 8
        and arc_inference_sema.get(
            "weak_unowned_semantics_lowering_weak_reference_sites"
        )
        == 0,
        "expected arc inference fixture to normalize eight ownership-qualified candidates without weak-reference lowering",
    )
    expect(
        "; arc_cleanup_weak_lifetime_hooks = "
        "contract=objc3c.arc.cleanup.weak.lifetime.hooks.v1" in arc_inference_ll,
        "expected arc inference fixture LLVM IR to publish the ARC cleanup/weak lifetime hooks surface",
    )
    expect(
        arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected ARC cleanup scope fixture to publish one retain/release transfer pair without autorelease insertion",
    )
    expect(
        arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected ARC implicit cleanup fixture to publish one retain/release cleanup pair without autorelease insertion",
    )
    expect(
        arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 0
        and arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 0
        and arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 2,
        "expected ARC autorelease-return fixture to publish two autorelease insertions without retain/release insertion",
    )
    expect(
        "; arc_block_autorelease_return_lowering = " in arc_autorelease_return_ll,
        "expected ARC autorelease-return fixture LLVM IR to publish the ARC block/autorelease-return lowering summary",
    )

    return CaseResult(
        case_id="block-storage-arc-automation-semantics",
        probe="compile-manifest-diagnostics-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "owned_copy_helper_required_sites": owned_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "nonowning_copy_helper_required_sites": nonowning_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "arc_mode_retain_insertions": arc_mode_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_inference_retain_insertions": arc_inference_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_cleanup_scope_release_insertions": arc_cleanup_scope_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_implicit_cleanup_release_insertions": arc_implicit_cleanup_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_autorelease_return_autorelease_insertions": arc_autorelease_return_sema.get(
                "retain_release_operation_lowering_autorelease_insertion_sites"
            ),
            "weak_negative_diagnostic_count": weak_negative["diagnostic_count"],
            "unowned_negative_diagnostic_count": unowned_negative["diagnostic_count"],
        },
    )

def check_block_arc_runtime_abi_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "block-arc-runtime-abi"
    probe = ROOT / Path(BLOCK_ARC_RUNTIME_ABI_PROBE)
    exe_path = case_dir / "block_arc_runtime_abi_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(run_probe(exe_path), "block ARC runtime ABI probe")

    expected_string_fields = {
        "block_promote_symbol": "objc3_runtime_promote_block_i32",
        "block_invoke_symbol": "objc3_runtime_invoke_block_i32",
        "retain_symbol": "objc3_runtime_retain_i32",
        "release_symbol": "objc3_runtime_release_i32",
        "autorelease_symbol": "objc3_runtime_autorelease_i32",
        "autoreleasepool_push_symbol": "objc3_runtime_push_autoreleasepool_scope",
        "autoreleasepool_pop_symbol": "objc3_runtime_pop_autoreleasepool_scope",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "arc_debug_state_snapshot_symbol": (
            "objc3_runtime_copy_arc_debug_state_for_testing"
        ),
        "runtime_abi_boundary_model": BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
        "block_runtime_model": BLOCK_ARC_RUNTIME_BLOCK_MODEL,
        "arc_runtime_model": BLOCK_ARC_RUNTIME_ARC_MODEL,
        "fail_closed_model": BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
    }
    for field, expected_value in expected_string_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected block ARC runtime ABI probe to preserve {field}",
        )

    expected_integer_fields = {
        "abi_status": 0,
        "arc_status": 0,
        "retained": 77,
        "autoreleased": 77,
        "released": 77,
        "invoke_result": 17,
        "private_runtime_abi_ready": 1,
        "public_runtime_header_unchanged": 1,
        "deterministic": 1,
        "live_runtime_block_handle_count": 0,
        "block_promote_call_count": 1,
        "block_invoke_call_count": 1,
        "retain_call_count": 2,
        "release_call_count": 3,
        "autorelease_call_count": 1,
        "autoreleasepool_push_count": 1,
        "autoreleasepool_pop_count": 1,
        "current_property_read_count": 0,
        "current_property_write_count": 0,
        "current_property_exchange_count": 0,
        "weak_current_property_load_count": 0,
        "weak_current_property_store_count": 0,
        "last_promote_has_pointer_capture_storage": 1,
        "last_block_invoke_result": 17,
        "last_autorelease_value": 77,
        "arc_retain_call_count": 2,
        "arc_release_call_count": 3,
        "arc_autorelease_call_count": 1,
        "arc_autoreleasepool_push_count": 1,
        "arc_autoreleasepool_pop_count": 1,
    }
    for field, expected_value in expected_integer_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected block ARC runtime ABI probe to preserve {field}",
        )

    handle = payload.get("handle")
    expect(isinstance(handle, int) and handle > 0, "expected block ARC runtime ABI probe to publish a live promoted block handle")
    expect(
        payload.get("retain_handle_result") == handle
        and payload.get("release_handle_result") == handle
        and payload.get("final_release_result") == handle,
        "expected block ARC runtime ABI probe to preserve block handle retain/release traffic",
    )
    expect(
        payload.get("last_promoted_block_handle") == handle
        and payload.get("last_invoked_block_handle") == handle,
        "expected block ARC runtime ABI probe to preserve the last promoted and invoked block handle",
    )
    expect(
        payload.get("last_retain_value") == handle
        and payload.get("last_release_value") == handle,
        "expected block ARC runtime ABI probe to preserve the last ARC retain/release value",
    )

    return CaseResult(
        case_id="block-arc-runtime-abi",
        probe=BLOCK_ARC_RUNTIME_ABI_PROBE,
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "handle": handle,
            "invoke_result": payload.get("invoke_result"),
            "block_promote_call_count": payload.get("block_promote_call_count"),
            "block_invoke_call_count": payload.get("block_invoke_call_count"),
            "retain_call_count": payload.get("retain_call_count"),
            "release_call_count": payload.get("release_call_count"),
            "autorelease_call_count": payload.get("autorelease_call_count"),
        },
    )


def check_block_helper_runtime_execution_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "block-helper-runtime-execution"

    byref_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "byref_cell_copy_dispose_runtime_positive.objc3"
    )
    byref_obj, _, byref_manifest_path = compile_fixture_outputs(
        byref_fixture, case_dir / "byref-runtime-positive"
    )
    byref_manifest = json.loads(byref_manifest_path.read_text(encoding="utf-8"))
    byref_copy_dispose_surface = (
        byref_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    byref_exe = case_dir / "byref-runtime-positive" / "byref_runtime.exe"
    link_fixture_executable(clangxx, byref_obj, byref_exe)
    byref_run = run([str(byref_exe)])
    expect(
        byref_run.returncode == 14,
        f"expected byref runtime positive fixture to exit 14, saw {byref_run.returncode}",
    )
    expect(
        byref_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and byref_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected byref runtime positive fixture to require copy/dispose helpers",
    )
    byref_forwarding_probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "block_runtime_byref_forwarding_probe.cpp"
    )
    byref_forwarding_exe = (
        case_dir / "block_runtime_byref_forwarding_probe.exe"
    )
    compile_probe(clangxx, byref_forwarding_probe, byref_forwarding_exe, [])
    byref_forwarding_payload = parse_json_output(
        run_probe(byref_forwarding_exe),
        "block runtime byref forwarding probe",
    )
    expect(
        isinstance(byref_forwarding_payload.get("handle"), int)
        and byref_forwarding_payload.get("handle", 0) > 0,
        "expected byref forwarding probe to publish a positive runtime block handle",
    )
    expect(
        byref_forwarding_payload.get("copy_count_after_promotion") == 1,
        "expected byref forwarding probe to execute one copy helper during promotion",
    )
    expect(
        byref_forwarding_payload.get("first_invoke_result") == 23
        and byref_forwarding_payload.get("second_invoke_result") == 25,
        "expected byref forwarding probe to preserve runtime-owned forwarded cell state across invokes",
    )
    expect(
        byref_forwarding_payload.get("dispose_count_before_final_release") == 0
        and byref_forwarding_payload.get("dispose_count_after_final_release") == 1,
        "expected byref forwarding probe to defer dispose helper execution until final release",
    )
    expect(
        byref_forwarding_payload.get("last_disposed_value") == 11,
        "expected byref forwarding probe to dispose the original owned capture payload",
    )
    expect(
        byref_forwarding_payload.get("final_release_result")
        == byref_forwarding_payload.get("handle")
        and byref_forwarding_payload.get("invoke_after_release_result") == 0,
        "expected byref forwarding probe to release the block handle and reject post-release invocation",
    )

    owned_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "owned_object_capture_runtime_positive.objc3"
    )
    owned_obj, _, owned_manifest_path = compile_fixture_outputs(
        owned_fixture, case_dir / "owned-runtime-positive"
    )
    owned_manifest = json.loads(owned_manifest_path.read_text(encoding="utf-8"))
    owned_copy_dispose_surface = (
        owned_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    owned_exe = case_dir / "owned-runtime-positive" / "owned_runtime.exe"
    link_fixture_executable(clangxx, owned_obj, owned_exe)
    owned_run = run([str(owned_exe)])
    expect(
        owned_run.returncode == 11,
        f"expected owned object capture runtime positive fixture to exit 11, saw {owned_run.returncode}",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected owned runtime positive fixture to require copy/dispose helpers",
    )

    nonowning_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "nonowning_object_capture_runtime_positive.objc3"
    )
    nonowning_obj, _, nonowning_manifest_path = compile_fixture_outputs(
        nonowning_fixture, case_dir / "nonowning-runtime-positive"
    )
    nonowning_manifest = json.loads(
        nonowning_manifest_path.read_text(encoding="utf-8")
    )
    nonowning_copy_dispose_surface = (
        nonowning_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    nonowning_exe = (
        case_dir / "nonowning-runtime-positive" / "nonowning_runtime.exe"
    )
    link_fixture_executable(clangxx, nonowning_obj, nonowning_exe)
    nonowning_run = run([str(nonowning_exe)])
    expect(
        nonowning_run.returncode == 9,
        f"expected non-owning object capture runtime positive fixture to exit 9, saw {nonowning_run.returncode}",
    )
    expect(
        nonowning_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected non-owning runtime positive fixture to elide copy/dispose helpers",
    )

    arc_mode_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_mode_handling_positive.objc3"
    )
    arc_mode_obj, _, arc_mode_manifest_path = compile_fixture_outputs_with_args(
        arc_mode_fixture,
        case_dir / "arc-mode-runtime-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_mode_manifest = json.loads(arc_mode_manifest_path.read_text(encoding="utf-8"))
    arc_mode_sema = arc_mode_manifest.get("frontend", {}).get("pipeline", {}).get(
        "sema_pass_manager", {}
    )
    arc_mode_exe = case_dir / "arc-mode-runtime-positive" / "arc_mode.exe"
    link_fixture_executable(clangxx, arc_mode_obj, arc_mode_exe)
    arc_mode_run = run([str(arc_mode_exe)])
    expect(
        arc_mode_run.returncode == 17,
        f"expected ARC mode runtime positive fixture to exit 17, saw {arc_mode_run.returncode}",
    )
    expect(
        arc_mode_sema.get("retain_release_operation_lowering_retain_insertion_sites")
        == 8
        and arc_mode_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected ARC mode runtime positive fixture to preserve eight retain and eight release insertions",
    )

    arc_inference_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_inference_lifetime_positive.objc3"
    )
    arc_inference_obj, _, arc_inference_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_inference_fixture,
            case_dir / "arc-inference-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_inference_manifest = json.loads(
        arc_inference_manifest_path.read_text(encoding="utf-8")
    )
    arc_inference_sema = arc_inference_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})
    arc_inference_exe = (
        case_dir / "arc-inference-runtime-positive" / "arc_inference.exe"
    )
    link_fixture_executable(clangxx, arc_inference_obj, arc_inference_exe)
    arc_inference_run = run([str(arc_inference_exe)])
    expect(
        arc_inference_run.returncode == 17,
        f"expected ARC inference runtime positive fixture to exit 17, saw {arc_inference_run.returncode}",
    )
    expect(
        arc_inference_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected ARC inference runtime positive fixture to preserve eight retain and eight release insertions",
    )

    arc_cleanup_scope_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_cleanup_scope_positive.objc3"
    )
    arc_cleanup_scope_obj, _, arc_cleanup_scope_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_cleanup_scope_fixture,
            case_dir / "arc-cleanup-scope-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_cleanup_scope_manifest = json.loads(
        arc_cleanup_scope_manifest_path.read_text(encoding="utf-8")
    )
    arc_cleanup_scope_sema = arc_cleanup_scope_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})
    arc_cleanup_scope_exe = (
        case_dir / "arc-cleanup-scope-runtime-positive" / "arc_cleanup_scope.exe"
    )
    link_fixture_executable(clangxx, arc_cleanup_scope_obj, arc_cleanup_scope_exe)
    arc_cleanup_scope_run = run([str(arc_cleanup_scope_exe)])
    expect(
        arc_cleanup_scope_run.returncode == 9,
        f"expected ARC cleanup scope runtime positive fixture to exit 9, saw {arc_cleanup_scope_run.returncode}",
    )
    expect(
        arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1,
        "expected ARC cleanup scope runtime positive fixture to preserve one retain/release cleanup pair",
    )

    arc_implicit_cleanup_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_implicit_cleanup_void_positive.objc3"
    )
    arc_implicit_cleanup_obj, _, arc_implicit_cleanup_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_implicit_cleanup_fixture,
            case_dir / "arc-implicit-cleanup-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_implicit_cleanup_manifest = json.loads(
        arc_implicit_cleanup_manifest_path.read_text(encoding="utf-8")
    )
    arc_implicit_cleanup_sema = arc_implicit_cleanup_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})
    arc_implicit_cleanup_exe = (
        case_dir
        / "arc-implicit-cleanup-runtime-positive"
        / "arc_implicit_cleanup.exe"
    )
    link_fixture_executable(
        clangxx, arc_implicit_cleanup_obj, arc_implicit_cleanup_exe
    )
    arc_implicit_cleanup_run = run([str(arc_implicit_cleanup_exe)])
    expect(
        arc_implicit_cleanup_run.returncode == 0,
        f"expected ARC implicit cleanup runtime positive fixture to exit 0, saw {arc_implicit_cleanup_run.returncode}",
    )
    expect(
        arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1,
        "expected ARC implicit cleanup runtime positive fixture to preserve one retain/release cleanup pair",
    )

    return CaseResult(
        case_id="block-helper-runtime-execution",
        probe="linked-fixture-main",
        fixture="tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "byref_runtime_exit_code": byref_run.returncode,
            "owned_runtime_exit_code": owned_run.returncode,
            "nonowning_runtime_exit_code": nonowning_run.returncode,
            "arc_mode_runtime_exit_code": arc_mode_run.returncode,
            "arc_inference_runtime_exit_code": arc_inference_run.returncode,
            "arc_cleanup_scope_runtime_exit_code": arc_cleanup_scope_run.returncode,
            "arc_implicit_cleanup_runtime_exit_code": arc_implicit_cleanup_run.returncode,
            "byref_forwarding_probe_handle": byref_forwarding_payload.get("handle"),
            "byref_forwarding_first_invoke_result": byref_forwarding_payload.get(
                "first_invoke_result"
            ),
            "byref_forwarding_second_invoke_result": byref_forwarding_payload.get(
                "second_invoke_result"
            ),
            "byref_copy_helper_required_sites": byref_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "owned_copy_helper_required_sites": owned_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "nonowning_copy_helper_required_sites": nonowning_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "arc_mode_retain_insertions": arc_mode_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_inference_retain_insertions": arc_inference_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_cleanup_scope_release_insertions": arc_cleanup_scope_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_implicit_cleanup_release_insertions": arc_implicit_cleanup_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
        },
    )


def check_arc_property_helper_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "arc-property-helper-abi"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_property_interaction_positive.objc3"
    )
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "arc_debug_instrumentation_probe.cpp"
    exe_path = case_dir / "arc_debug_instrumentation_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "arc property helper probe")

    inside = payload.get("inside", {})
    after = payload.get("after", {})

    expect(payload.get("parent", 0) != 0 and payload.get("child", 0) != 0,
           "expected ArcBox runtime helper probe to allocate live receivers")
    expect(payload.get("bind_current_status") == 0,
           "expected strong-property current context binding to succeed")
    expect(payload.get("bind_weak_status") == 0,
           "expected weak-property current context binding to succeed")
    expect(payload.get("rebind_current_status") == 0 and payload.get("rebind_weak_status") == 0,
           "expected current-property helper rebinds to succeed")
    expect(payload.get("getter_value") == payload.get("child"),
           "expected current-property getter helper to read the stored child value")
    expect(payload.get("release_local_result") == payload.get("child"),
           "expected releasing the retained local to return the child handle")
    expect(payload.get("retained") == 9,
           "expected the retain helper to preserve the canonical retained payload")
    expect(payload.get("autoreleased") == 9,
           "expected the autorelease helper to preserve the canonical autoreleased payload")
    expect(payload.get("weak_set_result") == payload.get("child"),
           "expected weak-property helper write to preserve the child value")
    expect(payload.get("weak_inside_pool") == payload.get("child"),
           "expected weak-property helper read inside the pool to preserve the child value")
    expect(payload.get("weak_after_pool") == payload.get("child"),
           "expected weak-property helper read after pool pop to stay coherent")
    expect(payload.get("released") == 9,
           "expected release helper accounting to preserve the released payload")
    expect(payload.get("parent_release_result") == payload.get("parent"),
           "expected releasing the parent to return the parent handle")
    expect(payload.get("strong_set_result") == 0,
           "expected first strong-property exchange to replace an empty slot")
    expect(payload.get("clear_strong_result") == payload.get("child"),
           "expected clearing the strong property to return the previous child value")
    expect(inside.get("retain_call_count") == 1,
           "expected one retain helper call before the autoreleasepool drains")
    expect(inside.get("release_call_count") == 1,
           "expected one release helper call before the autoreleasepool drains")
    expect(inside.get("autorelease_call_count") == 1,
           "expected one autorelease helper call before the autoreleasepool drains")
    expect(inside.get("autoreleasepool_push_count") == 1,
           "expected one autoreleasepool push before the autoreleasepool drains")
    expect(inside.get("autoreleasepool_pop_count") == 0,
           "expected no autoreleasepool pop before the autoreleasepool drains")
    expect(inside.get("current_property_read_count") == 2,
           "expected live current-property reads to execute through the runtime helper ABI")
    expect(inside.get("current_property_write_count") == 1,
           "expected live current-property writes to execute through the runtime helper ABI")
    expect(inside.get("current_property_exchange_count") == 2,
           "expected strong ownership accessors to execute through exchange helper traffic")
    expect(inside.get("weak_current_property_load_count") == 1,
           "expected weak-property loads to execute through the runtime helper ABI")
    expect(inside.get("weak_current_property_store_count") == 1,
           "expected weak-property stores to execute through the runtime helper ABI")
    expect(inside.get("last_retain_value") == 9,
           "expected helper ABI debug state to preserve the retained payload")
    expect(inside.get("last_release_value") == payload.get("child"),
           "expected helper ABI debug state to preserve the pre-pool child release")
    expect(inside.get("last_autorelease_value") == 9,
           "expected helper ABI debug state to preserve the autoreleased payload")
    expect(inside.get("last_property_exchange_previous_value") == payload.get("child"),
           "expected helper ABI debug state to preserve the exchanged child handle")
    expect(inside.get("last_property_exchange_new_value") == 0,
           "expected helper ABI debug state to preserve the cleared strong slot")
    expect(inside.get("last_property_receiver") == payload.get("parent"),
           "expected helper ABI debug state to preserve the bound receiver")
    expect(inside.get("last_property_name") == "weakValue",
           "expected helper ABI debug state to report the bound weak property")
    expect(inside.get("last_property_owner_identity") == "implementation:ArcBox",
           "expected helper ABI debug state to report the ArcBox owner identity")
    expect(after.get("retain_call_count") == 1,
           "expected retain helper accounting to remain stable after the autoreleasepool drains")
    expect(after.get("release_call_count") == 3,
           "expected helper ABI probe to release the child, retained value, and parent exactly once each")
    expect(after.get("autorelease_call_count") == 1,
           "expected autorelease helper accounting to remain stable after the autoreleasepool drains")
    expect(after.get("autoreleasepool_push_count") == 1,
           "expected helper ABI probe to preserve a single autoreleasepool push")
    expect(after.get("autoreleasepool_pop_count") == 1,
           "expected helper ABI probe to pop one autorelease pool")
    expect(after.get("current_property_read_count") == 3,
           "expected post-pool helper accounting to include the final weak-property read")
    expect(after.get("current_property_write_count") == 1,
           "expected post-pool helper accounting to preserve one current-property write")
    expect(after.get("current_property_exchange_count") == 2,
           "expected post-pool helper accounting to preserve two strong-property exchanges")
    expect(after.get("weak_current_property_load_count") == 2,
           "expected post-pool helper accounting to preserve two weak-property loads")
    expect(after.get("weak_current_property_store_count") == 1,
           "expected post-pool helper accounting to preserve one weak-property store")
    expect(after.get("last_release_value") == payload.get("parent"),
           "expected helper ABI debug state to report the final parent release after pool drain")
    expect(after.get("last_property_name") == "weakValue",
           "expected post-pool helper ABI debug state to preserve the bound weak property")
    expect(after.get("last_property_owner_identity") == "implementation:ArcBox",
           "expected post-pool helper ABI debug state to preserve the ArcBox owner identity")

    return CaseResult(
        case_id="arc-property-helper-abi",
        probe="tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        fixture="tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "parent": payload.get("parent"),
            "child": payload.get("child"),
            "release_local_result": payload.get("release_local_result"),
            "retained": payload.get("retained"),
            "autoreleased": payload.get("autoreleased"),
            "released": payload.get("released"),
            "getter_value": payload.get("getter_value"),
            "weak_after_pool": payload.get("weak_after_pool"),
            "inside_retain_call_count": inside.get("retain_call_count"),
            "inside_current_property_exchange_count": inside.get("current_property_exchange_count"),
            "after_autoreleasepool_pop_count": after.get("autoreleasepool_pop_count"),
            "after_release_call_count": after.get("release_call_count"),
        },
    )


def check_storage_ownership_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "storage-ownership-reflection"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_reflection_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_backed_storage_ownership_reflection_probe.cpp"
    exe_path = case_dir / "runtime_backed_storage_ownership_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "storage ownership reflection probe")

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    box_entry = payload.get("box_entry", {})
    implementation_surface = payload.get("implementation_surface", {})
    manifest_implementation_surface = manifest.get(
        "runtime_property_ivar_accessor_reflection_implementation_surface", {}
    )

    expect(box_entry.get("found") == 1, "expected Box to be realized for storage ownership reflection")
    expect(box_entry.get("runtime_property_accessor_count", 0) >= 5,
           "expected Box to publish five runtime-backed storage accessors")
    expect(box_entry.get("runtime_instance_size_bytes", 0) >= 40,
           "expected Box instance layout to reserve five object-backed storage slots")
    expect(registration_manifest.get("property_descriptor_count") == 10,
           "expected storage ownership fixture to publish ten property descriptors")
    expect(registration_manifest.get("ivar_descriptor_count") == 5,
           "expected storage ownership fixture to publish five ivar layout descriptors")
    expect(registration_manifest.get("compile_output_truthfulness_property_descriptor_count") == 10,
           "expected compile-output truthfulness to certify ten property descriptors")
    expect(registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count") == 5,
           "expected compile-output truthfulness to certify five ivar descriptors")
    expect(
        "; runtime_backed_object_ownership_attribute_surface = "
        "contract=objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime-backed object ownership attribute surface",
    )
    expect("property_attribute_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten property-attribute profiles")
    expect("ownership_lifetime_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten ownership lifetime profiles")
    expect("ownership_runtime_hook_profiles=6" in ll_text,
           "expected LLVM IR ownership surface to publish six runtime hook profiles")
    expect("accessor_ownership_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten accessor ownership profiles")
    expected_manifest_implementation_surface = {
        "contract_id": RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
        ),
        "storage_accessor_runtime_abi_surface_contract_id": (
            RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "property_metadata_reflection_contract_id": (
            "objc3c.runtime.property.metadata.reflection.v1"
        ),
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "implementation_snapshot_symbol": (
            "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "implementation_model": (
            "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation"
        ),
        "reflection_model": (
            "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts"
        ),
        "fail_closed_model": (
            "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis"
        ),
    }
    for field, expected_value in expected_manifest_implementation_surface.items():
        expect(
            manifest_implementation_surface.get(field) == expected_value,
            f"expected property/accessor runtime implementation surface to preserve {field}",
        )
    expect(
        manifest_implementation_surface.get("requires_coupled_registration_manifest")
        is True,
        "expected property/accessor runtime implementation surface to require the coupled runtime registration manifest",
    )
    expect(
        manifest_implementation_surface.get("requires_real_compile_output") is True,
        "expected property/accessor runtime implementation surface to require real compile output",
    )
    expect(
        manifest_implementation_surface.get("requires_linked_runtime_probe") is True,
        "expected property/accessor runtime implementation surface to require a linked runtime probe",
    )
    expected_implementation_surface = {
        "property_registry_ready": 1,
        "runtime_accessor_dispatch_ready": 1,
        "runtime_layout_ready": 1,
        "reflection_query_ready": 1,
        "deterministic": 1,
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "implementation_model": (
            "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation"
        ),
        "reflection_model": (
            "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts"
        ),
        "fail_closed_model": (
            "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis"
        ),
    }
    for field, expected_value in expected_implementation_surface.items():
        expect(
            implementation_surface.get(field) == expected_value,
            f"expected live storage/accessor implementation snapshot to preserve {field}",
        )

    expected_properties = {
        "current_value_property": {
            "property_name": "currentValue",
            "slot_index": 0,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;strong=1;weak=0;unowned=0;assign=0;attributes=nonatomic,strong",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=currentValue;setter_available=1;setter=setCurrentValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "currentValue",
            "effective_setter_selector": "setCurrentValue:",
        },
        "copied_value_property": {
            "property_name": "copiedValue",
            "slot_index": 1,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=1;strong=0;weak=0;unowned=0;assign=0;attributes=copy,nonatomic",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=copiedValue;setter_available=1;setter=setCopiedValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "copiedValue",
            "effective_setter_selector": "setCopiedValue:",
        },
        "weak_value_property": {
            "property_name": "weakValue",
            "slot_index": 2,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;strong=0;weak=1;unowned=0;assign=0;attributes=nonatomic,weak",
            "ownership_lifetime_profile": "weak",
            "ownership_runtime_hook_profile": "objc-weak-side-table",
            "accessor_ownership_profile": "getter=weakValue;setter_available=1;setter=setWeakValue:;ownership_lifetime=weak;runtime_hook=objc-weak-side-table",
            "effective_getter_selector": "weakValue",
            "effective_setter_selector": "setWeakValue:",
        },
        "borrowed_value_property": {
            "property_name": "borrowedValue",
            "slot_index": 3,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=0;assign=1;attributes=assign",
            "ownership_lifetime_profile": "unowned-unsafe",
            "ownership_runtime_hook_profile": "objc-unowned-unsafe-direct",
            "accessor_ownership_profile": "getter=borrowedValue;setter_available=1;setter=setBorrowedValue:;ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct",
            "effective_getter_selector": "borrowedValue",
            "effective_setter_selector": "setBorrowedValue:",
        },
        "guarded_value_property": {
            "property_name": "guardedValue",
            "slot_index": 4,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=1;assign=0;attributes=unowned",
            "ownership_lifetime_profile": "unowned-safe",
            "ownership_runtime_hook_profile": "objc-unowned-safe-guard",
            "accessor_ownership_profile": "getter=guardedValue;setter_available=1;setter=setGuardedValue:;ownership_lifetime=unowned-safe;runtime_hook=objc-unowned-safe-guard",
            "effective_getter_selector": "guardedValue",
            "effective_setter_selector": "setGuardedValue:",
        },
    }

    for payload_key, expected in expected_properties.items():
        prop = payload.get(payload_key, {})
        expect(prop.get("found") == 1, f"expected {expected['property_name']} to be reflectable")
        expect(prop.get("has_runtime_getter") == 1 and prop.get("has_runtime_setter") == 1,
               f"expected {expected['property_name']} to execute through runtime-backed accessors")
        expect(prop.get("base_identity") == box_entry.get("base_identity"),
               f"expected {expected['property_name']} to share Box base identity")
        expect(prop.get("slot_index") == expected["slot_index"],
               f"expected {expected['property_name']} to keep slot index {expected['slot_index']}")
        expect(prop.get("size_bytes") == 8 and prop.get("alignment_bytes") == 8,
               f"expected {expected['property_name']} to preserve 8-byte object storage layout")
        expect(prop.get("property_name") == expected["property_name"],
               f"expected runtime property name for {expected['property_name']}")
        expect(prop.get("effective_getter_selector") == expected["effective_getter_selector"],
               f"expected getter selector for {expected['property_name']}")
        expect(prop.get("effective_setter_selector") == expected["effective_setter_selector"],
               f"expected setter selector for {expected['property_name']}")
        expect(prop.get("property_attribute_profile") == expected["property_attribute_profile"],
               f"expected property attribute profile for {expected['property_name']}")
        expect(prop.get("ownership_lifetime_profile") == expected["ownership_lifetime_profile"],
               f"expected ownership lifetime profile for {expected['property_name']}")
        expected_runtime_hook = expected["ownership_runtime_hook_profile"]
        if expected_runtime_hook is None:
            expect(prop.get("ownership_runtime_hook_profile") in (None, ""),
                   f"expected no runtime hook profile for {expected['property_name']}")
        else:
            expect(prop.get("ownership_runtime_hook_profile") == expected_runtime_hook,
                   f"expected runtime hook profile for {expected['property_name']}")
        expect(prop.get("accessor_ownership_profile") == expected["accessor_ownership_profile"],
               f"expected accessor ownership profile for {expected['property_name']}")
        expect(prop.get("getter_owner_identity"), f"expected getter owner identity for {expected['property_name']}")
        expect(prop.get("setter_owner_identity"), f"expected setter owner identity for {expected['property_name']}")

    return CaseResult(
        case_id="storage-ownership-reflection",
        probe="tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "runtime_property_accessor_count": box_entry.get("runtime_property_accessor_count"),
            "runtime_instance_size_bytes": box_entry.get("runtime_instance_size_bytes"),
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
            "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
            "implementation_surface_contract_id": manifest_implementation_surface.get(
                "contract_id"
            ),
            "implementation_snapshot_symbol": manifest_implementation_surface.get(
                "implementation_snapshot_symbol"
            ),
            "guarded_runtime_hook_profile": payload.get("guarded_value_property", {}).get("ownership_runtime_hook_profile"),
            "weak_runtime_hook_profile": payload.get("weak_value_property", {}).get("ownership_runtime_hook_profile"),
        },
    )


def check_storage_legality_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "storage-legality-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_legality_positive.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "positive")
    registration_manifest_path = (
        case_dir / "positive" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    sema_pass_manager_manifest = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("sema_pass_manager", {})
    )
    ll_text = ll_path.read_text(encoding="utf-8")

    expect(
        registration_manifest.get("property_descriptor_count") == 10,
        "expected storage legality positive fixture to publish ten property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 5,
        "expected storage legality positive fixture to publish five ivar descriptors",
    )
    expect(
        manifest.get("runtime_property_ivar_storage_accessor_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected storage legality positive fixture to publish the property/ivar/storage/accessor source surface",
    )
    expect(
        manifest.get(
            "runtime_property_atomicity_synthesis_reflection_source_surface", {}
        ).get("contract_id")
        == RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
        "expected storage legality positive fixture to publish the property atomicity/synthesis/reflection source surface",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_boundary_ready") is True,
        "expected storage legality positive fixture to publish a ready runtime export legality boundary",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_property_attribute_invalid_entries")
        == 0,
        "expected storage legality positive fixture to publish zero invalid property-attribute entries",
    )
    expect(
        sema_pass_manager_manifest.get(
            "runtime_export_property_attribute_contract_violations"
        )
        == 0,
        "expected storage legality positive fixture to publish zero property contract violations",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_property_ivar_binding_missing")
        == 0,
        "expected storage legality positive fixture to publish zero missing property ivar bindings",
    )
    expect(
        sema_pass_manager_manifest.get(
            "runtime_export_property_ivar_binding_conflicts"
        )
        == 0,
        "expected storage legality positive fixture to publish zero conflicting property ivar bindings",
    )
    for needle, label in (
        ("runtime_backed_storage_ownership_legality", "runtime-backed storage ownership legality"),
        ("property_attribute_profiles=10", "ten property-attribute profiles"),
        ("accessor_ownership_profiles=10", "ten accessor ownership profiles"),
    ):
        expect(
            needle in ll_text,
            f"expected storage legality positive fixture to publish {label} in LLVM IR",
        )

    atomic_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_atomic_ownership_negative.objc3",
        case_dir / "negative-atomic-ownership",
        expected_snippets=[
            "atomic ownership-aware property 'value' in interface 'Widget' is unsupported until executable accessor storage semantics land"
        ],
        expected_codes=["O3S206"],
    )
    weak_mismatch_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_weak_mismatch_negative.objc3",
        case_dir / "negative-weak-mismatch",
        expected_snippets=[
            "property ownership qualifier '__weak' conflicts with @property ownership modifier 'assign'"
        ],
        expected_codes=["O3S206"],
    )
    unowned_mismatch_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_unowned_mismatch_negative.objc3",
        case_dir / "negative-unowned-mismatch",
        expected_snippets=[
            "property ownership qualifier '__unsafe_unretained' conflicts with @property ownership modifier 'unowned'"
        ],
        expected_codes=["O3S206"],
    )
    scalar_ownership_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_scalar_ownership_negative.objc3",
        case_dir / "negative-scalar-ownership",
        expected_snippets=[
            "@property ownership modifier 'strong' requires an Objective-C object property"
        ],
        expected_codes=["O3S206"],
    )
    duplicate_getter_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "accessor_duplicate_getter_negative.objc3",
        case_dir / "negative-duplicate-getter",
        expected_snippets=[
            "duplicate effective getter selector 'value' for properties 'token' and 'alias'"
        ],
        expected_codes=["O3S206"],
    )
    duplicate_setter_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "accessor_duplicate_setter_negative.objc3",
        case_dir / "negative-duplicate-setter",
        expected_snippets=[
            "duplicate effective setter selector 'setValue:' for properties 'token' and 'alias'"
        ],
        expected_codes=["O3S206"],
    )
    readonly_setter_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_readonly_setter_negative.objc3",
        case_dir / "negative-readonly-setter",
        expected_snippets=[
            "readonly property 'value' in interface 'Widget' must not declare a setter modifier"
        ],
        expected_codes=["O3S206"],
    )

    return CaseResult(
        case_id="storage-legality-semantics",
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/runtime_backed_storage_ownership_legality_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
            "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
            "runtime_export_property_attribute_invalid_entries": sema_pass_manager_manifest.get(
                "runtime_export_property_attribute_invalid_entries"
            ),
            "runtime_export_property_attribute_contract_violations": sema_pass_manager_manifest.get(
                "runtime_export_property_attribute_contract_violations"
            ),
            "atomic_negative_diagnostic_count": atomic_negative["diagnostic_count"],
            "weak_mismatch_diagnostic_count": weak_mismatch_negative["diagnostic_count"],
            "unowned_mismatch_diagnostic_count": unowned_mismatch_negative["diagnostic_count"],
            "scalar_ownership_negative_diagnostic_count": scalar_ownership_negative[
                "diagnostic_count"
            ],
            "duplicate_getter_negative_diagnostic_count": duplicate_getter_negative[
                "diagnostic_count"
            ],
            "duplicate_setter_negative_diagnostic_count": duplicate_setter_negative[
                "diagnostic_count"
            ],
            "readonly_setter_negative_diagnostic_count": readonly_setter_negative[
                "diagnostic_count"
            ],
        },
    )


def check_property_synthesis_storage_binding_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-synthesis-storage-binding-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_synthesis_default_ivar_binding_no_redeclaration.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "positive")
    registration_manifest_path = (
        case_dir / "positive" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(
        isinstance(lowering_surface, dict),
        "expected property synthesis/storage-binding positive fixture to publish the lowering surface",
    )
    expect(
        lowering_surface.get("property_synthesis_sites") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesis sites",
    )
    expect(
        lowering_surface.get("property_synthesis_default_ivar_bindings") == 2,
        "expected no-redeclaration property synthesis fixture to publish two default ivar bindings",
    )
    expect(
        lowering_surface.get("interface_owned_property_synthesis_sites") == 2,
        "expected no-redeclaration property synthesis fixture to publish two interface-owned synthesis sites",
    )
    expect(
        lowering_surface.get("implementation_property_redeclaration_sites") == 0,
        "expected no-redeclaration property synthesis fixture to publish zero implementation redeclaration sites",
    )
    expect(
        lowering_surface.get("ivar_binding_resolved") == 2,
        "expected no-redeclaration property synthesis fixture to publish two resolved ivar bindings",
    )
    expect(
        lowering_surface.get("synthesized_accessor_owner_entries") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesized accessor owner entries",
    )
    expect(
        lowering_surface.get("synthesized_getter_entries") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesized getter entries",
    )
    expect(
        lowering_surface.get("synthesized_setter_entries") == 1,
        "expected no-redeclaration property synthesis fixture to publish one synthesized setter entry",
    )
    expect(
        lowering_surface.get("current_property_read_entries") == 2,
        "expected no-redeclaration property synthesis fixture to route both getters through current-property reads",
    )
    expect(
        lowering_surface.get("current_property_exchange_entries") == 1,
        "expected no-redeclaration property synthesis fixture to route the strong setter through current-property exchange",
    )
    expect(
        lowering_surface.get("current_property_write_entries") == 0,
        "expected no-redeclaration property synthesis fixture to avoid plain current-property writes for the strong setter path",
    )
    expect(
        lowering_surface.get("weak_current_property_load_entries") == 0
        and lowering_surface.get("weak_current_property_store_entries") == 0,
        "expected no-redeclaration property synthesis fixture to avoid weak helper selection",
    )
    expect(
        registration_manifest.get("property_descriptor_count") == 2,
        "expected no-redeclaration property synthesis fixture to publish two property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 2,
        "expected no-redeclaration property synthesis fixture to publish two ivar descriptors",
    )
    replay_key = manifest.get("lowering_property_synthesis_ivar_binding", {}).get(
        "replay_key", ""
    )
    for snippet, label in (
        (
            "interface_owned_property_synthesis_sites=2",
            "two interface-owned synthesis sites in the replay key",
        ),
        (
            "implementation_property_redeclaration_sites=0",
            "zero implementation redeclaration sites in the replay key",
        ),
        (
            "define void @objc3_method_Widget_instance_setCurrentValue_(i32 %arg0)",
            "the synthesized setter definition",
        ),
        (
            "call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)",
            "the runtime-backed setter exchange path",
        ),
    ):
        expect(
            snippet in (replay_key if "sites=" in snippet else ll_text),
            f"expected no-redeclaration property synthesis fixture to publish {label}",
        )

    incompatible_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_synthesis_default_ivar_binding_incompatible_redeclaration.objc3",
        case_dir / "negative-incompatible-redeclaration",
        expected_snippets=[
            "type mismatch: property synthesis for 'token' in implementation 'Widget' drifted from the interface default ivar binding",
            "type mismatch: incompatible property signature for 'token' in implementation 'Widget'",
        ],
        expected_codes=["O3S206"],
    )

    return CaseResult(
        case_id="property-synthesis-storage-binding-semantics",
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_synthesis_sites": lowering_surface.get("property_synthesis_sites"),
            "interface_owned_property_synthesis_sites": lowering_surface.get(
                "interface_owned_property_synthesis_sites"
            ),
            "implementation_property_redeclaration_sites": lowering_surface.get(
                "implementation_property_redeclaration_sites"
            ),
            "synthesized_getter_entries": lowering_surface.get(
                "synthesized_getter_entries"
            ),
            "synthesized_setter_entries": lowering_surface.get(
                "synthesized_setter_entries"
            ),
            "current_property_exchange_entries": lowering_surface.get(
                "current_property_exchange_entries"
            ),
            "negative_incompatible_redeclaration_diagnostic_count": incompatible_negative[
                "diagnostic_count"
            ],
        },
    )


def check_property_reflection_accessor_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "property-reflection-accessor-compatibility-diagnostics"
    getter_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_accessor_selector_compatibility_negative.objc3",
        case_dir / "accessor-selector-mismatch",
        expected_snippets=[
            "type mismatch: effective getter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
        ],
        expected_codes=["O3S206"],
    )
    setter_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_setter_selector_compatibility_negative.objc3",
        case_dir / "setter-selector-mismatch",
        expected_snippets=[
            "type mismatch: effective setter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
        ],
        expected_codes=["O3S206"],
    )
    reflection_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_reflection_attribute_compatibility_negative.objc3",
        case_dir / "reflection-attribute-mismatch",
        expected_snippets=[
            "type mismatch: reflected property attribute and ownership profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
        ],
        expected_codes=["O3S206"],
    )

    return CaseResult(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        probe="compile-diagnostics",
        fixture="tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "getter_selector_negative_diagnostic_count": getter_negative[
                "diagnostic_count"
            ],
            "setter_selector_negative_diagnostic_count": setter_negative[
                "diagnostic_count"
            ],
            "reflection_attribute_negative_diagnostic_count": reflection_negative[
                "diagnostic_count"
            ],
        },
    )


def check_property_ivar_ordering_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-ivar-ordering-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_ivar_source_model_completion_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

    surface = manifest.get("runtime_property_ivar_storage_accessor_source_surface", {})
    expect(
        surface.get("layout_init_order_field")
        == "Objc3PropertyDecl.executable_ivar_init_order_index",
        "expected property/ivar storage source surface to publish the init-order field",
    )
    expect(
        surface.get("layout_destroy_order_field")
        == "Objc3PropertyDecl.executable_ivar_destroy_order_index",
        "expected property/ivar storage source surface to publish the destruction-order field",
    )
    expect(
        surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected property/ivar storage source surface to publish the init/destroy ordering model",
    )

    property_records = manifest.get("runtime_metadata_source_records", {}).get(
        "properties", []
    )
    ivar_records = manifest.get("runtime_metadata_source_records", {}).get(
        "ivars", []
    )
    expect(
        isinstance(property_records, list) and property_records,
        "expected property ordering fixture to publish property source records",
    )
    expect(
        isinstance(ivar_records, list) and ivar_records,
        "expected property ordering fixture to publish ivar source records",
    )

    property_index = {
        (record.get("owner_kind"), record.get("owner_name"), record.get("property_name")): record
        for record in property_records
        if isinstance(record, dict)
    }
    ivar_index = {
        (record.get("owner_kind"), record.get("owner_name"), record.get("property_name")): record
        for record in ivar_records
        if isinstance(record, dict)
    }
    expected_property_records = (
        ("class-interface", "Widget", "token", 0, 2),
        ("class-interface", "Widget", "value", 1, 1),
        ("class-interface", "Widget", "count", 2, 0),
        ("class-implementation", "Widget", "token", 0, 2),
        ("class-implementation", "Widget", "value", 1, 1),
        ("class-implementation", "Widget", "count", 2, 0),
    )
    for owner_kind, owner_name, property_name, init_index, destroy_index in expected_property_records:
        record = property_index.get((owner_kind, owner_name, property_name), {})
        expect(
            record.get("executable_ivar_layout_slot_index") == init_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve slot index {init_index}",
        )
        expect(
            record.get("executable_ivar_init_order_index") == init_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve init order {init_index}",
        )
        expect(
            record.get("executable_ivar_destroy_order_index") == destroy_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve destruction order {destroy_index}",
        )

    expected_ivar_records = (
        ("class-interface", "Widget", "token", 0, 2),
        ("class-interface", "Widget", "value", 1, 1),
        ("class-interface", "Widget", "count", 2, 0),
    )
    for owner_kind, owner_name, property_name, init_index, destroy_index in expected_ivar_records:
        record = ivar_index.get((owner_kind, owner_name, property_name), {})
        expect(
            record.get("executable_ivar_init_order_index") == init_index,
            f"expected ivar record for {owner_name}.{property_name} to preserve init order {init_index}",
        )
        expect(
            record.get("executable_ivar_destroy_order_index") == destroy_index,
            f"expected ivar record for {owner_name}.{property_name} to preserve destruction order {destroy_index}",
        )

    interface_property_records = [
        property_index[( "class-interface", "Widget", property_name)]
        for property_name in ("token", "value", "count")
    ]
    implementation_property_records = [
        property_index[( "class-implementation", "Widget", property_name)]
        for property_name in ("token", "value", "count")
    ]
    interface_init_order = [
        record.get("executable_ivar_init_order_index")
        for record in interface_property_records
    ]
    interface_destroy_order = [
        record.get("executable_ivar_destroy_order_index")
        for record in interface_property_records
    ]
    implementation_init_order = [
        record.get("executable_ivar_init_order_index")
        for record in implementation_property_records
    ]
    implementation_destroy_order = [
        record.get("executable_ivar_destroy_order_index")
        for record in implementation_property_records
    ]
    expect(
        interface_init_order == [0, 1, 2],
        "expected interface property init order to remain monotonic",
    )
    expect(
        interface_destroy_order == [2, 1, 0],
        "expected interface property destruction order to remain reverse-monotonic",
    )
    expect(
        implementation_init_order == [0, 1, 2],
        "expected implementation property init order to match interface ordering",
    )
    expect(
        implementation_destroy_order == [2, 1, 0],
        "expected implementation property destruction order to match interface reverse ordering",
    )

    return CaseResult(
        case_id="property-ivar-ordering-semantics",
        probe="compile-manifest-source-records",
        fixture="tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_record_count": len(property_records),
            "ivar_record_count": len(ivar_records),
            "token_destroy_order_index": property_index.get(
                ("class-interface", "Widget", "token"), {}
            ).get("executable_ivar_destroy_order_index"),
            "count_init_order_index": property_index.get(
                ("class-interface", "Widget", "count"), {}
            ).get("executable_ivar_init_order_index"),
            "interface_init_order": interface_init_order,
            "interface_destroy_order": interface_destroy_order,
            "implementation_init_order": implementation_init_order,
            "implementation_destroy_order": implementation_destroy_order,
        },
    )


def check_synthesized_accessor_runtime_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "synthesized-accessor-runtime"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "synthesized_accessor_probe.cpp"
    exe_path = case_dir / "synthesized_accessor_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "synthesized accessor runtime probe")

    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})
    enabled_entry = payload.get("enabled_entry", {})
    set_enabled_entry = payload.get("set_enabled_entry", {})
    value_entry = payload.get("value_entry", {})
    set_value_entry = payload.get("set_value_entry", {})

    expect(payload.get("widget_instance", 0) > 0, "expected synthesized-accessor runtime probe to allocate a positive Widget receiver")
    expect(payload.get("set_count_result") == 0, "expected synthesized-accessor count setter dispatch to return zero")
    expect(payload.get("count_value") == 37, "expected synthesized-accessor count getter to reload 37")
    expect(payload.get("set_enabled_result") == 0, "expected synthesized-accessor enabled setter dispatch to return zero")
    expect(payload.get("enabled_value") == 1, "expected synthesized-accessor enabled getter to reload 1")
    expect(payload.get("set_value_result") == 0, "expected synthesized-accessor value setter dispatch to return zero")
    expect(payload.get("value_result") == 55, "expected synthesized-accessor value getter to reload 55")

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected synthesized-accessor runtime probe to report at least one registered image")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected synthesized-accessor runtime probe to report a non-zero descriptor total")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected synthesized-accessor runtime probe to materialize the accessor selector surface")
    expect(selector_state.get("metadata_backed_selector_count", 0) >= 6, "expected synthesized-accessor runtime probe to preserve metadata-backed selectors")

    expected_entries = (
        (count_entry, "count", 0, "implementation:Widget::instance_method:count"),
        (set_count_entry, "setCount:", 1, "implementation:Widget::instance_method:setCount:"),
        (enabled_entry, "enabled", 0, "implementation:Widget::instance_method:enabled"),
        (set_enabled_entry, "setEnabled:", 1, "implementation:Widget::instance_method:setEnabled:"),
        (value_entry, "value", 0, "implementation:Widget::instance_method:value"),
        (set_value_entry, "setValue:", 1, "implementation:Widget::instance_method:setValue:"),
    )
    for entry, selector, parameter_count, owner_identity in expected_entries:
        expect(entry.get("found") == 1 and entry.get("resolved") == 1, f"expected {selector} cache entry to resolve live")
        expect(entry.get("selector") == selector, f"expected {selector} cache entry to preserve selector spelling")
        expect(entry.get("parameter_count") == parameter_count, f"expected {selector} cache entry to preserve parameter count {parameter_count}")
        expect(entry.get("resolved_class_name") == "Widget", f"expected {selector} cache entry to resolve against Widget")
        expect(entry.get("resolved_owner_identity") == owner_identity, f"expected {selector} cache entry to preserve owner identity")

    return CaseResult(
        case_id="synthesized-accessor-runtime",
        probe="tests/tooling/runtime/synthesized_accessor_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "widget_instance": payload["widget_instance"],
            "count_value": payload["count_value"],
            "enabled_value": payload["enabled_value"],
            "value_result": payload["value_result"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


def check_accessor_storage_lowering_metadata_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "accessor-storage-lowering-metadata"

    def load_compile_artifacts(fixture_name: str, output_name: str) -> tuple[dict[str, Any], str]:
        fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name
        _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / output_name)
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        return manifest, ll_path.read_text(encoding="utf-8")

    def find_property_entry(entries: list[dict[str, Any]], owner_kind: str, owner_name: str, property_name: str) -> dict[str, Any]:
        for entry in entries:
            if (
                entry.get("owner_kind") == owner_kind
                and entry.get("owner_name") == owner_name
                and entry.get("property_name") == property_name
            ):
                return entry
        raise RuntimeError(
            f"missing property entry for {owner_kind}:{owner_name}:{property_name}"
        )

    def expect_property_lowering(
        manifest: dict[str, Any],
        owner_kind: str,
        owner_name: str,
        property_name: str,
        synthesizes_executable_accessors: bool,
        getter_helper_symbol: str,
        setter_helper_symbol: str,
    ) -> None:
        runtime_metadata_records = manifest.get("runtime_metadata_source_records", {})
        property_records = runtime_metadata_records.get("properties", [])
        expect(
            isinstance(property_records, list),
            "expected runtime metadata source records to publish property entries",
        )
        property_record = find_property_entry(
            property_records, owner_kind, owner_name, property_name
        )
        expect(
            property_record.get("synthesizes_executable_accessors")
            is synthesizes_executable_accessors,
            f"expected runtime metadata property record to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_record.get("getter_storage_runtime_helper_symbol")
            == getter_helper_symbol,
            f"expected runtime metadata property record to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_record.get("setter_storage_runtime_helper_symbol")
            == setter_helper_symbol,
            f"expected runtime metadata property record to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )

        source_graph = manifest.get("objc_executable_metadata_source_graph")
        if not isinstance(source_graph, dict):
            source_graph = manifest.get("executable_metadata_source_graph")
        if not isinstance(source_graph, dict):
            source_graph = (
                manifest.get("frontend", {})
                .get("pipeline", {})
                .get("semantic_surface", {})
                .get("objc_executable_metadata_source_graph", {})
            )
        property_nodes = source_graph.get("property_node_entries", [])
        expect(
            isinstance(property_nodes, list),
            "expected executable metadata source graph to publish property nodes",
        )
        property_node = find_property_entry(
            property_nodes, owner_kind, owner_name, property_name
        )
        expect(
            property_node.get("synthesizes_executable_accessors")
            is synthesizes_executable_accessors,
            f"expected executable metadata property node to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_node.get("getter_storage_runtime_helper_symbol")
            == getter_helper_symbol,
            f"expected executable metadata property node to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_node.get("setter_storage_runtime_helper_symbol")
            == setter_helper_symbol,
            f"expected executable metadata property node to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )

    synthesized_manifest, synthesized_ll = load_compile_artifacts(
        "synthesized_accessor_property_lowering_positive.objc3",
        "synthesized-accessors",
    )
    synthesized_lowering_surface = synthesized_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        isinstance(synthesized_lowering_surface, dict),
        "expected synthesized accessor lowering metadata fixture to publish the lowering surface",
    )
    expect(
        synthesized_lowering_surface.get("contract_id")
        == DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        "expected synthesized accessor lowering metadata fixture to preserve the lowering surface contract id",
    )
    expect(
        synthesized_lowering_surface.get("compile_manifest_artifact") == "module.manifest.json",
        "expected lowering surface to couple back to the compile manifest artifact",
    )
    expect(
        synthesized_lowering_surface.get("registration_manifest_artifact")
        == "module.runtime-registration-manifest.json",
        "expected lowering surface to couple back to the runtime registration manifest artifact",
    )
    expect(
        synthesized_lowering_surface.get("object_artifact") == "module.obj"
        and synthesized_lowering_surface.get("backend_artifact") == "module.ll",
        "expected lowering surface to couple back to the emitted object and LLVM IR artifacts",
    )
    expect(
        synthesized_lowering_surface.get(
            "runtime_property_ivar_storage_accessor_source_surface_contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected lowering surface to point back at the runtime property/ivar storage source surface",
    )
    expect(
        synthesized_lowering_surface.get(
            "storage_accessor_runtime_abi_surface_contract_id"
        )
        == RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "expected lowering surface to point at the storage/accessor runtime ABI surface",
    )
    expect(
        synthesized_lowering_surface.get("lowering_contract_source_path")
        == "native/objc3c/src/lower/objc3_lowering_contract.h",
        "expected lowering surface to publish the lowering contract source path",
    )
    expect(
        synthesized_lowering_surface.get("ir_emitter_source_path")
        == "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "expected lowering surface to publish the IR emitter source path",
    )
    expect(
        synthesized_lowering_surface.get("frontend_artifacts_source_path")
        == "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        "expected lowering surface to publish the frontend artifacts source path",
    )
    expect(
        synthesized_lowering_surface.get("runtime_source_path")
        == "native/objc3c/src/runtime/objc3_runtime.cpp",
        "expected lowering surface to publish the runtime source path",
    )
    expect(
        synthesized_lowering_surface.get("accessor_storage_lowering_metadata_model")
        == "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path",
        "expected lowering surface to publish the accessor-storage metadata model",
    )
    expect(
        synthesized_lowering_surface.get(
            "accessor_storage_lowering_helper_selection_model"
        )
        == "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers",
        "expected lowering surface to publish the helper-selection model",
    )
    expect(
        synthesized_lowering_surface.get("authoritative_fixture_paths")
        == [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "expected lowering surface to publish the authoritative fixture set",
    )
    expect(
        synthesized_lowering_surface.get("authoritative_probe_paths")
        == [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "expected lowering surface to publish the authoritative probe set",
    )
    expect(
        synthesized_lowering_surface.get("explicit_non_goals")
        == [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-lowering-proof",
        ],
        "expected lowering surface to publish explicit non-goals",
    )
    expect(
        synthesized_lowering_surface.get("requires_coupled_registration_manifest")
        is True
        and synthesized_lowering_surface.get("requires_real_compile_output") is True
        and synthesized_lowering_surface.get("requires_linked_runtime_probe") is True,
        "expected lowering surface to require the coupled registration manifest, real compile output, and linked runtime probes",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_accessor_owner_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered property owners",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_getter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered getters",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_setter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered setters",
    )
    expect(
        synthesized_lowering_surface.get("current_property_read_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three current-property read entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_write_entries") == 2,
        "expected synthesized accessor lowering metadata fixture to publish two current-property write entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_exchange_entries") == 1,
        "expected synthesized accessor lowering metadata fixture to publish one current-property exchange entry",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_load_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-load entries",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_store_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-store entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_read_symbol")
        == "objc3_runtime_read_current_property_i32",
        "expected lowering surface to publish the canonical current-property read symbol",
    )
    expect(
        synthesized_lowering_surface.get("current_property_write_symbol")
        == "objc3_runtime_write_current_property_i32",
        "expected lowering surface to publish the canonical current-property write symbol",
    )
    expect(
        synthesized_lowering_surface.get("current_property_exchange_symbol")
        == "objc3_runtime_exchange_current_property_i32",
        "expected lowering surface to publish the canonical current-property exchange symbol",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_load_symbol")
        == "objc3_runtime_load_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property load symbol",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_store_symbol")
        == "objc3_runtime_store_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property store symbol",
    )
    expect(
        "getter_definitions=3" in synthesized_ll
        and "setter_definitions=3" in synthesized_ll
        and "read_current_property_calls=3" in synthesized_ll
        and "write_current_property_calls=2" in synthesized_ll
        and "exchange_current_property_calls=1" in synthesized_ll,
        "expected synthesized accessor lowering metadata fixture LLVM IR to agree with the published lowering counts",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-interface",
        "Widget",
        "count",
        False,
        "",
        "",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "count",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "enabled",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "value",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )

    arc_manifest, arc_ll = load_compile_artifacts(
        "arc_property_interaction_positive.objc3",
        "arc-accessors",
    )
    arc_lowering_surface = arc_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        arc_lowering_surface.get("synthesized_accessor_owner_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered property owners",
    )
    expect(
        arc_lowering_surface.get("synthesized_getter_entries") == 2
        and arc_lowering_surface.get("synthesized_setter_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered getters and setters",
    )
    expect(
        arc_lowering_surface.get("current_property_read_entries") == 1,
        "expected ARC property interaction fixture to publish one plain/strong getter read entry",
    )
    expect(
        arc_lowering_surface.get("current_property_write_entries") == 0,
        "expected ARC property interaction fixture to publish zero plain write entries",
    )
    expect(
        arc_lowering_surface.get("current_property_exchange_entries") == 1,
        "expected ARC property interaction fixture to publish one strong exchange entry",
    )
    expect(
        arc_lowering_surface.get("weak_current_property_load_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-load entry",
    )
    expect(
        arc_lowering_surface.get("weak_current_property_store_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-store entry",
    )
    expect(
        "exchange_current_property_calls=1" in arc_ll
        and "weak_load_current_property_calls=1" in arc_ll
        and "weak_store_current_property_calls=1" in arc_ll,
        "expected ARC property interaction fixture LLVM IR to agree with the published helper-lowering counts",
    )
    expect_property_lowering(
        arc_manifest,
        "class-interface",
        "ArcBox",
        "currentValue",
        False,
        "",
        "",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "currentValue",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "weakValue",
        True,
        "objc3_runtime_load_weak_current_property_i32",
        "objc3_runtime_store_weak_current_property_i32",
    )

    return CaseResult(
        case_id="accessor-storage-lowering-metadata-surface",
        probe="compile-manifest-and-executable-metadata-surface",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "synthesized_accessor_owner_entries": synthesized_lowering_surface.get(
                "synthesized_accessor_owner_entries"
            ),
            "synthesized_getter_entries": synthesized_lowering_surface.get(
                "synthesized_getter_entries"
            ),
            "synthesized_setter_entries": synthesized_lowering_surface.get(
                "synthesized_setter_entries"
            ),
            "strong_exchange_entries": synthesized_lowering_surface.get(
                "current_property_exchange_entries"
            ),
            "weak_store_entries": arc_lowering_surface.get(
                "weak_current_property_store_entries"
            ),
        },
    )


def check_property_accessor_layout_lowering_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-accessor-layout-lowering"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "synthesized_accessor_property_lowering_positive.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = (
        case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    ll_text = ll_path.read_text(encoding="utf-8")

    property_source_surface = manifest.get(
        "runtime_property_ivar_storage_accessor_source_surface", {}
    )
    expect(
        property_source_surface.get(
            "executable_property_accessor_layout_lowering_contract_id"
        )
        == "objc3c.executable.property.accessor.layout.lowering.v1",
        "expected property/ivar storage source surface to point at the executable accessor/layout lowering surface",
    )
    expect(
        property_source_surface.get("executable_ivar_layout_emission_contract_id")
        == "objc3c.executable.ivar.layout.emission.v1",
        "expected property/ivar storage source surface to point at the executable ivar layout emission surface",
    )
    expect(
        property_source_surface.get(
            "executable_synthesized_accessor_property_lowering_contract_id"
        )
        == "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "expected property/ivar storage source surface to point at the synthesized accessor lowering surface",
    )

    accessor_layout_surface = manifest.get(
        "executable_property_accessor_layout_lowering_surface", {}
    )
    expect(
        isinstance(accessor_layout_surface, dict),
        "expected compile manifest to publish the executable accessor/layout lowering surface",
    )
    expected_accessor_layout_fields = {
        "contract_id": "objc3c.executable.property.accessor.layout.lowering.v1",
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            "objc3c.runtime.property.ivar.storage.accessor.source.surface.v1"
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "property_table_model": (
            "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records"
        ),
        "ivar_layout_model": (
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-slot-size-alignment-records"
        ),
        "accessor_binding_model": (
            "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis"
        ),
        "scope_model": "ast-sema-property-layout-handoff-ir-object-metadata-publication",
        "fail_closed_model": (
            "no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation"
        ),
        "compile_manifest_artifact": "module.manifest.json",
        "registration_manifest_artifact": "module.runtime-registration-manifest.json",
        "object_artifact": "module.obj",
        "backend_artifact": "module.ll",
    }
    for field, expected_value in expected_accessor_layout_fields.items():
        expect(
            accessor_layout_surface.get(field) == expected_value,
            f"expected accessor/layout lowering surface to preserve {field}",
        )
    expect(
        accessor_layout_surface.get("property_metadata_entries") == 6,
        "expected accessor/layout lowering surface to publish six property metadata entries",
    )
    expect(
        accessor_layout_surface.get("ivar_metadata_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar metadata entries",
    )
    expect(
        accessor_layout_surface.get("property_descriptor_entries") == 6,
        "expected accessor/layout lowering surface to publish six property descriptors",
    )
    expect(
        accessor_layout_surface.get("ivar_descriptor_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar descriptors",
    )
    expect(
        accessor_layout_surface.get("property_attribute_profile_entries") == 6,
        "expected accessor/layout lowering surface to publish six property attribute profiles",
    )
    expect(
        accessor_layout_surface.get("accessor_ownership_profile_entries") == 6,
        "expected accessor/layout lowering surface to publish six accessor ownership profiles",
    )
    expect(
        accessor_layout_surface.get("synthesized_binding_entries") == 6,
        "expected accessor/layout lowering surface to publish six synthesized binding entries",
    )
    expect(
        accessor_layout_surface.get("ivar_layout_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar layout entries",
    )
    expect(
        accessor_layout_surface.get("ivar_layout_owner_entries") == 1,
        "expected accessor/layout lowering surface to publish one ivar layout owner",
    )
    expect(
        accessor_layout_surface.get("descriptor_counts_match_source_graph") is True,
        "expected accessor/layout lowering surface descriptor counts to match the executable source graph",
    )

    ivar_layout_surface = manifest.get("executable_ivar_layout_emission_surface", {})
    expect(
        isinstance(ivar_layout_surface, dict),
        "expected compile manifest to publish the executable ivar layout emission surface",
    )
    expected_ivar_layout_fields = {
        "contract_id": "objc3c.executable.ivar.layout.emission.v1",
        "executable_property_accessor_layout_lowering_surface_contract_id": (
            "objc3c.executable.property.accessor.layout.lowering.v1"
        ),
        "descriptor_model": (
            "ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment"
        ),
        "offset_global_model": "one-retained-i64-offset-global-per-emitted-ivar-binding",
        "layout_table_model": (
            "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size"
        ),
        "scope_model": (
            "sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation"
        ),
        "fail_closed_model": (
            "no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis"
        ),
    }
    for field, expected_value in expected_ivar_layout_fields.items():
        expect(
            ivar_layout_surface.get(field) == expected_value,
            f"expected ivar layout emission surface to preserve {field}",
        )
    expect(
        ivar_layout_surface.get("offset_global_entries") == 3,
        "expected ivar layout emission surface to publish three offset globals",
    )
    expect(
        ivar_layout_surface.get("layout_table_entries") == 1,
        "expected ivar layout emission surface to publish one layout table",
    )
    expect(
        ivar_layout_surface.get("layout_owner_entries") == 1,
        "expected ivar layout emission surface to publish one layout owner",
    )
    expect(
        ivar_layout_surface.get("ivar_descriptor_entries") == 3,
        "expected ivar layout emission surface to publish three ivar descriptors",
    )

    synthesized_accessor_surface = manifest.get(
        "executable_synthesized_accessor_property_lowering_surface", {}
    )
    expect(
        isinstance(synthesized_accessor_surface, dict),
        "expected compile manifest to publish the synthesized accessor lowering surface",
    )
    expected_synthesized_accessor_fields = {
        "contract_id": "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "executable_property_accessor_layout_lowering_surface_contract_id": (
            "objc3c.executable.property.accessor.layout.lowering.v1"
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "source_model": (
            "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists"
        ),
        "storage_model": (
            "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals"
        ),
        "property_descriptor_model": (
            "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers"
        ),
        "fail_closed_model": (
            "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-storage-global-fallbacks"
        ),
    }
    for field, expected_value in expected_synthesized_accessor_fields.items():
        expect(
            synthesized_accessor_surface.get(field) == expected_value,
            f"expected synthesized accessor lowering surface to preserve {field}",
        )
    expect(
        synthesized_accessor_surface.get("implementation_owned_property_entries") == 3,
        "expected synthesized accessor lowering surface to publish three implementation-owned properties",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_getter_entries") == 3,
        "expected synthesized accessor lowering surface to publish three synthesized getters",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_setter_entries") == 3,
        "expected synthesized accessor lowering surface to publish three synthesized setters",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_accessor_entries") == 6,
        "expected synthesized accessor lowering surface to publish six synthesized accessors",
    )
    expect(
        synthesized_accessor_surface.get("property_descriptor_entries") == 6,
        "expected synthesized accessor lowering surface to publish six property descriptors",
    )

    expect(
        registration_manifest.get("property_descriptor_count") == 6,
        "expected registration manifest to publish six property descriptors for the synthesized accessor lowering fixture",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 3,
        "expected registration manifest to publish three ivar descriptors for the synthesized accessor lowering fixture",
    )

    for snippet, label in (
        (
            "; executable_property_accessor_layout_lowering = "
            "contract=objc3c.executable.property.accessor.layout.lowering.v1",
            "the executable property accessor/layout lowering summary",
        ),
        (
            "property_metadata_entries=6;ivar_metadata_entries=3;"
            "property_attribute_profiles=6;accessor_ownership_profiles=6;"
            "synthesized_binding_entries=6;ivar_layout_entries=3",
            "the accessor/layout lowering inventory counts",
        ),
        (
            "; executable_ivar_layout_emission = "
            "contract=objc3c.executable.ivar.layout.emission.v1",
            "the executable ivar layout emission summary",
        ),
        (
            "offset_global_entries=3;layout_table_entries=1;layout_owner_entries=1",
            "the ivar layout emission inventory counts",
        ),
        (
            "; executable_synthesized_accessor_property_lowering = "
            "contract=objc3c.executable.synthesized.accessor.property.lowering.v1",
            "the synthesized accessor lowering summary",
        ),
        (
            "synthesized_accessor_entries=6",
            "the synthesized accessor entry count",
        ),
        (
            "define i32 @objc3_method_Widget_instance_count() {",
            "the synthesized count getter body",
        ),
        (
            "define void @objc3_method_Widget_instance_setCount_(i32 %arg0) {",
            "the synthesized count setter body",
        ),
        (
            "@__objc3_meta_ivar_layout_table_0000 = private global",
            "the emitted ivar layout table",
        ),
        (
            "@__objc3_meta_ivar_offset_0000",
            "the emitted ivar offset globals",
        ),
    ):
        expect(
            snippet in ll_text,
            f"expected synthesized accessor/layout lowering fixture LLVM IR to publish {label}",
        )

    return CaseResult(
        case_id="property-accessor-layout-lowering",
        probe="compile-manifest-registration-manifest-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_descriptor_entries": accessor_layout_surface.get(
                "property_descriptor_entries"
            ),
            "ivar_descriptor_entries": accessor_layout_surface.get(
                "ivar_descriptor_entries"
            ),
            "synthesized_accessor_entries": synthesized_accessor_surface.get(
                "synthesized_accessor_entries"
            ),
            "layout_table_entries": ivar_layout_surface.get("layout_table_entries"),
        },
    )


def check_property_layout_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-layout"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path, ll_path, _ = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "property_layout_runtime_probe.cpp"
    exe_path = case_dir / "property_layout_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property layout runtime probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})

    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime property/layout consumption surface",
    )
    expect("synthesized_accessor_entries=6" in ll_text, "expected property-layout fixture to preserve six synthesized accessors")
    expect("property_descriptor_entries=" in ll_text, "expected property-layout fixture to publish property descriptor inventory")
    expect("ivar_layout_owner_entries=" in ll_text, "expected property-layout fixture to publish ivar layout owner inventory")

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))
    same_instance_mode = first_alloc == 1025 and second_alloc == 1025
    distinct_instance_mode = first_alloc > 0 and second_alloc > 0 and first_alloc != second_alloc

    expect(first_alloc > 0, "expected first alloc to materialize a positive Widget instance identity")
    expect(
        same_instance_mode or distinct_instance_mode,
        "expected property-layout probe to preserve either the historical single-instance mode or the forward-compatible distinct-instance mode",
    )
    expect(payload.get("set_count_result") == 0, "expected count setter dispatch to return zero")
    expect(payload.get("count_value_first") == 37, "expected count getter to observe the written value on the first alloc")
    expect(
        payload.get("count_value_second") == (37 if same_instance_mode else 0),
        "expected second alloc to preserve historical shared storage or observe the forward-compatible distinct-instance default",
    )
    expect(payload.get("set_enabled_result") == 0, "expected enabled setter dispatch to return zero")
    expect(
        payload.get("enabled_value_second") == (1 if same_instance_mode else 0),
        "expected second alloc to preserve historical shared bool storage or observe the forward-compatible distinct-instance default",
    )
    expect(payload.get("set_value_result") == 0, "expected value setter dispatch to return zero")
    expect(
        payload.get("value_result_second") == (55 if same_instance_mode else 0),
        "expected second alloc to preserve historical shared scalar storage or observe the forward-compatible distinct-instance default",
    )

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected property-layout runtime to report at least one registered image")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected property-layout runtime to report a non-zero descriptor total")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected property-layout runtime to materialize the synthesized accessor selector surface")

    expect(count_entry.get("found") == 1 and count_entry.get("resolved") == 1, "expected count getter cache entry to resolve")
    expect(count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        str(count_entry.get("resolved_owner_identity", "")).endswith("implementation:Widget::instance_method:count"),
        "expected count getter cache entry to preserve the synthesized owner identity",
    )

    expect(set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1, "expected setCount setter cache entry to resolve")
    expect(set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        str(set_count_entry.get("resolved_owner_identity", "")).endswith("implementation:Widget::instance_method:setCount:"),
        "expected setCount setter cache entry to preserve the synthesized owner identity",
    )

    allocation_mode = "shared-instance-freeze" if same_instance_mode else "distinct-instance-forward-compatible"
    return CaseResult(
        case_id="property-layout",
        probe="tests/tooling/runtime/property_layout_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "allocation_mode": allocation_mode,
            "first_alloc": first_alloc,
            "second_alloc": second_alloc,
            "count_value_first": payload["count_value_first"],
            "count_value_second": payload["count_value_second"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


def check_synthesized_accessor_codegen_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-codegen"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))

    required_ir_snippets = {
        "count getter": "define i32 @objc3_method_Widget_instance_count()",
        "count setter": "define void @objc3_method_Widget_instance_setCount_(i32 %arg0)",
        "enabled getter": "define i1 @objc3_method_Widget_instance_enabled()",
        "enabled setter": "define void @objc3_method_Widget_instance_setEnabled_(i1 %arg0)",
        "value getter": "define i32 @objc3_method_Widget_instance_value()",
        "value setter": "define void @objc3_method_Widget_instance_setValue_(i32 %arg0)",
        "getter runtime read": "call i32 @objc3_runtime_read_current_property_i32()",
        "setter runtime write": "call void @objc3_runtime_write_current_property_i32(i32 %arg0)",
        "bool setter coercion": "%objc3_property_value = zext i1 %arg0 to i32",
        "strong getter retain": "%objc3_property_retained = call i32 @objc3_runtime_retain_i32(i32 %objc3_property_slot)",
        "strong getter autorelease": "%objc3_property_autoreleased = call i32 @objc3_runtime_autorelease_i32(i32 %objc3_property_retained)",
        "strong setter exchange": "%objc3_property_previous = call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)",
        "strong setter release": "%objc3_property_release = call i32 @objc3_runtime_release_i32(i32 %objc3_property_previous)",
        "count descriptor getter binding": "ptr @objc3_method_Widget_instance_count, ptr @objc3_method_Widget_instance_setCount_",
        "enabled descriptor getter binding": "ptr @objc3_method_Widget_instance_enabled, ptr @objc3_method_Widget_instance_setEnabled_",
        "value descriptor getter binding": "ptr @objc3_method_Widget_instance_value, ptr @objc3_method_Widget_instance_setValue_",
    }
    for label, snippet in required_ir_snippets.items():
        expect(snippet in ll_text, f"expected synthesized accessor codegen to emit {label}")

    synthesis_summary = manifest.get("lowering_property_synthesis_ivar_binding", {})
    expect(isinstance(synthesis_summary, dict), "expected property synthesis lowering summary in compile manifest")
    expect(synthesis_summary.get("deterministic_handoff") is True,
           "expected property synthesis lowering summary to report deterministic handoff")
    replay_key = synthesis_summary.get("replay_key", "")
    expect("property_synthesis_sites=3" in replay_key,
           "expected property synthesis replay key to record the three synthesized properties")
    expect("property_synthesis_default_ivar_bindings=3" in replay_key,
           "expected property synthesis replay key to record the default ivar bindings")
    expect(registration_manifest.get("property_descriptor_count", 0) >= 6,
           "expected runtime registration manifest to publish synthesized property descriptors")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(isinstance(lowering_surface, dict),
           "expected authoritative dispatch and synthesized-accessor lowering surface in compile manifest")
    expect(lowering_surface.get("contract_id") ==
           "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
           "expected lowering surface contract id for dispatch and synthesized accessors")
    expect(lowering_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected lowering surface to publish canonical runtime dispatch symbol")
    expect(lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
           "expected lowering surface to bind lowering, shim, and runtime library dispatch symbols together")
    expect(lowering_surface.get("property_synthesis_sites") == 3,
           "expected lowering surface to publish three synthesized properties")
    expect(lowering_surface.get("property_synthesis_default_ivar_bindings") == 3,
           "expected lowering surface to publish three default ivar bindings")
    expect(lowering_surface.get("property_descriptor_count") == registration_manifest.get("property_descriptor_count"),
           "expected lowering surface property descriptor count to match runtime registration manifest")
    expect(lowering_surface.get("ivar_descriptor_count") == registration_manifest.get("ivar_descriptor_count"),
           "expected lowering surface ivar descriptor count to match runtime registration manifest")
    expect(lowering_surface.get("deterministic_handoff") is True,
           "expected lowering surface to report deterministic handoff")
    expect(
        "; dispatch_and_synthesized_accessor_lowering_surface = "
        "contract_id=objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        in ll_text,
        "expected LLVM IR banner to publish dispatch and synthesized-accessor lowering surface",
    )
    expect("property_synthesis_sites=3" in ll_text,
           "expected LLVM IR banner to report three synthesized properties")
    expect("property_descriptor_count=6" in ll_text,
           "expected LLVM IR banner to report synthesized property descriptor count")
    expect("member_table_emission_ready=true" in ll_text,
           "expected LLVM IR banner to report member table emission readiness")
    expect(
        "; synthesized_getter_setter_llvm_ir_generation_surface = "
        "contract_id=objc3c.synthesized.getter.setter.llvm.ir.generation.v1"
        in ll_text,
        "expected LLVM IR to publish synthesized getter/setter generation surface",
    )
    expect("getter_definitions=3" in ll_text,
           "expected synthesized accessor fixture to emit three getter definitions")
    expect("setter_definitions=3" in ll_text,
           "expected synthesized accessor fixture to emit three setter definitions")
    expect("read_current_property_calls=3" in ll_text,
           "expected synthesized accessor fixture to emit three current-property reads")
    expect("write_current_property_calls=2" in ll_text,
           "expected synthesized accessor fixture to emit two current-property writes")
    expect("exchange_current_property_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one strong current-property exchange")
    expect("retain_calls=2" in ll_text,
           "expected synthesized accessor fixture to emit two retain helper calls")
    expect("release_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one release helper call")
    expect("autorelease_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one autorelease helper call")

    return CaseResult(
        case_id="property-codegen",
        probe="real-compile-llvm-inspection",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(registration_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        },
    )


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_dir = TMP_ROOT / run_id
    report_path = REPORT_ROOT / "summary.json"
    run_dir.mkdir(parents=True, exist_ok=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)

    ensure_native_binaries()
    clangxx = find_clangxx()

    results = [
        check_runtime_library_case(clangxx, run_dir),
        check_installation_lifecycle_case(clangxx, run_dir),
        check_metaprogramming_source_surface_case(run_dir),
        check_metaprogramming_package_provenance_source_surface_case(run_dir),
        check_metaprogramming_semantics_case(run_dir),
        check_metaprogramming_derive_property_behavior_semantics_case(run_dir),
        check_unified_concurrency_runtime_architecture_case(run_dir),
        check_async_task_actor_normalization_completion_case(run_dir),
        check_unified_concurrency_lowering_metadata_surface_case(run_dir),
        check_unified_concurrency_runtime_abi_case(clangxx, run_dir),
        check_live_unified_concurrency_runtime_implementation_case(clangxx, run_dir),
        check_error_execution_cleanup_source_case(run_dir),
        check_catch_filter_finalization_source_case(run_dir),
        check_error_propagation_cleanup_semantics_case(run_dir),
        check_executable_try_throw_do_catch_semantics_case(run_dir),
        check_bridging_filter_unwind_compatibility_diagnostics_case(run_dir),
        check_error_lowering_unwind_bridge_helper_surface_case(run_dir),
        check_executable_throw_catch_cleanup_lowering_case(run_dir),
        check_cross_module_error_metadata_replay_preservation_case(run_dir),
        check_error_runtime_abi_cleanup_case(clangxx, run_dir),
        check_live_error_runtime_integration_case(clangxx, run_dir),
        check_cross_module_concurrency_actor_artifact_preservation_case(run_dir),
        check_cross_module_block_ownership_artifact_preservation_case(run_dir),
        check_cross_module_storage_reflection_artifact_preservation_case(run_dir),
        check_imported_runtime_packaging_replay_case(clangxx, run_dir),
        check_canonical_dispatch_case(clangxx, run_dir),
        check_canonical_sample_set_case(clangxx, run_dir),
        check_realization_lookup_reflection_runtime_case(clangxx, run_dir),
        check_live_dispatch_fast_path_case(clangxx, run_dir),
        check_storage_ownership_reflection_case(clangxx, run_dir),
        check_property_ivar_ordering_semantics_case(run_dir),
        check_accessor_storage_lowering_metadata_surface_case(run_dir),
        check_property_accessor_layout_lowering_case(run_dir),
        check_property_reflection_accessor_compatibility_diagnostics_case(run_dir),
        check_property_synthesis_storage_binding_semantics_case(run_dir),
        check_storage_legality_semantics_case(run_dir),
        check_synthesized_accessor_codegen_case(run_dir),
        check_synthesized_accessor_runtime_case(clangxx, run_dir),
        check_property_layout_case(clangxx, run_dir),
        check_property_execution_case(clangxx, run_dir),
        check_property_reflection_case(clangxx, run_dir),
        check_escaping_block_capture_legality_case(run_dir),
        check_block_storage_arc_automation_semantics_case(run_dir),
        check_block_arc_runtime_abi_case(clangxx, run_dir),
        check_block_helper_runtime_execution_case(clangxx, run_dir),
        check_arc_property_helper_case(clangxx, run_dir),
    ]

    summary = {
        "status": "PASS",
        "run_dir": str(run_dir.relative_to(ROOT)).replace("\\", "/"),
        "clangxx": clangxx,
        "runtime_library": str(RUNTIME_LIB.relative_to(ROOT)).replace("\\", "/"),
        "case_count": len(results),
        "cases": [
            {
                "case_id": result.case_id,
                "probe": result.probe,
                "fixture": result.fixture,
                "claim_class": result.claim_class,
                "passed": result.passed,
                "summary": result.summary,
            }
            for result in results
        ],
        "claim_boundary": build_claim_boundary(),
        "runtime_state_publication_surface": build_runtime_state_publication_surface(),
        "runtime_bootstrap_registration_source_surface": build_runtime_bootstrap_registration_source_surface(),
        "runtime_bootstrap_lowering_registration_artifact_surface": (
            build_runtime_bootstrap_lowering_registration_artifact_surface()
        ),
        "runtime_multi_image_startup_ordering_source_surface": (
            build_runtime_multi_image_startup_ordering_source_surface(results)
        ),
        "runtime_metaprogramming_source_surface": (
            build_runtime_metaprogramming_source_surface(results)
        ),
        "runtime_metaprogramming_package_provenance_source_surface": (
            build_runtime_metaprogramming_package_provenance_source_surface(results)
        ),
        "runtime_metaprogramming_semantics_surface": (
            build_runtime_metaprogramming_semantics_surface(results)
        ),
        "runtime_unified_concurrency_source_surface": (
            build_runtime_unified_concurrency_source_surface(results)
        ),
        "runtime_async_task_actor_normalization_completion_surface": (
            build_runtime_async_task_actor_normalization_completion_surface(results)
        ),
        "runtime_unified_concurrency_lowering_metadata_surface": (
            build_runtime_unified_concurrency_lowering_metadata_surface(results)
        ),
        "runtime_unified_concurrency_runtime_abi_surface": (
            build_runtime_unified_concurrency_runtime_abi_surface(results)
        ),
        "runtime_error_execution_cleanup_source_surface": (
            build_runtime_error_execution_cleanup_source_surface(results)
        ),
        "runtime_catch_filter_finalization_source_surface": (
            build_runtime_catch_filter_finalization_source_surface(results)
        ),
        "runtime_error_propagation_cleanup_semantics_surface": (
            build_runtime_error_propagation_cleanup_semantics_surface(results)
        ),
        "runtime_bridging_filter_unwind_diagnostics_surface": (
            build_runtime_bridging_filter_unwind_diagnostics_surface(results)
        ),
        "runtime_error_lowering_unwind_bridge_helper_surface": (
            build_runtime_error_lowering_unwind_bridge_helper_surface(results)
        ),
        "runtime_error_runtime_abi_cleanup_surface": (
            build_runtime_error_runtime_abi_cleanup_surface(results)
        ),
        "runtime_error_propagation_catch_cleanup_runtime_implementation_surface": (
            build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface(
                results
            )
        ),
        "runtime_object_model_realization_source_surface": (
            build_runtime_object_model_realization_source_surface(results)
        ),
        "runtime_block_arc_unified_source_surface": (
            build_runtime_block_arc_unified_source_surface(results)
        ),
        "runtime_ownership_transfer_capture_family_source_surface": (
            build_runtime_ownership_transfer_capture_family_source_surface(results)
        ),
        "runtime_block_arc_lowering_helper_surface": (
            build_runtime_block_arc_lowering_helper_surface(results)
        ),
        "runtime_block_arc_runtime_abi_surface": (
            build_runtime_block_arc_runtime_abi_surface(results)
        ),
        "runtime_property_ivar_storage_accessor_source_surface": (
            build_runtime_property_ivar_storage_accessor_source_surface(results)
        ),
        "runtime_property_atomicity_synthesis_reflection_source_surface": (
            build_runtime_property_atomicity_synthesis_reflection_source_surface(results)
        ),
        "dispatch_and_synthesized_accessor_lowering_surface": (
            build_dispatch_and_synthesized_accessor_lowering_surface(results)
        ),
        "executable_property_accessor_layout_lowering_surface": (
            build_executable_property_accessor_layout_lowering_surface(results)
        ),
        "executable_ivar_layout_emission_surface": (
            build_executable_ivar_layout_emission_surface(results)
        ),
        "executable_synthesized_accessor_property_lowering_surface": (
            build_executable_synthesized_accessor_property_lowering_surface(results)
        ),
        "runtime_realization_lowering_reflection_artifact_surface": (
            build_runtime_realization_lowering_reflection_artifact_surface(results)
        ),
        "runtime_dispatch_table_reflection_record_lowering_surface": (
            build_runtime_dispatch_table_reflection_record_lowering_surface(results)
        ),
        "runtime_cross_module_realized_metadata_replay_preservation_surface": (
            build_runtime_cross_module_realized_metadata_replay_preservation_surface(results)
        ),
        "runtime_object_model_abi_query_surface": (
            build_runtime_object_model_abi_query_surface(results)
        ),
        "runtime_realization_lookup_reflection_implementation_surface": (
            build_runtime_realization_lookup_reflection_implementation_surface(results)
        ),
        "runtime_reflection_query_surface": build_runtime_reflection_query_surface(results),
        "runtime_realization_lookup_semantics_surface": (
            build_runtime_realization_lookup_semantics_surface(results)
        ),
        "runtime_class_metaclass_protocol_realization_surface": (
            build_runtime_class_metaclass_protocol_realization_surface(results)
        ),
        "runtime_category_attachment_merged_dispatch_surface": (
            build_runtime_category_attachment_merged_dispatch_surface(results)
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface": (
            build_runtime_reflection_visibility_coherence_diagnostics_surface(results)
        ),
        "acceptance_suite_surface": build_acceptance_suite_surface(results, report_path),
        "runtime_installation_abi_surface": build_runtime_installation_abi_surface(),
        "runtime_loader_lifecycle_surface": build_runtime_loader_lifecycle_surface(results),
        "dispatch_accessor_runtime_abi_surface": {
            "contract_id": "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "proof_cases": [
                "canonical-sample-set",
                "dispatch-fast-path",
                "synthesized-accessor-runtime",
                "property-layout",
                "property-execution",
                "arc-property-helper-abi",
            ],
            "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
            "dispatch_state_snapshot_symbol": "objc3_runtime_copy_dispatch_state_for_testing",
            "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
            "property_registry_state_snapshot_symbol": "objc3_runtime_copy_property_registry_state_for_testing",
            "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
            "arc_debug_state_snapshot_symbol": "objc3_runtime_copy_arc_debug_state_for_testing",
            "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
            "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
            "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
            "weak_current_property_load_symbol": "objc3_runtime_load_weak_current_property_i32",
            "weak_current_property_store_symbol": "objc3_runtime_store_weak_current_property_i32",
            "retain_symbol": "objc3_runtime_retain_i32",
            "release_symbol": "objc3_runtime_release_i32",
            "autorelease_symbol": "objc3_runtime_autorelease_i32",
            "private_testing_surface_only": True,
            "deterministic": True,
        },
        "storage_accessor_runtime_abi_surface": {
            "contract_id": RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
            "proof_cases": [
                "dispatch-fast-path",
                "accessor-storage-lowering-metadata-surface",
                "property-accessor-layout-lowering",
                "synthesized-accessor-runtime",
                "property-layout",
                "property-execution",
                "arc-property-helper-abi",
            ],
            "property_registry_state_snapshot_symbol": "objc3_runtime_copy_property_registry_state_for_testing",
            "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
            "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
            "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
            "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
            "bind_current_property_context_symbol": "objc3_runtime_bind_current_property_context_for_testing",
            "clear_current_property_context_symbol": "objc3_runtime_clear_current_property_context_for_testing",
            "weak_current_property_load_symbol": "objc3_runtime_load_weak_current_property_i32",
            "weak_current_property_store_symbol": "objc3_runtime_store_weak_current_property_i32",
            "private_testing_surface_only": True,
            "deterministic": True,
        },
        "runtime_property_ivar_accessor_reflection_implementation_surface": {
            "contract_id": (
                RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID
            ),
            "proof_cases": [
                "property-execution",
                "property-layout",
                "storage-ownership-reflection",
            ],
            "implementation_snapshot_symbol": (
                "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing"
            ),
            "property_registry_state_snapshot_symbol": (
                "objc3_runtime_copy_property_registry_state_for_testing"
            ),
            "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
            "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
            "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
            "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
            "bind_current_property_context_symbol": (
                "objc3_runtime_bind_current_property_context_for_testing"
            ),
            "clear_current_property_context_symbol": (
                "objc3_runtime_clear_current_property_context_for_testing"
            ),
            "weak_current_property_load_symbol": (
                "objc3_runtime_load_weak_current_property_i32"
            ),
            "weak_current_property_store_symbol": (
                "objc3_runtime_store_weak_current_property_i32"
            ),
            "implementation_model": (
                "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation"
            ),
            "reflection_model": (
                "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts"
            ),
            "fail_closed_model": (
                "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis"
            ),
            "deterministic": True,
        },
    }
    report_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"runtime-acceptance: PASS ({report_path})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())


