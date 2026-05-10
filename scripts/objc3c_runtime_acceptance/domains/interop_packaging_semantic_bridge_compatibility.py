"""C/C++/Swift bridge compatibility interop packaging semantic acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
)
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


def check_c_cpp_swift_bridge_compatibility_semantics_case(
    run_dir: Path,
) -> CaseResult:
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
        and "BridgeProviderExtrasGate" in provider_bridge_header
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


__all__ = ["check_c_cpp_swift_bridge_compatibility_semantics_case"]
