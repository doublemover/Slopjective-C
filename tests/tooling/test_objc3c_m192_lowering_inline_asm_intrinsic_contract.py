from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m192_lowering_inline_asm_intrinsic_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Inline asm and intrinsic governance lowering artifact contract (M192-C001)",
        "kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract",
        "Objc3InlineAsmIntrinsicGovernanceLoweringContract",
        "IsValidObjc3InlineAsmIntrinsicGovernanceLoweringContract(...)",
        "Objc3InlineAsmIntrinsicGovernanceLoweringReplayKey(...)",
        "inline_asm_intrinsic_governance_lowering = inline_asm_intrinsic_sites=<N>",
        "frontend_objc_inline_asm_intrinsic_governance_lowering_profile",
        "!objc3.objc_inline_asm_intrinsic_governance_lowering = !{!38}",
        "python -m pytest tests/tooling/test_objc3c_m192_lowering_inline_asm_intrinsic_contract.py -q",
    ):
        assert text in fragment


def test_m192_lowering_inline_asm_intrinsic_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract",
        "struct Objc3InlineAsmIntrinsicGovernanceLoweringContract",
        "std::size_t inline_asm_intrinsic_sites = 0;",
        "std::size_t inline_asm_sites = 0;",
        "std::size_t intrinsic_sites = 0;",
        "std::size_t governed_intrinsic_sites = 0;",
        "std::size_t privileged_intrinsic_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t gate_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3InlineAsmIntrinsicGovernanceLoweringContract(",
        "std::string Objc3InlineAsmIntrinsicGovernanceLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3InlineAsmIntrinsicGovernanceLoweringContract(",
        "Objc3InlineAsmIntrinsicGovernanceLoweringReplayKey(",
        "contract.normalized_sites + contract.gate_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"inline_asm_intrinsic_sites="',
        '";inline_asm_sites="',
        '";intrinsic_sites="',
        '";governed_intrinsic_sites="',
        '";privileged_intrinsic_sites="',
        '";normalized_sites="',
        '";gate_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_inline_asm_intrinsic_governance_replay_key;",
        "std::size_t inline_asm_intrinsic_governance_lowering_sites = 0;",
        "std::size_t inline_asm_intrinsic_governance_lowering_inline_asm_sites = 0;",
        "std::size_t inline_asm_intrinsic_governance_lowering_intrinsic_sites = 0;",
        "std::size_t inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites =",
        "inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites = 0;",
        "std::size_t inline_asm_intrinsic_governance_lowering_normalized_sites = 0;",
        "std::size_t inline_asm_intrinsic_governance_lowering_gate_blocked_sites = 0;",
        "inline_asm_intrinsic_governance_lowering_contract_violation_sites = 0;",
        "bool deterministic_inline_asm_intrinsic_governance_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; inline_asm_intrinsic_governance_lowering = "',
        "frontend_objc_inline_asm_intrinsic_governance_lowering_profile",
        "!objc3.objc_inline_asm_intrinsic_governance_lowering = !{!38}",
        "!38 = !{i64 ",
    ):
        assert marker in ir_source
