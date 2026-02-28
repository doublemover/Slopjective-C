from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
TOKEN_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m161_frontend_ownership_qualifier_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M161 frontend ownership qualifier parser/AST surface",
        "IsOwnershipQualifierSpelling(...)",
        "BuildOwnershipQualifierSymbol(...)",
        "Objc3SemaTokenKind::OwnershipQualifier",
        "param.has_ownership_qualifier",
        "fn.has_return_ownership_qualifier",
        "python -m pytest tests/tooling/test_objc3c_m161_frontend_ownership_qualifier_parser_contract.py -q",
    ):
        assert text in fragment


def test_m161_frontend_ownership_qualifier_markers_map_to_sources() -> None:
    token_contract = _read(TOKEN_CONTRACT_HEADER)
    ast_source = _read(AST_SOURCE)
    parser_source = _read(PARSER_SOURCE)

    assert "OwnershipQualifier" in token_contract

    for marker in (
        "bool has_ownership_qualifier = false;",
        "std::string ownership_qualifier_spelling;",
        "std::string ownership_qualifier_symbol;",
        "std::vector<Objc3SemaTokenMetadata> ownership_qualifier_tokens;",
        "bool has_return_ownership_qualifier = false;",
        "std::string return_ownership_qualifier_spelling;",
        "std::string return_ownership_qualifier_symbol;",
        "std::vector<Objc3SemaTokenMetadata> return_ownership_qualifier_tokens;",
    ):
        assert marker in ast_source

    for marker in (
        "static bool IsOwnershipQualifierSpelling(const std::string &text)",
        "static std::string BuildOwnershipQualifierSymbol(const std::string &spelling, bool is_return_type)",
        "MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier)",
        "param.ownership_qualifier_symbol = BuildOwnershipQualifierSymbol(param.ownership_qualifier_spelling, false);",
        "fn.return_ownership_qualifier_symbol =",
        "BuildOwnershipQualifierSymbol(fn.return_ownership_qualifier_spelling, true);",
    ):
        assert marker in parser_source
