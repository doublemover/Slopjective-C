"""Runtime package-loader bridge ABI contract surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..c_api import (
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)
from ..runtime_contract_interop import (
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
    INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE,
    RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
)


def build_runtime_package_loader_bridge_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"runtime-package-loader-bridge-abi"}
    ]
    return {
        "contract_id": RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
        ],
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "authoritative_case_ids": authoritative_case_ids,
        "runtime_abi_model": (
            "private-runtime-snapshots-publish-package-loader-topology-deferred-bridge-generation-disabled-state-and-invalid-descriptor-fail-closed-diagnostics-through-the-live-runtime-library-without-public-abi-widening"
        ),
        "authoritative_code_paths": [
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_probe_paths": [
            INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
            INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
            INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE,
        ],
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
        "requires_fail_closed_null_descriptor_probe": True,
        "fail_closed_status": "OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR",
    }


__all__ = ["build_runtime_package_loader_bridge_abi_surface"]
