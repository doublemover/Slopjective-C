"""Registration runtime acceptance lifecycle cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture
from objc3c_runtime_acceptance.probes import compile_probe_with_args
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    INSTALLATION_LIFECYCLE_FIXTURE,
    INSTALLATION_LIFECYCLE_PROBE,
    ROOT,
)

_EXPORTED_CASE_NAMES = ["check_installation_lifecycle_case"]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


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


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
