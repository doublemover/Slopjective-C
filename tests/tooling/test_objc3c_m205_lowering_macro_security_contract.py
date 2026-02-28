from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
FRONTEND_PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m205_lowering_runtime_macro_security_policy_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M205 lowering/runtime macro security policy enforcement",
        "tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/",
        "tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/",
        "tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.diagnostics.json",
        "tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/macro-security-source-anchors.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "ConsumeLanguageVersionPragmas(diagnostics)",
        "ConsumeLanguageVersionPragmaDirective(...)",
        "LanguageVersionPragmaPlacement::kNonLeading",
        "O3L005",
        "O3L006",
        "O3L007",
        "O3L008",
        "frontend.language_version_pragma_contract",
        "directive_count",
        "duplicate",
        "non_leading",
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "runtime_dispatch_symbol",
        "runtime_dispatch_arg_slots",
        "selector_global_ordering",
        "ConsumeLanguageVersionPragmas(diagnostics);",
        "ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))",
        "if (placement == LanguageVersionPragmaPlacement::kNonLeading) {",
        'diagnostics.push_back(MakeDiag(version_line, version_column, "O3L006",',
        'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));',
        'diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));',
        "result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;",
        "result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;",
        "result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;",
        'manifest << "    \\"language_version_pragma_contract\\":{\\"seen\\":"',
        '<< ",\\"directive_count\\":" << pipeline_result.language_version_pragma_contract.directive_count',
        '<< ",\\"duplicate\\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")',
        '<< ",\\"non_leading\\":"',
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
        'rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\\"lowering\\":{\\"runtime_dispatch_symbol\\"" tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.ll tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.manifest.json > tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/abi-ir-anchors.txt',
        'rg -n "ConsumeLanguageVersionPragmas\\(diagnostics\\)|ConsumeLanguageVersionPragmaDirective\\(|LanguageVersionPragmaPlacement::kNonLeading|O3L005|O3L006|O3L007|O3L008|language_version_pragma_contract|directive_count|duplicate|non_leading|Objc3LoweringIRBoundaryReplayKey\\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/lex/objc3_lexer.cpp native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/macro-security-source-anchors.txt',
        "python -m pytest tests/tooling/test_objc3c_m205_lowering_macro_security_contract.py -q",
    ):
        assert text in fragment


def test_m205_lowering_runtime_macro_security_source_anchors_align_to_real_surfaces() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(FRONTEND_PIPELINE_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    emitter_source = _read(IR_EMITTER_SOURCE)

    mapped_source_anchors = (
        ("ConsumeLanguageVersionPragmas(diagnostics);", lexer_source, "ConsumeLanguageVersionPragmas(diagnostics);"),
        (
            "ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))",
            lexer_source,
            "ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))",
        ),
        (
            "if (placement == LanguageVersionPragmaPlacement::kNonLeading) {",
            lexer_source,
            "if (placement == LanguageVersionPragmaPlacement::kNonLeading) {",
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
            "result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;",
            pipeline_source,
            "result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;",
        ),
        (
            "result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;",
            pipeline_source,
            "result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;",
        ),
        (
            "result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;",
            pipeline_source,
            "result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;",
        ),
        (
            'manifest << "    \\"language_version_pragma_contract\\":{\\"seen\\":"',
            artifacts_source,
            'manifest << "    \\"language_version_pragma_contract\\":{\\"seen\\":"',
        ),
        (
            '<< ",\\"directive_count\\":" << pipeline_result.language_version_pragma_contract.directive_count',
            artifacts_source,
            '<< ",\\"directive_count\\":" << pipeline_result.language_version_pragma_contract.directive_count',
        ),
        (
            '<< ",\\"duplicate\\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")',
            artifacts_source,
            '<< ",\\"duplicate\\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")',
        ),
        (
            '<< ",\\"non_leading\\":"',
            artifacts_source,
            '<< ",\\"non_leading\\":"',
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
