"""Runtime package-loader bridge ABI acceptance case."""

from __future__ import annotations

from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
    INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE,
)
from ..paths import ROOT
from ..probes import compile_probe, parse_key_value_output, run_probe


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

    fail_closed_probe = ROOT / Path(INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE)
    fail_closed_exe = case_dir / "package_loader_fail_closed_diagnostics_probe.exe"
    compile_probe(clangxx, fail_closed_probe, fail_closed_exe, [])
    fail_closed_payload = parse_key_value_output(
        run_probe(fail_closed_exe),
        "runtime package-loader fail-closed ABI diagnostics probe",
    )
    for field_name in (
        "fail_closed_statuses",
        "diagnostics_present",
        "no_public_fallback_claim",
    ):
        expect(
            fail_closed_payload.get(field_name) == 1,
            f"expected runtime package-loader fail-closed ABI probe to preserve {field_name}",
        )
    expect(
        fail_closed_payload.get("packaging_null_status")
        == fail_closed_payload.get("invalid_descriptor_status")
        and fail_closed_payload.get("bridge_null_status")
        == fail_closed_payload.get("invalid_descriptor_status"),
        "expected runtime package-loader null descriptors to fail closed with the stable invalid-descriptor status",
    )

    return CaseResult(
        case_id="runtime-package-loader-bridge-abi",
        probe="linked-runtime-abi-and-fail-closed-diagnostics-probes",
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
            "fail_closed_status": fail_closed_payload.get(
                "invalid_descriptor_status"
            ),
            "fail_closed_diagnostics_present": fail_closed_payload.get(
                "diagnostics_present"
            ),
        },
    )


__all__ = ["check_runtime_package_loader_bridge_abi_case"]
