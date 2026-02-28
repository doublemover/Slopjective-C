from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m163_sema_autoreleasepool_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M163 sema/type autoreleasepool scope contract (M163-B001)",
        "Objc3AutoreleasePoolScopeSiteMetadata",
        "Objc3AutoreleasePoolScopeSummary",
        "BuildAutoreleasePoolScopeSiteMetadataLexicographic",
        "BuildAutoreleasePoolScopeSummaryFromIntegrationSurface",
        "BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff",
        "autoreleasepool_scope_sites_total",
        "deterministic_autoreleasepool_scope_handoff",
        "python -m pytest tests/tooling/test_objc3c_m163_sema_autorelease_pool_scope_contract.py -q",
    ):
        assert text in fragment


def test_m163_sema_autoreleasepool_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3AutoreleasePoolScopeSiteMetadata {",
        "std::string scope_symbol;",
        "unsigned scope_depth = 0;",
        "struct Objc3AutoreleasePoolScopeSummary {",
        "std::size_t scope_sites = 0;",
        "std::size_t scope_symbolized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "unsigned max_scope_depth = 0;",
        "std::vector<Objc3AutoreleasePoolScopeSiteMetadata> autoreleasepool_scope_sites_lexicographic;",
        "Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "autoreleasepool_scope_sites_total",
        "autoreleasepool_scope_symbolized_sites_total",
        "autoreleasepool_scope_contract_violation_sites_total",
        "autoreleasepool_scope_max_depth_total",
        "deterministic_autoreleasepool_scope_handoff",
        "surface.autoreleasepool_scope_summary.scope_sites ==",
        "surface.autoreleasepool_scope_summary.max_scope_depth ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentAutoreleasePoolScopeSummary",
        "result.autoreleasepool_scope_summary = result.integration_surface.autoreleasepool_scope_summary;",
        "result.deterministic_autoreleasepool_scope_handoff =",
        "result.parity_surface.autoreleasepool_scope_summary =",
        "result.parity_surface.autoreleasepool_scope_sites_total =",
        "result.parity_surface.deterministic_autoreleasepool_scope_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildAutoreleasePoolScopeSiteMetadata(",
        "CollectAutoreleasePoolScopeSiteMetadataFromStatements(",
        "IsAutoreleasePoolScopeSiteMetadataLess(",
        "BuildAutoreleasePoolScopeSiteMetadataLexicographic(",
        "BuildAutoreleasePoolScopeSummaryFromSites(",
        "BuildAutoreleasePoolScopeSummaryFromIntegrationSurface(",
        "BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff(",
        "surface.autoreleasepool_scope_sites_lexicographic =",
        "handoff.autoreleasepool_scope_sites_lexicographic =",
        "handoff.autoreleasepool_scope_summary =",
    ):
        assert marker in sema_passes
