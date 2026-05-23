"""Live package-loading interop implementation contract surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_interop import (
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
    INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE,
    RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
)


def build_runtime_package_loading_interop_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"live-package-loading-interop-runtime-implementation"}
    ]
    return {
        "contract_id": (
            RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
            RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
        ],
        "implementation_model": (
            "live-runtime-package-loader-snapshots-preserve-runtime-archive-and-deferred-bridge-paths-while-consumer-imports-fail-closed-until-live-interop-bridge-generation-and-cross-module-link-planning-are-implemented"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
            INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        ],
        "authoritative_probe_paths": [
            INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
            INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
            INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": False,
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
        "requires_fail_closed_consumer_import": True,
        "requires_tampered_runtime_library_rejection": True,
    }


__all__ = ["build_runtime_package_loading_interop_implementation_surface"]
