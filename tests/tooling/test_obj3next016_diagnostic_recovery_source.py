from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PARSE = ROOT / "native" / "objc3c" / "src" / "parse"
SEMA = ROOT / "native" / "objc3c" / "src" / "sema"
FIXTURES = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery"


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_parser_diagnostic_builder_publishes_machine_fixit_and_recovery_metadata() -> None:
    header = read(PARSE / "objc3_parser_diagnostics.h")
    builder = read(PARSE / "objc3_parser_diagnostic_builder.cpp")
    rejection = read(PARSE / "objc3_parser_rejection_diagnostics.cpp")

    assert "struct Objc3ParserDiagnosticFixIt" in header
    assert "BuildObjc3ParserDiagnosticWithFixIt" in header
    assert "BuildObjc3ParserDiagnosticWithRecovery" in header
    assert "machine-applicable=" in builder
    assert "recovery-counts-as-success=false" in builder
    assert "insert-missing-semicolon-after-" in rejection
    assert "replace-optional-alias-with-Optional" in rejection
    assert "skip-unsupported-top-level-fragment" in rejection


def test_parser_recovery_boundaries_stop_at_declaration_and_case_anchors() -> None:
    boundaries = read(PARSE / "objc3_parser_recovery_boundaries.cpp")

    assert "Objc3LexTokenKind::KwVar" in boundaries
    assert "Objc3LexTokenKind::KwCase" in boundaries
    assert "Objc3LexTokenKind::KwDefault" in boundaries
    assert "Objc3LexTokenKind::KwAtEnd" in boundaries


def test_sema_missing_return_diagnostic_uses_recovery_contract_helper() -> None:
    contract_header = read(SEMA / "objc3_sema_diagnostic_contract.h")
    contract_impl = read(SEMA / "objc3_sema_diagnostic_contract.cpp")
    sema_entrypoints = read(SEMA / "objc3_semantic_passes_body_validation_entrypoints.inc")

    assert "BuildObjc3SemaDiagnosticWithRecovery" in contract_header
    assert "BuildObjc3SemaMissingReturnDiagnostic" in contract_header
    assert "preserve-callable-body-and-continue-semantic-validation" in contract_impl
    assert "callable-body-end" in contract_impl
    assert "BuildObjc3SemaMissingReturnDiagnostic" in sema_entrypoints
    assert "MakeDiag(fn.line, fn.column, \"O3S205\"" not in sema_entrypoints


def test_obj3next016_recovery_fixtures_cover_parser_then_sema_followup() -> None:
    negative = read(
        FIXTURES
        / "negative"
        / "negative_obj3next016_parser_recovery_preserves_followup_sema.objc3"
    )
    positive = read(
        FIXTURES / "positive" / "obj3next016_parser_recovery_followup_baseline.objc3"
    )

    assert "// Expected diagnostic code(s): O3P104 O3S205." in negative
    assert "let value = 1\n" in negative
    assert "fn second() -> i32" in negative
    assert "return value + 1;" in positive
    assert "Expected diagnostic" not in positive
