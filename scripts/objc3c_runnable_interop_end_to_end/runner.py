"""Runnable mixed-module interop end-to-end validation runner."""

from __future__ import annotations

from datetime import datetime, timezone

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths
from objc3c_tooling.subprocesses import run_capture

from .assertions import expect
from .compilation import assert_artifacts_exist
from .compilation import compile_consumer_fixture
from .compilation import compile_fixture
from .compilation import consumer_artifact_paths
from .manifest import manifest_path
from .manifest import validate_package_manifest
from .paths import PACKAGE_PS1
from .paths import PWSH
from .paths import REPORT_PATH
from .paths import ROOT
from .paths import SUMMARY_CONTRACT_ID
from .probes import assert_bridge_probe_payload
from .probes import assert_packaging_probe_payload
from .probes import compile_and_run_probe


def assert_runtime_interop_link_plan(
    *,
    consumer_link_plan: dict[str, object],
    provider_bridge_json: dict[str, object],
    provider_import_surface: dict[str, object],
) -> None:
    expect(
        consumer_link_plan.get("module_image_count") == 2
        and consumer_link_plan.get("direct_import_input_count") == 1,
        "packaged interop consumer link plan drifted from the two-image mixed-module topology",
    )
    expect(
        consumer_link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and consumer_link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and consumer_link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "packaged interop consumer link plan drifted from the bridge artifact paths",
    )
    expect(
        consumer_link_plan.get("interop_ffi_imported_module_count") == 1
        and consumer_link_plan.get("interop_header_module_bridge_imported_module_count") == 1,
        "packaged interop consumer link plan drifted from the imported ffi and bridge module counts",
    )
    expect(
        isinstance(provider_import_surface.get("module_name"), str)
        and provider_import_surface.get("module_name") != ""
        and provider_bridge_json.get("header_artifact_relative_path") == "module.interop-bridge.h"
        and provider_bridge_json.get("module_artifact_relative_path") == "module.interop-bridge.modulemap"
        and provider_bridge_json.get("bridge_artifact_relative_path") == "module.interop-bridge.json",
        "packaged interop provider artifacts drifted from the emitted provider identity or bridge artifact paths",
    )


def assert_header_bridge_link_plan(header_consumer_link_plan: dict[str, object]) -> None:
    expect(
        "m274_header_module_bridge_provider"
        in header_consumer_link_plan.get(
            "interop_header_module_bridge_imported_module_names_lexicographic", []
        ),
        "packaged header-bridge consumer link plan drifted from the imported bridge provider identity",
    )


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-interop-e2e" / run_id
    manifest_json_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "runnable-interop-e2e"

    package_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(PACKAGE_PS1),
            "-PackageRoot",
            str(package_root),
        ],
        cwd=ROOT,
    )
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_json(manifest_json_path)
    validate_package_manifest(manifest, package_root=package_root)

    compile_script = manifest_path(manifest, "compile_wrapper", package_root=package_root)
    runtime_library = manifest_path(manifest, "runtime_library", package_root=package_root)
    smoke_script = manifest_path(manifest, "execution_smoke_script", package_root=package_root)
    replay_script = manifest_path(manifest, "execution_replay_script", package_root=package_root)
    provider_fixture = manifest_path(manifest, "interop_runtime_fixture", package_root=package_root)
    consumer_fixture = manifest_path(manifest, "interop_runtime_consumer_fixture", package_root=package_root)
    header_provider_fixture = manifest_path(manifest, "interop_header_bridge_fixture", package_root=package_root)
    header_consumer_fixture = manifest_path(manifest, "interop_header_bridge_consumer_fixture", package_root=package_root)
    packaging_probe = manifest_path(manifest, "interop_packaging_probe", package_root=package_root)
    bridge_probe = manifest_path(manifest, "interop_bridge_generation_probe", package_root=package_root)

    provider_artifacts = compile_fixture(
        compile_script,
        provider_fixture,
        artifacts_root / "provider" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    consumer_compile_dir = artifacts_root / "consumer" / "compile"
    consumer_compile_result = compile_consumer_fixture(
        compile_script,
        consumer_fixture,
        consumer_compile_dir,
        cwd=package_root,
        runtime_surface=provider_artifacts["runtime_import_surface"],
        registration_order="2",
        failure_message="packaged compile wrapper failed for the interop consumer fixture",
    )

    consumer_artifacts = consumer_artifact_paths(consumer_compile_dir)
    assert_artifacts_exist(
        consumer_artifacts,
        message_prefix="packaged interop consumer compile did not publish",
    )

    provider_bridge_json = load_json(provider_artifacts["bridge_json"])
    provider_import_surface = load_json(provider_artifacts["runtime_import_surface"])
    consumer_link_plan = load_json(consumer_artifacts["cross_module_link_plan"])
    assert_runtime_interop_link_plan(
        consumer_link_plan=consumer_link_plan,
        provider_bridge_json=provider_bridge_json,
        provider_import_surface=provider_import_surface,
    )

    header_provider_artifacts = compile_fixture(
        compile_script,
        header_provider_fixture,
        artifacts_root / "header-provider" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    header_consumer_compile_dir = artifacts_root / "header-consumer" / "compile"
    header_consumer_compile_result = compile_consumer_fixture(
        compile_script,
        header_consumer_fixture,
        header_consumer_compile_dir,
        cwd=package_root,
        runtime_surface=header_provider_artifacts["runtime_import_surface"],
        registration_order="2",
        failure_message="packaged compile wrapper failed for the header-bridge consumer fixture",
    )

    header_consumer_link_plan = load_json(
        header_consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    )
    assert_header_bridge_link_plan(header_consumer_link_plan)

    clangxx = find_clangxx()
    packaging_probe_exe = artifacts_root / "bridge_packaging_toolchain_probe.exe"
    packaging_probe_payload = compile_and_run_probe(
        clangxx=clangxx,
        probe_source=packaging_probe,
        probe_exe=packaging_probe_exe,
        cwd=package_root,
        runtime_library=runtime_library,
        label="packaged interop packaging-topology probe",
    )
    assert_packaging_probe_payload(
        packaging_probe_payload,
        consumer_link_plan=consumer_link_plan,
    )

    bridge_probe_exe = artifacts_root / "header_module_bridge_generation_probe.exe"
    bridge_probe_payload = compile_and_run_probe(
        clangxx=clangxx,
        probe_source=bridge_probe,
        probe_exe=bridge_probe_exe,
        cwd=package_root,
        runtime_library=runtime_library,
        label="packaged interop bridge-generation probe",
    )
    assert_bridge_probe_payload(
        bridge_probe_payload,
        consumer_link_plan=consumer_link_plan,
        provider_bridge_json=provider_bridge_json,
    )

    smoke_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(smoke_script)],
        cwd=package_root,
    )
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke validation failed")

    replay_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(replay_script),
            "-CaseId",
            "canonical-runnable",
        ],
        cwd=package_root,
    )
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay proof failed")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_interop_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_json_path),
        "package_root": repo_rel(package_root),
        "packaged_provider_fixture": repo_rel(provider_fixture),
        "packaged_consumer_fixture": repo_rel(consumer_fixture),
        "packaged_packaging_probe": repo_rel(packaging_probe),
        "packaged_bridge_probe": repo_rel(bridge_probe),
        "packaged_probe_executables": {
            "packaging_probe": repo_rel(packaging_probe_exe),
            "bridge_probe": repo_rel(bridge_probe_exe),
        },
        "provider_bridge_json_path": repo_rel(provider_artifacts["bridge_json"]),
        "consumer_cross_module_link_plan_path": repo_rel(
            consumer_artifacts["cross_module_link_plan"]
        ),
        "packaging_probe_payload": packaging_probe_payload,
        "bridge_probe_payload": bridge_probe_payload,
        "child_report_paths": [
            *extract_report_paths(package_result.stdout),
            *extract_report_paths(smoke_result.stdout),
            *extract_report_paths(replay_result.stdout),
        ],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "compile-interop-provider-fixture",
                "exit_code": 0,
            },
            {
                "action": "compile-interop-consumer-fixture",
                "exit_code": consumer_compile_result.returncode,
            },
            {
                "action": "compile-header-bridge-provider-fixture",
                "exit_code": 0,
            },
            {
                "action": "compile-header-bridge-consumer-fixture",
                "exit_code": header_consumer_compile_result.returncode,
            },
            {
                "action": "compile-packaged-interop-probes",
                "exit_code": 0,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-interop-probes",
                "exit_code": 0,
            },
            {
                "action": "packaged-execution-smoke",
                "exit_code": smoke_result.returncode,
                "report_paths": extract_report_paths(smoke_result.stdout),
            },
            {
                "action": "packaged-execution-replay",
                "exit_code": replay_result.returncode,
                "report_paths": extract_report_paths(replay_result.stdout),
            },
        ],
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = [
    "assert_header_bridge_link_plan",
    "assert_runtime_interop_link_plan",
    "main",
]
