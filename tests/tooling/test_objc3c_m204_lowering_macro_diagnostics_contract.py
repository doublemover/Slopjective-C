from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
FRONTEND_PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DIAGNOSTICS_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m204_lowering_runtime_macro_diagnostics_and_provenance_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M204 lowering/runtime macro diagnostics and provenance",
        "tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/",
        "tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/",
        "tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.diagnostics.json",
        "tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/macro-diagnostics-provenance-source-anchors.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "MakeDiag(...)",
        "error:<line>:<column>: <message> [<code>]",
        "ConsumeLanguageVersionPragmas(diagnostics)",
        "ConsumeLanguageVersionPragmaDirective(...)",
        "O3L005",
        "O3L006",
        "O3L007",
        "O3L008",
        "first_line",
        "first_column",
        "last_line",
        "last_column",
        "ParseDiagSortKey(...)",
        '"severity"',
        '"line"',
        '"column"',
        '"code"',
        '"message"',
        '"raw"',
        "Objc3LoweringIRBoundaryReplayKey(...)",
        'rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\\"lowering\\":{\\"runtime_dispatch_symbol\\"" tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.ll tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.manifest.json > tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/abi-ir-anchors.txt',
        'rg -n "MakeDiag\\(|error:|ConsumeLanguageVersionPragmas\\(diagnostics\\)|ConsumeLanguageVersionPragmaDirective\\(|O3L005|O3L006|O3L007|O3L008|first_line|first_column|last_line|last_column|ParseDiagSortKey\\(|\\"severity\\":|\\"line\\":|\\"column\\":|\\"code\\":|\\"message\\":|\\"raw\\":|Objc3LoweringIRBoundaryReplayKey\\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/lex/objc3_lexer.cpp native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/io/objc3_diagnostics_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/macro-diagnostics-provenance-source-anchors.txt',
        "python -m pytest tests/tooling/test_objc3c_m204_lowering_macro_diagnostics_contract.py -q",
    ):
        assert text in fragment


def test_m204_lowering_runtime_macro_diagnostics_source_anchors_align_to_real_surfaces() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(FRONTEND_PIPELINE_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    diagnostics_artifacts_source = _read(DIAGNOSTICS_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    emitter_source = _read(IR_EMITTER_SOURCE)

    mapped_source_anchors = (
        (
            'out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";',
            lexer_source,
            'out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";',
        ),
        ("ConsumeLanguageVersionPragmas(diagnostics);", lexer_source, "ConsumeLanguageVersionPragmas(diagnostics);"),
        (
            "ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))",
            lexer_source,
            "ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))",
        ),
        (
            'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));',
            lexer_source,
            'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));',
        ),
        (
            'diagnostics.push_back(MakeDiag(version_line, version_column, "O3L006",',
            lexer_source,
            'diagnostics.push_back(MakeDiag(version_line, version_column, "O3L006",',
        ),
        (
            'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));',
            lexer_source,
            'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));',
        ),
        (
            'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));',
            lexer_source,
            'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));',
        ),
        (
            "language_version_pragma_contract_.first_line = line;",
            lexer_source,
            "language_version_pragma_contract_.first_line = line;",
        ),
        (
            "language_version_pragma_contract_.first_column = column;",
            lexer_source,
            "language_version_pragma_contract_.first_column = column;",
        ),
        (
            "language_version_pragma_contract_.last_line = line;",
            lexer_source,
            "language_version_pragma_contract_.last_line = line;",
        ),
        (
            "language_version_pragma_contract_.last_column = column;",
            lexer_source,
            "language_version_pragma_contract_.last_column = column;",
        ),
        (
            "result.language_version_pragma_contract.first_line = pragma_contract.first_line;",
            pipeline_source,
            "result.language_version_pragma_contract.first_line = pragma_contract.first_line;",
        ),
        (
            "result.language_version_pragma_contract.first_column = pragma_contract.first_column;",
            pipeline_source,
            "result.language_version_pragma_contract.first_column = pragma_contract.first_column;",
        ),
        (
            "result.language_version_pragma_contract.last_line = pragma_contract.last_line;",
            pipeline_source,
            "result.language_version_pragma_contract.last_line = pragma_contract.last_line;",
        ),
        (
            "result.language_version_pragma_contract.last_column = pragma_contract.last_column;",
            pipeline_source,
            "result.language_version_pragma_contract.last_column = pragma_contract.last_column;",
        ),
        (
            'manifest << "    \\"language_version_pragma_contract\\":{\\"seen\\":"',
            artifacts_source,
            'manifest << "    \\"language_version_pragma_contract\\":{\\"seen\\":"',
        ),
        (
            '<< ",\\"first_line\\":" << pipeline_result.language_version_pragma_contract.first_line',
            artifacts_source,
            '<< ",\\"first_line\\":" << pipeline_result.language_version_pragma_contract.first_line',
        ),
        (
            '<< ",\\"first_column\\":" << pipeline_result.language_version_pragma_contract.first_column',
            artifacts_source,
            '<< ",\\"first_column\\":" << pipeline_result.language_version_pragma_contract.first_column',
        ),
        (
            '<< ",\\"last_line\\":" << pipeline_result.language_version_pragma_contract.last_line',
            artifacts_source,
            '<< ",\\"last_line\\":" << pipeline_result.language_version_pragma_contract.last_line',
        ),
        (
            '<< ",\\"last_column\\":" << pipeline_result.language_version_pragma_contract.last_column << "},\\n";',
            artifacts_source,
            '<< ",\\"last_column\\":" << pipeline_result.language_version_pragma_contract.last_column << "},\\n";',
        ),
        (
            "const DiagSortKey key = ParseDiagSortKey(diagnostics[i]);",
            diagnostics_artifacts_source,
            "const DiagSortKey key = ParseDiagSortKey(diagnostics[i]);",
        ),
        (
            'out << "    {\\"severity\\":\\"" << EscapeJsonString(ToLower(key.severity)) << "\\",\\"line\\":" << line',
            diagnostics_artifacts_source,
            'out << "    {\\"severity\\":\\"" << EscapeJsonString(ToLower(key.severity)) << "\\",\\"line\\":" << line',
        ),
        (
            '<< ",\\"column\\":" << column << ",\\"code\\":\\"" << EscapeJsonString(key.code) << "\\",\\"message\\":\\""',
            diagnostics_artifacts_source,
            '<< ",\\"column\\":" << column << ",\\"code\\":\\"" << EscapeJsonString(key.code) << "\\",\\"message\\":\\""',
        ),
        (
            '<< EscapeJsonString(key.message) << "\\",\\"raw\\":\\"" << EscapeJsonString(diagnostics[i]) << "\\"}";',
            diagnostics_artifacts_source,
            '<< EscapeJsonString(key.message) << "\\",\\"raw\\":\\"" << EscapeJsonString(diagnostics[i]) << "\\"}";',
        ),
        (
            'WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());',
            diagnostics_artifacts_source,
            'WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());',
        ),
        ("Objc3LoweringIRBoundaryReplayKey(...)", lowering_contract_source, "Objc3LoweringIRBoundaryReplayKey("),
        (
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
            artifacts_source,
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        ),
        (
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
            artifacts_source,
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        ),
        (
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
            artifacts_source,
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
        ),
    )
    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text

    assert '"O3L005"' in lexer_source
    assert '"O3L006"' in lexer_source
    assert '"O3L007"' in lexer_source
    assert '"O3L008"' in lexer_source
    assert 'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";' in (
        emitter_source
    )
    assert 'out << "; frontend_profile = language_version="' in emitter_source
    assert 'out << "!objc3.frontend = !{!0}\\n";' in emitter_source
    assert 'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";' in emitter_source
