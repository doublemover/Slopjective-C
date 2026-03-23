#!/usr/bin/env python3
"""Checker for M270-A001 actor/isolation/sendable source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m270-a001-part7-actor-isolation-sendable-source-closure-v1"
CONTRACT_ID = "objc3c-part7-actor-isolation-sendable-source-closure/m270-a001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m270" / "M270-A001" / "actor_isolation_sendable_source_closure_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m270_actor_isolation_and_sendable_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m270" / "m270_a001_actor_isolation_and_sendable_source_closure_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_CONFORMANCE = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_actor_isolation_sendable_source_closure_positive.objc3"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M270-A001-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m270-a001-readiness",
        "--summary-out",
        "tmp/reports/m270/M270-A001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        "ensure_objc3c_native_build.py",
        "M270-A001-DYN-01",
        f"fast build failed: {ensure_build.stderr or ensure_build.stdout}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        args.runner_exe.exists(),
        display_path(args.runner_exe),
        "M270-A001-DYN-02",
        "frontend runner missing after build",
        failures,
    )

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m270" / "a001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M270-A001-DYN-03", f"positive fixture failed: {output}", failures)
    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M270-A001-DYN-04", "positive manifest missing", failures)

    manifest: dict[str, object] = {}
    async_effect: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = manifest["frontend"]["pipeline"]["semantic_surface"]
        async_effect = semantic_surface["objc_part7_async_effect_and_suspension_semantic_model"]

    expected = {
        "actor_isolation_sendability_sites": 4,
        "actor_isolation_decl_sites": 1,
        "actor_hop_sites": 1,
        "sendable_annotation_sites": 2,
        "non_sendable_crossing_sites": 1,
        "isolation_boundary_sites": 1,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(
            async_effect.get(field) == expected_value,
            display_path(manifest_path),
            f"M270-A001-DYN-{index:02d}",
            f"actor source field {field} mismatch",
            failures,
        )

    checks_total += 1
    checks_passed += require(
        async_effect.get("actor_isolation_sendability_semantics_landed") is True,
        display_path(manifest_path),
        "M270-A001-DYN-11",
        "actor isolation/sendability semantic carriage should already be live",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        async_effect.get("executor_runtime_deferred") is True,
        display_path(manifest_path),
        "M270-A001-DYN-12",
        "executor runtime must remain deferred at M270-A001",
        failures,
    )

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "async_effect_semantics": async_effect,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M270-A001-EXP-01", "# M270 Actor, Isolation, And Sendable Source Closure Contract And Architecture Freeze Expectations (A001)"),
            SnippetCheck("M270-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M270-A001-EXP-03", "The compiler must admit one truthful frontend source boundary for actor, isolation, and sendability markers"),
        ],
        PACKET_DOC: [
            SnippetCheck("M270-A001-PKT-01", "# M270-A001 Packet: Actor, Isolation, And Sendable Source Closure Contract And Architecture Freeze"),
            SnippetCheck("M270-A001-PKT-02", "no dedicated `actor`, `sendable`, or `nonisolated` keyword is claimed here"),
            SnippetCheck("M270-A001-PKT-03", "actor-isolation declarations, actor hops, sendable markers, and non-sendable crossings remain parser-owned symbol profiles"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M270-A001-GRM-01", "## M270 actor, isolation, and sendable source closure"),
            SnippetCheck("M270-A001-GRM-02", "no dedicated `actor`, `sendable`, or `nonisolated` keyword is claimed by this issue"),
            SnippetCheck("M270-A001-GRM-03", "parser-owned symbol profiling remains the deterministic source contract"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M270-A001-DOC-01", "## M270 actor, isolation, and sendable source closure"),
            SnippetCheck("M270-A001-DOC-02", "the happy path is proven through the frontend C API runner"),
        ],
        SPEC_AM: [
            SnippetCheck("M270-A001-AM-01", "M270-A001 actor/isolation/sendable source-closure note:"),
            SnippetCheck("M270-A001-AM-02", "no dedicated `actor`, `sendable`, or `nonisolated` keyword is claimed yet"),
        ],
        SPEC_CONFORMANCE: [
            SnippetCheck("M270-A001-CONF-01", "M270-A001 implementation note:"),
            SnippetCheck("M270-A001-CONF-02", "the current repo source boundary still relies on parser-owned actor/isolation/sendability symbol profiling"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M270-A001-TOK-01", "kObjc3ActorIsolationSendableSourceClosureContractId"),
            SnippetCheck("M270-A001-TOK-02", "kObjc3SourceOnlyFeatureClaimActorDeclarationMarkers"),
            SnippetCheck("M270-A001-TOK-03", "kObjc3SourceOnlyFeatureClaimSendableMarkers"),
        ],
        LEXER_CPP: [
            SnippetCheck("M270-A001-LEX-01", "M270-A001 source-closure note:"),
            SnippetCheck("M270-A001-LEX-02", "Do not add dedicated actor/sendable/nonisolated lexer keywords yet"),
        ],
        PARSER_CPP: [
            SnippetCheck("M270-A001-PARSE-01", "M270-A001 source-closure anchor:"),
            SnippetCheck("M270-A001-PARSE-02", "static bool IsActorIsolationDeclSymbol"),
            SnippetCheck("M270-A001-PARSE-03", "static bool IsActorHopSymbol"),
            SnippetCheck("M270-A001-PARSE-04", "static bool IsSendableAnnotationSymbol"),
            SnippetCheck("M270-A001-PARSE-05", "static bool IsNonSendableCrossingSymbol"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M270-A001-PKG-01", '"check:objc3c:m270-a001-actor-isolation-and-sendable-source-closure-contract-and-architecture-freeze"'),
            SnippetCheck("M270-A001-PKG-02", '"check:objc3c:m270-a001-lane-a-readiness"'),
        ],
        FIXTURE: [
            SnippetCheck("M270-A001-FIX-01", "fn actor_isolation_sendable_probe() -> i32 {"),
            SnippetCheck("M270-A001-FIX-02", "actor_isolation_marker()"),
            SnippetCheck("M270-A001-FIX-03", "sendable_transfer_marker()"),
            SnippetCheck("M270-A001-FIX-04", "non_sendable_bridge()"),
        ],
    }

    for path, required in snippets.items():
        checks_total += len(required)
        checks_passed += ensure_snippets(path, required, failures)

    dynamic = {}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "dynamic_probes_executed": dynamic_executed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}")
        print(f"[info] wrote summary: {display_path(args.summary_out)}")
        return 1
    print(f"[ok] M270-A001 actor/isolation/sendable source closure checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
