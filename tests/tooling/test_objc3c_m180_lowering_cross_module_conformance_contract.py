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


def test_m180_lowering_cross_module_conformance_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Cross-module conformance suite lowering artifact contract (M180-C001)",
        "kObjc3CrossModuleConformanceLoweringLaneContract",
        "Objc3CrossModuleConformanceLoweringContract",
        "IsValidObjc3CrossModuleConformanceLoweringContract(...)",
        "Objc3CrossModuleConformanceLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_cross_module_conformance_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_cross_module_conformance_lowering_surface",
        "lowering_cross_module_conformance.replay_key",
        "cross_module_conformance_lowering = cross_module_conformance_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m180_lowering_cross_module_conformance_contract.py -q",
    ):
        assert text in fragment


def test_m180_lowering_cross_module_conformance_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3CrossModuleConformanceLoweringLaneContract",
        "struct Objc3CrossModuleConformanceLoweringContract",
        "std::size_t cross_module_conformance_sites = 0;",
        "std::size_t import_edge_candidate_sites = 0;",
        "std::size_t namespace_segment_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t cache_invalidation_candidate_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3CrossModuleConformanceLoweringContract(",
        "Objc3CrossModuleConformanceLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3CrossModuleConformanceLoweringContract(",
        "Objc3CrossModuleConformanceLoweringReplayKey(",
        '"cross_module_conformance_sites="',
        '";cache_invalidation_candidate_sites="',
        '";normalized_sites="',
        "kObjc3CrossModuleConformanceLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "BuildCrossModuleConformanceLoweringContract(",
        "IsValidObjc3CrossModuleConformanceLoweringContract(",
        "cross_module_conformance_lowering_replay_key",
        '\\"deterministic_cross_module_conformance_lowering_handoff\\":',
        '\\"objc_cross_module_conformance_lowering_surface\\":{\\"cross_module_conformance_sites\\":',
        '\\"lowering_cross_module_conformance_replay_key\\":\\"',
        "lowering_cross_module_conformance_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_cross_module_conformance_replay_key;",
        "std::size_t cross_module_conformance_lowering_sites = 0;",
        "std::size_t cross_module_conformance_lowering_import_edge_candidate_sites = 0;",
        "std::size_t cross_module_conformance_lowering_namespace_segment_sites = 0;",
        "std::size_t cross_module_conformance_lowering_object_pointer_type_sites = 0;",
        "std::size_t cross_module_conformance_lowering_pointer_declarator_sites = 0;",
        "std::size_t cross_module_conformance_lowering_normalized_sites = 0;",
        "std::size_t cross_module_conformance_lowering_cache_invalidation_candidate_sites = 0;",
        "std::size_t cross_module_conformance_lowering_contract_violation_sites = 0;",
        "bool deterministic_cross_module_conformance_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; cross_module_conformance_lowering = "',
        "frontend_objc_cross_module_conformance_lowering_profile",
        "!objc3.objc_cross_module_conformance_lowering = !{!33}",
        "!33 = !{i64 ",
    ):
        assert marker in ir_source
