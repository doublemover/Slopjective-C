"""Cross-module Block/ARC artifact preservation acceptance case."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_with_args
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.runtime_contract_block_arc import (
    BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE,
    BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE,
    RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
)


def check_cross_module_block_ownership_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-block-ownership-artifact-preservation"
    provider_fixture = ROOT / Path(BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_block_surface = provider_import_payload.get(
        "objc_runtime_block_ownership_artifact_preservation", {}
    )
    expect(
        isinstance(provider_block_surface, dict),
        "expected block-ownership provider import surface to publish the preservation packet",
    )
    expected_provider_fields = {
        "contract_id": RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        "source_contract_id": RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        "block_object_invoke_thunk_lowering_contract_id": "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
        "block_byref_helper_lowering_contract_id": "objc3c.executable.block.byref.helper.lowering.v1",
        "block_escape_runtime_hook_lowering_contract_id": "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        "runtime_support_library_link_wiring_contract_id": "objc3c.runtime.support.library.link.wiring.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_runtime_block_ownership_artifact_preservation",
        "import_artifact_member_name": "objc_runtime_block_ownership_artifact_preservation",
        "source_model": "runtime-block-lowering-helper-surfaces-preserve-invoke-thunk-byref-copy-dispose-escape-and-runtime-link-facts-for-separate-compilation",
        "preservation_model": "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission",
        "fail_closed_model": "missing-or-drifted-block-ownership-preservation-packets-disable-cross-module-block-ownership-claims",
    }
    for field_name, expected_value in expected_provider_fields.items():
        expect(
            provider_block_surface.get(field_name) == expected_value,
            f"expected block-ownership provider import surface to preserve {field_name}",
        )
    for field_name, expected_value in (
        ("local_block_literal_sites", 1),
        ("local_invoke_trampoline_symbolized_sites", 1),
        ("local_copy_helper_required_sites", 1),
        ("local_dispose_helper_required_sites", 1),
        ("local_copy_helper_symbolized_sites", 1),
        ("local_dispose_helper_symbolized_sites", 1),
        ("local_escape_to_heap_sites", 1),
        ("local_byref_layout_symbolized_sites", 1),
    ):
        expect(
            provider_block_surface.get(field_name) == expected_value,
            f"expected block-ownership provider import surface to preserve {field_name}",
        )
    expect(
        provider_block_surface.get("runtime_import_artifact_ready") is True
        and provider_block_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_block_surface.get("runtime_support_library_link_wiring_ready")
        is True
        and provider_block_surface.get("deterministic") is True,
        "expected block-ownership provider import surface to be import-ready deterministic and runtime-link ready",
    )
    expect(
        isinstance(provider_block_surface.get("replay_key"), str)
        and provider_block_surface.get("replay_key") != "",
        "expected block-ownership provider import surface to publish a replay key",
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    for field_name, expected_value in (
        (
            "runtime_cross_module_block_ownership_artifact_preservation_surface_contract_id",
            RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "runtime_block_arc_lowering_helper_surface_contract_id",
            RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        ),
        (
            "block_object_invoke_thunk_lowering_contract_id",
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
        ),
        (
            "block_byref_helper_lowering_contract_id",
            "objc3c.executable.block.byref.helper.lowering.v1",
        ),
        (
            "block_escape_runtime_hook_lowering_contract_id",
            "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        ),
        (
            "block_runtime_support_library_link_wiring_contract_id",
            "objc3c.runtime.support.library.link.wiring.v1",
        ),
        (
            "block_ownership_artifact_preservation_model",
            "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission",
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module block-ownership link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("block_ownership_cross_module_preservation_ready") is True,
        "expected cross-module link plan to mark block-ownership preservation ready",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected block-ownership link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == provider_import_payload.get("module_name")
        == "m261_byref_cell_copy_dispose_runtime_positive",
        "expected block-ownership link plan to preserve the provider module name",
    )
    for field_name, expected_value in (
        ("block_ownership_artifact_preservation_present", True),
        ("block_ownership_runtime_import_artifact_ready", True),
        ("block_ownership_separate_compilation_preservation_ready", True),
        ("block_ownership_runtime_support_library_link_wiring_ready", True),
        ("block_ownership_deterministic", True),
        (
            "block_ownership_contract_id",
            RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "block_ownership_source_contract_id",
            RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        ),
        (
            "block_ownership_object_invoke_thunk_lowering_contract_id",
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
        ),
        (
            "block_ownership_byref_helper_lowering_contract_id",
            "objc3c.executable.block.byref.helper.lowering.v1",
        ),
        (
            "block_ownership_escape_runtime_hook_lowering_contract_id",
            "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        ),
        (
            "block_ownership_runtime_support_library_link_wiring_contract_id",
            "objc3c.runtime.support.library.link.wiring.v1",
        ),
        ("block_ownership_local_block_literal_sites", 1),
        ("block_ownership_local_invoke_trampoline_symbolized_sites", 1),
        ("block_ownership_local_copy_helper_required_sites", 1),
        ("block_ownership_local_dispose_helper_required_sites", 1),
        ("block_ownership_local_copy_helper_symbolized_sites", 1),
        ("block_ownership_local_dispose_helper_symbolized_sites", 1),
        ("block_ownership_local_escape_to_heap_sites", 1),
        ("block_ownership_local_byref_layout_symbolized_sites", 1),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported block-ownership module to preserve {field_name}",
        )
    expect(
        isinstance(imported_module.get("block_ownership_replay_key"), str)
        and imported_module.get("block_ownership_replay_key") != "",
        "expected imported block-ownership module to preserve a replay key",
    )

    local_expected = {
        "local_block_ownership_block_literal_sites": 0,
        "local_block_ownership_invoke_trampoline_symbolized_sites": 0,
        "local_block_ownership_copy_helper_required_sites": 0,
        "local_block_ownership_dispose_helper_required_sites": 0,
        "local_block_ownership_copy_helper_symbolized_sites": 0,
        "local_block_ownership_dispose_helper_symbolized_sites": 0,
        "local_block_ownership_escape_to_heap_sites": 0,
        "local_block_ownership_byref_layout_symbolized_sites": 0,
    }
    imported_expected = {
        "imported_block_ownership_block_literal_sites": 1,
        "imported_block_ownership_invoke_trampoline_symbolized_sites": 1,
        "imported_block_ownership_copy_helper_required_sites": 1,
        "imported_block_ownership_dispose_helper_required_sites": 1,
        "imported_block_ownership_copy_helper_symbolized_sites": 1,
        "imported_block_ownership_dispose_helper_symbolized_sites": 1,
        "imported_block_ownership_escape_to_heap_sites": 1,
        "imported_block_ownership_byref_layout_symbolized_sites": 1,
    }
    transitive_expected = {
        "transitive_block_ownership_block_literal_sites": 1,
        "transitive_block_ownership_invoke_trampoline_symbolized_sites": 1,
        "transitive_block_ownership_copy_helper_required_sites": 1,
        "transitive_block_ownership_dispose_helper_required_sites": 1,
        "transitive_block_ownership_copy_helper_symbolized_sites": 1,
        "transitive_block_ownership_dispose_helper_symbolized_sites": 1,
        "transitive_block_ownership_escape_to_heap_sites": 1,
        "transitive_block_ownership_byref_layout_symbolized_sites": 1,
    }
    for field_name, expected_value in local_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in imported_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in transitive_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-block-ownership-artifact-preservation",
        probe=None,
        fixture=BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE,
            "consumer_fixture": BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": link_plan.get("local_module", {}).get("module_name"),
            "imported_block_literal_sites": link_plan.get(
                "imported_block_ownership_block_literal_sites"
            ),
            "imported_copy_helper_required_sites": link_plan.get(
                "imported_block_ownership_copy_helper_required_sites"
            ),
            "imported_byref_layout_symbolized_sites": link_plan.get(
                "imported_block_ownership_byref_layout_symbolized_sites"
            ),
        },
    )


__all__ = ["check_cross_module_block_ownership_artifact_preservation_case"]
