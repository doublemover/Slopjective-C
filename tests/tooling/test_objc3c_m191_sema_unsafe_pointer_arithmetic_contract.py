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


M191_HEADING = "M191 sema/type unsafe-pointer extension gating contract (M191-B001)"
M191_ANCHOR = '<a id="m191-sema-type-unsafe-pointer-extension-gating-contract-m191-b001"></a>'


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


def test_m191_sema_unsafe_pointer_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M191_ANCHOR)}\s*\n## {re.escape(M191_HEADING)}",
    )

    section = _extract_h2_section(fragment, M191_HEADING)
    for marker in (
        "Objc3UnsafePointerExtensionSummary",
        "BuildUnsafePointerExtensionSummaryFromTypeAnnotationAndWeakUnownedSummaries",
        "unsafe_pointer_extension_sites_total",
        "unsafe_pointer_extension_normalized_sites_total",
        "unsafe_pointer_extension_gate_blocked_sites_total",
        "normalized_sites + gate_blocked_sites == unsafe_pointer_extension_sites",
        "deterministic_unsafe_pointer_extension_handoff",
        "python -m pytest tests/tooling/test_objc3c_m191_sema_unsafe_pointer_arithmetic_contract.py -q",
    ):
        assert marker in section


def test_m191_sema_unsafe_pointer_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3UnsafePointerExtensionSummary {",
        "std::size_t unsafe_pointer_extension_sites = 0;",
        "std::size_t unsafe_keyword_sites = 0;",
        "std::size_t pointer_arithmetic_sites = 0;",
        "std::size_t raw_pointer_type_sites = 0;",
        "std::size_t gate_blocked_sites = 0;",
        "Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "unsafe_pointer_extension_sites_total",
        "unsafe_pointer_extension_unsafe_keyword_sites_total",
        "unsafe_pointer_extension_pointer_arithmetic_sites_total",
        "unsafe_pointer_extension_raw_pointer_type_sites_total",
        "unsafe_pointer_extension_unsafe_operation_sites_total",
        "unsafe_pointer_extension_normalized_sites_total",
        "unsafe_pointer_extension_gate_blocked_sites_total",
        "unsafe_pointer_extension_contract_violation_sites_total",
        "deterministic_unsafe_pointer_extension_handoff",
        "surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    _assert_regex(
        sema_pass_manager_contract,
        r"unsafe_pointer_extension_summary\.normalized_sites\s*\+\s*"
        r"surface\.unsafe_pointer_extension_summary\.gate_blocked_sites\s*==\s*"
        r"surface\.unsafe_pointer_extension_summary\.unsafe_pointer_extension_sites",
    )

    for marker in (
        "IsEquivalentUnsafePointerExtensionSummary",
        "result.unsafe_pointer_extension_summary =",
        "result.deterministic_unsafe_pointer_extension_handoff =",
        "result.parity_surface.unsafe_pointer_extension_summary =",
        "result.parity_surface.unsafe_pointer_extension_sites_total =",
        "result.parity_surface.deterministic_unsafe_pointer_extension_handoff =",
    ):
        assert marker in sema_pass_manager

    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\.unsafe_pointer_extension_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.unsafe_pointer_extension_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\.unsafe_pointer_extension_summary\s*"
        r"\.unsafe_pointer_extension_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.unsafe_pointer_extension_summary\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.unsafe_pointer_extension_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.parity_surface\.unsafe_pointer_extension_summary\s*"
        r"\.unsafe_pointer_extension_sites",
    )

    for marker in (
        "BuildUnsafePointerExtensionSummaryFromTypeAnnotationAndWeakUnownedSummaries(",
        "surface.unsafe_pointer_extension_summary =",
        "handoff.unsafe_pointer_extension_summary =",
        "handoff.unsafe_pointer_extension_summary.deterministic",
    ):
        assert marker in sema_passes

    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.gate_blocked_sites\s*==\s*"
        r"summary\.unsafe_pointer_extension_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.unsafe_pointer_extension_summary\.normalized_sites\s*\+\s*"
        r"handoff\.unsafe_pointer_extension_summary\.gate_blocked_sites\s*==\s*"
        r"handoff\.unsafe_pointer_extension_summary\.unsafe_pointer_extension_sites",
    )
