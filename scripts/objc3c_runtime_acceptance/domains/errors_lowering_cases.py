"""Error-handling lowering and replay acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    ROOT,
    compile_fixture_outputs,
    compile_fixture_outputs_with_args,
)


_EXPORTED_CASE_NAMES = [
    "check_error_lowering_unwind_bridge_helper_surface_case",
    "check_executable_throw_catch_cleanup_lowering_case",
    "check_cross_module_error_metadata_replay_preservation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_error_lowering_unwind_bridge_helper_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-lowering-unwind-bridge-helper-surface"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "error_out_abi_positive.objc3"
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    throws_abi = manifest.get("lowering_error_handling_throws_abi_propagation", {})
    expect(
        isinstance(throws_abi, dict)
        and throws_abi.get("contract_id") == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected error lowering fixture to publish the throws ABI propagation lowering surface",
    )
    expect(
        "objc3_runtime_store_thrown_error_i32" in ll_text
        and "objc3_runtime_load_thrown_error_i32" in ll_text
        and "objc3_runtime_bridge_status_error_i32" in ll_text
        and "objc3_runtime_catch_matches_error_i32" in ll_text,
        "expected error lowering fixture to emit the runtime bridge helper calls",
    )
    return CaseResult(
        case_id="error-lowering-unwind-bridge-helper-surface",
        probe="compile-artifact-llvm-helper-lowering",
        fixture="tests/tooling/fixtures/native/error_out_abi_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "throws_abi_contract": throws_abi.get("contract_id"),
        },
    )


def check_executable_throw_catch_cleanup_lowering_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "executable-throw-catch-cleanup-lowering"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "error_out_abi_positive.objc3"
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    throws_abi = manifest.get("lowering_error_handling_throws_abi_propagation", {})
    result_replay = manifest.get("error_handling_result_and_bridging_artifact_replay", {})
    ns_error_bridging = manifest.get("ns_error_bridging_lowering_surface", {})
    unwind_cleanup = manifest.get("unwind_cleanup_lowering_surface", {})
    expect(
        isinstance(throws_abi, dict)
        and throws_abi.get("contract_id")
        == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected executable error lowering fixture to preserve the throws ABI propagation contract",
    )
    expect(
        isinstance(result_replay, dict)
        and result_replay.get("contract_id")
        == "objc3c.error_handling.result.and.bridging.artifact.replay.v1",
        "expected executable error lowering fixture to preserve the error result/bridging replay contract",
    )
    expect(
        isinstance(ns_error_bridging, dict)
        and ns_error_bridging.get("lane_contract")
        == "objc3c.ns.error.bridging.lowering.v1",
        "expected executable error lowering fixture to preserve the NSError bridging lowering contract",
    )
    expect(
        isinstance(unwind_cleanup, dict)
        and unwind_cleanup.get("lane_contract")
        == "objc3c.unwind.cleanup.lowering.v1",
        "expected executable error lowering fixture to preserve the unwind cleanup lowering contract",
    )
    return CaseResult(
        case_id="executable-throw-catch-cleanup-lowering",
        probe="compile-artifact-llvm-helper-lowering",
        fixture="tests/tooling/fixtures/native/error_out_abi_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_abi_contract": throws_abi.get("contract_id"),
            "result_replay_contract": result_replay.get("contract_id"),
            "ns_error_bridging_contract": ns_error_bridging.get("lane_contract"),
            "unwind_cleanup_contract": unwind_cleanup.get("lane_contract"),
            "helper_calls": {
                "store": "objc3_runtime_store_thrown_error_i32" in ll_text,
                "load": "objc3_runtime_load_thrown_error_i32" in ll_text,
                "status_bridge": "objc3_runtime_bridge_status_error_i32" in ll_text,
                "catch_match": "objc3_runtime_catch_matches_error_i32" in ll_text,
            },
        },
    )


def check_cross_module_error_metadata_replay_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "cross-module-error-metadata-replay-preservation"
    provider_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "artifact_replay_producer.objc3"
    )
    consumer_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "result_bridge_consumer.objc3"
    )
    provider_dir = case_dir / "provider"
    consumer_dir = case_dir / "consumer"
    compile_fixture_outputs_with_args(
        provider_fixture,
        provider_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = provider_dir / "module.runtime-import-surface.json"
    expect(
        provider_import_surface.is_file(),
        "expected cross-module error provider to emit a runtime import surface",
    )
    compile_fixture_outputs_with_args(
        consumer_fixture,
        consumer_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    link_plan_path = consumer_dir / "module.cross-module-runtime-link-plan.json"
    expect(
        link_plan_path.is_file(),
        "expected cross-module error consumer to emit a cross-module runtime link plan",
    )
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))
    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module error consumer to publish one imported module in the link plan",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name") == "m267_c003_error_handling_artifact_replay_producer",
        "expected cross-module error consumer to preserve the imported producer module name",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected cross-module error consumer to preserve the imported producer registration ordinal",
    )
    expect(
        imported_module.get("error_handling_result_and_bridging_artifact_replay_present")
        is True,
        "expected cross-module error consumer to preserve the error_handling replay packet presence flag",
    )
    expect(
        imported_module.get("error_handling_binary_artifact_replay_ready") is True
        and imported_module.get("error_handling_runtime_import_artifact_ready") is True
        and imported_module.get("error_handling_separate_compilation_replay_ready") is True,
        "expected cross-module error consumer to preserve the error_handling replay readiness flags",
    )
    expect(
        imported_module.get("error_handling_contract_id")
        == "objc3c.error_handling.result.and.bridging.artifact.replay.v1",
        "expected cross-module error consumer to preserve the error_handling replay contract id",
    )
    expect(
        imported_module.get("error_handling_source_contract_id")
        == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected cross-module error consumer to preserve the error_handling replay source contract id",
    )
    replay_key = imported_module.get("error_handling_result_and_bridging_artifact_replay_key", "")
    throws_replay_key = imported_module.get("error_handling_replay_key", "")
    expect(
        isinstance(replay_key, str)
        and "runtime_import_artifact_ready=true" in replay_key
        and "separate_compilation_replay_ready=true" in replay_key,
        "expected cross-module error consumer to preserve the error_handling replay packet readiness in the imported replay key",
    )
    expect(
        isinstance(throws_replay_key, str)
        and "ready_for_runtime_execution=true" in throws_replay_key,
        "expected cross-module error consumer to preserve runtime-executable throws replay in the imported error_handling lowering key",
    )
    local_module = link_plan.get("local_module", {})
    expect(
        local_module.get("module_name") == "m267_result_bridge_consumer"
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module error consumer to preserve the local module identity and registration ordinal",
    )
    return CaseResult(
        case_id="cross-module-error-metadata-replay-preservation",
        probe="cross-module-runtime-link-plan",
        fixture="tests/tooling/fixtures/native/result_bridge_consumer.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_import_surface": str(provider_import_surface.relative_to(ROOT)).replace("\\", "/"),
            "consumer_link_plan": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "imported_module_name": imported_module.get("module_name"),
            "imported_module_registration_ordinal": imported_module.get(
                "translation_unit_registration_order_ordinal"
            ),
            "local_module_name": local_module.get("module_name"),
            "local_module_registration_ordinal": local_module.get(
                "translation_unit_registration_order_ordinal"
            ),
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
