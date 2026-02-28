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


def test_m154_lowering_property_synthesis_ivar_binding_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Property synthesis/ivar binding lowering artifact contract (M154-C001)",
        "kObjc3PropertySynthesisIvarBindingLaneContract",
        "Objc3PropertySynthesisIvarBindingContract",
        "Objc3DefaultPropertySynthesisIvarBindingContract(...)",
        "frontend.pipeline.sema_pass_manager.property_synthesis_sites",
        "frontend.pipeline.semantic_surface.objc_property_synthesis_ivar_binding_surface",
        "lowering_property_synthesis_ivar_binding.replay_key",
        "property_synthesis_ivar_binding_lowering = property_synthesis_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m154_lowering_property_synthesis_ivar_binding_contract.py -q",
    ):
        assert text in fragment


def test_m154_lowering_property_synthesis_ivar_binding_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3PropertySynthesisIvarBindingLaneContract",
        "struct Objc3PropertySynthesisIvarBindingContract",
        "std::size_t property_synthesis_sites = 0;",
        "std::size_t property_synthesis_explicit_ivar_bindings = 0;",
        "std::size_t property_synthesis_default_ivar_bindings = 0;",
        "std::size_t ivar_binding_sites = 0;",
        "std::size_t ivar_binding_resolved = 0;",
        "std::size_t ivar_binding_missing = 0;",
        "std::size_t ivar_binding_conflicts = 0;",
        "Objc3DefaultPropertySynthesisIvarBindingContract(",
        "IsValidObjc3PropertySynthesisIvarBindingContract(",
        "Objc3PropertySynthesisIvarBindingReplayKey(",
    ):
        assert marker in header

    for marker in (
        "Objc3DefaultPropertySynthesisIvarBindingContract(",
        "contract.ivar_binding_sites = property_synthesis_sites;",
        "if (contract.ivar_binding_sites != contract.property_synthesis_sites) {",
        "Objc3PropertySynthesisIvarBindingReplayKey(",
        '"property_synthesis_sites="',
        '";ivar_binding_conflicts="',
        '";lane_contract=" + kObjc3PropertySynthesisIvarBindingLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildPropertySynthesisIvarBindingContract(",
        "Objc3DefaultPropertySynthesisIvarBindingContract(",
        "IsValidObjc3PropertySynthesisIvarBindingContract(",
        "property_synthesis_ivar_binding_replay_key",
        '\\"deterministic_property_synthesis_ivar_binding_handoff\\":',
        '\\"objc_property_synthesis_ivar_binding_surface\\":{\\"property_synthesis_sites\\":',
        '\\"lowering_property_synthesis_ivar_binding\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key =",
    ):
        assert marker in artifacts_source

    assert "std::string lowering_property_synthesis_ivar_binding_replay_key;" in ir_header
    assert 'out << "; property_synthesis_ivar_binding_lowering = "' in ir_source
