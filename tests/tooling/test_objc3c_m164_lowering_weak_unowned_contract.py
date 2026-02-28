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


def test_m164_lowering_weak_unowned_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Weak/unowned semantics lowering artifact contract (M164-C001)",
        "kObjc3WeakUnownedSemanticsLoweringLaneContract",
        "Objc3WeakUnownedSemanticsLoweringContract",
        "IsValidObjc3WeakUnownedSemanticsLoweringContract(...)",
        "Objc3WeakUnownedSemanticsLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.weak_unowned_semantics_lowering_ownership_candidate_sites",
        "frontend.pipeline.semantic_surface.objc_weak_unowned_semantics_lowering_surface",
        "lowering_weak_unowned_semantics.replay_key",
        "weak_unowned_semantics_lowering = ownership_candidate_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m164_lowering_weak_unowned_contract.py -q",
    ):
        assert text in fragment


def test_m164_lowering_weak_unowned_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3WeakUnownedSemanticsLoweringLaneContract",
        "struct Objc3WeakUnownedSemanticsLoweringContract",
        "std::size_t ownership_candidate_sites = 0;",
        "std::size_t weak_reference_sites = 0;",
        "std::size_t unowned_reference_sites = 0;",
        "std::size_t unowned_safe_reference_sites = 0;",
        "std::size_t weak_unowned_conflict_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3WeakUnownedSemanticsLoweringContract(",
        "Objc3WeakUnownedSemanticsLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3WeakUnownedSemanticsLoweringContract(",
        "Objc3WeakUnownedSemanticsLoweringReplayKey(",
        '"ownership_candidate_sites="',
        '";weak_reference_sites="',
        '";unowned_reference_sites="',
        '";unowned_safe_reference_sites="',
        '";weak_unowned_conflict_sites="',
        '";contract_violation_sites="',
        '";lane_contract=" + kObjc3WeakUnownedSemanticsLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildWeakUnownedSemanticsLoweringContract(",
        "IsValidObjc3WeakUnownedSemanticsLoweringContract(",
        "weak_unowned_semantics_lowering_replay_key",
        '\\"deterministic_weak_unowned_semantics_lowering_handoff\\":',
        '\\"objc_weak_unowned_semantics_lowering_surface\\":{\\"ownership_candidate_sites\\":',
        '\\"lowering_weak_unowned_semantics\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_weak_unowned_semantics_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_weak_unowned_semantics_replay_key;",
        "std::size_t weak_unowned_semantics_lowering_ownership_candidate_sites = 0;",
        "std::size_t weak_unowned_semantics_lowering_weak_reference_sites = 0;",
        "std::size_t weak_unowned_semantics_lowering_unowned_reference_sites = 0;",
        "std::size_t weak_unowned_semantics_lowering_unowned_safe_reference_sites = 0;",
        "std::size_t weak_unowned_semantics_lowering_conflict_sites = 0;",
        "std::size_t weak_unowned_semantics_lowering_contract_violation_sites = 0;",
        "bool deterministic_weak_unowned_semantics_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; weak_unowned_semantics_lowering = "',
        "frontend_objc_weak_unowned_semantics_lowering_profile",
        "!objc3.objc_weak_unowned_semantics_lowering = !{!17}",
        "!17 = !{i64 ",
    ):
        assert marker in ir_source
