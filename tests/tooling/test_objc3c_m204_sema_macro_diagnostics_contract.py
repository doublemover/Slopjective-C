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


def test_m204_sema_macro_diagnostics_and_provenance_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M204 sema/type macro diagnostics and provenance",
        "macro diagnostics packet 1.1 deterministic canonical sema diagnostics hooks",
        "m204_macro_diagnostics_packet",
        "### 1.1 Deterministic canonical sema diagnostics packet",
        "if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {",
        "AppendMigrationAssistDiagnostics(input, pass_diagnostics);",
        '"O3S216"',
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "if (options_.migration_assist) {",
        "++migration_hints_.legacy_yes_count;",
        "++migration_hints_.legacy_no_count;",
        "++migration_hints_.legacy_null_count;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "result.deterministic_semantic_diagnostics = deterministic_semantic_diagnostics;",
        "result.parity_surface.deterministic_semantic_diagnostics = result.deterministic_semantic_diagnostics;",
        "macro provenance packet 1.2 deterministic sema/type provenance hooks",
        "m204_macro_provenance_packet",
        "### 1.2 Deterministic sema/type provenance packet",
        "bool migration_assist = false;",
        "Objc3SemaMigrationHints migration_hints;",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;",
        "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;",
        "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
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
        "python -m pytest tests/tooling/test_objc3c_m204_sema_macro_diagnostics_contract.py -q",
    ):
        assert text in fragment


def test_m204_sema_macro_diagnostics_and_provenance_markers_map_to_source_anchors() -> None:
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
        "bool deterministic_semantic_diagnostics = false;",
        "bool deterministic_type_metadata_handoff = false;",
    ):
        assert marker in sema_contract_header

    for marker in (
        "if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {",
        "AppendMigrationAssistDiagnostics(input, pass_diagnostics);",
        '"O3S216"',
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "result.deterministic_semantic_diagnostics = deterministic_semantic_diagnostics;",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "result.parity_surface.deterministic_semantic_diagnostics = result.deterministic_semantic_diagnostics;",
        "result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;",
    ):
        assert marker in sema_source

    for marker in (
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;",
        "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;",
        "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
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
