from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m146_sema_interface_implementation_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M146 sema/type @interface/@implementation parity contract (M146-B001)",
        "Objc3InterfaceImplementationSummary",
        "deterministic_interface_implementation_handoff",
        "interfaces_total",
        "implementations_total",
        "type_metadata_interface_entries",
        "type_metadata_implementation_entries",
        "python -m pytest tests/tooling/test_objc3c_m146_sema_interface_implementation_contract.py -q",
    ):
        assert text in fragment


def test_m146_sema_interface_implementation_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3MethodInfo",
        "struct Objc3InterfaceInfo",
        "struct Objc3ImplementationInfo",
        "struct Objc3InterfaceImplementationSummary",
        "std::unordered_map<std::string, Objc3InterfaceInfo> interfaces;",
        "std::unordered_map<std::string, Objc3ImplementationInfo> implementations;",
    ):
        assert marker in sema_contract

    for marker in (
        "interfaces_total",
        "implementations_total",
        "type_metadata_interface_entries",
        "type_metadata_implementation_entries",
        "deterministic_interface_implementation_handoff",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.deterministic_interface_implementation_handoff =",
        "result.parity_surface.interfaces_total = result.integration_surface.interfaces.size();",
        "result.parity_surface.implementations_total = result.integration_surface.implementations.size();",
        "result.parity_surface.deterministic_interface_implementation_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildMethodInfo(const Objc3MethodDecl &method)",
        "missing interface declaration for implementation",
        "incompatible method signature for selector",
        "handoff.interfaces_lexicographic",
        "handoff.implementations_lexicographic",
    ):
        assert marker in sema_passes
