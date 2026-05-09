"""Metaprogramming runtime ABI and live cache runtime acceptance cases."""

from __future__ import annotations

import json
import shutil
from datetime import datetime
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    compile_fixture_outputs,
    compile_fixture_with_args,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import ROOT
from ..progress import repo_display_path

def remove_metaprogramming_cache_entry_from_artifact(artifact: dict[str, Any]) -> bool:
    relative_entry = artifact.get("cache_entry_relative_path")
    if not isinstance(relative_entry, str) or relative_entry == "":
        return False
    cache_entry = ROOT / Path(relative_entry)
    allowed_root = ROOT / "tmp" / "artifacts" / "objc3c-native" / "cache" / "metaprogramming"
    try:
        cache_entry.resolve().relative_to(allowed_root.resolve())
    except ValueError:
        return False
    if cache_entry.is_dir():
        shutil.rmtree(cache_entry)
        return True
    if cache_entry.is_file():
        cache_entry.unlink()
        return True
    return False

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

def check_live_metaprogramming_cache_runtime_integration_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "live-metaprogramming-cache-runtime-integration"
    case_dir.mkdir(parents=True, exist_ok=True)

    provider_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_provider.objc3"
    )
    provider_source = provider_fixture.read_text(encoding="utf-8")
    unique_suffix = datetime.now().strftime("%H%M%S%f")
    unique_module_name = f"MetaprogrammingHostProcessProvider{unique_suffix}"
    cache_root = case_dir / "metaprogramming-cache-root"
    cache_root_override = repo_display_path(cache_root)
    cache_root_args = [
        "--objc3-metaprogramming-cache-root",
        cache_root_override,
    ]
    provider_source = provider_source.replace(
        "module MetaprogrammingHostProcessProvider;",
        f"module {unique_module_name};",
        1,
    )
    temp_provider_fixture = case_dir / "metaprogramming_cache_provider_materialize.objc3"
    first_compile_dir: Path | None = None
    first_host_cache_artifact_path: Path | None = None
    first_runtime_import_path: Path | None = None
    first_host_cache_artifact: dict[str, Any] | None = None
    first_runtime_import_surface: dict[str, Any] | None = None
    first_host_cache_import_surface: dict[str, Any] | None = None

    def compile_candidate(
        compile_dir: Path,
    ) -> tuple[Path, Path, dict[str, Any], dict[str, Any], dict[str, Any]]:
        compile_fixture_with_args(
            temp_provider_fixture,
            compile_dir,
            [
                "--objc3-bootstrap-registration-order-ordinal",
                "1",
                *cache_root_args,
            ],
        )
        host_cache_artifact_path = (
            compile_dir / "module.metaprogramming-macro-host-cache.json"
        )
        runtime_import_path = compile_dir / "module.runtime-import-surface.json"
        host_cache_artifact = json.loads(
            host_cache_artifact_path.read_text(encoding="utf-8")
        )
        runtime_import_surface = json.loads(
            runtime_import_path.read_text(encoding="utf-8")
        )
        host_cache_import_surface = runtime_import_surface.get(
            "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
        )
        return (
            host_cache_artifact_path,
            runtime_import_path,
            host_cache_artifact,
            runtime_import_surface,
            host_cache_import_surface,
        )

    for materialization_attempt in range(0, 16):
        extra_macros = "".join(
            [
                "\n"
                f"pure fn cacheSeed{materialization_attempt}_{index}() -> i32 "
                '__attribute__((objc_macro(named("Trace")), '
                f'objc_macro_package(named("std.metaprogramming.trace.{materialization_attempt}")), '
                f'objc_macro_provenance(named("sha256:{unique_suffix}{index:02d}")))) {{\n'
                f"  return {17 + index};\n"
                "}\n"
                for index in range(materialization_attempt)
            ]
        )
        temp_provider_fixture.write_text(provider_source + extra_macros, encoding="utf-8")
        compile_dir = case_dir / f"provider-first-{materialization_attempt:02d}"
        (
            candidate_host_cache_artifact_path,
            candidate_runtime_import_path,
            candidate_host_cache_artifact,
            candidate_runtime_import_surface,
            candidate_host_cache_import_surface,
        ) = compile_candidate(compile_dir)
        if (
            candidate_host_cache_artifact.get("launch_attempted") is not True
            and remove_metaprogramming_cache_entry_from_artifact(candidate_host_cache_artifact)
        ):
            compile_dir = case_dir / f"provider-first-{materialization_attempt:02d}-materialize"
            (
                candidate_host_cache_artifact_path,
                candidate_runtime_import_path,
                candidate_host_cache_artifact,
                candidate_runtime_import_surface,
                candidate_host_cache_import_surface,
            ) = compile_candidate(compile_dir)
        if candidate_host_cache_artifact.get("launch_attempted") is True:
            first_compile_dir = compile_dir
            first_host_cache_artifact_path = candidate_host_cache_artifact_path
            first_runtime_import_path = candidate_runtime_import_path
            first_host_cache_artifact = candidate_host_cache_artifact
            first_runtime_import_surface = candidate_runtime_import_surface
            first_host_cache_import_surface = candidate_host_cache_import_surface
            break
    expect(
        first_compile_dir is not None
        and first_host_cache_artifact_path is not None
        and first_runtime_import_path is not None
        and first_host_cache_artifact is not None
        and first_runtime_import_surface is not None
        and first_host_cache_import_surface is not None,
        "expected live metaprogramming host-cache implementation case to force a materializing cache miss before the cache-hit replay check",
    )
    for field_name, expected_value in {
        "cache_ready": True,
        "launch_attempted": True,
        "cache_hit": True,
        "cache_summary_present": True,
        "cache_runtime_import_surface_present": True,
        "cache_manifest_present": True,
        "cache_materialization_state": "materialized",
        "host_process_exit_code": 0,
        "deterministic": True,
    }.items():
        expect(
            first_host_cache_artifact.get(field_name) == expected_value,
            f"expected first metaprogramming host-cache materialization artifact to preserve {field_name}",
        )
    for relative_field in (
        "cache_entry_relative_path",
        "cache_summary_relative_path",
        "cache_runtime_import_surface_relative_path",
        "cache_manifest_relative_path",
    ):
        relative_value = first_host_cache_artifact.get(relative_field)
        expect(
            isinstance(relative_value, str) and relative_value != "",
            f"expected first metaprogramming host-cache materialization artifact to publish {relative_field}",
        )
        expect(
            (ROOT / Path(relative_value)).is_file()
            or (ROOT / Path(relative_value)).is_dir(),
            f"expected first metaprogramming host-cache materialization artifact path {relative_field} to exist",
        )
    expect(
        first_runtime_import_surface.get("module_name") == unique_module_name,
        "expected first metaprogramming host-cache materialization compile to publish the unique module name",
    )
    expect(
        first_host_cache_artifact.get("cache_root_relative_path") == cache_root_override
        and first_host_cache_import_surface.get("cache_root_relative_path")
        == cache_root_override,
        "expected first metaprogramming host-cache materialization compile to use the test-owned cache root override",
    )
    expect(
        first_host_cache_artifact.get("cache_root_relative_path")
        != "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "expected live metaprogramming host-cache test not to depend on the shared scratch cache root",
    )
    expect(
        first_host_cache_import_surface.get("host_executable_relative_path")
        == first_host_cache_artifact.get("host_executable_relative_path")
        and first_host_cache_import_surface.get("cache_root_relative_path")
        == first_host_cache_artifact.get("cache_root_relative_path"),
        "expected first metaprogramming host-cache materialization compile to align import-surface and artifact cache paths",
    )

    second_compile_dir = case_dir / "provider-second"
    compile_fixture_with_args(
        temp_provider_fixture,
        second_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "1",
            *cache_root_args,
        ],
    )
    second_host_cache_artifact_path = (
        second_compile_dir / "module.metaprogramming-macro-host-cache.json"
    )
    second_runtime_import_path = second_compile_dir / "module.runtime-import-surface.json"
    second_host_cache_artifact = json.loads(
        second_host_cache_artifact_path.read_text(encoding="utf-8")
    )
    second_runtime_import_surface = json.loads(
        second_runtime_import_path.read_text(encoding="utf-8")
    )
    second_host_cache_import_surface = second_runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    for field_name, expected_value in {
        "cache_ready": True,
        "launch_attempted": False,
        "cache_hit": True,
        "cache_summary_present": True,
        "cache_runtime_import_surface_present": True,
        "cache_manifest_present": True,
        "cache_materialization_state": "cache-hit",
        "host_process_exit_code": 0,
        "deterministic": True,
    }.items():
        expect(
            second_host_cache_artifact.get(field_name) == expected_value,
            f"expected second metaprogramming host-cache materialization artifact to preserve {field_name}",
        )
    for field_name in (
        "cache_key",
        "cache_entry_relative_path",
        "cache_summary_relative_path",
        "cache_runtime_import_surface_relative_path",
        "cache_manifest_relative_path",
        "host_executable_relative_path",
        "cache_root_relative_path",
        "replay_key",
    ):
        expect(
            second_host_cache_artifact.get(field_name)
            == first_host_cache_artifact.get(field_name),
            f"expected second metaprogramming host-cache materialization artifact to preserve {field_name}",
        )
    expect(
        second_host_cache_import_surface.get("replay_key")
        == first_host_cache_import_surface.get("replay_key"),
        "expected repeated metaprogramming host-cache materialization compile to preserve the same import-surface replay key",
    )

    consumer_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_consumer.objc3"
    )
    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(first_runtime_import_path),
            *cache_root_args,
        ],
    )
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
            first_host_cache_artifact.get("host_executable_relative_path"),
        ),
        (
            "expected_metaprogramming_host_cache_root_relative_path",
            first_host_cache_artifact.get("cache_root_relative_path"),
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected live metaprogramming host-cache consumer link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("metaprogramming_host_cache_imported_module_count") == 1
        and link_plan.get("metaprogramming_host_cache_imported_module_names_lexicographic")
        == [unique_module_name]
        and link_plan.get("metaprogramming_host_cache_cross_module_preservation_ready")
        is True,
        "expected live metaprogramming host-cache consumer link plan to preserve imported module readiness",
    )
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected live metaprogramming host-cache consumer link plan to publish one imported module",
    )
    imported_module = imported_modules[0]
    for field_name, expected_value in (
        ("module_name", unique_module_name),
        ("metaprogramming_macro_host_process_cache_runtime_integration_present", True),
        ("metaprogramming_macro_host_process_cache_runtime_ready", True),
        ("metaprogramming_macro_host_process_cache_separate_compilation_ready", True),
        ("metaprogramming_macro_host_process_cache_deterministic", True),
        (
            "metaprogramming_macro_host_process_cache_host_executable_relative_path",
            first_host_cache_artifact.get("host_executable_relative_path"),
        ),
        (
            "metaprogramming_macro_host_process_cache_root_relative_path",
            first_host_cache_artifact.get("cache_root_relative_path"),
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected live metaprogramming host-cache imported module to preserve {field_name}",
        )

    host_cache_probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "macro_host_process_cache_integration_probe.cpp"
    )
    host_cache_exe = case_dir / "macro_host_process_cache_integration_probe.exe"
    compile_probe(clangxx, host_cache_probe, host_cache_exe, [])
    host_cache_payload = parse_key_value_output(
        run_probe(
            host_cache_exe,
            env={"OBJC3C_METAPROGRAMMING_CACHE_ROOT": cache_root_override},
        ),
        "live metaprogramming host-cache runtime integration probe",
    )
    expect(
        host_cache_payload.get("host_executable_relative_path")
        == first_host_cache_artifact.get("host_executable_relative_path")
        and host_cache_payload.get("cache_root_relative_path")
        == first_host_cache_artifact.get("cache_root_relative_path")
        and host_cache_payload.get("macro_host_execution_ready") == 1
        and host_cache_payload.get("macro_host_process_launch_ready") == 1,
        "expected live metaprogramming host-cache runtime probe to stay aligned with the cache artifact paths and readiness",
    )

    return CaseResult(
        case_id="live-metaprogramming-cache-runtime-integration",
        probe="tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        fixture=str(temp_provider_fixture.relative_to(ROOT)).replace("\\", "/"),
        claim_class="compile-linked-runtime-probe",
        passed=True,
        summary={
            "provider_fixture": str(temp_provider_fixture.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "provider_module_name": unique_module_name,
            "cache_root_override_flag": "--objc3-metaprogramming-cache-root",
            "cache_root_environment_variable": "OBJC3C_METAPROGRAMMING_CACHE_ROOT",
            "cache_root_relative_path": cache_root_override,
            "cache_root_is_test_owned": True,
            "shared_cache_prune_used": False,
            "first_host_cache_artifact": {
                "path": str(first_host_cache_artifact_path.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "cache_key": first_host_cache_artifact.get("cache_key"),
                "cache_materialization_state": first_host_cache_artifact.get(
                    "cache_materialization_state"
                ),
                "launch_attempted": first_host_cache_artifact.get("launch_attempted"),
            },
            "second_host_cache_artifact": {
                "path": str(second_host_cache_artifact_path.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "cache_key": second_host_cache_artifact.get("cache_key"),
                "cache_materialization_state": second_host_cache_artifact.get(
                    "cache_materialization_state"
                ),
                "launch_attempted": second_host_cache_artifact.get("launch_attempted"),
            },
            "consumer_link_plan": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "imported_module_names": link_plan.get(
                "metaprogramming_host_cache_imported_module_names_lexicographic"
            ),
        },
    )
