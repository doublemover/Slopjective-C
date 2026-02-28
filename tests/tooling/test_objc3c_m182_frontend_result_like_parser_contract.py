from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m182_frontend_result_like_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M182 frontend result-like control-flow/lowering preparatory parser/AST surface (M182-A001)",
        "BuildResultLikeProfile(...)",
        "IsResultLikeProfileNormalized(...)",
        "CollectResultLikeExprProfile(...)",
        "CollectResultLikeStmtProfile(...)",
        "BuildResultLikeProfileFromBody(...)",
        "BuildResultLikeProfileFromOpaqueBody(...)",
        "FinalizeResultLikeProfile(FunctionDecl &fn)",
        "FinalizeResultLikeProfile(Objc3MethodDecl &method)",
        "target.result_like_profile = source.result_like_profile;",
        "normalized_sites + branch_merge_sites == result_like_sites",
        "python -m pytest tests/tooling/test_objc3c_m182_frontend_result_like_parser_contract.py -q",
    ):
        assert text in fragment


def test_m182_frontend_result_like_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool result_like_profile_is_normalized = false;",
        "bool deterministic_result_like_lowering_handoff = false;",
        "std::size_t result_like_sites = 0;",
        "std::size_t result_success_sites = 0;",
        "std::size_t result_failure_sites = 0;",
        "std::size_t result_branch_sites = 0;",
        "std::size_t result_payload_sites = 0;",
        "std::size_t result_normalized_sites = 0;",
        "std::size_t result_branch_merge_sites = 0;",
        "std::size_t result_contract_violation_sites = 0;",
        "std::string result_like_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildResultLikeProfile(",
        "IsResultLikeProfileNormalized(",
        "CollectResultLikeExprProfile(",
        "CollectResultLikeStmtProfile(",
        "BuildResultLikeProfileFromBody(",
        "BuildResultLikeProfileFromOpaqueBody(",
        "FinalizeResultLikeProfile(FunctionDecl &fn)",
        "FinalizeResultLikeProfile(Objc3MethodDecl &method)",
        "target.result_like_profile = source.result_like_profile;",
        "target.deterministic_result_like_lowering_handoff =",
        "FinalizeResultLikeProfile(*fn);",
        "FinalizeResultLikeProfile(method);",
        "result-like-lowering:result_like_sites=",
        "profile.normalized_sites + profile.branch_merge_sites != profile.result_like_sites",
    ):
        assert marker in parser_source
