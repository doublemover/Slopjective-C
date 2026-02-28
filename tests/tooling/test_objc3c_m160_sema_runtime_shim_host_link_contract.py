from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m160_sema_runtime_shim_host_link_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M160 sema/type runtime-shim host-link contract (M160-B001)",
        "Objc3RuntimeShimHostLinkSummary",
        "runtime_shim_host_link_summary",
        "BuildRuntimeShimHostLinkSummaryFromSites",
        "deterministic_runtime_shim_host_link_handoff",
        "result.parity_surface.runtime_shim_host_link_summary",
        "python -m pytest tests/tooling/test_objc3c_m160_sema_runtime_shim_host_link_contract.py -q",
    ):
        assert text in fragment


def test_m160_sema_runtime_shim_host_link_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "inline constexpr const char *kObjc3RuntimeShimHostLinkDefaultDispatchSymbol = \"objc3_msgsend_i32\";",
        "struct Objc3RuntimeShimHostLinkSummary",
        "std::size_t runtime_shim_required_sites = 0;",
        "std::size_t runtime_shim_elided_sites = 0;",
        "std::size_t runtime_dispatch_arg_slots = 0;",
        "std::size_t runtime_dispatch_declaration_parameter_count = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::string runtime_dispatch_symbol = kObjc3RuntimeShimHostLinkDefaultDispatchSymbol;",
        "bool default_runtime_dispatch_symbol_binding = true;",
        "bool runtime_shim_host_link_required = true;",
        "bool runtime_shim_host_link_elided = false;",
        "std::size_t runtime_shim_host_link_runtime_dispatch_arg_slots = 0;",
        "std::size_t runtime_shim_host_link_declaration_parameter_count = 0;",
        "std::string runtime_dispatch_bridge_symbol;",
        "std::string runtime_shim_host_link_symbol;",
        "bool runtime_shim_host_link_is_normalized = false;",
        "Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "runtime_shim_host_link_message_send_sites_total",
        "runtime_shim_host_link_required_sites_total",
        "runtime_shim_host_link_elided_sites_total",
        "runtime_shim_host_link_runtime_dispatch_arg_slots_total",
        "runtime_shim_host_link_runtime_dispatch_declaration_parameter_count_total",
        "runtime_shim_host_link_contract_violation_sites_total",
        "runtime_shim_host_link_runtime_dispatch_symbol",
        "runtime_shim_host_link_default_runtime_dispatch_symbol_binding",
        "deterministic_runtime_shim_host_link_handoff",
        "Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentRuntimeShimHostLinkSummary(",
        "result.runtime_shim_host_link_summary =",
        "result.deterministic_runtime_shim_host_link_handoff =",
        "result.parity_surface.runtime_shim_host_link_summary =",
        "result.parity_surface.runtime_shim_host_link_message_send_sites_total =",
        "result.parity_surface.deterministic_runtime_shim_host_link_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildRuntimeShimHostLinkSummaryFromSites(",
        "BuildRuntimeShimHostLinkSummaryFromIntegrationSurface(",
        "BuildRuntimeShimHostLinkSummaryFromTypeMetadataHandoff(",
        "metadata.runtime_shim_host_link_required =",
        "metadata.runtime_shim_host_link_elided =",
        "metadata.runtime_shim_host_link_declaration_parameter_count =",
        "metadata.runtime_dispatch_bridge_symbol =",
        "metadata.runtime_shim_host_link_symbol =",
        "metadata.runtime_shim_host_link_is_normalized =",
        "surface.runtime_shim_host_link_summary =",
        "handoff.runtime_shim_host_link_summary =",
    ):
        assert marker in sema_passes
