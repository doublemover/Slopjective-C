"""Assertions for cross-module metaprogramming acceptance cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


def expect_provider_replay_surface(surface: dict[str, Any]) -> None:
    expect(
        isinstance(surface, dict),
        "expected metaprogramming preservation provider import surface to publish the replay-preservation packet",
    )
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.module.interface.replay.preservation.v1",
        "expected metaprogramming preservation provider import surface to preserve the replay-preservation contract",
    )
    expect(
        surface.get("source_contract_id")
        == "objc3c.metaprogramming.synthesized.ast.ir.emission.v1",
        "expected metaprogramming preservation provider import surface to preserve the synthesized emission source contract",
    )
    expect(
        surface.get("local_derive_method_count") == 1
        and surface.get("local_macro_artifact_count") == 1
        and surface.get("local_interface_property_behavior_artifact_count") == 1
        and surface.get("local_implementation_property_behavior_artifact_count") == 1
        and surface.get("local_runtime_method_list_count") == 1,
        "expected metaprogramming preservation provider import surface to preserve local metaprogramming artifact counts",
    )
    expect(
        surface.get("runtime_import_artifact_ready") is True
        and surface.get("separate_compilation_preservation_ready") is True
        and surface.get("deterministic") is True,
        "expected metaprogramming preservation provider import surface to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        isinstance(surface.get("replay_key"), str) and surface.get("replay_key") != "",
        "expected metaprogramming preservation provider import surface to publish a replay key",
    )


def expect_provider_host_cache_surface(
    surface: dict[str, Any], replay_surface: dict[str, Any]
) -> None:
    expect(
        isinstance(surface, dict),
        "expected metaprogramming preservation provider import surface to publish the host-cache packet",
    )
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected metaprogramming preservation provider import surface to preserve the host-cache contract",
    )
    expect(
        surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        "expected metaprogramming preservation provider import surface to preserve the host runtime boundary contract",
    )
    expect(
        surface.get("local_macro_artifact_count") == 1
        and surface.get("local_property_behavior_artifact_count") == 2
        and surface.get("imported_module_count") == 0,
        "expected metaprogramming preservation provider import surface to preserve host-cache local artifact counts",
    )
    expect(
        surface.get("runtime_import_artifact_ready") is True
        and surface.get("separate_compilation_ready") is True
        and surface.get("deterministic") is True,
        "expected metaprogramming preservation provider host-cache packet to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        surface.get("metaprogramming_replay_key") == replay_surface.get("replay_key"),
        "expected provider host-cache packet to preserve the metaprogramming replay key",
    )


def expect_cross_module_link_plan(
    link_plan: dict[str, Any], provider_import_payload: dict[str, Any]
) -> None:
    for field_name, expected_value in (
        (
            "expected_metaprogramming_host_cache_contract_id",
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        ),
        (
            "expected_metaprogramming_host_cache_source_contract_id",
            "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        ),
        (
            "expected_metaprogramming_host_cache_executable_relative_path",
            "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        ),
        (
            "expected_metaprogramming_host_cache_root_relative_path",
            "tmp/artifacts/objc3c-native/cache/metaprogramming",
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module metaprogramming link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("metaprogramming_host_cache_imported_module_count") == 1
        and link_plan.get("metaprogramming_host_cache_imported_module_names_lexicographic")
        == [provider_import_payload.get("module_name")]
        and link_plan.get("metaprogramming_host_cache_cross_module_preservation_ready")
        is True,
        "expected cross-module metaprogramming link plan to preserve host-cache imported module readiness",
    )


def expect_imported_metaprogramming_module(
    imported_module: dict[str, Any],
    provider_import_payload: dict[str, Any],
    provider_host_cache_surface: dict[str, Any],
) -> None:
    expect(
        imported_module.get("module_name")
        == provider_import_payload.get("module_name")
        == "MetaprogrammingPreservationProvider",
        "expected cross-module metaprogramming link plan to preserve the provider module name",
    )
    for field_name, expected_value in (
        ("metaprogramming_macro_host_process_cache_runtime_integration_present", True),
        ("metaprogramming_macro_host_process_cache_runtime_ready", True),
        ("metaprogramming_macro_host_process_cache_separate_compilation_ready", True),
        ("metaprogramming_macro_host_process_cache_deterministic", True),
        (
            "metaprogramming_macro_host_process_cache_contract_id",
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        ),
        (
            "metaprogramming_macro_host_process_cache_source_contract_id",
            "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        ),
        (
            "metaprogramming_macro_host_process_cache_host_executable_relative_path",
            "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        ),
        (
            "metaprogramming_macro_host_process_cache_root_relative_path",
            "tmp/artifacts/objc3c-native/cache/metaprogramming",
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported metaprogramming module to preserve {field_name}",
        )
    expect(
        imported_module.get("metaprogramming_macro_host_process_cache_replay_key")
        == provider_host_cache_surface.get("replay_key"),
        "expected imported metaprogramming module to preserve the provider host-cache replay key",
    )
    imported_replay_key = imported_module.get(
        "metaprogramming_macro_host_process_cache_replay_key", ""
    )
    for snippet in (
        "objc_metaprogramming_module_interface_and_replay_preservation",
        "local_derive_method_count=1",
        "local_macro_artifact_count=1",
        "local_interface_property_behavior_artifact_count=1",
        "local_implementation_property_behavior_artifact_count=1",
        "local_runtime_method_list_count=1",
        "emitted_runtime_method_list_sites=1",
    ):
        expect(
            snippet in imported_replay_key,
            f"expected imported metaprogramming replay key to preserve {snippet}",
        )
