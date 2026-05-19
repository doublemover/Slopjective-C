"""Cross-module error metadata replay preservation acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_live_error_runtime_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT


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
    compile_live_error_runtime_fixture_outputs(
        provider_fixture,
        provider_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = provider_dir / "module.runtime-import-surface.json"
    expect(
        provider_import_surface.is_file(),
        "expected cross-module error provider to emit a runtime import surface",
    )
    compile_live_error_runtime_fixture_outputs(
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
        imported_module.get("module_name")
        == "m267_c003_error_handling_artifact_replay_producer",
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
        and imported_module.get("error_handling_separate_compilation_replay_ready")
        is True,
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
    replay_key = imported_module.get(
        "error_handling_result_and_bridging_artifact_replay_key", ""
    )
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
            "provider_import_surface": str(
                provider_import_surface.relative_to(ROOT)
            ).replace("\\", "/"),
            "consumer_link_plan": str(link_plan_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
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


__all__ = ["check_cross_module_error_metadata_replay_preservation_case"]
