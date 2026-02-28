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


def test_m167_lowering_block_abi_invoke_trampoline_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Block ABI invoke-trampoline lowering artifact contract (M167-C001)",
        "kObjc3BlockAbiInvokeTrampolineLoweringLaneContract",
        "Objc3BlockAbiInvokeTrampolineLoweringContract",
        "IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(...)",
        "Objc3BlockAbiInvokeTrampolineLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_sites",
        "frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface",
        "lowering_block_abi_invoke_trampoline.replay_key",
        "block_abi_invoke_trampoline_lowering = block_literal_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m167_lowering_block_abi_invoke_trampoline_contract.py -q",
    ):
        assert text in fragment


def test_m167_lowering_block_abi_invoke_trampoline_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3BlockAbiInvokeTrampolineLoweringLaneContract",
        "struct Objc3BlockAbiInvokeTrampolineLoweringContract",
        "std::size_t invoke_argument_slots_total = 0;",
        "std::size_t capture_word_count_total = 0;",
        "std::size_t descriptor_symbolized_sites = 0;",
        "std::size_t invoke_trampoline_symbolized_sites = 0;",
        "std::size_t missing_invoke_trampoline_sites = 0;",
        "std::size_t non_normalized_layout_sites = 0;",
        "IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(",
        "Objc3BlockAbiInvokeTrampolineLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(",
        "Objc3BlockAbiInvokeTrampolineLoweringReplayKey(",
        '";invoke_argument_slots_total="',
        '";capture_word_count_total="',
        '";descriptor_symbolized_sites="',
        '";invoke_trampoline_symbolized_sites="',
        '";missing_invoke_trampoline_sites="',
        '";non_normalized_layout_sites="',
        '";lane_contract=" + kObjc3BlockAbiInvokeTrampolineLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildBlockAbiInvokeTrampolineLoweringContract(",
        "IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(",
        "block_abi_invoke_trampoline_lowering_replay_key",
        '\\"deterministic_block_abi_invoke_trampoline_lowering_handoff\\":',
        '\\"objc_block_abi_invoke_trampoline_lowering_surface\\":{\\"block_literal_sites\\":',
        '\\"lowering_block_abi_invoke_trampoline\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_block_abi_invoke_trampoline_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_block_abi_invoke_trampoline_replay_key;",
        "std::size_t block_abi_invoke_trampoline_lowering_block_literal_sites = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_invoke_argument_slots_total = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_capture_word_count_total = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_parameter_entries_total = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_capture_entries_total = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_body_statement_entries_total = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_invoke_symbolized_sites = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_missing_invoke_sites = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_non_normalized_layout_sites = 0;",
        "std::size_t block_abi_invoke_trampoline_lowering_contract_violation_sites = 0;",
        "bool deterministic_block_abi_invoke_trampoline_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; block_abi_invoke_trampoline_lowering = "',
        "frontend_objc_block_abi_invoke_trampoline_lowering_profile",
        "!objc3.objc_block_abi_invoke_trampoline_lowering = !{!20}",
        "!20 = !{i64 ",
    ):
        assert marker in ir_source
