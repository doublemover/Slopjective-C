from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m193_frontend_simd_vector_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M193 frontend SIMD/vector type lowering",
        "TryParseVectorTypeSpelling(...)",
        "fn.return_vector_spelling = true;",
        "fn.return_vector_base_spelling = vector_base_spelling;",
        "fn.return_vector_lane_count = vector_lane_count;",
        "param.vector_spelling = true;",
        "param.vector_base_spelling = vector_base_spelling;",
        "param.vector_lane_count = vector_lane_count;",
        "i32x2/i32x4/i32x8/i32x16",
        "boolx2/boolx4/boolx8/boolx16",
        "python -m pytest tests/tooling/test_objc3c_m193_frontend_simd_vector_lowering_contract.py -q",
    ):
        assert text in fragment


def test_m193_frontend_simd_vector_markers_map_to_sources() -> None:
    ast_source = _read(AST_SOURCE)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool vector_spelling = false;",
        "unsigned vector_lane_count = 1;",
        "bool return_vector_spelling = false;",
        "unsigned return_vector_lane_count = 1;",
    ):
        assert marker in ast_source

    for marker in (
        "static bool TryParseVectorTypeSpelling(const Token &type_token,",
        "const bool is_i32_vector = text.rfind(\"i32x\", 0) == 0;",
        "const bool is_bool_vector = text.rfind(\"boolx\", 0) == 0;",
        "if (lane_count != 2u && lane_count != 4u && lane_count != 8u && lane_count != 16u) {",
        "fn.return_vector_spelling = true;",
        "fn.return_vector_base_spelling = vector_base_spelling;",
        "fn.return_vector_lane_count = vector_lane_count;",
        "param.vector_spelling = true;",
        "param.vector_base_spelling = vector_base_spelling;",
        "param.vector_lane_count = vector_lane_count;",
    ):
        assert marker in parser_source
