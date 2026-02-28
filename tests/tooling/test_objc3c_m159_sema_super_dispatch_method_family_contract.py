from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m159_sema_super_dispatch_method_family_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M159 sema/type super-dispatch and method-family contract (M159-B001)",
        "Objc3SuperDispatchMethodFamilySummary",
        "super_dispatch_method_family_summary",
        "BuildSuperDispatchMethodFamilySummaryFromSites",
        "deterministic_super_dispatch_method_family_handoff",
        "result.parity_surface.super_dispatch_method_family_summary",
        "python -m pytest tests/tooling/test_objc3c_m159_sema_super_dispatch_method_family_contract.py -q",
    ):
        assert text in fragment


def test_m159_sema_super_dispatch_method_family_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3SuperDispatchMethodFamilySummary",
        "std::size_t receiver_super_identifier_sites = 0;",
        "std::size_t super_dispatch_enabled_sites = 0;",
        "std::size_t super_dispatch_requires_class_context_sites = 0;",
        "std::size_t method_family_init_sites = 0;",
        "std::size_t method_family_copy_sites = 0;",
        "std::size_t method_family_mutable_copy_sites = 0;",
        "std::size_t method_family_new_sites = 0;",
        "std::size_t method_family_none_sites = 0;",
        "std::size_t method_family_returns_retained_result_sites = 0;",
        "std::size_t method_family_returns_related_result_sites = 0;",
        "Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;",
        "bool receiver_is_super_identifier = false;",
        "bool super_dispatch_enabled = false;",
        "bool super_dispatch_requires_class_context = false;",
        "bool super_dispatch_semantics_is_normalized = false;",
        "std::string method_family_name;",
        "bool method_family_returns_retained_result = false;",
        "bool method_family_returns_related_result = false;",
        "bool method_family_semantics_is_normalized = false;",
    ):
        assert marker in sema_contract

    for marker in (
        "super_dispatch_method_family_sites_total",
        "super_dispatch_method_family_receiver_super_identifier_sites_total",
        "super_dispatch_method_family_enabled_sites_total",
        "super_dispatch_method_family_requires_class_context_sites_total",
        "super_dispatch_method_family_init_sites_total",
        "super_dispatch_method_family_copy_sites_total",
        "super_dispatch_method_family_mutable_copy_sites_total",
        "super_dispatch_method_family_new_sites_total",
        "super_dispatch_method_family_none_sites_total",
        "super_dispatch_method_family_returns_retained_result_sites_total",
        "super_dispatch_method_family_returns_related_result_sites_total",
        "super_dispatch_method_family_contract_violation_sites_total",
        "deterministic_super_dispatch_method_family_handoff",
        "Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentSuperDispatchMethodFamilySummary(",
        "result.super_dispatch_method_family_summary =",
        "result.deterministic_super_dispatch_method_family_handoff =",
        "result.parity_surface.super_dispatch_method_family_summary =",
        "result.parity_surface.super_dispatch_method_family_sites_total =",
        "result.parity_surface.deterministic_super_dispatch_method_family_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "ClassifyMethodFamilyFromSelector(",
        "BuildSuperDispatchMethodFamilySummaryFromSites(",
        "BuildSuperDispatchMethodFamilySummaryFromIntegrationSurface(",
        "BuildSuperDispatchMethodFamilySummaryFromTypeMetadataHandoff(",
        "metadata.receiver_is_super_identifier =",
        "metadata.super_dispatch_enabled =",
        "metadata.super_dispatch_requires_class_context =",
        "metadata.method_family_name =",
        "surface.super_dispatch_method_family_summary =",
        "handoff.super_dispatch_method_family_summary =",
    ):
        assert marker in sema_passes
