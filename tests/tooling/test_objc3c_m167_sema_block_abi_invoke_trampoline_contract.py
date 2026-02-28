from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m167_sema_block_abi_invoke_trampoline_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M167 sema/type block ABI invoke-trampoline semantics contract (M167-B001)",
        "Objc3BlockAbiInvokeTrampolineSiteMetadata",
        "Objc3BlockAbiInvokeTrampolineSemanticsSummary",
        "BuildBlockAbiInvokeTrampolineSemanticsSummaryFromIntegrationSurface",
        "BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff",
        "block_abi_invoke_trampoline_sites_total",
        "deterministic_block_abi_invoke_trampoline_handoff",
        "python -m pytest tests/tooling/test_objc3c_m167_sema_block_abi_invoke_trampoline_contract.py -q",
    ):
        assert text in fragment


def test_m167_sema_block_abi_invoke_trampoline_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3BlockAbiInvokeTrampolineSiteMetadata {",
        "std::size_t invoke_argument_slots = 0;",
        "std::size_t capture_word_count = 0;",
        "bool has_invoke_trampoline = false;",
        "bool layout_is_normalized = false;",
        "std::string layout_profile;",
        "std::string descriptor_symbol;",
        "std::string invoke_trampoline_symbol;",
        "struct Objc3BlockAbiInvokeTrampolineSemanticsSummary {",
        "std::size_t block_literal_sites = 0;",
        "std::size_t invoke_argument_slots_total = 0;",
        "std::size_t capture_word_count_total = 0;",
        "std::size_t descriptor_symbolized_sites = 0;",
        "std::size_t invoke_trampoline_symbolized_sites = 0;",
        "std::size_t missing_invoke_trampoline_sites = 0;",
        "std::size_t non_normalized_layout_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> block_abi_invoke_trampoline_sites_lexicographic;",
        "Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "block_abi_invoke_trampoline_sites_total",
        "block_abi_invoke_trampoline_invoke_argument_slots_total",
        "block_abi_invoke_trampoline_capture_word_count_total",
        "block_abi_invoke_trampoline_descriptor_symbolized_sites_total",
        "block_abi_invoke_trampoline_invoke_symbolized_sites_total",
        "block_abi_invoke_trampoline_missing_invoke_sites_total",
        "block_abi_invoke_trampoline_contract_violation_sites_total",
        "deterministic_block_abi_invoke_trampoline_handoff",
        "surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentBlockAbiInvokeTrampolineSemanticsSummary",
        "result.block_abi_invoke_trampoline_semantics_summary =",
        "result.deterministic_block_abi_invoke_trampoline_handoff =",
        "result.parity_surface.block_abi_invoke_trampoline_semantics_summary =",
        "result.parity_surface.block_abi_invoke_trampoline_sites_total =",
        "result.parity_surface.deterministic_block_abi_invoke_trampoline_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildBlockAbiInvokeTrampolineSiteMetadata(",
        "BuildBlockAbiInvokeTrampolineSiteMetadataLexicographic",
        "BuildBlockAbiInvokeTrampolineSemanticsSummaryFromIntegrationSurface",
        "BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff",
        "surface.block_abi_invoke_trampoline_sites_lexicographic =",
        "handoff.block_abi_invoke_trampoline_sites_lexicographic =",
        "handoff.block_abi_invoke_trampoline_semantics_summary =",
        "case Expr::Kind::BlockLiteral:",
    ):
        assert marker in sema_passes
