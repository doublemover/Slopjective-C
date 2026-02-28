from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

DIAGNOSTICS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "40-diagnostics.md"
DOC_AGGREGATE = ROOT / "docs" / "objc3c-native.md"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m223_sema_docs_capture_migration_operator_behavior() -> None:
    diagnostics_fragment = _read(DIAGNOSTICS_DOC_FRAGMENT)
    aggregate_doc = _read(DOC_AGGREGATE)

    for text in (
        "## M223 semantic migration operator guide",
        "compatibility_mode=canonical` + `migration_assist=false",
        "compatibility_mode=canonical` + `migration_assist=true",
        "compatibility_mode=legacy` + `migration_assist=true",
        "deterministic `O3S216` diagnostics",
        "frontend.migration_hints",
        "legacy aliases (`YES`->`true`, `NO`->`false`, `NULL`->`nil`)",
    ):
        assert text in diagnostics_fragment
        assert text in aggregate_doc


def test_m223_sema_docs_contract_matches_pipeline_and_sema_surfaces() -> None:
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    assert "AppendMigrationAssistDiagnostics(" in sema_source
    assert "input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical" in sema_source
    assert "O3S216" in sema_source

    assert "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy" in pipeline_source
    assert "sema_input.migration_assist = options.migration_assist;" in pipeline_source

    assert 'manifest << "    \\"migration_hints\\":{\\"legacy_yes\\":' in artifacts_source
    assert "pipeline_result.migration_hints.legacy_total()" in artifacts_source
