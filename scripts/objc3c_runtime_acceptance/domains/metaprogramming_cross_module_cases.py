"""Metaprogramming cross-module runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_with_args

from ..core import ROOT


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
    expect(
        isinstance(provider_replay_surface, dict),
        "expected metaprogramming preservation provider import surface to publish the replay-preservation packet",
    )
    expect(
        provider_replay_surface.get("contract_id")
        == "objc3c.metaprogramming.module.interface.replay.preservation.v1",
        "expected metaprogramming preservation provider import surface to preserve the replay-preservation contract",
    )
    expect(
        provider_replay_surface.get("source_contract_id")
        == "objc3c.metaprogramming.synthesized.ast.ir.emission.v1",
        "expected metaprogramming preservation provider import surface to preserve the synthesized emission source contract",
    )
    expect(
        provider_replay_surface.get("local_derive_method_count") == 1
        and provider_replay_surface.get("local_macro_artifact_count") == 1
        and provider_replay_surface.get("local_interface_property_behavior_artifact_count")
        == 1
        and provider_replay_surface.get(
            "local_implementation_property_behavior_artifact_count"
        )
        == 1
        and provider_replay_surface.get("local_runtime_method_list_count") == 1,
        "expected metaprogramming preservation provider import surface to preserve local metaprogramming artifact counts",
    )
    expect(
        provider_replay_surface.get("runtime_import_artifact_ready") is True
        and provider_replay_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_replay_surface.get("deterministic") is True,
        "expected metaprogramming preservation provider import surface to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        isinstance(provider_replay_surface.get("replay_key"), str)
        and provider_replay_surface.get("replay_key") != "",
        "expected metaprogramming preservation provider import surface to publish a replay key",
    )

    expect(
        isinstance(provider_host_cache_surface, dict),
        "expected metaprogramming preservation provider import surface to publish the host-cache packet",
    )
    expect(
        provider_host_cache_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected metaprogramming preservation provider import surface to preserve the host-cache contract",
    )
    expect(
        provider_host_cache_surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        "expected metaprogramming preservation provider import surface to preserve the host runtime boundary contract",
    )
    expect(
        provider_host_cache_surface.get("local_macro_artifact_count") == 1
        and provider_host_cache_surface.get("local_property_behavior_artifact_count")
        == 2
        and provider_host_cache_surface.get("imported_module_count") == 0,
        "expected metaprogramming preservation provider import surface to preserve host-cache local artifact counts",
    )
    expect(
        provider_host_cache_surface.get("runtime_import_artifact_ready") is True
        and provider_host_cache_surface.get("separate_compilation_ready") is True
        and provider_host_cache_surface.get("deterministic") is True,
        "expected metaprogramming preservation provider host-cache packet to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        provider_host_cache_surface.get("metaprogramming_replay_key")
        == provider_replay_surface.get("replay_key"),
        "expected provider host-cache packet to preserve the metaprogramming replay key",
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

    for field_name, expected_value in (
        (
            "expected_metaprogramming_host_cache_contract_id",
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        ),
        (
            "expected_metaprogramming_host_cache_source_contract_id",
            "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        ),
        (
            "expected_metaprogramming_host_cache_executable_relative_path",
            "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        ),
        (
            "expected_metaprogramming_host_cache_root_relative_path",
            "tmp/artifacts/objc3c-native/cache/metaprogramming",
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module metaprogramming link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("metaprogramming_host_cache_imported_module_count") == 1
        and link_plan.get("metaprogramming_host_cache_imported_module_names_lexicographic")
        == [provider_import_payload.get("module_name")]
        and link_plan.get("metaprogramming_host_cache_cross_module_preservation_ready")
        is True,
        "expected cross-module metaprogramming link plan to preserve host-cache imported module readiness",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module metaprogramming link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == provider_import_payload.get("module_name")
        == "MetaprogrammingPreservationProvider",
        "expected cross-module metaprogramming link plan to preserve the provider module name",
    )
    for field_name, expected_value in (
        ("metaprogramming_macro_host_process_cache_runtime_integration_present", True),
        ("metaprogramming_macro_host_process_cache_runtime_ready", True),
        ("metaprogramming_macro_host_process_cache_separate_compilation_ready", True),
        ("metaprogramming_macro_host_process_cache_deterministic", True),
        (
            "metaprogramming_macro_host_process_cache_contract_id",
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        ),
        (
            "metaprogramming_macro_host_process_cache_source_contract_id",
            "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        ),
        (
            "metaprogramming_macro_host_process_cache_host_executable_relative_path",
            "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        ),
        (
            "metaprogramming_macro_host_process_cache_root_relative_path",
            "tmp/artifacts/objc3c-native/cache/metaprogramming",
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported metaprogramming module to preserve {field_name}",
        )
    expect(
        imported_module.get("metaprogramming_macro_host_process_cache_replay_key")
        == provider_host_cache_surface.get("replay_key"),
        "expected imported metaprogramming module to preserve the provider host-cache replay key",
    )
    imported_replay_key = imported_module.get(
        "metaprogramming_macro_host_process_cache_replay_key", ""
    )
    for snippet in (
        "objc_metaprogramming_module_interface_and_replay_preservation",
        "local_derive_method_count=1",
        "local_macro_artifact_count=1",
        "local_interface_property_behavior_artifact_count=1",
        "local_implementation_property_behavior_artifact_count=1",
        "local_runtime_method_list_count=1",
        "emitted_runtime_method_list_sites=1",
    ):
        expect(
            snippet in imported_replay_key,
            f"expected imported metaprogramming replay key to preserve {snippet}",
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
