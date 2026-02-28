from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m198_frontend_swift_metadata_bridge_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M198 frontend swift metadata bridge",
        "MakeSemaTokenMetadata(...)",
        "Objc3SemaTokenKind::PointerDeclarator",
        "Objc3SemaTokenKind::NullabilitySuffix",
        "std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens",
        "std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens",
        "BuildObjc3AstFromTokens(tokens)",
        "sema_input.program = &result.program;",
        "result.type_metadata_handoff",
        "result.deterministic_type_metadata_handoff",
        "python -m pytest tests/tooling/test_objc3c_m198_frontend_swift_metadata_bridge_contract.py -q",
    ):
        assert text in fragment


def test_m198_frontend_swift_metadata_bridge_markers_map_to_sources() -> None:
    ast_source = _read(AST_SOURCE)
    parser_source = _read(PARSER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)

    for marker in (
        "std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;",
        "std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;",
        "std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;",
        "std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;",
    ):
        assert marker in ast_source

    for marker in (
        "MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous())",
        "MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance())",
    ):
        assert marker in parser_source

    for marker in (
        "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);",
        "sema_input.program = &result.program;",
    ):
        assert marker in pipeline_source

    for marker in (
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
    ):
        assert marker in sema_pass_manager_source
