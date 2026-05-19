"""Synthesized accessor lowering metadata assertion catalog."""

from __future__ import annotations

from objc3c_runtime_acceptance.runtime_contract_storage_reflection import (
    DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)

from .data import SynthesizedPropertyLoweringRow
from .data import SurfaceValueExpectation


LOWERING_SURFACE_KEY = "dispatch_and_synthesized_accessor_lowering_surface"
LOWERING_SURFACE_TYPE_MESSAGE = (
    "expected synthesized accessor lowering metadata fixture to publish the "
    "lowering surface"
)
LOWERING_SURFACE_ARTIFACT_MESSAGE = (
    "expected lowering surface to couple back to the emitted object and LLVM IR "
    "artifacts"
)
LOWERING_REQUIREMENTS_MESSAGE = (
    "expected lowering surface to require the coupled registration manifest, "
    "real compile output, and linked runtime probes"
)
LLVM_COUNT_FRAGMENT_MESSAGE = (
    "expected synthesized accessor lowering metadata fixture LLVM IR to agree "
    "with the published lowering counts"
)

LOWERING_SURFACE_METADATA_EXPECTATIONS = (
    SurfaceValueExpectation(
        "contract_id",
        DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        "expected synthesized accessor lowering metadata fixture to preserve the lowering surface contract id",
    ),
    SurfaceValueExpectation(
        "compile_manifest_artifact",
        "module.manifest.json",
        "expected lowering surface to couple back to the compile manifest artifact",
    ),
    SurfaceValueExpectation(
        "registration_manifest_artifact",
        "module.runtime-registration-manifest.json",
        "expected lowering surface to couple back to the runtime registration manifest artifact",
    ),
    SurfaceValueExpectation(
        "runtime_property_ivar_storage_accessor_source_surface_contract_id",
        RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected lowering surface to point back at the runtime property/ivar storage source surface",
    ),
    SurfaceValueExpectation(
        "storage_accessor_runtime_abi_surface_contract_id",
        RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "expected lowering surface to point at the storage/accessor runtime ABI surface",
    ),
    SurfaceValueExpectation(
        "lowering_contract_source_path",
        "native/objc3c/src/lower/objc3_lowering_contract.h",
        "expected lowering surface to publish the lowering contract source path",
    ),
    SurfaceValueExpectation(
        "ir_emitter_source_path",
        "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "expected lowering surface to publish the IR emitter source path",
    ),
    SurfaceValueExpectation(
        "frontend_artifacts_source_path",
        "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        "expected lowering surface to publish the frontend artifacts source path",
    ),
    SurfaceValueExpectation(
        "runtime_source_path",
        "native/objc3c/src/runtime/objc3_runtime.cpp",
        "expected lowering surface to publish the runtime source path",
    ),
    SurfaceValueExpectation(
        "accessor_storage_lowering_metadata_model",
        "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path",
        "expected lowering surface to publish the accessor-storage metadata model",
    ),
    SurfaceValueExpectation(
        "accessor_storage_lowering_helper_selection_model",
        "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers",
        "expected lowering surface to publish the helper-selection model",
    ),
    SurfaceValueExpectation(
        "authoritative_fixture_paths",
        [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "expected lowering surface to publish the authoritative fixture set",
    ),
    SurfaceValueExpectation(
        "authoritative_probe_paths",
        [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "expected lowering surface to publish the authoritative probe set",
    ),
    SurfaceValueExpectation(
        "explicit_non_goals",
        [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-lowering-proof",
        ],
        "expected lowering surface to publish explicit non-goals",
    ),
)

SYNTHESIZED_ACCESSOR_COUNT_EXPECTATIONS = (
    SurfaceValueExpectation(
        "synthesized_accessor_owner_entries",
        3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered property owners",
    ),
    SurfaceValueExpectation(
        "synthesized_getter_entries",
        3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered getters",
    ),
    SurfaceValueExpectation(
        "synthesized_setter_entries",
        3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered setters",
    ),
    SurfaceValueExpectation(
        "current_property_read_entries",
        3,
        "expected synthesized accessor lowering metadata fixture to publish three current-property read entries",
    ),
    SurfaceValueExpectation(
        "current_property_write_entries",
        2,
        "expected synthesized accessor lowering metadata fixture to publish two current-property write entries",
    ),
    SurfaceValueExpectation(
        "current_property_exchange_entries",
        1,
        "expected synthesized accessor lowering metadata fixture to publish one current-property exchange entry",
    ),
    SurfaceValueExpectation(
        "weak_current_property_load_entries",
        0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-load entries",
    ),
    SurfaceValueExpectation(
        "weak_current_property_store_entries",
        0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-store entries",
    ),
)

SYNTHESIZED_ACCESSOR_SYMBOL_EXPECTATIONS = (
    SurfaceValueExpectation(
        "current_property_read_symbol",
        "objc3_runtime_read_current_property_i32",
        "expected lowering surface to publish the canonical current-property read symbol",
    ),
    SurfaceValueExpectation(
        "current_property_write_symbol",
        "objc3_runtime_write_current_property_i32",
        "expected lowering surface to publish the canonical current-property write symbol",
    ),
    SurfaceValueExpectation(
        "current_property_exchange_symbol",
        "objc3_runtime_exchange_current_property_i32",
        "expected lowering surface to publish the canonical current-property exchange symbol",
    ),
    SurfaceValueExpectation(
        "weak_current_property_load_symbol",
        "objc3_runtime_load_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property load symbol",
    ),
    SurfaceValueExpectation(
        "weak_current_property_store_symbol",
        "objc3_runtime_store_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property store symbol",
    ),
)

SYNTHESIZED_LL_COUNT_FRAGMENTS = (
    "getter_definitions=3",
    "setter_definitions=3",
    "read_current_property_calls=3",
    "write_current_property_calls=2",
    "exchange_current_property_calls=1",
)

SYNTHESIZED_PROPERTY_LOWERING_ROWS = (
    SynthesizedPropertyLoweringRow("class-interface", "Widget", "count", False, "", ""),
    SynthesizedPropertyLoweringRow(
        "class-implementation",
        "Widget",
        "count",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    ),
    SynthesizedPropertyLoweringRow(
        "class-implementation",
        "Widget",
        "enabled",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    ),
    SynthesizedPropertyLoweringRow(
        "class-implementation",
        "Widget",
        "value",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    ),
)
