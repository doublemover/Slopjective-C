from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOKEN_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
LEXER_HEADER = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.h"
PARSER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_token_contract_surface_exists() -> None:
    contract = _read(TOKEN_CONTRACT_HEADER)
    assert "enum class Objc3LexTokenKind {" in contract
    assert "KwModule" in contract
    assert "GreaterGreaterEqual" in contract
    assert "struct Objc3LexToken {" in contract
    assert "using Objc3LexTokenStream = std::vector<Objc3LexToken>;" in contract
    assert "enum class Objc3SemaTokenKind {" in contract
    assert "PointerDeclarator" in contract
    assert "NullabilitySuffix" in contract
    assert "struct Objc3SemaTokenMetadata {" in contract
    assert "MakeObjc3SemaTokenMetadata(" in contract


def test_lexer_and_parser_depend_on_explicit_token_contract() -> None:
    lexer = _read(LEXER_HEADER)
    parser = _read(PARSER_HEADER)
    assert '#include "token/objc3_token_contract.h"' in lexer
    assert '#include "token/objc3_token.h"' not in lexer
    assert "std::vector<Objc3LexToken> Run(" in lexer
    assert '#include "token/objc3_token_contract.h"' in parser
    assert '#include "token/objc3_token.h"' not in parser
    assert "ParseObjc3Program(const std::vector<Objc3LexToken> &tokens)" in parser


def test_ast_uses_sema_token_contract_metadata() -> None:
    ast = _read(AST_HEADER)
    assert '#include "token/objc3_token_contract.h"' in ast
    assert "std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;" in ast
    assert "std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;" in ast
    assert "std::vector<Token> pointer_declarator_tokens;" not in ast
    assert "std::vector<Token> nullability_suffix_tokens;" not in ast


def test_parser_populates_sema_token_contract_metadata() -> None:
    parser = _read(PARSER_SOURCE)
    assert "MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator" in parser
    assert "MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix" in parser
