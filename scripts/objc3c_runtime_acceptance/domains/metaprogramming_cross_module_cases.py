"""Metaprogramming cross-module runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_cross_module_assertions import (
    expect_cross_module_link_plan,
    expect_imported_metaprogramming_module,
    expect_provider_host_cache_surface,
    expect_provider_replay_surface,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_with_args
from objc3c_runtime_acceptance.paths import ROOT


def check_cross_module_metaprogramming_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-metaprogramming-artifact-preservation"
    provider_fixture = (
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "preservation_provider.objc3"
    )
    consumer_fixture = (
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "preservation_consumer.objc3"
    )

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface_path = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface_path.read_text(encoding="utf-8")
    )
    provider_replay_surface = provider_import_payload.get(
        "objc_metaprogramming_module_interface_and_replay_preservation", {}
    )
    provider_host_cache_surface = provider_import_payload.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    expect_provider_replay_surface(provider_replay_surface)
    expect_provider_host_cache_surface(
        provider_host_cache_surface, provider_replay_surface
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
            str(provider_import_surface_path),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    expect_cross_module_link_plan(link_plan, provider_import_payload)

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module metaprogramming link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect_imported_metaprogramming_module(
        imported_module, provider_import_payload, provider_host_cache_surface
    )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-metaprogramming-artifact-preservation",
        probe=None,
        fixture="tests/tooling/fixtures/native/preservation_provider.objc3",
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": "tests/tooling/fixtures/native/preservation_provider.objc3",
            "consumer_fixture": "tests/tooling/fixtures/native/preservation_consumer.objc3",
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": link_plan.get("local_module", {}).get("module_name"),
            "imported_module_names": link_plan.get(
                "metaprogramming_host_cache_imported_module_names_lexicographic"
            ),
        },
    )
