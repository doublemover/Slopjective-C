#!/usr/bin/env python3
"""Checker for M272-C002 direct/sealed/final dispatch lowering."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m272-c002-part9-direct-sealed-final-dispatch-lowering-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m272" / "M272-C002" / "direct_sealed_final_dispatch_lowering_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m272_direct_sealed_and_final_dispatch_lowering_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m272" / "m272_c002_direct_sealed_and_final_dispatch_lowering_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_c002_dispatch_lowering_positive.objc3"
CONTRACT_ID = "objc3c-part9-dispatch-control-lowering-contract/m272-c001-v1"
EXPECTED_PACKET = {
    "contract_id": CONTRACT_ID,
    "surface_path": "frontend.pipeline.semantic_surface.objc_part9_dispatch_control_lowering_contract",
    "semantic_contract_id": "objc3c-part9-dynamism-dispatch-control-semantic-model/m272-b001-v1",
    "legality_contract_id": "objc3c-part9-override-finality-sealing-legality/m272-b002-v1",
    "compatibility_contract_id": "objc3c-part9-dynamism-control-compatibility-diagnostics/m272-b003-v1",
    "lane_contract_id": "m272-part9-dispatch-control-lowering-contract-v1",
    "direct_call_candidate_sites": 6,
    "direct_members_defaulted_sites": 4,
    "dynamic_opt_out_sites": 2,
    "final_container_sites": 1,
    "sealed_container_sites": 1,
    "override_legality_sites": 0,
    "metadata_preserved_callable_sites": 4,
    "metadata_preserved_container_sites": 1,
    "guard_blocked_sites": 0,
    "contract_violation_sites": 0,
}
IR_REQUIRED_SNIPPETS = [
    "define i32 @main() {",
    "call i32 @objc3_method_PolicyBox_class_callers()",
    "define i32 @objc3_method_PolicyBox_class_callers() {",
    "call i32 @objc3_method_PolicyBox_class_implicitDirect()",
    "call i32 @objc3_runtime_dispatch_i32(",
    "call i32 @objc3_method_PolicyBox_class_explicitDirect()",
    "{ i64, ptr, ptr, [4 x { ptr, ptr, ptr, i64, ptr, i64, i1, i1 }] }",
    "@__objc3_meta_class_0000 = private global { { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } }",
    "i64 1, i1 1, i1 0",
    "i1 1, i1 1",
]


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
        failures.append(Finding(display_path(path), "M272-C002-MISSING", f"missing artifact: {display_path(path)}"))
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def packet_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part9_dispatch_control_lowering_contract"]


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_PACKET.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M272-C002-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M272-C002-PAYLOAD-30", "replay_key missing", failures)
    total += 1
    passed += require(payload.get("deterministic_handoff") is True, artifact, "M272-C002-PAYLOAD-31", "deterministic_handoff must stay true", failures)
    total += 1
    passed += require(payload.get("ready_for_ir_emission") is True, artifact, "M272-C002-PAYLOAD-32", "ready_for_ir_emission must stay true", failures)
    return total, passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m272-c002-readiness",
        "--summary-out",
        "tmp/reports/m272/M272-C002/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M272-C002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M272-C002-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m272" / "c002" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    object_path = out_dir / "module.obj"
    total += 1
    passed += require(run.returncode == 0, display_path(FIXTURE), "M272-C002-DYN-03", f"positive fixture failed: {output}", failures)
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M272-C002-DYN-04", "manifest missing", failures)
    total += 1
    passed += require(ir_path.exists(), display_path(ir_path), "M272-C002-DYN-05", "module.ll missing", failures)
    total += 1
    passed += require(object_path.exists(), display_path(object_path), "M272-C002-DYN-06", "module.obj missing", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "manifest": display_path(manifest_path),
        "ir_path": display_path(ir_path),
        "object_path": display_path(object_path),
    }
    if manifest_path.exists():
        payload = packet_payload(manifest_path)
        sub_total, sub_passed = validate_payload(payload, display_path(manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["dispatch_control_lowering_contract"] = payload
    if ir_path.exists():
        ir_text = read_text(ir_path)
        hits: list[str] = []
        for index, snippet in enumerate(IR_REQUIRED_SNIPPETS, start=40):
            present = snippet in ir_text
            total += 1
            passed += require(present, display_path(ir_path), f"M272-C002-IR-{index}", f"missing IR snippet: {snippet}", failures)
            if present:
                hits.append(snippet)
        dynamic["ir_snippet_hits"] = hits
    return total, passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M272-C002-EXP-01", "# M272 Direct, Sealed, And Final Dispatch Lowering Expectations (C002)"),
            SnippetCheck("M272-C002-EXP-02", "Lower effective `objc_direct` sends on concrete `self` receivers as exact LLVM direct calls."),
            SnippetCheck("M272-C002-EXP-03", "Preserve `objc_dynamic` opt-out sites on the runtime dispatch entrypoint."),
        ],
        PACKET_DOC: [
            SnippetCheck("M272-C002-PKT-01", "# M272-C002 Packet: Direct, Sealed, And Final Dispatch Lowering - Core Feature Implementation"),
            SnippetCheck("M272-C002-PKT-02", "lower effective `objc_direct` sends on concrete `self` and known class receivers as exact LLVM direct calls"),
            SnippetCheck("M272-C002-PKT-03", "stable evidence under `tmp/reports/m272/M272-C002/`"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M272-C002-GRM-01", "## M272 direct, final, and sealed dispatch lowering"),
            SnippetCheck("M272-C002-GRM-02", "effective `objc_direct` sends on concrete `self` and known class receivers now"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M272-C002-DOC-01", "## M272 direct, final, and sealed dispatch lowering"),
            SnippetCheck("M272-C002-DOC-02", "effective `objc_direct` sends on concrete `self` and known class receivers now"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M272-C002-ATTR-01", "## M272 direct, final, and sealed dispatch lowering (C002)"),
            SnippetCheck("M272-C002-ATTR-02", "emitted method-list payloads now preserve:"),
        ],
        SPEC_DECISIONS: [
            SnippetCheck("M272-C002-DEC-01", "## D-030: Part 9 direct/final/sealed lowering executes direct sites only on the concrete supported slice"),
            SnippetCheck("M272-C002-DEC-02", "effective `objc_direct` sends on concrete `self` receivers, and"),
            SnippetCheck("M272-C002-DEC-03", "any new public runtime ABI."),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M272-C002-TYP-01", "bool objc_final_declared = false;"),
            SnippetCheck("M272-C002-TYP-02", "bool objc_sealed_declared = false;"),
            SnippetCheck("M272-C002-TYP-03", "bool effective_direct_dispatch = false;"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M272-C002-PIP-01", "node.effective_direct_dispatch ="),
            SnippetCheck("M272-C002-PIP-02", "class_node.objc_final_declared = aggregate.objc_final_declared;"),
            SnippetCheck("M272-C002-PIP-03", "class_node.objc_sealed_declared = aggregate.objc_sealed_declared;"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M272-C002-ART-01", "entry.effective_direct_dispatch ="),
            SnippetCheck("M272-C002-ART-02", "bundle.objc_final_declared = class_it->second->objc_final_declared;"),
            SnippetCheck("M272-C002-ART-03", "bundle.objc_sealed_declared = class_it->second->objc_sealed_declared;"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M272-C002-LWR-01", "M272-C002 dispatch-control lowering anchor: class/metaclass payloads now"),
            SnippetCheck("M272-C002-LWR-02", "and-final-sealed-flags"),
            SnippetCheck("M272-C002-LWR-03", "and-direct-final-flags"),
        ],
        IR_EMITTER: [
            SnippetCheck("M272-C002-IRSRC-01", "BuildDirectDispatchMethodKey("),
            SnippetCheck("M272-C002-IRSRC-02", "TryResolveDirectDispatchSymbol("),
            SnippetCheck("M272-C002-IRSRC-03", "concrete self/known-class"),
            SnippetCheck("M272-C002-IRSRC-04", "entry.effective_direct_dispatch"),
            SnippetCheck("M272-C002-IRSRC-05", "bundle.objc_final_declared"),
            SnippetCheck("M272-C002-IRSRC-06", "bundle.objc_sealed_declared"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M272-C002-PKG-01", '"check:objc3c:m272-c002-direct-sealed-and-final-dispatch-lowering-core-feature-implementation"'),
            SnippetCheck("M272-C002-PKG-02", '"test:tooling:m272-c002-direct-sealed-and-final-dispatch-lowering-core-feature-implementation"'),
            SnippetCheck("M272-C002-PKG-03", '"check:objc3c:m272-c002-lane-c-readiness"'),
        ],
    }
    for path, checks in snippets.items():
        total += len(checks)
        passed += ensure_snippets(path, checks, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        total += dyn_total
        passed += dyn_passed

    ok = not failures
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "ok": ok,
        "total_checks": total,
        "passed_checks": passed,
        "failed_checks": total - passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M272-C002 checker passed ({passed}/{total} checks)")
        return 0
    print(json.dumps(payload, indent=2))
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
