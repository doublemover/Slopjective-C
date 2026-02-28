from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m205_sema_macro_security_policy_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M205 sema/type macro security policy enforcement",
        "macro policy packet 1.1 deterministic migration-hint transport hooks",
        "m205_macro_policy_migration_transport_packet",
        "### 1.1 Deterministic migration-hint transport packet",
        "if (options_.migration_assist) {",
        "++migration_hints_.legacy_yes_count;",
        "++migration_hints_.legacy_no_count;",
        "++migration_hints_.legacy_null_count;",
        "result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;",
        "sema_input.migration_assist = options.migration_assist;",
        "bool migration_assist = false;",
        "if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {",
        "macro policy packet 1.2 deterministic canonical macro-policy/type-surface hooks",
        "m205_macro_policy_canonical_type_surface_packet",
        "### 1.2 Deterministic canonical macro-policy/type-surface packet",
        "AppendMigrationAssistDiagnostics(input, pass_diagnostics);",
        'append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");',
        'append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");',
        'append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");',
        '"O3S216"',
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "migration_assist",
        "migration_hints",
        "legacy_yes",
        "legacy_no",
        "legacy_null",
        "legacy_total",
        "deterministic_semantic_diagnostics",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "python -m pytest tests/tooling/test_objc3c_m205_sema_macro_security_contract.py -q",
    ):
        assert text in fragment


def test_m205_sema_macro_security_policy_markers_map_to_sources() -> None:
    lexer_source = _read(LEXER_SOURCE)
    sema_contract_header = _read(SEMA_CONTRACT_HEADER)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "if (options_.migration_assist) {",
        "++migration_hints_.legacy_yes_count;",
        "++migration_hints_.legacy_no_count;",
        "++migration_hints_.legacy_null_count;",
    ):
        assert marker in lexer_source

    for marker in (
        "bool migration_assist = false;",
        "Objc3SemaMigrationHints migration_hints;",
    ):
        assert marker in sema_contract_header

    for marker in (
        "if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {",
        'append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");',
        'append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");',
        'append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");',
        "AppendMigrationAssistDiagnostics(input, pass_diagnostics);",
        '"O3S216"',
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
    ):
        assert marker in sema_source

    for marker in (
        "result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;",
        "result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;",
        "result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;",
        "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;",
        "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;",
    ):
        assert marker in pipeline_source

    for marker in (
        '\\"migration_assist\\":',
        '\\"migration_hints\\":{\\"legacy_yes\\":',
        '\\"legacy_no\\":',
        '\\"legacy_null\\":',
        '\\"legacy_total\\":',
        '\\"deterministic_semantic_diagnostics\\":',
        '\\"deterministic_type_metadata_handoff\\":',
        '\\"parity_ready\\":',
    ):
        assert marker in artifacts_source
