"""Registration runtime acceptance surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.commands import (
    RUNTIME_ACCEPTANCE_COMMAND,
    VALIDATE_RUNTIME_ARCHITECTURE_COMMAND,
)

from ..c_api import (
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_INSTALLATION_ABI_BOUNDARY,
    RUNTIME_LOADER_TESTING_BOUNDARY,
    RUNTIME_PUBLIC_HEADER_PATH,
)
from ..runtime_contract_interop import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
)
from ..runtime_contract_registration import (
    INSTALLATION_LIFECYCLE_FIXTURE,
    INSTALLATION_LIFECYCLE_PROBE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
    RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL,
    RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID,
    RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_multi_image_startup_ordering_source_surface",
    "build_runtime_installation_abi_surface",
    "build_runtime_loader_lifecycle_surface",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def build_runtime_multi_image_startup_ordering_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "installation-lifecycle",
            "multi-image-registration-reset-replay",
            "imported-runtime-packaging-replay",
        }
    ]
    installation_lifecycle = next(
        (result for result in results if result.case_id == "installation-lifecycle"),
        None,
    )
    multi_image_replay = next(
        (
            result
            for result in results
            if result.case_id == "multi-image-registration-reset-replay"
        ),
        None,
    )
    latest_summary = (
        installation_lifecycle.summary if installation_lifecycle is not None else {}
    )
    return {
        "contract_id": RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID,
        "runtime_installation_abi_surface_contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "runtime_loader_lifecycle_surface_contract_id": RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID,
        "authoritative_case_ids": authoritative_case_ids,
        "fixture_path": INSTALLATION_LIFECYCLE_FIXTURE,
        "probe_path": INSTALLATION_LIFECYCLE_PROBE,
        "fixture_paths": [
            INSTALLATION_LIFECYCLE_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        ],
        "probe_paths": [
            INSTALLATION_LIFECYCLE_PROBE,
            MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
            IMPORTED_RUNTIME_PACKAGING_PROBE,
        ],
        "runtime_public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "runtime_bootstrap_internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "validation_commands": [
            RUNTIME_ACCEPTANCE_COMMAND,
            VALIDATE_RUNTIME_ARCHITECTURE_COMMAND,
        ],
        "runtime_symbols": [
            "objc3_runtime_register_image",
            "objc3_runtime_copy_registration_state_for_testing",
            "objc3_runtime_reset_for_testing",
            "objc3_runtime_replay_registered_images_for_testing",
            "objc3_runtime_copy_reset_replay_state_for_testing",
        ],
        "diagnostic_status_codes": {
            "duplicate_registration": -2,
            "out_of_order_registration": -3,
        },
        "diagnostic_models": {
            "duplicate_install": RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL,
            "out_of_order_install": RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL,
        },
        "rejected_registration_fields": [
            "last_rejected_module_name",
            "last_rejected_translation_unit_identity_key",
            "last_rejected_registration_order_ordinal",
        ],
        "ordering_snapshot_fields": [
            "next_expected_registration_order_ordinal",
            "last_successful_registration_order_ordinal",
            "last_rejected_registration_order_ordinal",
        ],
        "measured_summary_fields": [
            "fixture_compile_ms",
            "probe_link_ms",
            "probe_run_ms",
            "case_total_ms",
        ],
        "latest_installation_lifecycle_measurements": latest_summary,
        "latest_multi_image_reset_replay_measurements": (
            multi_image_replay.summary if multi_image_replay is not None else {}
        ),
        "latest_duplicate_install_diagnostic": {
            "status": latest_summary.get("duplicate_status"),
            "rejected_module_name": latest_summary.get("duplicate_rejected_module_name"),
            "rejected_translation_unit_identity_key": latest_summary.get(
                "duplicate_rejected_translation_unit_identity_key"
            ),
            "rejected_registration_order_ordinal": latest_summary.get(
                "duplicate_rejected_registration_order_ordinal"
            ),
        },
        "latest_out_of_order_install_diagnostic": {
            "status": latest_summary.get("out_of_order_status"),
            "rejected_module_name": latest_summary.get("out_of_order_rejected_module_name"),
            "rejected_translation_unit_identity_key": latest_summary.get(
                "out_of_order_rejected_translation_unit_identity_key"
            ),
            "rejected_registration_order_ordinal": latest_summary.get(
                "out_of_order_rejected_registration_order_ordinal"
            ),
        },
    }


def build_runtime_installation_abi_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "bootstrap_api_contract_id": "objc3c.runtime.bootstrap.api.freeze.v1",
        "bootstrap_reset_contract_id": "objc3c.runtime.bootstrap.reset.replay.v1",
        "bootstrap_registrar_contract_id": "objc3c.runtime.bootstrap.registrar.image.walk.v1",
        "public_installation_abi_boundary": RUNTIME_INSTALLATION_ABI_BOUNDARY,
        "private_loader_testing_boundary": RUNTIME_LOADER_TESTING_BOUNDARY,
        "installation_requires_coupled_registration_manifest": True,
        "register_image_consumes_staged_registration_table_once": True,
        "deterministic_reset_replay_supported": True,
    }


def build_runtime_loader_lifecycle_surface(results: list[CaseResult]) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id for result in results if result.case_id == "installation-lifecycle"
    ]
    return {
        "contract_id": RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID,
        "runtime_installation_abi_surface_contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "bootstrap_semantics_contract_id": "objc3c.runtime.startup.bootstrap.semantics.v1",
        "bootstrap_reset_contract_id": "objc3c.runtime.bootstrap.reset.replay.v1",
        "bootstrap_registrar_contract_id": "objc3c.runtime.bootstrap.registrar.image.walk.v1",
        "authoritative_probe_path": INSTALLATION_LIFECYCLE_PROBE,
        "authoritative_case_ids": authoritative_case_ids,
        "loader_testing_boundary_symbols": RUNTIME_LOADER_TESTING_BOUNDARY,
        "lifecycle_phases": [
            "startup-installed-runtime-state",
            "duplicate-registration-rejected-without-state-advance",
            "out-of-order-registration-rejected-without-state-advance",
            "invalid-anchor-root-rejected-without-state-advance",
            "invalid-discovery-root-rejected-without-state-advance",
            "reset-retained-bootstrap-catalog",
            "replay-restored-installed-runtime-state",
        ],
        "rejected_registration_status_codes": {
            "duplicate_translation_unit_identity_key": -2,
            "out_of_order_registration": -3,
            "invalid_registration_roots": -4,
        },
        "retained_bootstrap_catalog_required": True,
        "deterministic_replay_required": True,
        "requires_linked_fixture_or_loader_retained_roots": True,
    }


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
