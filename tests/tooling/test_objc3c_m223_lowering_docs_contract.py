from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_AGGREGATE = ROOT / "docs" / "objc3c-native.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m223_lowering_docs_capture_frontend_ir_metadata_envelope() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    aggregate = _read(DOC_AGGREGATE)

    for text in (
        "## M223 lowering/IR metadata envelope",
        "frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "i64 <legacy_total>",
        "tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata/module.manifest.json",
    ):
        assert text in fragment
        assert text in aggregate


def test_m223_lowering_docs_contract_matches_emitter_and_manifest_surfaces() -> None:
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    assert 'out << "; frontend_profile = language_version="' in emitter_source
    assert 'out << "!objc3.frontend = !{!0}\\n";' in emitter_source
    assert "Objc3IRFrontendMetadata ir_frontend_metadata;" in artifacts_source
    assert "ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;" in artifacts_source
    assert "ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;" in artifacts_source
