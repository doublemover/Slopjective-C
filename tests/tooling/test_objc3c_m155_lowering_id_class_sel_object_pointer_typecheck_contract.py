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


def test_m155_lowering_id_class_sel_object_pointer_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## id/Class/SEL/object-pointer typecheck lowering artifact contract (M155-C001)",
        "kObjc3IdClassSelObjectPointerTypecheckLaneContract",
        "Objc3IdClassSelObjectPointerTypecheckContract",
        "IsValidObjc3IdClassSelObjectPointerTypecheckContract(...)",
        "Objc3IdClassSelObjectPointerTypecheckReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.id_typecheck_sites",
        "frontend.pipeline.semantic_surface.objc_id_class_sel_object_pointer_typecheck_surface",
        "lowering_id_class_sel_object_pointer_typecheck.replay_key",
        "id_class_sel_object_pointer_typecheck_lowering = id_typecheck_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m155_lowering_id_class_sel_object_pointer_typecheck_contract.py -q",
    ):
        assert text in fragment


def test_m155_lowering_id_class_sel_object_pointer_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3IdClassSelObjectPointerTypecheckLaneContract",
        "struct Objc3IdClassSelObjectPointerTypecheckContract",
        "std::size_t id_typecheck_sites = 0;",
        "std::size_t class_typecheck_sites = 0;",
        "std::size_t sel_typecheck_sites = 0;",
        "std::size_t object_pointer_typecheck_sites = 0;",
        "std::size_t total_typecheck_sites = 0;",
        "IsValidObjc3IdClassSelObjectPointerTypecheckContract(",
        "Objc3IdClassSelObjectPointerTypecheckReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3IdClassSelObjectPointerTypecheckContract(",
        "Objc3IdClassSelObjectPointerTypecheckReplayKey(",
        '"id_typecheck_sites="',
        '";object_pointer_typecheck_sites="',
        '";lane_contract=" + kObjc3IdClassSelObjectPointerTypecheckLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildIdClassSelObjectPointerTypecheckContract(",
        "IsValidObjc3IdClassSelObjectPointerTypecheckContract(",
        "id_class_sel_object_pointer_typecheck_replay_key",
        '\\"deterministic_id_class_sel_object_pointer_typecheck_handoff\\":',
        '\\"objc_id_class_sel_object_pointer_typecheck_surface\\":{\\"id_typecheck_sites\\":',
        '\\"lowering_id_class_sel_object_pointer_typecheck\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_id_class_sel_object_pointer_typecheck_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_id_class_sel_object_pointer_typecheck_replay_key;",
        "std::size_t id_typecheck_sites = 0;",
        "std::size_t class_typecheck_sites = 0;",
        "std::size_t sel_typecheck_sites = 0;",
        "std::size_t object_pointer_typecheck_sites = 0;",
        "std::size_t id_class_sel_object_pointer_typecheck_sites_total = 0;",
        "bool deterministic_id_class_sel_object_pointer_typecheck_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; id_class_sel_object_pointer_typecheck_lowering = "',
        "frontend_objc_id_class_sel_object_pointer_typecheck_profile",
        "!objc3.objc_id_class_sel_object_pointer_typecheck = !{!8}",
    ):
        assert marker in ir_source
