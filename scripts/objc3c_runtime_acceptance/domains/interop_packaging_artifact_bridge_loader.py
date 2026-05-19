"""Runtime packaging bridge-loader artifact-surface acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
)
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


def check_runtime_packaging_bridge_loader_artifact_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "runtime-packaging-bridge-loader-artifact-surface"
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
    cross_module_linker_rsp = (
        consumer_compile_dir / "module.cross-module-runtime-linker-options.rsp"
    )
    runtime_metadata_linker_rsp = (
        consumer_compile_dir / "module.runtime-metadata-linker-options.rsp"
    )
    cross_module_linker_flags = cross_module_linker_rsp.read_text(encoding="utf-8")
    runtime_metadata_linker_flags = runtime_metadata_linker_rsp.read_text(
        encoding="utf-8"
    )

    for artifact_name in (
        "module.interop-bridge.h",
        "module.interop-bridge.modulemap",
        "module.interop-bridge.json",
    ):
        expect(
            (provider_compile_dir / artifact_name).is_file(),
            f"expected provider compile to publish {artifact_name}",
        )
    expect(
        cross_module_linker_rsp.is_file() and runtime_metadata_linker_rsp.is_file(),
        "expected consumer compile to publish both cross-module and runtime-metadata linker response artifacts",
    )
    expect(
        link_plan.get("linker_response_artifact")
        == "module.cross-module-runtime-linker-options.rsp",
        "expected runtime package loader link plan to preserve the cross-module linker response artifact name",
    )
    expect(
        link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected runtime package loader link plan to preserve the bridge artifact paths",
    )
    expect(
        isinstance(link_plan.get("link_object_artifacts"), list)
        and len(link_plan["link_object_artifacts"]) == 2,
        "expected runtime package loader link plan to preserve both provider and consumer link objects",
    )
    expect(
        isinstance(link_plan.get("driver_linker_flags"), list)
        and len(link_plan["driver_linker_flags"]) == 2
        and "objc3_runtime_metadata_link_anchor" in cross_module_linker_flags
        and "objc3_runtime_metadata_link_anchor" in runtime_metadata_linker_flags,
        "expected runtime package loader artifacts to preserve the metadata anchor linker flags",
    )

    return CaseResult(
        case_id="runtime-packaging-bridge-loader-artifact-surface",
        probe="compile-artifact-and-linker-response-inspection",
        fixture=INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "linker_response_artifact": link_plan.get("linker_response_artifact"),
            "link_object_count": len(link_plan.get("link_object_artifacts", [])),
            "driver_linker_flag_count": len(link_plan.get("driver_linker_flags", [])),
            "bridge_header_path": "module.interop-bridge.h",
        },
    )


__all__ = ["check_runtime_packaging_bridge_loader_artifact_surface_case"]
