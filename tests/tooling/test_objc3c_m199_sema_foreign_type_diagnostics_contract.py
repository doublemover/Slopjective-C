from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m199_sema_foreign_type_import_diagnostics_section_marker_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    assert "## M199 sema/type foreign type import diagnostics" in fragment


def test_m199_sema_foreign_type_import_diagnostics_source_anchor_phrases_mapped_in_doc_content() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "foreign import packet 1.1 deterministic sema/type foreign-type architecture anchors",
        "m199_sema_type_foreign_import_architecture_packet",
        "foreign import packet 1.2 deterministic sema foreign-type diagnostic isolation anchors",
        "m199_sema_foreign_import_diagnostic_isolation_packet",
        "static bool SupportsGenericParamTypeSuffix(const FuncParam &param) {",
        "return param.id_spelling || param.class_spelling || param.instancetype_spelling;",
        "static bool SupportsGenericReturnTypeSuffix(const FunctionDecl &fn) {",
        "return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;",
        "HasInvalidParamTypeSuffix(param)",
        "info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));",
        "metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;",
        "metadata.param_has_invalid_type_suffix.size() == metadata.arity;",
        "ValidateReturnTypeSuffixes(fn, diagnostics);",
        "ValidateParameterTypeSuffixes(fn, diagnostics);",
        "\"type mismatch: generic parameter type suffix '\" + suffix +",
        "\"type mismatch: pointer parameter type declarator '\" + token.text +",
        "\"type mismatch: nullability parameter type suffix '\" + token.text +",
        "\"type mismatch: unsupported function return type suffix '\" + suffix +",
        "\"type mismatch: unsupported function return type declarator '\" + token.text +",
        "\"O3S206\"",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "python -m pytest tests/tooling/test_objc3c_m199_sema_foreign_type_diagnostics_contract.py -q",
    ):
        assert text in fragment
