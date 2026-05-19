from __future__ import annotations

import argparse
from pathlib import Path

from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs
from objc3c_tooling.validation import contains_all

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "reports" / "claimability" / "parser-container-layout"
JSON_OUT = REPORT_DIR / "parser_container_layout_summary.json"
MD_OUT = REPORT_DIR / "parser_container_layout_summary.md"

CONTRACT_ID = "objc3c.parser.container-ivar-layout-closure.v1"
POSITIVE_FIXTURE = ROOT / "tests/tooling/fixtures/native/recovery/dispatch/parser_container_inherited_ivar_layout.objc3"
NEGATIVE_FIXTURE = ROOT / "tests/tooling/fixtures/native/recovery/negative/negative_parser_container_ivar_layout_cycle.objc3"
README = ROOT / "tests/conformance/parser/README.md"
AST = ROOT / "native/objc3c/src/ast/objc3_ast.h"
PARSER = ROOT / "native/objc3c/src/parse/objc3_parser.cpp"
SEMA = ROOT / "native/objc3c/src/sema/objc3_sema_contract.h"
PIPELINE_TYPES = ROOT / "native/objc3c/src/pipeline/objc3_frontend_types.h"
PIPELINE = ROOT / "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
ARTIFACTS = ROOT / "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp"
IMPORT_SURFACE = ROOT / "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp"

REQUIRED_LAYOUT_FIELDS = [
    "executable_ivar_layout_symbol",
    "executable_ivar_layout_slot_index",
    "executable_ivar_layout_size_bytes",
    "executable_ivar_layout_alignment_bytes",
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

REQUIRED_PARSER_ANCHORS = [
    "FinalizeObjcProgramPropertyIvarLayoutClosure",
    "FinalizeObjcInterfacePropertyIvarLayoutClosure",
    "BuildObjcIvarLayoutReplayKey",
    "O3P150",
    "O3P151",
    "O3P152",
]


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")



def build_summary() -> dict:
    ast_text = read(AST)
    parser_text = read(PARSER)
    sema_text = read(SEMA)
    pipeline_types_text = read(PIPELINE_TYPES)
    pipeline_text = read(PIPELINE)
    artifacts_text = read(ARTIFACTS)
    import_text = read(IMPORT_SURFACE)
    positive_text = read(POSITIVE_FIXTURE)
    negative_text = read(NEGATIVE_FIXTURE)
    readme_text = read(README)

    field_presence = {
        "ast_property_decl": contains_all(ast_text, REQUIRED_LAYOUT_FIELDS),
        "sema_property_info": contains_all(sema_text, REQUIRED_LAYOUT_FIELDS),
        "pipeline_runtime_records": contains_all(pipeline_types_text, REQUIRED_LAYOUT_FIELDS),
        "pipeline_copy": contains_all(pipeline_text, REQUIRED_LAYOUT_FIELDS),
        "artifact_json": contains_all(artifacts_text, REQUIRED_LAYOUT_FIELDS),
        "runtime_import_surface": contains_all(import_text, REQUIRED_LAYOUT_FIELDS),
    }
    parser_presence = contains_all(parser_text, REQUIRED_PARSER_ANCHORS)
    fixture_presence = {
        "positive_fixture_exists": POSITIVE_FIXTURE.is_file(),
        "negative_fixture_exists": NEGATIVE_FIXTURE.is_file(),
        "positive_fixture_declares_superclass": "@interface OC3LayoutChild : OC3LayoutBase" in positive_text,
        "positive_fixture_declares_mixed_alignment_properties": "@property bool enabled;" in positive_text and "@property id token;" in positive_text,
        "negative_fixture_declares_cycle": "@interface OC3CycleA : OC3CycleB" in negative_text and "@interface OC3CycleB : OC3CycleA" in negative_text,
        "negative_fixture_expects_o3p150": "Expected diagnostic code(s): O3P150" in negative_text,
        "readme_references_positive_fixture": rel(POSITIVE_FIXTURE) in readme_text,
        "readme_references_negative_fixture": rel(NEGATIVE_FIXTURE) in readme_text,
    }
    source_truth_paths = [
        AST, PARSER, SEMA, PIPELINE_TYPES, PIPELINE, ARTIFACTS,
        IMPORT_SURFACE, POSITIVE_FIXTURE, NEGATIVE_FIXTURE, README,
    ]
    no_tmp_source_truth = all("/tmp/" not in rel(path) and not rel(path).startswith("tmp/") for path in source_truth_paths)
    all_fields_present = all(all(section.values()) for section in field_presence.values())
    all_parser_anchors_present = all(parser_presence.values())
    all_fixtures_present = all(fixture_presence.values())
    status = "PASS" if all_fields_present and all_parser_anchors_present and all_fixtures_present and no_tmp_source_truth else "FAIL"
    return {
        "contract_id": CONTRACT_ID,
        "status": status,
        "issue": "#8010",
        "source_truth_paths": [rel(path) for path in source_truth_paths],
        "no_tmp_source_truth": no_tmp_source_truth,
        "required_layout_fields": REQUIRED_LAYOUT_FIELDS,
        "field_presence": field_presence,
        "required_parser_anchors": REQUIRED_PARSER_ANCHORS,
        "parser_presence": parser_presence,
        "fixture_presence": fixture_presence,
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "validation_commands": [
            "npm run objc3c -- test-fixture-matrix",
            "npm run objc3c -- test-negative-expectations",
            "npm run objc3c -- test-execution-replay",
        ],
    }


def render_markdown(summary: dict) -> str:
    lines = [
        "# Parser Container Ivar Layout Closure",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        f"- Negative fixture: `{summary['negative_fixture']}`",
        f"- Source truth avoids tmp: `{summary['no_tmp_source_truth']}`",
        "",
        "## Checks",
    ]
    for section, values in summary["field_presence"].items():
        lines.append(f"- `{section}`: `{'PASS' if all(values.values()) else 'FAIL'}`")
    lines.append(f"- `parser_anchors`: `{'PASS' if all(summary['parser_presence'].values()) else 'FAIL'}`")
    lines.append(f"- `fixtures`: `{'PASS' if all(summary['fixture_presence'].values()) else 'FAIL'}`")
    lines.append("")
    lines.append("## Validation Commands")
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)


def write_outputs(summary: dict) -> None:
    write_report_outputs(
        summary=summary,
        json_path=JSON_OUT,
        markdown_path=MD_OUT,
        markdown=render_markdown(summary),
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    add_check_argument(parser)
    args = parser.parse_args()
    summary = build_summary()
    expected_json = expected_json_report(summary)
    expected_md = render_markdown(summary)
    if args.check:
        if not JSON_OUT.is_file() or JSON_OUT.read_text(encoding="utf-8") != expected_json:
            raise SystemExit(f"{rel(JSON_OUT)} is stale; run this script without --check")
        if not MD_OUT.is_file() or MD_OUT.read_text(encoding="utf-8") != expected_md:
            raise SystemExit(f"{rel(MD_OUT)} is stale; run this script without --check")
        if summary["status"] != "PASS":
            raise SystemExit("parser container layout closure summary failed")
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
