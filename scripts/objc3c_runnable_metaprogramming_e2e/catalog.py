"""Scenario catalog data for runnable metaprogramming validation."""

from __future__ import annotations

from dataclasses import dataclass

REQUIRED_MANIFEST_KEYS = (
    "compile_wrapper",
    "runtime_library",
    "execution_smoke_script",
    "execution_replay_script",
    "runtime_internal_header",
    "metaprogramming_runtime_fixture",
    "metaprogramming_runtime_consumer_fixture",
    "metaprogramming_runtime_probe",
)


@dataclass(frozen=True)
class HostCacheExpectation:
    context: str
    expected_values: dict[str, object]


FIRST_HOST_CACHE_EXPECTATION = HostCacheExpectation(
    context="packaged metaprogramming first compile",
    expected_values={
        "cache_ready": True,
        "launch_attempted": True,
        "cache_hit": True,
        "cache_summary_present": True,
        "cache_runtime_import_surface_present": True,
        "cache_manifest_present": True,
        "cache_materialization_state": "materialized",
        "host_process_exit_code": 0,
        "deterministic": True,
    },
)

SECOND_HOST_CACHE_EXPECTATION = HostCacheExpectation(
    context="packaged metaprogramming second compile",
    expected_values={
        "cache_ready": True,
        "launch_attempted": False,
        "cache_hit": True,
        "cache_summary_present": True,
        "cache_runtime_import_surface_present": True,
        "cache_manifest_present": True,
        "cache_materialization_state": "cache-hit",
        "host_process_exit_code": 0,
        "deterministic": True,
    },
)

PRESERVED_HOST_CACHE_FIELDS = (
    "cache_key",
    "cache_entry_relative_path",
    "cache_summary_relative_path",
    "cache_runtime_import_surface_relative_path",
    "cache_manifest_relative_path",
    "host_executable_relative_path",
    "cache_root_relative_path",
    "replay_key",
)

RUNTIME_PROBE_EXPECTATION = {
    "copy_status": 0,
    "macro_host_execution_ready": 1,
    "macro_host_process_launch_ready": 1,
    "runtime_package_loader_ready": 0,
}

__all__ = [
    "FIRST_HOST_CACHE_EXPECTATION",
    "HostCacheExpectation",
    "PRESERVED_HOST_CACHE_FIELDS",
    "REQUIRED_MANIFEST_KEYS",
    "RUNTIME_PROBE_EXPECTATION",
    "SECOND_HOST_CACHE_EXPECTATION",
]
