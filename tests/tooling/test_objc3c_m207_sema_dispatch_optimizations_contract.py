from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMANTIC_PASSES_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m207_sema_dispatch_optimizations_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M207 sema/type dispatch-specific optimization passes",
        "dispatch optimization packet 1.1 deterministic sema pass-manager dispatch hooks",
        "m207_sema_pass_dispatch_optimization_packet",
        "### 1.1 Deterministic sema pass-manager dispatch optimization packet",
        "kObjc3SemaPassOrder",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "frontend.pipeline.sema_pass_manager",
        "diagnostics_after_build",
        "diagnostics_after_validate_bodies",
        "diagnostics_after_validate_pure_contract",
        "diagnostics_emitted_by_build",
        "diagnostics_emitted_by_validate_bodies",
        "diagnostics_emitted_by_validate_pure_contract",
        "dispatch optimization packet 1.2 deterministic message-send type/arity optimization hooks",
        "m207_message_send_type_arity_optimization_packet",
        "### 1.2 Deterministic message-send type/arity optimization packet",
        "Objc3SemanticValidationOptions",
        "std::size_t max_message_send_args = 4;",
        "semantic_options.max_message_send_args = options.lowering.max_message_send_args;",
        "static ValueType ValidateMessageSendExpr(",
        "if (receiver_type != ValueType::Unknown && !IsMessageI32CompatibleType(receiver_type)) {",
        "if (expr->args.size() > max_message_send_args) {",
        "0, 0, options.max_message_send_args);",
        "max_message_send_args",
        "runtime_dispatch_symbol",
        "runtime_dispatch_arg_slots",
        "python -m pytest tests/tooling/test_objc3c_m207_sema_dispatch_optimizations_contract.py -q",
    ):
        assert text in fragment


def test_m207_sema_dispatch_optimizations_markers_map_to_sources() -> None:
    sema_contract_header = _read(SEMA_CONTRACT_HEADER)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    semantic_passes_source = _read(SEMANTIC_PASSES_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "struct Objc3SemanticValidationOptions {" in sema_contract_header
    assert "std::size_t max_message_send_args = 4;" in sema_contract_header
    assert "kObjc3SemaPassOrder" in sema_pass_manager_contract

    for marker in (
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "if (pass == Objc3SemaPassId::BuildIntegrationSurface) {",
        "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
    ):
        assert marker in sema_pass_manager_source

    for marker in (
        "static ValueType ValidateMessageSendExpr(",
        "if (receiver_type != ValueType::Unknown && !IsMessageI32CompatibleType(receiver_type)) {",
        "if (expr->args.size() > max_message_send_args) {",
        "ValidateStatements(fn.body, scopes, surface.globals, surface.functions, fn.return_type, fn.name, diagnostics,",
        "0, 0, options.max_message_send_args);",
    ):
        assert marker in semantic_passes_source

    assert "semantic_options.max_message_send_args = options.lowering.max_message_send_args;" in pipeline_source
    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline_source

    for marker in (
        'manifest << "    \\"max_message_send_args\\":" << options.lowering.max_message_send_args << ",\\n";',
        'manifest << "      \\"sema_pass_manager\\": {\\"diagnostics_after_build\\":"',
        '\\"diagnostics_after_validate_bodies\\":',
        '\\"diagnostics_after_validate_pure_contract\\":',
        '\\"diagnostics_emitted_by_build\\":',
        '\\"diagnostics_emitted_by_validate_bodies\\":',
        '\\"diagnostics_emitted_by_validate_pure_contract\\":',
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
    ):
        assert marker in artifacts_source
