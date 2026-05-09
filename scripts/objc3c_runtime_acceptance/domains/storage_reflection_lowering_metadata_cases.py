"""Storage/reflection runtime acceptance case helpers."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs

from ..core import (
    DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
    ROOT,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)

def check_accessor_storage_lowering_metadata_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "accessor-storage-lowering-metadata"

    def load_compile_artifacts(fixture_name: str, output_name: str) -> tuple[dict[str, Any], str]:
        fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name
        _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / output_name)
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        return manifest, ll_path.read_text(encoding="utf-8")

    def find_property_entry(entries: list[dict[str, Any]], owner_kind: str, owner_name: str, property_name: str) -> dict[str, Any]:
        for entry in entries:
            if (
                entry.get("owner_kind") == owner_kind
                and entry.get("owner_name") == owner_name
                and entry.get("property_name") == property_name
            ):
                return entry
        raise RuntimeError(
            f"missing property entry for {owner_kind}:{owner_name}:{property_name}"
        )

    def expect_property_lowering(
        manifest: dict[str, Any],
        owner_kind: str,
        owner_name: str,
        property_name: str,
        synthesizes_executable_accessors: bool,
        getter_helper_symbol: str,
        setter_helper_symbol: str,
    ) -> None:
        runtime_metadata_records = manifest.get("runtime_metadata_source_records", {})
        property_records = runtime_metadata_records.get("properties", [])
        expect(
            isinstance(property_records, list),
            "expected runtime metadata source records to publish property entries",
        )
        property_record = find_property_entry(
            property_records, owner_kind, owner_name, property_name
        )
        expect(
            property_record.get("synthesizes_executable_accessors")
            is synthesizes_executable_accessors,
            f"expected runtime metadata property record to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_record.get("getter_storage_runtime_helper_symbol")
            == getter_helper_symbol,
            f"expected runtime metadata property record to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_record.get("setter_storage_runtime_helper_symbol")
            == setter_helper_symbol,
            f"expected runtime metadata property record to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )

        source_graph = manifest.get("objc_executable_metadata_source_graph")
        if not isinstance(source_graph, dict):
            source_graph = manifest.get("executable_metadata_source_graph")
        if not isinstance(source_graph, dict):
            source_graph = (
                manifest.get("frontend", {})
                .get("pipeline", {})
                .get("semantic_surface", {})
                .get("objc_executable_metadata_source_graph", {})
            )
        property_nodes = source_graph.get("property_node_entries", [])
        expect(
            isinstance(property_nodes, list),
            "expected executable metadata source graph to publish property nodes",
        )
        property_node = find_property_entry(
            property_nodes, owner_kind, owner_name, property_name
        )
        expect(
            property_node.get("synthesizes_executable_accessors")
            is synthesizes_executable_accessors,
            f"expected executable metadata property node to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_node.get("getter_storage_runtime_helper_symbol")
            == getter_helper_symbol,
            f"expected executable metadata property node to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_node.get("setter_storage_runtime_helper_symbol")
            == setter_helper_symbol,
            f"expected executable metadata property node to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )

    synthesized_manifest, synthesized_ll = load_compile_artifacts(
        "synthesized_accessor_property_lowering_positive.objc3",
        "synthesized-accessors",
    )
    synthesized_lowering_surface = synthesized_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        isinstance(synthesized_lowering_surface, dict),
        "expected synthesized accessor lowering metadata fixture to publish the lowering surface",
    )
    expect(
        synthesized_lowering_surface.get("contract_id")
        == DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        "expected synthesized accessor lowering metadata fixture to preserve the lowering surface contract id",
    )
    expect(
        synthesized_lowering_surface.get("compile_manifest_artifact") == "module.manifest.json",
        "expected lowering surface to couple back to the compile manifest artifact",
    )
    expect(
        synthesized_lowering_surface.get("registration_manifest_artifact")
        == "module.runtime-registration-manifest.json",
        "expected lowering surface to couple back to the runtime registration manifest artifact",
    )
    expect(
        synthesized_lowering_surface.get("object_artifact") == "module.obj"
        and synthesized_lowering_surface.get("backend_artifact") == "module.ll",
        "expected lowering surface to couple back to the emitted object and LLVM IR artifacts",
    )
    expect(
        synthesized_lowering_surface.get(
            "runtime_property_ivar_storage_accessor_source_surface_contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected lowering surface to point back at the runtime property/ivar storage source surface",
    )
    expect(
        synthesized_lowering_surface.get(
            "storage_accessor_runtime_abi_surface_contract_id"
        )
        == RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "expected lowering surface to point at the storage/accessor runtime ABI surface",
    )
    expect(
        synthesized_lowering_surface.get("lowering_contract_source_path")
        == "native/objc3c/src/lower/objc3_lowering_contract.h",
        "expected lowering surface to publish the lowering contract source path",
    )
    expect(
        synthesized_lowering_surface.get("ir_emitter_source_path")
        == "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "expected lowering surface to publish the IR emitter source path",
    )
    expect(
        synthesized_lowering_surface.get("frontend_artifacts_source_path")
        == "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        "expected lowering surface to publish the frontend artifacts source path",
    )
    expect(
        synthesized_lowering_surface.get("runtime_source_path")
        == "native/objc3c/src/runtime/objc3_runtime.cpp",
        "expected lowering surface to publish the runtime source path",
    )
    expect(
        synthesized_lowering_surface.get("accessor_storage_lowering_metadata_model")
        == "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path",
        "expected lowering surface to publish the accessor-storage metadata model",
    )
    expect(
        synthesized_lowering_surface.get(
            "accessor_storage_lowering_helper_selection_model"
        )
        == "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers",
        "expected lowering surface to publish the helper-selection model",
    )
    expect(
        synthesized_lowering_surface.get("authoritative_fixture_paths")
        == [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "expected lowering surface to publish the authoritative fixture set",
    )
    expect(
        synthesized_lowering_surface.get("authoritative_probe_paths")
        == [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "expected lowering surface to publish the authoritative probe set",
    )
    expect(
        synthesized_lowering_surface.get("explicit_non_goals")
        == [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-lowering-proof",
        ],
        "expected lowering surface to publish explicit non-goals",
    )
    expect(
        synthesized_lowering_surface.get("requires_coupled_registration_manifest")
        is True
        and synthesized_lowering_surface.get("requires_real_compile_output") is True
        and synthesized_lowering_surface.get("requires_linked_runtime_probe") is True,
        "expected lowering surface to require the coupled registration manifest, real compile output, and linked runtime probes",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_accessor_owner_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered property owners",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_getter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered getters",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_setter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered setters",
    )
    expect(
        synthesized_lowering_surface.get("current_property_read_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three current-property read entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_write_entries") == 2,
        "expected synthesized accessor lowering metadata fixture to publish two current-property write entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_exchange_entries") == 1,
        "expected synthesized accessor lowering metadata fixture to publish one current-property exchange entry",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_load_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-load entries",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_store_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-store entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_read_symbol")
        == "objc3_runtime_read_current_property_i32",
        "expected lowering surface to publish the canonical current-property read symbol",
    )
    expect(
        synthesized_lowering_surface.get("current_property_write_symbol")
        == "objc3_runtime_write_current_property_i32",
        "expected lowering surface to publish the canonical current-property write symbol",
    )
    expect(
        synthesized_lowering_surface.get("current_property_exchange_symbol")
        == "objc3_runtime_exchange_current_property_i32",
        "expected lowering surface to publish the canonical current-property exchange symbol",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_load_symbol")
        == "objc3_runtime_load_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property load symbol",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_store_symbol")
        == "objc3_runtime_store_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property store symbol",
    )
    expect(
        "getter_definitions=3" in synthesized_ll
        and "setter_definitions=3" in synthesized_ll
        and "read_current_property_calls=3" in synthesized_ll
        and "write_current_property_calls=2" in synthesized_ll
        and "exchange_current_property_calls=1" in synthesized_ll,
        "expected synthesized accessor lowering metadata fixture LLVM IR to agree with the published lowering counts",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-interface",
        "Widget",
        "count",
        False,
        "",
        "",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "count",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "enabled",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "value",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )

    arc_manifest, arc_ll = load_compile_artifacts(
        "arc_property_interaction_positive.objc3",
        "arc-accessors",
    )
    arc_lowering_surface = arc_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        arc_lowering_surface.get("synthesized_accessor_owner_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered property owners",
    )
    expect(
        arc_lowering_surface.get("synthesized_getter_entries") == 2
        and arc_lowering_surface.get("synthesized_setter_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered getters and setters",
    )
    expect(
        arc_lowering_surface.get("current_property_read_entries") == 1,
        "expected ARC property interaction fixture to publish one plain/strong getter read entry",
    )
    expect(
        arc_lowering_surface.get("current_property_write_entries") == 0,
        "expected ARC property interaction fixture to publish zero plain write entries",
    )
    expect(
        arc_lowering_surface.get("current_property_exchange_entries") == 1,
        "expected ARC property interaction fixture to publish one strong exchange entry",
    )
    expect(
        arc_lowering_surface.get("weak_current_property_load_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-load entry",
    )
    expect(
        arc_lowering_surface.get("weak_current_property_store_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-store entry",
    )
    expect(
        "exchange_current_property_calls=1" in arc_ll
        and "weak_load_current_property_calls=1" in arc_ll
        and "weak_store_current_property_calls=1" in arc_ll,
        "expected ARC property interaction fixture LLVM IR to agree with the published helper-lowering counts",
    )
    expect_property_lowering(
        arc_manifest,
        "class-interface",
        "ArcBox",
        "currentValue",
        False,
        "",
        "",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "currentValue",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "weakValue",
        True,
        "objc3_runtime_load_weak_current_property_i32",
        "objc3_runtime_store_weak_current_property_i32",
    )

    return CaseResult(
        case_id="accessor-storage-lowering-metadata-surface",
        probe="compile-manifest-and-executable-metadata-surface",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "synthesized_accessor_owner_entries": synthesized_lowering_surface.get(
                "synthesized_accessor_owner_entries"
            ),
            "synthesized_getter_entries": synthesized_lowering_surface.get(
                "synthesized_getter_entries"
            ),
            "synthesized_setter_entries": synthesized_lowering_surface.get(
                "synthesized_setter_entries"
            ),
            "strong_exchange_entries": synthesized_lowering_surface.get(
                "current_property_exchange_entries"
            ),
            "weak_store_entries": arc_lowering_surface.get(
                "weak_current_property_store_entries"
            ),
        },
    )


