#!/usr/bin/env python3
"""Deterministic checker for M262-B003 ARC interaction semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-b003-weak-autorelease-property-synthesis-and-block-interaction-arc-semantics-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-arc-interaction-semantics/m262-b003-v1"
SOURCE_MODEL = "explicit-arc-mode-now-covers-weak-autorelease-return-property-synthesis-and-block-ownership-interactions-for-the-supported-runnable-slice"
SEMANTIC_MODEL = "weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-stay-profiled-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc"
FAIL_CLOSED_MODEL = "unsupported-arc-cleanup-and-broader-interactions-still-remain-explicitly-deferred"
NON_GOAL_MODEL = "no-general-arc-cleanup-insertion-no-cross-module-arc-interop-no-full-method-family-automation-yet"
NEXT_ISSUE = "M262-C001"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SCAFFOLD_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ownership_aware_lowering_behavior_scaffold.h"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m262_b003_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_property_interaction_positive.objc3"
AUTORELEASE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_autorelease_return_positive.objc3"
OWNED_BLOCK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_owned_object_capture_runtime_positive.objc3"
NONOWNING_BLOCK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_nonowning_object_capture_runtime_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "b003-arc-interaction"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-B003" / "arc_interaction_semantics_summary.json"
BOUNDARY_PREFIX = "; arc_interaction_semantics = "
NAMED_METADATA_LINE = "!objc3.objc_arc_interaction_semantics = !{!79}"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M262-B003-EXP-01", "# M262 Weak, Autorelease-Return, Property-Synthesis, And Block-Interaction ARC Semantics Core Feature Expansion Expectations (B003)"),
        SnippetCheck("M262-B003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-B003-EXP-03", "attribute-only weak object properties must publish a"),
        SnippetCheck("M262-B003-EXP-04", "The contract must explicitly hand off to `M262-C001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-B003-PKT-01", "# M262-B003 Weak, Autorelease-Return, Property-Synthesis, And Block-Interaction ARC Semantics Core Feature Expansion Packet"),
        SnippetCheck("M262-B003-PKT-02", "Issue: `#7198`"),
        SnippetCheck("M262-B003-PKT-03", "Packet: `M262-B003`"),
        SnippetCheck("M262-B003-PKT-04", "`M262-C001` is the explicit next handoff after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-B003-SRC-01", "## M262 weak, autorelease-return, property-synthesis, and block-interaction ARC semantics (M262-B003)"),
        SnippetCheck("M262-B003-SRC-02", "attribute-only weak properties now publish weak synthesized accessor"),
        SnippetCheck("M262-B003-SRC-03", "`!objc3.objc_arc_interaction_semantics`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-B003-NDOC-01", "## M262 weak, autorelease-return, property-synthesis, and block-interaction ARC semantics (M262-B003)"),
        SnippetCheck("M262-B003-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-B003-NDOC-03", "`M262-C001` is the next issue."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-B003-SPC-01", "## M262 weak, autorelease-return, property-synthesis, and block-interaction ARC semantics (B003)"),
        SnippetCheck("M262-B003-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-B003-SPC-03", f"`{SEMANTIC_MODEL}`"),
        SnippetCheck("M262-B003-SPC-04", "`M262-C001` is the next issue."),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M262-B003-ATTR-01", "### B.2.7 ARC interaction semantics (implementation note) {#b-2-7}"),
        SnippetCheck("M262-B003-ATTR-02", "attribute-only weak properties now publish weak lifetime"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-B003-ARCH-01", "## M262 Weak, Autorelease-Return, Property-Synthesis, And Block-Interaction ARC Semantics (B003)"),
        SnippetCheck("M262-B003-ARCH-02", "owned block captures remain distinguishable from weak/unowned captures"),
        SnippetCheck("M262-B003-ARCH-03", "the next issue is `M262-C001`"),
    ),
    AST_HEADER: (
        SnippetCheck("M262-B003-AST-01", "kObjc3ArcInteractionSemanticsContractId"),
        SnippetCheck("M262-B003-AST-02", "kObjc3ArcInteractionSemanticsSemanticModel"),
    ),
    SEMA_CPP: (
        SnippetCheck("M262-B003-SEMA-01", "M262-B003 ARC interaction-semantics anchor"),
        SnippetCheck("M262-B003-SEMA-02", "attribute-only strong/weak"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-B003-PM-01", "M262-B003 ARC interaction-semantics expansion anchor"),
        SnippetCheck("M262-B003-PM-02", "explicit autorelease returns"),
    ),
    SCAFFOLD_HEADER: (
        SnippetCheck("M262-B003-SCAF-01", "M262-B003 ARC interaction-semantics expansion anchor"),
        SnippetCheck("M262-B003-SCAF-02", "weak/non-owning, autorelease-return"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-B003-LHDR-01", "kObjc3ArcInteractionSemanticsContractId"),
        SnippetCheck("M262-B003-LHDR-02", "std::string Objc3ArcInteractionSemanticsSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-B003-LCPP-01", "std::string Objc3ArcInteractionSemanticsSummary()"),
        SnippetCheck("M262-B003-LCPP-02", ";semantic_model="),
        SnippetCheck("M262-B003-LCPP-03", ";next_issue=M262-C001"),
    ),
    IR_CPP: (
        SnippetCheck("M262-B003-IR-01", "M262-B003 ARC interaction-semantics expansion anchor"),
        SnippetCheck("M262-B003-IR-02", "; arc_interaction_semantics = "),
        SnippetCheck("M262-B003-IR-03", "objc_arc_interaction_semantics"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-B003-PKG-01", '"check:objc3c:m262-b003-weak-autorelease-property-synthesis-and-block-interaction-arc-semantics-contract": "python scripts/check_m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion.py"'),
        SnippetCheck("M262-B003-PKG-02", '"test:tooling:m262-b003-weak-autorelease-property-synthesis-and-block-interaction-arc-semantics-contract": "python -m pytest tests/tooling/test_check_m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion.py -q"'),
        SnippetCheck("M262-B003-PKG-03", '"check:objc3c:m262-b003-lane-b-readiness": "python scripts/run_m262_b003_lane_b_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M262-B003-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M262-B003-RUN-02", "build:objc3c-native"),
        SnippetCheck("M262-B003-RUN-03", "test_check_m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M262-B003-TEST-01", "def test_m262_b003_checker_emits_summary() -> None:"),
        SnippetCheck("M262-B003-TEST-02", CONTRACT_ID),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def compile_fixture(fixture: Path, out_dir: Path, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(NATIVE_EXE), str(fixture), *extra_args, "--out-dir", str(out_dir), "--emit-prefix", "module"]
    return run_process(command)


def parse_replay_key(text: str) -> dict[str, str]:
    result: dict[str, str] = {}
    for part in text.split(";"):
        key, sep, value = part.partition("=")
        if sep:
            result[key.strip()] = value.strip()
    return result


def require_int(mapping: dict[str, str], key: str) -> int:
    return int(mapping.get(key, "0"))


def extract_property_records(manifest_payload: dict[str, Any]) -> list[dict[str, Any]]:
    return list(manifest_payload.get("runtime_metadata_source_records", {}).get("properties", []))


def find_property_record(records: list[dict[str, Any]], owner_kind: str, property_name: str) -> dict[str, Any] | None:
    for record in records:
        if record.get("owner_kind") == owner_kind and record.get("property_name") == property_name:
            return record
    return None


def dynamic_case(
    fixture: Path,
    out_dir: Path,
    failures: list[Finding],
    extra_args: Sequence[str] = (),
) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    completed = compile_fixture(fixture, out_dir, extra_args)
    manifest_path = out_dir / "module.manifest.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    checks_total += require(completed.returncode == 0, display_path(out_dir), "DYN-COMPILE", f"compile failed: {completed.stdout}{completed.stderr}", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "DYN-MANIFEST", "manifest missing", failures)
    checks_total += require(diagnostics_path.exists(), display_path(diagnostics_path), "DYN-DIAGNOSTICS", "diagnostics missing", failures)
    checks_total += require(ir_path.exists(), display_path(ir_path), "DYN-IR", "IR missing", failures)
    checks_total += require(obj_path.exists(), display_path(obj_path), "DYN-OBJ", "object missing", failures)
    if failures and any(f.check_id.startswith("DYN-") for f in failures):
        return checks_total, {}
    manifest = load_json(manifest_path)
    diagnostics = load_json(diagnostics_path)
    ir_text = ir_path.read_text(encoding="utf-8")
    checks_total += require(not diagnostics.get("diagnostics"), display_path(diagnostics_path), "DYN-DIAGNOSTICS-EMPTY", "expected zero diagnostics", failures)
    checks_total += require(BOUNDARY_PREFIX in ir_text, display_path(ir_path), "DYN-IR-BOUNDARY", "arc interaction boundary missing from IR", failures)
    checks_total += require(NAMED_METADATA_LINE in ir_text, display_path(ir_path), "DYN-IR-METADATA", "arc interaction named metadata missing from IR", failures)
    return checks_total, {
        "manifest_path": display_path(manifest_path),
        "diagnostics_path": display_path(diagnostics_path),
        "ir_path": display_path(ir_path),
        "obj_path": display_path(obj_path),
        "manifest": manifest,
    }


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M262-B003-DYN-01", "native binary is missing", failures)
    for check_id, fixture in (
        ("M262-B003-DYN-02", PROPERTY_FIXTURE),
        ("M262-B003-DYN-03", AUTORELEASE_FIXTURE),
        ("M262-B003-DYN-04", OWNED_BLOCK_FIXTURE),
        ("M262-B003-DYN-05", NONOWNING_BLOCK_FIXTURE),
    ):
        checks_total += require(fixture.exists(), display_path(fixture), check_id, "fixture is missing", failures)
    if failures:
        return checks_total, payload

    property_checks, property_case = dynamic_case(PROPERTY_FIXTURE, PROBE_ROOT / "property", failures, ["-fobjc-arc"])
    autorelease_checks, autorelease_case = dynamic_case(AUTORELEASE_FIXTURE, PROBE_ROOT / "autorelease-return", failures, ["-fobjc-arc"])
    owned_checks, owned_case = dynamic_case(OWNED_BLOCK_FIXTURE, PROBE_ROOT / "owned-block", failures, ["-fobjc-arc"])
    nonowning_checks, nonowning_case = dynamic_case(NONOWNING_BLOCK_FIXTURE, PROBE_ROOT / "nonowning-block", failures, ["-fobjc-arc"])
    checks_total += property_checks + autorelease_checks + owned_checks + nonowning_checks

    if failures:
        return checks_total, payload

    property_manifest = property_case["manifest"]
    property_records = extract_property_records(property_manifest)
    current_value = find_property_record(property_records, "class-interface", "currentValue")
    weak_value = find_property_record(property_records, "class-interface", "weakValue")

    checks_total += require(current_value is not None, display_path(PROPERTY_FIXTURE), "M262-B003-DYN-06", "currentValue property record missing", failures)
    checks_total += require(weak_value is not None, display_path(PROPERTY_FIXTURE), "M262-B003-DYN-07", "weakValue property record missing", failures)
    if current_value is not None:
        checks_total += require(current_value.get("ownership_lifetime_profile") == "strong-owned", property_case["manifest_path"], "M262-B003-DYN-08", "currentValue lifetime must be strong-owned", failures)
        checks_total += require("ownership_lifetime=strong-owned" in str(current_value.get("accessor_ownership_profile", "")), property_case["manifest_path"], "M262-B003-DYN-09", "currentValue accessor ownership profile must publish strong-owned", failures)
        checks_total += require(str(current_value.get("ownership_runtime_hook_profile", "")) in ("", "None"), property_case["manifest_path"], "M262-B003-DYN-10", "currentValue runtime hook should stay empty", failures)
    if weak_value is not None:
        checks_total += require(weak_value.get("ownership_lifetime_profile") == "weak", property_case["manifest_path"], "M262-B003-DYN-11", "weakValue lifetime must be weak", failures)
        checks_total += require(weak_value.get("ownership_runtime_hook_profile") == "objc-weak-side-table", property_case["manifest_path"], "M262-B003-DYN-12", "weakValue runtime hook must be objc-weak-side-table", failures)
        checks_total += require("ownership_lifetime=weak" in str(weak_value.get("accessor_ownership_profile", "")) and "runtime_hook=objc-weak-side-table" in str(weak_value.get("accessor_ownership_profile", "")), property_case["manifest_path"], "M262-B003-DYN-13", "weakValue accessor ownership profile drifted", failures)

    autorelease_replay = parse_replay_key(str(autorelease_case["manifest"]["lowering_retain_release_operation"]["replay_key"]))
    checks_total += require(require_int(autorelease_replay, "ownership_qualified_sites") > 0, autorelease_case["manifest_path"], "M262-B003-DYN-14", "autorelease-return probe must carry ownership-qualified sites", failures)
    checks_total += require(require_int(autorelease_replay, "autorelease_insertion_sites") > 0, autorelease_case["manifest_path"], "M262-B003-DYN-15", "autorelease-return probe must report autorelease insertion sites", failures)
    checks_total += require(require_int(autorelease_replay, "retain_insertion_sites") == 0, autorelease_case["manifest_path"], "M262-B003-DYN-16", "autorelease-return probe must not report retain insertions", failures)
    checks_total += require(require_int(autorelease_replay, "release_insertion_sites") == 0, autorelease_case["manifest_path"], "M262-B003-DYN-17", "autorelease-return probe must not report release insertions", failures)

    owned_replay = parse_replay_key(str(owned_case["manifest"]["lowering_retain_release_operation"]["replay_key"]))
    checks_total += require(require_int(owned_replay, "ownership_qualified_sites") > 0, owned_case["manifest_path"], "M262-B003-DYN-18", "owned block capture probe must carry ownership-qualified sites", failures)
    checks_total += require(require_int(owned_replay, "retain_insertion_sites") > 0, owned_case["manifest_path"], "M262-B003-DYN-19", "owned block capture probe must retain", failures)
    checks_total += require(require_int(owned_replay, "release_insertion_sites") > 0, owned_case["manifest_path"], "M262-B003-DYN-20", "owned block capture probe must release", failures)

    nonowning_replay = parse_replay_key(str(nonowning_case["manifest"]["lowering_retain_release_operation"]["replay_key"]))
    checks_total += require(require_int(nonowning_replay, "ownership_qualified_sites") > 0, nonowning_case["manifest_path"], "M262-B003-DYN-21", "non-owning block capture probe must carry ownership-qualified sites", failures)
    checks_total += require(require_int(nonowning_replay, "retain_insertion_sites") == 0, nonowning_case["manifest_path"], "M262-B003-DYN-22", "non-owning block capture probe must not retain", failures)
    checks_total += require(require_int(nonowning_replay, "release_insertion_sites") == 0, nonowning_case["manifest_path"], "M262-B003-DYN-23", "non-owning block capture probe must not release", failures)

    payload = {
        "property_case": {
            "paths": {k: property_case[k] for k in ("manifest_path", "diagnostics_path", "ir_path", "obj_path")},
            "currentValue": current_value,
            "weakValue": weak_value,
        },
        "autorelease_case": {
            "paths": {k: autorelease_case[k] for k in ("manifest_path", "diagnostics_path", "ir_path", "obj_path")},
            "retain_release_replay": autorelease_replay,
        },
        "owned_block_case": {
            "paths": {k: owned_case[k] for k in ("manifest_path", "diagnostics_path", "ir_path", "obj_path")},
            "retain_release_replay": owned_replay,
        },
        "nonowning_block_case": {
            "paths": {k: nonowning_case[k] for k in ("manifest_path", "diagnostics_path", "ir_path", "obj_path")},
            "retain_release_replay": nonowning_replay,
        },
    }
    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0

    for path, snippets in STATIC_SNIPPETS.items():
        subtotal, found = check_static_contract(path, snippets)
        checks_total += subtotal
        failures.extend(found)

    dynamic_payload: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_payload = {"skipped": True}
    else:
        subtotal, dynamic_payload = run_dynamic_probes(failures)
        checks_total += subtotal

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "source_model": SOURCE_MODEL,
        "semantic_model": SEMANTIC_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "non_goal_model": NON_GOAL_MODEL,
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_case": dynamic_payload,
    }
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"{finding.artifact}: {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[m262-b003] summary written to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
