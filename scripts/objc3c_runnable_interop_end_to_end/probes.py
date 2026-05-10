"""Interop probe compilation and payload validation."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.probe_compile import compile_probe
from objc3c_tooling.probe_output import parse_key_value_output
from objc3c_tooling.subprocesses import run_capture

from .assertions import expect, expect_probe_fields


def compile_and_run_probe(
    *,
    clangxx: str,
    probe_source: Path,
    probe_exe: Path,
    cwd: Path,
    runtime_library: Path,
    label: str,
) -> dict[str, object]:
    compile_probe(clangxx, probe_source, probe_exe, cwd=cwd, runtime_library=runtime_library)
    return parse_key_value_output(run_capture([str(probe_exe)], cwd=cwd), label)


def assert_packaging_probe_payload(
    payload: dict[str, object],
    *,
    consumer_link_plan: dict[str, object],
) -> None:
    expect_probe_fields(
        payload,
        {
            "copy_status": 0,
            "packaging_topology_ready": 1,
            "operator_visible_evidence_ready": 1,
            "header_generation_ready": 0,
            "module_generation_ready": 0,
            "bridge_generation_ready": 0,
            "deterministic": 1,
        },
        context="packaged interop packaging probe",
    )
    expect(
        payload.get("runtime_support_library_archive_relative_path")
        == consumer_link_plan.get("runtime_support_library_archive_relative_path"),
        "packaged interop packaging probe drifted from the emitted runtime archive path",
    )


def assert_bridge_probe_payload(
    payload: dict[str, object],
    *,
    consumer_link_plan: dict[str, object],
    provider_bridge_json: dict[str, object],
) -> None:
    expect_probe_fields(
        payload,
        {
            "copy_status": 0,
            "runtime_generation_ready": 1,
            "cross_module_packaging_ready": 1,
            "header_generation_ready": 1,
            "module_generation_ready": 1,
            "bridge_generation_ready": 1,
            "deterministic": 1,
        },
        context="packaged interop bridge probe",
    )
    expect(
        payload.get("header_artifact_relative_path")
        == consumer_link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == provider_bridge_json.get("header_artifact_relative_path"),
        "packaged interop bridge probe drifted from the emitted bridge header path",
    )
    expect(
        payload.get("module_artifact_relative_path")
        == consumer_link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == provider_bridge_json.get("module_artifact_relative_path"),
        "packaged interop bridge probe drifted from the emitted bridge modulemap path",
    )
    expect(
        payload.get("bridge_artifact_relative_path")
        == consumer_link_plan.get("expected_interop_bridge_artifact_relative_path")
        == provider_bridge_json.get("bridge_artifact_relative_path"),
        "packaged interop bridge probe drifted from the emitted bridge json path",
    )


__all__ = [
    "assert_bridge_probe_payload",
    "assert_packaging_probe_payload",
    "compile_and_run_probe",
]
