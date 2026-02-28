from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m188_frontend_actor_isolation_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M188 frontend actor-isolation and sendability packetization",
        "IsActorIsolationDeclSymbol(...)",
        "IsActorHopSymbol(...)",
        "IsSendableAnnotationSymbol(...)",
        "IsNonSendableCrossingSymbol(...)",
        "struct Objc3ActorIsolationSendabilityProfile",
        "BuildActorIsolationSendabilityProfile(...)",
        "IsActorIsolationSendabilityProfileNormalized(...)",
        "FinalizeActorIsolationSendabilityProfile(FunctionDecl &fn)",
        "FinalizeActorIsolationSendabilityProfile(Objc3MethodDecl &method)",
        "fn.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;",
        "method.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;",
        "bool actor_isolation_sendability_profile_is_normalized = false;",
        "bool deterministic_actor_isolation_sendability_handoff = false;",
        "std::string actor_isolation_sendability_profile;",
        "python -m pytest tests/tooling/test_objc3c_m188_frontend_actor_isolation_sendability_parser_contract.py -q",
    ):
        assert text in fragment


def test_m188_frontend_actor_isolation_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool actor_isolation_sendability_profile_is_normalized = false;",
        "bool deterministic_actor_isolation_sendability_handoff = false;",
        "std::size_t actor_isolation_sendability_sites = 0;",
        "std::size_t actor_isolation_decl_sites = 0;",
        "std::size_t actor_hop_sites = 0;",
        "std::size_t sendable_annotation_sites = 0;",
        "std::size_t non_sendable_crossing_sites = 0;",
        "std::size_t isolation_boundary_sites = 0;",
        "std::size_t actor_isolation_sendability_normalized_sites = 0;",
        "std::size_t actor_isolation_sendability_gate_blocked_sites = 0;",
        "std::size_t actor_isolation_sendability_contract_violation_sites = 0;",
        "std::string actor_isolation_sendability_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsActorIsolationDeclSymbol(",
        "IsActorHopSymbol(",
        "IsSendableAnnotationSymbol(",
        "IsNonSendableCrossingSymbol(",
        "CollectActorIsolationSendabilityExprSites(",
        "CollectActorIsolationSendabilityStmtSites(",
        "CountActorIsolationSendabilitySitesInBody(",
        "struct Objc3ActorIsolationSendabilityProfile {",
        "BuildActorIsolationSendabilityProfile(",
        "IsActorIsolationSendabilityProfileNormalized(",
        "BuildActorIsolationSendabilityProfileFromCounts(",
        "BuildActorIsolationSendabilityProfileFromFunction(",
        "BuildActorIsolationSendabilityProfileFromOpaqueBody(",
        "FinalizeActorIsolationSendabilityProfile(FunctionDecl &fn)",
        "FinalizeActorIsolationSendabilityProfile(Objc3MethodDecl &method)",
        "target.actor_isolation_sendability_profile =",
        "fn.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;",
        "method.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;",
        "FinalizeActorIsolationSendabilityProfile(*fn);",
        "FinalizeActorIsolationSendabilityProfile(method);",
        "actor-isolation-sendability:actor_isolation_sendability_sites=",
        "profile.normalized_sites + profile.gate_blocked_sites !=",
    ):
        assert marker in parser_source
