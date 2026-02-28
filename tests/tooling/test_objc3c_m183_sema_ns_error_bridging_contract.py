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


M183_HEADING = "M183 sema/type NSError bridging contract (M183-B001)"
M183_ANCHOR = '<a id="m183-sema-type-ns-error-bridging-contract-m183-b001"></a>'


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


def test_m183_sema_ns_error_bridging_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M183_ANCHOR)}\s*\n## {re.escape(M183_HEADING)}",
    )

    section = _extract_h2_section(fragment, M183_HEADING)
    for marker in (
        "Objc3NSErrorBridgingSummary",
        "BuildNSErrorBridgingSummaryFromIntegrationSurface",
        "BuildNSErrorBridgingSummaryFromTypeMetadataHandoff",
        "ns_error_bridging_sites_total",
        "ns_error_bridging_normalized_sites_total + ns_error_bridging_bridge_boundary_sites_total == ns_error_bridging_sites_total",
        "deterministic_ns_error_bridging_handoff",
        "python -m pytest tests/tooling/test_objc3c_m183_sema_ns_error_bridging_contract.py -q",
    ):
        assert marker in section


def test_m183_sema_ns_error_bridging_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3NSErrorBridgingSummary {",
        "std::size_t ns_error_bridging_sites = 0;",
        "std::size_t ns_error_parameter_sites = 0;",
        "std::size_t ns_error_out_parameter_sites = 0;",
        "std::size_t ns_error_bridge_path_sites = 0;",
        "Objc3NSErrorBridgingSummary ns_error_bridging_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "ns_error_bridging_sites_total",
        "ns_error_bridging_ns_error_parameter_sites_total",
        "ns_error_bridging_ns_error_out_parameter_sites_total",
        "ns_error_bridging_ns_error_bridge_path_sites_total",
        "ns_error_bridging_failable_call_sites_total",
        "ns_error_bridging_normalized_sites_total",
        "ns_error_bridging_bridge_boundary_sites_total",
        "ns_error_bridging_contract_violation_sites_total",
        "deterministic_ns_error_bridging_handoff",
        "surface.ns_error_bridging_summary.ns_error_bridging_sites ==",
    ):
        assert marker in sema_pass_manager_contract
    _assert_regex(
        sema_pass_manager_contract,
        r"ns_error_bridging_summary\.normalized_sites\s*\+\s*"
        r"surface\.ns_error_bridging_summary\.bridge_boundary_sites\s*==\s*"
        r"surface\.ns_error_bridging_summary\.ns_error_bridging_sites",
    )

    for marker in (
        "IsEquivalentNSErrorBridgingSummary",
        "result.ns_error_bridging_summary =",
        "result.deterministic_ns_error_bridging_handoff =",
        "result.parity_surface.ns_error_bridging_summary =",
        "result.parity_surface.ns_error_bridging_sites_total =",
        "result.parity_surface.deterministic_ns_error_bridging_handoff =",
    ):
        assert marker in sema_pass_manager
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\.ns_error_bridging_summary\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.ns_error_bridging_summary\s*"
        r"\.bridge_boundary_sites\s*==\s*"
        r"result\.type_metadata_handoff\.ns_error_bridging_summary\s*"
        r"\.ns_error_bridging_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.ns_error_bridging_summary\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.ns_error_bridging_summary\s*"
        r"\.bridge_boundary_sites\s*==\s*"
        r"result\.parity_surface\.ns_error_bridging_summary\s*"
        r"\.ns_error_bridging_sites",
    )

    for marker in (
        "BuildNSErrorBridgingSummaryFromIntegrationSurface(",
        "BuildNSErrorBridgingSummaryFromTypeMetadataHandoff(",
        "surface.ns_error_bridging_summary =",
        "handoff.ns_error_bridging_summary =",
        "handoff.ns_error_bridging_summary.deterministic",
    ):
        assert marker in sema_passes
    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.bridge_boundary_sites\s*==\s*"
        r"summary\.ns_error_bridging_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.ns_error_bridging_summary\.normalized_sites\s*\+\s*"
        r"handoff\.ns_error_bridging_summary\.bridge_boundary_sites\s*==\s*"
        r"handoff\.ns_error_bridging_summary\.ns_error_bridging_sites",
    )
