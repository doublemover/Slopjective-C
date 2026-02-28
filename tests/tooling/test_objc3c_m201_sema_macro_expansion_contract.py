from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m201_sema_macro_expansion_architecture_and_isolation_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M201 sema/type macro expansion architecture and isolation",
        "deterministic sema/type macro-expansion architecture and isolation",
        "macro expansion packet 1.1 deterministic lexer expansion architecture hooks",
        "m201_sema_type_macro_expansion_architecture_packet",
        "### 1.1 Deterministic lexer expansion architecture packet",
        '} else if (ident == "YES") {',
        '} else if (ident == "NO") {',
        '} else if (ident == "NULL") {',
        "kind = TokenKind::KwTrue;",
        "kind = TokenKind::KwFalse;",
        "kind = TokenKind::KwNil;",
        "++migration_hints_.legacy_yes_count;",
        "++migration_hints_.legacy_no_count;",
        "++migration_hints_.legacy_null_count;",
        "const Objc3LexerMigrationHints &lexer_hints = lexer.MigrationHints();",
        "result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;",
        "result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;",
        "result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;",
        "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;",
        "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;",
        "bool migration_assist = false;",
        "Objc3SemaMigrationHints migration_hints;",
        "macro expansion packet 1.2 deterministic sema isolation hooks",
        "m201_sema_type_macro_expansion_isolation_packet",
        "### 1.2 Deterministic sema isolation packet",
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {",
        "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
        "AppendMigrationAssistDiagnostics(input, pass_diagnostics);",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        'manifest << "    \\"migration_assist\\":',
        'manifest << "    \\"migration_hints\\":{\\"legacy_yes\\":',
        "legacy_total()",
        "python -m pytest tests/tooling/test_objc3c_m201_sema_macro_expansion_contract.py -q",
    ):
        assert text in fragment


def test_m201_sema_macro_expansion_architecture_and_isolation_markers_map_to_sources() -> None:
    lexer_source = _read(LEXER_SOURCE)
    sema_contract_header = _read(SEMA_CONTRACT_HEADER)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        '} else if (ident == "YES") {',
        '} else if (ident == "NO") {',
        '} else if (ident == "NULL") {',
        "kind = TokenKind::KwTrue;",
        "kind = TokenKind::KwFalse;",
        "kind = TokenKind::KwNil;",
        "++migration_hints_.legacy_yes_count;",
        "++migration_hints_.legacy_no_count;",
        "++migration_hints_.legacy_null_count;",
    ):
        assert marker in lexer_source

    for marker in (
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "bool migration_assist = false;",
        "Objc3SemaMigrationHints migration_hints;",
    ):
        assert marker in sema_contract_header

    for marker in (
        "if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {",
        "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
        "AppendMigrationAssistDiagnostics(input, pass_diagnostics);",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
    ):
        assert marker in sema_source

    for marker in (
        "const Objc3LexerMigrationHints &lexer_hints = lexer.MigrationHints();",
        "result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;",
        "result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;",
        "result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;",
        "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;",
        "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;",
    ):
        assert marker in pipeline_source

    for marker in (
        'manifest << "    \\"migration_assist\\":',
        'manifest << "    \\"migration_hints\\":{\\"legacy_yes\\":',
        "legacy_total()",
    ):
        assert marker in artifacts_source
