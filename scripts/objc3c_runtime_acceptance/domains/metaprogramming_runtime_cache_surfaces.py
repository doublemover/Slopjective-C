"""Metaprogramming runtime ABI/cache acceptance contract surfaces."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_lowering_surfaces import (
    RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
)

from ..c_api import (
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)

RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.runtime.abi.cache.surface.v1"
)


RUNTIME_METAPROGRAMMING_CACHE_RUNTIME_INTEGRATION_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.cache.runtime.integration.implementation.surface.v1"
)


PRIVATE_METAPROGRAMMING_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing",
    "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing",
]


METAPROGRAMMING_RUNTIME_ABI_BOUNDARY_MODEL = (
    "private-metaprogramming-boundary-and-host-cache-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header"
)


METAPROGRAMMING_EXPANSION_RUNTIME_MODEL = (
    "property-behavior-runtime-support-is-live-while-macro-host-execution-runtime-process-launch-and-package-loading-remain-fail-closed-on-the-expansion-boundary-snapshot"
)


METAPROGRAMMING_HOST_CACHE_RUNTIME_MODEL = (
    "deterministic-host-process-launch-cache-root-selection-and-replay-key-compatible-cache-materialization-stay-on-bootstrap-internal-snapshots-and-runtime-import-surfaces"
)


METAPROGRAMMING_RUNTIME_FAIL_CLOSED_MODEL = (
    "public-runtime-header-remains-registration-lookup-dispatch-only-and-runtime-package-loading-stays-disabled-until-deliberate-metaprogramming-runtime-abi-widening"
)


def build_runtime_metaprogramming_runtime_abi_cache_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "metaprogramming-executable-lowering",
            "cross-module-metaprogramming-artifact-preservation",
            "metaprogramming-runtime-abi-cache-surface",
        }
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.metaprogramming-macro-host-cache.json",
            "<emit-prefix>.runtime-import-surface.json",
        ],
        "source_contract_ids": [
            RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ],
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_metaprogramming_runtime_abi_boundary": (
            PRIVATE_METAPROGRAMMING_RUNTIME_ABI_BOUNDARY
        ),
        "expansion_host_boundary_snapshot_symbol": (
            "objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing"
        ),
        "macro_host_process_cache_integration_snapshot_symbol": (
            "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing"
        ),
        "runtime_abi_boundary_model": METAPROGRAMMING_RUNTIME_ABI_BOUNDARY_MODEL,
        "expansion_runtime_model": METAPROGRAMMING_EXPANSION_RUNTIME_MODEL,
        "host_cache_runtime_model": METAPROGRAMMING_HOST_CACHE_RUNTIME_MODEL,
        "fail_closed_model": METAPROGRAMMING_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/preservation_provider.objc3",
            "tests/tooling/fixtures/native/preservation_consumer.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
            "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_metaprogramming_cache_runtime_integration_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"live-metaprogramming-cache-runtime-integration"}
    ]
    return {
        "contract_id": (
            RUNTIME_METAPROGRAMMING_CACHE_RUNTIME_INTEGRATION_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ],
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "macro_host_process_cache_integration_snapshot_symbol": (
            "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing"
        ),
        "implementation_model": (
            "live-host-cache-artifacts-publish-cache-materialization-truth-and-cross-module-runtime-import-consumers-preserve-the-same-host-cache-runtime-boundary"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/macro_host_process_consumer.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp"
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
    }
