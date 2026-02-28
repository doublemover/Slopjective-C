from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
TOKEN_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m211_sema_lsp_profile_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M211 sema/type LSP semantic profile",
        "semantic tokens/navigation support in LSP clients",
        "lsp packet 1.1 deterministic semantic-token metadata",
        "m211_lsp_semantic_token_metadata_packet",
        "### 1.1 Deterministic semantic-token metadata packet",
        "Objc3SemaTokenMetadata",
        "MakeObjc3SemaTokenMetadata(...)",
        "MakeSemaTokenMetadata(...)",
        "Objc3SemaTokenKind::PointerDeclarator",
        "Objc3SemaTokenKind::NullabilitySuffix",
        "pointer_declarator_tokens",
        "nullability_suffix_tokens",
        "return_pointer_declarator_tokens",
        "return_nullability_suffix_tokens",
        "lsp packet 1.2 deterministic navigation source-anchor",
        "m211_lsp_navigation_source_anchor_packet",
        "### 1.2 Deterministic navigation source-anchor packet",
        "CanonicalizePassDiagnostics(...)",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "frontend.pipeline.semantic_surface",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "program.globals[i].line",
        "program.globals[i].column",
        "fn.line",
        "fn.column",
        "python -m pytest tests/tooling/test_objc3c_m211_sema_lsp_contract.py -q",
    ):
        assert text in fragment


def test_m211_sema_lsp_profile_maps_to_source_anchors() -> None:
    token_contract_source = _read(TOKEN_CONTRACT_SOURCE)
    ast_source = _read(AST_SOURCE)
    parser_source = _read(PARSER_SOURCE)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in ("Objc3SemaTokenMetadata", "MakeObjc3SemaTokenMetadata"):
        assert marker in token_contract_source

    for marker in (
        "pointer_declarator_tokens",
        "nullability_suffix_tokens",
        "return_pointer_declarator_tokens",
        "return_nullability_suffix_tokens",
    ):
        assert marker in ast_source

    assert "MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous())" in parser_source
    assert "MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance())" in parser_source

    assert "CanonicalizePassDiagnostics(pass_diagnostics);" in sema_source
    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline_source

    for marker in (
        '\\"semantic_surface\\":',
        '\\"resolved_global_symbols\\":',
        '\\"resolved_function_symbols\\":',
        "program.globals[i].line",
        "program.globals[i].column",
        "fn.line",
        "fn.column",
    ):
        assert marker in artifacts_source
