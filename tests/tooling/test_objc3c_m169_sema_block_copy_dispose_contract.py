from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m169_sema_block_copy_dispose_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M169 sema/type block copy-dispose helper semantics contract (M169-B001)",
        "Objc3BlockCopyDisposeSiteMetadata",
        "Objc3BlockCopyDisposeSemanticsSummary",
        "BuildBlockCopyDisposeSemanticsSummaryFromIntegrationSurface",
        "BuildBlockCopyDisposeSemanticsSummaryFromTypeMetadataHandoff",
        "block_copy_dispose_sites_total",
        "deterministic_block_copy_dispose_handoff",
        "python -m pytest tests/tooling/test_objc3c_m169_sema_block_copy_dispose_contract.py -q",
    ):
        assert text in fragment


def test_m169_sema_block_copy_dispose_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3BlockCopyDisposeSiteMetadata {",
        "bool copy_helper_required = false;",
        "bool dispose_helper_required = false;",
        "bool copy_dispose_profile_is_normalized = false;",
        "std::string copy_dispose_profile;",
        "std::string copy_helper_symbol;",
        "std::string dispose_helper_symbol;",
        "struct Objc3BlockCopyDisposeSemanticsSummary {",
        "std::size_t block_literal_sites = 0;",
        "std::size_t copy_helper_required_sites = 0;",
        "std::size_t dispose_helper_required_sites = 0;",
        "std::size_t profile_normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::vector<Objc3BlockCopyDisposeSiteMetadata> block_copy_dispose_sites_lexicographic;",
        "Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "block_copy_dispose_sites_total",
        "block_copy_dispose_copy_helper_required_sites_total",
        "block_copy_dispose_dispose_helper_required_sites_total",
        "block_copy_dispose_contract_violation_sites_total",
        "deterministic_block_copy_dispose_handoff",
        "surface.block_copy_dispose_semantics_summary.block_literal_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentBlockCopyDisposeSemanticsSummary",
        "result.block_copy_dispose_semantics_summary =",
        "result.deterministic_block_copy_dispose_handoff =",
        "result.parity_surface.block_copy_dispose_semantics_summary =",
        "result.parity_surface.block_copy_dispose_sites_total =",
        "result.parity_surface.deterministic_block_copy_dispose_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildBlockCopyDisposeSiteMetadata(",
        "BuildBlockCopyDisposeSiteMetadataLexicographic",
        "BuildBlockCopyDisposeSemanticsSummaryFromIntegrationSurface",
        "BuildBlockCopyDisposeSemanticsSummaryFromTypeMetadataHandoff",
        "surface.block_copy_dispose_sites_lexicographic =",
        "handoff.block_copy_dispose_sites_lexicographic =",
        "handoff.block_copy_dispose_semantics_summary =",
        "case Expr::Kind::BlockLiteral:",
    ):
        assert marker in sema_passes
