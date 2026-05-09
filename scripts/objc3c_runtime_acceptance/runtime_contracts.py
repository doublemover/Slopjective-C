"""Runtime acceptance contract IDs, fixtures, probes, and shared boundaries."""

from __future__ import annotations

from objc3c_runtime_acceptance.c_api import *
from objc3c_runtime_acceptance.domains.probe_helpers import (
    RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.native_build import ROOT as _ROOT

TMP_ROOT = _ROOT / "tmp" / "artifacts" / "objc3c-runtime-acceptance"
REPORT_ROOT = _ROOT / "tmp" / "reports" / "runtime" / "acceptance"

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
INTEROP_HEADER_MODULE_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3"
)
RELEASE_CLAIMABLE_SURFACE_FIXTURE = "tests/tooling/fixtures/native/hello.objc3"
DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES = [
    "module.objc3-release-runtime-claim-matrix.json",
    "module.objc3-dashboard-ready-summary.json",
    "module.objc3-toolchain-runtime-ga-operations-scaffold.json",
]
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
    "npm run objc3c -- validate-runtime-architecture"
)

__all__ = [
    name
    for name, value in globals().items()
    if name.isupper() and not name.startswith("_")
]
