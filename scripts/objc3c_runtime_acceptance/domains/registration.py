"""Registration and startup-ordering runtime acceptance domain."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture
from objc3c_runtime_acceptance.native_build import compile_fixture_with_args
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import compile_probe_with_args
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    INSTALLATION_LIFECYCLE_FIXTURE,
    INSTALLATION_LIFECYCLE_PROBE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
    ROOT,
    RUNTIME_ACCEPTANCE_COMMAND,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL,
    RUNTIME_INSTALLATION_ABI_BOUNDARY,
    RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID,
    RUNTIME_LOADER_TESTING_BOUNDARY,
    RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL,
    RUNTIME_PUBLIC_HEADER_PATH,
    VALIDATE_RUNTIME_ARCHITECTURE_COMMAND,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_multi_image_startup_ordering_source_surface",
    "build_runtime_installation_abi_surface",
    "build_runtime_loader_lifecycle_surface",
    "check_installation_lifecycle_case",
    "check_multi_image_registration_reset_replay_case",
]


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


def check_installation_lifecycle_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "installation-lifecycle"
    fixture = ROOT / Path(INSTALLATION_LIFECYCLE_FIXTURE)
    probe = ROOT / Path(INSTALLATION_LIFECYCLE_PROBE)
    compile_started = perf_counter()
    obj_path = compile_fixture(fixture, case_dir / "compile")
    fixture_compile_ms = int((perf_counter() - compile_started) * 1000)
    registration_descriptor = json.loads(
        (case_dir / "compile" / "module.runtime-registration-descriptor.json").read_text(
            encoding="utf-8"
        )
    )
    probe_fixture_config = (
        case_dir / "runtime_installation_loader_lifecycle_probe_fixture_config.h"
    )
    probe_fixture_config.write_text(
        "\n".join(
            [
                "#pragma once",
                f"#define OBJC3_RUNTIME_FIXTURE_MODULE_NAME {json.dumps(registration_descriptor['registration_descriptor_identifier'].removesuffix('_registration_descriptor'))}",
                f"#define OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY {json.dumps(registration_descriptor['translation_unit_identity_key'])}",
                f"#define OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL {registration_descriptor['translation_unit_registration_order_ordinal']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT {registration_descriptor['class_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT {registration_descriptor['protocol_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT {registration_descriptor['category_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT {registration_descriptor['property_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT {registration_descriptor['ivar_descriptor_count']}ULL",
                "",
            ]
        ),
        encoding="utf-8",
    )
    exe_path = case_dir / "runtime_installation_loader_lifecycle_probe.exe"
    probe_link_started = perf_counter()
    compile_probe_with_args(
        clangxx,
        probe,
        exe_path,
        [obj_path],
        [
            "-include",
            str(probe_fixture_config),
        ],
    )
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)
    probe_run_started = perf_counter()
    payload = parse_json_output(
        run_probe(exe_path), "runtime installation loader lifecycle probe"
    )
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    expect(payload.get("startup_registration_copy_status") == 0, "expected startup registration snapshot copy to succeed")
    expect(payload.get("startup_image_walk_copy_status") == 0, "expected startup image walk snapshot copy to succeed")
    expect(payload.get("startup_reset_replay_copy_status") == 0, "expected startup reset/replay snapshot copy to succeed")
    expect(payload.get("startup_registered_image_count") == 1, "expected startup runtime installation state to contain one registered image")
    expect(payload.get("startup_next_expected_registration_order_ordinal") == 2, "expected startup installation state to advance the next registration ordinal")
    expect(payload.get("startup_walked_image_count") == 1, "expected startup image walk state to publish one walked image")
    expect(payload.get("startup_last_discovery_root_entry_count", 0) > 0, "expected startup image walk state to publish a non-empty discovery root")
    expect(payload.get("startup_last_registration_used_staged_table") == 1, "expected startup installation state to consume the staged registration table")
    expect(payload.get("startup_retained_bootstrap_image_count") == 1, "expected startup reset/replay state to retain one bootstrap image")

    startup_module_name = payload.get("startup_last_registered_module_name")
    startup_identity_key = payload.get("startup_last_registered_translation_unit_identity_key")
    expect(isinstance(startup_module_name, str) and startup_module_name != "", "expected startup installation state to publish a registered module name")
    expect(isinstance(startup_identity_key, str) and startup_identity_key != "", "expected startup installation state to publish a registered translation unit identity key")
    expect(payload.get("duplicate_status") == -2, "expected duplicate registration to fail with duplicate translation-unit identity status")
    expect(payload.get("after_duplicate_registration_copy_status") == 0, "expected duplicate rejection registration snapshot copy to succeed")
    expect(payload.get("after_duplicate_image_walk_copy_status") == 0, "expected duplicate rejection image walk snapshot copy to succeed")
    expect(payload.get("after_duplicate_registered_image_count") == 1, "expected duplicate rejection to leave installed image count unchanged")
    expect(payload.get("after_duplicate_next_expected_registration_order_ordinal") == 2, "expected duplicate rejection to preserve the next expected registration ordinal")
    expect(payload.get("after_duplicate_last_successful_registration_order_ordinal") == 1, "expected duplicate rejection to preserve the last successful registration ordinal")
    expect(payload.get("after_duplicate_last_registration_status") == -2, "expected duplicate rejection snapshot to publish duplicate registration status")
    expect(payload.get("after_duplicate_last_rejected_module_name") == startup_module_name, "expected duplicate rejection snapshot to publish the rejected module name")
    expect(payload.get("after_duplicate_last_rejected_translation_unit_identity_key") == startup_identity_key, "expected duplicate rejection snapshot to publish the rejected translation unit identity key")
    expect(payload.get("after_duplicate_last_rejected_registration_order_ordinal") == 1, "expected duplicate rejection snapshot to publish the rejected registration ordinal")
    expect(payload.get("after_duplicate_walked_image_count") == 1, "expected duplicate rejection to leave image walk state unchanged")
    expect(payload.get("out_of_order_status") == -3, "expected out-of-order registration to fail with out-of-order status")
    expect(payload.get("after_out_of_order_registration_copy_status") == 0, "expected out-of-order rejection registration snapshot copy to succeed")
    expect(payload.get("after_out_of_order_image_walk_copy_status") == 0, "expected out-of-order rejection image walk snapshot copy to succeed")
    expect(payload.get("after_out_of_order_registered_image_count") == 1, "expected out-of-order rejection to leave installed image count unchanged")
    expect(payload.get("after_out_of_order_next_expected_registration_order_ordinal") == 2, "expected out-of-order rejection to preserve the next expected registration ordinal")
    expect(payload.get("after_out_of_order_last_successful_registration_order_ordinal") == 1, "expected out-of-order rejection to preserve the last successful registration ordinal")
    expect(payload.get("after_out_of_order_last_registration_status") == -3, "expected out-of-order rejection snapshot to publish out-of-order status")
    expect(payload.get("after_out_of_order_last_rejected_module_name") == "out-of-order-module", "expected out-of-order rejection snapshot to publish the rejected module name")
    expect(
        payload.get("after_out_of_order_last_rejected_translation_unit_identity_key")
        == startup_identity_key + "-out-of-order",
        "expected out-of-order rejection snapshot to publish the rejected translation unit identity key",
    )
    expect(payload.get("after_out_of_order_last_rejected_registration_order_ordinal") == 3, "expected out-of-order rejection snapshot to publish the rejected registration ordinal")
    expect(payload.get("after_out_of_order_walked_image_count") == 1, "expected out-of-order rejection to leave image walk state unchanged")

    expect(payload.get("post_reset_registration_copy_status") == 0, "expected post-reset registration snapshot copy to succeed")
    expect(payload.get("post_reset_reset_replay_copy_status") == 0, "expected post-reset reset/replay snapshot copy to succeed")
    expect(payload.get("post_reset_registered_image_count") == 0, "expected reset to clear installed runtime images")
    expect(payload.get("post_reset_next_expected_registration_order_ordinal") == 1, "expected reset to restore the initial registration ordinal")
    expect(payload.get("post_reset_retained_bootstrap_image_count") == 1, "expected reset to retain one bootstrap image for replay")
    expect(payload.get("post_reset_last_reset_cleared_image_local_init_state_count") == 1, "expected reset to clear one image-local initialization state record")
    expect(payload.get("post_reset_invalid_anchor_status") == -4, "expected mismatched linker-anchor registration to fail with invalid registration roots status")
    expect(payload.get("after_invalid_anchor_registration_copy_status") == 0, "expected invalid-anchor rejection registration snapshot copy to succeed")
    expect(payload.get("after_invalid_anchor_image_walk_copy_status") == 0, "expected invalid-anchor rejection image walk snapshot copy to succeed")
    expect(payload.get("after_invalid_anchor_registered_image_count") == 0, "expected invalid-anchor rejection to preserve the cleared installed image count")
    expect(payload.get("after_invalid_anchor_next_expected_registration_order_ordinal") == 1, "expected invalid-anchor rejection to preserve the reset registration ordinal")
    expect(payload.get("after_invalid_anchor_last_registration_status") == -4, "expected invalid-anchor rejection snapshot to publish invalid registration roots status")
    expect(payload.get("after_invalid_anchor_walked_image_count") == 0, "expected invalid-anchor rejection to leave image walk state empty")
    expect(payload.get("after_invalid_anchor_last_linker_anchor_matches_discovery_root") == 0, "expected invalid-anchor rejection to leave linker-anchor/discovery-root proof unset")
    expect(payload.get("post_reset_invalid_discovery_root_status") == -4, "expected malformed discovery-root registration to fail with invalid registration roots status")
    expect(payload.get("after_invalid_discovery_root_registration_copy_status") == 0, "expected invalid-discovery-root rejection registration snapshot copy to succeed")
    expect(payload.get("after_invalid_discovery_root_image_walk_copy_status") == 0, "expected invalid-discovery-root rejection image walk snapshot copy to succeed")
    expect(payload.get("after_invalid_discovery_root_registered_image_count") == 0, "expected invalid-discovery-root rejection to preserve the cleared installed image count")
    expect(payload.get("after_invalid_discovery_root_next_expected_registration_order_ordinal") == 1, "expected invalid-discovery-root rejection to preserve the reset registration ordinal")
    expect(payload.get("after_invalid_discovery_root_last_registration_status") == -4, "expected invalid-discovery-root rejection snapshot to publish invalid registration roots status")
    expect(payload.get("after_invalid_discovery_root_walked_image_count") == 0, "expected invalid-discovery-root rejection to leave image walk state empty")
    expect(payload.get("after_invalid_discovery_root_last_linker_anchor_matches_discovery_root") == 0, "expected invalid-discovery-root rejection to leave linker-anchor/discovery-root proof unset")

    expect(payload.get("replay_status") == 0, "expected replay_registered_images_for_testing to succeed")
    expect(payload.get("post_replay_registration_copy_status") == 0, "expected post-replay registration snapshot copy to succeed")
    expect(payload.get("post_replay_image_walk_copy_status") == 0, "expected post-replay image walk snapshot copy to succeed")
    expect(payload.get("post_replay_reset_replay_copy_status") == 0, "expected post-replay reset/replay snapshot copy to succeed")
    expect(payload.get("post_replay_registered_image_count") == 1, "expected replay to restore one registered runtime image")
    expect(payload.get("post_replay_next_expected_registration_order_ordinal") == 2, "expected replay to restore the next registration ordinal")
    expect(payload.get("post_replay_walked_image_count") == 1, "expected replay to restore one walked image")
    expect(payload.get("post_replay_last_discovery_root_entry_count") == payload.get("startup_last_discovery_root_entry_count"), "expected replay to preserve discovery root entry count")
    expect(payload.get("post_replay_last_registration_used_staged_table") == 1, "expected replay registration to consume the staged registration table")
    expect(payload.get("post_replay_retained_bootstrap_image_count") == 1, "expected replay to preserve the retained bootstrap catalog")
    expect(payload.get("post_replay_last_replayed_image_count") == 1, "expected replay state to publish one replayed image")
    expect(payload.get("post_replay_replay_generation", 0) >= 1, "expected replay state to advance the replay generation")
    expect(payload.get("post_replay_last_replay_status") == 0, "expected replay state to publish a successful replay status")
    expect(payload.get("post_replay_last_registered_module_name") == startup_module_name, "expected replay to restore the registered module name")
    expect(payload.get("post_replay_last_walked_module_name") == startup_module_name, "expected replay image walk state to publish the registered module name")
    expect(payload.get("post_replay_last_replayed_module_name") == startup_module_name, "expected replay state to publish the replayed module name")
    expect(payload.get("post_replay_last_registered_translation_unit_identity_key") == startup_identity_key, "expected replay to restore the registered translation unit identity key")
    expect(payload.get("post_replay_last_walked_translation_unit_identity_key") == startup_identity_key, "expected replay image walk state to publish the translation unit identity key")
    expect(payload.get("post_replay_last_replayed_translation_unit_identity_key") == startup_identity_key, "expected replay state to publish the replayed translation unit identity key")

    return CaseResult(
        case_id="installation-lifecycle",
        probe="tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "fixture_compile_ms": fixture_compile_ms,
            "probe_link_ms": probe_link_ms,
            "probe_run_ms": probe_run_ms,
            "case_total_ms": case_total_ms,
            "duplicate_status": payload["duplicate_status"],
            "duplicate_rejected_module_name": payload[
                "after_duplicate_last_rejected_module_name"
            ],
            "duplicate_rejected_translation_unit_identity_key": payload[
                "after_duplicate_last_rejected_translation_unit_identity_key"
            ],
            "duplicate_rejected_registration_order_ordinal": payload[
                "after_duplicate_last_rejected_registration_order_ordinal"
            ],
            "out_of_order_status": payload["out_of_order_status"],
            "out_of_order_rejected_module_name": payload[
                "after_out_of_order_last_rejected_module_name"
            ],
            "out_of_order_rejected_translation_unit_identity_key": payload[
                "after_out_of_order_last_rejected_translation_unit_identity_key"
            ],
            "out_of_order_rejected_registration_order_ordinal": payload[
                "after_out_of_order_last_rejected_registration_order_ordinal"
            ],
            "invalid_anchor_status": payload["post_reset_invalid_anchor_status"],
            "invalid_discovery_root_status": payload[
                "post_reset_invalid_discovery_root_status"
            ],
            "startup_registered_image_count": payload["startup_registered_image_count"],
            "post_reset_registered_image_count": payload["post_reset_registered_image_count"],
            "post_replay_registered_image_count": payload[
                "post_replay_registered_image_count"
            ],
            "retained_bootstrap_image_count": payload[
                "post_replay_retained_bootstrap_image_count"
            ],
            "replay_generation": payload["post_replay_replay_generation"],
        },
    )


def check_multi_image_registration_reset_replay_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "multi-image-registration-reset-replay"
    provider_fixture = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE)
    probe = ROOT / Path(MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    provider_obj = compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    consumer_obj = compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    expect(
        link_plan.get("module_image_count") == 2,
        "expected multi-image reset/replay link plan to publish two images",
    )
    expect(
        link_plan.get("module_names_lexicographic")
        == ["runtimePackagingConsumer", "runtimePackagingProvider"],
        "expected multi-image reset/replay link plan to keep deterministic module ordering",
    )
    expect(
        link_plan.get("bootstrap_live_registration_contract_id")
        == "objc3c.runtime.live.registration.discovery.replay.v1",
        "expected multi-image reset/replay link plan to preserve the live registration replay contract",
    )
    expect(
        link_plan.get("bootstrap_reset_for_testing_symbol")
        == "objc3_runtime_reset_for_testing",
        "expected multi-image reset/replay link plan to preserve the reset symbol",
    )
    expect(
        link_plan.get("bootstrap_replay_registered_images_symbol")
        == "objc3_runtime_replay_registered_images_for_testing",
        "expected multi-image reset/replay link plan to preserve the replay symbol",
    )
    expect(
        link_plan.get("bootstrap_reset_replay_state_snapshot_symbol")
        == "objc3_runtime_copy_reset_replay_state_for_testing",
        "expected multi-image reset/replay link plan to preserve the reset/replay snapshot symbol",
    )
    expect(
        link_plan.get("imported_live_registration_replay_ready") is True
        and provider_registration_manifest.get("ready_for_live_registration_discovery_replay") is True
        and consumer_registration_manifest.get("ready_for_live_registration_discovery_replay") is True,
        "expected imported and local manifests to be live-registration replay ready",
    )

    exe_path = case_dir / "multi_image_registration_reset_replay_probe.exe"
    probe_link_started = perf_counter()
    compile_probe(clangxx, probe, exe_path, [provider_obj, consumer_obj])
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)
    probe_run_started = perf_counter()
    payload = parse_json_output(
        run_probe(exe_path), "multi-image registration reset/replay probe"
    )
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    provider_identity = provider_registration_manifest["translation_unit_identity_key"]
    consumer_identity = consumer_registration_manifest["translation_unit_identity_key"]

    for status_key in (
        "startup_registration_status",
        "startup_walk_status",
        "startup_graph_status",
        "startup_provider_status",
        "startup_consumer_status",
        "startup_replay_status",
        "first_reset_registration_status",
        "first_reset_replay_status",
        "first_replay_registration_status",
        "first_replay_walk_status",
        "first_replay_graph_status",
        "first_replay_provider_status",
        "first_replay_consumer_status",
        "first_replay_state_status",
        "blocked_replay_state_status",
        "second_reset_registration_status",
        "second_reset_replay_status",
        "second_replay_registration_status",
        "second_replay_walk_status",
        "second_replay_graph_status",
        "second_replay_provider_status",
        "second_replay_consumer_status",
        "second_replay_state_status",
    ):
        expect(payload.get(status_key) == 0, f"expected {status_key} to succeed")

    expect(payload.get("startup_registered_image_count") == 2, "expected startup to install two images")
    expect(payload.get("startup_next_expected_registration_order_ordinal") == 3, "expected startup next registration ordinal to be three")
    expect(payload.get("startup_walked_image_count") == 2, "expected startup to walk two images")
    expect(payload.get("startup_realized_class_count") == 2, "expected startup to realize two classes")
    expect(payload.get("startup_retained_bootstrap_image_count") == 2, "expected startup to retain two bootstrap images")
    expect(payload.get("startup_last_walked_module_name") == "runtimePackagingConsumer", "expected startup to walk the local consumer image last")
    expect(payload.get("startup_provider_registration_order_ordinal") == 1, "expected provider ordinal one at startup")
    expect(payload.get("startup_consumer_registration_order_ordinal") == 2, "expected consumer ordinal two at startup")
    expect(payload.get("startup_provider_identity") == provider_identity, "expected startup provider identity to match the provider manifest")
    expect(payload.get("startup_consumer_identity") == consumer_identity, "expected startup consumer identity to match the consumer manifest")

    expect(payload.get("first_reset_registered_image_count") == 0, "expected first reset to clear installed images")
    expect(payload.get("first_reset_next_expected_registration_order_ordinal") == 1, "expected first reset to restore ordinal one")
    expect(payload.get("first_reset_retained_bootstrap_image_count") == 2, "expected first reset to retain both bootstrap images")
    expect(payload.get("first_reset_cleared_image_local_init_state_count") == 2, "expected first reset to clear two image-local init states")
    expect(payload.get("first_reset_generation") == 1, "expected first reset generation to advance to one")

    expect(payload.get("first_replay_status") == 0, "expected first replay to succeed")
    expect(payload.get("first_replay_registered_image_count") == 2, "expected first replay to restore two images")
    expect(payload.get("first_replay_next_expected_registration_order_ordinal") == 3, "expected first replay to restore ordinal three")
    expect(payload.get("first_replay_walked_image_count") == 2, "expected first replay to walk two images")
    expect(payload.get("first_replay_realized_class_count") == 2, "expected first replay to restore two realized classes")
    expect(payload.get("first_replay_last_replayed_image_count") == 2, "expected first replay to publish two replayed images")
    expect(payload.get("first_replay_generation") == 1, "expected first replay generation to advance to one")
    expect(payload.get("first_replay_last_walked_module_name") == "runtimePackagingConsumer", "expected first replay to walk the consumer image last")
    expect(payload.get("first_replay_last_replayed_module_name") == "runtimePackagingConsumer", "expected first replay to replay the consumer image last")
    expect(payload.get("first_replay_provider_registration_order_ordinal") == 1, "expected first replay provider ordinal one")
    expect(payload.get("first_replay_consumer_registration_order_ordinal") == 2, "expected first replay consumer ordinal two")
    expect(payload.get("first_replay_provider_identity") == provider_identity, "expected first replay provider identity to survive reset")
    expect(payload.get("first_replay_consumer_identity") == consumer_identity, "expected first replay consumer identity to survive reset")

    expect(payload.get("replay_without_reset_status") == -1, "expected replay without reset to fail closed")
    expect(payload.get("blocked_replay_last_replay_status") == -1, "expected blocked replay status to be captured")
    expect(payload.get("blocked_replay_last_replayed_image_count") == 0, "expected blocked replay to avoid replaying images")

    expect(payload.get("second_reset_registered_image_count") == 0, "expected second reset to clear installed images")
    expect(payload.get("second_reset_next_expected_registration_order_ordinal") == 1, "expected second reset to restore ordinal one")
    expect(payload.get("second_reset_retained_bootstrap_image_count") == 2, "expected second reset to retain both bootstrap images")
    expect(payload.get("second_reset_cleared_image_local_init_state_count") == 2, "expected second reset to clear two image-local init states")
    expect(payload.get("second_reset_generation") == 2, "expected second reset generation to advance to two")

    expect(payload.get("second_replay_status") == 0, "expected second replay to succeed")
    expect(payload.get("second_replay_registered_image_count") == 2, "expected second replay to restore two images")
    expect(payload.get("second_replay_next_expected_registration_order_ordinal") == 3, "expected second replay to restore ordinal three")
    expect(payload.get("second_replay_walked_image_count") == 2, "expected second replay to walk two images")
    expect(payload.get("second_replay_realized_class_count") == 2, "expected second replay to restore two realized classes")
    expect(payload.get("second_replay_last_replayed_image_count") == 2, "expected second replay to publish two replayed images")
    expect(payload.get("second_replay_generation") == 2, "expected second replay generation to advance to two")
    expect(payload.get("second_replay_last_walked_module_name") == "runtimePackagingConsumer", "expected second replay to walk the consumer image last")
    expect(payload.get("second_replay_last_replayed_module_name") == "runtimePackagingConsumer", "expected second replay to replay the consumer image last")
    expect(payload.get("second_replay_provider_registration_order_ordinal") == 1, "expected second replay provider ordinal one")
    expect(payload.get("second_replay_consumer_registration_order_ordinal") == 2, "expected second replay consumer ordinal two")
    expect(payload.get("second_replay_provider_identity") == provider_identity, "expected second replay provider identity to survive reset")
    expect(payload.get("second_replay_consumer_identity") == consumer_identity, "expected second replay consumer identity to survive reset")

    return CaseResult(
        case_id="multi-image-registration-reset-replay",
        probe=MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
        fixture=IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "provider_fixture": IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            "provider_import_surface": str(provider_import_surface.relative_to(ROOT)).replace("\\", "/"),
            "link_plan": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "probe_link_ms": probe_link_ms,
            "probe_run_ms": probe_run_ms,
            "case_total_ms": case_total_ms,
            "module_image_count": link_plan.get("module_image_count"),
            "first_replay_generation": payload["first_replay_generation"],
            "second_replay_generation": payload["second_replay_generation"],
            "blocked_replay_status": payload["replay_without_reset_status"],
            "provider_translation_unit_identity_key": provider_identity,
            "consumer_translation_unit_identity_key": consumer_identity,
        },
    )


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
