"""Object Model dispatch fast-path linked-runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    ROOT,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = ["check_live_dispatch_fast_path_case"]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


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
    expect(payload.get("strict_error_entry_status") == 0, "expected strict error method-cache entry lookup to succeed")
    expect(payload.get("implicit_value") == 3, "expected implicit direct call to remain direct")
    expect(payload.get("explicit_value") == 5, "expected explicit direct call to remain direct")
    expect(payload.get("mixed_first") == 12 and payload.get("mixed_second") == 12,
           "expected mixed dispatch fixture to execute through the live runtime")
    expect(payload.get("strict_error_first") == payload.get("strict_error_expected") == payload.get("strict_error_second"),
           "expected strict dispatch error to stay deterministic across cache miss/hit")
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
    expect(payload.get("mixed_first_state_last_dispatch_strict_error") == 0,
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
    expect(payload.get("mixed_second_state_last_dispatch_strict_error") == 0,
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
    expect(payload.get("strict_error_first_state_last_dispatch_used_cache") == 0,
           "expected first missingDispatch: call to miss the cache")
    expect(payload.get("strict_error_first_state_last_dispatch_used_fast_path") == 0,
           "expected first missingDispatch: call to avoid the fast path")
    expect(payload.get("strict_error_first_state_last_dispatch_resolved_live_method") == 0,
           "did not expect first missingDispatch: call to resolve live")
    expect(payload.get("strict_error_first_state_last_dispatch_strict_error") == 1,
           "expected first missingDispatch: call to fall back")
    expect(payload.get("strict_error_first_dispatch_state_status") == 0,
           "expected first missingDispatch: call to publish dispatch state")
    expect(payload.get("strict_error_first_dispatch_state_last_dispatch_path") == "slow-path-error",
           "expected first missingDispatch: call to report slow-path strict dispatch error")
    expect(payload.get("strict_error_first_dispatch_state_last_implementation_kind") == "strict-dispatch-error",
           "expected first missingDispatch: call to report strict dispatch error status")
    expect(payload.get("strict_error_second_state_last_dispatch_used_cache") == 1,
           "expected repeated missingDispatch: call to hit the strict error cache entry")
    expect(payload.get("strict_error_second_state_last_dispatch_used_fast_path") == 0,
           "expected repeated missingDispatch: call to stay off the fast path")
    expect(payload.get("strict_error_second_state_last_dispatch_strict_error") == 1,
           "expected repeated missingDispatch: call to remain a strict dispatch error")
    expect(payload.get("strict_error_second_dispatch_state_status") == 0,
           "expected repeated missingDispatch: call to publish dispatch state")
    expect(payload.get("strict_error_second_dispatch_state_last_dispatch_path") == "cache-hit-error",
           "expected repeated missingDispatch: call to report cached strict dispatch error")
    expect(payload.get("strict_error_second_dispatch_state_last_implementation_kind") == "strict-dispatch-error",
           "expected repeated missingDispatch: call to report cached strict dispatch error status")
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
            "strict_error_first_dispatch_path": payload.get("strict_error_first_dispatch_state_last_dispatch_path"),
            "strict_error_first_implementation_kind": payload.get("strict_error_first_dispatch_state_last_implementation_kind"),
            "strict_error_second_dispatch_path": payload.get("strict_error_second_dispatch_state_last_dispatch_path"),
            "strict_error_second_implementation_kind": payload.get("strict_error_second_dispatch_state_last_implementation_kind"),
            "mixed_first_live_dispatch_count": payload.get("mixed_first_state_live_dispatch_count"),
            "mixed_second_live_dispatch_count": payload.get("mixed_second_state_live_dispatch_count"),
            "strict_error_first_strict_dispatch_error_count": payload.get("strict_error_first_state_strict_dispatch_error_count"),
            "strict_error_second_strict_dispatch_error_count": payload.get("strict_error_second_state_strict_dispatch_error_count"),
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
