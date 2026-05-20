"""Object Model standalone runtime-library acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.expectation_matching import expect_equal
from objc3c_runtime_acceptance.expectation_matching import expect_required_mapping
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..paths import ROOT

_EXPORTED_CASE_NAMES = [
    "check_dispatch_lookup_runtime_probe_case",
    "check_method_cache_slow_path_probe_case",
    "check_runtime_library_case",
    "check_typed_dispatch_abi_probe_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_runtime_library_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "runtime-library"
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_library_probe.cpp"
    exe_path = case_dir / "runtime_library_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    run_probe(exe_path)
    dispatch_expectations_probe = (
        ROOT / "tests" / "tooling" / "runtime" / "dispatch_expectations_support_test.cpp"
    )
    dispatch_expectations_exe = case_dir / "dispatch_expectations_support_test.exe"
    compile_probe(clangxx, dispatch_expectations_probe, dispatch_expectations_exe, [])
    run_probe(dispatch_expectations_exe)
    strict_dispatch_probe = (
        ROOT / "tests" / "tooling" / "runtime" / "strict_dispatch_error_status_probe.cpp"
    )
    strict_dispatch_exe = case_dir / "strict_dispatch_error_status_probe.exe"
    compile_probe(clangxx, strict_dispatch_probe, strict_dispatch_exe, [])
    run_probe(strict_dispatch_exe)
    return CaseResult(
        case_id="runtime-library",
        probe="tests/tooling/runtime/runtime_library_probe.cpp",
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "kind": "standalone-runtime-probe",
            "dispatch_expectations_drift_probe": (
                "tests/tooling/runtime/dispatch_expectations_support_test.cpp"
            ),
            "strict_dispatch_error_status_probe": (
                "tests/tooling/runtime/strict_dispatch_error_status_probe.cpp"
            ),
        },
    )


def check_typed_dispatch_abi_probe_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "typed-dispatch-abi-probe"
    probe = ROOT / "tests" / "tooling" / "runtime" / "typed_dispatch_abi_probe.cpp"
    exe_path = case_dir / "typed_dispatch_abi_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    run_probe(exe_path)
    return CaseResult(
        case_id="typed-dispatch-abi-probe",
        probe="tests/tooling/runtime/typed_dispatch_abi_probe.cpp",
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "kind": "typed-dispatch-abi-probe",
            "strict_i32_projection_rejection": "all-non-i32-return-kinds",
            "negative_matrix": [
                "nil-receiver",
                "unknown-selector",
                "unsupported-return",
                "unsupported-argument-layout",
            ],
        },
    )


def check_dispatch_lookup_runtime_probe_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "dispatch-lookup-runtime-probe"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "metaclass_graph_root_class_library.objc3"
    )
    obj_path, _, _ = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = (
        ROOT / "tests" / "tooling" / "runtime" / "dispatch_lookup_runtime_probe.cpp"
    )
    exe_path = case_dir / "dispatch_lookup_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "dispatch lookup runtime probe")
    expect(
        payload.get("lookup_null_is_null") is True,
        "selector null lookup must fail closed",
    )
    expect(
        payload.get("copy_selector_reused") is True,
        "copy selector handle must be reused",
    )
    expect(
        payload.get("copy_selector_stable_id", 0) > 0,
        "copy selector stable id must publish",
    )
    expect_equal(
        payload.get("typed_super_status"),
        0,
        "typed from-class super lookup must succeed",
    )
    expect_equal(
        payload.get("typed_super_value"),
        17,
        "typed from-class super lookup must return RootObject value",
    )
    expect_equal(
        payload.get("i32_super_status"),
        0,
        "i32 from-class super lookup must succeed",
    )
    expect_equal(
        payload.get("i32_super_value"),
        17,
        "i32 from-class super lookup must return RootObject value",
    )
    expect_equal(
        payload.get("typed_self_status"),
        0,
        "typed from-class self lookup must succeed",
    )
    expect_equal(
        payload.get("typed_self_value"),
        23,
        "typed from-class self lookup must return Widget value",
    )
    expect_equal(
        payload.get("i32_self_status"),
        0,
        "i32 from-class self lookup must succeed",
    )
    expect_equal(
        payload.get("i32_self_value"),
        23,
        "i32 from-class self lookup must return Widget value",
    )
    for field in (
        "null_lookup_start_status",
        "empty_lookup_start_status",
        "missing_lookup_start_status",
        "unreachable_lookup_start_status",
    ):
        expect_equal(
            payload.get(field),
            -3,
            f"{field} must fail as missing class graph",
        )
    expect(
        payload.get("registered_image_count", 0) >= 1,
        "linked fixture must register an image",
    )
    return CaseResult(
        case_id="dispatch-lookup-runtime-probe",
        probe="tests/tooling/runtime/dispatch_lookup_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/metaclass_graph_root_class_library.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "typed_super_value": payload.get("typed_super_value"),
            "i32_super_value": payload.get("i32_super_value"),
            "typed_self_value": payload.get("typed_self_value"),
            "invalid_lookup_start_statuses": {
                field: payload.get(field)
                for field in (
                    "null_lookup_start_status",
                    "empty_lookup_start_status",
                    "missing_lookup_start_status",
                    "unreachable_lookup_start_status",
                )
            },
        },
    )


def check_method_cache_slow_path_probe_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "method-cache-slow-path-probe"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "live_method_dispatch.objc3"
    )
    obj_path, _, _ = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "method_cache_slow_path_probe.cpp"
    exe_path = case_dir / "method_cache_slow_path_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "method cache slow path probe")
    expect_equal(
        payload.get("instance_first"),
        11,
        "first instance dispatch must return currentValue",
    )
    expect_equal(
        payload.get("instance_second"),
        11,
        "cached instance dispatch must return currentValue",
    )
    expect_equal(
        payload.get("instance_after_stale"),
        11,
        "stale-cache revalidation must preserve currentValue",
    )
    expect_equal(payload.get("class_self"), 22, "class self dispatch must return shared")
    expect_equal(payload.get("known_class"), 22, "known-class dispatch must return shared")
    expect_equal(
        payload.get("strict_error_first"),
        payload.get("strict_error_expected"),
        "first strict-error dispatch must stay zero-valued",
    )
    expect_equal(
        payload.get("strict_error_second"),
        payload.get("strict_error_expected"),
        "cached strict-error dispatch must stay zero-valued",
    )
    expect_equal(
        payload.get("mutation_registration_status"),
        0,
        "public empty-image mutation registration must succeed",
    )
    registration_state = expect_required_mapping(
        payload,
        "registration_state",
        "missing startup registration state",
    )
    registration_after_mutation_state = expect_required_mapping(
        payload,
        "registration_after_mutation_state",
        "missing mutation registration state",
    )
    first_state = expect_required_mapping(
        payload,
        "instance_first_state",
        "missing first instance cache state",
    )
    second_state = expect_required_mapping(
        payload,
        "instance_second_state",
        "missing cached instance cache state",
    )
    stale_state = expect_required_mapping(
        payload,
        "instance_after_stale_state",
        "missing stale revalidation cache state",
    )
    strict_first_state = expect_required_mapping(
        payload,
        "strict_error_first_state",
        "missing first strict-error state",
    )
    strict_second_state = expect_required_mapping(
        payload,
        "strict_error_second_state",
        "missing cached strict-error state",
    )
    expect_equal(
        first_state.get("last_dispatch_used_cache"),
        0,
        "first dispatch must use slow path",
    )
    expect(
        second_state.get("cache_hit_count", 0)
        > first_state.get("cache_hit_count", 0),
        "second dispatch must hit cache",
    )
    expect(
        registration_after_mutation_state.get("registered_image_count", 0)
        > registration_state.get("registered_image_count", 0),
        "public empty-image mutation must advance registered image count",
    )
    expect_equal(
        registration_after_mutation_state.get(
            "last_successful_registration_order_ordinal"
        ),
        registration_state.get("next_expected_registration_order_ordinal"),
        "public empty-image mutation must consume next registration ordinal",
    )
    expect(
        stale_state.get("stale_method_cache_entry_count", 0)
        > second_state.get("stale_method_cache_entry_count", 0),
        "stale revalidation must be counted",
    )
    expect(
        stale_state.get("cache_miss_count", 0)
        > second_state.get("cache_miss_count", 0),
        "stale revalidation must repopulate through slow path",
    )
    expect_equal(
        stale_state.get("last_dispatch_resolved_live_method"),
        1,
        "stale revalidation must resolve live",
    )
    expect(
        strict_second_state.get("cache_hit_count", 0)
        > strict_first_state.get("cache_hit_count", 0),
        "strict-error second dispatch must hit cache",
    )
    return CaseResult(
        case_id="method-cache-slow-path-probe",
        probe="tests/tooling/runtime/method_cache_slow_path_probe.cpp",
        fixture="tests/tooling/fixtures/native/live_method_dispatch.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "instance_after_stale": payload.get("instance_after_stale"),
            "mutation_registration_status": payload.get(
                "mutation_registration_status"
            ),
            "stale_method_cache_entry_count": stale_state.get(
                "stale_method_cache_entry_count"
            ),
            "strict_error_second_cache_hits": strict_second_state.get(
                "cache_hit_count"
            ),
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
