from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m201_lowering_macro_expansion_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M201 lowering/runtime macro expansion architecture and isolation",
        "tmp/artifacts/compilation/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/",
        "tmp/reports/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/",
        "migration_hints_.legacy_yes_count",
        "migration_hints_.legacy_no_count",
        "migration_hints_.legacy_null_count",
        "language_version_pragma_contract_.directive_count",
        'append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");',
        'append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");',
        'append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");',
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "runtime_dispatch_symbol",
        "runtime_dispatch_arg_slots",
        "selector_global_ordering",
        "python -m pytest tests/tooling/test_objc3c_m201_lowering_macro_expansion_contract.py -q",
    ):
        assert text in fragment


def test_m201_lowering_macro_expansion_markers_map_to_sources() -> None:
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    sema_source = _read(SEMA_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_source = _read(IR_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)

    for marker in (
        "++migration_hints_.legacy_yes_count;",
        "++migration_hints_.legacy_no_count;",
        "++migration_hints_.legacy_null_count;",
        "++language_version_pragma_contract_.directive_count;",
    ):
        assert marker in lexer_source

    for marker in (
        "result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;",
        "result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;",
        "result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;",
        "result.language_version_pragma_contract.seen = pragma_contract.seen;",
        "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;",
        "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;",
        "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;",
    ):
        assert marker in pipeline_source

    for marker in (
        'append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");',
        'append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");',
        'append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");',
    ):
        assert marker in sema_source

    for marker in (
        'manifest << "    \\"migration_hints\\":{\\"legacy_yes\\":" << pipeline_result.migration_hints.legacy_yes_count',
        'manifest << "    \\"language_version_pragma_contract\\":{\\"seen\\":"',
        "ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;",
        "ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;",
        "ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;",
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
    ):
        assert marker in artifacts_source

    assert 'out << "; frontend_profile = language_version="' in ir_source
    assert 'out << "!objc3.frontend = !{!0}\\n";' in ir_source
    assert "Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)" in lowering_contract_source
    assert "return \"runtime_dispatch_symbol=\" + boundary.runtime_dispatch_symbol +" in lowering_contract_source
