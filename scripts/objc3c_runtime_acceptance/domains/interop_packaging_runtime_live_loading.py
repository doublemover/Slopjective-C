"""Live package-loading interop runtime implementation acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
)
from ..fixture_compile_runner import run_fixture_compile
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT
from ..probes import compile_probe, parse_key_value_output, run_probe


def _write_tampered_runtime_library_import_surface(
    source_path: Path,
    target_path: Path,
) -> None:
    payload = json.loads(source_path.read_text(encoding="utf-8"))
    payload["runtime_support_library_archive_relative_path"] = (
        "artifacts/lib/tampered_objc3_runtime.lib"
    )
    target_path.write_text(
        json.dumps(payload, indent=2, sort_keys=True),
        encoding="utf-8",
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

    tampered_surface = (
        case_dir / "provider.tampered-runtime-library.runtime-import-surface.json"
    )
    _write_tampered_runtime_library_import_surface(
        provider_compile_dir / "module.runtime-import-surface.json",
        tampered_surface,
    )
    tampered_result, _ = run_fixture_compile(
        consumer_fixture,
        case_dir / "consumer-tampered-runtime-library",
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(tampered_surface),
        ],
        write_provenance=False,
    )
    tampered_diagnostics = (
        case_dir / "consumer-tampered-runtime-library" / "module.diagnostics.txt"
    )
    tampered_output = f"{tampered_result.stderr}\n{tampered_result.stdout}"
    if tampered_diagnostics.is_file():
        tampered_output += "\n" + tampered_diagnostics.read_text(encoding="utf-8")
    expect(
        tampered_result.returncode != 0,
        "expected live package-loading interop compile to fail closed on runtime library import-surface drift",
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
            "tampered_runtime_library_rejected": tampered_result.returncode != 0,
        },
    )


__all__ = ["check_live_package_loading_interop_runtime_implementation_case"]
