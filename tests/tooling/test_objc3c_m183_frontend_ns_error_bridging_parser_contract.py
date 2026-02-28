from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m183_frontend_ns_error_bridging_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M183 frontend NSError-bridging parser/AST surface (M183-A001)",
        "BuildNSErrorBridgingProfile(...)",
        "IsNSErrorBridgingProfileNormalized(...)",
        "IsNSErrorTypeSpelling(...)",
        "IsNSErrorOutParameterSite(...)",
        "IsFailableCallSymbol(...)",
        "CountFailableCallSitesInExpr(...)",
        "BuildNSErrorBridgingProfileFromParameters(...)",
        "BuildNSErrorBridgingProfileFromFunction(...)",
        "BuildNSErrorBridgingProfileFromOpaqueBody(...)",
        "FinalizeNSErrorBridgingProfile(FunctionDecl &fn)",
        "FinalizeNSErrorBridgingProfile(Objc3MethodDecl &method)",
        "target.ns_error_bridging_profile = source.ns_error_bridging_profile;",
        "normalized_sites + bridge_boundary_sites == ns_error_bridging_sites",
        "python -m pytest tests/tooling/test_objc3c_m183_frontend_ns_error_bridging_parser_contract.py -q",
    ):
        assert text in fragment


def test_m183_frontend_ns_error_bridging_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool ns_error_bridging_profile_is_normalized = false;",
        "bool deterministic_ns_error_bridging_lowering_handoff = false;",
        "std::size_t ns_error_bridging_sites = 0;",
        "std::size_t ns_error_parameter_sites = 0;",
        "std::size_t ns_error_out_parameter_sites = 0;",
        "std::size_t ns_error_bridge_path_sites = 0;",
        "std::size_t failable_call_sites = 0;",
        "std::size_t ns_error_bridging_normalized_sites = 0;",
        "std::size_t ns_error_bridge_boundary_sites = 0;",
        "std::size_t ns_error_bridging_contract_violation_sites = 0;",
        "std::string ns_error_bridging_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildNSErrorBridgingProfile(",
        "IsNSErrorBridgingProfileNormalized(",
        "IsNSErrorTypeSpelling(",
        "IsNSErrorOutParameterSite(",
        "IsFailableCallSymbol(",
        "CountFailableCallSitesInExpr(",
        "BuildNSErrorBridgingProfileFromParameters(",
        "BuildNSErrorBridgingProfileFromFunction(",
        "BuildNSErrorBridgingProfileFromOpaqueBody(",
        "FinalizeNSErrorBridgingProfile(FunctionDecl &fn)",
        "FinalizeNSErrorBridgingProfile(Objc3MethodDecl &method)",
        "target.ns_error_bridging_profile = source.ns_error_bridging_profile;",
        "target.deterministic_ns_error_bridging_lowering_handoff =",
        "FinalizeNSErrorBridgingProfile(*fn);",
        "FinalizeNSErrorBridgingProfile(method);",
        "ns-error-bridging:ns_error_bridging_sites=",
        "profile.normalized_sites + profile.bridge_boundary_sites != profile.ns_error_bridging_sites",
    ):
        assert marker in parser_source
