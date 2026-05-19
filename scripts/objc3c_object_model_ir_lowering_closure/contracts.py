from __future__ import annotations

import json

from objc3c_object_model_ir_lowering_closure.inputs import REQUIRED_LAYOUT_FIELDS
from objc3c_object_model_ir_lowering_closure.inputs import REQUIRED_RUNTIME_FIELDS
from objc3c_object_model_ir_lowering_closure.paths import ARTIFACTS
from objc3c_object_model_ir_lowering_closure.paths import CONFORMANCE_MANIFEST
from objc3c_object_model_ir_lowering_closure.paths import CONFORMANCE_NEGATIVE
from objc3c_object_model_ir_lowering_closure.paths import CONFORMANCE_POSITIVE
from objc3c_object_model_ir_lowering_closure.paths import CONFORMANCE_README
from objc3c_object_model_ir_lowering_closure.paths import IR_EMITTER
from objc3c_object_model_ir_lowering_closure.paths import IR_EMITTER_H
from objc3c_object_model_ir_lowering_closure.paths import LOWERING_CONTRACT
from objc3c_object_model_ir_lowering_closure.paths import POSITIVE_FIXTURE
from objc3c_object_model_ir_lowering_closure.paths import RUNTIME
from objc3c_object_model_ir_lowering_closure.paths import RUNTIME_BOOTSTRAP
from objc3c_object_model_ir_lowering_closure.paths import STRESS_MANIFEST
from objc3c_object_model_ir_lowering_closure.paths import read
from objc3c_object_model_ir_lowering_closure.paths import rel


def expected_replay_key(property_name: str, layout: dict[str, int]) -> str:
    owner = "interface:OC3LayoutChild" if layout["inherited_slots"] else "interface:OC3LayoutBase"
    return (
        f"owner={owner};property={property_name};slot={layout['slot']};"
        f"offset={layout['offset']};size={layout['size']};"
        f"alignment={layout['alignment']};padding={layout['padding']};"
        f"inherited_slots={layout['inherited_slots']};"
        f"inherited_size={layout['inherited_size']};owner_size={layout['owner_size']}"
    )


def check_source_surfaces() -> dict[str, object]:
    emitter_text = read(IR_EMITTER)
    emitter_h_text = read(IR_EMITTER_H)
    artifacts_text = read(ARTIFACTS)
    runtime_text = read(RUNTIME)
    runtime_bootstrap_text = read(RUNTIME_BOOTSTRAP)
    lowering_contract_text = read(LOWERING_CONTRACT)
    stress_manifest = json.loads(read(STRESS_MANIFEST))
    conformance_manifest = json.loads(read(CONFORMANCE_MANIFEST))
    conformance_files = {
        "positive_exists": CONFORMANCE_POSITIVE.is_file(),
        "negative_exists": CONFORMANCE_NEGATIVE.is_file(),
        "readme_references_objir": "OBJIR-8016-01.json" in read(CONFORMANCE_README)
        and "OBJIR-8016-02.json" in read(CONFORMANCE_README),
        "manifest_references_objir": any(
            set(group.get("files", [])) >= {"OBJIR-8016-01.json", "OBJIR-8016-02.json"}
            for group in conformance_manifest.get("groups", [])
        ),
    }
    return {
        "ir_bundle_fields": {
            field: field in emitter_h_text and field in artifacts_text
            for field in REQUIRED_LAYOUT_FIELDS
        },
        "ir_emitter_uses_sema_offsets": {
            "offset_globals_from_sema_offsets": "bundle.executable_ivar_layout_offset_bytes" in emitter_text,
            "layout_records_include_replay_keys": "layout_replay_key_symbol" in emitter_text,
            "layout_records_include_padding": "bundle.executable_ivar_layout_padding_bytes" in emitter_text,
            "layout_records_include_inheritance": "bundle.executable_ivar_layout_inherited_slot_count" in emitter_text
            and "bundle.executable_ivar_layout_inherited_size_bytes" in emitter_text,
            "layout_tables_sort_by_slot": "std::stable_sort(\n                owner_descriptor_symbols.begin()" in emitter_text,
            "no_descriptor_offset_rederivation_vector": "descriptor_offsets" not in emitter_text,
        },
        "runtime_consumes_layout_payload": {
            field: field in runtime_text or field in runtime_bootstrap_text
            for field in REQUIRED_RUNTIME_FIELDS
        },
        "contract_strings": {
            "descriptor_model_expanded": "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering" in lowering_contract_text,
            "bundle_model_expanded": "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records" in lowering_contract_text,
            "property_payload_expanded": "property-descriptor-records-with-accessor-binding-and-sema-ivar-layout-fields" in lowering_contract_text,
            "ivar_payload_expanded": "ivar-descriptor-records-with-property-binding-layout-replay-key-and-offset-global" in lowering_contract_text,
        },
        "stress_manifest": {
            "positive_fixture_in_compile_cases": rel(POSITIVE_FIXTURE)
            in stress_manifest.get("compile_cases", []),
        },
        "conformance": conformance_files,
    }
