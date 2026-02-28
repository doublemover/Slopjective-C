from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
OBJECTIVEC_PATH_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objectivec_path.cpp"
DIAG_UTILS_SOURCE = ROOT / "native" / "objc3c" / "src" / "diag" / "objc3_diag_utils.cpp"
DIAGNOSTICS_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m199_lowering_runtime_foreign_type_import_diagnostics_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M199 lowering/runtime foreign type import diagnostics",
        "tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/",
        "tmp/reports/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/",
        "tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/module.diagnostics.json",
        "tmp/reports/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/foreign-type-import-diagnostics-source-anchors.txt",
        "FormatDiagnostic(...)",
        "NormalizeDiagnostics(...)",
        "WriteDiagnosticsArtifacts(...)",
        "FlattenStageDiagnostics(...)",
        "ParseDiagSortKey(...)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "python -m pytest tests/tooling/test_objc3c_m199_lowering_foreign_type_diagnostics_contract.py -q",
    ):
        assert text in fragment


def test_m199_lowering_runtime_foreign_type_import_diagnostics_source_anchors_map_to_sources() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    objectivec_path_source = _read(OBJECTIVEC_PATH_SOURCE)
    diag_utils_source = _read(DIAG_UTILS_SOURCE)
    diagnostics_artifacts_source = _read(DIAGNOSTICS_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    mapped_source_anchors = (
        (
            "std::string FormatDiagnostic(CXDiagnostic diagnostic) {",
            objectivec_path_source,
            "std::string FormatDiagnostic(CXDiagnostic diagnostic) {",
        ),
        (
            'out << severity_text << ":" << line << ":" << column << ": " << ToString(clang_getDiagnosticSpelling(diagnostic));',
            objectivec_path_source,
            'out << severity_text << ":" << line << ":" << column << ": " << ToString(clang_getDiagnosticSpelling(diagnostic));',
        ),
        (
            "diagnostics.push_back(FormatDiagnostic(diagnostic));",
            objectivec_path_source,
            "diagnostics.push_back(FormatDiagnostic(diagnostic));",
        ),
        ("NormalizeDiagnostics(diagnostics);", objectivec_path_source, "NormalizeDiagnostics(diagnostics);"),
        (
            "WriteDiagnosticsArtifacts(cli_options.out_dir, cli_options.emit_prefix, diagnostics);",
            objectivec_path_source,
            "WriteDiagnosticsArtifacts(cli_options.out_dir, cli_options.emit_prefix, diagnostics);",
        ),
        (
            "DiagSortKey ParseDiagSortKey(const std::string &diag) {",
            diag_utils_source,
            "DiagSortKey ParseDiagSortKey(const std::string &diag) {",
        ),
        (
            "std::stable_sort(rows.begin(), rows.end(), [](const DiagSortKey &a, const DiagSortKey &b) {",
            diag_utils_source,
            "std::stable_sort(rows.begin(), rows.end(), [](const DiagSortKey &a, const DiagSortKey &b) {",
        ),
        (
            "if (diagnostics.empty() || diagnostics.back() != row.raw) {",
            diag_utils_source,
            "if (diagnostics.empty() || diagnostics.back() != row.raw) {",
        ),
        (
            "const std::vector<std::string> diagnostics = FlattenStageDiagnostics(stage_diagnostics, post_pipeline_diagnostics);",
            diagnostics_artifacts_source,
            "const std::vector<std::string> diagnostics = FlattenStageDiagnostics(stage_diagnostics, post_pipeline_diagnostics);",
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
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
            frontend_artifacts_source,
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        ),
        (
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
            frontend_artifacts_source,
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        ),
        (
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
            frontend_artifacts_source,
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
        ),
        (
            "Objc3LoweringIRBoundaryReplayKey(...)",
            lowering_contract_source,
            "Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)",
        ),
        (
            "invalid lowering contract runtime_dispatch_symbol",
            lowering_contract_source,
            "invalid lowering contract runtime_dispatch_symbol",
        ),
    )

    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text
