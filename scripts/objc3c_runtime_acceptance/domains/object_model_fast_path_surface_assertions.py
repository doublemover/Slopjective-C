"""Dispatch fast-path compile artifact assertions."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect

from ..runtime_contract_storage_reflection import RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID


def assert_dispatch_fast_path_compile_surfaces(
    ll_text: str,
    manifest: dict[str, Any],
    registration_manifest: dict[str, Any],
) -> None:
    expect(
        "; method_dispatch_and_selector_thunk_lowering_surface = "
        "contract_id=objc3c.method.dispatch.selector.thunk.lowering.v1"
        in ll_text,
        "expected LLVM IR to publish authoritative method dispatch and selector thunk lowering surface",
    )
    expect("direct_dispatch_call_sites=5" in ll_text,
           "expected mixed dispatch fixture to emit five direct dispatch calls")
    expect("runtime_dispatch_call_sites=0" in ll_text,
           "expected mixed dispatch fixture to route eligible i32 sends through cache-aware dispatch")
    expect("cache_aware_dispatch_call_sites=1" in ll_text,
           "expected mixed dispatch fixture to emit one cache-aware runtime dispatch call")
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
    expect("call { i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr } @objc3_runtime_cache_aware_dispatch_i32_checked(" in ll_text,
           "expected dynamicEscape lowering to retain the checked live runtime dispatch call")
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


__all__ = ["assert_dispatch_fast_path_compile_surfaces"]
