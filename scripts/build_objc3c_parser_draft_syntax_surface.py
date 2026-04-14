from __future__ import annotations

import argparse
from pathlib import Path

from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "reports" / "claimability" / "parser-draft-syntax-surface"
JSON_OUT = REPORT_DIR / "parser_draft_syntax_surface_summary.json"
MD_OUT = REPORT_DIR / "parser_draft_syntax_surface_summary.md"

CONTRACT_ID = "objc3c.parser.draft-syntax-surface.v1"
POSITIVE_FIXTURE = ROOT / "tests/tooling/fixtures/native/recovery/dispatch/parser_draft_syntax_surfaces.objc3"
NEGATIVE_FIXTURE = ROOT / "tests/tooling/fixtures/native/recovery/negative/negative_parser_draft_syntax_macro_payload.objc3"
README = ROOT / "tests/conformance/parser/README.md"
AST = ROOT / "native/objc3c/src/ast/objc3_ast.h"
PARSER = ROOT / "native/objc3c/src/parse/objc3_parser.cpp"
AST_BUILDER_HEADER = ROOT / "native/objc3c/src/parse/objc3_ast_builder.h"
AST_BUILDER_SOURCE = ROOT / "native/objc3c/src/parse/objc3_ast_builder.cpp"
PARSER_CONTRACT = ROOT / "native/objc3c/src/parse/objc3_parser_contract.h"
ARTIFACTS = ROOT / "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"

REQUIRED_AST_FIELDS = [
    "struct Objc3DraftSyntaxSurfaceSummary",
    "block_literal_sites",
    "try_expression_sites",
    "throw_statement_sites",
    "do_catch_sites",
    "throws_callable_sites",
    "async_callable_sites",
    "await_expression_sites",
    "actor_interface_sites",
    "macro_attribute_sites",
    "macro_package_sites",
    "macro_provenance_sites",
    "property_behavior_sites",
    "interop_attribute_sites",
    "draft_syntax_surface_sites",
    "draft_syntax_surface_summary",
]

REQUIRED_PARSER_ANCHORS = [
    "BuildObjc3DraftSyntaxSurfaceSummary",
    "BuildObjc3DraftSyntaxSurfaceReplayKey",
    "CountDraftSyntaxExpr",
    "CountDraftSyntaxCallable",
    "ParseBlockLiteralExpression",
    "ParseOptionalThrowsClause",
    "ParseOptionalAsyncClause",
    "ParseObjcActorInterfaceDecl",
    "objc_macro_package",
    "objc_macro_provenance",
    "property_behavior_declared",
    "objc_import_module",
    "objc_status_code",
]

REQUIRED_CONTRACT_ANCHORS = [
    "draft_syntax_surface_count",
    "draft_syntax_surface_fingerprint",
    "draft_syntax_surface_handoff_key",
    "draft_syntax_surface_handoff_deterministic",
    "BuildObjc3DraftSyntaxSurfaceFingerprint",
]

REQUIRED_ARTIFACT_FIELDS = [
    "draft_syntax_surface_count",
    "draft_syntax_surface_fingerprint",
    "draft_syntax_surface_handoff_key",
    "draft_syntax_surface_deterministic",
]

POSITIVE_TOKENS = [
    "let callback = ^(i32 value)",
    "fn risky(value: i32) throws -> i32",
    "throw 7;",
    "let direct = try risky(0);",
    "let bridged = try? bridgeStatus(1, 0);",
    "do {",
    "catch (NSError* error)",
    "async fn runTask()",
    "return await produce();",
    "actor class SyntaxActor",
    "objc_executor(named(\"parser.actor\"))",
    "objc_macro(named(\"Trace\"))",
    "objc_macro_package(named(\"std.metaprogramming.trace\"))",
    "objc_macro_provenance(named(\"sha256:abc123\"))",
    "behavior=Observed",
    "objc_foreign",
    "objc_import_module(named(\"ParserKit\"))",
    "objc_status_code(success: 0, error_type: NSError, mapping: mapStatus)",
]


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def contains_all(text: str, tokens: list[str]) -> dict[str, bool]:
    return {token: token in text for token in tokens}


def build_summary() -> dict:
    ast_text = read(AST)
    parser_text = read(PARSER)
    builder_header_text = read(AST_BUILDER_HEADER)
    builder_source_text = read(AST_BUILDER_SOURCE)
    parser_contract_text = read(PARSER_CONTRACT)
    artifacts_text = read(ARTIFACTS)
    positive_text = read(POSITIVE_FIXTURE)
    negative_text = read(NEGATIVE_FIXTURE)
    readme_text = read(README)

    source_truth_paths = [
        AST,
        PARSER,
        AST_BUILDER_HEADER,
        AST_BUILDER_SOURCE,
        PARSER_CONTRACT,
        ARTIFACTS,
        POSITIVE_FIXTURE,
        NEGATIVE_FIXTURE,
        README,
    ]
    checks = {
        "ast_fields": contains_all(ast_text, REQUIRED_AST_FIELDS),
        "parser_anchors": contains_all(parser_text, REQUIRED_PARSER_ANCHORS),
        "ast_builder": {
            "header_declares_summary_setter": "SetDraftSyntaxSurfaceSummary" in builder_header_text,
            "source_writes_summary_to_program": "draft_syntax_surface_summary =" in builder_source_text,
        },
        "parser_contract": contains_all(parser_contract_text, REQUIRED_CONTRACT_ANCHORS),
        "artifact_manifest": contains_all(artifacts_text, REQUIRED_ARTIFACT_FIELDS),
        "positive_fixture": contains_all(positive_text, POSITIVE_TOKENS),
        "negative_fixture": {
            "negative_fixture_exists": NEGATIVE_FIXTURE.is_file(),
            "expects_o3p341": "Expected diagnostic code(s): O3P341" in negative_text,
            "malformed_macro_payload": "objc_macro(\"Trace\")" in negative_text,
        },
        "readme": {
            "references_positive_fixture": rel(POSITIVE_FIXTURE) in readme_text,
            "references_negative_fixture": rel(NEGATIVE_FIXTURE) in readme_text,
            "references_issue": "issue #8011" in readme_text,
        },
    }
    no_tmp_source_truth = all(not rel(path).startswith("tmp/") for path in source_truth_paths)
    status = "PASS" if all(all(section.values()) for section in checks.values()) and no_tmp_source_truth else "FAIL"
    return {
        "contract_id": CONTRACT_ID,
        "issue": "#8011",
        "status": status,
        "source_truth_paths": [rel(path) for path in source_truth_paths],
        "no_tmp_source_truth": no_tmp_source_truth,
        "checks": checks,
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "validation_commands": [
            "npm run test:objc3c:fixture-matrix",
            "npm run test:objc3c:negative-expectations",
            "npm run test:objc3c:execution-replay-proof",
        ],
    }


def render_markdown(summary: dict) -> str:
    lines = [
        "# Parser Draft Syntax Surface",
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
    for section, values in summary["checks"].items():
        lines.append(f"- `{section}`: `{'PASS' if all(values.values()) else 'FAIL'}`")
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
    parser.add_argument("--check", action="store_true")
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
            raise SystemExit("parser draft syntax surface summary failed")
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
