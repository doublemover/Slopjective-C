from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m148_sema_selector_normalization_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M148 sema/type selector-normalized method declaration contract (M148-B001)",
        "Objc3SelectorNormalizationSummary",
        "selector_contract_normalized",
        "selector_piece_count",
        "deterministic_selector_normalization_handoff",
        "python -m pytest tests/tooling/test_objc3c_m148_sema_selector_normalization_contract.py -q",
    ):
        assert text in fragment


def test_m148_sema_selector_normalization_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3SelectorNormalizationSummary",
        "selector_normalized",
        "selector_piece_count",
        "selector_parameter_piece_count",
        "selector_contract_normalized",
        "selector_has_parameter_linkage_mismatch",
        "Objc3SelectorNormalizationSummary selector_normalization_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "selector_normalization_methods_total",
        "selector_normalization_piece_entries_total",
        "selector_normalization_parameter_linkage_mismatches_total",
        "deterministic_selector_normalization_handoff",
        "selector_normalization_summary",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.deterministic_selector_normalization_handoff =",
        "result.parity_surface.selector_normalization_summary =",
        "result.parity_surface.selector_normalization_methods_total =",
        "result.parity_surface.selector_normalization_parameter_linkage_mismatches_total =",
        "result.parity_surface.deterministic_selector_normalization_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildNormalizedMethodSelectorFromPieces",
        "BuildMethodSelectorNormalizationContractInfo",
        "BuildSelectorNormalizationSummaryFromSurface",
        "type mismatch: selector normalization requires selector pieces",
        "type mismatch: selector normalization mismatch",
        "type mismatch: selector parameter linkage mismatch",
        "handoff.selector_normalization_summary = Objc3SelectorNormalizationSummary{};",
    ):
        assert marker in sema_passes
