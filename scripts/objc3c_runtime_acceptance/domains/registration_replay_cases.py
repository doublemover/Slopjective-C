"""Registration runtime acceptance reset/replay cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import ROOT, compile_fixture_with_args
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..runtime_contracts import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
)

_EXPORTED_CASE_NAMES = ["check_multi_image_registration_reset_replay_case"]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


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


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
