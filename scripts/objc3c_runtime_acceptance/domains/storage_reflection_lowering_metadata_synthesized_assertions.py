"""Synthesized accessor metadata assertions for storage/reflection lowering."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_property_assertions import (
    expect_property_lowering,
)
from objc3c_runtime_acceptance.expectation_matching import expect

from ..runtime_contract_storage_reflection import (
    DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)


def assert_synthesized_accessor_lowering_metadata_surface(
    synthesized_manifest: dict[str, Any],
    synthesized_ll: str,
) -> dict[str, Any]:
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
        synthesized_lowering_surface.get("compile_manifest_artifact")
        == "module.manifest.json",
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
    _assert_synthesized_accessor_counts(synthesized_lowering_surface)
    _assert_synthesized_accessor_symbols(synthesized_lowering_surface)
    expect(
        "getter_definitions=3" in synthesized_ll
        and "setter_definitions=3" in synthesized_ll
        and "read_current_property_calls=3" in synthesized_ll
        and "write_current_property_calls=2" in synthesized_ll
        and "exchange_current_property_calls=1" in synthesized_ll,
        "expected synthesized accessor lowering metadata fixture LLVM IR to agree with the published lowering counts",
    )
    _assert_synthesized_property_lowering_rows(synthesized_manifest)
    return synthesized_lowering_surface


def _assert_synthesized_accessor_counts(surface: dict[str, Any]) -> None:
    expect(
        surface.get("synthesized_accessor_owner_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered property owners",
    )
    expect(
        surface.get("synthesized_getter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered getters",
    )
    expect(
        surface.get("synthesized_setter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered setters",
    )
    expect(
        surface.get("current_property_read_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three current-property read entries",
    )
    expect(
        surface.get("current_property_write_entries") == 2,
        "expected synthesized accessor lowering metadata fixture to publish two current-property write entries",
    )
    expect(
        surface.get("current_property_exchange_entries") == 1,
        "expected synthesized accessor lowering metadata fixture to publish one current-property exchange entry",
    )
    expect(
        surface.get("weak_current_property_load_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-load entries",
    )
    expect(
        surface.get("weak_current_property_store_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-store entries",
    )


def _assert_synthesized_accessor_symbols(surface: dict[str, Any]) -> None:
    expect(
        surface.get("current_property_read_symbol")
        == "objc3_runtime_read_current_property_i32",
        "expected lowering surface to publish the canonical current-property read symbol",
    )
    expect(
        surface.get("current_property_write_symbol")
        == "objc3_runtime_write_current_property_i32",
        "expected lowering surface to publish the canonical current-property write symbol",
    )
    expect(
        surface.get("current_property_exchange_symbol")
        == "objc3_runtime_exchange_current_property_i32",
        "expected lowering surface to publish the canonical current-property exchange symbol",
    )
    expect(
        surface.get("weak_current_property_load_symbol")
        == "objc3_runtime_load_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property load symbol",
    )
    expect(
        surface.get("weak_current_property_store_symbol")
        == "objc3_runtime_store_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property store symbol",
    )


def _assert_synthesized_property_lowering_rows(
    synthesized_manifest: dict[str, Any],
) -> None:
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


__all__ = ["assert_synthesized_accessor_lowering_metadata_surface"]
