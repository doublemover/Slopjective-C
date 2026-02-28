from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m190_frontend_concurrency_replay_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M190 frontend concurrency replay-proof and race-guard packetization",
        "IsConcurrencyReplaySymbol(...)",
        "IsReplayProofSymbol(...)",
        "IsRaceGuardSymbol(...)",
        "IsTaskHandoffSymbol(...)",
        "IsActorIsolationSymbol(...)",
        "struct Objc3ConcurrencyReplayRaceGuardProfile",
        "BuildConcurrencyReplayRaceGuardProfile(...)",
        "IsConcurrencyReplayRaceGuardProfileNormalized(...)",
        "FinalizeConcurrencyReplayRaceGuardProfile(FunctionDecl &fn)",
        "FinalizeConcurrencyReplayRaceGuardProfile(Objc3MethodDecl &method)",
        "fn.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;",
        "method.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;",
        "bool concurrency_replay_race_guard_profile_is_normalized = false;",
        "bool deterministic_concurrency_replay_race_guard_handoff = false;",
        "std::string concurrency_replay_race_guard_profile;",
        "python -m pytest tests/tooling/test_objc3c_m190_frontend_concurrency_replay_parser_contract.py -q",
    ):
        assert text in fragment


def test_m190_frontend_concurrency_replay_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool concurrency_replay_race_guard_profile_is_normalized = false;",
        "bool deterministic_concurrency_replay_race_guard_handoff = false;",
        "std::size_t concurrency_replay_race_guard_sites = 0;",
        "std::size_t concurrency_replay_sites = 0;",
        "std::size_t replay_proof_sites = 0;",
        "std::size_t race_guard_sites = 0;",
        "std::size_t task_handoff_sites = 0;",
        "std::size_t actor_isolation_sites = 0;",
        "std::size_t deterministic_schedule_sites = 0;",
        "std::size_t concurrency_replay_guard_blocked_sites = 0;",
        "std::size_t concurrency_replay_contract_violation_sites = 0;",
        "std::string concurrency_replay_race_guard_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsConcurrencyReplaySymbol(",
        "IsReplayProofSymbol(",
        "IsRaceGuardSymbol(",
        "IsTaskHandoffSymbol(",
        "IsActorIsolationSymbol(",
        "CollectConcurrencyReplayRaceGuardExprSites(",
        "CollectConcurrencyReplayRaceGuardStmtSites(",
        "CountConcurrencyReplayRaceGuardSitesInBody(",
        "struct Objc3ConcurrencyReplayRaceGuardProfile {",
        "BuildConcurrencyReplayRaceGuardProfile(",
        "IsConcurrencyReplayRaceGuardProfileNormalized(",
        "BuildConcurrencyReplayRaceGuardProfileFromCounts(",
        "BuildConcurrencyReplayRaceGuardProfileFromFunction(",
        "BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(",
        "FinalizeConcurrencyReplayRaceGuardProfile(FunctionDecl &fn)",
        "FinalizeConcurrencyReplayRaceGuardProfile(Objc3MethodDecl &method)",
        "target.concurrency_replay_race_guard_profile =",
        "fn.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;",
        "method.concurrency_replay_race_guard_sites =",
        "profile.concurrency_replay_race_guard_sites;",
        "FinalizeConcurrencyReplayRaceGuardProfile(*fn);",
        "FinalizeConcurrencyReplayRaceGuardProfile(method);",
        "concurrency-replay-race-guard:concurrency_replay_race_guard_sites=",
        "profile.deterministic_schedule_sites + profile.guard_blocked_sites !=",
    ):
        assert marker in parser_source
