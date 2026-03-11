#!/usr/bin/env python3
"""Validate M260-B001 retainable object semantic rules freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-b001-retainable-object-semantic-rules-freeze-v1"
CONTRACT_ID = "objc3c-retainable-object-semantic-rules-freeze/m260-b001-v1"
SEMANTIC_MODEL = "runtime-backed-object-semantic-rules-freeze-property-member-ownership-metadata-while-retain-release-and-storage-legality-remain-summary-driven"
DESTRUCTION_MODEL = "destruction-order-autoreleasepool-and-live-arc-execution-stay-fail-closed-outside-runtime-backed-storage-legality"
FAILURE_MODEL = "fail-closed-on-retainable-object-semantic-drift-or-premature-live-storage-legality-claim"
NEXT_ISSUE = "M260-B002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-B001" / "retainable_object_semantic_rules_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_retainable_object_semantic_rules_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_b001_retainable_object_semantic_rules_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PACKAGE_JSON = ROOT / "package.json"
COMPILE_SCRIPT = ROOT / "scripts" / "objc3c_native_compile.ps1"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_object_ownership_attribute_surface_positive.objc3"
PROBE_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "b001" / "positive"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=DOC_NATIVE)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--sema-source", type=Path, default=SEMA_SOURCE)
    parser.add_argument("--ir-source", type=Path, default=IR_SOURCE)
    parser.add_argument("--lowering-header", type=Path, default=LOWERING_HEADER)
    parser.add_argument("--lowering-source", type=Path, default=LOWERING_SOURCE)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--compile-script", type=Path, default=COMPILE_SCRIPT)
    parser.add_argument("--fixture", type=Path, default=FIXTURE)
    parser.add_argument("--probe-out-dir", type=Path, default=PROBE_OUT_DIR)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_compile(compile_script: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    cmd = [
        "pwsh",
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(compile_script),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    return subprocess.run(
        cmd,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        check=False,
    )


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    dynamic_probe_summary: dict[str, Any] = {"executed": False}

    checks_total += 5
    checks_passed += ensure_snippets(
        args.expectations_doc,
        (
            SnippetCheck("M260-B001-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M260-B001-DOC-02", "Issue: `#7170`"),
            SnippetCheck("M260-B001-DOC-03", "`tests/tooling/fixtures/native/m260_runtime_backed_object_ownership_attribute_surface_positive.objc3`"),
            SnippetCheck("M260-B001-DOC-04", "No runnable destruction-order semantics yet."),
            SnippetCheck("M260-B001-DOC-05", "`M260-B002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M260-B001-PKT-01", "Packet: `M260-B001`"),
            SnippetCheck("M260-B001-PKT-02", "Issue: `#7170`"),
            SnippetCheck("M260-B001-PKT-03", "Dependencies: none"),
            SnippetCheck("M260-B001-PKT-04", "No live ARC retain/release/autorelease execution semantics."),
            SnippetCheck("M260-B001-PKT-05", "`M260-B002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M260-B001-SRC-01", "## M260 retainable object semantic rules freeze (B001)"),
            SnippetCheck("M260-B001-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B001-SRC-03", "`retain_release_operation_lowering`"),
            SnippetCheck("M260-B001-SRC-04", "destruction ordering remains deferred and non-runnable in this freeze"),
            SnippetCheck("M260-B001-SRC-05", "`M260-B002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M260-B001-NDOC-01", "## M260 retainable object semantic rules freeze (B001)"),
            SnippetCheck("M260-B001-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B001-NDOC-03", "retain/release legality remains expressed through the legacy"),
            SnippetCheck("M260-B001-NDOC-04", "destruction ordering remains deferred and non-runnable in this freeze"),
            SnippetCheck("M260-B001-NDOC-05", "`M260-B002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.architecture_doc,
        (
            SnippetCheck("M260-B001-ARCH-01", "## M260 Retainable Object Semantic Rules Freeze (B001)"),
            SnippetCheck("M260-B001-ARCH-02", "runtime-backed property/member ownership metadata is the truthful live"),
            SnippetCheck("M260-B001-ARCH-03", "`@autoreleasepool` remains non-runnable"),
            SnippetCheck("M260-B001-ARCH-04", "destruction ordering remains deferred until later `M260` runtime work"),
            SnippetCheck("M260-B001-ARCH-05", "`M260-B002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M260-B001-SPC-01", "## M260 retainable object semantic rules freeze (B001)"),
            SnippetCheck("M260-B001-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B001-SPC-03", f"`{SEMANTIC_MODEL}`"),
            SnippetCheck("M260-B001-SPC-04", f"`{DESTRUCTION_MODEL}`"),
            SnippetCheck("M260-B001-SPC-05", f"`{FAILURE_MODEL}`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M260-B001-META-01", "## M260 retainable object semantic metadata anchors (B001)"),
            SnippetCheck("M260-B001-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B001-META-03", "tmp/artifacts/compilation/objc3c-native/m260/b001/positive/module.manifest.json"),
            SnippetCheck("M260-B001-META-04", "`M260-B002`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.sema_source,
        (
            SnippetCheck("M260-B001-SEMA-01", "M260-B001 retainable-object semantic-rule freeze anchor:"),
            SnippetCheck("M260-B001-SEMA-02", "runtime-backed"),
            SnippetCheck("M260-B001-SEMA-03", "`@autoreleasepool`, and destruction ordering"),
            SnippetCheck("M260-B001-SEMA-04", "remain fail-closed until the later M260 storage legality/runtime issues"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.ir_source,
        (
            SnippetCheck("M260-B001-IR-01", "M260-B001 retainable-object semantic-rule freeze anchor:"),
            SnippetCheck("M260-B001-IR-02", "retainable_object_semantic_rules_freeze"),
            SnippetCheck("M260-B001-IR-03", "autoreleasepool, and destruction-order behavior are still represented by"),
            SnippetCheck("M260-B001-IR-04", "Objc3RetainableObjectSemanticRulesFreezeSummary()"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_header,
        (
            SnippetCheck("M260-B001-LHDR-01", "kObjc3RetainableObjectSemanticRulesFreezeContractId"),
            SnippetCheck("M260-B001-LHDR-02", SEMANTIC_MODEL),
            SnippetCheck("M260-B001-LHDR-03", DESTRUCTION_MODEL),
            SnippetCheck("M260-B001-LHDR-04", FAILURE_MODEL),
            SnippetCheck("M260-B001-LHDR-05", "Objc3RetainableObjectSemanticRulesFreezeSummary"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.lowering_source,
        (
            SnippetCheck("M260-B001-LOW-01", "M260-B001 retainable-object semantic-rule freeze anchor:"),
            SnippetCheck("M260-B001-LOW-02", "runtime-backed"),
            SnippetCheck("M260-B001-LOW-03", "retain/release legality, autoreleasepool execution, and destruction-order"),
            SnippetCheck("M260-B001-LOW-04", "M260-B002+ land"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M260-B001-PKG-01", '"check:objc3c:m260-b001-retainable-object-semantic-rules":'),
            SnippetCheck("M260-B001-PKG-02", '"test:tooling:m260-b001-retainable-object-semantic-rules":'),
            SnippetCheck("M260-B001-PKG-03", '"check:objc3c:m260-b001-lane-b-readiness":'),
            SnippetCheck("M260-B001-PKG-04", 'python scripts/run_m260_b001_lane_b_readiness.py'),
            SnippetCheck("M260-B001-PKG-05", '"compile:objc3c":'),
        ),
        failures,
    )

    if not args.skip_dynamic_probes:
        dynamic_probe_summary["executed"] = True
        result = run_compile(args.compile_script, args.fixture, args.probe_out_dir)
        dynamic_probe_summary["returncode"] = result.returncode
        dynamic_probe_summary["stdout"] = result.stdout.strip()
        dynamic_probe_summary["stderr"] = result.stderr.strip()

        manifest_path = args.probe_out_dir / "module.manifest.json"
        ir_path = args.probe_out_dir / "module.ll"

        checks_total += 16
        checks_passed += require(result.returncode == 0, display_path(args.compile_script), "M260-B001-DYN-01", "retainable-object freeze fixture compile failed", failures)
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M260-B001-DYN-02", "manifest artifact missing", failures)
        checks_passed += require(ir_path.exists(), display_path(ir_path), "M260-B001-DYN-03", "IR artifact missing", failures)

        manifest_payload = json.loads(read_text(manifest_path)) if manifest_path.exists() else {}
        ir_text = read_text(ir_path) if ir_path.exists() else ""

        rr = manifest_payload.get("lowering_retain_release_operation", {})
        ap = manifest_payload.get("lowering_autoreleasepool_scope", {})
        wu = manifest_payload.get("lowering_weak_unowned_semantics", {})
        records = (((manifest_payload.get("runtime_metadata_source_records") or {}).get("properties")) or [])

        checks_passed += require(rr.get("lane_contract") == "m162-retain-release-operation-lowering-v1", display_path(manifest_path), "M260-B001-DYN-04", "retain/release lane contract drifted", failures)
        checks_passed += require(rr.get("replay_key") == "ownership_qualified_sites=0;retain_insertion_sites=0;release_insertion_sites=0;autorelease_insertion_sites=0;contract_violation_sites=0;deterministic=true;lane_contract=m162-retain-release-operation-lowering-v1", display_path(manifest_path), "M260-B001-DYN-05", "retain/release freeze replay key drifted", failures)
        checks_passed += require(ap.get("replay_key") == "scope_sites=0;scope_symbolized_sites=0;max_scope_depth=0;scope_entry_transition_sites=0;scope_exit_transition_sites=0;contract_violation_sites=0;deterministic=true;lane_contract=m163-autoreleasepool-scope-lowering-v1", display_path(manifest_path), "M260-B001-DYN-06", "autoreleasepool freeze replay key drifted", failures)
        checks_passed += require(wu.get("replay_key") == "ownership_candidate_sites=6;weak_reference_sites=2;unowned_reference_sites=4;unowned_safe_reference_sites=0;weak_unowned_conflict_sites=0;contract_violation_sites=0;deterministic=true;lane_contract=m164-weak-unowned-semantics-lowering-v1", display_path(manifest_path), "M260-B001-DYN-07", "weak/unowned freeze replay key drifted", failures)
        checks_passed += require(any(record.get("property_name") == "weakValue" and record.get("ownership_lifetime_profile") == "weak" for record in records), display_path(manifest_path), "M260-B001-DYN-08", "weak runtime-backed ownership metadata missing", failures)
        checks_passed += require(any(record.get("property_name") == "borrowedValue" and record.get("ownership_lifetime_profile") == "unowned-unsafe" for record in records), display_path(manifest_path), "M260-B001-DYN-09", "unowned runtime-backed ownership metadata missing", failures)
        checks_passed += require(f"; retainable_object_semantic_rules_freeze = contract={CONTRACT_ID};semantic_model={SEMANTIC_MODEL};destruction_model={DESTRUCTION_MODEL};failure_model={FAILURE_MODEL}" in ir_text, display_path(ir_path), "M260-B001-DYN-10", "IR semantic freeze summary missing", failures)
        checks_passed += require("; frontend_objc_retain_release_operation_lowering_profile = ownership_qualified_sites=0, retain_insertion_sites=0, release_insertion_sites=0, autorelease_insertion_sites=0, contract_violation_sites=0, deterministic_retain_release_operation_lowering_handoff=true" in ir_text, display_path(ir_path), "M260-B001-DYN-11", "IR retain/release zero-site freeze summary missing", failures)
        checks_passed += require("; frontend_objc_autoreleasepool_scope_lowering_profile = scope_sites=0, scope_symbolized_sites=0, max_scope_depth=0, scope_entry_transition_sites=0, scope_exit_transition_sites=0, contract_violation_sites=0, deterministic_autoreleasepool_scope_lowering_handoff=true" in ir_text, display_path(ir_path), "M260-B001-DYN-12", "IR autoreleasepool zero-site freeze summary missing", failures)
        checks_passed += require("; frontend_objc_weak_unowned_semantics_lowering_profile = ownership_candidate_sites=6, weak_reference_sites=2, unowned_reference_sites=4, unowned_safe_reference_sites=0, weak_unowned_conflict_sites=0, contract_violation_sites=0, deterministic_weak_unowned_semantics_lowering_handoff=true" in ir_text, display_path(ir_path), "M260-B001-DYN-13", "IR weak/unowned freeze summary missing", failures)
        checks_passed += require("; frontend_objc_arc_diagnostics_fixit_lowering_profile = ownership_arc_diagnostic_candidate_sites=0, ownership_arc_fixit_available_sites=0, ownership_arc_profiled_sites=0, ownership_arc_weak_unowned_conflict_diagnostic_sites=0, ownership_arc_empty_fixit_hint_sites=0, contract_violation_sites=0, deterministic_arc_diagnostics_fixit_lowering_handoff=true" in ir_text, display_path(ir_path), "M260-B001-DYN-14", "IR ARC diagnostics zero-site freeze summary missing", failures)
        checks_passed += require("@__objc3_meta_property_ownership_runtime_hook_profile_0004" in ir_text and 'c"objc-weak-side-table\\00"' in ir_text, display_path(ir_path), "M260-B001-DYN-15", "weak runtime hook property metadata missing from IR", failures)
        checks_passed += require("@__objc3_meta_property_ownership_runtime_hook_profile_0000" in ir_text and 'c"objc-unowned-unsafe-direct\\00"' in ir_text, display_path(ir_path), "M260-B001-DYN-16", "unowned runtime hook property metadata missing from IR", failures)

        dynamic_probe_summary["manifest"] = display_path(manifest_path)
        dynamic_probe_summary["ir"] = display_path(ir_path)
    else:
        dynamic_probe_summary["skipped"] = True

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "semantic_model": SEMANTIC_MODEL,
        "destruction_model": DESTRUCTION_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probe": dynamic_probe_summary,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
