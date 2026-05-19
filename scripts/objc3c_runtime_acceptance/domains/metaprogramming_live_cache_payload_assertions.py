"""Cache payload assertions for live metaprogramming host-cache cases."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_compilation import (
    LiveMetaprogrammingCacheCompile,
)

from ..paths import ROOT

_MATERIALIZED_HOST_CACHE_FIELDS: dict[str, Any] = {
    "cache_ready": True,
    "launch_attempted": True,
    "cache_hit": True,
    "cache_summary_present": True,
    "cache_runtime_import_surface_present": True,
    "cache_manifest_present": True,
    "cache_materialization_state": "materialized",
    "host_process_exit_code": 0,
    "deterministic": True,
}

_CACHE_HIT_HOST_CACHE_FIELDS: dict[str, Any] = {
    "cache_ready": True,
    "launch_attempted": False,
    "cache_hit": True,
    "cache_summary_present": True,
    "cache_runtime_import_surface_present": True,
    "cache_manifest_present": True,
    "cache_materialization_state": "cache-hit",
    "host_process_exit_code": 0,
    "deterministic": True,
}

_HOST_CACHE_RELATIVE_PATH_FIELDS = (
    "cache_entry_relative_path",
    "cache_summary_relative_path",
    "cache_runtime_import_surface_relative_path",
    "cache_manifest_relative_path",
)

_CACHE_HIT_STABLE_FIELDS = (
    "cache_key",
    "cache_entry_relative_path",
    "cache_summary_relative_path",
    "cache_runtime_import_surface_relative_path",
    "cache_manifest_relative_path",
    "host_executable_relative_path",
    "cache_root_relative_path",
    "replay_key",
)


def assert_materialized_host_cache_payload(
    materialized: LiveMetaprogrammingCacheCompile,
) -> None:
    _assert_host_cache_fields(
        materialized.host_cache_artifact,
        _MATERIALIZED_HOST_CACHE_FIELDS,
        "first metaprogramming host-cache materialization artifact",
    )
    for relative_field in _HOST_CACHE_RELATIVE_PATH_FIELDS:
        relative_value = materialized.host_cache_artifact.get(relative_field)
        expect(
            isinstance(relative_value, str) and relative_value != "",
            f"expected first metaprogramming host-cache materialization artifact to publish {relative_field}",
        )
        expect(
            (ROOT / Path(relative_value)).is_file()
            or (ROOT / Path(relative_value)).is_dir(),
            f"expected first metaprogramming host-cache materialization artifact path {relative_field} to exist",
        )


def assert_cache_hit_replay_payload(
    materialized: LiveMetaprogrammingCacheCompile,
    replay: LiveMetaprogrammingCacheCompile,
) -> None:
    _assert_host_cache_fields(
        replay.host_cache_artifact,
        _CACHE_HIT_HOST_CACHE_FIELDS,
        "second metaprogramming host-cache materialization artifact",
    )
    for field_name in _CACHE_HIT_STABLE_FIELDS:
        expect(
            replay.host_cache_artifact.get(field_name)
            == materialized.host_cache_artifact.get(field_name),
            f"expected second metaprogramming host-cache materialization artifact to preserve {field_name}",
        )


def _assert_host_cache_fields(
    artifact: dict[str, Any],
    expected_fields: dict[str, Any],
    artifact_label: str,
) -> None:
    for field_name, expected_value in expected_fields.items():
        expect(
            artifact.get(field_name) == expected_value,
            f"expected {artifact_label} to preserve {field_name}",
        )


__all__ = [
    "assert_cache_hit_replay_payload",
    "assert_materialized_host_cache_payload",
]
