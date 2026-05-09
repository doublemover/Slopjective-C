"""CaseResult summary helpers for live metaprogramming host-cache cases."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_compilation import (
    LiveMetaprogrammingCacheCompile,
    LiveMetaprogrammingCacheConsumerLink,
    LiveMetaprogrammingCacheProvider,
)

from ..paths import ROOT


def build_live_metaprogramming_cache_runtime_integration_summary(
    provider: LiveMetaprogrammingCacheProvider,
    materialized: LiveMetaprogrammingCacheCompile,
    replay: LiveMetaprogrammingCacheCompile,
    consumer_link: LiveMetaprogrammingCacheConsumerLink,
) -> dict[str, Any]:
    return {
        "provider_fixture": _relative_to_root(provider.fixture),
        "provider_module_name": provider.module_name,
        "cache_root_override_flag": "--objc3-metaprogramming-cache-root",
        "cache_root_environment_variable": "OBJC3C_METAPROGRAMMING_CACHE_ROOT",
        "cache_root_relative_path": provider.cache_root_override,
        "cache_root_is_test_owned": True,
        "shared_cache_prune_used": False,
        "first_host_cache_artifact": {
            "path": _relative_to_root(materialized.host_cache_artifact_path),
            "cache_key": materialized.host_cache_artifact.get("cache_key"),
            "cache_materialization_state": materialized.host_cache_artifact.get(
                "cache_materialization_state"
            ),
            "launch_attempted": materialized.host_cache_artifact.get(
                "launch_attempted"
            ),
        },
        "second_host_cache_artifact": {
            "path": _relative_to_root(replay.host_cache_artifact_path),
            "cache_key": replay.host_cache_artifact.get("cache_key"),
            "cache_materialization_state": replay.host_cache_artifact.get(
                "cache_materialization_state"
            ),
            "launch_attempted": replay.host_cache_artifact.get("launch_attempted"),
        },
        "consumer_link_plan": _relative_to_root(consumer_link.link_plan_path),
        "imported_module_names": consumer_link.link_plan.get(
            "metaprogramming_host_cache_imported_module_names_lexicographic"
        ),
    }


def live_metaprogramming_cache_provider_fixture_summary_path(
    provider: LiveMetaprogrammingCacheProvider,
) -> str:
    return _relative_to_root(provider.fixture)


def _relative_to_root(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


__all__ = [
    "build_live_metaprogramming_cache_runtime_integration_summary",
    "live_metaprogramming_cache_provider_fixture_summary_path",
]
