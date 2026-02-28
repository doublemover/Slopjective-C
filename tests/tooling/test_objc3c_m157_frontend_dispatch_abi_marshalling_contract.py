from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m157_frontend_dispatch_abi_marshalling_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M157 frontend dispatch ABI marshalling parser/AST surface",
        "ComputeDispatchAbiArgumentPaddingSlots(...)",
        "BuildDispatchAbiMarshallingSymbol(...)",
        "message->dispatch_abi_receiver_slots_marshaled = 1u;",
        "message->dispatch_abi_selector_slots_marshaled = 1u;",
        "message->dispatch_abi_argument_value_slots_marshaled = static_cast<unsigned>(message->args.size());",
        "message->dispatch_abi_runtime_arg_slots = kDispatchAbiMarshallingRuntimeArgSlots;",
        "message->dispatch_abi_argument_padding_slots_marshaled = ComputeDispatchAbiArgumentPaddingSlots(...)",
        "message->dispatch_abi_marshalling_symbol = BuildDispatchAbiMarshallingSymbol(...)",
        "message->dispatch_abi_marshalling_is_normalized = true;",
        "python -m pytest tests/tooling/test_objc3c_m157_frontend_dispatch_abi_marshalling_contract.py -q",
    ):
        assert text in fragment


def test_m157_frontend_dispatch_abi_marshalling_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "constexpr unsigned kDispatchAbiMarshallingRuntimeArgSlots = 4u;",
        "static unsigned ComputeDispatchAbiArgumentPaddingSlots(",
        "static std::string BuildDispatchAbiMarshallingSymbol(",
        "message->dispatch_abi_receiver_slots_marshaled = 1u;",
        "message->dispatch_abi_selector_slots_marshaled = 1u;",
        "message->dispatch_abi_argument_value_slots_marshaled = static_cast<unsigned>(message->args.size());",
        "message->dispatch_abi_runtime_arg_slots = kDispatchAbiMarshallingRuntimeArgSlots;",
        "message->dispatch_abi_argument_padding_slots_marshaled = ComputeDispatchAbiArgumentPaddingSlots(",
        "message->dispatch_abi_argument_total_slots_marshaled = message->dispatch_abi_argument_value_slots_marshaled +",
        "message->dispatch_abi_total_slots_marshaled = message->dispatch_abi_receiver_slots_marshaled +",
        "message->dispatch_abi_marshalling_symbol = BuildDispatchAbiMarshallingSymbol(",
        "message->dispatch_abi_marshalling_is_normalized = true;",
    ):
        assert marker in parser_source

    for marker in (
        "unsigned dispatch_abi_receiver_slots_marshaled = 0;",
        "unsigned dispatch_abi_selector_slots_marshaled = 0;",
        "unsigned dispatch_abi_argument_value_slots_marshaled = 0;",
        "unsigned dispatch_abi_argument_padding_slots_marshaled = 0;",
        "unsigned dispatch_abi_argument_total_slots_marshaled = 0;",
        "unsigned dispatch_abi_total_slots_marshaled = 0;",
        "unsigned dispatch_abi_runtime_arg_slots = 0;",
        "std::string dispatch_abi_marshalling_symbol;",
        "bool dispatch_abi_marshalling_is_normalized = false;",
    ):
        assert marker in ast_source
