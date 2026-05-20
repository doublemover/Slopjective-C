"""Concurrency runtime acceptance actor cross-module cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.concurrency_actor_link_plan_assertions import (
    expect_actor_cross_module_link_plan,
    expect_provider_actor_import_surface,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_with_args
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_concurrency import (
    CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE,
    CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
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
    expect_provider_actor_import_surface(provider_actor_surface)

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
    imported_module, local_module = expect_actor_cross_module_link_plan(
        link_plan,
        provider_import_payload,
        provider_registration_manifest,
        consumer_registration_manifest,
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
            "imported_actor_interface_sites": imported_module.get(
                "concurrency_actor_interface_sites"
            ),
            "imported_actor_method_sites": imported_module.get(
                "concurrency_actor_method_sites"
            ),
            "imported_actor_executor_affinity_sites": imported_module.get(
                "concurrency_actor_executor_affinity_sites"
            ),
            "imported_actor_replay_key": imported_module.get(
                "concurrency_actor_mailbox_runtime_replay_key"
            ),
        },
    )


__all__ = ["check_cross_module_concurrency_actor_artifact_preservation_case"]
