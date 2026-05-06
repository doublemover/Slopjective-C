#!/usr/bin/env python3
"""Compile and run the live objc3 runtime acceptance workload."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from time import perf_counter
from typing import Any, Callable
from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.checksums import file_sha256_hex
from objc3c_runtime_acceptance.checksums import optional_file_sha256_hex
from objc3c_runtime_acceptance.checksums import replay_key_counter
from objc3c_runtime_acceptance.checksums import sha256_text_hex
from objc3c_runtime_acceptance.commands import run
from objc3c_runtime_acceptance.native_build import ACCEPTANCE_ARTIFACT_REGISTRY
from objc3c_runtime_acceptance.native_build import COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID
from objc3c_runtime_acceptance.native_build import COMPILE_PROVENANCE_CONTRACT_ID
from objc3c_runtime_acceptance.native_build import DEFAULT_COMPILE_BACKEND
from objc3c_runtime_acceptance.native_build import DIRECT_COMPILE_BACKEND
from objc3c_runtime_acceptance.native_build import NATIVE_EXE
from objc3c_runtime_acceptance.native_build import NegativeDiagnosticExpectation
from objc3c_runtime_acceptance.native_build import RUNTIME_LIB
from objc3c_runtime_acceptance.native_build import WRAPPER_COMPILE_BACKEND
from objc3c_runtime_acceptance.native_build import compile_command
from objc3c_runtime_acceptance.native_build import compile_fixture
from objc3c_runtime_acceptance.native_build import compile_fixture_expect_failure
from objc3c_runtime_acceptance.native_build import compile_fixture_manifest_only
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs_with_args
from objc3c_runtime_acceptance.native_build import compile_fixture_with_args
from objc3c_runtime_acceptance.native_build import compile_negative_diagnostic_batch
from objc3c_runtime_acceptance.native_build import compile_output_truthfulness
from objc3c_runtime_acceptance.native_build import ensure_native_binaries
from objc3c_runtime_acceptance.native_build import find_clangxx
from objc3c_runtime_acceptance.native_build import link_fixture_executable
from objc3c_runtime_acceptance.native_build import run_fixture_compile
from objc3c_runtime_acceptance.native_build import write_compile_output_provenance
from objc3c_runtime_acceptance.progress import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.progress import get_acceptance_progress
from objc3c_runtime_acceptance.progress import repo_display_path
from objc3c_runtime_acceptance.progress import round_seconds
from objc3c_runtime_acceptance.progress import set_acceptance_progress
from objc3c_runtime_acceptance.probes import ACCEPTANCE_PROBE_RETRY_EVENTS
from objc3c_runtime_acceptance.probes import DEFAULT_PROBE_RETRIES
from objc3c_runtime_acceptance.probes import RETRYABLE_PROBE_EXIT_CODES
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe
from objc3c_runtime_acceptance.reports import write_json_report


ROOT = Path(__file__).resolve().parents[2]
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-runtime-acceptance"
REPORT_ROOT = ROOT / "tmp" / "reports" / "runtime" / "acceptance"
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
RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.package.interop.source.surface.v1"
)
RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.textual.binary.interface.parity.source.surface.v1"
)
RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.claimable.surface.residual.non.claimable.gaps.source.surface.v1"
)
RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.strict.profile.feature.claim.source.surface.v1"
)
RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.claimability.semantics.release.policy.surface.v1"
)
RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.strict.profile.claim.implementation.surface.v1"
)
RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.scaffold.retirement.deprecated.sidecar.compatibility.diagnostics.surface.v1"
)
RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.claim.publication.dashboard.schema.surface.v1"
)
RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.final.claim.publication.deprecated.path.shutdown.surface.v1"
)
RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.release.candidate.claim.abi.surface.v1"
)
RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.final.release.evidence.descaffolding.implementation.surface.v1"
)
RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.mixed.image.compatibility.interop.semantics.surface.v1"
)
RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.package.loading.module.identity.semantics.surface.v1"
)
RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.c.cpp.swift.bridge.compatibility.semantics.surface.v1"
)
RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.import.version.feature.claim.diagnostics.surface.v1"
)
RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.packaging.bridge.loader.artifact.surface.v1"
)
RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.mixed.image.package.lowering.bridge.emission.surface.v1"
)
RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.language.replay.import.surface.preservation.surface.v1"
)
RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.package.loader.bridge.abi.surface.v1"
)
RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.package.loading.interop.implementation.surface.v1"
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
RUNTIME_PUBLIC_HEADER_PATH = "native/objc3c/src/runtime/public/objc3_runtime_api.h"
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
MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE = (
    "tests/tooling/runtime/multi_image_registration_reset_replay_probe.cpp"
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
INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3"
)
INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3"
)
INTEROP_HEADER_MODULE_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/header_module_bridge_provider.objc3"
)
RELEASE_CLAIMABLE_SURFACE_FIXTURE = "tests/tooling/fixtures/native/hello.objc3"
DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES = [
    "module.objc3-release-runtime-claim-matrix.json",
    "module.objc3-dashboard-ready-summary.json",
    "module.objc3-toolchain-runtime-ga-operations-scaffold.json",
]
INTEROP_HEADER_MODULE_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3"
)
INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp"
)
RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp"
)
RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE = (
    "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp"
)
INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/header_module_bridge_generation_probe.cpp"
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
    "python -m scripts.objc3c_workflow validate-runtime-architecture"
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
PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing",
]
PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY = [
    "objc3_runtime_copy_release_candidate_evidence_state_for_testing",
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
RUNTIME_ACCEPTANCE_SUITE_CASES: dict[str, tuple[str, ...]] = {
    "full": (),
    "helpers": (
        "runtime-probe-helper-support",
    ),
    "fast": (
        "runtime-library",
        "runtime-probe-helper-support",
        "compile-backend-parity",
        "artifact-registry-key-isolation",
        "installation-lifecycle",
        "multi-image-registration-reset-replay",
        "metaprogramming-source-surface",
        "live-metaprogramming-cache-runtime-integration",
        "cross-module-runtime-package-interop-source-surface",
        "unified-concurrency-runtime-abi",
        "live-error-runtime-integration",
        "canonical-dispatch",
        "metaclass-graph-root-class",
        "live-dispatch-fast-path",
        "storage-ownership-reflection",
        "accessor-storage-lowering-metadata-surface",
        "property-layout",
        "instance-allocation-layout-runtime",
        "escaping-block-capture-legality",
        "block-arc-runtime-abi",
        "arc-property-helper",
    ),
    "diagnostics": (
        "metaprogramming-derive-property-behavior-semantics",
        "metaprogramming-macro-safety-cache-diagnostics",
        "import-version-feature-claim-diagnostics",
        "scaffold-retirement-deprecated-sidecar-compatibility-diagnostics",
        "executable-try-throw-do-catch-semantics",
        "bridging-filter-unwind-compatibility-diagnostics",
        "property-reflection-accessor-compatibility-diagnostics",
        "property-synthesis-storage-binding-semantics",
        "storage-legality-semantics",
        "escaping-block-capture-legality",
        "block-storage-arc-automation-semantics",
    ),
    "cross-module": (
        "cross-module-metaprogramming-artifact-preservation",
        "cross-module-runtime-package-interop-source-surface",
        "mixed-image-compatibility-interop-semantics",
        "c-cpp-swift-bridge-compatibility-semantics",
        "runtime-packaging-bridge-loader-artifact-surface",
        "mixed-image-package-lowering-bridge-emission",
        "cross-language-replay-import-surface-preservation",
        "live-package-loading-interop-runtime-implementation",
        "cross-module-error-metadata-replay-preservation",
        "cross-module-concurrency-actor-artifact-preservation",
        "cross-module-block-ownership-artifact-preservation",
        "cross-module-storage-reflection-artifact-preservation",
        "imported-runtime-packaging-replay",
        "multi-image-registration-reset-replay",
    ),
    "block-arc": (
        "escaping-block-capture-legality",
        "block-storage-arc-automation-semantics",
        "block-arc-runtime-abi",
        "block-helper-runtime-execution",
        "arc-property-helper",
        "cross-module-block-ownership-artifact-preservation",
    ),
    "concurrency": (
        "unified-concurrency-runtime-architecture",
        "async-task-actor-normalization-completion",
        "unified-concurrency-lowering-metadata-surface",
        "unified-concurrency-runtime-abi",
        "live-unified-concurrency-runtime-implementation",
        "cross-module-concurrency-actor-artifact-preservation",
    ),
}


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run ObjC3 runtime acceptance suites."
    )
    parser.add_argument(
        "--suite",
        choices=sorted(RUNTIME_ACCEPTANCE_SUITE_CASES),
        default="full",
        help="named runtime acceptance suite to run",
    )
    parser.add_argument(
        "--case",
        action="append",
        default=[],
        dest="cases",
        help="run one case label; may be repeated and overrides --suite",
    )
    parser.add_argument(
        "--list-suites",
        action="store_true",
        help="print suite names and case labels without running acceptance",
    )
    return parser.parse_args(argv)


def filter_case_factories(
    case_factories: list[tuple[str, Callable[[], CaseResult]]],
    *,
    selected_suite: str,
    selected_cases: list[str],
) -> list[tuple[str, Callable[[], CaseResult]]]:
    available = {label for label, _ in case_factories}
    requested = tuple(selected_cases) or RUNTIME_ACCEPTANCE_SUITE_CASES[selected_suite]
    if selected_suite == "full" and not selected_cases:
        return case_factories
    unknown = [label for label in requested if label not in available]
    if unknown:
        raise RuntimeError("unknown runtime acceptance case(s): " + ", ".join(unknown))
    requested_set = set(requested)
    return [(label, factory) for label, factory in case_factories if label in requested_set]


def check_compile_backend_parity_case(run_dir: Path) -> CaseResult:
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "synthesized_accessor_property_lowering_positive.objc3"
    )
    case_dir = run_dir / "compile-backend-parity"
    direct_dir = case_dir / "direct"
    wrapper_dir = case_dir / "wrapper"
    parity_cache_root = case_dir / "metaprogramming-cache-root"
    parity_args = [
        "--objc3-metaprogramming-cache-root",
        repo_display_path(parity_cache_root),
    ]
    direct_result, direct_backend = run_fixture_compile(
        fixture,
        direct_dir,
        backend=DIRECT_COMPILE_BACKEND,
        extra_args=parity_args,
    )
    if direct_result.returncode != 0:
        raise RuntimeError(
            "direct compile backend failed during parity check:\nSTDOUT:\n"
            + direct_result.stdout
            + "\nSTDERR:\n"
            + direct_result.stderr
        )
    shutil.rmtree(parity_cache_root, ignore_errors=True)
    wrapper_result, wrapper_backend = run_fixture_compile(
        fixture,
        wrapper_dir,
        backend=WRAPPER_COMPILE_BACKEND,
        extra_args=parity_args,
    )
    if wrapper_result.returncode != 0:
        raise RuntimeError(
            "wrapper compile backend failed during parity check:\nSTDOUT:\n"
            + wrapper_result.stdout
            + "\nSTDERR:\n"
            + wrapper_result.stderr
        )

    direct_provenance = json.loads(
        (direct_dir / "module.compile-provenance.json").read_text(encoding="utf-8")
    )
    wrapper_provenance = json.loads(
        (wrapper_dir / "module.compile-provenance.json").read_text(encoding="utf-8")
    )
    direct_truthfulness = direct_provenance.get("compile_output_truthfulness", {})
    wrapper_truthfulness = wrapper_provenance.get("compile_output_truthfulness", {})
    compared_truthfulness_fields = [
        "runtime_dispatch_symbol",
        "runtime_dispatch_declaration_count",
        "runtime_dispatch_call_count",
        "property_descriptor_count_expected",
        "property_descriptor_definition_count",
        "property_descriptor_section_present",
        "ivar_descriptor_count_expected",
        "ivar_descriptor_definition_count",
        "ivar_descriptor_section_present",
        "property_synthesis_sites_expected",
        "synthesized_accessor_definition_count",
        "current_property_helper_call_count",
        "property_descriptor_counts_match",
        "ivar_descriptor_counts_match",
        "synthesized_property_surface_matches",
        "truthful",
    ]
    for field in compared_truthfulness_fields:
        if direct_truthfulness.get(field) != wrapper_truthfulness.get(field):
            raise RuntimeError(
                "direct compile backend truthfulness drifted from wrapper for "
                f"{field}: direct={direct_truthfulness.get(field)!r} "
                f"wrapper={wrapper_truthfulness.get(field)!r}"
            )
    if (
        direct_provenance.get("artifact_set_digest_sha256")
        != wrapper_provenance.get("artifact_set_digest_sha256")
    ):
        raise RuntimeError(
            "direct compile backend artifact digest drifted from wrapper output"
        )
    if direct_provenance.get("compile_backend") != DIRECT_COMPILE_BACKEND:
        raise RuntimeError("direct compile backend did not stamp direct provenance")

    return CaseResult(
        case_id="compile-backend-parity",
        probe="direct-native-compile-plus-wrapper-contract-parity",
        fixture=repo_display_path(fixture),
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "direct_backend": direct_backend,
            "wrapper_backend": wrapper_backend,
            "artifact_set_digest_sha256": direct_provenance.get(
                "artifact_set_digest_sha256"
            ),
            "truthfulness_fields_compared": compared_truthfulness_fields,
            "provenance_contract_id": direct_provenance.get("contract_id"),
            "compile_output_truthfulness_contract_id": direct_truthfulness.get(
                "contract_id"
            ),
        },
    )


def check_artifact_registry_key_isolation_case(run_dir: Path) -> CaseResult:
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
    case_dir = run_dir / "artifact-registry-key-isolation"
    args_a = ["--objc3-bootstrap-registration-order-ordinal", "21"]
    args_b = ["--objc3-bootstrap-registration-order-ordinal", "22"]
    reuse_before = len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)
    miss_before = len(ACCEPTANCE_ARTIFACT_REGISTRY.miss_events)

    def compile_direct(
        out_dir: Path, extra_args: list[str]
    ) -> subprocess.CompletedProcess[str]:
        result, selected_backend = run_fixture_compile(
            fixture,
            out_dir,
            extra_args=extra_args,
            backend=DIRECT_COMPILE_BACKEND,
            reuse_policy="immutable-inspection",
        )
        if result.returncode != 0:
            raise RuntimeError(
                "artifact registry key isolation compile failed for "
                f"{fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
            )
        expect(
            selected_backend == DIRECT_COMPILE_BACKEND,
            "expected artifact registry key isolation to use direct-native backend",
        )
        ACCEPTANCE_ARTIFACT_REGISTRY.validate_artifacts(out_dir, "module")
        return result

    first_result = compile_direct(case_dir / "ordinal-21-producer", args_a)
    second_result = compile_direct(case_dir / "ordinal-22-distinct-args", args_b)
    reuse_after_distinct_args = len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)
    expect(
        reuse_after_distinct_args == reuse_before,
        "artifact registry reused stale outputs after bootstrap ordinal changed",
    )
    third_result = compile_direct(case_dir / "ordinal-21-consumer", args_a)
    reuse_after_same_args = len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)
    expect(
        reuse_after_same_args == reuse_before + 1,
        "artifact registry did not reuse immutable outputs when the key was unchanged",
    )
    reuse_event = ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events[-1]
    expect(
        reuse_event.get("producer_dir")
        == repo_display_path(case_dir / "ordinal-21-producer")
        and reuse_event.get("consumer_dir")
        == repo_display_path(case_dir / "ordinal-21-consumer"),
        "artifact registry reused from an unexpected producer or consumer directory",
    )

    key_a, key_payload_a = ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=args_a,
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix="module",
    )
    key_b, key_payload_b = ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=args_b,
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix="module",
    )
    import_surface_a = repo_display_path(case_dir / "surface-a.runtime-import-surface.json")
    import_surface_b = repo_display_path(case_dir / "surface-b.runtime-import-surface.json")
    key_import_a, key_payload_import_a = ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=["--objc3-import-runtime-surface", import_surface_a],
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix="module",
    )
    key_import_b, key_payload_import_b = ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=["--objc3-import-runtime-surface", import_surface_b],
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix="module",
    )
    expect(
        key_a != key_b and key_import_a != key_import_b,
        "artifact registry key did not distinguish changed args or changed import surfaces",
    )

    return CaseResult(
        case_id="artifact-registry-key-isolation",
        probe="direct-native-artifact-registry-negative-reuse-proof",
        fixture=repo_display_path(fixture),
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "producer_returncode": first_result.returncode,
            "distinct_args_returncode": second_result.returncode,
            "same_args_reuse_returncode": third_result.returncode,
            "reuse_events_before": reuse_before,
            "reuse_events_after_distinct_args": reuse_after_distinct_args,
            "reuse_events_after_same_args": reuse_after_same_args,
            "miss_events_added": len(ACCEPTANCE_ARTIFACT_REGISTRY.miss_events)
            - miss_before,
            "changed_args_key_a": key_a,
            "changed_args_key_b": key_b,
            "changed_args_payload_a": key_payload_a,
            "changed_args_payload_b": key_payload_b,
            "changed_import_surface_key_a": key_import_a,
            "changed_import_surface_key_b": key_import_b,
            "changed_import_surface_payload_a": key_payload_import_a,
            "changed_import_surface_payload_b": key_payload_import_b,
            "negative_reuse_model": (
                "changed bootstrap args and changed import-surface paths produce "
                "different immutable artifact registry keys; only exact key "
                "matches may copy producer artifacts"
            ),
        },
    )


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
            "non-authoritative test surfaces without a coupled emitted object and runtime probe",
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
            "instance-allocation-layout-runtime",
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
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
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
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
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
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
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
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
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
            "instance-allocation-layout-runtime",
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
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
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
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
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
            "instance-allocation-layout-runtime",
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
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "property_table_model": (
            "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records"
        ),
        "ivar_layout_model": (
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records"
        ),
        "accessor_binding_model": (
            "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
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
            "instance-allocation-layout-runtime",
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
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "descriptor_model": (
            "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering"
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
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
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
            "instance-allocation-layout-runtime",
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
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
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
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
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


def check_runtime_probe_helper_support_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "runtime-probe-helper-support"
    helper_probes = [
        ROOT / "tests" / "tooling" / "runtime" / "json_probe_writer_support_test.cpp",
        ROOT / "tests" / "tooling" / "runtime" / "runtime_snapshot_stabilizers_support_test.cpp",
        ROOT / "tests" / "tooling" / "runtime" / "dispatch_expectations_support_test.cpp",
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "runtime_probe_helper_output_equivalence_test.cpp",
    ]
    completed_probes: list[str] = []
    for helper_probe in helper_probes:
        helper_exe = case_dir / f"{helper_probe.stem}.exe"
        compile_probe(clangxx, helper_probe, helper_exe, [])
        run_probe(helper_exe)
        completed_probes.append(repo_display_path(helper_probe))
    return CaseResult(
        case_id="runtime-probe-helper-support",
        probe="tests/tooling/runtime/*_support_test.cpp",
        fixture=None,
        claim_class="runtime-probe-helper-tests",
        passed=True,
        summary={
            "kind": "fast-runtime-probe-helper-tests",
            "helper_probes": completed_probes,
            "bounded_mismatch_diagnostics": True,
            "representative_output_equivalence": [
                "json-field-writer-comma-and-null-output",
                "labeled-method-cache-state-output",
                "labeled-fast-path-method-cache-state-output",
                "labeled-dispatch-state-output",
            ],
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
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;retain=0;strong=1;weak=0;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=nonatomic,strong",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=currentValue;setter_available=1;setter=setCurrentValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "currentValue",
            "effective_setter_selector": "setCurrentValue:",
        },
        "copied_value_property": {
            "property_name": "copiedValue",
            "slot_index": 1,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=1;retain=0;strong=0;weak=0;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=copy,nonatomic",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=copiedValue;setter_available=1;setter=setCopiedValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "copiedValue",
            "effective_setter_selector": "setCopiedValue:",
        },
        "weak_value_property": {
            "property_name": "weakValue",
            "slot_index": 2,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;retain=0;strong=0;weak=1;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=nonatomic,weak",
            "ownership_lifetime_profile": "weak",
            "ownership_runtime_hook_profile": "objc-weak-side-table",
            "accessor_ownership_profile": "getter=weakValue;setter_available=1;setter=setWeakValue:;ownership_lifetime=weak;runtime_hook=objc-weak-side-table",
            "effective_getter_selector": "weakValue",
            "effective_setter_selector": "setWeakValue:",
        },
        "borrowed_value_property": {
            "property_name": "borrowedValue",
            "slot_index": 3,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;retain=0;strong=0;weak=0;unowned=0;unsafe_unretained=0;assign=1;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=assign",
            "ownership_lifetime_profile": "unowned-unsafe",
            "ownership_runtime_hook_profile": "objc-unowned-unsafe-direct",
            "accessor_ownership_profile": "getter=borrowedValue;setter_available=1;setter=setBorrowedValue:;ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct",
            "effective_getter_selector": "borrowedValue",
            "effective_setter_selector": "setBorrowedValue:",
        },
        "guarded_value_property": {
            "property_name": "guardedValue",
            "slot_index": 4,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;retain=0;strong=0;weak=0;unowned=1;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=unowned",
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

    negative_batch = compile_negative_diagnostic_batch(
        case_id="storage-legality-semantics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="negative-atomic-ownership",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_atomic_ownership_negative.objc3",
                expected_snippets=[
                    "atomic ownership-aware property 'value' in interface 'Widget' is unsupported until executable accessor storage semantics land"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-weak-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "runtime_backed_storage_ownership_weak_mismatch_negative.objc3",
                expected_snippets=[
                    "property ownership qualifier '__weak' conflicts with @property ownership modifier 'assign'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-unowned-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "runtime_backed_storage_ownership_unowned_mismatch_negative.objc3",
                expected_snippets=[
                    "property ownership qualifier '__unsafe_unretained' conflicts with @property ownership modifier 'unowned'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-scalar-ownership",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_scalar_ownership_negative.objc3",
                expected_snippets=[
                    "@property ownership modifier 'strong' requires an Objective-C object property"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-duplicate-getter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "accessor_duplicate_getter_negative.objc3",
                expected_snippets=[
                    "duplicate effective getter selector 'value' for properties 'token' and 'alias'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-duplicate-setter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "accessor_duplicate_setter_negative.objc3",
                expected_snippets=[
                    "duplicate effective setter selector 'setValue:' for properties 'token' and 'alias'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-readonly-setter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_readonly_setter_negative.objc3",
                expected_snippets=[
                    "readonly property 'value' in interface 'Widget' must not declare a setter modifier"
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    storage_negative_results = {
        str(entry["key"]): entry for entry in negative_batch["results"]
    }

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
            "atomic_negative_diagnostic_count": storage_negative_results[
                "negative-atomic-ownership"
            ]["diagnostic_count"],
            "weak_mismatch_diagnostic_count": storage_negative_results[
                "negative-weak-mismatch"
            ]["diagnostic_count"],
            "unowned_mismatch_diagnostic_count": storage_negative_results[
                "negative-unowned-mismatch"
            ]["diagnostic_count"],
            "scalar_ownership_negative_diagnostic_count": storage_negative_results[
                "negative-scalar-ownership"
            ]["diagnostic_count"],
            "duplicate_getter_negative_diagnostic_count": storage_negative_results[
                "negative-duplicate-getter"
            ][
                "diagnostic_count"
            ],
            "duplicate_setter_negative_diagnostic_count": storage_negative_results[
                "negative-duplicate-setter"
            ][
                "diagnostic_count"
            ],
            "readonly_setter_negative_diagnostic_count": storage_negative_results[
                "negative-readonly-setter"
            ][
                "diagnostic_count"
            ],
            "negative_diagnostics_batch": negative_batch,
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
    negative_batch = compile_negative_diagnostic_batch(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="accessor-selector-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_accessor_selector_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: effective getter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="setter-selector-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_setter_selector_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: effective setter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="reflection-attribute-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_reflection_attribute_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: reflected property attribute and ownership profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    negative_results = {str(entry["key"]): entry for entry in negative_batch["results"]}

    return CaseResult(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        probe="compile-diagnostics",
        fixture="tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "getter_selector_negative_diagnostic_count": negative_results[
                "accessor-selector-mismatch"
            ][
                "diagnostic_count"
            ],
            "setter_selector_negative_diagnostic_count": negative_results[
                "setter-selector-mismatch"
            ][
                "diagnostic_count"
            ],
            "reflection_attribute_negative_diagnostic_count": negative_results[
                "reflection-attribute-mismatch"
            ][
                "diagnostic_count"
            ],
            "negative_diagnostics_batch": negative_batch,
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
        == "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
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
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records"
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
            "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering"
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

    expect(first_alloc > 0, "expected first alloc to materialize a positive Widget instance identity")
    expect(second_alloc > 0, "expected second alloc to materialize a positive Widget instance identity")
    expect(
        first_alloc != second_alloc,
        "expected property-layout runtime to allocate distinct Widget instance identities",
    )
    expect(payload.get("set_count_result") == 0, "expected count setter dispatch to return zero")
    expect(payload.get("count_value_first") == 37, "expected count getter to observe the written value on the first alloc")
    expect(
        payload.get("count_value_second") == 0,
        "expected second alloc to observe zero-filled per-instance count storage",
    )
    expect(payload.get("set_enabled_result") == 0, "expected enabled setter dispatch to return zero")
    expect(
        payload.get("enabled_value_second") == 0,
        "expected second alloc to observe zero-filled per-instance enabled storage",
    )
    expect(payload.get("set_value_result") == 0, "expected value setter dispatch to return zero")
    expect(
        payload.get("value_result_second") == 0,
        "expected second alloc to observe zero-filled per-instance strong value storage",
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

    return CaseResult(
        case_id="property-layout",
        probe="tests/tooling/runtime/property_layout_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "allocation_mode": "distinct-instance-runtime-storage",
            "first_alloc": first_alloc,
            "second_alloc": second_alloc,
            "count_value_first": payload["count_value_first"],
            "count_value_second": payload["count_value_second"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


def check_instance_allocation_layout_runtime_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "instance-allocation-layout-runtime"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "instance_allocation_runtime_probe.cpp"
    exe_path = case_dir / "instance_allocation_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "instance allocation runtime probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    graph_state = payload.get("graph_state", {})
    widget_entry = payload.get("widget_entry", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})

    expect(
        "; runtime_instance_allocation_layout_support = "
        "contract=objc3c.runtime.instance.allocation.layout.support.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime instance allocation/layout support surface",
    )
    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in ll_text,
        "expected LLVM IR to preserve the property/layout consumption surface coupled to instance allocation",
    )
    expect("synthesized_accessor_entries=6" in ll_text, "expected instance allocation fixture to preserve six synthesized accessors")
    expect("property_descriptor_entries=" in ll_text, "expected instance allocation fixture to publish property descriptors")
    expect("ivar_layout_owner_entries=" in ll_text, "expected instance allocation fixture to publish ivar layout owners")

    runtime_surface = manifest.get("runtime_property_ivar_storage_accessor_source_surface", {})
    expect(
        runtime_surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected compile manifest to keep the source layout model coupled to runtime allocation",
    )
    synthesized_surface = manifest.get("executable_synthesized_accessor_property_lowering_surface", {})
    expect(
        synthesized_surface.get("storage_model")
        == "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals",
        "expected synthesized accessor lowering to route storage through runtime helpers",
    )

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))
    expect(first_alloc == 1048576, "expected first runtime instance identity to start at 1048576")
    expect(second_alloc == 1048577, "expected second runtime instance identity to increment deterministically")
    expect(first_alloc != second_alloc, "expected alloc to materialize distinct receiver identities")

    expect(payload.get("set_count_first") == 0, "expected first count setter dispatch to return zero")
    expect(payload.get("count_value_first") == 37, "expected first count getter to read its written value")
    expect(payload.get("count_value_second_before") == 0, "expected second count getter to start from zero-filled storage")
    expect(payload.get("set_enabled_first") == 0, "expected first enabled setter dispatch to return zero")
    expect(payload.get("enabled_value_first") == 1, "expected first enabled getter to read its written value")
    expect(payload.get("enabled_value_second") == 0, "expected second enabled getter to start from zero-filled storage")
    expect(payload.get("set_value_first") == 0, "expected first strong value setter dispatch to return zero")
    expect(payload.get("value_result_first") == 55, "expected first strong value getter to read its retained slot value")
    expect(payload.get("value_result_second_before") == 0, "expected second strong value getter to start from nil/zero storage")
    expect(payload.get("set_count_second") == 0, "expected second count setter dispatch to return zero")
    expect(payload.get("count_value_first_after_second") == 37, "expected second count write not to affect the first instance")
    expect(payload.get("count_value_second_after") == 9, "expected second count getter to read its own written value")
    expect(payload.get("set_value_second") == 0, "expected second strong value setter dispatch to return zero")
    expect(payload.get("value_result_first_after_second") == 55, "expected second value write not to affect the first instance")
    expect(payload.get("value_result_second_after") == 91, "expected second value getter to read its own written value")

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected registered image state for instance allocation probe")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected registered descriptors for instance allocation probe")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected synthesized accessor selectors in the selector table")
    expect(selector_state.get("metadata_backed_selector_count", 0) >= 6, "expected selector table to distinguish metadata-backed selectors")

    expect(graph_state.get("realized_class_count") == 1, "expected one realized Widget class")
    expect(graph_state.get("root_class_count") == 1, "expected Widget to be realized as a root class")
    expect(graph_state.get("receiver_class_binding_count") == 1, "expected one class receiver binding")
    expect(graph_state.get("live_instance_count") == 2, "expected two live runtime instances")
    expect(graph_state.get("last_allocated_receiver_identity") == second_alloc, "expected graph state to record the last allocated receiver")
    expect(graph_state.get("last_allocated_base_identity") == 1024, "expected graph state to record the Widget class base identity")
    expect(graph_state.get("last_allocated_instance_size_bytes") == 16, "expected Widget instance storage size to remain 16 bytes")
    expect(graph_state.get("last_allocated_class_name") == "Widget", "expected graph state to record the allocated class name")

    expect(widget_entry.get("found") == 1, "expected Widget realized class entry to be queryable")
    expect(widget_entry.get("base_identity") == 1024, "expected Widget base identity to remain deterministic")
    expect(widget_entry.get("is_root_class") == 1, "expected Widget fixture to be a root class")
    expect(widget_entry.get("implementation_backed") == 1, "expected Widget entry to be implementation backed")
    expect(widget_entry.get("runtime_property_accessor_count") == 3, "expected Widget to publish three runtime-backed property accessors")
    expect(widget_entry.get("runtime_instance_size_bytes") == 16, "expected Widget entry to publish 16 bytes of instance storage")
    expect(widget_entry.get("class_owner_identity") == "class:Widget", "expected Widget class owner identity")
    expect(widget_entry.get("metaclass_owner_identity") == "metaclass:Widget", "expected Widget metaclass owner identity")

    expect(count_entry.get("found") == 1 and count_entry.get("resolved") == 1, "expected count getter cache entry to resolve")
    expect(count_entry.get("dispatch_family_is_class") == 0, "expected count getter dispatch to be instance-family")
    expect(count_entry.get("normalized_receiver_identity") == 1025, "expected instance dispatch to normalize to Widget instance identity")
    expect(count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        count_entry.get("resolved_owner_identity") == "implementation:Widget::instance_method:count",
        "expected count getter cache entry to preserve synthesized owner identity",
    )
    expect(set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1, "expected setCount setter cache entry to resolve")
    expect(set_count_entry.get("dispatch_family_is_class") == 0, "expected setCount setter dispatch to be instance-family")
    expect(set_count_entry.get("normalized_receiver_identity") == 1025, "expected setter dispatch to normalize to Widget instance identity")
    expect(set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        set_count_entry.get("resolved_owner_identity") == "implementation:Widget::instance_method:setCount:",
        "expected setCount setter cache entry to preserve synthesized owner identity",
    )

    return CaseResult(
        case_id="instance-allocation-layout-runtime",
        probe="tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "allocation_mode": "distinct-instance-runtime-storage",
            "first_alloc": first_alloc,
            "second_alloc": second_alloc,
            "live_instance_count": graph_state.get("live_instance_count"),
            "instance_size_bytes": graph_state.get("last_allocated_instance_size_bytes"),
            "widget_property_accessor_count": widget_entry.get("runtime_property_accessor_count"),
            "count_first_after_second": payload["count_value_first_after_second"],
            "count_second_after": payload["count_value_second_after"],
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
           "expected lowering surface to bind lowering and runtime library dispatch symbols together")
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


def main(argv: list[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_dir = TMP_ROOT / run_id
    report_path = REPORT_ROOT / "summary.json"
    progress_path = REPORT_ROOT / "progress.json"
    run_dir.mkdir(parents=True, exist_ok=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)

    ensure_native_binaries()
    clangxx = find_clangxx()
    from objc3c_runtime_acceptance.domains import release_claims as release_claims_domain
    from objc3c_runtime_acceptance.domains import block_arc as block_arc_domain
    from objc3c_runtime_acceptance.domains import concurrency as concurrency_domain
    from objc3c_runtime_acceptance.domains import errors as errors_domain
    from objc3c_runtime_acceptance.domains import interop_packaging as interop_packaging_domain
    from objc3c_runtime_acceptance.domains import metaprogramming as metaprogramming_domain
    from objc3c_runtime_acceptance.domains import object_model as object_model_domain
    from objc3c_runtime_acceptance.domains import registration as registration_domain

    case_factories: list[tuple[str, Callable[[], CaseResult]]] = [
        ("runtime-library", lambda: object_model_domain.check_runtime_library_case(clangxx, run_dir)),
        (
            "runtime-probe-helper-support",
            lambda: check_runtime_probe_helper_support_case(clangxx, run_dir),
        ),
        ("compile-backend-parity", lambda: check_compile_backend_parity_case(run_dir)),
        ("artifact-registry-key-isolation", lambda: check_artifact_registry_key_isolation_case(run_dir)),
        ("installation-lifecycle", lambda: registration_domain.check_installation_lifecycle_case(clangxx, run_dir)),
        ("multi-image-registration-reset-replay", lambda: registration_domain.check_multi_image_registration_reset_replay_case(clangxx, run_dir)),
        ("metaprogramming-source-surface", lambda: metaprogramming_domain.check_metaprogramming_source_surface_case(run_dir)),
        ("metaprogramming-package-provenance-source-surface", lambda: metaprogramming_domain.check_metaprogramming_package_provenance_source_surface_case(run_dir)),
        ("metaprogramming-semantics", lambda: metaprogramming_domain.check_metaprogramming_semantics_case(run_dir)),
        ("metaprogramming-derive-property-behavior-semantics", lambda: metaprogramming_domain.check_metaprogramming_derive_property_behavior_semantics_case(run_dir)),
        ("metaprogramming-macro-safety-cache-diagnostics", lambda: metaprogramming_domain.check_metaprogramming_macro_safety_cache_diagnostics_case(run_dir)),
        ("metaprogramming-lowering-host-cache-surface", lambda: metaprogramming_domain.check_metaprogramming_lowering_host_cache_surface_case(run_dir)),
        ("metaprogramming-executable-lowering", lambda: metaprogramming_domain.check_metaprogramming_executable_lowering_case(clangxx, run_dir)),
        ("cross-module-metaprogramming-artifact-preservation", lambda: metaprogramming_domain.check_cross_module_metaprogramming_artifact_preservation_case(run_dir)),
        ("metaprogramming-runtime-abi-cache-surface", lambda: metaprogramming_domain.check_metaprogramming_runtime_abi_cache_surface_case(clangxx, run_dir)),
        ("live-metaprogramming-cache-runtime-integration", lambda: metaprogramming_domain.check_live_metaprogramming_cache_runtime_integration_case(clangxx, run_dir)),
        ("cross-module-runtime-package-interop-source-surface", lambda: interop_packaging_domain.check_cross_module_runtime_package_interop_source_surface_case(run_dir)),
        ("textual-binary-interface-parity-source-surface", lambda: interop_packaging_domain.check_textual_binary_interface_parity_source_surface_case(run_dir)),
        ("claimable-surface-residual-non-claimable-gaps-source-surface", lambda: release_claims_domain.check_claimable_surface_residual_non_claimable_gaps_source_surface_case(run_dir)),
        ("strict-profile-feature-claim-source-surface", lambda: release_claims_domain.check_strict_profile_feature_claim_source_surface_case(run_dir)),
        ("claimability-semantics-release-policy", lambda: release_claims_domain.check_claimability_semantics_release_policy_case(run_dir)),
        ("strict-profile-claim-implementation", lambda: release_claims_domain.check_strict_profile_claim_implementation_case(run_dir)),
        ("scaffold-retirement-deprecated-sidecar-compatibility-diagnostics", lambda: release_claims_domain.check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case(run_dir)),
        ("claim-publication-dashboard-schema-surface", lambda: release_claims_domain.check_claim_publication_dashboard_schema_surface_case(run_dir)),
        ("final-claim-publication-deprecated-path-shutdown", lambda: release_claims_domain.check_final_claim_publication_deprecated_path_shutdown_case(run_dir)),
        ("release-candidate-runtime-claim-abi", lambda: release_claims_domain.check_release_candidate_runtime_claim_abi_case(clangxx, run_dir)),
        ("final-release-evidence-descaffolding-implementation", lambda: release_claims_domain.check_final_release_evidence_descaffolding_implementation_case(clangxx, run_dir)),
        ("mixed-image-compatibility-interop-semantics", lambda: interop_packaging_domain.check_mixed_image_compatibility_interop_semantics_case(run_dir)),
        ("c-cpp-swift-bridge-compatibility-semantics", lambda: interop_packaging_domain.check_c_cpp_swift_bridge_compatibility_semantics_case(run_dir)),
        ("import-version-feature-claim-diagnostics", lambda: interop_packaging_domain.check_import_version_feature_claim_diagnostics_case(run_dir)),
        ("runtime-packaging-bridge-loader-artifact-surface", lambda: interop_packaging_domain.check_runtime_packaging_bridge_loader_artifact_surface_case(run_dir)),
        ("mixed-image-package-lowering-bridge-emission", lambda: interop_packaging_domain.check_mixed_image_package_lowering_bridge_emission_case(run_dir)),
        ("cross-language-replay-import-surface-preservation", lambda: interop_packaging_domain.check_cross_language_replay_import_surface_preservation_case(run_dir)),
        ("runtime-package-loader-bridge-abi", lambda: interop_packaging_domain.check_runtime_package_loader_bridge_abi_case(clangxx, run_dir)),
        ("live-package-loading-interop-runtime-implementation", lambda: interop_packaging_domain.check_live_package_loading_interop_runtime_implementation_case(clangxx, run_dir)),
        ("unified-concurrency-runtime-architecture", lambda: concurrency_domain.check_unified_concurrency_runtime_architecture_case(run_dir)),
        ("async-task-actor-normalization-completion", lambda: concurrency_domain.check_async_task_actor_normalization_completion_case(run_dir)),
        ("unified-concurrency-lowering-metadata-surface", lambda: concurrency_domain.check_unified_concurrency_lowering_metadata_surface_case(run_dir)),
        ("unified-concurrency-runtime-abi", lambda: concurrency_domain.check_unified_concurrency_runtime_abi_case(clangxx, run_dir)),
        ("live-unified-concurrency-runtime-implementation", lambda: concurrency_domain.check_live_unified_concurrency_runtime_implementation_case(clangxx, run_dir)),
        ("error-execution-cleanup-source", lambda: errors_domain.check_error_execution_cleanup_source_case(run_dir)),
        ("catch-filter-finalization-source", lambda: errors_domain.check_catch_filter_finalization_source_case(run_dir)),
        ("error-propagation-cleanup-semantics", lambda: errors_domain.check_error_propagation_cleanup_semantics_case(run_dir)),
        ("executable-try-throw-do-catch-semantics", lambda: errors_domain.check_executable_try_throw_do_catch_semantics_case(run_dir)),
        ("bridging-filter-unwind-compatibility-diagnostics", lambda: errors_domain.check_bridging_filter_unwind_compatibility_diagnostics_case(run_dir)),
        ("error-lowering-unwind-bridge-helper-surface", lambda: errors_domain.check_error_lowering_unwind_bridge_helper_surface_case(run_dir)),
        ("executable-throw-catch-cleanup-lowering", lambda: errors_domain.check_executable_throw_catch_cleanup_lowering_case(run_dir)),
        ("cross-module-error-metadata-replay-preservation", lambda: errors_domain.check_cross_module_error_metadata_replay_preservation_case(run_dir)),
        ("error-runtime-abi-cleanup", lambda: errors_domain.check_error_runtime_abi_cleanup_case(clangxx, run_dir)),
        ("live-error-runtime-integration", lambda: errors_domain.check_live_error_runtime_integration_case(clangxx, run_dir)),
        ("cross-module-concurrency-actor-artifact-preservation", lambda: concurrency_domain.check_cross_module_concurrency_actor_artifact_preservation_case(run_dir)),
        ("cross-module-block-ownership-artifact-preservation", lambda: check_cross_module_block_ownership_artifact_preservation_case(run_dir)),
        ("cross-module-storage-reflection-artifact-preservation", lambda: check_cross_module_storage_reflection_artifact_preservation_case(run_dir)),
        ("imported-runtime-packaging-replay", lambda: interop_packaging_domain.check_imported_runtime_packaging_replay_case(clangxx, run_dir)),
        ("canonical-dispatch", lambda: object_model_domain.check_canonical_dispatch_case(clangxx, run_dir)),
        ("metaclass-graph-root-class", lambda: object_model_domain.check_metaclass_graph_root_class_case(clangxx, run_dir)),
        ("canonical-sample-set", lambda: object_model_domain.check_canonical_sample_set_case(clangxx, run_dir)),
        ("realization-lookup-reflection-runtime", lambda: object_model_domain.check_realization_lookup_reflection_runtime_case(clangxx, run_dir)),
        ("live-dispatch-fast-path", lambda: object_model_domain.check_live_dispatch_fast_path_case(clangxx, run_dir)),
        ("storage-ownership-reflection", lambda: check_storage_ownership_reflection_case(clangxx, run_dir)),
        ("property-ivar-ordering-semantics", lambda: check_property_ivar_ordering_semantics_case(run_dir)),
        ("accessor-storage-lowering-metadata-surface", lambda: check_accessor_storage_lowering_metadata_surface_case(run_dir)),
        ("property-accessor-layout-lowering", lambda: check_property_accessor_layout_lowering_case(run_dir)),
        ("property-reflection-accessor-compatibility-diagnostics", lambda: check_property_reflection_accessor_compatibility_diagnostics_case(run_dir)),
        ("property-synthesis-storage-binding-semantics", lambda: check_property_synthesis_storage_binding_semantics_case(run_dir)),
        ("storage-legality-semantics", lambda: check_storage_legality_semantics_case(run_dir)),
        ("synthesized-accessor-codegen", lambda: check_synthesized_accessor_codegen_case(run_dir)),
        ("synthesized-accessor-runtime", lambda: check_synthesized_accessor_runtime_case(clangxx, run_dir)),
        ("property-layout", lambda: check_property_layout_case(clangxx, run_dir)),
        ("instance-allocation-layout-runtime", lambda: check_instance_allocation_layout_runtime_case(clangxx, run_dir)),
        ("property-execution", lambda: check_property_execution_case(clangxx, run_dir)),
        ("property-reflection", lambda: check_property_reflection_case(clangxx, run_dir)),
        ("escaping-block-capture-legality", lambda: block_arc_domain.check_escaping_block_capture_legality_case(run_dir)),
        ("block-storage-arc-automation-semantics", lambda: block_arc_domain.check_block_storage_arc_automation_semantics_case(run_dir)),
        ("block-arc-runtime-abi", lambda: block_arc_domain.check_block_arc_runtime_abi_case(clangxx, run_dir)),
        ("block-helper-runtime-execution", lambda: block_arc_domain.check_block_helper_runtime_execution_case(clangxx, run_dir)),
        ("arc-property-helper", lambda: block_arc_domain.check_arc_property_helper_case(clangxx, run_dir)),
    ]
    if args.list_suites:
        print(json.dumps({
            suite: list(cases) if cases else [label for label, _ in case_factories]
            for suite, cases in RUNTIME_ACCEPTANCE_SUITE_CASES.items()
        }, indent=2))
        return 0
    case_factories = filter_case_factories(
        case_factories,
        selected_suite=args.suite,
        selected_cases=list(args.cases),
    )

    acceptance_progress = RuntimeAcceptanceProgress(
        run_id=run_id,
        run_dir=run_dir,
        progress_path=progress_path,
        total_cases=len(case_factories),
    )
    set_acceptance_progress(acceptance_progress)
    print(f"runtime-acceptance-progress-log: {repo_display_path(progress_path)}", flush=True)
    results: list[CaseResult] = []
    for index, (label, factory) in enumerate(case_factories, start=1):
        case_started_at = acceptance_progress.start_case(index=index, label=label)
        try:
            result = factory()
        except BaseException as exc:
            acceptance_progress.fail_case(
                index=index,
                label=label,
                started_at=case_started_at,
                error=exc,
            )
            raise
        acceptance_progress.finish_case(
            index=index,
            label=label,
            result=result,
            started_at=case_started_at,
        )
        results.append(result)

    summary = {
        "status": "PASS",
        "run_dir": str(run_dir.relative_to(ROOT)).replace("\\", "/"),
        "clangxx": clangxx,
        "runtime_library": str(RUNTIME_LIB.relative_to(ROOT)).replace("\\", "/"),
        "case_count": len(results),
        "selected_suite": args.suite,
        "selected_cases": list(args.cases),
        "available_suites": {
            suite: list(cases) if cases else "all"
            for suite, cases in RUNTIME_ACCEPTANCE_SUITE_CASES.items()
        },
        "default_compile_backend": DEFAULT_COMPILE_BACKEND,
        "direct_compile_backend": DIRECT_COMPILE_BACKEND,
        "wrapper_compile_backend": WRAPPER_COMPILE_BACKEND,
        "probe_retry_policy": {
            "contract_id": "objc3c.runtime.acceptance.probe.retry.policy.v1",
            "default_probe_retries": DEFAULT_PROBE_RETRIES,
            "retryable_exit_codes": sorted(RETRYABLE_PROBE_EXIT_CODES),
            "fail_closed": True,
        },
        "probe_retry_events": ACCEPTANCE_PROBE_RETRY_EVENTS,
        "progress_report_path": repo_display_path(progress_path),
        "timing": acceptance_progress.final_summary(),
        "artifact_registry": ACCEPTANCE_ARTIFACT_REGISTRY.summary(),
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
            registration_domain.build_runtime_multi_image_startup_ordering_source_surface(results)
        ),
        "runtime_metaprogramming_source_surface": (
            metaprogramming_domain.build_runtime_metaprogramming_source_surface(results)
        ),
        "runtime_metaprogramming_package_provenance_source_surface": (
            metaprogramming_domain.build_runtime_metaprogramming_package_provenance_source_surface(results)
        ),
        "runtime_metaprogramming_semantics_surface": (
            metaprogramming_domain.build_runtime_metaprogramming_semantics_surface(results)
        ),
        "runtime_metaprogramming_lowering_host_cache_surface": (
            metaprogramming_domain.build_runtime_metaprogramming_lowering_host_cache_surface(results)
        ),
        "runtime_cross_module_metaprogramming_artifact_preservation_surface": (
            metaprogramming_domain.build_runtime_cross_module_metaprogramming_artifact_preservation_surface(
                results
            )
        ),
        "runtime_metaprogramming_runtime_abi_cache_surface": (
            metaprogramming_domain.build_runtime_metaprogramming_runtime_abi_cache_surface(results)
        ),
        "runtime_metaprogramming_cache_runtime_integration_implementation_surface": (
            metaprogramming_domain.build_runtime_metaprogramming_cache_runtime_integration_implementation_surface(
                results
            )
        ),
        "runtime_cross_module_package_interop_source_surface": (
            interop_packaging_domain.build_runtime_cross_module_package_interop_source_surface(results)
        ),
        "runtime_textual_binary_interface_parity_source_surface": (
            interop_packaging_domain.build_runtime_textual_binary_interface_parity_source_surface(results)
        ),
        "runtime_claimable_surface_residual_non_claimable_gaps_source_surface": (
            release_claims_domain.build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface(
                results
            )
        ),
        "runtime_strict_profile_feature_claim_source_surface": (
            release_claims_domain.build_runtime_strict_profile_feature_claim_source_surface(results)
        ),
        "runtime_claimability_semantics_release_policy_surface": (
            release_claims_domain.build_runtime_claimability_semantics_release_policy_surface(results)
        ),
        "runtime_strict_profile_claim_implementation_surface": (
            release_claims_domain.build_runtime_strict_profile_claim_implementation_surface(results)
        ),
        "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface": (
            release_claims_domain.build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface(
                results
            )
        ),
        "runtime_claim_publication_dashboard_schema_surface": (
            release_claims_domain.build_runtime_claim_publication_dashboard_schema_surface(results)
        ),
        "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
            release_claims_domain.build_runtime_final_claim_publication_deprecated_path_shutdown_surface(
                results
            )
        ),
        "runtime_release_candidate_claim_abi_surface": (
            release_claims_domain.build_runtime_release_candidate_claim_abi_surface(results)
        ),
        "runtime_final_release_evidence_descaffolding_implementation_surface": (
            release_claims_domain.build_runtime_final_release_evidence_descaffolding_implementation_surface(
                results
            )
        ),
        "runtime_mixed_image_compatibility_interop_semantics_surface": (
            interop_packaging_domain.build_runtime_mixed_image_compatibility_interop_semantics_surface(results)
        ),
        "runtime_package_loading_module_identity_semantics_surface": (
            interop_packaging_domain.build_runtime_package_loading_module_identity_semantics_surface(results)
        ),
        "runtime_c_cpp_swift_bridge_compatibility_semantics_surface": (
            interop_packaging_domain.build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface(results)
        ),
        "runtime_import_version_feature_claim_diagnostics_surface": (
            interop_packaging_domain.build_runtime_import_version_feature_claim_diagnostics_surface(results)
        ),
        "runtime_packaging_bridge_loader_artifact_surface": (
            interop_packaging_domain.build_runtime_packaging_bridge_loader_artifact_surface(results)
        ),
        "runtime_mixed_image_package_lowering_bridge_emission_surface": (
            interop_packaging_domain.build_runtime_mixed_image_package_lowering_bridge_emission_surface(results)
        ),
        "runtime_cross_language_replay_import_surface_preservation_surface": (
            interop_packaging_domain.build_runtime_cross_language_replay_import_surface_preservation_surface(
                results
            )
        ),
        "runtime_package_loader_bridge_abi_surface": (
            interop_packaging_domain.build_runtime_package_loader_bridge_abi_surface(results)
        ),
        "runtime_package_loading_interop_implementation_surface": (
            interop_packaging_domain.build_runtime_package_loading_interop_implementation_surface(results)
        ),
        "runtime_unified_concurrency_source_surface": (
            concurrency_domain.build_runtime_unified_concurrency_source_surface(results)
        ),
        "runtime_async_task_actor_normalization_completion_surface": (
            concurrency_domain.build_runtime_async_task_actor_normalization_completion_surface(results)
        ),
        "runtime_unified_concurrency_lowering_metadata_surface": (
            concurrency_domain.build_runtime_unified_concurrency_lowering_metadata_surface(results)
        ),
        "runtime_unified_concurrency_runtime_abi_surface": (
            concurrency_domain.build_runtime_unified_concurrency_runtime_abi_surface(results)
        ),
        "runtime_error_execution_cleanup_source_surface": (
            errors_domain.build_runtime_error_execution_cleanup_source_surface(results)
        ),
        "runtime_catch_filter_finalization_source_surface": (
            errors_domain.build_runtime_catch_filter_finalization_source_surface(results)
        ),
        "runtime_error_propagation_cleanup_semantics_surface": (
            errors_domain.build_runtime_error_propagation_cleanup_semantics_surface(results)
        ),
        "runtime_bridging_filter_unwind_diagnostics_surface": (
            errors_domain.build_runtime_bridging_filter_unwind_diagnostics_surface(results)
        ),
        "runtime_error_lowering_unwind_bridge_helper_surface": (
            errors_domain.build_runtime_error_lowering_unwind_bridge_helper_surface(results)
        ),
        "runtime_error_runtime_abi_cleanup_surface": (
            errors_domain.build_runtime_error_runtime_abi_cleanup_surface(results)
        ),
        "runtime_error_propagation_catch_cleanup_runtime_implementation_surface": (
            errors_domain.build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface(
                results
            )
        ),
        "runtime_object_model_realization_source_surface": (
            object_model_domain.build_runtime_object_model_realization_source_surface(results)
        ),
        "runtime_block_arc_unified_source_surface": (
            block_arc_domain.build_runtime_block_arc_unified_source_surface(results)
        ),
        "runtime_ownership_transfer_capture_family_source_surface": (
            block_arc_domain.build_runtime_ownership_transfer_capture_family_source_surface(results)
        ),
        "runtime_block_arc_lowering_helper_surface": (
            block_arc_domain.build_runtime_block_arc_lowering_helper_surface(results)
        ),
        "runtime_block_arc_runtime_abi_surface": (
            block_arc_domain.build_runtime_block_arc_runtime_abi_surface(results)
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
            object_model_domain.build_runtime_realization_lowering_reflection_artifact_surface(results)
        ),
        "runtime_dispatch_table_reflection_record_lowering_surface": (
            object_model_domain.build_runtime_dispatch_table_reflection_record_lowering_surface(results)
        ),
        "runtime_cross_module_realized_metadata_replay_preservation_surface": (
            object_model_domain.build_runtime_cross_module_realized_metadata_replay_preservation_surface(results)
        ),
        "runtime_object_model_abi_query_surface": (
            object_model_domain.build_runtime_object_model_abi_query_surface(results)
        ),
        "runtime_realization_lookup_reflection_implementation_surface": (
            object_model_domain.build_runtime_realization_lookup_reflection_implementation_surface(results)
        ),
        "runtime_reflection_query_surface": object_model_domain.build_runtime_reflection_query_surface(results),
        "runtime_realization_lookup_semantics_surface": (
            object_model_domain.build_runtime_realization_lookup_semantics_surface(results)
        ),
        "runtime_class_metaclass_protocol_realization_surface": (
            object_model_domain.build_runtime_class_metaclass_protocol_realization_surface(results)
        ),
        "runtime_category_attachment_merged_dispatch_surface": (
            object_model_domain.build_runtime_category_attachment_merged_dispatch_surface(results)
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface": (
            object_model_domain.build_runtime_reflection_visibility_coherence_diagnostics_surface(results)
        ),
        "acceptance_suite_surface": build_acceptance_suite_surface(results, report_path),
        "runtime_installation_abi_surface": registration_domain.build_runtime_installation_abi_surface(),
        "runtime_loader_lifecycle_surface": registration_domain.build_runtime_loader_lifecycle_surface(results),
        "dispatch_accessor_runtime_abi_surface": {
            "contract_id": "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "proof_cases": [
                "canonical-sample-set",
                "dispatch-fast-path",
                "synthesized-accessor-runtime",
                "property-layout",
                "instance-allocation-layout-runtime",
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
                "instance-allocation-layout-runtime",
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
                "instance-allocation-layout-runtime",
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
    write_json_report(progress_path, acceptance_progress.final_summary())
    write_json_report(report_path, summary)
    set_acceptance_progress(None)
    print(f"runtime-acceptance: PASS ({report_path})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
