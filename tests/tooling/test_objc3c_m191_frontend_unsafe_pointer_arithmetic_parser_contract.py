from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m191_frontend_unsafe_pointer_arithmetic_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M191 frontend unsafe pointer-arithmetic extension gating",
        "struct Objc3UnsafePointerExtensionProfile",
        "BuildUnsafePointerExtensionProfile(...)",
        "IsUnsafePointerExtensionProfileNormalized(...)",
        "FinalizeUnsafePointerExtensionProfile(FunctionDecl &fn)",
        "FinalizeUnsafePointerExtensionProfile(Objc3MethodDecl &method)",
        "fn.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;",
        "method.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;",
        "bool unsafe_pointer_extension_profile_is_normalized = false;",
        "bool deterministic_unsafe_pointer_extension_handoff = false;",
        "std::string unsafe_pointer_extension_profile;",
        "python -m pytest tests/tooling/test_objc3c_m191_frontend_unsafe_pointer_arithmetic_parser_contract.py -q",
    ):
        assert text in fragment


def test_m191_frontend_unsafe_pointer_arithmetic_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool unsafe_pointer_extension_profile_is_normalized = false;",
        "bool deterministic_unsafe_pointer_extension_handoff = false;",
        "std::size_t unsafe_pointer_extension_sites = 0;",
        "std::size_t unsafe_keyword_sites = 0;",
        "std::size_t pointer_arithmetic_sites = 0;",
        "std::size_t raw_pointer_type_sites = 0;",
        "std::size_t unsafe_operation_sites = 0;",
        "std::size_t unsafe_pointer_extension_normalized_sites = 0;",
        "std::size_t unsafe_pointer_extension_gate_blocked_sites = 0;",
        "std::size_t unsafe_pointer_extension_contract_violation_sites = 0;",
        "std::string unsafe_pointer_extension_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsUnsafeOwnershipQualifierSpelling(",
        "CountRawPointerTypeSites(",
        "CountUnsafeKeywordSites(",
        "IsPointerArithmeticMutationOperator(",
        "CollectPointerArithmeticExprSites(",
        "CollectPointerArithmeticStmtSites(",
        "CountPointerArithmeticSitesInBody(",
        "struct Objc3UnsafePointerExtensionProfile {",
        "BuildUnsafePointerExtensionProfile(",
        "IsUnsafePointerExtensionProfileNormalized(",
        "BuildUnsafePointerExtensionProfileFromCounts(",
        "BuildUnsafePointerExtensionProfileFromFunction(",
        "BuildUnsafePointerExtensionProfileFromOpaqueBody(",
        "FinalizeUnsafePointerExtensionProfile(FunctionDecl &fn)",
        "FinalizeUnsafePointerExtensionProfile(Objc3MethodDecl &method)",
        "target.unsafe_pointer_extension_profile = source.unsafe_pointer_extension_profile;",
        "fn.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;",
        "method.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;",
        "FinalizeUnsafePointerExtensionProfile(*fn);",
        "FinalizeUnsafePointerExtensionProfile(method);",
        "unsafe-pointer-extension:unsafe_pointer_extension_sites=",
        "profile.normalized_sites + profile.gate_blocked_sites != profile.unsafe_pointer_extension_sites",
    ):
        assert marker in parser_source
