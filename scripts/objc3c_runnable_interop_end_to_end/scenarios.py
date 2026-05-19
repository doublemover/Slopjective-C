"""Scenario execution for runnable interop end-to-end validation."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.probe_compile import find_clangxx

from .compilation import assert_artifacts_exist
from .compilation import compile_consumer_fixture
from .compilation import compile_fixture
from .compilation import consumer_artifact_paths
from .probes import assert_bridge_probe_payload
from .probes import assert_packaging_probe_payload
from .probes import compile_and_run_probe
from .results import assert_header_bridge_link_plan
from .results import assert_runtime_interop_link_plan


@dataclass(frozen=True)
class RuntimeInteropScenarioResult:
    provider_fixture: Path
    consumer_fixture: Path
    provider_artifacts: dict[str, Path]
    consumer_artifacts: dict[str, Path]
    provider_bridge_json: dict[str, object]
    consumer_link_plan: dict[str, object]
    consumer_compile_result: object


@dataclass(frozen=True)
class HeaderBridgeScenarioResult:
    provider_artifacts: dict[str, Path]
    consumer_link_plan: dict[str, object]
    consumer_compile_result: object


@dataclass(frozen=True)
class ProbeScenarioResult:
    clangxx: str
    packaging_probe: Path
    bridge_probe: Path
    packaging_probe_exe: Path
    bridge_probe_exe: Path
    packaging_probe_payload: dict[str, object]
    bridge_probe_payload: dict[str, object]


def run_runtime_interop_scenario(
    *,
    compile_script: Path,
    provider_fixture: Path,
    consumer_fixture: Path,
    artifacts_root: Path,
    package_root: Path,
) -> RuntimeInteropScenarioResult:
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

    return RuntimeInteropScenarioResult(
        provider_fixture=provider_fixture,
        consumer_fixture=consumer_fixture,
        provider_artifacts=provider_artifacts,
        consumer_artifacts=consumer_artifacts,
        provider_bridge_json=provider_bridge_json,
        consumer_link_plan=consumer_link_plan,
        consumer_compile_result=consumer_compile_result,
    )


def run_header_bridge_scenario(
    *,
    compile_script: Path,
    provider_fixture: Path,
    consumer_fixture: Path,
    artifacts_root: Path,
    package_root: Path,
) -> HeaderBridgeScenarioResult:
    provider_artifacts = compile_fixture(
        compile_script,
        provider_fixture,
        artifacts_root / "header-provider" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    consumer_compile_dir = artifacts_root / "header-consumer" / "compile"
    consumer_compile_result = compile_consumer_fixture(
        compile_script,
        consumer_fixture,
        consumer_compile_dir,
        cwd=package_root,
        runtime_surface=provider_artifacts["runtime_import_surface"],
        registration_order="2",
        failure_message="packaged compile wrapper failed for the header-bridge consumer fixture",
    )

    consumer_link_plan = load_json(
        consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    )
    assert_header_bridge_link_plan(consumer_link_plan)

    return HeaderBridgeScenarioResult(
        provider_artifacts=provider_artifacts,
        consumer_link_plan=consumer_link_plan,
        consumer_compile_result=consumer_compile_result,
    )


def run_probe_scenarios(
    *,
    runtime_library: Path,
    packaging_probe: Path,
    bridge_probe: Path,
    artifacts_root: Path,
    package_root: Path,
    consumer_link_plan: dict[str, object],
    provider_bridge_json: dict[str, object],
) -> ProbeScenarioResult:
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

    return ProbeScenarioResult(
        clangxx=clangxx,
        packaging_probe=packaging_probe,
        bridge_probe=bridge_probe,
        packaging_probe_exe=packaging_probe_exe,
        bridge_probe_exe=bridge_probe_exe,
        packaging_probe_payload=packaging_probe_payload,
        bridge_probe_payload=bridge_probe_payload,
    )


__all__ = [
    "HeaderBridgeScenarioResult",
    "ProbeScenarioResult",
    "RuntimeInteropScenarioResult",
    "run_header_bridge_scenario",
    "run_probe_scenarios",
    "run_runtime_interop_scenario",
]
