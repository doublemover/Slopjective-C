from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "reports" / "claimability" / "parser-draft-syntax-surface"
JSON_OUT = REPORT_DIR / "parser_draft_syntax_surface_summary.json"
MD_OUT = REPORT_DIR / "parser_draft_syntax_surface_summary.md"

POSITIVE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "recovery"
    / "dispatch"
    / "parser_draft_syntax_surfaces.objc3"
)
NEGATIVE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "recovery"
    / "negative"
    / "negative_parser_draft_syntax_macro_payload.objc3"
)
README = ROOT / "tests" / "conformance" / "parser" / "README.md"
AST = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_BUILDER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder.h"
AST_BUILDER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder.cpp"
PARSER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h"
ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.cpp"


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")
