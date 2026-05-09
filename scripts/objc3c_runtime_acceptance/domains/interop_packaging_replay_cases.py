"""Interop packaging replay-preservation acceptance cases."""

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
    "check_cross_language_replay_import_surface_preservation_case",
]

def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_cross_language_replay_import_surface_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "cross-language-replay-import-surface-preservation"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = json.loads(
        (provider_compile_dir / "module.runtime-import-surface.json").read_text(
            encoding="utf-8"
        )
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
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected replay-preservation consumer compile to publish one imported module",
    )
    imported_module = imported_modules[0]
    provider_ffi_surface = provider_import_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    provider_bridge_surface = provider_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        imported_module.get("interop_ffi_replay_key")
        == provider_bridge_surface.get("preservation_replay_key")
        and imported_module.get("interop_ffi_lowering_replay_key")
        in imported_module.get("interop_ffi_replay_key", ""),
        "expected consumer link plan to preserve the imported ffi replay and lowering replay keys",
    )
    expect(
        imported_module.get("interop_header_module_bridge_replay_key")
        == provider_bridge_surface.get("replay_key")
        and imported_module.get("interop_header_module_bridge_preservation_replay_key")
        == provider_bridge_surface.get("preservation_replay_key"),
        "expected consumer link plan to preserve the imported bridge replay and preservation replay keys",
    )
    expect(
        imported_module.get("interop_ffi_preservation_replay_key")
        == provider_ffi_surface.get("replay_key"),
        "expected consumer link plan to preserve the imported ffi preservation replay key through the full ffi preservation packet",
    )
    expect(
        imported_module.get("interop_ffi_source_contract_id")
        == "objc3c.interop.foreign.call.and.lifetime.lowering.v1"
        and imported_module.get("interop_header_module_bridge_source_contract_id")
        == "objc3c.interop.bridge.packaging.and.toolchain.contract.v1",
        "expected consumer link plan to preserve the imported replay source contracts",
    )

    return CaseResult(
        case_id="cross-language-replay-import-surface-preservation",
        probe="compile-runtime-import-surface-and-cross-module-link-plan-replay-key-inspection",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "imported_module_name": imported_module.get("module_name"),
            "ffi_replay_key": imported_module.get("interop_ffi_replay_key"),
            "bridge_replay_key": imported_module.get(
                "interop_header_module_bridge_replay_key"
            ),
        },
    )

__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
