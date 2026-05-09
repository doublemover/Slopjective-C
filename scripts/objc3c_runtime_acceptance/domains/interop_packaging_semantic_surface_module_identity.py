"""Package-loading module identity semantic contract surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from .probe_helpers import (
    RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
)
from ..runtime_contract_interop import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
)
from ..runtime_contract_object_model import (
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
)
from ..runtime_contract_registration import MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE


def build_runtime_package_loading_module_identity_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "imported-runtime-packaging-replay",
            "multi-image-registration-reset-replay",
        }
    ]
    return {
        "contract_id": (
            RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.obj",
        ],
        "runtime_probe": IMPORTED_RUNTIME_PACKAGING_PROBE,
        "package_loading_model": (
            "runtime-package-loading-and-reset-replay-preserve-imported-and-local-module-identities-registration-ordinals-and-realized-class-ownership-through-the-live-runtime"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
    }


__all__ = ["build_runtime_package_loading_module_identity_semantics_surface"]
