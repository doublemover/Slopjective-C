from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m185_lowering_error_diagnostics_recovery_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Error diagnostics UX and recovery lowering artifact contract (M185-C001)",
        "kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract",
        "Objc3ErrorDiagnosticsRecoveryLoweringContract",
        "IsValidObjc3ErrorDiagnosticsRecoveryLoweringContract(...)",
        "Objc3ErrorDiagnosticsRecoveryLoweringReplayKey(...)",
        "error_diagnostics_recovery_lowering = error_diagnostic_sites=<N>",
        "frontend_objc_error_diagnostics_recovery_lowering_profile",
        "!objc3.objc_error_diagnostics_recovery_lowering = !{!44}",
        "python -m pytest tests/tooling/test_objc3c_m185_lowering_error_diagnostics_recovery_contract.py -q",
    ):
        assert text in fragment


def test_m185_lowering_error_diagnostics_recovery_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract",
        "struct Objc3ErrorDiagnosticsRecoveryLoweringContract",
        "std::size_t error_diagnostic_sites = 0;",
        "std::size_t parser_diagnostic_sites = 0;",
        "std::size_t semantic_diagnostic_sites = 0;",
        "std::size_t fixit_hint_sites = 0;",
        "std::size_t recovery_candidate_sites = 0;",
        "std::size_t recovery_applied_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t guard_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3ErrorDiagnosticsRecoveryLoweringContract(",
        "std::string Objc3ErrorDiagnosticsRecoveryLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3ErrorDiagnosticsRecoveryLoweringContract(",
        "Objc3ErrorDiagnosticsRecoveryLoweringReplayKey(",
        "contract.parser_diagnostic_sites + contract.semantic_diagnostic_sites >",
        "contract.recovery_applied_sites > contract.recovery_candidate_sites",
        "contract.normalized_sites + contract.guard_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"error_diagnostic_sites="',
        '";parser_diagnostic_sites="',
        '";semantic_diagnostic_sites="',
        '";fixit_hint_sites="',
        '";recovery_candidate_sites="',
        '";recovery_applied_sites="',
        '";normalized_sites="',
        '";guard_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_error_diagnostics_recovery_replay_key;",
        "std::size_t error_diagnostics_recovery_lowering_sites = 0;",
        "error_diagnostics_recovery_lowering_parser_diagnostic_sites = 0;",
        "error_diagnostics_recovery_lowering_semantic_diagnostic_sites = 0;",
        "std::size_t error_diagnostics_recovery_lowering_fixit_hint_sites = 0;",
        "error_diagnostics_recovery_lowering_recovery_candidate_sites = 0;",
        "error_diagnostics_recovery_lowering_recovery_applied_sites = 0;",
        "std::size_t error_diagnostics_recovery_lowering_normalized_sites = 0;",
        "std::size_t error_diagnostics_recovery_lowering_guard_blocked_sites = 0;",
        "error_diagnostics_recovery_lowering_contract_violation_sites = 0;",
        "bool deterministic_error_diagnostics_recovery_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; error_diagnostics_recovery_lowering = "',
        "frontend_objc_error_diagnostics_recovery_lowering_profile",
        "!objc3.objc_error_diagnostics_recovery_lowering = !{!44}",
        "!44 = !{i64 ",
    ):
        assert marker in ir_source
