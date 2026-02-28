from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m160_frontend_runtime_shim_host_link_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M160 frontend runtime-shim host-link parser/AST surface",
        "kRuntimeShimHostLinkDispatchSymbol",
        "BuildRuntimeShimHostLinkSymbol(...)",
        "message->runtime_shim_host_link_required = message->nil_receiver_requires_runtime_dispatch;",
        "message->runtime_shim_host_link_elided = !message->runtime_shim_host_link_required;",
        "message->runtime_shim_host_link_declaration_parameter_count = message->dispatch_abi_runtime_arg_slots + 2u;",
        "message->runtime_dispatch_bridge_symbol = kRuntimeShimHostLinkDispatchSymbol;",
        "message->runtime_shim_host_link_symbol = BuildRuntimeShimHostLinkSymbol(...)",
        "message->runtime_shim_host_link_is_normalized = true;",
        "python -m pytest tests/tooling/test_objc3c_m160_frontend_runtime_shim_host_link_contract.py -q",
    ):
        assert text in fragment


def test_m160_frontend_runtime_shim_host_link_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        'constexpr const char *kRuntimeShimHostLinkDispatchSymbol = "objc3_msgsend_i32";',
        "static std::string BuildRuntimeShimHostLinkSymbol(",
        'out << "runtime-shim-host-link:required=" << (runtime_shim_required ? "true" : "false")',
        "message->runtime_shim_host_link_required = message->nil_receiver_requires_runtime_dispatch;",
        "message->runtime_shim_host_link_elided = !message->runtime_shim_host_link_required;",
        "message->runtime_shim_host_link_declaration_parameter_count = message->dispatch_abi_runtime_arg_slots + 2u;",
        "message->runtime_dispatch_bridge_symbol = kRuntimeShimHostLinkDispatchSymbol;",
        "message->runtime_shim_host_link_symbol = BuildRuntimeShimHostLinkSymbol(",
        "message->runtime_shim_host_link_is_normalized = true;",
    ):
        assert marker in parser_source

    for marker in (
        "bool runtime_shim_host_link_required = true;",
        "bool runtime_shim_host_link_elided = false;",
        "unsigned runtime_shim_host_link_declaration_parameter_count = 0;",
        "std::string runtime_dispatch_bridge_symbol;",
        "std::string runtime_shim_host_link_symbol;",
        "bool runtime_shim_host_link_is_normalized = false;",
    ):
        assert marker in ast_source
