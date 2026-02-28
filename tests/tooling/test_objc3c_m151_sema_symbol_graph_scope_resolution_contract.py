from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m151_sema_symbol_graph_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M151 sema/type symbol graph and scope resolution contract (M151-B001)",
        "Objc3SymbolGraphScopeResolutionSummary",
        "symbol_graph_scope_resolution_summary",
        "deterministic_symbol_graph_scope_resolution_handoff",
        "python -m pytest tests/tooling/test_objc3c_m151_sema_symbol_graph_scope_resolution_contract.py -q",
    ):
        assert text in fragment


def test_m151_sema_symbol_graph_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3SymbolGraphScopeResolutionSummary",
        "std::size_t global_symbol_nodes = 0;",
        "std::size_t function_symbol_nodes = 0;",
        "std::size_t interface_symbol_nodes = 0;",
        "std::size_t implementation_symbol_nodes = 0;",
        "std::size_t top_level_scope_symbols = 0;",
        "std::size_t nested_scope_symbols = 0;",
        "std::size_t scope_frames_total = 0;",
        "std::size_t method_resolution_sites = 0;",
        "std::size_t method_resolution_hits = 0;",
        "std::size_t method_resolution_misses = 0;",
        "Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "symbol_graph_global_symbol_nodes_total",
        "symbol_graph_function_symbol_nodes_total",
        "symbol_graph_interface_symbol_nodes_total",
        "symbol_graph_implementation_symbol_nodes_total",
        "symbol_graph_top_level_scope_symbols_total",
        "symbol_graph_nested_scope_symbols_total",
        "symbol_graph_scope_frames_total",
        "symbol_graph_method_resolution_sites_total",
        "symbol_graph_method_resolution_hits_total",
        "symbol_graph_method_resolution_misses_total",
        "deterministic_symbol_graph_scope_resolution_handoff",
        "Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.symbol_graph_scope_resolution_summary = result.integration_surface.symbol_graph_scope_resolution_summary;",
        "result.deterministic_symbol_graph_scope_resolution_handoff =",
        "result.parity_surface.symbol_graph_scope_resolution_summary =",
        "result.parity_surface.symbol_graph_global_symbol_nodes_total =",
        "result.parity_surface.symbol_graph_top_level_scope_symbols_total =",
        "result.parity_surface.symbol_graph_method_resolution_hits_total =",
        "result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildSymbolGraphScopeResolutionSummaryFromIntegrationSurface(",
        "BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(",
        "surface.symbol_graph_scope_resolution_summary = BuildSymbolGraphScopeResolutionSummaryFromIntegrationSurface(surface);",
        "handoff.symbol_graph_scope_resolution_summary =",
        "summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols",
        "summary.resolution_hits_total() + summary.resolution_misses_total() ==",
    ):
        assert marker in sema_passes
