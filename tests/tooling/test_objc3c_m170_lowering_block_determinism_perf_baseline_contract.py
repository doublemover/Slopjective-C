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


def test_m170_lowering_block_determinism_perf_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Block determinism/perf baseline lowering artifact contract (M170-C001)",
        "kObjc3BlockDeterminismPerfBaselineLoweringLaneContract",
        "Objc3BlockDeterminismPerfBaselineLoweringContract",
        "IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(...)",
        "Objc3BlockDeterminismPerfBaselineLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_block_determinism_perf_baseline_handoff",
        "frontend.pipeline.semantic_surface.objc_block_determinism_perf_baseline_lowering_surface",
        "lowering_block_determinism_perf_baseline.replay_key",
        "block_determinism_perf_baseline_lowering = block_literal_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m170_lowering_block_determinism_perf_baseline_contract.py -q",
    ):
        assert text in fragment


def test_m170_lowering_block_determinism_perf_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3BlockDeterminismPerfBaselineLoweringLaneContract",
        "struct Objc3BlockDeterminismPerfBaselineLoweringContract",
        "std::size_t baseline_weight_total = 0;",
        "std::size_t deterministic_capture_sites = 0;",
        "std::size_t heavy_tier_sites = 0;",
        "std::size_t normalized_profile_sites = 0;",
        "IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(",
        "Objc3BlockDeterminismPerfBaselineLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(",
        "Objc3BlockDeterminismPerfBaselineLoweringReplayKey(",
        '\";baseline_weight_total=\"',
        '\";deterministic_capture_sites=\"',
        '\";heavy_tier_sites=\"',
        '\";normalized_profile_sites=\"',
        '\";lane_contract=\" + kObjc3BlockDeterminismPerfBaselineLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildBlockDeterminismPerfBaselineLoweringContract(",
        "IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(",
        "block_determinism_perf_baseline_lowering_replay_key",
        '\\"deterministic_block_determinism_perf_baseline_lowering_handoff\\":',
        '\\"objc_block_determinism_perf_baseline_lowering_surface\\":{\\"block_literal_sites\\":',
        '\\"lowering_block_determinism_perf_baseline_replay_key\\":\\"',
        "ir_frontend_metadata.lowering_block_determinism_perf_baseline_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_block_determinism_perf_baseline_replay_key;",
        "std::size_t block_determinism_perf_baseline_lowering_block_literal_sites = 0;",
        "std::size_t block_determinism_perf_baseline_lowering_baseline_weight_total = 0;",
        "std::size_t block_determinism_perf_baseline_lowering_parameter_entries_total = 0;",
        "std::size_t block_determinism_perf_baseline_lowering_capture_entries_total = 0;",
        "std::size_t block_determinism_perf_baseline_lowering_body_statement_entries_total = 0;",
        "std::size_t block_determinism_perf_baseline_lowering_deterministic_capture_sites = 0;",
        "std::size_t block_determinism_perf_baseline_lowering_heavy_tier_sites = 0;",
        "std::size_t block_determinism_perf_baseline_lowering_normalized_profile_sites = 0;",
        "std::size_t block_determinism_perf_baseline_lowering_contract_violation_sites = 0;",
        "bool deterministic_block_determinism_perf_baseline_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; block_determinism_perf_baseline_lowering = "',
        "frontend_objc_block_determinism_perf_baseline_lowering_profile",
        "!objc3.objc_block_determinism_perf_baseline_lowering = !{!23}",
        "!23 = !{i64 ",
    ):
        assert marker in ir_source
