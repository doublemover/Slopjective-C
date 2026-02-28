from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMANTIC_PASSES_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m198_sema_swift_metadata_bridge_section_marker_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    assert "## M198 sema/type swift metadata bridge" in fragment


def test_m198_sema_swift_metadata_bridge_source_anchor_mapping() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "swift metadata packet 1.1 deterministic sema/type bridge architecture anchors",
        "m198_sema_type_swift_metadata_architecture_packet",
        "static Objc3SemaTokenMetadata MakeSemaTokenMetadata(Objc3SemaTokenKind kind, const Token &token) {",
        "return MakeObjc3SemaTokenMetadata(kind, token.text, token.line, token.column);",
        "MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));",
        "MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));",
        "std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;",
        "std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;",
        "std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;",
        "std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;",
        "ValidateReturnTypeSuffixes(fn, diagnostics);",
        "ValidateParameterTypeSuffixes(fn, diagnostics);",
        "for (const auto &token : param.pointer_declarator_tokens) {",
        "for (const auto &token : param.nullability_suffix_tokens) {",
        "for (const auto &token : fn.return_pointer_declarator_tokens) {",
        "for (const auto &token : fn.return_nullability_suffix_tokens) {",
        "metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;",
        "metadata.param_has_invalid_type_suffix.size() == metadata.arity;",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "swift metadata packet 1.2 deterministic sema/type bridge isolation anchors",
        "m198_sema_type_swift_metadata_isolation_packet",
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "sema_input.program = &result.program;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        "result.sema_parity_surface = sema_result.parity_surface;",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "python -m pytest tests/tooling/test_objc3c_m198_sema_swift_metadata_bridge_contract.py -q",
    ):
        assert text in fragment

    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)
    semantic_passes_source = _read(SEMANTIC_PASSES_SOURCE)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "static Objc3SemaTokenMetadata MakeSemaTokenMetadata(Objc3SemaTokenKind kind, const Token &token) {",
        "return MakeObjc3SemaTokenMetadata(kind, token.text, token.line, token.column);",
        "MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));",
        "MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));",
    ):
        assert marker in parser_source

    for marker in (
        "std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;",
        "std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;",
        "std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;",
        "std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;",
    ):
        assert marker in ast_source

    for marker in (
        "ValidateReturnTypeSuffixes(fn, diagnostics);",
        "ValidateParameterTypeSuffixes(fn, diagnostics);",
        "for (const auto &token : param.pointer_declarator_tokens) {",
        "for (const auto &token : param.nullability_suffix_tokens) {",
        "for (const auto &token : fn.return_pointer_declarator_tokens) {",
        "for (const auto &token : fn.return_nullability_suffix_tokens) {",
        "metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;",
        "metadata.param_has_invalid_type_suffix.size() == metadata.arity;",
        "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {",
        "bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {",
    ):
        assert marker in semantic_passes_source

    for marker in (
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "Objc3SemaDiagnosticsBus diagnostics_bus;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "result.parity_surface.ready =",
    ):
        assert marker in sema_pass_manager_source

    for marker in (
        "sema_input.program = &result.program;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        "result.sema_parity_surface = sema_result.parity_surface;",
    ):
        assert marker in pipeline_source

    for marker in (
        '\\"deterministic_type_metadata_handoff\\":',
        '\\"parity_ready\\":',
        '\\"type_metadata_global_entries\\":',
        '\\"type_metadata_function_entries\\":',
    ):
        assert marker in artifacts_source
