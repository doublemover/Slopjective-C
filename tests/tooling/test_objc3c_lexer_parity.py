from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LEX_HEADER = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.h"
LEX_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_lexer_module_exists_and_pipeline_consumes_it() -> None:
    assert LEX_HEADER.exists(), f"missing lexer header: {LEX_HEADER}"
    assert LEX_SOURCE.exists(), f"missing lexer source: {LEX_SOURCE}"

    pipeline_cpp = read_text(PIPELINE_CPP)
    assert '#include "lex/objc3_lexer.h"' in pipeline_cpp
    assert "class Objc3Lexer {" not in pipeline_cpp


def test_cmake_registers_lexer_target() -> None:
    cmake = read_text(CMAKE_FILE)
    assert "add_library(objc3c_lex STATIC" in cmake
    assert "src/lex/objc3_lexer.cpp" in cmake


def test_lexer_consumes_language_version_pragmas_with_deterministic_diagnostics() -> None:
    lexer_header = read_text(LEX_HEADER)
    lexer_source = read_text(LEX_SOURCE)
    assert "struct Objc3LexerOptions" in lexer_header
    assert "struct Objc3LexerMigrationHints" in lexer_header
    assert "const Objc3LexerMigrationHints &MigrationHints() const;" in lexer_header
    assert "ConsumeLanguageVersionPragmas(diagnostics);" in lexer_source
    assert "MatchLiteral(\"objc_language_version\")" in lexer_source
    assert "if (options_.migration_assist) {" in lexer_source
    assert "migration_hints_.legacy_yes_count" in lexer_source
    assert "migration_hints_.legacy_no_count" in lexer_source
    assert "migration_hints_.legacy_null_count" in lexer_source
    assert "version != std::to_string(options_.language_version)" in lexer_source
    assert (
        "malformed '#pragma objc_language_version' directive; expected '#pragma objc_language_version(3)'"
        in lexer_source
    )
    assert "MakeDiag(directive_line, directive_column, \"O3L005\"" in lexer_source
    assert "unsupported objc language version '" in lexer_source
    assert "MakeDiag(version_line, version_column, \"O3L006\"" in lexer_source


def test_pipeline_consumes_lexer_migration_hints_surface() -> None:
    pipeline_cpp = read_text(PIPELINE_CPP)
    assert "Objc3LexerOptions lexer_options;" in pipeline_cpp
    assert "lexer_options.migration_assist = options.migration_assist;" in pipeline_cpp
    assert "result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;" in pipeline_cpp
