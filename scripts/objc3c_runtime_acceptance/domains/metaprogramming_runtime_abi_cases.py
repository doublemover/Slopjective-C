"""Metaprogramming runtime ABI cache acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import ROOT


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
    for field_name, expected_value in {
        "copy_status": 0,
        "property_runtime_ready": 1,
        "macro_host_execution_ready": 0,
        "macro_host_process_launch_ready": 0,
        "runtime_package_loader_ready": 0,
        "deterministic": 1,
        "runtime_support_library_archive_relative_path": "artifacts/lib/objc3_runtime.lib",
        "property_behavior_runtime_model": (
            "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks"
        ),
        "macro_expansion_host_model": (
            "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed"
        ),
        "fail_closed_model": "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet",
    }.items():
        expect(
            boundary_payload.get(field_name) == expected_value,
            f"expected metaprogramming runtime ABI expansion boundary probe to preserve {field_name}",
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
    for field_name, expected_value in {
        "copy_status": 0,
        "property_runtime_ready": 1,
        "macro_host_execution_ready": 1,
        "macro_host_process_launch_ready": 1,
        "runtime_package_loader_ready": 0,
        "deterministic": 1,
        "host_executable_relative_path": "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        "cache_root_relative_path": "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "host_model": (
            "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization"
        ),
        "toolchain_model": (
            "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization"
        ),
        "cache_model": (
            "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-and-reused-on-subsequent-runs"
        ),
        "fail_closed_model": (
            "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims"
        ),
    }.items():
        expect(
            host_cache_payload.get(field_name) == expected_value,
            f"expected metaprogramming runtime ABI host-cache probe to preserve {field_name}",
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
    expect(
        host_cache_artifact.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        and host_cache_import_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected metaprogramming runtime ABI host-cache artifact and runtime import surface to preserve the host-cache integration contract",
    )
    for field_name in (
        "host_executable_relative_path",
        "cache_root_relative_path",
        "deterministic",
        "replay_key",
    ):
        expect(
            host_cache_artifact.get(field_name) == host_cache_import_surface.get(field_name),
            f"expected metaprogramming runtime ABI host-cache artifact and runtime import surface to preserve {field_name}",
        )
    expect(
        host_cache_artifact.get("host_executable_relative_path")
        == host_cache_payload.get("host_executable_relative_path")
        and host_cache_artifact.get("cache_root_relative_path")
        == host_cache_payload.get("cache_root_relative_path"),
        "expected metaprogramming runtime ABI host-cache artifact to stay aligned with the runtime snapshot paths",
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
