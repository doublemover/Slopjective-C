"""Interop packaging artifact-surface acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from ..case_result import CaseResult
from ..native_build import (
    ROOT,
    compile_fixture_expect_failure,
    compile_fixture_outputs_with_args,
    compile_fixture_with_args,
)
from ..probes import compile_probe, parse_json_output, parse_key_value_output, run_probe
from ..assertions import expect
from ..core import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)
_EXPORTED_CASE_NAMES = [
    "check_runtime_packaging_bridge_loader_artifact_surface_case",
]

def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


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

__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
