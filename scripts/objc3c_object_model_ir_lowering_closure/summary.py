from __future__ import annotations

import json

from objc3c_object_model_ir_lowering_closure.compiler import parse_ivar_offsets
from objc3c_object_model_ir_lowering_closure.compiler import run_compiler
from objc3c_object_model_ir_lowering_closure.contracts import check_source_surfaces
from objc3c_object_model_ir_lowering_closure.contracts import expected_replay_key
from objc3c_object_model_ir_lowering_closure.inputs import EXPECTED_LAYOUT
from objc3c_object_model_ir_lowering_closure.paths import ARTIFACTS
from objc3c_object_model_ir_lowering_closure.paths import CONFORMANCE_MANIFEST
from objc3c_object_model_ir_lowering_closure.paths import CONFORMANCE_NEGATIVE
from objc3c_object_model_ir_lowering_closure.paths import CONFORMANCE_POSITIVE
from objc3c_object_model_ir_lowering_closure.paths import CONFORMANCE_README
from objc3c_object_model_ir_lowering_closure.paths import CONTRACT_ID
from objc3c_object_model_ir_lowering_closure.paths import IR_EMITTER
from objc3c_object_model_ir_lowering_closure.paths import IR_EMITTER_H
from objc3c_object_model_ir_lowering_closure.paths import ISSUE
from objc3c_object_model_ir_lowering_closure.paths import LOWERING_CONTRACT
from objc3c_object_model_ir_lowering_closure.paths import NEGATIVE_FIXTURE
from objc3c_object_model_ir_lowering_closure.paths import POSITIVE_FIXTURE
from objc3c_object_model_ir_lowering_closure.paths import RUNTIME
from objc3c_object_model_ir_lowering_closure.paths import RUNTIME_BOOTSTRAP
from objc3c_object_model_ir_lowering_closure.paths import SCRATCH
from objc3c_object_model_ir_lowering_closure.paths import STRESS_MANIFEST
from objc3c_object_model_ir_lowering_closure.paths import read
from objc3c_object_model_ir_lowering_closure.paths import rel


def build_summary() -> dict:
    positive_out = SCRATCH / "positive"
    negative_out = SCRATCH / "negative"
    positive_result = run_compiler(POSITIVE_FIXTURE, positive_out)
    negative_result = run_compiler(NEGATIVE_FIXTURE, negative_out)

    manifest_path = positive_out / "module.manifest.json"
    ir_path = positive_out / "module.ll"
    diagnostics_path = negative_out / "module.diagnostics.json"
    manifest = json.loads(read(manifest_path)) if manifest_path.is_file() else {}
    ir_text = read(ir_path) if ir_path.is_file() else ""
    negative_diagnostics = (
        json.loads(read(diagnostics_path)).get("diagnostics", [])
        if diagnostics_path.is_file()
        else []
    )

    property_records = {
        record["property_name"]: record
        for record in manifest.get("runtime_metadata_source_records", {}).get("properties", [])
    }
    ivar_records = {
        record["property_name"]: record
        for record in manifest.get("runtime_metadata_source_records", {}).get("ivars", [])
    }
    source_layout_checks = {}
    ir_layout_checks = {}
    ir_offsets = parse_ivar_offsets(ir_text)
    for property_name, expected in EXPECTED_LAYOUT.items():
        source_record = property_records.get(property_name, {})
        ivar_record = ivar_records.get(property_name, {})
        replay_key = expected_replay_key(property_name, expected)
        source_layout_checks[property_name] = {
            "property_offset_matches": source_record.get("executable_ivar_layout_offset_bytes") == expected["offset"],
            "property_padding_matches": source_record.get("executable_ivar_layout_padding_bytes") == expected["padding"],
            "property_inherited_slots_match": source_record.get("executable_ivar_layout_inherited_slot_count") == expected["inherited_slots"],
            "property_inherited_size_matches": source_record.get("executable_ivar_layout_inherited_size_bytes") == expected["inherited_size"],
            "property_owner_size_matches": source_record.get("executable_ivar_layout_owner_size_bytes") == expected["owner_size"],
            "property_replay_key_matches": source_record.get("executable_ivar_layout_replay_key") == replay_key,
            "property_layout_valid": source_record.get("executable_ivar_layout_valid") is True,
            "ivar_offset_matches": ivar_record.get("executable_ivar_layout_offset_bytes") == expected["offset"],
            "ivar_replay_key_matches": ivar_record.get("executable_ivar_layout_replay_key") == replay_key,
        }
        ir_layout_checks[property_name] = {
            "offset_global_matches": ir_offsets.get(property_name) == expected["offset"],
            "replay_key_emitted": replay_key in ir_text,
            "layout_record_tuple_emitted": (
                f"i64 {expected['slot']}, i64 {expected['offset']}, "
                f"i64 {expected['size']}, i64 {expected['alignment']}, "
                f"i64 {expected['padding']}, i64 {expected['inherited_slots']}, "
                f"i64 {expected['inherited_size']}, i64 {expected['owner_size']}"
            )
            in ir_text,
        }

    source_surfaces = check_source_surfaces()
    negative_codes = [diag.get("code") for diag in negative_diagnostics]
    emission_summary_checks = {
        "descriptor_model_expanded": "descriptor_model=ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering" in ir_text,
        "offset_global_entries": "offset_global_entries=4" in ir_text,
        "layout_table_entries": "layout_table_entries=2" in ir_text,
        "layout_owner_entries": "layout_owner_entries=2" in ir_text,
        "child_owner_table_size": "i64 24 }" in ir_text,
    }
    source_truth_paths = [
        POSITIVE_FIXTURE,
        NEGATIVE_FIXTURE,
        IR_EMITTER,
        IR_EMITTER_H,
        ARTIFACTS,
        RUNTIME,
        RUNTIME_BOOTSTRAP,
        LOWERING_CONTRACT,
        STRESS_MANIFEST,
        CONFORMANCE_MANIFEST,
        CONFORMANCE_README,
        CONFORMANCE_POSITIVE,
        CONFORMANCE_NEGATIVE,
    ]
    no_tmp_source_truth = all(not rel(path).startswith("tmp/") for path in source_truth_paths)
    status_inputs = [
        positive_result.returncode == 0,
        negative_result.returncode != 0,
        "O3P150" in negative_codes,
        all(all(values.values()) for values in source_layout_checks.values()),
        all(all(values.values()) for values in ir_layout_checks.values()),
        all(emission_summary_checks.values()),
        all(all(values.values()) for values in source_surfaces.values()),
        no_tmp_source_truth,
    ]
    return {
        "contract_id": CONTRACT_ID,
        "issue": ISSUE,
        "status": "PASS" if all(status_inputs) else "FAIL",
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "positive_compile_returncode": positive_result.returncode,
        "negative_compile_returncode": negative_result.returncode,
        "negative_diagnostic_codes": negative_codes,
        "expected_layout": EXPECTED_LAYOUT,
        "source_layout_checks": source_layout_checks,
        "ir_offsets_by_property": ir_offsets,
        "ir_layout_checks": ir_layout_checks,
        "emission_summary_checks": emission_summary_checks,
        "source_surfaces": source_surfaces,
        "source_truth_paths": [rel(path) for path in source_truth_paths],
        "no_tmp_source_truth": no_tmp_source_truth,
        "scratch_outputs": [rel(positive_out), rel(negative_out)],
        "validation_commands": [
            "python scripts/build_objc3c_object_model_ir_lowering_closure.py --check",
            "python -m pytest tests/tooling/test_build_objc3c_object_model_ir_lowering_closure.py",
            "npm run objc3c -- test-execution-replay",
            "npm run objc3c -- test-lowering-runtime-stress",
            "npm run objc3c -- test-full",
        ],
    }
