"""Runtime import, link-plan, and probe assertions for live host-cache cases."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_compilation import (
    LiveMetaprogrammingCacheCompile,
    LiveMetaprogrammingCacheConsumerLink,
    LiveMetaprogrammingCacheProbeRun,
    LiveMetaprogrammingCacheProvider,
)

_SHARED_SCRATCH_CACHE_ROOT = "tmp/artifacts/objc3c-native/cache/metaprogramming"


def assert_materialized_runtime_surfaces(
    provider: LiveMetaprogrammingCacheProvider,
    materialized: LiveMetaprogrammingCacheCompile,
) -> None:
    expect(
        materialized.runtime_import_surface.get("module_name") == provider.module_name,
        "expected first metaprogramming host-cache materialization compile to publish the unique module name",
    )
    expect(
        materialized.host_cache_artifact.get("cache_root_relative_path")
        == provider.cache_root_override
        and materialized.host_cache_import_surface.get("cache_root_relative_path")
        == provider.cache_root_override,
        "expected first metaprogramming host-cache materialization compile to use the test-owned cache root override",
    )
    expect(
        materialized.host_cache_artifact.get("cache_root_relative_path")
        != _SHARED_SCRATCH_CACHE_ROOT,
        "expected live metaprogramming host-cache test not to depend on the shared scratch cache root",
    )
    expect(
        materialized.host_cache_import_surface.get("host_executable_relative_path")
        == materialized.host_cache_artifact.get("host_executable_relative_path")
        and materialized.host_cache_import_surface.get("cache_root_relative_path")
        == materialized.host_cache_artifact.get("cache_root_relative_path"),
        "expected first metaprogramming host-cache materialization compile to align import-surface and artifact cache paths",
    )


def assert_cache_hit_runtime_surfaces(
    materialized: LiveMetaprogrammingCacheCompile,
    replay: LiveMetaprogrammingCacheCompile,
) -> None:
    expect(
        replay.host_cache_import_surface.get("replay_key")
        == materialized.host_cache_import_surface.get("replay_key"),
        "expected repeated metaprogramming host-cache materialization compile to preserve the same import-surface replay key",
    )


def assert_consumer_link_plan(
    provider: LiveMetaprogrammingCacheProvider,
    materialized: LiveMetaprogrammingCacheCompile,
    consumer_link: LiveMetaprogrammingCacheConsumerLink,
) -> None:
    link_plan = consumer_link.link_plan
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
            materialized.host_cache_artifact.get("host_executable_relative_path"),
        ),
        (
            "expected_metaprogramming_host_cache_root_relative_path",
            materialized.host_cache_artifact.get("cache_root_relative_path"),
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected live metaprogramming host-cache consumer link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("metaprogramming_host_cache_imported_module_count") == 1
        and link_plan.get("metaprogramming_host_cache_imported_module_names_lexicographic")
        == [provider.module_name]
        and link_plan.get("metaprogramming_host_cache_cross_module_preservation_ready")
        is True,
        "expected live metaprogramming host-cache consumer link plan to preserve imported module readiness",
    )

    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected live metaprogramming host-cache consumer link plan to publish one imported module",
    )
    imported_module = imported_modules[0]
    for field_name, expected_value in (
        ("module_name", provider.module_name),
        ("metaprogramming_macro_host_process_cache_runtime_integration_present", True),
        ("metaprogramming_macro_host_process_cache_runtime_ready", True),
        ("metaprogramming_macro_host_process_cache_separate_compilation_ready", True),
        ("metaprogramming_macro_host_process_cache_deterministic", True),
        (
            "metaprogramming_macro_host_process_cache_host_executable_relative_path",
            materialized.host_cache_artifact.get("host_executable_relative_path"),
        ),
        (
            "metaprogramming_macro_host_process_cache_root_relative_path",
            materialized.host_cache_artifact.get("cache_root_relative_path"),
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected live metaprogramming host-cache imported module to preserve {field_name}",
        )


def assert_runtime_probe_payload(
    materialized: LiveMetaprogrammingCacheCompile,
    probe_run: LiveMetaprogrammingCacheProbeRun,
) -> None:
    expect(
        probe_run.payload.get("host_executable_relative_path")
        == materialized.host_cache_artifact.get("host_executable_relative_path")
        and probe_run.payload.get("cache_root_relative_path")
        == materialized.host_cache_artifact.get("cache_root_relative_path")
        and probe_run.payload.get("macro_host_execution_ready") == 1
        and probe_run.payload.get("macro_host_process_launch_ready") == 1,
        "expected live metaprogramming host-cache runtime probe to stay aligned with the cache artifact paths and readiness",
    )


__all__ = [
    "assert_cache_hit_runtime_surfaces",
    "assert_consumer_link_plan",
    "assert_materialized_runtime_surfaces",
    "assert_runtime_probe_payload",
]
