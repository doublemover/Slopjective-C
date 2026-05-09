"""Interop packaging semantic acceptance cases."""

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
    "check_mixed_image_compatibility_interop_semantics_case",
    "check_c_cpp_swift_bridge_compatibility_semantics_case",
]

def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_mixed_image_compatibility_interop_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "mixed-image-compatibility-interop-semantics"
    provider_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_bridge_json = json.loads(
        (provider_compile_dir / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )

    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_registration_manifest = json.loads(
        (
            consumer_compile_dir / "module.runtime-registration-manifest.json"
        ).read_text(encoding="utf-8")
    )
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    expect(
        link_plan.get("interop_header_module_bridge_imported_module_count") == 1
        and link_plan.get("interop_ffi_imported_module_count") == 1,
        "expected mixed-image interop link plan to preserve one imported bridge/ffi provider module",
    )
    expect(
        link_plan.get("module_image_count") == 2
        and link_plan.get("ready") is True,
        "expected mixed-image interop link plan to publish a ready two-image package topology",
    )
    expect(
        link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected mixed-image interop link plan to preserve the canonical bridge artifact paths",
    )

    local_module = link_plan.get("local_module", {})
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(local_module, dict) and isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected mixed-image interop link plan to publish one local module and one imported module summary",
    )
    imported_module = imported_modules[0]
    expect(
        local_module.get("translation_unit_registration_order_ordinal") == 2
        and consumer_registration_manifest.get("translation_unit_registration_order_ordinal") == 2,
        "expected mixed-image interop consumer compile to preserve registration ordinal 2",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected mixed-image interop link plan to preserve the imported provider registration ordinal",
    )
    expect(
        imported_module.get("interop_header_module_bridge_contract_id")
        == link_plan.get("expected_interop_header_module_bridge_contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected mixed-image interop link plan to preserve the imported bridge-generation contract",
    )
    expect(
        imported_module.get("interop_ffi_contract_id")
        == link_plan.get("expected_interop_ffi_contract_id")
        == "objc3c.interop.ffi.metadata.interface.preservation.v1",
        "expected mixed-image interop link plan to preserve the imported ffi metadata/interface preservation contract",
    )
    expect(
        imported_module.get("interop_ffi_preservation_contract_id")
        == link_plan.get("expected_interop_ffi_preservation_contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected mixed-image interop link plan to preserve the imported foreign surface/interface preservation contract",
    )
    expect(
        imported_module.get("interop_bridge_header_artifact_relative_path")
        == provider_bridge_json.get("header_artifact_relative_path")
        == "module.interop-bridge.h",
        "expected mixed-image interop link plan to preserve the provider header bridge path",
    )
    expect(
        imported_module.get("interop_header_module_bridge_cross_module_packaging_ready")
        is True
        and imported_module.get("interop_header_module_bridge_runtime_generation_ready")
        is True
        and provider_import_payload.get(
            "objc_interop_header_module_and_bridge_generation", {}
        ).get("cross_module_packaging_ready")
        is True,
        "expected mixed-image interop provider and consumer artifacts to agree on bridge packaging readiness",
    )

    duplicate_ordinal_negative = compile_fixture_expect_failure(
        consumer_fixture,
        case_dir / "negative-duplicate-registration-order",
        expected_snippets=[
            "cross-module runtime link-plan duplicate registration order ordinal: 1"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "1",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
        allow_missing_structured_diagnostics=True,
    )

    return CaseResult(
        case_id="mixed-image-compatibility-interop-semantics",
        probe="compile-runtime-import-surface-link-plan-and-fail-closed-diagnostics",
        fixture=INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_runtime_import_surface_path": str(
                provider_import_surface.relative_to(ROOT)
            ).replace("\\", "/"),
            "link_plan_path": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "module_image_count": link_plan.get("module_image_count"),
            "imported_module_count": len(imported_modules),
            "duplicate_registration_order_returncode": duplicate_ordinal_negative[
                "returncode"
            ],
            "duplicate_registration_order_diagnostics_path": duplicate_ordinal_negative[
                "diagnostics_path"
            ],
        },
    )

def check_c_cpp_swift_bridge_compatibility_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "c-cpp-swift-bridge-compatibility-semantics"
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
    provider_bridge_header = (provider_compile_dir / "module.interop-bridge.h").read_text(
        encoding="utf-8"
    )
    provider_bridge_json = json.loads(
        (provider_compile_dir / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )
    provider_ffi_surface = provider_import_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    provider_bridge_surface = provider_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        provider_ffi_surface.get("local_foreign_callable_count") == 2
        and provider_ffi_surface.get("local_cpp_name_annotation_count") == 2
        and provider_ffi_surface.get("local_header_name_annotation_count") == 2
        and provider_ffi_surface.get("local_swift_name_annotation_count") == 1,
        "expected provider import surface to preserve the C/C++/Swift-facing annotation counts",
    )
    expect(
        provider_bridge_surface.get("local_import_module_names_lexicographic")
        == ['"BridgeProviderKit"'],
        "expected provider bridge-generation surface to preserve the import-module name",
    )
    expect(
        "ffiHeaderBridge" in provider_bridge_header
        and "BridgeProviderExtrasShim" in provider_bridge_header
        and "ffiInbound" in provider_bridge_header
        and "BridgeProvider.forward" in provider_bridge_header,
        "expected generated bridge header to preserve the C, C++, and Swift-facing bridge names",
    )
    expect(
        isinstance(provider_bridge_json.get("foreign_callables"), list)
        and len(provider_bridge_json["foreign_callables"]) == 2,
        "expected provider bridge json to publish two foreign callables",
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
        "expected bridge-compatibility consumer compile to publish one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        link_plan.get("interop_ffi_imported_module_count") == 1
        and link_plan.get("interop_header_module_bridge_imported_module_count") == 1,
        "expected consumer link plan to preserve one imported ffi/bridge provider module",
    )
    expect(
        imported_module.get("interop_ffi_local_interface_annotation_sites") == 12
        and imported_module.get("interop_ffi_local_metadata_preservation_sites") == 2,
        "expected consumer link plan to preserve the imported C/C++/Swift annotation footprint",
    )
    expect(
        imported_module.get("interop_header_module_bridge_local_foreign_callable_count")
        == 2
        and imported_module.get("interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and imported_module.get("interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and imported_module.get("interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected consumer link plan to preserve the imported bridge callable count and artifact paths",
    )
    expect(
        imported_module.get("interop_ffi_preservation_contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1"
        and imported_module.get("interop_header_module_bridge_contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected consumer link plan to preserve the imported ffi and bridge contracts",
    )

    return CaseResult(
        case_id="c-cpp-swift-bridge-compatibility-semantics",
        probe="compile-runtime-import-surface-bridge-artifacts-and-cross-module-link-plan",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_module_name": provider_import_surface.get("module_name"),
            "foreign_callable_count": provider_ffi_surface.get(
                "local_foreign_callable_count"
            ),
            "cpp_name_annotation_count": provider_ffi_surface.get(
                "local_cpp_name_annotation_count"
            ),
            "swift_name_annotation_count": provider_ffi_surface.get(
                "local_swift_name_annotation_count"
            ),
            "bridge_imported_module_count": link_plan.get(
                "interop_header_module_bridge_imported_module_count"
            ),
        },
    )

__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
