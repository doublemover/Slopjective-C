"""Metaprogramming macro-safety/cache diagnostic acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_macro_safety_assertions import (
    expect_macro_host_cache_surface,
    expect_macro_runtime_import_surface,
    expect_macro_safety_surface,
)
from objc3c_runtime_acceptance.domains.metaprogramming_macro_safety_negative_cases import (
    build_macro_safety_negative_expectations,
    summarize_negative_batch,
)
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_fixture_outputs,
    compile_negative_diagnostic_batch,
)
from objc3c_runtime_acceptance.paths import ROOT


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
    expect_macro_safety_surface(macro_safety_surface)

    host_cache_path = (
        case_dir / "positive" / "compile" / "module.metaprogramming-macro-host-cache.json"
    )
    expect(
        host_cache_path.is_file(),
        "expected macro host process provider fixture to publish module.metaprogramming-macro-host-cache.json",
    )
    host_cache_surface = json.loads(host_cache_path.read_text(encoding="utf-8"))
    expect_macro_host_cache_surface(host_cache_surface)

    runtime_import_path = case_dir / "positive" / "compile" / "module.runtime-import-surface.json"
    expect(
        runtime_import_path.is_file(),
        "expected macro host process provider fixture to publish module.runtime-import-surface.json",
    )
    runtime_import_surface = json.loads(runtime_import_path.read_text(encoding="utf-8"))
    host_cache_import_surface = runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    expect_macro_runtime_import_surface(host_cache_import_surface)

    negative_batch = compile_negative_diagnostic_batch(
        case_id="metaprogramming-macro-safety-cache-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=build_macro_safety_negative_expectations(ROOT),
    )
    negative_summary = summarize_negative_batch(negative_batch)

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
