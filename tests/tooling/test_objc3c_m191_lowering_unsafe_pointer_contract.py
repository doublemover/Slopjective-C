from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m191_lowering_unsafe_pointer_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Unsafe and pointer-arithmetic extension gating lowering artifact contract (M191-C001)",
        "kObjc3UnsafePointerExtensionLoweringLaneContract",
        "Objc3UnsafePointerExtensionLoweringContract",
        "IsValidObjc3UnsafePointerExtensionLoweringContract(...)",
        "Objc3UnsafePointerExtensionLoweringReplayKey(...)",
        "unsafe_pointer_extension_lowering = unsafe_pointer_extension_sites=<N>",
        "frontend_objc_unsafe_pointer_extension_lowering_profile",
        "!objc3.objc_unsafe_pointer_extension_lowering = !{!37}",
        "python -m pytest tests/tooling/test_objc3c_m191_lowering_unsafe_pointer_contract.py -q",
    ):
        assert text in fragment


def test_m191_lowering_unsafe_pointer_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3UnsafePointerExtensionLoweringLaneContract",
        "struct Objc3UnsafePointerExtensionLoweringContract",
        "std::size_t unsafe_pointer_extension_sites = 0;",
        "std::size_t unsafe_keyword_sites = 0;",
        "std::size_t pointer_arithmetic_sites = 0;",
        "std::size_t raw_pointer_type_sites = 0;",
        "std::size_t unsafe_operation_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t gate_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3UnsafePointerExtensionLoweringContract(",
        "std::string Objc3UnsafePointerExtensionLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3UnsafePointerExtensionLoweringContract(",
        "Objc3UnsafePointerExtensionLoweringReplayKey(",
        "contract.normalized_sites + contract.gate_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"unsafe_pointer_extension_sites="',
        '";unsafe_keyword_sites="',
        '";pointer_arithmetic_sites="',
        '";raw_pointer_type_sites="',
        '";unsafe_operation_sites="',
        '";normalized_sites="',
        '";gate_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3UnsafePointerExtensionLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_unsafe_pointer_extension_replay_key;",
        "std::size_t unsafe_pointer_extension_lowering_sites = 0;",
        "std::size_t unsafe_pointer_extension_lowering_unsafe_keyword_sites = 0;",
        "std::size_t unsafe_pointer_extension_lowering_pointer_arithmetic_sites = 0;",
        "std::size_t unsafe_pointer_extension_lowering_raw_pointer_type_sites = 0;",
        "std::size_t unsafe_pointer_extension_lowering_unsafe_operation_sites = 0;",
        "std::size_t unsafe_pointer_extension_lowering_normalized_sites = 0;",
        "std::size_t unsafe_pointer_extension_lowering_gate_blocked_sites = 0;",
        "std::size_t unsafe_pointer_extension_lowering_contract_violation_sites = 0;",
        "bool deterministic_unsafe_pointer_extension_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; unsafe_pointer_extension_lowering = "',
        "frontend_objc_unsafe_pointer_extension_lowering_profile",
        "!objc3.objc_unsafe_pointer_extension_lowering = !{!37}",
        "!37 = !{i64 ",
    ):
        assert marker in ir_source
