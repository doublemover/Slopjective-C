from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m156_sema_message_send_selector_lowering_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M156 sema/type message-send AST form and selector-lowering contract (M156-B001)",
        "Objc3MessageSendSelectorLoweringSummary",
        "message_send_selector_lowering_summary",
        "deterministic_message_send_selector_lowering_handoff",
        "python -m pytest tests/tooling/test_objc3c_m156_sema_message_send_selector_lowering_contract.py -q",
    ):
        assert text in fragment


def test_m156_sema_message_send_selector_lowering_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3MessageSendSelectorLoweringSiteMetadata",
        "struct Objc3MessageSendSelectorLoweringSummary",
        "std::size_t selector_lowering_contract_violation_sites = 0;",
        "std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> message_send_selector_lowering_sites_lexicographic;",
        "Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "message_send_selector_lowering_sites_total",
        "message_send_selector_lowering_unary_form_sites_total",
        "message_send_selector_lowering_keyword_form_sites_total",
        "message_send_selector_lowering_argument_piece_entries_total",
        "message_send_selector_lowering_contract_violation_sites_total",
        "deterministic_message_send_selector_lowering_handoff",
        "Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentMessageSendSelectorLoweringSummary(",
        "result.message_send_selector_lowering_summary =",
        "result.deterministic_message_send_selector_lowering_handoff =",
        "result.parity_surface.message_send_selector_lowering_summary =",
        "result.parity_surface.message_send_selector_lowering_sites_total =",
        "result.parity_surface.deterministic_message_send_selector_lowering_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildMessageSendSelectorLoweringSiteMetadataLexicographic(",
        "BuildMessageSendSelectorLoweringSummaryFromIntegrationSurface(",
        "BuildMessageSendSelectorLoweringSummaryFromTypeMetadataHandoff(",
        "surface.message_send_selector_lowering_sites_lexicographic =",
        "surface.message_send_selector_lowering_summary =",
        "handoff.message_send_selector_lowering_sites_lexicographic =",
        "handoff.message_send_selector_lowering_summary =",
        "handoff.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites",
    ):
        assert marker in sema_passes
