from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LEXER_HEADER = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.h"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_m222_validation_contract_for_pragma_semantics() -> None:
    lexer_header = _read(LEXER_HEADER)
    lexer_source = _read(LEXER_SOURCE)

    assert "struct Objc3LexerLanguageVersionPragmaContract {" in lexer_header
    assert "bool duplicate = false;" in lexer_header
    assert "bool non_leading = false;" in lexer_header
    assert "const Objc3LexerLanguageVersionPragmaContract &LanguageVersionPragmaContract() const;" in lexer_header

    assert '"O3L007"' in lexer_source
    assert '"O3L008"' in lexer_source
    assert "duplicate '#pragma objc_language_version' directive; only one file-scope prelude pragma is allowed" in lexer_source
    assert "language-version pragma must stay in the file-scope prelude before declarations or tokens" in lexer_source
    _assert_in_order(
        lexer_source,
        [
            "if (version != std::to_string(options_.language_version)) {",
            "RecordLanguageVersionPragmaObservation(directive_line, directive_column, placement);",
            'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));',
            'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));',
        ],
    )


def test_m222_validation_contract_for_migration_diagnostics() -> None:
    sema_contract = _read(SEMA_CONTRACT_HEADER)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)

    assert "enum class Objc3SemaCompatibilityMode" in sema_contract
    assert "struct Objc3SemaMigrationHints" in sema_contract
    assert "Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;" in sema_contract
    assert "bool migration_assist = false;" in sema_contract
    assert "Objc3SemaMigrationHints migration_hints;" in sema_contract

    assert "AppendMigrationAssistDiagnostics(const Objc3SemaPassManagerInput &input, std::vector<std::string> &diagnostics)" in sema_source
    assert "if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {" in sema_source
    assert '"O3S216"' in sema_source
    _assert_in_order(
        sema_source,
        [
            'append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");',
            'append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");',
            'append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");',
        ],
    )
    _assert_in_order(
        sema_source,
        [
            "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
            "AppendMigrationAssistDiagnostics(input, pass_diagnostics);",
            "CanonicalizePassDiagnostics(pass_diagnostics);",
        ],
    )

    _assert_in_order(
        pipeline_source,
        [
            "sema_input.validation_options = semantic_options;",
            "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy",
            "? Objc3SemaCompatibilityMode::Legacy",
            ": Objc3SemaCompatibilityMode::Canonical;",
            "sema_input.migration_assist = options.migration_assist;",
        ],
    )
