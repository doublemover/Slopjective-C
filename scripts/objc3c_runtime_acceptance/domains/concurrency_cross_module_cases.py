"""Concurrency runtime acceptance cross-module cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_with_args

from ..core import (
    CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE,
    CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
    ROOT,
)


def check_cross_module_concurrency_actor_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-concurrency-actor-artifact-preservation"
    provider_fixture = ROOT / Path(CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    provider_actor_surface = provider_import_payload.get(
        "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface", {}
    )
    expect(
        isinstance(provider_actor_surface, dict),
        "expected concurrency actor provider import surface to publish the actor mailbox preservation packet",
    )
    expected_provider_fields = {
        "contract_id": "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        "source_contract_id": "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface",
        "source_model": "runtime-import-surface-preserves-actor-lowering-and-isolation-replay-facts-for-cross-module-runtime-link-planning",
        "fail_closed_model": "missing-or-drifted-actor-mailbox-runtime-import-packets-disable-cross-module-actor-runtime-preservation-claims",
    }
    for field_name, expected_value in expected_provider_fields.items():
        expect(
            provider_actor_surface.get(field_name) == expected_value,
            f"expected concurrency actor provider import surface to preserve {field_name}",
        )
    expect(
        provider_actor_surface.get("actor_mailbox_runtime_ready") is True
        and provider_actor_surface.get("deterministic") is True,
        "expected concurrency actor provider import surface to be runtime-ready and deterministic",
    )
    for field_name in (
        "replay_key",
        "actor_lowering_replay_key",
        "actor_isolation_lowering_replay_key",
    ):
        expect(
            isinstance(provider_actor_surface.get(field_name), str)
            and provider_actor_surface.get(field_name) != "",
            f"expected concurrency actor provider import surface to publish {field_name}",
        )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
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
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    expect(
        link_plan.get("concurrency_actor_imported_module_count") == 1,
        "expected cross-module link plan to publish one imported actor module",
    )
    expect(
        link_plan.get("concurrency_actor_imported_module_names_lexicographic")
        == [provider_import_payload.get("module_name")],
        "expected cross-module link plan to preserve the imported actor module names",
    )
    expect(
        link_plan.get("concurrency_actor_cross_module_isolation_ready") is True,
        "expected cross-module link plan to mark actor cross-module isolation ready",
    )
    expect(
        link_plan.get("expected_concurrency_actor_contract_id")
        == "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        "expected cross-module link plan to preserve the actor import contract id",
    )
    expect(
        link_plan.get("expected_concurrency_actor_source_contract_id")
        == "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "expected cross-module link plan to preserve the actor source contract id",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module actor link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name") == provider_import_payload.get("module_name"),
        "expected imported actor module name to match the provider import surface",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected imported actor module registration ordinal to remain one",
    )
    expect(
        imported_module.get("concurrency_actor_mailbox_runtime_import_present") is True
        and imported_module.get("concurrency_actor_mailbox_runtime_ready") is True
        and imported_module.get("concurrency_actor_mailbox_runtime_deterministic")
        is True,
        "expected imported actor module to preserve actor runtime import readiness",
    )
    for field_name, expected_value in (
        (
            "concurrency_actor_contract_id",
            "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        ),
        (
            "concurrency_actor_source_contract_id",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported actor module to preserve {field_name}",
        )
    for field_name in (
        "concurrency_actor_mailbox_runtime_replay_key",
        "concurrency_actor_lowering_replay_key",
        "concurrency_actor_isolation_lowering_replay_key",
    ):
        expect(
            isinstance(imported_module.get(field_name), str)
            and imported_module.get(field_name) != "",
            f"expected imported actor module to preserve {field_name}",
        )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            imported_module.get(field_name) == provider_registration_manifest.get(field_name),
            f"expected imported actor module to preserve {field_name}",
        )

    local_module = link_plan.get("local_module", {})
    expect(
        local_module.get("module_name") == "M270D003Consumer"
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module actor consumer to preserve the local module identity and registration ordinal",
    )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            local_module.get(field_name) == consumer_registration_manifest.get(field_name),
            f"expected local actor module to preserve {field_name}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-concurrency-actor-artifact-preservation",
        probe=None,
        fixture=CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
            "consumer_fixture": CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": local_module.get("module_name"),
            "imported_actor_registration_ordinal": imported_module.get(
                "translation_unit_registration_order_ordinal"
            ),
            "local_actor_registration_ordinal": local_module.get(
                "translation_unit_registration_order_ordinal"
            ),
        },
    )
