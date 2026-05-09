"""Metaprogramming macro-safety/cache diagnostic acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    NegativeDiagnosticExpectation,
    compile_fixture_outputs,
    compile_negative_diagnostic_batch,
)

from ..core import ROOT


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
