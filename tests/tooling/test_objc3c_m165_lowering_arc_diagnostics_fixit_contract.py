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


def test_m165_lowering_arc_diagnostics_fixit_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## ARC diagnostics/fix-it lowering artifact contract (M165-C001)",
        "kObjc3ArcDiagnosticsFixitLoweringLaneContract",
        "Objc3ArcDiagnosticsFixitLoweringContract",
        "IsValidObjc3ArcDiagnosticsFixitLoweringContract(...)",
        "Objc3ArcDiagnosticsFixitLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites",
        "frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface",
        "lowering_arc_diagnostics_fixit.replay_key",
        "arc_diagnostics_fixit_lowering = ownership_arc_diagnostic_candidate_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m165_lowering_arc_diagnostics_fixit_contract.py -q",
    ):
        assert text in fragment


def test_m165_lowering_arc_diagnostics_fixit_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3ArcDiagnosticsFixitLoweringLaneContract",
        "struct Objc3ArcDiagnosticsFixitLoweringContract",
        "std::size_t ownership_arc_diagnostic_candidate_sites = 0;",
        "std::size_t ownership_arc_fixit_available_sites = 0;",
        "std::size_t ownership_arc_profiled_sites = 0;",
        "std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;",
        "std::size_t ownership_arc_empty_fixit_hint_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3ArcDiagnosticsFixitLoweringContract(",
        "Objc3ArcDiagnosticsFixitLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3ArcDiagnosticsFixitLoweringContract(",
        "Objc3ArcDiagnosticsFixitLoweringReplayKey(",
        '"ownership_arc_diagnostic_candidate_sites="',
        '";ownership_arc_fixit_available_sites="',
        '";ownership_arc_profiled_sites="',
        '";ownership_arc_weak_unowned_conflict_diagnostic_sites="',
        '";ownership_arc_empty_fixit_hint_sites="',
        '";contract_violation_sites="',
        '";lane_contract=" + kObjc3ArcDiagnosticsFixitLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildArcDiagnosticsFixitLoweringContract(",
        "IsValidObjc3ArcDiagnosticsFixitLoweringContract(",
        "arc_diagnostics_fixit_lowering_replay_key",
        '\\"deterministic_arc_diagnostics_fixit_lowering_handoff\\":',
        '\\"objc_arc_diagnostics_fixit_lowering_surface\\":{\\"ownership_arc_diagnostic_candidate_sites\\":',
        '\\"lowering_arc_diagnostics_fixit\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_arc_diagnostics_fixit_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_arc_diagnostics_fixit_replay_key;",
        "std::size_t arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites = 0;",
        "std::size_t arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites = 0;",
        "std::size_t arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites = 0;",
        "std::size_t arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;",
        "std::size_t arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites = 0;",
        "std::size_t arc_diagnostics_fixit_lowering_contract_violation_sites = 0;",
        "bool deterministic_arc_diagnostics_fixit_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; arc_diagnostics_fixit_lowering = "',
        "frontend_objc_arc_diagnostics_fixit_lowering_profile",
        "!objc3.objc_arc_diagnostics_fixit_lowering = !{!18}",
        "!18 = !{i64 ",
    ):
        assert marker in ir_source
