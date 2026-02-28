from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m168_lowering_block_storage_escape_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Block storage escape lowering artifact contract (M168-C001)",
        "kObjc3BlockStorageEscapeLoweringLaneContract",
        "Objc3BlockStorageEscapeLoweringContract",
        "IsValidObjc3BlockStorageEscapeLoweringContract(...)",
        "Objc3BlockStorageEscapeLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_sites",
        "frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface",
        "lowering_block_storage_escape.replay_key",
        "block_storage_escape_lowering = block_literal_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m168_lowering_block_storage_escape_contract.py -q",
    ):
        assert text in fragment


def test_m168_lowering_block_storage_escape_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3BlockStorageEscapeLoweringLaneContract",
        "struct Objc3BlockStorageEscapeLoweringContract",
        "std::size_t mutable_capture_count_total = 0;",
        "std::size_t byref_slot_count_total = 0;",
        "std::size_t requires_byref_cells_sites = 0;",
        "std::size_t escape_analysis_enabled_sites = 0;",
        "std::size_t escape_to_heap_sites = 0;",
        "std::size_t escape_profile_normalized_sites = 0;",
        "std::size_t byref_layout_symbolized_sites = 0;",
        "IsValidObjc3BlockStorageEscapeLoweringContract(",
        "Objc3BlockStorageEscapeLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3BlockStorageEscapeLoweringContract(",
        "Objc3BlockStorageEscapeLoweringReplayKey(",
        '";mutable_capture_count_total="',
        '";byref_slot_count_total="',
        '";requires_byref_cells_sites="',
        '";escape_analysis_enabled_sites="',
        '";escape_to_heap_sites="',
        '";escape_profile_normalized_sites="',
        '";byref_layout_symbolized_sites="',
        '";lane_contract=" + kObjc3BlockStorageEscapeLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildBlockStorageEscapeLoweringContract(",
        "IsValidObjc3BlockStorageEscapeLoweringContract(",
        "block_storage_escape_lowering_replay_key",
        '\\"deterministic_block_storage_escape_lowering_handoff\\":',
        '\\"objc_block_storage_escape_lowering_surface\\":{\\"block_literal_sites\\":',
        '\\"lowering_block_storage_escape\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_block_storage_escape_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_block_storage_escape_replay_key;",
        "std::size_t block_storage_escape_lowering_block_literal_sites = 0;",
        "std::size_t block_storage_escape_lowering_mutable_capture_count_total = 0;",
        "std::size_t block_storage_escape_lowering_byref_slot_count_total = 0;",
        "std::size_t block_storage_escape_lowering_parameter_entries_total = 0;",
        "std::size_t block_storage_escape_lowering_capture_entries_total = 0;",
        "std::size_t block_storage_escape_lowering_body_statement_entries_total = 0;",
        "std::size_t block_storage_escape_lowering_requires_byref_cells_sites = 0;",
        "std::size_t block_storage_escape_lowering_escape_analysis_enabled_sites = 0;",
        "std::size_t block_storage_escape_lowering_escape_to_heap_sites = 0;",
        "std::size_t block_storage_escape_lowering_escape_profile_normalized_sites = 0;",
        "std::size_t block_storage_escape_lowering_byref_layout_symbolized_sites = 0;",
        "std::size_t block_storage_escape_lowering_contract_violation_sites = 0;",
        "bool deterministic_block_storage_escape_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; block_storage_escape_lowering = "',
        "frontend_objc_block_storage_escape_lowering_profile",
        "!objc3.objc_block_storage_escape_lowering = !{!21}",
        "!21 = !{i64 ",
    ):
        assert marker in ir_source
