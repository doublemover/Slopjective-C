"""Textual/binary interop packaging source-surface acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import INTEROP_HEADER_MODULE_PROVIDER_FIXTURE
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


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
    manifest = json.loads(
        (compile_dir / "module.manifest.json").read_text(encoding="utf-8")
    )
    runtime_import_surface = json.loads(
        (compile_dir / "module.runtime-import-surface.json").read_text(
            encoding="utf-8"
        )
    )
    bridge_header_path = compile_dir / "module.interop-bridge.h"
    bridge_modulemap_path = compile_dir / "module.interop-bridge.modulemap"
    bridge_json_path = compile_dir / "module.interop-bridge.json"
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
    for artifact_path, label in (
        (bridge_header_path, "bridge header"),
        (bridge_modulemap_path, "bridge modulemap"),
        (bridge_json_path, "bridge json"),
    ):
        expect(
            not artifact_path.exists(),
            f"expected textual/binary source-surface provider not to publish the deferred {label} artifact",
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
        "expected header/module bridge provider source and runtime import surfaces to preserve the canonical deferred bridge artifact paths",
    )
    expect(
        bridge_generation_surface.get("runtime_generation_ready") is False
        and bridge_generation_surface.get("cross_module_packaging_ready") is False
        and bridge_generation_surface.get("deterministic") is False
        and runtime_bridge_packet.get("runtime_generation_ready") is False
        and runtime_bridge_packet.get("cross_module_packaging_ready") is False
        and runtime_bridge_packet.get("deterministic") is False,
        "expected textual/binary source-surface provider to preserve bridge paths without claiming generated bridge artifacts",
    )
    expect(
        runtime_bridge_packet.get("local_foreign_callable_count") == 2
        and foreign_preservation_surface.get("local_foreign_callable_count") == 2,
        "expected header/module bridge provider source surfaces to preserve two foreign callables",
    )
    expect(
        annotation_source_surface.get("swift_name_annotation_sites") == 1
        and annotation_source_surface.get("cpp_name_annotation_sites") == 2
        and annotation_source_surface.get("header_name_annotation_sites") == 2,
        "expected header/module bridge provider source-completion surface to preserve the foreign C++/Swift annotation inventory",
    )
    return CaseResult(
        case_id="textual-binary-interface-parity-source-surface",
        probe="compile-manifest-runtime-import-surface-and-deferred-bridge-boundary",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "module_name": runtime_import_surface.get("module_name"),
            "foreign_callable_count": runtime_bridge_packet.get(
                "local_foreign_callable_count"
            ),
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


__all__ = ["check_textual_binary_interface_parity_source_surface_case"]
