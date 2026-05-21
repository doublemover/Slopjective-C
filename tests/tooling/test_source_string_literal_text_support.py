from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CAPABILITY_ID = "language.text.source-string-literal-text-shape-handle"
SUPPORT_CLAIM = (
    "objc3c.behavior.language.text.source-string-literal-text-shape-handle"
)
POSITIVE_FIXTURE = (
    "tests/tooling/fixtures/native/execution/positive/"
    "source_string_literal_text_shape_handle.objc3"
)
INVALID_ESCAPE_FIXTURE = (
    "tests/tooling/fixtures/native/recovery/negative/"
    "negative_text_literal_invalid_escape_rejected.objc3"
)
INTERPOLATION_FIXTURE = (
    "tests/tooling/fixtures/native/recovery/negative/"
    "negative_text_literal_interpolation_rejected.objc3"
)
PUBLIC_COMMAND = (
    "npm run objc3c -- compile-objc3c "
    "tests/tooling/fixtures/native/execution/positive/"
    "source_string_literal_text_shape_handle.objc3"
)


def _read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def _read_json(path: str) -> dict:
    payload = json.loads(_read(path))
    assert isinstance(payload, dict)
    return payload


def test_source_string_literals_are_fail_closed_text_shape_handles() -> None:
    lexer_run = _read("native/objc3c/src/lex/objc3_lexer_run.cpp")
    lexer_scan = _read("native/objc3c/src/lex/objc3_lexer_scanning.cpp")
    lexer_chars = _read("native/objc3c/src/lex/objc3_lexer_char_class.cpp")
    primary_parse = _read(
        "native/objc3c/src/parse/"
        "objc3_parser_core_primary_message_expressions_primary_literals_"
        "identifiers_dispatch.inc"
    )
    param_type_parse = _read(
        "native/objc3c/src/parse/"
        "objc3_parser_core_objc_declarations_parameter_type_spelling_identifier.inc"
    )
    return_type_parse = _read(
        "native/objc3c/src/parse/"
        "objc3_parser_core_objc_declarations_function_return_type_parser_"
        "spelling_capture.inc"
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
    type_queries = _read("native/objc3c/src/ast/objc3_ast_value_type_queries.cpp")
    ir_expr = _read("native/objc3c/src/ir/objc3_ir_expression_emission.cpp")
    ir_decls = _read(
        "native/objc3c/src/ir/"
        "objc3_ir_prototype_declarations_runtime_helpers.cpp"
    )

    assert "ConsumeStringLiteral" in lexer_run
    assert {"O3L010", "O3L011", "O3L012"}.issubset(set(lexer_scan.split('"')))
    assert "TryCountObjc3Utf8Scalars(value, unit_count)" in lexer_scan
    assert "EscapeObjc3StringTokenText(value)" in lexer_scan
    assert "TryDecodeObjc3StringTokenText" in lexer_chars

    assert "Match(TokenKind::String)" in primary_parse
    assert "BuildObjc3StringLiteralExpr" in primary_parse
    assert "TryDecodeObjc3StringTokenText" in primary_parse
    assert "TryCountObjc3Utf8Scalars(value, unit_count)" in primary_parse
    assert 'type_token.text == "Text"' in param_type_parse
    assert "param.type = ValueType::TextHandle" in param_type_parse
    assert 'type_token.text == "Text"' in return_type_parse
    assert "fn.return_type = ValueType::TextHandle" in return_type_parse

    assert "StringLiteral" in expr_kind
    assert "std::string string_literal_value" in expr_members
    assert "string_literal_byte_count" in expr_members
    assert "string_literal_unit_count" in expr_members
    assert "literal->kind = Expr::Kind::StringLiteral" in literal_builders

    assert "ValueType::TextHandle" in sema_literals
    assert 'return "Text";' in type_queries
    assert "type == ValueType::TextHandle" in type_queries

    assert "objc3_runtime_stdlib_text_utf8_literal_i32" in ir_expr
    assert "string_literal_byte_count" in ir_expr
    assert "string_literal_unit_count" in ir_expr
    assert "Objc3IRRequiresTextLiteralHelperDeclarations" in ir_decls
    assert "kObjc3RuntimeStdlibTextUtf8LiteralI32Symbol" in ir_decls
    assert "(i32, i32, i32)\\n" in ir_decls


def test_source_string_literal_claim_has_truthful_evidence_rows() -> None:
    matrix = _read_json("docs/support/capability_matrix.json")
    manifest = _read_json("tests/fixtures/canonical/manifest.json")
    catalog = _read_json("tests/conformance/support_claim_runnable_evidence_catalog.json")

    matrix_rows = {row["id"]: row for row in matrix["capabilities"]}
    manifest_claims = {row["claim_id"]: row for row in manifest["support_claims"]}
    catalog_rows = {row["support_claim"]: row for row in catalog["rows"]}

    matrix_row = matrix_rows[CAPABILITY_ID]
    manifest_claim = manifest_claims[SUPPORT_CLAIM]
    catalog_row = catalog_rows[SUPPORT_CLAIM]

    assert matrix_row["state"] == "implemented"
    assert matrix_row["support_claims"] == [SUPPORT_CLAIM]
    assert manifest_claim["owner_phase"] == "lowering"
    assert manifest_claim["behavior_fixture"] == POSITIVE_FIXTURE
    assert manifest_claim["executable_command"] == PUBLIC_COMMAND

    assert catalog_row["capability_id"] == CAPABILITY_ID
    assert catalog_row["owner_phase"] == "lowering"
    assert catalog_row["conformance_fixture"] == POSITIVE_FIXTURE
    assert catalog_row["traceability_fixture"] == INVALID_ESCAPE_FIXTURE
    assert catalog_row["runnable_command"] == PUBLIC_COMMAND
    assert catalog_row["runtime_acceptance_case"] == (
        "source-string-literal-text-shape-handle"
    )
    assert POSITIVE_FIXTURE in catalog_row["positive_evidence"]
    assert INVALID_ESCAPE_FIXTURE in catalog_row["negative_evidence"]
    assert INTERPOLATION_FIXTURE in catalog_row["negative_evidence"]
    assert {"O3L010", "O3L011"}.issubset(
        set(catalog_row["required_diagnostic_codes"])
    )

    requirement_text = " ".join(catalog_row["source_truth_requirements"]).lower()
    for reserved in [
        "content retrieval",
        "interpolation",
        "formatting",
        "normalization",
        "NSString".lower(),
    ]:
        assert reserved in requirement_text

    for path in [
        POSITIVE_FIXTURE,
        INVALID_ESCAPE_FIXTURE,
        INTERPOLATION_FIXTURE,
        *catalog_row["positive_evidence"],
        *catalog_row["negative_evidence"],
    ]:
        assert not path.startswith(("tmp/", "tmp\\"))
        assert (ROOT / path).is_file(), path
