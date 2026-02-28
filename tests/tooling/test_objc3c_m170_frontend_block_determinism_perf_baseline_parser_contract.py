from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m170_frontend_block_determinism_perf_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M170 frontend block determinism/perf baseline parser/AST surface (M170-A001)",
        "BuildBlockDeterminismPerfBaselineWeight(...)",
        "BuildBlockDeterminismPerfBaselineProfile(...)",
        "block_determinism_perf_baseline_weight",
        "block_determinism_perf_baseline_profile",
        "block_determinism_perf_baseline_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m170_frontend_block_determinism_perf_baseline_parser_contract.py -q",
    ):
        assert text in fragment


def test_m170_frontend_block_determinism_perf_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "std::size_t block_determinism_perf_baseline_weight = 0;",
        "bool block_determinism_perf_baseline_profile_is_normalized = false;",
        "std::string block_determinism_perf_baseline_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildBlockDeterminismPerfBaselineWeight(",
        "BuildBlockDeterminismPerfBaselineProfile(",
        "ParseBlockLiteralExpression(",
        "block->block_determinism_perf_baseline_weight =",
        "block->block_determinism_perf_baseline_profile =",
        "block->block_determinism_perf_baseline_profile_is_normalized =",
    ):
        assert marker in parser_source
