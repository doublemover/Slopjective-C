from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m173_sema_protocol_qualified_object_type_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M173 sema/type protocol-qualified object type contract (M173-B001)",
        "Objc3ProtocolQualifiedObjectTypeSummary",
        "BuildProtocolQualifiedObjectTypeSummaryFromTypeAnnotationSurfaceSummary",
        "protocol_qualified_object_type_sites_total",
        "deterministic_protocol_qualified_object_type_handoff",
        "python -m pytest tests/tooling/test_objc3c_m173_sema_protocol_qualified_object_type_contract.py -q",
    ):
        assert text in fragment


def test_m173_sema_protocol_qualified_object_type_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ProtocolQualifiedObjectTypeSummary {",
        "std::size_t protocol_qualified_object_type_sites = 0;",
        "std::size_t protocol_composition_sites = 0;",
        "std::size_t normalized_protocol_composition_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "protocol_qualified_object_type_sites_total",
        "protocol_qualified_object_type_protocol_composition_sites_total",
        "protocol_qualified_object_type_contract_violation_sites_total",
        "deterministic_protocol_qualified_object_type_handoff",
        "surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentProtocolQualifiedObjectTypeSummary",
        "result.protocol_qualified_object_type_summary =",
        "result.deterministic_protocol_qualified_object_type_handoff =",
        "result.parity_surface.protocol_qualified_object_type_summary =",
        "result.parity_surface.protocol_qualified_object_type_sites_total =",
        "result.parity_surface.deterministic_protocol_qualified_object_type_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildProtocolQualifiedObjectTypeSummaryFromTypeAnnotationSurfaceSummary(",
        "surface.protocol_qualified_object_type_summary =",
        "handoff.protocol_qualified_object_type_summary =",
        "handoff.protocol_qualified_object_type_summary.deterministic",
    ):
        assert marker in sema_passes
