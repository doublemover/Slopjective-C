from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m184_lowering_unwind_cleanup_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Unwind safety and cleanup emission lowering artifact contract (M184-C001)",
        "kObjc3UnwindCleanupLoweringLaneContract",
        "Objc3UnwindCleanupLoweringContract",
        "IsValidObjc3UnwindCleanupLoweringContract(...)",
        "Objc3UnwindCleanupLoweringReplayKey(...)",
        "unwind_cleanup_lowering = unwind_cleanup_sites=<N>",
        "frontend_objc_unwind_cleanup_lowering_profile",
        "!objc3.objc_unwind_cleanup_lowering = !{!35}",
        "python -m pytest tests/tooling/test_objc3c_m184_lowering_unwind_cleanup_contract.py -q",
    ):
        assert text in fragment


def test_m184_lowering_unwind_cleanup_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3UnwindCleanupLoweringLaneContract",
        "struct Objc3UnwindCleanupLoweringContract",
        "std::size_t unwind_cleanup_sites = 0;",
        "std::size_t unwind_edge_sites = 0;",
        "std::size_t cleanup_scope_sites = 0;",
        "std::size_t cleanup_emit_sites = 0;",
        "std::size_t landing_pad_sites = 0;",
        "std::size_t cleanup_resume_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t guard_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3UnwindCleanupLoweringContract(",
        "std::string Objc3UnwindCleanupLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3UnwindCleanupLoweringContract(",
        "Objc3UnwindCleanupLoweringReplayKey(",
        "contract.cleanup_emit_sites > contract.cleanup_scope_sites",
        "contract.landing_pad_sites + contract.cleanup_resume_sites >",
        "contract.normalized_sites + contract.guard_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"unwind_cleanup_sites="',
        '";unwind_edge_sites="',
        '";cleanup_scope_sites="',
        '";cleanup_emit_sites="',
        '";landing_pad_sites="',
        '";cleanup_resume_sites="',
        '";normalized_sites="',
        '";guard_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3UnwindCleanupLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_unwind_cleanup_replay_key;",
        "std::size_t unwind_cleanup_lowering_sites = 0;",
        "std::size_t unwind_cleanup_lowering_unwind_edge_sites = 0;",
        "std::size_t unwind_cleanup_lowering_cleanup_scope_sites = 0;",
        "std::size_t unwind_cleanup_lowering_cleanup_emit_sites = 0;",
        "std::size_t unwind_cleanup_lowering_landing_pad_sites = 0;",
        "std::size_t unwind_cleanup_lowering_cleanup_resume_sites = 0;",
        "std::size_t unwind_cleanup_lowering_normalized_sites = 0;",
        "std::size_t unwind_cleanup_lowering_guard_blocked_sites = 0;",
        "std::size_t unwind_cleanup_lowering_contract_violation_sites = 0;",
        "bool deterministic_unwind_cleanup_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; unwind_cleanup_lowering = "',
        "frontend_objc_unwind_cleanup_lowering_profile",
        "!objc3.objc_unwind_cleanup_lowering = !{!35}",
        "!35 = !{i64 ",
    ):
        assert marker in ir_source
