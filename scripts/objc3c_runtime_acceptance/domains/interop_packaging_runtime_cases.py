"""Interop packaging linked-runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from ..case_result import CaseResult
from ..native_build import ROOT, compile_fixture_with_args
from ..probes import compile_probe, parse_key_value_output, run_probe
from ..assertions import expect
from ..core import (
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
)

_EXPORTED_CASE_NAMES = [
    "check_runtime_package_loader_bridge_abi_case",
    "check_live_package_loading_interop_runtime_implementation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_runtime_package_loader_bridge_abi_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "runtime-package-loader-bridge-abi"

    packaging_probe = ROOT / Path(INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE)
    packaging_exe = case_dir / "bridge_packaging_toolchain_probe.exe"
    compile_probe(clangxx, packaging_probe, packaging_exe, [])
    packaging_payload = parse_key_value_output(
        run_probe(packaging_exe), "runtime package-loader packaging-topology ABI probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "packaging_topology_ready": 1,
        "operator_visible_evidence_ready": 1,
        "header_generation_ready": 0,
        "module_generation_ready": 0,
        "bridge_generation_ready": 0,
        "deterministic": 1,
    }.items():
        expect(
            packaging_payload.get(field_name) == expected_value,
            f"expected runtime package-loader ABI probe to preserve {field_name}",
        )

    bridge_probe = ROOT / Path(INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE)
    bridge_exe = case_dir / "header_module_bridge_generation_probe.exe"
    compile_probe(clangxx, bridge_probe, bridge_exe, [])
    bridge_payload = parse_key_value_output(
        run_probe(bridge_exe), "runtime bridge-generation ABI probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "runtime_generation_ready": 1,
        "cross_module_packaging_ready": 1,
        "header_generation_ready": 1,
        "module_generation_ready": 1,
        "bridge_generation_ready": 1,
        "deterministic": 1,
    }.items():
        expect(
            bridge_payload.get(field_name) == expected_value,
            f"expected runtime bridge-generation ABI probe to preserve {field_name}",
        )
    expect(
        bridge_payload.get("header_artifact_relative_path") == "module.interop-bridge.h"
        and bridge_payload.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and bridge_payload.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected runtime bridge-generation ABI probe to preserve the bridge artifact paths",
    )

    return CaseResult(
        case_id="runtime-package-loader-bridge-abi",
        probe="linked-runtime-abi-probes",
        fixture=None,
        claim_class="runtime-linked-execution",
        passed=True,
        summary={
            "packaging_topology_ready": packaging_payload.get(
                "packaging_topology_ready"
            ),
            "bridge_generation_ready": bridge_payload.get("bridge_generation_ready"),
            "header_artifact_relative_path": bridge_payload.get(
                "header_artifact_relative_path"
            ),
        },
    )

def check_live_package_loading_interop_runtime_implementation_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "live-package-loading-interop-runtime-implementation"
    provider_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_compile_dir / "module.runtime-import-surface.json"),
        ],
    )
    link_plan = json.loads(
        (
            consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
        ).read_text(encoding="utf-8")
    )
    provider_bridge_json = json.loads(
        (provider_compile_dir / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )

    packaging_probe = ROOT / Path(INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE)
    packaging_exe = case_dir / "bridge_packaging_toolchain_probe.exe"
    compile_probe(clangxx, packaging_probe, packaging_exe, [])
    packaging_payload = parse_key_value_output(
        run_probe(packaging_exe), "live package-loading interop packaging-topology probe"
    )

    bridge_probe = ROOT / Path(INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE)
    bridge_exe = case_dir / "header_module_bridge_generation_probe.exe"
    compile_probe(clangxx, bridge_probe, bridge_exe, [])
    bridge_payload = parse_key_value_output(
        run_probe(bridge_exe), "live package-loading interop bridge-generation probe"
    )

    expect(
        packaging_payload.get("runtime_support_library_archive_relative_path")
        == link_plan.get("runtime_support_library_archive_relative_path"),
        "expected live package-loader runtime snapshot to preserve the emitted runtime archive path",
    )
    expect(
        bridge_payload.get("header_artifact_relative_path")
        == link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == provider_bridge_json.get("header_artifact_relative_path")
        == "module.interop-bridge.h",
        "expected live package-loading runtime snapshot to preserve the emitted bridge header path",
    )
    expect(
        bridge_payload.get("module_artifact_relative_path")
        == link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == provider_bridge_json.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap",
        "expected live package-loading runtime snapshot to preserve the emitted bridge modulemap path",
    )
    expect(
        bridge_payload.get("bridge_artifact_relative_path")
        == link_plan.get("expected_interop_bridge_artifact_relative_path")
        == provider_bridge_json.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected live package-loading runtime snapshot to preserve the emitted bridge json path",
    )
    expect(
        packaging_payload.get("packaging_topology_ready") == 1
        and bridge_payload.get("cross_module_packaging_ready") == 1
        and link_plan.get("ready") is True,
        "expected compile artifacts and runtime snapshots to agree on package-loading readiness",
    )

    return CaseResult(
        case_id="live-package-loading-interop-runtime-implementation",
        probe="compile-artifact-plus-linked-runtime-snapshot-integration",
        fixture=INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        claim_class="runtime-linked-execution",
        passed=True,
        summary={
            "runtime_support_library_archive_relative_path": packaging_payload.get(
                "runtime_support_library_archive_relative_path"
            ),
            "bridge_header_artifact_relative_path": bridge_payload.get(
                "header_artifact_relative_path"
            ),
            "link_plan_ready": link_plan.get("ready"),
        },
    )

__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
