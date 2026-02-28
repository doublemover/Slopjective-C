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


def test_m163_lowering_autoreleasepool_scope_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Autoreleasepool scope lowering artifact contract (M163-C001)",
        "kObjc3AutoreleasePoolScopeLoweringLaneContract",
        "Objc3AutoreleasePoolScopeLoweringContract",
        "IsValidObjc3AutoreleasePoolScopeLoweringContract(...)",
        "Objc3AutoreleasePoolScopeLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.autoreleasepool_scope_lowering_scope_sites",
        "frontend.pipeline.semantic_surface.objc_autoreleasepool_scope_lowering_surface",
        "lowering_autoreleasepool_scope.replay_key",
        "autoreleasepool_scope_lowering = scope_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m163_lowering_autoreleasepool_scope_contract.py -q",
    ):
        assert text in fragment


def test_m163_lowering_autoreleasepool_scope_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3AutoreleasePoolScopeLoweringLaneContract",
        "struct Objc3AutoreleasePoolScopeLoweringContract",
        "std::size_t scope_sites = 0;",
        "std::size_t scope_symbolized_sites = 0;",
        "unsigned max_scope_depth = 0;",
        "std::size_t scope_entry_transition_sites = 0;",
        "std::size_t scope_exit_transition_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3AutoreleasePoolScopeLoweringContract(",
        "Objc3AutoreleasePoolScopeLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3AutoreleasePoolScopeLoweringContract(",
        "Objc3AutoreleasePoolScopeLoweringReplayKey(",
        '"scope_sites="',
        '";scope_symbolized_sites="',
        '";max_scope_depth="',
        '";scope_entry_transition_sites="',
        '";scope_exit_transition_sites="',
        '";contract_violation_sites="',
        '";lane_contract=" + kObjc3AutoreleasePoolScopeLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildAutoreleasePoolScopeLoweringContract(",
        "IsValidObjc3AutoreleasePoolScopeLoweringContract(",
        "autoreleasepool_scope_lowering_replay_key",
        '\\"deterministic_autoreleasepool_scope_lowering_handoff\\":',
        '\\"objc_autoreleasepool_scope_lowering_surface\\":{\\"scope_sites\\":',
        '\\"lowering_autoreleasepool_scope\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_autoreleasepool_scope_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_autoreleasepool_scope_replay_key;",
        "std::size_t autoreleasepool_scope_lowering_scope_sites = 0;",
        "std::size_t autoreleasepool_scope_lowering_scope_symbolized_sites = 0;",
        "unsigned autoreleasepool_scope_lowering_max_scope_depth = 0;",
        "std::size_t autoreleasepool_scope_lowering_scope_entry_transition_sites = 0;",
        "std::size_t autoreleasepool_scope_lowering_scope_exit_transition_sites = 0;",
        "std::size_t autoreleasepool_scope_lowering_contract_violation_sites = 0;",
        "bool deterministic_autoreleasepool_scope_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; autoreleasepool_scope_lowering = "',
        "frontend_objc_autoreleasepool_scope_lowering_profile",
        "!objc3.objc_autoreleasepool_scope_lowering = !{!16}",
        "!16 = !{i64 ",
    ):
        assert marker in ir_source
