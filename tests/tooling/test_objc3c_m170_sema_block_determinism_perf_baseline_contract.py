from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m170_sema_block_determinism_perf_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M170 sema/type block determinism perf baseline contract (M170-B001)",
        "Objc3BlockDeterminismPerfBaselineSiteMetadata",
        "Objc3BlockDeterminismPerfBaselineSummary",
        "BuildBlockDeterminismPerfBaselineSummaryFromIntegrationSurface",
        "BuildBlockDeterminismPerfBaselineSummaryFromTypeMetadataHandoff",
        "block_determinism_perf_baseline_sites_total",
        "deterministic_block_determinism_perf_baseline_handoff",
        "python -m pytest tests/tooling/test_objc3c_m170_sema_block_determinism_perf_baseline_contract.py -q",
    ):
        assert text in fragment


def test_m170_sema_block_determinism_perf_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3BlockDeterminismPerfBaselineSiteMetadata {",
        "std::size_t baseline_weight = 0;",
        "bool capture_set_deterministic = false;",
        "bool baseline_profile_is_normalized = false;",
        "std::string baseline_profile;",
        "struct Objc3BlockDeterminismPerfBaselineSummary {",
        "std::size_t baseline_weight_total = 0;",
        "std::size_t heavy_tier_sites = 0;",
        "std::size_t normalized_profile_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> block_determinism_perf_baseline_sites_lexicographic;",
        "Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "block_determinism_perf_baseline_sites_total",
        "block_determinism_perf_baseline_weight_total",
        "block_determinism_perf_baseline_contract_violation_sites_total",
        "deterministic_block_determinism_perf_baseline_handoff",
        "surface.block_determinism_perf_baseline_summary.block_literal_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentBlockDeterminismPerfBaselineSummary",
        "result.block_determinism_perf_baseline_summary =",
        "result.deterministic_block_determinism_perf_baseline_handoff =",
        "result.parity_surface.block_determinism_perf_baseline_summary =",
        "result.parity_surface.block_determinism_perf_baseline_sites_total =",
        "result.parity_surface.deterministic_block_determinism_perf_baseline_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildBlockDeterminismPerfBaselineSiteMetadata(",
        "BuildBlockDeterminismPerfBaselineSiteMetadataLexicographic",
        "BuildBlockDeterminismPerfBaselineSummaryFromIntegrationSurface",
        "BuildBlockDeterminismPerfBaselineSummaryFromTypeMetadataHandoff",
        "surface.block_determinism_perf_baseline_sites_lexicographic =",
        "handoff.block_determinism_perf_baseline_sites_lexicographic =",
        "handoff.block_determinism_perf_baseline_summary =",
        "case Expr::Kind::BlockLiteral:",
    ):
        assert marker in sema_passes
