"""Assertions for metaprogramming runtime ABI cache acceptance cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.paths import (
    FRONTEND_RUNNER_RELATIVE_PATH,
    RUNTIME_LIB_RELATIVE_PATH,
)


BOUNDARY_EXPECTED_PAYLOAD = {
    "copy_status": 0,
    "property_runtime_ready": 1,
    "macro_host_execution_ready": 0,
    "macro_host_process_launch_ready": 0,
    "runtime_package_loader_ready": 0,
    "deterministic": 1,
    "runtime_support_library_archive_relative_path": RUNTIME_LIB_RELATIVE_PATH,
    "property_behavior_runtime_model": (
        "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks"
    ),
    "macro_expansion_host_model": (
        "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed"
    ),
    "fail_closed_model": "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet",
}

HOST_CACHE_EXPECTED_PAYLOAD = {
    "copy_status": 0,
    "property_runtime_ready": 1,
    "macro_host_execution_ready": 1,
    "macro_host_process_launch_ready": 1,
    "runtime_package_loader_ready": 0,
    "deterministic": 1,
    "host_executable_relative_path": FRONTEND_RUNNER_RELATIVE_PATH,
    "cache_root_relative_path": "tmp/artifacts/objc3c-native/cache/metaprogramming",
    "host_model": (
        "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization"
    ),
    "toolchain_model": (
        "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization"
    ),
    "cache_model": (
        "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-and-reused-on-subsequent-runs"
    ),
    "fail_closed_model": (
        "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims"
    ),
}


def expect_runtime_abi_payload(
    payload: dict[str, Any], expected_payload: dict[str, Any], label: str
) -> None:
    for field_name, expected_value in expected_payload.items():
        expect(
            payload.get(field_name) == expected_value,
            f"expected metaprogramming runtime ABI {label} probe to preserve {field_name}",
        )


def expect_host_cache_artifact_surface(
    host_cache_artifact: dict[str, Any],
    host_cache_import_surface: dict[str, Any],
    host_cache_payload: dict[str, Any],
) -> None:
    expect(
        host_cache_artifact.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        and host_cache_import_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected metaprogramming runtime ABI host-cache artifact and runtime import surface to preserve the host-cache integration contract",
    )
    for field_name in (
        "host_executable_relative_path",
        "cache_root_relative_path",
        "deterministic",
        "replay_key",
    ):
        expect(
            host_cache_artifact.get(field_name) == host_cache_import_surface.get(field_name),
            f"expected metaprogramming runtime ABI host-cache artifact and runtime import surface to preserve {field_name}",
        )
    expect(
        host_cache_artifact.get("host_executable_relative_path")
        == host_cache_payload.get("host_executable_relative_path")
        and host_cache_artifact.get("cache_root_relative_path")
        == host_cache_payload.get("cache_root_relative_path"),
        "expected metaprogramming runtime ABI host-cache artifact to stay aligned with the runtime snapshot paths",
    )
