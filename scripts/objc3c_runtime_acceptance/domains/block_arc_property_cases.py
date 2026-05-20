"""Block/ARC property helper runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

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
    reference_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "reference_counting_weak_autoreleasepool_positive.objc3"
    )
    reference_obj_path = compile_fixture(
        reference_fixture, case_dir / "reference-counting-compile"
    )
    reference_probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "reference_counting_weak_autoreleasepool_probe.cpp"
    )
    reference_exe_path = (
        case_dir / "reference_counting_weak_autoreleasepool_probe.exe"
    )
    compile_probe(clangxx, reference_probe, reference_exe_path, [reference_obj_path])
    reference_payload = parse_json_output(
        run_probe(reference_exe_path),
        "reference counting weak/autoreleasepool probe",
    )

    inside = payload.get("inside", {})
    after = payload.get("after", {})
    reference_nested = reference_payload.get("memory_nested_pool", {})
    reference_after_inner = reference_payload.get("memory_after_inner_pool", {})
    reference_after_outer = reference_payload.get("memory_after_outer_pool", {})
    reference_after_cleanup = reference_payload.get(
        "memory_after_nested_release_cleanup", {}
    )
    reference_before_reset_cleanup = reference_payload.get(
        "memory_before_reset_cleanup", {}
    )
    reference_after_reset_cleanup = reference_payload.get(
        "memory_after_reset_cleanup", {}
    )

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
    expect(payload.get("weak_after_pool") == 0,
           "expected weak-property helper read after strong owner release to zero the stale weak slot")
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
    expect(inside.get("last_property_owner_identity") == "interface:ArcBox",
           "expected helper ABI debug state to report the ArcBox declaration owner identity")
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
    expect(after.get("last_property_owner_identity") == "interface:ArcBox",
           "expected post-pool helper ABI debug state to preserve the ArcBox declaration owner identity")
    expect(reference_payload.get("parent", 0) != 0 and reference_payload.get("child", 0) != 0,
           "expected reference-counting weak/autoreleasepool probe to allocate live receivers")
    expect(reference_payload.get("weak_inside_pool") == reference_payload.get("getter_value"),
           "expected reference-counting probe to preserve the weak child inside the autoreleasepool")
    expect(reference_payload.get("weak_after_pool") == 0,
           "expected reference-counting probe to zero the weak slot after the child drains")
    expect(reference_payload.get("weak_stale_zeroed") == 1,
           "expected reference-counting probe to publish stale weak zeroing")
    expect(reference_payload.get("nested_lifo_drain_order_observed") == 1,
           "expected reference-counting probe to observe nested autoreleasepool LIFO drain order")
    expect(reference_nested.get("autoreleasepool_depth") == 2,
           "expected reference-counting probe to observe two nested autoreleasepool frames")
    expect(reference_after_inner.get("autoreleasepool_depth") == 1,
           "expected reference-counting probe to preserve the outer frame after inner drain")
    expect(reference_after_outer.get("autoreleasepool_depth") == 0,
           "expected reference-counting probe to drain all nested autoreleasepool frames")
    expect(reference_after_inner.get("last_drained_autorelease_value") == reference_payload.get("nested_inner_autoreleased"),
           "expected the inner autoreleasepool frame to drain before the outer frame")
    expect(reference_after_outer.get("last_drained_autorelease_value") == reference_payload.get("nested_outer_autoreleased"),
           "expected the outer autoreleasepool frame to drain after the inner frame")
    expect(reference_after_inner.get("live_runtime_instance_count") + 1 == reference_nested.get("live_runtime_instance_count"),
           "expected the inner autoreleasepool drain to destroy one managed receiver")
    expect(reference_after_outer.get("live_runtime_instance_count") + 2 == reference_nested.get("live_runtime_instance_count"),
           "expected nested autoreleasepool drains to destroy two managed receivers")
    expect(reference_after_cleanup.get("live_runtime_instance_count") == reference_after_outer.get("live_runtime_instance_count"),
           "expected nested autoreleasepool cleanup accounting to stay stable after drain")
    expect(reference_payload.get("reset_cleanup_observed") == 1,
           "expected reference-counting probe to publish deterministic reset cleanup for allocated runtime instances")
    expect(reference_before_reset_cleanup.get("live_runtime_instance_count", 0) >= 2,
           "expected reset cleanup setup to allocate live runtime instances before reset")
    expect(reference_before_reset_cleanup.get("weak_slot_ref_count", 0) >= 1,
           "expected reset cleanup setup to register at least one weak slot before reset")
    expect(reference_after_reset_cleanup.get("live_runtime_instance_count") == 0,
           "expected reset cleanup to clear allocated runtime instances")
    expect(reference_after_reset_cleanup.get("weak_target_count") == 0,
           "expected reset cleanup to clear weak target bookkeeping")
    expect(reference_after_reset_cleanup.get("weak_slot_ref_count") == 0,
           "expected reset cleanup to clear weak slot bookkeeping")
    expect(reference_after_reset_cleanup.get("autoreleasepool_depth") == 0,
           "expected reset cleanup to clear autoreleasepool depth")
    expect(reference_after_reset_cleanup.get("queued_autorelease_value_count") == 0,
           "expected reset cleanup to clear queued autorelease values")

    return CaseResult(
        case_id="arc-property-helper-abi",
        probe="tests/tooling/runtime/arc_debug_instrumentation_probe.cpp;tests/tooling/runtime/reference_counting_weak_autoreleasepool_probe.cpp",
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
            "weak_after_strong_owner_release": payload.get("weak_after_pool"),
            "inside_retain_call_count": inside.get("retain_call_count"),
            "inside_current_property_exchange_count": inside.get("current_property_exchange_count"),
            "after_autoreleasepool_pop_count": after.get("autoreleasepool_pop_count"),
            "after_release_call_count": after.get("release_call_count"),
            "reference_weak_stale_zeroed": reference_payload.get("weak_stale_zeroed"),
            "reference_nested_lifo_drain_order_observed": reference_payload.get("nested_lifo_drain_order_observed"),
            "reference_after_inner_last_drained": reference_after_inner.get("last_drained_autorelease_value"),
            "reference_after_outer_last_drained": reference_after_outer.get("last_drained_autorelease_value"),
            "reference_after_outer_live_runtime_instance_count": reference_after_outer.get("live_runtime_instance_count"),
            "reference_reset_cleanup_observed": reference_payload.get("reset_cleanup_observed"),
            "reference_after_reset_live_runtime_instance_count": reference_after_reset_cleanup.get("live_runtime_instance_count"),
            "reference_after_reset_weak_slot_ref_count": reference_after_reset_cleanup.get("weak_slot_ref_count"),
        },
    )
