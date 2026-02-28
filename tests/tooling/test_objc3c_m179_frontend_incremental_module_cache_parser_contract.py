from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m179_frontend_incremental_module_cache_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M179 frontend incremental module cache and invalidation parser/AST surface (M179-A001)",
        "BuildIncrementalModuleCacheInvalidationProfile(...)",
        "IsIncrementalModuleCacheInvalidationProfileNormalized(...)",
        "incremental_module_cache_invalidation_profile",
        "incremental_module_cache_invalidation_profile_is_normalized",
        "return_incremental_module_cache_invalidation_profile",
        "return_incremental_module_cache_invalidation_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m179_frontend_incremental_module_cache_parser_contract.py -q",
    ):
        assert text in fragment


def test_m179_frontend_incremental_module_cache_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool incremental_module_cache_invalidation_profile_is_normalized = false;",
        "std::string incremental_module_cache_invalidation_profile;",
        "bool return_incremental_module_cache_invalidation_profile_is_normalized = false;",
        "std::string return_incremental_module_cache_invalidation_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildIncrementalModuleCacheInvalidationProfile(",
        "IsIncrementalModuleCacheInvalidationProfileNormalized(",
        "CountNamespaceSegments(",
        "param.incremental_module_cache_invalidation_profile =",
        "param.incremental_module_cache_invalidation_profile_is_normalized =",
        "fn.return_incremental_module_cache_invalidation_profile =",
        "fn.return_incremental_module_cache_invalidation_profile_is_normalized =",
        "target.incremental_module_cache_invalidation_profile =",
        "target.return_incremental_module_cache_invalidation_profile =",
    ):
        assert marker in parser_source
