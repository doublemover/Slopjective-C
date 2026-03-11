#!/usr/bin/env python3
"""Validate M260-A001 runtime-backed object ownership surface freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-a001-runtime-backed-object-ownership-surface-freeze-v1"
CONTRACT_ID = "objc3c-runtime-backed-object-ownership-surface-freeze/m260-a001-v1"
SURFACE_MODEL = "runtime-backed-object-ownership-surface-freezes-property-accessor-and-legacy-lowering-ownership-profiles-before-live-arc-runtime-semantics"
EVIDENCE_MODEL = "canonical-runnable-sample-manifest-and-ir-ownership-profile-proof"
FAILURE_MODEL = "fail-closed-on-ownership-surface-drift-or-premature-arc-runnable-claim"
NEXT_ISSUE = "M260-A002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-A001" / "runtime_backed_object_ownership_surface_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_a001_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PACKAGE_JSON = ROOT / "package.json"
COMPILE_SCRIPT = ROOT / "scripts" / "objc3c_native_compile.ps1"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_a002_canonical_runnable_sample_set.objc3"
PROBE_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "a001" / "positive"


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
            SnippetCheck("M260-A001-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M260-A001-DOC-02", "Issue: `#7168`"),
            SnippetCheck("M260-A001-DOC-03", "`tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`"),
            SnippetCheck("M260-A001-DOC-04", "executable function/method ownership qualifiers remain fail-closed outside the runnable slice"),
            SnippetCheck("M260-A001-DOC-05", "`M260-A002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M260-A001-PKT-01", "Packet: `M260-A001`"),
            SnippetCheck("M260-A001-PKT-02", "Issue: `#7168`"),
            SnippetCheck("M260-A001-PKT-03", "Dependencies: none"),
            SnippetCheck("M260-A001-PKT-04", "Live ARC retain/release/autorelease runtime behavior does not land here."),
            SnippetCheck("M260-A001-PKT-05", "`M260-A002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M260-A001-SRC-01", "## M260 runtime-backed object ownership surface freeze (A001)"),
            SnippetCheck("M260-A001-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-A001-SRC-03", "`tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`"),
            SnippetCheck("M260-A001-SRC-04", "strong/copy/weak object-member ownership remains declaration/profile level"),
            SnippetCheck("M260-A001-SRC-05", "`M260-A002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M260-A001-NDOC-01", "## M260 runtime-backed object ownership surface freeze (A001)"),
            SnippetCheck("M260-A001-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-A001-NDOC-03", "scalar `assign` properties already publish"),
            SnippetCheck("M260-A001-NDOC-04", "live ARC retain/release/autorelease runtime semantics do not land here"),
            SnippetCheck("M260-A001-NDOC-05", "`M260-A002`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.architecture_doc,
        (
            SnippetCheck("M260-A001-ARCH-01", "## M260 Runtime-Backed Object Ownership Surface Freeze (A001)"),
            SnippetCheck("M260-A001-ARCH-02", "emitted runtime-backed object artifacts already preserve property/accessor"),
            SnippetCheck("M260-A001-ARCH-03", "legacy ownership qualifier, retain/release,"),
            SnippetCheck("M260-A001-ARCH-04", "`M260-A002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M260-A001-SPC-01", "## M260 runtime-backed object ownership surface freeze (A001)"),
            SnippetCheck("M260-A001-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-A001-SPC-03", f"`{SURFACE_MODEL}`"),
            SnippetCheck("M260-A001-SPC-04", f"`{FAILURE_MODEL}`"),
            SnippetCheck("M260-A001-SPC-05", "`M260-A002`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M260-A001-META-01", "## M260 runtime-backed object ownership metadata anchors (A001)"),
            SnippetCheck("M260-A001-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-A001-META-03", "`tmp/artifacts/compilation/objc3c-native/m260/a001/positive/module.manifest.json`"),
            SnippetCheck("M260-A001-META-04", "`M260-A002`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        args.sema_source,
        (
            SnippetCheck("M260-A001-SEMA-01", "M260-A001 runtime-backed-object-ownership freeze anchor:"),
            SnippetCheck("M260-A001-SEMA-02", "ownership qualifiers on executable functions/methods remain fail-closed"),
            SnippetCheck("M260-A001-SEMA-03", "property/member ownership profiles are already preserved"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        args.ir_source,
        (
            SnippetCheck("M260-A001-IR-01", "M260-A001 runtime-backed-object-ownership freeze anchor:"),
            SnippetCheck("M260-A001-IR-02", "property/accessor"),
            SnippetCheck("M260-A001-IR-03", "live ARC runtime retain/release/autorelease execution hooks"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        args.lowering_source,
        (
            SnippetCheck("M260-A001-LOW-01", "M260-A001 runtime-backed-object-ownership freeze anchor:"),
            SnippetCheck("M260-A001-LOW-02", "truthful runtime-backed"),
            SnippetCheck("M260-A001-LOW-03", "function/method ownership qualifiers are already runnable"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M260-A001-PKG-01", '"check:objc3c:m260-a001-ownership-surface-closure-for-runtime-backed-objects":'),
            SnippetCheck("M260-A001-PKG-02", '"test:tooling:m260-a001-ownership-surface-closure-for-runtime-backed-objects":'),
            SnippetCheck("M260-A001-PKG-03", '"check:objc3c:m260-a001-lane-a-readiness":'),
            SnippetCheck("M260-A001-PKG-04", '"check:objc3c:m260-a001-lane-a-readiness": "python scripts/run_m260_a001_lane_a_readiness.py"'),
            SnippetCheck("M260-A001-PKG-05", '"compile:objc3c":'),
        ),
        failures,
    )

    if not args.skip_dynamic_probes:
        dynamic_probe_summary["executed"] = True
        result = run_compile(args.compile_script, args.fixture, args.probe_out_dir)
        dynamic_probe_summary["returncode"] = result.returncode
        dynamic_probe_summary["stdout"] = result.stdout.strip()
        dynamic_probe_summary["stderr"] = result.stderr.strip()
        checks_total += 12
        checks_passed += require(result.returncode == 0, display_path(args.compile_script), "M260-A001-DYN-01", "canonical runnable sample compile failed", failures)
        manifest_path = args.probe_out_dir / "module.manifest.json"
        ir_path = args.probe_out_dir / "module.ll"
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M260-A001-DYN-02", "manifest artifact missing", failures)
        checks_passed += require(ir_path.exists(), display_path(ir_path), "M260-A001-DYN-03", "IR artifact missing", failures)
        manifest_text = read_text(manifest_path) if manifest_path.exists() else ""
        ir_text = read_text(ir_path) if ir_path.exists() else ""
        checks_passed += require('"property_attribute_profile":"readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;strong=1;weak=0;unowned=0;assign=0;attributes=getter=currentValue,nonatomic,setter=setCurrentValue:,strong"' in manifest_text, display_path(manifest_path), "M260-A001-DYN-04", "strong object-property ownership profile missing", failures)
        checks_passed += require('"property_attribute_profile":"readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=0;assign=1;attributes=assign,setter=setCount:"' in manifest_text, display_path(manifest_path), "M260-A001-DYN-05", "assign property ownership profile missing", failures)
        checks_passed += require('"ownership_lifetime_profile":"unowned-unsafe"' in manifest_text, display_path(manifest_path), "M260-A001-DYN-06", "unowned-unsafe lifetime profile missing", failures)
        checks_passed += require('"ownership_runtime_hook_profile":"objc-unowned-unsafe-direct"' in manifest_text, display_path(manifest_path), "M260-A001-DYN-07", "unowned-unsafe runtime hook profile missing", failures)
        checks_passed += require('"lowering_retain_release_operation":{"replay_key":"ownership_qualified_sites=0;retain_insertion_sites=0;release_insertion_sites=0;autorelease_insertion_sites=0;contract_violation_sites=0;deterministic=true;lane_contract=m162-retain-release-operation-lowering-v1"' in manifest_text, display_path(manifest_path), "M260-A001-DYN-08", "retain/release lowering summary drifted", failures)
        checks_passed += require('"lowering_weak_unowned_semantics":{"replay_key":"ownership_candidate_sites=4;weak_reference_sites=0;unowned_reference_sites=4;unowned_safe_reference_sites=0;weak_unowned_conflict_sites=0;contract_violation_sites=0;deterministic=true;lane_contract=m164-weak-unowned-semantics-lowering-v1"' in manifest_text, display_path(manifest_path), "M260-A001-DYN-09", "weak/unowned lowering summary drifted", failures)
        checks_passed += require("; ownership_qualifier_lowering = ownership_qualifier_sites=0;invalid_ownership_qualifier_sites=0;object_pointer_type_annotation_sites=4;deterministic=true;lane_contract=m161-ownership-qualifier-lowering-v1" in ir_text, display_path(ir_path), "M260-A001-DYN-10", "IR ownership qualifier summary missing", failures)
        checks_passed += require("; retain_release_operation_lowering = ownership_qualified_sites=0;retain_insertion_sites=0;release_insertion_sites=0;autorelease_insertion_sites=0;contract_violation_sites=0;deterministic=true;lane_contract=m162-retain-release-operation-lowering-v1" in ir_text, display_path(ir_path), "M260-A001-DYN-11", "IR retain/release summary missing", failures)
        checks_passed += require("; weak_unowned_semantics_lowering = ownership_candidate_sites=4;weak_reference_sites=0;unowned_reference_sites=4;unowned_safe_reference_sites=0;weak_unowned_conflict_sites=0;contract_violation_sites=0;deterministic=true;lane_contract=m164-weak-unowned-semantics-lowering-v1" in ir_text, display_path(ir_path), "M260-A001-DYN-12", "IR weak/unowned summary missing", failures)
        dynamic_probe_summary["manifest"] = display_path(manifest_path)
        dynamic_probe_summary["ir"] = display_path(ir_path)
    else:
        dynamic_probe_summary["skipped"] = True

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_model": SURFACE_MODEL,
        "evidence_model": EVIDENCE_MODEL,
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
