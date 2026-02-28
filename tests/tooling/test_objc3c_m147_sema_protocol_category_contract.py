from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m147_sema_protocol_category_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M147 sema/type @protocol/@category composition contract (M147-B001)",
        "Objc3ProtocolCategoryCompositionSummary",
        "param_has_protocol_composition",
        "return_protocol_composition_lexicographic",
        "deterministic_protocol_category_composition_handoff",
        "python -m pytest tests/tooling/test_objc3c_m147_sema_protocol_category_contract.py -q",
    ):
        assert text in fragment


def test_m147_sema_protocol_category_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ProtocolCategoryCompositionSummary",
        "param_has_protocol_composition",
        "param_protocol_composition_lexicographic",
        "param_has_invalid_protocol_composition",
        "return_has_protocol_composition",
        "return_protocol_composition_lexicographic",
        "return_has_invalid_protocol_composition",
        "Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "protocol_composition_sites_total",
        "category_composition_sites_total",
        "invalid_protocol_composition_sites_total",
        "deterministic_protocol_category_composition_handoff",
        "protocol_category_composition_summary",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.deterministic_protocol_category_composition_handoff =",
        "result.parity_surface.protocol_category_composition_summary =",
        "result.parity_surface.protocol_composition_sites_total =",
        "result.parity_surface.category_composition_sites_total =",
        "result.parity_surface.deterministic_protocol_category_composition_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "ParseProtocolCompositionSuffixText",
        "BuildProtocolCategoryCompositionSummaryFromSurface",
        "type mismatch: malformed protocol composition suffix",
        "type mismatch: empty protocol composition suffix",
        "type mismatch: invalid protocol identifier",
        "type mismatch: duplicate protocol identifier",
        "protocol_category_composition_summary = BuildProtocolCategoryCompositionSummaryFromSurface(surface);",
        "handoff.protocol_category_composition_summary = Objc3ProtocolCategoryCompositionSummary{};",
    ):
        assert marker in sema_passes
