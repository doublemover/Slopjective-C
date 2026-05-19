from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LEX_HEADER = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.h"
LEX_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_STAGE_RUNNER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_stage_runner.cpp"
LEX_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "lex" / "CMakeLists.txt"


def read_text(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "'):
            continue
        include_path = stripped.split('"', 2)[1]
        target = ROOT / "native" / "objc3c" / "src" / include_path
        if target.exists():
            expanded.append(target.read_text(encoding="utf-8"))
    return "\n".join(expanded)


def test_lexer_module_exists_and_pipeline_consumes_it() -> None:
    assert LEX_HEADER.exists(), f"missing lexer header: {LEX_HEADER}"
    assert LEX_SOURCE.exists(), f"missing lexer source: {LEX_SOURCE}"

    assert PIPELINE_STAGE_RUNNER.exists(), f"missing pipeline stage runner: {PIPELINE_STAGE_RUNNER}"

    pipeline_cpp = read_text(PIPELINE_STAGE_RUNNER)
    assert '#include "lex/objc3_lexer.h"' in pipeline_cpp
    assert "class Objc3Lexer {" not in pipeline_cpp


def test_cmake_registers_lexer_target() -> None:
    cmake = read_text(LEX_CMAKE_FILE)
    assert "add_library(objc3c_lex STATIC" in cmake
    assert "objc3_lexer.cpp" in cmake


def test_lexer_consumes_language_version_pragmas_with_deterministic_diagnostics() -> None:
    lexer_header = read_text(LEX_HEADER)
    lexer_source = read_text(LEX_SOURCE)
    assert "struct Objc3LexerOptions" in lexer_header
    assert "struct Objc3LexerCanonicalLiteralRejectionCounts" in lexer_header
    assert "struct Objc3LexerLanguageVersionPragmaContract" in lexer_header
    assert "const Objc3LexerCanonicalLiteralRejectionCounts" in lexer_header
    assert "&CanonicalLiteralRejectionCounts() const;" in lexer_header
    assert (
        "const Objc3LexerLanguageVersionPragmaContract &LanguageVersionPragmaContract() const;"
        in lexer_header
    )
    assert "ConsumePreludePragmas(diagnostics);" in lexer_source
    assert "ConsumeLanguageVersionPragmaDirective(" in lexer_source
    assert "LanguageVersionPragmaPlacement::kNonLeading" in lexer_source
    assert "MatchLiteral(\"objc_language_version\")" in lexer_source
    assert "canonical_literal_rejection_counts_.yes_literal_sites" in lexer_source
    assert "canonical_literal_rejection_counts_.no_literal_sites" in lexer_source
    assert "canonical_literal_rejection_counts_.null_literal_sites" in lexer_source
    assert 'MakeDiag(token_line, token_column, "O3C002"' in lexer_source
    assert "ClassifyObjc3RejectedCanonicalLiteral(ident)" in lexer_source
    assert "Objc3RejectedCanonicalLiteralReplacementSpelling(rejected_literal)" in lexer_source
    assert "directive_count > 1" in lexer_source
    assert "language_version_pragma_contract_.non_leading = true;" in lexer_source
    assert "version != std::to_string(options_.language_version)" in lexer_source
    assert (
        "malformed '#pragma objc_language_version' directive; expected '#pragma objc_language_version(3)'"
        in lexer_source
    )
    assert "MakeDiag(directive_line, directive_column, \"O3L005\"" in lexer_source
    assert "unsupported objc language version '" in lexer_source
    assert "MakeDiag(version_line, version_column, \"O3L006\"" in lexer_source
    assert (
        "duplicate '#pragma objc_language_version' directive; only one file-scope prelude pragma is allowed"
        in lexer_source
    )
    assert "MakeDiag(directive_line, directive_column, \"O3L007\"" in lexer_source
    assert (
        "language-version pragma must stay in the file-scope prelude before declarations or tokens"
        in lexer_source
    )
    assert "MakeDiag(directive_line, directive_column, \"O3L008\"" in lexer_source


def test_pipeline_consumes_lexer_canonical_literal_rejection_counts_surface() -> None:
    pipeline_cpp = read_text(PIPELINE_STAGE_RUNNER)
    assert "Objc3LexerOptions lexer_options;" in pipeline_cpp
    assert "const Objc3LexerCanonicalLiteralRejectionCounts &lexer_counts =" in pipeline_cpp
    assert "lexer.CanonicalLiteralRejectionCounts()" in pipeline_cpp
    assert (
        "result.canonical_literal_rejection_counts.yes_literal_sites ="
        in pipeline_cpp
    )
    assert "lexer_counts.yes_literal_sites;" in pipeline_cpp
    assert "lexer.LanguageVersionPragmaContract()" in pipeline_cpp
    assert "result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;" in pipeline_cpp
