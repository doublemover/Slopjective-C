"""Cross-module interop packaging source-surface acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


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
    runtime_bridge_packet = runtime_import_surface.get(
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
        registration_manifest.get("translation_unit_registration_order_ordinal") == 1,
        "expected interop provider registration manifest to preserve the explicit registration order ordinal",
    )
    for artifact_path, label in (
        (bridge_header_path, "bridge header"),
        (bridge_modulemap_path, "bridge modulemap"),
        (bridge_json_path, "bridge json"),
    ):
        expect(
            not artifact_path.exists(),
            f"expected source-surface-only interop provider not to publish the deferred {label} artifact",
        )
    expect(
        runtime_import_surface.get(
            "objc_interop_foreign_surface_interface_and_module_preservation", {}
        ).get("contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected interop provider runtime import surface to preserve the foreign surface/interface preservation contract",
    )
    expect(
        runtime_bridge_packet.get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected interop provider runtime import surface to preserve the header/module/bridge generation contract",
    )
    expect(
        bridge_generation_surface.get("header_artifact_relative_path")
        == runtime_bridge_packet.get("header_artifact_relative_path")
        == "module.interop-bridge.h"
        and bridge_generation_surface.get("module_artifact_relative_path")
        == runtime_bridge_packet.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and bridge_generation_surface.get("bridge_artifact_relative_path")
        == runtime_bridge_packet.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected interop provider source and runtime import surfaces to preserve the canonical deferred bridge artifact paths",
    )
    expect(
        bridge_generation_surface.get("runtime_generation_ready") is False
        and bridge_generation_surface.get("cross_module_packaging_ready") is False
        and bridge_generation_surface.get("deterministic") is False
        and runtime_bridge_packet.get("runtime_generation_ready") is False
        and runtime_bridge_packet.get("cross_module_packaging_ready") is False
        and runtime_bridge_packet.get("deterministic") is False,
        "expected source-surface-only interop provider to preserve bridge paths without claiming generated bridge artifacts",
    )

    return CaseResult(
        case_id="cross-module-runtime-package-interop-source-surface",
        probe="compile-manifest-runtime-import-surface-and-deferred-bridge-boundary",
        fixture=INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "module_name": runtime_import_surface.get("module_name"),
            "runtime_import_surface_path": str(
                runtime_import_surface_path.relative_to(ROOT)
            ).replace("\\", "/"),
            "bridge_artifact_paths_deferred": [
                "module.interop-bridge.h",
                "module.interop-bridge.modulemap",
                "module.interop-bridge.json",
            ],
            "runtime_generation_ready": runtime_bridge_packet.get(
                "runtime_generation_ready"
            ),
            "cross_module_packaging_ready": runtime_bridge_packet.get(
                "cross_module_packaging_ready"
            ),
        },
    )


__all__ = ["check_cross_module_runtime_package_interop_source_surface_case"]
