from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m157_sema_dispatch_abi_marshalling_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M157 sema/type dispatch ABI marshalling contract (M157-B001)",
        "Objc3DispatchAbiMarshallingSummary",
        "dispatch_abi_marshalling_summary",
        "BuildDispatchAbiMarshallingSummaryFromSites",
        "deterministic_dispatch_abi_marshalling_handoff",
        "result.parity_surface.dispatch_abi_marshalling_summary",
        "python -m pytest tests/tooling/test_objc3c_m157_sema_dispatch_abi_marshalling_contract.py -q",
    ):
        assert text in fragment


def test_m157_sema_dispatch_abi_marshalling_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3DispatchAbiMarshallingSummary",
        "std::size_t receiver_slots = 0;",
        "std::size_t selector_symbol_slots = 0;",
        "std::size_t argument_slots = 0;",
        "std::size_t keyword_argument_slots = 0;",
        "std::size_t unary_argument_slots = 0;",
        "std::size_t arity_mismatch_sites = 0;",
        "std::size_t missing_selector_symbol_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "dispatch_abi_marshalling_sites_total",
        "dispatch_abi_marshalling_receiver_slots_total",
        "dispatch_abi_marshalling_selector_symbol_slots_total",
        "dispatch_abi_marshalling_argument_slots_total",
        "dispatch_abi_marshalling_keyword_argument_slots_total",
        "dispatch_abi_marshalling_unary_argument_slots_total",
        "dispatch_abi_marshalling_arity_mismatch_sites_total",
        "dispatch_abi_marshalling_missing_selector_symbol_sites_total",
        "dispatch_abi_marshalling_contract_violation_sites_total",
        "deterministic_dispatch_abi_marshalling_handoff",
        "Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentDispatchAbiMarshallingSummary(",
        "result.dispatch_abi_marshalling_summary =",
        "result.deterministic_dispatch_abi_marshalling_handoff =",
        "result.parity_surface.dispatch_abi_marshalling_summary =",
        "result.parity_surface.dispatch_abi_marshalling_sites_total =",
        "result.parity_surface.deterministic_dispatch_abi_marshalling_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildDispatchAbiMarshallingSummaryFromSites(",
        "BuildDispatchAbiMarshallingSummaryFromIntegrationSurface(",
        "BuildDispatchAbiMarshallingSummaryFromTypeMetadataHandoff(",
        "surface.dispatch_abi_marshalling_summary =",
        "handoff.dispatch_abi_marshalling_summary =",
        "handoff.dispatch_abi_marshalling_summary.contract_violation_sites",
    ):
        assert marker in sema_passes
