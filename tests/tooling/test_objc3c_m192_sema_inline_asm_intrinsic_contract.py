import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


M192_HEADING = "M192 sema/type inline-asm+intrinsic governance contract (M192-B001)"
M192_ANCHOR = '<a id="m192-sema-type-inline-asm-intrinsic-governance-contract-m192-b001"></a>'


def _extract_h2_section(doc: str, heading: str) -> str:
    match = re.search(
        rf"^## {re.escape(heading)}\n(?P<body>.*?)(?=^## |\Z)",
        doc,
        flags=re.MULTILINE | re.DOTALL,
    )
    assert match is not None, f"missing section heading: {heading}"
    return match.group(0)


def _assert_regex(text: str, pattern: str) -> None:
    assert re.search(pattern, text, flags=re.MULTILINE | re.DOTALL), pattern


def test_m192_sema_inline_asm_intrinsic_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M192_ANCHOR)}\s*\n## {re.escape(M192_HEADING)}",
    )

    section = _extract_h2_section(fragment, M192_HEADING)
    for marker in (
        "Objc3InlineAsmIntrinsicGovernanceSummary",
        "BuildInlineAsmIntrinsicGovernanceSummaryFromUnsafePointerAndThrowsSummaries",
        "inline_asm_intrinsic_governance_sites_total",
        "inline_asm_intrinsic_governance_inline_asm_sites_total == throws_propagation_cache_invalidation_candidate_sites_total",
        "inline_asm_intrinsic_governance_intrinsic_sites_total == unsafe_pointer_extension_unsafe_operation_sites_total",
        "inline_asm_intrinsic_governance_normalized_sites_total + inline_asm_intrinsic_governance_gate_blocked_sites_total == inline_asm_intrinsic_governance_sites_total",
        "deterministic_inline_asm_intrinsic_governance_handoff",
        "python -m pytest tests/tooling/test_objc3c_m192_sema_inline_asm_intrinsic_contract.py -q",
    ):
        assert marker in section


def test_m192_sema_inline_asm_intrinsic_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3InlineAsmIntrinsicGovernanceSummary {",
        "std::size_t inline_asm_intrinsic_sites = 0;",
        "std::size_t inline_asm_sites = 0;",
        "std::size_t intrinsic_sites = 0;",
        "std::size_t governed_intrinsic_sites = 0;",
        "std::size_t privileged_intrinsic_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t gate_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3InlineAsmIntrinsicGovernanceSummary inline_asm_intrinsic_governance_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "inline_asm_intrinsic_governance_sites_total",
        "inline_asm_intrinsic_governance_inline_asm_sites_total",
        "inline_asm_intrinsic_governance_intrinsic_sites_total",
        "inline_asm_intrinsic_governance_governed_intrinsic_sites_total",
        "inline_asm_intrinsic_governance_privileged_intrinsic_sites_total",
        "inline_asm_intrinsic_governance_normalized_sites_total",
        "inline_asm_intrinsic_governance_gate_blocked_sites_total",
        "inline_asm_intrinsic_governance_contract_violation_sites_total",
        "deterministic_inline_asm_intrinsic_governance_handoff",
        "surface.inline_asm_intrinsic_governance_summary.inline_asm_intrinsic_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    _assert_regex(
        sema_pass_manager_contract,
        r"inline_asm_intrinsic_governance_summary\s*\.normalized_sites\s*\+\s*"
        r"surface\.inline_asm_intrinsic_governance_summary\s*\.gate_blocked_sites\s*==\s*"
        r"surface\.inline_asm_intrinsic_governance_summary\s*\.inline_asm_intrinsic_sites",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"inline_asm_intrinsic_governance_summary\s*\.inline_asm_sites\s*==\s*"
        r"surface\.throws_propagation_summary\s*\.cache_invalidation_candidate_sites",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"inline_asm_intrinsic_governance_summary\s*\.intrinsic_sites\s*==\s*"
        r"surface\.unsafe_pointer_extension_summary\s*\.unsafe_operation_sites",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"inline_asm_intrinsic_governance_summary\s*\.normalized_sites\s*-\s*"
        r"surface\.inline_asm_intrinsic_governance_summary\s*\.inline_asm_sites\s*==\s*"
        r"surface\.inline_asm_intrinsic_governance_summary\s*\.governed_intrinsic_sites",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"inline_asm_intrinsic_governance_summary\s*\.gate_blocked_sites\s*==\s*"
        r"surface\.inline_asm_intrinsic_governance_summary\s*\.intrinsic_sites\s*-\s*"
        r"surface\.inline_asm_intrinsic_governance_summary\s*\.governed_intrinsic_sites",
    )

    for marker in (
        "IsEquivalentInlineAsmIntrinsicGovernanceSummary",
        "result.inline_asm_intrinsic_governance_summary =",
        "result.deterministic_inline_asm_intrinsic_governance_handoff =",
        "result.parity_surface.inline_asm_intrinsic_governance_summary =",
        "result.parity_surface.inline_asm_intrinsic_governance_sites_total =",
        "result.parity_surface.deterministic_inline_asm_intrinsic_governance_handoff =",
    ):
        assert marker in sema_pass_manager

    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.inline_asm_intrinsic_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.inline_asm_sites\s*==\s*"
        r"result\.type_metadata_handoff\.throws_propagation_summary\s*"
        r"\.cache_invalidation_candidate_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.intrinsic_sites\s*==\s*"
        r"result\.type_metadata_handoff\.unsafe_pointer_extension_summary\s*"
        r"\.unsafe_operation_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.normalized_sites\s*-\s*"
        r"result\.type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.inline_asm_sites\s*==\s*"
        r"result\.type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.governed_intrinsic_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.intrinsic_sites\s*-\s*"
        r"result\.type_metadata_handoff\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.governed_intrinsic_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\s*\.inline_asm_intrinsic_governance_summary\s*\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.parity_surface\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.inline_asm_intrinsic_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.inline_asm_sites\s*==\s*"
        r"result\.parity_surface\.throws_propagation_summary\s*"
        r"\.cache_invalidation_candidate_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\s*\.inline_asm_intrinsic_governance_summary\s*"
        r"\.intrinsic_sites\s*==\s*"
        r"result\.parity_surface\.unsafe_pointer_extension_summary\s*"
        r"\.unsafe_operation_sites",
    )

    for marker in (
        "BuildInlineAsmIntrinsicGovernanceSummaryFromUnsafePointerAndThrowsSummaries(",
        "surface.inline_asm_intrinsic_governance_summary =",
        "handoff.inline_asm_intrinsic_governance_summary =",
        "handoff.inline_asm_intrinsic_governance_summary.deterministic",
    ):
        assert marker in sema_passes

    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.gate_blocked_sites\s*==\s*"
        r"summary\.inline_asm_intrinsic_sites",
    )
    _assert_regex(
        sema_passes,
        r"summary\.inline_asm_sites\s*==\s*"
        r"throws_propagation_summary\.cache_invalidation_candidate_sites",
    )
    _assert_regex(
        sema_passes,
        r"summary\.intrinsic_sites\s*==\s*"
        r"unsafe_pointer_extension_summary\.unsafe_operation_sites",
    )
    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*-\s*summary\.inline_asm_sites\s*==\s*"
        r"summary\.governed_intrinsic_sites",
    )
    _assert_regex(
        sema_passes,
        r"summary\.gate_blocked_sites\s*==\s*summary\.intrinsic_sites\s*-\s*"
        r"summary\.governed_intrinsic_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.inline_asm_intrinsic_governance_summary\s*\.inline_asm_sites\s*==\s*"
        r"handoff\.throws_propagation_summary\s*\.cache_invalidation_candidate_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.inline_asm_intrinsic_governance_summary\s*\.intrinsic_sites\s*==\s*"
        r"handoff\.unsafe_pointer_extension_summary\s*\.unsafe_operation_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.inline_asm_intrinsic_governance_summary\s*\.normalized_sites\s*-\s*"
        r"handoff\.inline_asm_intrinsic_governance_summary\s*\.inline_asm_sites\s*==\s*"
        r"handoff\.inline_asm_intrinsic_governance_summary\s*\.governed_intrinsic_sites",
    )
