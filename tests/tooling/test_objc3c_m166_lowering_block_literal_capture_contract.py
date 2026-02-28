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


def test_m166_lowering_block_literal_capture_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Block literal capture lowering artifact contract (M166-C001)",
        "kObjc3BlockLiteralCaptureLoweringLaneContract",
        "Objc3BlockLiteralCaptureLoweringContract",
        "IsValidObjc3BlockLiteralCaptureLoweringContract(...)",
        "Objc3BlockLiteralCaptureLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_block_literal_sites",
        "frontend.pipeline.semantic_surface.objc_block_literal_capture_lowering_surface",
        "lowering_block_literal_capture.replay_key",
        "block_literal_capture_lowering = block_literal_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m166_lowering_block_literal_capture_contract.py -q",
    ):
        assert text in fragment


def test_m166_lowering_block_literal_capture_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3BlockLiteralCaptureLoweringLaneContract",
        "struct Objc3BlockLiteralCaptureLoweringContract",
        "std::size_t block_literal_sites = 0;",
        "std::size_t block_parameter_entries = 0;",
        "std::size_t block_capture_entries = 0;",
        "std::size_t block_body_statement_entries = 0;",
        "std::size_t block_empty_capture_sites = 0;",
        "std::size_t block_nondeterministic_capture_sites = 0;",
        "std::size_t block_non_normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3BlockLiteralCaptureLoweringContract(",
        "Objc3BlockLiteralCaptureLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3BlockLiteralCaptureLoweringContract(",
        "Objc3BlockLiteralCaptureLoweringReplayKey(",
        '"block_literal_sites="',
        '";block_parameter_entries="',
        '";block_capture_entries="',
        '";block_body_statement_entries="',
        '";block_empty_capture_sites="',
        '";block_nondeterministic_capture_sites="',
        '";block_non_normalized_sites="',
        '";contract_violation_sites="',
        '";lane_contract=" + kObjc3BlockLiteralCaptureLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildBlockLiteralCaptureLoweringContract(",
        "IsValidObjc3BlockLiteralCaptureLoweringContract(",
        "block_literal_capture_lowering_replay_key",
        '\\"deterministic_block_literal_capture_lowering_handoff\\":',
        '\\"objc_block_literal_capture_lowering_surface\\":{\\"block_literal_sites\\":',
        '\\"lowering_block_literal_capture\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_block_literal_capture_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_block_literal_capture_replay_key;",
        "std::size_t block_literal_capture_lowering_block_literal_sites = 0;",
        "std::size_t block_literal_capture_lowering_block_parameter_entries = 0;",
        "std::size_t block_literal_capture_lowering_block_capture_entries = 0;",
        "std::size_t block_literal_capture_lowering_block_body_statement_entries = 0;",
        "std::size_t block_literal_capture_lowering_block_empty_capture_sites = 0;",
        "std::size_t block_literal_capture_lowering_block_nondeterministic_capture_sites = 0;",
        "std::size_t block_literal_capture_lowering_block_non_normalized_sites = 0;",
        "std::size_t block_literal_capture_lowering_contract_violation_sites = 0;",
        "bool deterministic_block_literal_capture_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; block_literal_capture_lowering = "',
        "frontend_objc_block_literal_capture_lowering_profile",
        "!objc3.objc_block_literal_capture_lowering = !{!19}",
        "!19 = !{i64 ",
    ):
        assert marker in ir_source
