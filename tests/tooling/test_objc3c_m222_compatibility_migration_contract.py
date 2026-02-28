from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

CLI_OPTIONS_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
CLI_OPTIONS_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
FRONTEND_OPTIONS_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_frontend_options.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
FRONTEND_PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_TYPES_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
CLI_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md"
DIAGNOSTICS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "40-diagnostics.md"
CLI_DOC_AGGREGATE = ROOT / "docs" / "objc3c-native.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_m222_driver_cli_supports_language_version_and_migration_controls() -> None:
    header = _read(CLI_OPTIONS_HEADER)
    source = _read(CLI_OPTIONS_SOURCE)

    assert "enum class Objc3CompatMode" in header
    assert "std::uint32_t language_version = 3;" in header
    assert "Objc3CompatMode compat_mode = Objc3CompatMode::kCanonical;" in header
    assert "bool migration_assist = false;" in header

    assert "[-fobjc-version=<N>] [--objc3-language-version <N>]" in source
    assert "[--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist]" in source
    assert "if (flag.rfind(\"-fobjc-version=\", 0) == 0)" in source
    assert "flag == \"-fobjc-version\" || flag == \"--objc3-language-version\"" in source
    assert "flag == \"--objc3-compat-mode\"" in source
    assert "flag == \"--objc3-migration-assist\"" in source
    assert "unsupported Objective-C language version for native frontend (expected 3): " in source

    frontend_options_source = _read(FRONTEND_OPTIONS_SOURCE)
    assert "options.language_version = static_cast<std::uint8_t>(cli_options.language_version);" in frontend_options_source
    assert "options.compatibility_mode = cli_options.compat_mode == Objc3CompatMode::kLegacy" in frontend_options_source
    assert "? Objc3FrontendCompatibilityMode::kLegacy" in frontend_options_source
    assert ": Objc3FrontendCompatibilityMode::kCanonical;" in frontend_options_source
    assert "options.migration_assist = cli_options.migration_assist;" in frontend_options_source
    _assert_in_order(
        frontend_options_source,
        [
            "options.language_version = static_cast<std::uint8_t>(cli_options.language_version);",
            "options.compatibility_mode = cli_options.compat_mode == Objc3CompatMode::kLegacy",
            "? Objc3FrontendCompatibilityMode::kLegacy",
            ": Objc3FrontendCompatibilityMode::kCanonical;",
            "options.migration_assist = cli_options.migration_assist;",
            "options.lowering.max_message_send_args = cli_options.max_message_send_args;",
        ],
    )

    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    assert "CompatibilityModeName(Objc3FrontendCompatibilityMode mode)" in frontend_artifacts_source
    assert "case Objc3FrontendCompatibilityMode::kLegacy:" in frontend_artifacts_source
    assert 'return "legacy";' in frontend_artifacts_source
    assert 'return "canonical";' in frontend_artifacts_source
    assert "\\\"compatibility_mode\\\":\\\"" in frontend_artifacts_source
    assert "\\\"migration_assist\\\":" in frontend_artifacts_source
    assert "\\\"language_version_pragma_contract\\\"" in frontend_artifacts_source
    assert "\\\"directive_count\\\":" in frontend_artifacts_source
    assert "\\\"duplicate\\\":" in frontend_artifacts_source
    assert "\\\"non_leading\\\":" in frontend_artifacts_source
    assert "Objc3IRFrontendMetadata ir_frontend_metadata;" in frontend_artifacts_source
    assert "ir_frontend_metadata.language_version = options.language_version;" in frontend_artifacts_source
    assert "ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);" in frontend_artifacts_source
    assert "ir_frontend_metadata.migration_assist = options.migration_assist;" in frontend_artifacts_source
    assert "ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;" in frontend_artifacts_source
    assert "ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;" in frontend_artifacts_source
    assert "ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;" in frontend_artifacts_source
    _assert_in_order(
        frontend_artifacts_source,
        [
            'manifest << "  \\"frontend\\": {\\n";',
            'manifest << "    \\"language_version\\":"',
            'manifest << "    \\"compatibility_mode\\":\\""',
            'manifest << "    \\"migration_assist\\":"',
            'manifest << "    \\"language_version_pragma_contract\\":',
            'manifest << "    \\"max_message_send_args\\":"',
            "Objc3IRFrontendMetadata ir_frontend_metadata;",
            "EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)",
        ],
    )

    frontend_types_header = _read(FRONTEND_TYPES_HEADER)
    assert "struct Objc3FrontendLanguageVersionPragmaContract {" in frontend_types_header
    assert "Objc3FrontendLanguageVersionPragmaContract language_version_pragma_contract;" in frontend_types_header

    frontend_pipeline_source = _read(FRONTEND_PIPELINE_SOURCE)
    assert "const Objc3LexerLanguageVersionPragmaContract &pragma_contract = lexer.LanguageVersionPragmaContract();" in frontend_pipeline_source
    assert "result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;" in frontend_pipeline_source
    assert "result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;" in frontend_pipeline_source


def test_m222_sema_pipeline_emits_deterministic_migration_diagnostics_contract() -> None:
    pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    assert "enum class Objc3SemaCompatibilityMode" in pass_manager_contract
    assert "struct Objc3SemaMigrationHints" in pass_manager_contract
    assert "Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;" in pass_manager_contract
    assert "bool migration_assist = false;" in pass_manager_contract
    assert "Objc3SemaMigrationHints migration_hints;" in pass_manager_contract

    pipeline_source = _read(FRONTEND_PIPELINE_SOURCE)
    assert "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy" in pipeline_source
    assert "? Objc3SemaCompatibilityMode::Legacy" in pipeline_source
    assert ": Objc3SemaCompatibilityMode::Canonical;" in pipeline_source
    assert "sema_input.migration_assist = options.migration_assist;" in pipeline_source
    assert "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;" in pipeline_source
    assert "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;" in pipeline_source
    assert "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;" in pipeline_source
    _assert_in_order(
        pipeline_source,
        [
            "sema_input.validation_options = semantic_options;",
            "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy",
            "? Objc3SemaCompatibilityMode::Legacy",
            ": Objc3SemaCompatibilityMode::Canonical;",
            "sema_input.migration_assist = options.migration_assist;",
            "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;",
            "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;",
            "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;",
            "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        ],
    )

    pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    assert "AppendMigrationAssistDiagnostics(const Objc3SemaPassManagerInput &input, std::vector<std::string> &diagnostics)" in pass_manager_source
    assert "O3S216" in pass_manager_source
    assert "AppendMigrationAssistDiagnostics(input, pass_diagnostics);" in pass_manager_source
    _assert_in_order(
        pass_manager_source,
        [
            "append_for_literal(input.migration_hints.legacy_yes_count, 1u, \"YES\", \"true\");",
            "append_for_literal(input.migration_hints.legacy_no_count, 2u, \"NO\", \"false\");",
            "append_for_literal(input.migration_hints.legacy_null_count, 3u, \"NULL\", \"nil\");",
        ],
    )
    _assert_in_order(
        pass_manager_source,
        [
            "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
            "AppendMigrationAssistDiagnostics(input, pass_diagnostics);",
            "CanonicalizePassDiagnostics(pass_diagnostics);",
            "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
        ],
    )


def test_m222_docs_publish_new_compatibility_controls() -> None:
    cli_fragment = _read(CLI_DOC_FRAGMENT)
    diagnostics_fragment = _read(DIAGNOSTICS_DOC_FRAGMENT)
    aggregate_doc = _read(CLI_DOC_AGGREGATE)

    for text in (
        "[-fobjc-version=<N>] [--objc3-language-version <N>]",
        "[--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist]",
        "[--clang <path>] [--llc <path>] [--summary-out <path>]",
        "[--objc3-ir-object-backend <clang|llvm-direct>]",
        "Default language version: `3`",
        "Default `--objc3-compat-mode`: `canonical`",
        "Default `--objc3-migration-assist`: `off`",
        "Default `--objc3-ir-object-backend`: `clang`",
        "compatibility_mode",
        "migration_assist",
    ):
        assert text in cli_fragment
        assert text in aggregate_doc

    for text in (
        "## O3S201..O3S216 behavior (implemented now)",
        "- `O3S216`:",
        "migration assist requires canonical literal replacement for legacy Objective-C aliases (`YES`, `NO`, `NULL`) in canonical compatibility mode.",
    ):
        assert text in diagnostics_fragment
        assert text in aggregate_doc


def test_m222_ci_gate_integration_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"test:objc3c:m222-compatibility-migration"' in package_json
    assert "tests/tooling/test_objc3c_m222_compatibility_migration_contract.py" in package_json
    assert "tests/tooling/test_objc3c_ir_emitter_extraction.py" in package_json

    assert "Run M222 compatibility+migration contract gate" in workflow
    assert "npm run test:objc3c:m222-compatibility-migration" in workflow
