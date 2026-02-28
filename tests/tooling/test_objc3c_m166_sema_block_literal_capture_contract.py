from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m166_sema_block_literal_capture_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M166 sema/type block literal capture semantics contract (M166-B001)",
        "Objc3BlockLiteralCaptureSiteMetadata",
        "Objc3BlockLiteralCaptureSemanticsSummary",
        "BuildBlockLiteralCaptureSemanticsSummaryFromIntegrationSurface",
        "BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff",
        "block_literal_capture_semantics_sites_total",
        "deterministic_block_literal_capture_semantics_handoff",
        "python -m pytest tests/tooling/test_objc3c_m166_sema_block_literal_capture_contract.py -q",
    ):
        assert text in fragment


def test_m166_sema_block_literal_capture_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3BlockLiteralCaptureSiteMetadata {",
        "std::size_t parameter_count = 0;",
        "std::size_t capture_count = 0;",
        "std::size_t body_statement_count = 0;",
        "bool capture_set_deterministic = false;",
        "bool literal_is_normalized = false;",
        "bool has_count_mismatch = false;",
        "struct Objc3BlockLiteralCaptureSemanticsSummary {",
        "std::size_t block_literal_sites = 0;",
        "std::size_t block_parameter_entries = 0;",
        "std::size_t block_capture_entries = 0;",
        "std::size_t block_body_statement_entries = 0;",
        "std::size_t block_empty_capture_sites = 0;",
        "std::size_t block_nondeterministic_capture_sites = 0;",
        "std::size_t block_non_normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::vector<Objc3BlockLiteralCaptureSiteMetadata> block_literal_capture_sites_lexicographic;",
        "Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "block_literal_capture_semantics_sites_total",
        "block_literal_capture_semantics_parameter_entries_total",
        "block_literal_capture_semantics_capture_entries_total",
        "block_literal_capture_semantics_body_statement_entries_total",
        "block_literal_capture_semantics_empty_capture_sites_total",
        "block_literal_capture_semantics_nondeterministic_capture_sites_total",
        "block_literal_capture_semantics_non_normalized_sites_total",
        "block_literal_capture_semantics_contract_violation_sites_total",
        "deterministic_block_literal_capture_semantics_handoff",
        "surface.block_literal_capture_semantics_summary.block_literal_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentBlockLiteralCaptureSemanticsSummary",
        "result.block_literal_capture_semantics_summary =",
        "result.deterministic_block_literal_capture_semantics_handoff =",
        "result.parity_surface.block_literal_capture_semantics_summary =",
        "result.parity_surface.block_literal_capture_semantics_sites_total =",
        "result.parity_surface.deterministic_block_literal_capture_semantics_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildBlockLiteralCaptureSiteMetadata(",
        "BuildBlockLiteralCaptureSiteMetadataLexicographic",
        "BuildBlockLiteralCaptureSemanticsSummaryFromIntegrationSurface",
        "BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff",
        "surface.block_literal_capture_sites_lexicographic =",
        "handoff.block_literal_capture_sites_lexicographic =",
        "handoff.block_literal_capture_semantics_summary =",
        "case Expr::Kind::BlockLiteral:",
    ):
        assert marker in sema_passes
