from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = (
    ROOT
    / "tests"
    / "conformance"
    / "support_claim_runnable_evidence_catalog.json"
)
CAPABILITY_ID = "language.text.source-string-interpolation"
SUPPORT_CLAIM = "objc3c.behavior.language.text.source-string-interpolation"
POSITIVE_FIXTURE = (
    "tests/tooling/fixtures/native/execution/positive/"
    "source_string_interpolation_text_i32.objc3"
)
NEGATIVE_FIXTURES = [
    (
        "tests/tooling/fixtures/native/recovery/negative/"
        "negative_text_interpolation_empty_payload_rejected.objc3",
        "O3L011",
    ),
    (
        "tests/tooling/fixtures/native/recovery/negative/"
        "negative_text_interpolation_nested_rejected.objc3",
        "O3L011",
    ),
    (
        "tests/tooling/fixtures/native/recovery/negative/"
        "negative_text_interpolation_unterminated_rejected.objc3",
        "O3L011",
    ),
    (
        "tests/tooling/fixtures/native/recovery/negative/"
        "negative_text_interpolation_bool_payload_rejected.objc3",
        "O3S206",
    ),
]


def _read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def test_source_string_interpolation_lowering_is_source_backed() -> None:
    lexer_scan = _read("native/objc3c/src/lex/objc3_lexer_scanning.cpp")
    primary_parse = _read(
        "native/objc3c/src/parse/"
        "objc3_parser_core_primary_message_expressions_primary_literals_"
        "identifiers_dispatch.inc"
    )
    literal_builders = _read(
        "native/objc3c/src/parse/"
        "objc3_parser_literal_expression_nodes_scalar_literals.inc"
    )
    expr_kind = _read("native/objc3c/src/ast/objc3_ast_expr_kind_members.h")
    expr_members = _read(
        "native/objc3c/src/ast/objc3_ast_expr_control_flow_members.h"
    )
    sema_literals = _read(
        "native/objc3c/src/sema/"
        "objc3_semantic_passes_generic_protocol_message_validation_expr_"
        "literal_identifier_cases.inc"
    )
    ir_expr = _read("native/objc3c/src/ir/objc3_ir_expression_emission.cpp")
    ir_decls = _read(
        "native/objc3c/src/ir/"
        "objc3_ir_prototype_declarations_runtime_helpers.cpp"
    )
    positive_fixture = _read(POSITIVE_FIXTURE)

    assert "case '('" in lexer_scan
    assert "nested string interpolation is unsupported" in lexer_scan
    assert "unterminated string interpolation" in lexer_scan
    assert "empty string interpolation payload" in lexer_scan

    assert "TryDecodeStringInterpolationToken" in primary_parse
    assert "ParseStringInterpolationPayload" in primary_parse
    assert "BuildObjc3StringInterpolationExpr" in primary_parse
    assert "StringInterpolation" in expr_kind
    assert "string_interpolation_segments" in expr_members
    assert "string_interpolation_payload_is_text" in expr_members
    assert "literal->kind = Expr::Kind::StringInterpolation" in literal_builders

    assert "O3S206" in sema_literals
    assert "string interpolation payloads support only Text and i32" in sema_literals
    assert "objc3_runtime_stdlib_text_builder_i32" in ir_expr
    assert "objc3_runtime_stdlib_text_builder_append_text_i32" in ir_expr
    assert "objc3_runtime_stdlib_text_builder_append_i32_i32" in ir_expr
    assert "objc3_runtime_stdlib_text_builder_build_i32" in ir_expr
    assert "Objc3IRExprRequiresTextLiteralHelperDeclarations" in ir_decls

    assert '"\\(label)=\\(value)"' in positive_fixture
    assert "objc3_runtime_stdlib_text_equal_i32(actual, expected)" in positive_fixture


def test_source_string_interpolation_negative_fixtures_are_strict() -> None:
    for path, code in NEGATIVE_FIXTURES:
        fixture = _read(path)
        assert f"Expected diagnostic code(s): {code}" in fixture
        assert "\\(" in fixture


def test_source_string_interpolation_support_claim_is_evidence_backed() -> None:
    matrix = _read_json(MATRIX_PATH)
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)

    matrix_rows = {str(row["id"]): row for row in matrix["capabilities"]}
    manifest_claims = {
        str(claim["claim_id"]): claim
        for claim in manifest["support_claims"]
        if isinstance(claim, dict)
    }
    catalog_rows = {
        str(row["support_claim"]): row
        for row in catalog["rows"]
        if isinstance(row, dict)
    }
    fixture_paths = {
        str(fixture["path"])
        for fixture in manifest["fixtures"]
        if isinstance(fixture, dict)
    }

    matrix_row = matrix_rows[CAPABILITY_ID]
    claim = manifest_claims[SUPPORT_CLAIM]
    catalog_row = catalog_rows[SUPPORT_CLAIM]

    assert matrix_row["state"] == "implemented"
    assert matrix_row["support_claims"] == [SUPPORT_CLAIM]
    assert claim["owner_phase"] == catalog_row["owner_phase"] == "lowering"
    assert claim["behavior_fixture"] == POSITIVE_FIXTURE
    assert claim["behavior_fixture"] in fixture_paths
    assert catalog_row["capability_id"] == CAPABILITY_ID
    assert catalog_row["conformance_fixture"] == POSITIVE_FIXTURE
    assert catalog_row["runnable_command"] == "npm run objc3c -- test-execution-smoke"
    assert POSITIVE_FIXTURE in catalog_row["positive_evidence"]
    assert "tests/tooling/test_source_string_interpolation_text_support.py" in catalog_row[
        "positive_evidence"
    ]
    assert {"O3L011", "O3S206"}.issubset(
        set(catalog_row["required_diagnostic_codes"])
    )
    for path, _code in NEGATIVE_FIXTURES:
        assert path in catalog_row["negative_evidence"]
