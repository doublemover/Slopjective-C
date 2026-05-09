"""Metaprogramming semantic runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    NegativeDiagnosticExpectation,
    compile_fixture_outputs,
    compile_negative_diagnostic_batch,
    run_fixture_compile,
)

from ..core import ROOT

def check_metaprogramming_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "metaprogramming-semantics"
    fixtures: dict[str, tuple[Path, int, int, int]] = {
        "semantic_model": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_behavior_semantic_model_positive.objc3",
            0,
            1,
            2,
        ),
        "lowering_ready": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_lowering_positive.objc3",
            1,
            1,
            2,
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        expected_derive_marker_sites,
        expected_macro_marker_sites,
        expected_property_behavior_sites,
    ) in fixtures.items():
        _, _, manifest_path = compile_fixture_outputs(fixture_path, case_dir / fixture_key / "compile")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get("objc_metaprogramming_expansion_and_behavior_semantic_model", {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish objc_metaprogramming_expansion_and_behavior_semantic_model",
        )
        expect(
            semantic_surface.get("contract_id")
            == "objc3c.metaprogramming.expansion.behavior.semantic.model.v1",
            f"expected {fixture_key} fixture to preserve the metaprogramming expansion behavior semantic contract",
        )
        expect(
            semantic_surface.get("frontend_dependency_contract_id")
            == "objc3c.metaprogramming.property.behavior.source.completion.v1",
            f"expected {fixture_key} fixture to preserve the property behavior source dependency contract",
        )
        expect(
            semantic_surface.get("derive_marker_sites") == expected_derive_marker_sites
            and semantic_surface.get("macro_marker_sites") == expected_macro_marker_sites
            and semantic_surface.get("property_behavior_sites")
            == expected_property_behavior_sites,
            f"expected {fixture_key} fixture to preserve semantic site counts",
        )
        expect(
            semantic_surface.get("macro_package_provenance_surface_reused") is True
            and semantic_surface.get("property_behavior_source_supported") is True
            and semantic_surface.get("synthesized_visibility_surface_reused")
            is True,
            f"expected {fixture_key} fixture to preserve semantic surface reuse flags",
        )
        expect(
            semantic_surface.get("derive_synthesis_deferred") is True
            and semantic_surface.get("macro_execution_deferred") is True
            and semantic_surface.get("property_behavior_runtime_deferred") is True,
            f"expected {fixture_key} fixture to preserve deferred runtime semantics",
        )
        expect(
            semantic_surface.get("deterministic") is True
            and semantic_surface.get("ready_for_core_implementation") is True,
            f"expected {fixture_key} fixture to preserve deterministic semantic readiness",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "contract_id": semantic_surface.get("contract_id"),
            "derive_marker_sites": semantic_surface.get("derive_marker_sites"),
            "macro_marker_sites": semantic_surface.get("macro_marker_sites"),
            "property_behavior_sites": semantic_surface.get("property_behavior_sites"),
        }

    return CaseResult(
        case_id="metaprogramming-semantics",
        probe="compile-manifest-semantic-surface",
        fixture="tests/tooling/fixtures/native/expansion_behavior_semantic_model_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )

def check_metaprogramming_derive_property_behavior_semantics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-derive-property-behavior-semantics"
    derive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "derive_expansion_inventory_positive.objc3"
    )
    property_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_behavior_legality_positive.objc3"
    )
    _, _, derive_manifest_path = compile_fixture_outputs(
        derive_fixture, case_dir / "derive-positive" / "compile"
    )
    _, _, property_manifest_path = compile_fixture_outputs(
        property_fixture, case_dir / "property-positive" / "compile"
    )
    derive_manifest = json.loads(derive_manifest_path.read_text(encoding="utf-8"))
    property_manifest = json.loads(property_manifest_path.read_text(encoding="utf-8"))
    derive_surface = (
        derive_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_derive_expansion_inventory", {})
    )
    property_surface = (
        property_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get(
            "objc_metaprogramming_property_behavior_legality_and_interaction_completion",
            {},
        )
    )
    expect(
        derive_surface.get("contract_id")
        == "objc3c.metaprogramming.derive.expansion.inventory.v1",
        "expected derive expansion inventory fixture to preserve the derive inventory contract",
    )
    expect(
        derive_surface.get("derive_request_sites") == 4
        and derive_surface.get("supported_derive_request_sites") == 4
        and derive_surface.get("generated_method_entry_count") == 4,
        "expected derive expansion inventory fixture to preserve supported derive counts",
    )
    expect(
        derive_surface.get("equatable_alias_sites") == 1
        and derive_surface.get("equality_derive_sites") == 2
        and derive_surface.get("hash_derive_sites") == 1
        and derive_surface.get("debug_description_derive_sites") == 1,
        "expected derive expansion inventory fixture to preserve derive family counts",
    )
    expect(
        derive_surface.get("unsupported_derive_fail_closed") is True
        and derive_surface.get("selector_conflicts_fail_closed") is True
        and derive_surface.get("deterministic") is True
        and derive_surface.get("ready_for_lowering_and_runtime") is True,
        "expected derive expansion inventory fixture to preserve fail-closed readiness",
    )
    expect(
        property_surface.get("contract_id")
        == "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1",
        "expected property behavior legality fixture to preserve the legality contract",
    )
    expect(
        property_surface.get("property_behavior_sites") == 5
        and property_surface.get("supported_behavior_sites") == 5
        and property_surface.get("unsupported_behavior_sites") == 0,
        "expected property behavior legality fixture to preserve behavior counts",
    )
    expect(
        property_surface.get("observed_behavior_sites") == 2
        and property_surface.get("projected_behavior_sites") == 3,
        "expected property behavior legality fixture to preserve observed/projected counts",
    )
    expect(
        property_surface.get("unsupported_behavior_fail_closed") is True
        and property_surface.get("owner_topology_fail_closed") is True
        and property_surface.get("interaction_legality_fail_closed") is True
        and property_surface.get("storage_legality_fail_closed") is True
        and property_surface.get("deterministic") is True
        and property_surface.get("ready_for_lowering_and_runtime") is True,
        "expected property behavior legality fixture to preserve fail-closed readiness",
    )

    negative_fixtures = {
        "unsupported_derive": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "derive_expansion_inventory_negative_unsupported.objc3",
            "O3S317",
            "unsupported derive 'Networked'",
        ),
        "unsupported_behavior": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_unsupported.objc3",
            "O3S326",
            "unsupported property behavior 'Cached'",
        ),
        "nonobject_behavior": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_nonobject.objc3",
            "O3S327",
            "requires an Objective-C object property",
        ),
        "protocol_observed": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_protocol_observed.objc3",
            "O3S328",
            "requires a concrete interface or implementation property",
        ),
        "projected_writable": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_projected_writable.objc3",
            "O3S330",
            "requires a readonly getter-only property",
        ),
    }
    negative_summary: dict[str, Any] = {}
    for negative_key, (fixture_path, expected_code, expected_message) in negative_fixtures.items():
        compile_dir = case_dir / negative_key / "compile"
        compile_result, _ = run_fixture_compile(
            fixture_path,
            compile_dir,
            write_provenance=False,
        )
        expect(
            compile_result.returncode != 0,
            f"expected negative metaprogramming fixture {negative_key} to fail compilation",
        )
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        expect(diagnostics_path.is_file(), f"expected diagnostics for negative fixture {negative_key}")
        diagnostics_text = diagnostics_path.read_text(encoding="utf-8")
        expect(
            expected_code in diagnostics_text and expected_message in diagnostics_text,
            f"expected negative metaprogramming fixture {negative_key} to preserve {expected_code}",
        )
        negative_summary[negative_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "diagnostics": str(diagnostics_path.relative_to(ROOT)).replace("\\", "/"),
            "expected_code": expected_code,
        }

    return CaseResult(
        case_id="metaprogramming-derive-property-behavior-semantics",
        probe="compile-manifest-derive-property-behavior-semantics",
        fixture="tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "derive_positive": {
                "fixture": str(derive_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(derive_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "generated_method_entry_count": derive_surface.get(
                    "generated_method_entry_count"
                ),
                "expansion_inventory_rows_lexicographic": derive_surface.get(
                    "expansion_inventory_rows_lexicographic"
                ),
            },
            "property_positive": {
                "fixture": str(property_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(property_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "supported_behavior_sites": property_surface.get(
                    "supported_behavior_sites"
                ),
                "observed_behavior_sites": property_surface.get(
                    "observed_behavior_sites"
                ),
                "projected_behavior_sites": property_surface.get(
                    "projected_behavior_sites"
                ),
            },
            "negative_cases": negative_summary,
        },
    )

def check_metaprogramming_macro_safety_cache_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-macro-safety-cache-diagnostics"
    positive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_provider.objc3"
    )
    _, _, positive_manifest_path = compile_fixture_outputs(
        positive_fixture, case_dir / "positive" / "compile"
    )
    positive_manifest = json.loads(positive_manifest_path.read_text(encoding="utf-8"))
    macro_safety_surface = (
        positive_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics", {})
    )
    expect(
        macro_safety_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1",
        "expected macro host process provider fixture to preserve the macro safety semantic contract",
    )
    expect(
        macro_safety_surface.get("macro_marker_sites") == 1
        and macro_safety_surface.get("macro_package_sites") == 1
        and macro_safety_surface.get("macro_provenance_sites") == 1
        and macro_safety_surface.get("expansion_visible_macro_sites") == 1,
        "expected macro host process provider fixture to preserve macro metadata counts",
    )
    expect(
        macro_safety_surface.get("safe_macro_callable_sites") == 1
        and macro_safety_surface.get("incomplete_macro_metadata_sites") == 0
        and macro_safety_surface.get("orphan_macro_metadata_sites") == 0
        and macro_safety_surface.get("invalid_package_sites") == 0
        and macro_safety_surface.get("invalid_provenance_sites") == 0
        and macro_safety_surface.get("nondeterministic_callable_sites") == 0
        and macro_safety_surface.get("unsupported_callable_topology_sites") == 0,
        "expected macro host process provider fixture to preserve fail-closed macro safety counts",
    )
    expect(
        macro_safety_surface.get("metadata_completeness_enforced") is True
        and macro_safety_surface.get("sandbox_namespace_enforced") is True
        and macro_safety_surface.get("provenance_determinism_enforced") is True
        and macro_safety_surface.get("callable_determinism_enforced") is True
        and macro_safety_surface.get("deterministic") is True
        and macro_safety_surface.get("ready_for_lowering_and_runtime") is True,
        "expected macro host process provider fixture to preserve deterministic fail-closed enforcement flags",
    )

    host_cache_path = (
        case_dir / "positive" / "compile" / "module.metaprogramming-macro-host-cache.json"
    )
    expect(
        host_cache_path.is_file(),
        "expected macro host process provider fixture to publish module.metaprogramming-macro-host-cache.json",
    )
    host_cache_surface = json.loads(host_cache_path.read_text(encoding="utf-8"))
    expect(
        host_cache_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected macro host process provider fixture to publish the metaprogramming host-cache integration contract",
    )
    expect(
        host_cache_surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        "expected macro host process provider fixture to preserve the host runtime boundary source contract",
    )
    expect(
        host_cache_surface.get("host_executable_relative_path")
        == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
        and host_cache_surface.get("cache_root_relative_path")
        == "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "expected macro host process provider fixture to preserve host executable and cache root compatibility paths",
    )
    expect(
        host_cache_surface.get("deterministic") is True
        and host_cache_surface.get("host_process_exit_code") == 0
        and isinstance(host_cache_surface.get("cache_hit"), bool),
        "expected macro host process provider fixture to preserve deterministic host-cache readiness",
    )

    runtime_import_path = case_dir / "positive" / "compile" / "module.runtime-import-surface.json"
    expect(
        runtime_import_path.is_file(),
        "expected macro host process provider fixture to publish module.runtime-import-surface.json",
    )
    runtime_import_surface = json.loads(runtime_import_path.read_text(encoding="utf-8"))
    host_cache_import_surface = runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    expect(
        host_cache_import_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected runtime import surface to preserve the metaprogramming host-cache integration contract",
    )
    expect(
        host_cache_import_surface.get("runtime_import_artifact_ready") is True
        and host_cache_import_surface.get("separate_compilation_ready") is True
        and host_cache_import_surface.get("deterministic") is True,
        "expected runtime import surface to preserve host-cache compatibility readiness",
    )

    negative_fixtures = {
        "missing_metadata": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_missing_metadata.objc3",
            "O3S320",
            "requires both objc_macro_package and objc_macro_provenance",
        ),
        "orphan_metadata": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_orphan_metadata.objc3",
            "O3S321",
            "macro package/provenance markers require objc_macro",
        ),
        "invalid_package": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_invalid_package.objc3",
            "O3S322",
            "macro sandbox rejected package 'thirdparty.runtime'",
        ),
        "invalid_provenance": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_invalid_provenance.objc3",
            "O3S323",
            "macro provenance must be a lowercase sha256 digest",
        ),
        "nonpure_callable": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_nonpure.objc3",
            "O3S324",
            "must be pure, body-backed, non-async, and non-throws",
        ),
        "method_topology": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_method_topology.objc3",
            "O3S325",
            "not sandbox-admitted",
        ),
    }
    negative_batch = compile_negative_diagnostic_batch(
        case_id="metaprogramming-macro-safety-cache-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key=negative_key,
                fixture=fixture_path,
                expected_snippets=[expected_message],
                expected_codes=[expected_code],
            )
            for negative_key, (fixture_path, expected_code, expected_message) in negative_fixtures.items()
        ],
    )
    negative_summary = {
        entry["key"]: {
            "fixture": entry["fixture"],
            "diagnostics": entry["diagnostics"],
            "expected_code": entry["expected_codes"][0],
            "duration_seconds": entry["duration_seconds"],
        }
        for entry in negative_batch["results"]
    }

    return CaseResult(
        case_id="metaprogramming-macro-safety-cache-diagnostics",
        probe="compile-manifest-macro-safety-cache-diagnostics",
        fixture="tests/tooling/fixtures/native/macro_host_process_provider.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "positive_fixture": {
                "fixture": str(positive_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(positive_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "host_cache_artifact": str(host_cache_path.relative_to(ROOT)).replace("\\", "/"),
                "runtime_import_surface": str(runtime_import_path.relative_to(ROOT)).replace("\\", "/"),
                "safe_macro_callable_sites": macro_safety_surface.get(
                    "safe_macro_callable_sites"
                ),
                "cache_hit": host_cache_surface.get("cache_hit"),
                "host_process_exit_code": host_cache_surface.get(
                    "host_process_exit_code"
                ),
            },
            "negative_cases": negative_summary,
            "negative_diagnostics_batch": negative_batch,
        },
    )
