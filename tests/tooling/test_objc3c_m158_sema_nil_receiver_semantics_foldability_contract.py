from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m158_sema_nil_receiver_semantics_foldability_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M158 sema/type nil-receiver semantics/foldability contract (M158-B001)",
        "Objc3NilReceiverSemanticsFoldabilitySummary",
        "nil_receiver_semantics_foldability_summary",
        "BuildNilReceiverSemanticsFoldabilitySummaryFromSites",
        "deterministic_nil_receiver_semantics_foldability_handoff",
        "result.parity_surface.nil_receiver_semantics_foldability_summary",
        "python -m pytest tests/tooling/test_objc3c_m158_sema_nil_receiver_semantics_foldability_contract.py -q",
    ):
        assert text in fragment


def test_m158_sema_nil_receiver_semantics_foldability_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3NilReceiverSemanticsFoldabilitySummary",
        "std::size_t receiver_nil_literal_sites = 0;",
        "std::size_t nil_receiver_semantics_enabled_sites = 0;",
        "std::size_t nil_receiver_foldable_sites = 0;",
        "std::size_t nil_receiver_runtime_dispatch_required_sites = 0;",
        "std::size_t non_nil_receiver_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;",
        "bool receiver_is_nil_literal = false;",
        "bool nil_receiver_semantics_enabled = false;",
        "bool nil_receiver_foldable = false;",
        "bool nil_receiver_requires_runtime_dispatch = true;",
        "bool nil_receiver_semantics_is_normalized = false;",
    ):
        assert marker in sema_contract

    for marker in (
        "nil_receiver_semantics_foldability_sites_total",
        "nil_receiver_semantics_foldability_receiver_nil_literal_sites_total",
        "nil_receiver_semantics_foldability_enabled_sites_total",
        "nil_receiver_semantics_foldability_foldable_sites_total",
        "nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total",
        "nil_receiver_semantics_foldability_non_nil_receiver_sites_total",
        "nil_receiver_semantics_foldability_contract_violation_sites_total",
        "deterministic_nil_receiver_semantics_foldability_handoff",
        "Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentNilReceiverSemanticsFoldabilitySummary(",
        "result.nil_receiver_semantics_foldability_summary =",
        "result.deterministic_nil_receiver_semantics_foldability_handoff =",
        "result.parity_surface.nil_receiver_semantics_foldability_summary =",
        "result.parity_surface.nil_receiver_semantics_foldability_sites_total =",
        "result.parity_surface.deterministic_nil_receiver_semantics_foldability_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildNilReceiverSemanticsFoldabilitySummaryFromSites(",
        "BuildNilReceiverSemanticsFoldabilitySummaryFromIntegrationSurface(",
        "BuildNilReceiverSemanticsFoldabilitySummaryFromTypeMetadataHandoff(",
        "metadata.receiver_is_nil_literal = expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::NilLiteral;",
        "metadata.nil_receiver_semantics_enabled =",
        "metadata.nil_receiver_foldable =",
        "metadata.nil_receiver_requires_runtime_dispatch =",
        "surface.nil_receiver_semantics_foldability_summary =",
        "handoff.nil_receiver_semantics_foldability_summary =",
        "handoff.nil_receiver_semantics_foldability_summary.contract_violation_sites",
    ):
        assert marker in sema_passes
