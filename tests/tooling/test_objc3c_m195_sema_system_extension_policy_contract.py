from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m195_sema_system_extension_policy_section_marker_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M195 sema/type system-extension conformance and policy",
        "system-extension packet 1.1 deterministic sema/type conformance architecture anchors",
        "m195_sema_type_system_extension_conformance_architecture_packet",
        "### 1.1 Deterministic sema/type conformance architecture packet",
        "static bool ValidateSupportedLanguageVersion(uint8_t requested_language_version, std::string &error) {",
        "static bool ValidateSupportedCompatibilityMode(uint8_t requested_compatibility_mode, std::string &error) {",
        "if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {",
        "Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;",
        "bool migration_assist = false;",
        "Objc3SemaDiagnosticsBus diagnostics_bus;",
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {",
        "sema_input.validation_options = semantic_options;",
        "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "system-extension packet 1.2 deterministic sema/type policy isolation anchors",
        "m195_sema_type_system_extension_policy_isolation_packet",
        "### 1.2 Deterministic sema/type policy isolation packet",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "result.parity_surface.ready =",
        "diagnostics_after_build",
        "diagnostics_after_validate_bodies",
        "diagnostics_after_validate_pure_contract",
        "deterministic_semantic_diagnostics",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "function_signature_surface",
        "python -m pytest tests/tooling/test_objc3c_m195_sema_system_extension_policy_contract.py -q",
    ):
        assert text in fragment


def test_m195_sema_system_extension_policy_source_anchor_mapping() -> None:
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "static bool ValidateSupportedLanguageVersion(uint8_t requested_language_version, std::string &error) {",
        "static bool ValidateSupportedCompatibilityMode(uint8_t requested_compatibility_mode, std::string &error) {",
        "if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {",
    ):
        assert marker in frontend_anchor_source

    for marker in (
        "Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;",
        "bool migration_assist = false;",
        "Objc3SemaDiagnosticsBus diagnostics_bus;",
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {",
        "sema_input.validation_options = semantic_options;",
        "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
    ):
        assert marker in pipeline_source

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
        '\\"sema_pass_manager\\": {\\"diagnostics_after_build\\":',
        '\\"diagnostics_after_validate_bodies\\":',
        '\\"diagnostics_after_validate_pure_contract\\":',
        '\\"deterministic_semantic_diagnostics\\":',
        '\\"deterministic_type_metadata_handoff\\":',
        '\\"parity_ready\\":',
        '\\"resolved_global_symbols\\":',
        '\\"resolved_function_symbols\\":',
        '\\"function_signature_surface\\":',
    ):
        assert marker in artifacts_source
