#!/usr/bin/env python3
"""Compile and run the live objc3 runtime acceptance workload."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
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
from objc3c_runtime_acceptance.domains.probe_helpers import RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID
from objc3c_runtime_acceptance.domains.probe_helpers import RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID
from objc3c_runtime_acceptance.domains.probe_helpers import RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID
from objc3c_runtime_acceptance.domains.probe_helpers import build_runtime_bootstrap_lowering_registration_artifact_surface
from objc3c_runtime_acceptance.domains.probe_helpers import build_runtime_bootstrap_registration_source_surface
from objc3c_runtime_acceptance.domains.probe_helpers import build_runtime_state_publication_surface
from objc3c_runtime_acceptance.domains.probe_helpers import check_runtime_probe_helper_support_case
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
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe
from objc3c_runtime_acceptance.reports import write_json_report


ROOT = Path(__file__).resolve().parents[2]
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-runtime-acceptance"
REPORT_ROOT = ROOT / "tmp" / "reports" / "runtime" / "acceptance"
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
    from objc3c_runtime_acceptance.domains import compiler_artifacts as compiler_artifacts_domain
    from objc3c_runtime_acceptance.domains import concurrency as concurrency_domain
    from objc3c_runtime_acceptance.domains import errors as errors_domain
    from objc3c_runtime_acceptance.domains import interop_packaging as interop_packaging_domain
    from objc3c_runtime_acceptance.domains import metaprogramming as metaprogramming_domain
    from objc3c_runtime_acceptance.domains import object_model as object_model_domain
    from objc3c_runtime_acceptance.domains import object_model_cases as object_model_cases_domain
    from objc3c_runtime_acceptance.domains import registration as registration_domain
    from objc3c_runtime_acceptance.domains import storage_reflection as storage_reflection_domain

    case_factories: list[tuple[str, Callable[[], CaseResult]]] = [
        ("runtime-library", lambda: object_model_cases_domain.check_runtime_library_case(clangxx, run_dir)),
        (
            "runtime-probe-helper-support",
            lambda: check_runtime_probe_helper_support_case(clangxx, run_dir),
        ),
        (
            "compile-backend-parity",
            lambda: check_compile_backend_parity_case(run_dir),
        ),
        (
            "artifact-registry-key-isolation",
            lambda: compiler_artifacts_domain.check_artifact_registry_key_isolation_case(run_dir),
        ),
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
        ("cross-module-storage-reflection-artifact-preservation", lambda: storage_reflection_domain.check_cross_module_storage_reflection_artifact_preservation_case(run_dir)),
        ("imported-runtime-packaging-replay", lambda: interop_packaging_domain.check_imported_runtime_packaging_replay_case(clangxx, run_dir)),
        ("canonical-dispatch", lambda: object_model_cases_domain.check_canonical_dispatch_case(clangxx, run_dir)),
        ("metaclass-graph-root-class", lambda: object_model_cases_domain.check_metaclass_graph_root_class_case(clangxx, run_dir)),
        ("canonical-sample-set", lambda: object_model_cases_domain.check_canonical_sample_set_case(clangxx, run_dir)),
        ("realization-lookup-reflection-runtime", lambda: object_model_cases_domain.check_realization_lookup_reflection_runtime_case(clangxx, run_dir)),
        ("live-dispatch-fast-path", lambda: object_model_cases_domain.check_live_dispatch_fast_path_case(clangxx, run_dir)),
        ("storage-ownership-reflection", lambda: storage_reflection_domain.check_storage_ownership_reflection_case(clangxx, run_dir)),
        ("property-ivar-ordering-semantics", lambda: storage_reflection_domain.check_property_ivar_ordering_semantics_case(run_dir)),
        ("accessor-storage-lowering-metadata-surface", lambda: storage_reflection_domain.check_accessor_storage_lowering_metadata_surface_case(run_dir)),
        ("property-accessor-layout-lowering", lambda: storage_reflection_domain.check_property_accessor_layout_lowering_case(run_dir)),
        ("property-reflection-accessor-compatibility-diagnostics", lambda: storage_reflection_domain.check_property_reflection_accessor_compatibility_diagnostics_case(run_dir)),
        ("property-synthesis-storage-binding-semantics", lambda: storage_reflection_domain.check_property_synthesis_storage_binding_semantics_case(run_dir)),
        ("storage-legality-semantics", lambda: storage_reflection_domain.check_storage_legality_semantics_case(run_dir)),
        ("synthesized-accessor-codegen", lambda: storage_reflection_domain.check_synthesized_accessor_codegen_case(run_dir)),
        ("synthesized-accessor-runtime", lambda: storage_reflection_domain.check_synthesized_accessor_runtime_case(clangxx, run_dir)),
        ("property-layout", lambda: storage_reflection_domain.check_property_layout_case(clangxx, run_dir)),
        ("instance-allocation-layout-runtime", lambda: storage_reflection_domain.check_instance_allocation_layout_runtime_case(clangxx, run_dir)),
        ("property-execution", lambda: storage_reflection_domain.check_property_execution_case(clangxx, run_dir)),
        ("property-reflection", lambda: storage_reflection_domain.check_property_reflection_case(clangxx, run_dir)),
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
        "runtime_state_publication_surface": build_runtime_state_publication_surface(
            PUBLIC_RUNTIME_ABI_BOUNDARY
        ),
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
            storage_reflection_domain.build_runtime_property_ivar_storage_accessor_source_surface(results)
        ),
        "runtime_property_atomicity_synthesis_reflection_source_surface": (
            storage_reflection_domain.build_runtime_property_atomicity_synthesis_reflection_source_surface(results)
        ),
        "dispatch_and_synthesized_accessor_lowering_surface": (
            storage_reflection_domain.build_dispatch_and_synthesized_accessor_lowering_surface(results)
        ),
        "executable_property_accessor_layout_lowering_surface": (
            storage_reflection_domain.build_executable_property_accessor_layout_lowering_surface(results)
        ),
        "executable_ivar_layout_emission_surface": (
            storage_reflection_domain.build_executable_ivar_layout_emission_surface(results)
        ),
        "executable_synthesized_accessor_property_lowering_surface": (
            storage_reflection_domain.build_executable_synthesized_accessor_property_lowering_surface(results)
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
