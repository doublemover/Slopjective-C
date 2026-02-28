from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m159_frontend_super_dispatch_method_family_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M159 frontend super-dispatch and method-family parser/AST surface",
        "IsSuperDispatchReceiver(...)",
        "BuildSuperDispatchSymbol(...)",
        "ClassifyMethodFamilyFromSelector(...)",
        "BuildMethodFamilySemanticsSymbol(...)",
        "message->super_dispatch_enabled = IsSuperDispatchReceiver(*message->receiver);",
        "message->super_dispatch_requires_class_context = message->super_dispatch_enabled;",
        "message->super_dispatch_symbol = BuildSuperDispatchSymbol(...)",
        'message->method_family_name = ClassifyMethodFamilyFromSelector(message->selector);',
        'message->method_family_returns_related_result = message->method_family_name == "init";',
        "message->method_family_semantics_symbol = BuildMethodFamilySemanticsSymbol(...)",
        "python -m pytest tests/tooling/test_objc3c_m159_frontend_super_dispatch_method_family_contract.py -q",
    ):
        assert text in fragment


def test_m159_frontend_super_dispatch_method_family_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "static bool IsSuperDispatchReceiver(const Expr &receiver) {",
        'return receiver.kind == Expr::Kind::Identifier && receiver.ident == "super";',
        "static std::string ClassifyMethodFamilyFromSelector(const std::string &selector) {",
        'if (selector.rfind("mutableCopy", 0) == 0) {',
        'if (selector.rfind("copy", 0) == 0) {',
        'if (selector.rfind("init", 0) == 0) {',
        'if (selector.rfind("new", 0) == 0) {',
        'return "none";',
        "static std::string BuildSuperDispatchSymbol(",
        'out << "super-dispatch:enabled=" << (super_dispatch_enabled ? "true" : "false")',
        "static std::string BuildMethodFamilySemanticsSymbol(",
        'out << "method-family:name=" << method_family_name',
        "message->super_dispatch_enabled = IsSuperDispatchReceiver(*message->receiver);",
        "message->super_dispatch_requires_class_context = message->super_dispatch_enabled;",
        "message->super_dispatch_symbol = BuildSuperDispatchSymbol(",
        "message->super_dispatch_semantics_is_normalized = true;",
        "message->method_family_name = ClassifyMethodFamilyFromSelector(message->selector);",
        "message->method_family_returns_retained_result = message->method_family_name == \"init\" ||",
        "message->method_family_returns_related_result = message->method_family_name == \"init\";",
        "message->method_family_semantics_symbol = BuildMethodFamilySemanticsSymbol(",
        "message->method_family_semantics_is_normalized = true;",
    ):
        assert marker in parser_source

    for marker in (
        "bool super_dispatch_enabled = false;",
        "bool super_dispatch_requires_class_context = false;",
        "std::string super_dispatch_symbol;",
        "bool super_dispatch_semantics_is_normalized = false;",
        "std::string method_family_name;",
        "bool method_family_returns_retained_result = false;",
        "bool method_family_returns_related_result = false;",
        "std::string method_family_semantics_symbol;",
        "bool method_family_semantics_is_normalized = false;",
    ):
        assert marker in ast_source
