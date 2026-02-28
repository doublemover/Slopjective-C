from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m168_sema_block_storage_escape_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M168 sema/type __block storage and escape semantics contract (M168-B001)",
        "Objc3BlockStorageEscapeSiteMetadata",
        "Objc3BlockStorageEscapeSemanticsSummary",
        "BuildBlockStorageEscapeSemanticsSummaryFromIntegrationSurface",
        "BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff",
        "block_storage_escape_sites_total",
        "deterministic_block_storage_escape_handoff",
        "python -m pytest tests/tooling/test_objc3c_m168_sema_block_storage_escape_contract.py -q",
    ):
        assert text in fragment


def test_m168_sema_block_storage_escape_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3BlockStorageEscapeSiteMetadata {",
        "std::size_t mutable_capture_count = 0;",
        "std::size_t byref_slot_count = 0;",
        "bool requires_byref_cells = false;",
        "bool escape_analysis_enabled = false;",
        "bool escape_to_heap = false;",
        "bool escape_profile_is_normalized = false;",
        "std::string escape_profile;",
        "std::string byref_layout_symbol;",
        "struct Objc3BlockStorageEscapeSemanticsSummary {",
        "std::size_t block_literal_sites = 0;",
        "std::size_t mutable_capture_count_total = 0;",
        "std::size_t byref_slot_count_total = 0;",
        "std::size_t escape_to_heap_sites = 0;",
        "std::size_t byref_layout_symbolized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::vector<Objc3BlockStorageEscapeSiteMetadata> block_storage_escape_sites_lexicographic;",
        "Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "block_storage_escape_sites_total",
        "block_storage_escape_mutable_capture_count_total",
        "block_storage_escape_byref_slot_count_total",
        "block_storage_escape_escape_to_heap_sites_total",
        "block_storage_escape_byref_layout_symbolized_sites_total",
        "block_storage_escape_contract_violation_sites_total",
        "deterministic_block_storage_escape_handoff",
        "surface.block_storage_escape_semantics_summary.block_literal_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentBlockStorageEscapeSemanticsSummary",
        "result.block_storage_escape_semantics_summary =",
        "result.deterministic_block_storage_escape_handoff =",
        "result.parity_surface.block_storage_escape_semantics_summary =",
        "result.parity_surface.block_storage_escape_sites_total =",
        "result.parity_surface.deterministic_block_storage_escape_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildBlockStorageEscapeSiteMetadata(",
        "BuildBlockStorageEscapeSiteMetadataLexicographic",
        "BuildBlockStorageEscapeSemanticsSummaryFromIntegrationSurface",
        "BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff",
        "surface.block_storage_escape_sites_lexicographic =",
        "handoff.block_storage_escape_sites_lexicographic =",
        "handoff.block_storage_escape_semantics_summary =",
        "case Expr::Kind::BlockLiteral:",
    ):
        assert marker in sema_passes
