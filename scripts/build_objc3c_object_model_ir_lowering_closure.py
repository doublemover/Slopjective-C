from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "reports" / "claimability" / "object-model-ir-lowering"
JSON_OUT = REPORT_DIR / "object_model_ir_lowering_summary.json"
MD_OUT = REPORT_DIR / "object_model_ir_lowering_summary.md"

CONTRACT_ID = "objc3c.object-model-ir-lowering-closure.v1"
ISSUE = "#8016"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
SCRATCH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "object-model-ir-lowering-closure"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "dispatch" / "parser_container_inherited_ivar_layout.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_parser_container_ivar_layout_cycle.objc3"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
RUNTIME = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
RUNTIME_BOOTSTRAP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
CONFORMANCE_MANIFEST = ROOT / "tests" / "conformance" / "lowering_abi" / "manifest.json"
CONFORMANCE_README = ROOT / "tests" / "conformance" / "lowering_abi" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "OBJIR-8016-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "OBJIR-8016-02.json"

EXPECTED_LAYOUT = {
    "enabled": {
        "slot": 0,
        "offset": 0,
        "size": 1,
        "alignment": 1,
        "padding": 0,
        "inherited_slots": 0,
        "inherited_size": 0,
        "owner_size": 8,
    },
    "baseCount": {
        "slot": 1,
        "offset": 4,
        "size": 4,
        "alignment": 4,
        "padding": 3,
        "inherited_slots": 0,
        "inherited_size": 0,
        "owner_size": 8,
    },
    "token": {
        "slot": 2,
        "offset": 8,
        "size": 8,
        "alignment": 8,
        "padding": 0,
        "inherited_slots": 2,
        "inherited_size": 8,
        "owner_size": 24,
    },
    "childFlag": {
        "slot": 3,
        "offset": 16,
        "size": 1,
        "alignment": 1,
        "padding": 0,
        "inherited_slots": 2,
        "inherited_size": 8,
        "owner_size": 24,
    },
}

REQUIRED_LAYOUT_FIELDS = [
    "executable_ivar_layout_offset_bytes",
    "executable_ivar_layout_padding_bytes",
    "executable_ivar_layout_inherited_slot_count",
    "executable_ivar_layout_inherited_size_bytes",
    "executable_ivar_layout_owner_size_bytes",
    "executable_ivar_init_order_index",
    "executable_ivar_destroy_order_index",
    "executable_ivar_layout_valid",
    "executable_ivar_layout_replay_key",
]

REQUIRED_RUNTIME_FIELDS = [
    "ivar_layout_replay_key",
    "ivar_layout_offset_bytes",
    "ivar_layout_padding_bytes",
    "ivar_layout_inherited_slot_count",
    "ivar_layout_inherited_size_bytes",
    "ivar_layout_owner_size_bytes",
    "ivar_init_order_index",
    "ivar_destroy_order_index",
    "ivar_layout_valid",
    "layout_replay_key",
    "padding_bytes",
    "inherited_slot_count",
    "inherited_size_bytes",
    "owner_size_bytes",
    "init_order_index",
    "destroy_order_index",
    "layout_valid",
]


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def run_compiler(source: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    return subprocess.run(
        [
            str(COMPILER),
            str(source),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )


def parse_ivar_offsets(ir_text: str) -> dict[str, int]:
    name_by_index: dict[str, str] = {}
    offset_by_index: dict[str, int] = {}
    for match in re.finditer(
        r"@__objc3_meta_ivar_property_name_(\d+)\s*=\s*private constant \[\d+ x i8\] c\"([^\"]*)\\00\"",
        ir_text,
    ):
        name_by_index[match.group(1)] = match.group(2)
    for match in re.finditer(
        r"@__objc3_meta_ivar_offset_(\d+)\s*=\s*private global i64 (\d+)",
        ir_text,
    ):
        offset_by_index[match.group(1)] = int(match.group(2))
    return {
        name: offset_by_index[index]
        for index, name in name_by_index.items()
        if index in offset_by_index
    }


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
            "npm run test:objc3c:execution-replay-proof",
            "npm run test:objc3c:lowering-runtime-stress",
            "npm run test:objc3c:full",
        ],
    }


def render_markdown(summary: dict) -> str:
    lines = [
        "# Object Model IR Lowering Closure",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        f"- Negative fixture: `{summary['negative_fixture']}`",
        f"- Source truth avoids tmp: `{summary['no_tmp_source_truth']}`",
        "",
        "## Layout Offsets",
    ]
    for property_name, offset in summary["ir_offsets_by_property"].items():
        lines.append(f"- `{property_name}`: `{offset}`")
    lines.extend(["", "## Checks"])
    for name, values in summary["emission_summary_checks"].items():
        lines.append(f"- `{name}`: `{'PASS' if values else 'FAIL'}`")
    for section, values in summary["source_surfaces"].items():
        lines.append(f"- `{section}`: `{'PASS' if all(values.values()) else 'FAIL'}`")
    lines.extend(["", "## Validation Commands"])
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)


def write_outputs(summary: dict) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    MD_OUT.write_text(render_markdown(summary), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    summary = build_summary()
    expected_json = json.dumps(summary, indent=2, sort_keys=True) + "\n"
    expected_md = render_markdown(summary)
    if args.check:
        if not JSON_OUT.is_file() or JSON_OUT.read_text(encoding="utf-8") != expected_json:
            raise SystemExit(f"{rel(JSON_OUT)} is stale; run this script without --check")
        if not MD_OUT.is_file() or MD_OUT.read_text(encoding="utf-8") != expected_md:
            raise SystemExit(f"{rel(MD_OUT)} is stale; run this script without --check")
        if summary["status"] != "PASS":
            raise SystemExit("object model IR lowering closure summary failed")
        print(f"status: {summary['status']}")
        print(f"summary_path: {rel(JSON_OUT)}")
        return 0
    write_outputs(summary)
    print(f"wrote: {rel(JSON_OUT)}")
    print(f"wrote: {rel(MD_OUT)}")
    print(f"status: {summary['status']}")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
