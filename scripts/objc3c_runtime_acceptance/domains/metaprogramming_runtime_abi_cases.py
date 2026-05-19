"""Metaprogramming runtime ABI cache acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_runtime_abi_assertions import (
    BOUNDARY_EXPECTED_PAYLOAD,
    HOST_CACHE_EXPECTED_PAYLOAD,
    expect_host_cache_artifact_surface,
    expect_runtime_abi_payload,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe


def check_metaprogramming_runtime_abi_cache_surface_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-runtime-abi-cache-surface"

    boundary_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "expansion_host_runtime_boundary_positive.objc3"
    )
    boundary_obj_path, _, boundary_manifest_path = compile_fixture_outputs(
        boundary_fixture, case_dir / "boundary" / "compile"
    )
    boundary_probe = (
        ROOT / "tests" / "tooling" / "runtime" / "expansion_host_runtime_boundary_probe.cpp"
    )
    boundary_exe = case_dir / "boundary" / "expansion_host_runtime_boundary_probe.exe"
    compile_probe(clangxx, boundary_probe, boundary_exe, [boundary_obj_path])
    boundary_payload = parse_key_value_output(
        run_probe(boundary_exe), "metaprogramming runtime ABI expansion boundary probe"
    )
    expect_runtime_abi_payload(
        boundary_payload, BOUNDARY_EXPECTED_PAYLOAD, "expansion boundary"
    )

    host_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_provider.objc3"
    )
    _, _, host_manifest_path = compile_fixture_outputs(
        host_fixture, case_dir / "host-cache" / "compile"
    )
    host_cache_probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "macro_host_process_cache_integration_probe.cpp"
    )
    host_cache_exe = case_dir / "host-cache" / "macro_host_process_cache_integration_probe.exe"
    compile_probe(clangxx, host_cache_probe, host_cache_exe, [])
    host_cache_payload = parse_key_value_output(
        run_probe(host_cache_exe), "metaprogramming runtime ABI host-cache probe"
    )
    expect_runtime_abi_payload(
        host_cache_payload, HOST_CACHE_EXPECTED_PAYLOAD, "host-cache"
    )

    host_cache_artifact_path = (
        case_dir / "host-cache" / "compile" / "module.metaprogramming-macro-host-cache.json"
    )
    runtime_import_path = case_dir / "host-cache" / "compile" / "module.runtime-import-surface.json"
    host_cache_artifact = json.loads(host_cache_artifact_path.read_text(encoding="utf-8"))
    runtime_import_surface = json.loads(runtime_import_path.read_text(encoding="utf-8"))
    host_cache_import_surface = runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    expect_host_cache_artifact_surface(
        host_cache_artifact, host_cache_import_surface, host_cache_payload
    )

    return CaseResult(
        case_id="metaprogramming-runtime-abi-cache-surface",
        probe="tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp;tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        fixture="tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
        claim_class="compile-linked-runtime-probe",
        passed=True,
        summary={
            "boundary_fixture": {
                "fixture": str(boundary_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(boundary_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "probe": str(boundary_probe.relative_to(ROOT)).replace("\\", "/"),
                "probe_exe": str(boundary_exe.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
                "macro_host_execution_ready": boundary_payload.get(
                    "macro_host_execution_ready"
                ),
            },
            "host_cache_fixture": {
                "fixture": str(host_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(host_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "host_cache_artifact": str(host_cache_artifact_path.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "runtime_import_surface": str(runtime_import_path.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "probe": str(host_cache_probe.relative_to(ROOT)).replace("\\", "/"),
                "probe_exe": str(host_cache_exe.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": host_cache_artifact.get("contract_id"),
                "host_executable_relative_path": host_cache_payload.get(
                    "host_executable_relative_path"
                ),
                "cache_root_relative_path": host_cache_payload.get(
                    "cache_root_relative_path"
                ),
            },
        },
    )
