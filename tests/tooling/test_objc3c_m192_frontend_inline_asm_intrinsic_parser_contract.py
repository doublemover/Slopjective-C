from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m192_frontend_inline_asm_intrinsic_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M192 frontend inline asm + intrinsic governance packetization",
        "IsInlineAsmCallSymbol(...)",
        "IsIntrinsicCallSymbol(...)",
        "IsPrivilegedIntrinsicCallSymbol(...)",
        "struct Objc3InlineAsmIntrinsicGovernanceProfile",
        "BuildInlineAsmIntrinsicGovernanceProfile(...)",
        "IsInlineAsmIntrinsicGovernanceProfileNormalized(...)",
        "FinalizeInlineAsmIntrinsicGovernanceProfile(FunctionDecl &fn)",
        "FinalizeInlineAsmIntrinsicGovernanceProfile(Objc3MethodDecl &method)",
        "fn.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;",
        "method.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;",
        "bool inline_asm_intrinsic_governance_profile_is_normalized = false;",
        "bool deterministic_inline_asm_intrinsic_governance_handoff = false;",
        "std::string inline_asm_intrinsic_governance_profile;",
        "python -m pytest tests/tooling/test_objc3c_m192_frontend_inline_asm_intrinsic_parser_contract.py -q",
    ):
        assert text in fragment


def test_m192_frontend_inline_asm_intrinsic_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool inline_asm_intrinsic_governance_profile_is_normalized = false;",
        "bool deterministic_inline_asm_intrinsic_governance_handoff = false;",
        "std::size_t inline_asm_intrinsic_sites = 0;",
        "std::size_t inline_asm_sites = 0;",
        "std::size_t intrinsic_sites = 0;",
        "std::size_t governed_intrinsic_sites = 0;",
        "std::size_t privileged_intrinsic_sites = 0;",
        "std::size_t inline_asm_intrinsic_normalized_sites = 0;",
        "std::size_t inline_asm_intrinsic_gate_blocked_sites = 0;",
        "std::size_t inline_asm_intrinsic_contract_violation_sites = 0;",
        "std::string inline_asm_intrinsic_governance_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsInlineAsmCallSymbol(",
        "IsIntrinsicCallSymbol(",
        "IsPrivilegedIntrinsicCallSymbol(",
        "CollectInlineAsmIntrinsicExprSites(",
        "CollectInlineAsmIntrinsicStmtSites(",
        "CountInlineAsmIntrinsicSitesInBody(",
        "struct Objc3InlineAsmIntrinsicGovernanceProfile {",
        "BuildInlineAsmIntrinsicGovernanceProfile(",
        "IsInlineAsmIntrinsicGovernanceProfileNormalized(",
        "BuildInlineAsmIntrinsicGovernanceProfileFromCounts(",
        "BuildInlineAsmIntrinsicGovernanceProfileFromFunction(",
        "BuildInlineAsmIntrinsicGovernanceProfileFromOpaqueBody(",
        "FinalizeInlineAsmIntrinsicGovernanceProfile(FunctionDecl &fn)",
        "FinalizeInlineAsmIntrinsicGovernanceProfile(Objc3MethodDecl &method)",
        "target.inline_asm_intrinsic_governance_profile =",
        "fn.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;",
        "method.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;",
        "FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);",
        "FinalizeInlineAsmIntrinsicGovernanceProfile(method);",
        "inline-asm-intrinsic-governance:inline_asm_intrinsic_sites=",
        "profile.normalized_sites + profile.gate_blocked_sites != profile.inline_asm_intrinsic_sites",
    ):
        assert marker in parser_source
