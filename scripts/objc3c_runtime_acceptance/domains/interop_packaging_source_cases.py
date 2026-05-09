"""Interop packaging source-surface acceptance cases."""

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
    "check_cross_module_runtime_package_interop_source_surface_case",
    "check_textual_binary_interface_parity_source_surface_case",
]

def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_cross_module_runtime_package_interop_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "cross-module-runtime-package-interop-source-surface"
    fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(
        fixture,
        compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    manifest_path = compile_dir / "module.manifest.json"
    registration_manifest_path = (
        compile_dir / "module.runtime-registration-manifest.json"
    )
    runtime_import_surface_path = compile_dir / "module.runtime-import-surface.json"
    bridge_header_path = compile_dir / "module.interop-bridge.h"
    bridge_modulemap_path = compile_dir / "module.interop-bridge.modulemap"
    bridge_json_path = compile_dir / "module.interop-bridge.json"

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    runtime_import_surface = json.loads(
        runtime_import_surface_path.read_text(encoding="utf-8")
    )
    bridge_json = json.loads(bridge_json_path.read_text(encoding="utf-8"))
    semantic_surface = (
        manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    )
    foreign_source_surface = semantic_surface.get(
        "objc_interop_foreign_declaration_and_import_source_closure", {}
    )
    annotation_source_surface = semantic_surface.get(
        "objc_interop_cpp_and_swift_interop_annotation_source_completion", {}
    )
    foreign_preservation_surface = semantic_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    bridge_generation_surface = semantic_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        foreign_source_surface.get("contract_id")
        == "objc3c.interop.foreign.declaration.import.source.closure.v1",
        "expected interop provider manifest to publish the foreign/import source closure surface",
    )
    expect(
        annotation_source_surface.get("contract_id")
        == "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1",
        "expected interop provider manifest to publish the C++/Swift annotation source-completion surface",
    )
    expect(
        foreign_preservation_surface.get("contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected interop provider manifest to publish the foreign surface/interface preservation packet",
    )
    expect(
        bridge_generation_surface.get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected interop provider manifest to publish the header/module/bridge generation packet",
    )
    expect(
        runtime_import_surface.get("module_name") == bridge_json.get("module_name"),
        "expected interop provider import surface and bridge artifact to preserve one module identity",
    )
    expect(
        registration_manifest.get("translation_unit_registration_order_ordinal") == 1,
        "expected interop provider registration manifest to preserve the explicit registration order ordinal",
    )
    for artifact_path, label in (
        (bridge_header_path, "bridge header"),
        (bridge_modulemap_path, "bridge modulemap"),
        (bridge_json_path, "bridge json"),
    ):
        expect(
            artifact_path.is_file(),
            f"expected interop provider compile to publish the {label} artifact",
        )
    expect(
        runtime_import_surface.get(
            "objc_interop_foreign_surface_interface_and_module_preservation", {}
        ).get("contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected interop provider runtime import surface to preserve the foreign surface/interface preservation contract",
    )
    expect(
        runtime_import_surface.get(
            "objc_interop_header_module_and_bridge_generation", {}
        ).get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected interop provider runtime import surface to preserve the header/module/bridge generation contract",
    )
    expect(
        bridge_json.get("header_artifact_relative_path") == "module.interop-bridge.h"
        and bridge_json.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and bridge_json.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected interop provider bridge artifact to preserve the canonical textual/binary artifact paths",
    )
    expect(
        bridge_json.get("runtime_generation_ready") is True
        and bridge_json.get("cross_module_packaging_ready") is True
        and bridge_json.get("deterministic") is True,
        "expected interop provider bridge artifact to report runtime generation and cross-module packaging readiness",
    )

    return CaseResult(
        case_id="cross-module-runtime-package-interop-source-surface",
        probe="compile-manifest-runtime-import-surface-and-bridge-artifacts",
        fixture=INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "module_name": runtime_import_surface.get("module_name"),
            "runtime_import_surface_path": str(
                runtime_import_surface_path.relative_to(ROOT)
            ).replace("\\", "/"),
            "bridge_header_path": str(bridge_header_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "bridge_modulemap_path": str(
                bridge_modulemap_path.relative_to(ROOT)
            ).replace("\\", "/"),
            "bridge_json_path": str(bridge_json_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
        },
    )

def check_textual_binary_interface_parity_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "textual-binary-interface-parity-source-surface"
    fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(
        fixture,
        compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    manifest = json.loads((compile_dir / "module.manifest.json").read_text(encoding="utf-8"))
    runtime_import_surface = json.loads(
        (compile_dir / "module.runtime-import-surface.json").read_text(encoding="utf-8")
    )
    bridge_json = json.loads(
        (compile_dir / "module.interop-bridge.json").read_text(encoding="utf-8")
    )
    bridge_header = (compile_dir / "module.interop-bridge.h").read_text(
        encoding="utf-8"
    )
    bridge_modulemap = (compile_dir / "module.interop-bridge.modulemap").read_text(
        encoding="utf-8"
    )
    semantic_surface = (
        manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    )
    annotation_source_surface = semantic_surface.get(
        "objc_interop_cpp_and_swift_interop_annotation_source_completion", {}
    )
    foreign_preservation_surface = semantic_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    bridge_generation_surface = semantic_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )
    runtime_bridge_packet = runtime_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        annotation_source_surface.get("contract_id")
        == "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1",
        "expected header/module bridge provider manifest to publish the C++/Swift annotation source-completion surface",
    )
    expect(
        foreign_preservation_surface.get("contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected header/module bridge provider manifest to publish the foreign surface/interface preservation packet",
    )
    expect(
        bridge_generation_surface.get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected header/module bridge provider manifest to publish the header/module/bridge generation packet",
    )
    expect(
        runtime_bridge_packet.get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected header/module bridge provider runtime import surface to preserve the bridge-generation contract",
    )
    expect(
        bridge_json.get("header_artifact_relative_path")
        == runtime_bridge_packet.get("header_artifact_relative_path")
        == "module.interop-bridge.h",
        "expected header/module bridge provider textual and binary surfaces to preserve the header artifact path",
    )
    expect(
        bridge_json.get("module_artifact_relative_path")
        == runtime_bridge_packet.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap",
        "expected header/module bridge provider textual and binary surfaces to preserve the modulemap artifact path",
    )
    expect(
        bridge_json.get("bridge_artifact_relative_path")
        == runtime_bridge_packet.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected header/module bridge provider textual and binary surfaces to preserve the bridge-json artifact path",
    )
    expect(
        bridge_json.get("runtime_generation_ready") is True
        and bridge_json.get("cross_module_packaging_ready") is True
        and bridge_json.get("deterministic") is True
        and runtime_bridge_packet.get("runtime_generation_ready") is True
        and runtime_bridge_packet.get("cross_module_packaging_ready") is True
        and runtime_bridge_packet.get("deterministic") is True,
        "expected header/module bridge provider textual and binary surfaces to agree on readiness and determinism",
    )
    expect(
        bridge_json.get("module_name") == runtime_import_surface.get("module_name"),
        "expected header/module bridge provider bridge json and runtime import surface to preserve one module name",
    )
    expect(
        len(bridge_json.get("foreign_callables", [])) == 2
        and runtime_bridge_packet.get("local_foreign_callable_count") == 2
        and foreign_preservation_surface.get("local_foreign_callable_count") == 2,
        "expected header/module bridge provider textual and binary surfaces to preserve two foreign callables",
    )
    expect(
        annotation_source_surface.get("swift_name_annotation_sites") == 1
        and annotation_source_surface.get("cpp_name_annotation_sites") == 2
        and annotation_source_surface.get("header_name_annotation_sites") == 2,
        "expected header/module bridge provider source-completion surface to preserve the foreign C++/Swift annotation inventory",
    )
    for snippet, label in (
        ("module.interop-bridge.h", "header artifact path"),
        ("module.interop-bridge.modulemap", "modulemap artifact path"),
        ("ffiInbound", "primary foreign callable"),
        ("ffiHeaderBridge", "header-only foreign callable"),
        ("BridgeProviderShim", "C++ bridge annotation"),
        ("BridgeProvider.forward", "Swift bridge annotation"),
    ):
        expect(
            snippet in bridge_header,
            f"expected generated interop bridge header to preserve the {label}",
        )
    expect(
        "module.interop-bridge.h" in bridge_modulemap
        and "_objc3_interop_bridge" in bridge_modulemap,
        "expected generated interop bridge modulemap to preserve the bridge module identity",
    )

    return CaseResult(
        case_id="textual-binary-interface-parity-source-surface",
        probe="compile-manifest-runtime-import-surface-bridge-header-modulemap-and-bridge-json",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "module_name": runtime_import_surface.get("module_name"),
            "foreign_callable_count": len(bridge_json.get("foreign_callables", [])),
            "header_artifact_relative_path": bridge_json.get(
                "header_artifact_relative_path"
            ),
            "module_artifact_relative_path": bridge_json.get(
                "module_artifact_relative_path"
            ),
            "bridge_artifact_relative_path": bridge_json.get(
                "bridge_artifact_relative_path"
            ),
        },
    )

__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
