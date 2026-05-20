"""Object Model realization acceptance case catalog data."""

from __future__ import annotations

from .probe_helpers import RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID
from ..runtime_contract_interop import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
)
from ..runtime_contract_object_model import (
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
)
from ..runtime_contract_registration import MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE

EXPORTED_CASE_NAMES = (
    "build_runtime_object_model_realization_source_surface",
    "build_runtime_realization_lowering_reflection_artifact_surface",
    "build_runtime_dispatch_table_reflection_record_lowering_surface",
    "build_runtime_cross_module_realized_metadata_replay_preservation_surface",
)

REALIZATION_STANDARD_COMPILE_ARTIFACTS = (
    "<emit-prefix>.obj",
    "<emit-prefix>.ll",
    "<emit-prefix>.manifest.json",
    "<emit-prefix>.runtime-registration-manifest.json",
    "<emit-prefix>.runtime-registration-descriptor.json",
)
REALIZATION_PROVENANCE_COMPILE_ARTIFACTS = (
    *REALIZATION_STANDARD_COMPILE_ARTIFACTS,
    "<emit-prefix>.compile-provenance.json",
)
REALIZED_METADATA_REPLAY_COMPILE_ARTIFACTS = (
    "<emit-prefix>.obj",
    "<emit-prefix>.runtime-import-surface.json",
    "<emit-prefix>.runtime-registration-manifest.json",
    "<emit-prefix>.cross-module-runtime-link-plan.json",
    "<emit-prefix>.cross-module-runtime-linker-options.rsp",
)

OBJECT_MODEL_REALIZATION_SOURCE_CASE_IDS = frozenset(
    {
        "imported-runtime-packaging-replay",
        "multi-image-registration-reset-replay",
        "canonical-dispatch",
        "metaclass-graph-root-class",
        "canonical-sample-set",
        "dispatch-fast-path",
    }
)
REALIZATION_LOWERING_REFLECTION_ARTIFACT_CASE_IDS = frozenset(
    {
        "canonical-dispatch",
        "canonical-sample-set",
        "property-reflection",
        "property-execution",
        "storage-ownership-reflection",
    }
)
DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_CASE_IDS = frozenset(
    {
        "canonical-dispatch",
        "canonical-sample-set",
        "dispatch-fast-path",
        "property-execution",
    }
)
CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_CASE_IDS = frozenset(
    {
        "imported-runtime-packaging-replay",
        "multi-image-registration-reset-replay",
    }
)

OBJECT_MODEL_REALIZATION_SOURCE_CONTRACT_IDS = (
    "objc3c.executable.realization.records.v1",
    "objc3c.runtime.class.realization.freeze.v1",
    "objc3c.runtime.metaclass.graph.root.class.baseline.v1",
    "objc3c.runtime.category.attachment.protocol.conformance.v1",
    "objc3c.runtime.canonical.runnable.object.sample.support.v1",
)
REALIZATION_LOWERING_REFLECTION_ARTIFACT_CONTRACT_IDS = (
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
    "objc3c.executable.realization.records.v1",
    "objc3c.runtime.property.metadata.reflection.v1",
    "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
)
DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_CONTRACT_IDS = (
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
    "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
    "objc3c.method.dispatch.selector.thunk.lowering.v1",
    "objc3c.executable.realization.records.v1",
)
CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_CONTRACT_IDS = (
    "objc3c.cross.module.runtime.packaging.link.plan.v1",
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
)

PRIVATE_OBJECT_MODEL_QUERY_BOUNDARY = (
    "objc3_runtime_copy_realized_class_graph_state_for_testing",
    "objc3_runtime_copy_realized_class_entry_for_testing",
    "objc3_runtime_copy_instance_entry_for_testing",
    "objc3_runtime_copy_protocol_conformance_query_for_testing",
)
PRIVATE_REFLECTION_ARTIFACT_QUERY_BOUNDARY = (
    "objc3_runtime_copy_property_registry_state_for_testing",
    "objc3_runtime_copy_property_entry_for_testing",
)

OBJECT_MODEL_REALIZATION_SOURCE_FIXTURES = (
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    "tests/tooling/fixtures/native/metaclass_graph_root_class_library.objc3",
    "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
    "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
    "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
)
OBJECT_MODEL_REALIZATION_SOURCE_PROBES = (
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    "tests/tooling/runtime/metaclass_graph_root_class_probe.cpp",
    "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
    "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
    "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
)

REALIZATION_LOWERING_REFLECTION_ARTIFACT_FIXTURES = (
    "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
    "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
    "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
    "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
    "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
)
REALIZATION_LOWERING_REFLECTION_ARTIFACT_PROBES = (
    "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
    "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
    "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
    "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
    "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
)

DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_FIXTURES = (
    "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
    "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
    "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
    "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
)
DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_PROBES = (
    "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
    "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
    "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
    "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
)

CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_FIXTURES = (
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
)
CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_PROBES = (
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
)

LOWERING_ARTIFACT_BOUNDARY_MODEL = (
    "compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts"
)
REFLECTION_ARTIFACT_HANDOFF_MODEL = (
    "property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs"
)
DISPATCH_TABLE_LOWERING_MODEL = (
    "selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts"
)
REFLECTION_RECORD_LOWERING_MODEL = (
    "realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts"
)
REALIZED_METADATA_REPLAY_PRESERVATION_MODEL = (
    "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests"
)

__all__ = [
    "CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_CASE_IDS",
    "CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_CONTRACT_IDS",
    "CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_FIXTURES",
    "CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_PROBES",
    "DISPATCH_TABLE_LOWERING_MODEL",
    "DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_CASE_IDS",
    "DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_CONTRACT_IDS",
    "DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_FIXTURES",
    "DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_PROBES",
    "EXPORTED_CASE_NAMES",
    "LOWERING_ARTIFACT_BOUNDARY_MODEL",
    "OBJECT_MODEL_REALIZATION_SOURCE_CASE_IDS",
    "OBJECT_MODEL_REALIZATION_SOURCE_CONTRACT_IDS",
    "OBJECT_MODEL_REALIZATION_SOURCE_FIXTURES",
    "OBJECT_MODEL_REALIZATION_SOURCE_PROBES",
    "PRIVATE_OBJECT_MODEL_QUERY_BOUNDARY",
    "PRIVATE_REFLECTION_ARTIFACT_QUERY_BOUNDARY",
    "REALIZATION_LOWERING_REFLECTION_ARTIFACT_CASE_IDS",
    "REALIZATION_LOWERING_REFLECTION_ARTIFACT_CONTRACT_IDS",
    "REALIZATION_LOWERING_REFLECTION_ARTIFACT_FIXTURES",
    "REALIZATION_LOWERING_REFLECTION_ARTIFACT_PROBES",
    "REALIZATION_PROVENANCE_COMPILE_ARTIFACTS",
    "REALIZATION_STANDARD_COMPILE_ARTIFACTS",
    "REALIZED_METADATA_REPLAY_COMPILE_ARTIFACTS",
    "REALIZED_METADATA_REPLAY_PRESERVATION_MODEL",
    "REFLECTION_ARTIFACT_HANDOFF_MODEL",
    "REFLECTION_RECORD_LOWERING_MODEL",
    "RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID",
    "RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID",
    "RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID",
    "RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID",
]
