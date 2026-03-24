#!/usr/bin/env python3
"""Checker for M272-B001 dynamism/dispatch-control semantic model."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m272-b001-part9-dynamism-dispatch-control-semantic-model-v1"
CONTRACT_ID = "objc3c-part9-dynamism-dispatch-control-semantic-model/m272-b001-v1"
SURFACE_KEY = "objc_part9_dynamism_and_dispatch_control_semantic_model"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part9_dynamism_and_dispatch_control_semantic_model"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m272" / "M272-B001" / "dispatch_intent_semantic_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m272_dynamism_and_dispatch_control_semantic_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m272" / "m272_b001_dynamism_and_dispatch_control_semantic_model_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_b001_dispatch_intent_semantic_model_positive.objc3"


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
        failures.append(Finding(display_path(path), "M272-B001-MISSING", f"missing artifact: {display_path(path)}"))
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
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m272-b001-readiness",
        "--summary-out",
        "tmp/reports/m272/M272-B001/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M272-B001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M272-B001-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m272" / "b001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([str(args.runner_exe), str(FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    output = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M272-B001-DYN-03", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M272-B001-DYN-04", "positive manifest missing", failures)

    packet: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected_exact = {
        "prefixed_container_attribute_sites": 1,
        "direct_members_container_sites": 1,
        "final_container_sites": 1,
        "sealed_container_sites": 1,
        "override_conflicts": 0,
        "unresolved_base_interfaces": 0,
    }
    for index, (field, expected_value) in enumerate(expected_exact.items(), start=5):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M272-B001-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    at_least_one_fields = {
        "effective_direct_member_sites": 1,
        "direct_members_defaulted_method_sites": 1,
        "direct_members_dynamic_opt_out_sites": 1,
        "override_lookup_sites": 1,
        "override_lookup_hits": 1,
    }
    for index, (field, minimum) in enumerate(at_least_one_fields.items(), start=11):
        checks_total += 1
        value = packet.get(field)
        checks_passed += require(isinstance(value, int) and value >= minimum, display_path(manifest_path), f"M272-B001-DYN-{index:02d}", f"packet field {field} must be >= {minimum}", failures)

    checks_total += 1
    checks_passed += require(packet.get("override_lookup_hits", 0) + packet.get("override_lookup_misses", 0) == packet.get("override_lookup_sites", -1), display_path(manifest_path), "M272-B001-DYN-16", "override hit/miss accounting mismatch", failures)
    checks_total += 1
    checks_passed += require(packet.get("source_dependency_required") is True, display_path(manifest_path), "M272-B001-DYN-17", "source_dependency_required must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("dispatch_intent_source_supported") is True, display_path(manifest_path), "M272-B001-DYN-18", "dispatch_intent_source_supported must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("override_semantic_surface_reused") is True, display_path(manifest_path), "M272-B001-DYN-19", "override_semantic_surface_reused must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("direct_dispatch_reserved_non_goal") is True, display_path(manifest_path), "M272-B001-DYN-20", "direct_dispatch_reserved_non_goal must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("final_sealed_enforcement_deferred") is True, display_path(manifest_path), "M272-B001-DYN-21", "final_sealed_enforcement_deferred must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("lowering_runtime_deferred") is True, display_path(manifest_path), "M272-B001-DYN-22", "lowering_runtime_deferred must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("deterministic") is True, display_path(manifest_path), "M272-B001-DYN-23", "deterministic must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("ready_for_core_implementation") is True, display_path(manifest_path), "M272-B001-DYN-24", "ready_for_core_implementation must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("surface_path") == SURFACE_PATH, display_path(manifest_path), "M272-B001-DYN-25", "surface_path mismatch", failures)
    checks_total += 1
    checks_passed += require(bool(packet.get("replay_key")), display_path(manifest_path), "M272-B001-DYN-26", "replay_key missing", failures)
    checks_total += 1
    checks_passed += require(packet.get("failure_reason") == "", display_path(manifest_path), "M272-B001-DYN-27", "failure_reason must stay empty", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "part9_dispatch_intent_semantic_model_packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M272-B001-EXP-01", "# M272 Dynamism And Dispatch-Control Semantic Model Contract And Architecture Freeze Expectations (B001)"),
            SnippetCheck("M272-B001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M272-B001-EXP-03", SURFACE_PATH),
        ],
        PACKET_DOC: [
            SnippetCheck("M272-B001-PKT-01", "# M272-B001 Packet: Dynamism And Dispatch-Control Semantic Model - Contract And Architecture Freeze"),
            SnippetCheck("M272-B001-PKT-02", SURFACE_PATH),
            SnippetCheck("M272-B001-PKT-03", "reuse the existing override lookup/conflict accounting surface"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M272-B001-GRM-01", "## M272 dynamism and dispatch-control semantic model"),
            SnippetCheck("M272-B001-GRM-02", SURFACE_PATH),
        ],
        DOC_NATIVE: [
            SnippetCheck("M272-B001-DOC-01", "## M272 dynamism and dispatch-control semantic model"),
            SnippetCheck("M272-B001-DOC-02", SURFACE_PATH),
        ],
        SPEC_ATTR: [
            SnippetCheck("M272-B001-ATTR-01", "## M272 dynamism/dispatch-control semantic model (B001)"),
            SnippetCheck("M272-B001-ATTR-02", SURFACE_PATH),
        ],
        SPEC_DECISIONS: [
            SnippetCheck("M272-B001-DEC-01", "## D-026: Part 9 legality freeze must reuse the existing override-accounting surface before lowering begins"),
            SnippetCheck("M272-B001-DEC-02", "override lookup hits, misses, conflicts, and unresolved base-interface"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M272-B001-SEMACON-01", "kObjc3Part9DispatchIntentSemanticModelContractId"),
            SnippetCheck("M272-B001-SEMACON-02", "struct Objc3Part9DispatchIntentSemanticModelSummary"),
        ],
        SEMA_HEADER: [
            SnippetCheck("M272-B001-SEMAH-01", "BuildPart9DispatchIntentSemanticModelSummary("),
        ],
        SEMA_CPP: [
            SnippetCheck("M272-B001-SEMACPP-01", "BuildPart9DispatchIntentSemanticModelSummary("),
            SnippetCheck("M272-B001-SEMACPP-02", "surface.method_lookup_override_conflict_summary"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M272-B001-TYPE-01", "part9_dispatch_intent_semantic_model_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M272-B001-PIPE-01", "BuildPart9DispatchIntentSemanticModelSummary("),
            SnippetCheck("M272-B001-PIPE-02", "result.part9_dispatch_intent_semantic_model_summary"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M272-B001-ART-01", "BuildPart9DispatchIntentSemanticModelSummaryJson("),
            SnippetCheck("M272-B001-ART-02", "objc_part9_dynamism_and_dispatch_control_semantic_model"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M272-B001-PKG-01", '"check:objc3c:m272-b001-dynamism-and-dispatch-control-semantic-model-contract-and-architecture-freeze"'),
            SnippetCheck("M272-B001-PKG-02", '"check:objc3c:m272-b001-lane-b-readiness"'),
        ],
    }

    for path, snippet_checks in snippets.items():
        checks_total += len(snippet_checks)
        checks_passed += ensure_snippets(path, snippet_checks, failures)

    dynamic_payload: dict[str, object] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        **dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(payload, indent=2))
        return 1
    print(json.dumps(payload, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
