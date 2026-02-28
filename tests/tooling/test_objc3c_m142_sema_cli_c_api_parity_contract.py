from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DIAG_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp"
PIPELINE_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_m142_sema_cli_c_api_parity_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## Frontend sema/type CLI-C-API parity contract (M142-B001)",
        "native/objc3c/src/sema/objc3_sema_pass_manager_contract.h",
        "native/objc3c/src/sema/objc3_sema_pass_manager.cpp",
        "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
        "diagnostics_after_build",
        "diagnostics_after_validate_bodies",
        "deterministic_semantic_diagnostics",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "vector_type_lowering_total",
        "python -m pytest tests/tooling/test_objc3c_m142_sema_cli_c_api_parity_contract.py -q",
    ):
        assert text in fragment


def test_m142_sema_cli_c_api_parity_markers_map_to_sources() -> None:
    frontend_anchor = _read(FRONTEND_ANCHOR)
    diag_artifacts = _read(DIAG_ARTIFACTS)
    pipeline_artifacts = _read(PIPELINE_ARTIFACTS)

    for marker in (
        '\\"severity\\":\\"',
        '\\"line\\":',
        '\\"column\\":',
        '\\"code\\":\\"',
        '\\"message\\":\\"',
        '\\"raw\\":\\"',
    ):
        assert marker in frontend_anchor
        assert marker in diag_artifacts

    _assert_in_order(
        pipeline_artifacts,
        [
            'manifest << "  \\"frontend\\": {\\n";',
            'manifest << "    \\"language_version\\":"',
            'manifest << "    \\"compatibility_mode\\":\\""',
            'manifest << "    \\"migration_assist\\":"',
            'manifest << "    \\"max_message_send_args\\":"',
        ],
    )

    for marker in (
        "diagnostics_after_build",
        "diagnostics_after_validate_bodies",
        "diagnostics_after_validate_pure_contract",
        "diagnostics_emitted_by_build",
        "diagnostics_emitted_by_validate_bodies",
        "diagnostics_emitted_by_validate_pure_contract",
        "diagnostics_monotonic",
        "deterministic_semantic_diagnostics",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "deterministic_atomic_memory_order_mapping",
        "atomic_memory_order_mapping_total",
        "deterministic_vector_type_lowering",
        "vector_type_lowering_total",
    ):
        assert marker in pipeline_artifacts
