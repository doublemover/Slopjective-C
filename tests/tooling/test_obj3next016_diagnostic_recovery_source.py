from __future__ import annotations

import json
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PARSE = ROOT / "native" / "objc3c" / "src" / "parse"
SEMA = ROOT / "native" / "objc3c" / "src" / "sema"
FIXTURES = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def compile_negative_fixture(fixture: Path, tmp_path: Path) -> list[dict[str, object]]:
    assert NATIVE_EXE.exists(), (
        "native compiler binary must exist before running diagnostics recovery test"
    )

    out_dir = tmp_path / fixture.stem
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = subprocess.run(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode != 0

    diagnostics_path = out_dir / "module.diagnostics.json"
    payload = json.loads(diagnostics_path.read_text(encoding="utf-8"))
    diagnostics = payload["diagnostics"]
    assert isinstance(diagnostics, list)
    return [diagnostic for diagnostic in diagnostics if isinstance(diagnostic, dict)]


def test_parser_diagnostic_builder_publishes_machine_fixit_and_recovery_metadata() -> None:
    header = read(PARSE / "objc3_parser_diagnostics.h")
    builder = read(PARSE / "objc3_parser_diagnostic_builder.cpp")
    rejection = read(PARSE / "objc3_parser_rejection_diagnostics.cpp")

    assert "struct Objc3ParserDiagnosticFixIt" in header
    assert "BuildObjc3ParserDiagnosticWithFixIt" in header
    assert "BuildObjc3ParserDiagnosticWithFixItAndRecovery" in header
    assert "BuildObjc3ParserDiagnosticWithRecovery" in header
    assert "machine-applicable=" in builder
    assert "recovery-counts-as-success=false" in builder
    assert "AppendObjc3ParserRecoveryMetadata(" in builder
    assert "insert-missing-semicolon-after-" in rejection
    assert "parser-statement-boundary-synchronization" in rejection
    assert "replace-optional-alias-with-Optional" in rejection
    assert "parser-canonical-spelling-rejection" in rejection
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


def test_obj3next016_unknown_symbol_fixture_requires_nearest_symbol_suggestion() -> None:
    case_path = (
        ROOT
        / "tests"
        / "conformance"
        / "diagnostics"
        / "OBJ3-NEXT-016-SEMA-RECOVERY-02.json"
    )
    case = json.loads(read(case_path))
    diagnostic = case["expect"]["diagnostics"][0]

    assert diagnostic["code"] == "O3S203"
    assert diagnostic["message"] == "unknown function 'known_vaule'"
    assert diagnostic["suggestions"] == [
        {
            "kind": "nearest-symbol",
            "symbol_kind": "function",
            "target": "known_vaule",
            "replacement": "known_value",
            "confidence": "high",
            "range": {
                "start": {"line": 8, "column": 10},
                "end": {"line": 8, "column": 20},
            },
            "explanation": (
                "The only in-scope callable at edit distance two is 'known_value', "
                "so tooling can surface it as a nearest-symbol suggestion without "
                "accepting the misspelled call."
            ),
        }
    ]


def test_native_diagnostics_json_promotes_fixit_metadata_out_of_message(
    tmp_path: Path,
) -> None:
    diagnostics = compile_negative_fixture(
        FIXTURES
        / "negative"
        / "negative_obj3next016_parser_missing_semicolon_recovery.objc3",
        tmp_path,
    )
    missing_semicolon = diagnostics[0]

    assert missing_semicolon["code"] == "O3P104"
    assert missing_semicolon["message"] == "missing ';' after assignment"
    assert "{fix-it:" not in str(missing_semicolon["message"])
    assert missing_semicolon["explanation"]
    assert missing_semicolon["span"] == {
        "start": {"line": 10, "column": 3},
        "end": {"line": 10, "column": 4},
    }

    fixits = missing_semicolon["fixits"]
    assert isinstance(fixits, list)
    assert fixits == [
        {
            "applicability": "machine-applicable",
            "kind": "insert-text",
            "label": "insert-missing-semicolon-after-assignment",
            "range": {
                "start": {"line": 9, "column": 20},
                "end": {"line": 9, "column": 20},
            },
            "replacement": ";",
            "title": "insert-missing-semicolon-after-assignment",
        }
    ]

    recovery = missing_semicolon["recovery"]
    assert isinstance(recovery, dict)
    assert recovery == {
        "accepts_invalid_program": False,
        "available": True,
        "boundary": "next statement token",
        "deterministic": True,
        "recovery_counts_as_success": False,
        "strategy": "parser-statement-boundary-synchronization",
    }


def test_native_diagnostics_json_promotes_recovery_metadata_out_of_message(
    tmp_path: Path,
) -> None:
    diagnostics = compile_negative_fixture(
        FIXTURES
        / "negative"
        / "negative_obj3next016_optional_alias_fixit_recovery.objc3",
        tmp_path,
    )
    unsupported = next(
        diagnostic for diagnostic in diagnostics if diagnostic["code"] == "O3P100"
    )

    assert unsupported["message"] == "unsupported Objective-C 3 statement"
    assert "{recovery:" not in str(unsupported["message"])
    assert unsupported["explanation"]
    assert unsupported["fixits"] == []
    assert unsupported["recovery"] == {
        "accepts_invalid_program": False,
        "available": True,
        "boundary": "next-top-level-declaration-or-semicolon",
        "deterministic": True,
        "recovery_counts_as_success": False,
        "strategy": "skip-unsupported-top-level-fragment",
    }


def test_language_evolution_reserved_surfaces_fail_closed_with_specific_codes(
    tmp_path: Path,
) -> None:
    cases = {
        "negative_value_optional_canonical_reserved.objc3": "O3P159",
        "negative_typed_throws_reserved.objc3": "O3P182",
        "negative_match_expression_position_reserved.objc3": "O3P156",
        "negative_guarded_match_pattern_reserved.objc3": "O3P157",
        "negative_reify_generics_marker_reserved.objc3": "O3P114",
        "negative_generic_method_type_parameter_clause_reserved.objc3": "O3P114",
        "negative_cstyle_generic_function_reserved.objc3": "O3P114",
    }

    for fixture_name, expected_code in cases.items():
        diagnostics = compile_negative_fixture(
            FIXTURES / "negative" / fixture_name,
            tmp_path,
        )
        observed_codes = {
            diagnostic.get("code")
            for diagnostic in diagnostics
            if isinstance(diagnostic.get("code"), str)
        }
        assert expected_code in observed_codes
