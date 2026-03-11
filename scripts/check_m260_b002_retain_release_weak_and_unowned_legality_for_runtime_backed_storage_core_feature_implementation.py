#!/usr/bin/env python3
"""Validate M260-B002 runtime-backed storage ownership legality."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-b002-runtime-backed-storage-ownership-legality-v1"
CONTRACT_ID = "objc3c-runtime-backed-storage-ownership-legality/m260-b002-v1"
OWNED_STORAGE_MODEL = (
    "explicit-strong-object-property-qualifiers-remain-legal-for-owned-runtime-backed-storage-while-conflicting-weak-or-unowned-modifiers-fail-closed"
)
WEAK_UNOWNED_MODEL = (
    "explicit-weak-and-unsafe-unretained-object-property-qualifiers-bind-runtime-backed-storage-legality-and-reject-conflicting-property-modifiers"
)
FAILURE_MODEL = "fail-closed-on-runtime-backed-object-property-ownership-qualifier-modifier-drift"
NEXT_ISSUE = "M260-B003"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-B002" / "runtime_backed_storage_ownership_legality_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_retain_release_weak_and_unowned_legality_for_runtime_backed_storage_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_b002_retain_release_weak_and_unowned_legality_for_runtime_backed_storage_core_feature_implementation_packet.md"
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
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_storage_ownership_legality_positive.objc3"
NEGATIVE_WEAK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_storage_ownership_weak_mismatch_negative.objc3"
NEGATIVE_UNOWNED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_storage_ownership_unowned_mismatch_negative.objc3"
POSITIVE_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "b002" / "positive"
NEGATIVE_WEAK_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "b002" / "negative-weak-mismatch"
NEGATIVE_UNOWNED_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "b002" / "negative-unowned-mismatch"


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
    parser.add_argument("--positive-fixture", type=Path, default=POSITIVE_FIXTURE)
    parser.add_argument("--negative-weak-fixture", type=Path, default=NEGATIVE_WEAK_FIXTURE)
    parser.add_argument("--negative-unowned-fixture", type=Path, default=NEGATIVE_UNOWNED_FIXTURE)
    parser.add_argument("--positive-out-dir", type=Path, default=POSITIVE_OUT_DIR)
    parser.add_argument("--negative-weak-out-dir", type=Path, default=NEGATIVE_WEAK_OUT_DIR)
    parser.add_argument("--negative-unowned-out-dir", type=Path, default=NEGATIVE_UNOWNED_OUT_DIR)
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


def combined_output(result: subprocess.CompletedProcess[str]) -> str:
    return ((result.stdout or "") + "\n" + (result.stderr or "")).strip()


def diagnostics_text(out_dir: Path) -> str:
    diagnostics_txt = out_dir / "module.diagnostics.txt"
    if not diagnostics_txt.exists():
        return ""
    return read_text(diagnostics_txt)


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
            SnippetCheck("M260-B002-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M260-B002-DOC-02", "Issue: `#7171`"),
            SnippetCheck("M260-B002-DOC-03", "`tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_legality_positive.objc3`"),
            SnippetCheck("M260-B002-DOC-04", "`tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_unowned_mismatch_negative.objc3`"),
            SnippetCheck("M260-B002-DOC-05", "`M260-B003`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M260-B002-PKT-01", "Packet: `M260-B002`"),
            SnippetCheck("M260-B002-PKT-02", "Issue: `#7171`"),
            SnippetCheck("M260-B002-PKT-03", "Dependencies: `M260-B001`, `M260-A002`"),
            SnippetCheck("M260-B002-PKT-04", "m260_runtime_backed_storage_ownership_weak_mismatch_negative.objc3"),
            SnippetCheck("M260-B002-PKT-05", "`M260-B003`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M260-B002-SRC-01", "## M260 runtime-backed storage ownership legality (B002)"),
            SnippetCheck("M260-B002-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B002-SRC-03", "explicit `__unsafe_unretained` property qualifiers may target"),
            SnippetCheck("M260-B002-SRC-04", "`M260-B003` is the next issue"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M260-B002-NDOC-01", "## M260 runtime-backed storage ownership legality (B002)"),
            SnippetCheck("M260-B002-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B002-NDOC-03", "explicit `__weak` property qualifiers may target runtime-backed object"),
            SnippetCheck("M260-B002-NDOC-04", "`M260-B003` is the next issue"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.architecture_doc,
        (
            SnippetCheck("M260-B002-ARCH-01", "## M260 Runtime-Backed Storage Ownership Legality (B002)"),
            SnippetCheck("M260-B002-ARCH-02", "explicit `__unsafe_unretained` property qualifiers are legal only with"),
            SnippetCheck("M260-B002-ARCH-03", "not interchangeable with explicit `__unsafe_unretained`"),
            SnippetCheck("M260-B002-ARCH-04", "the next issue is `M260-B003`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M260-B002-SPC-01", "## M260 runtime-backed storage ownership legality (B002)"),
            SnippetCheck("M260-B002-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B002-SPC-03", f"`{OWNED_STORAGE_MODEL}`"),
            SnippetCheck("M260-B002-SPC-04", f"`{WEAK_UNOWNED_MODEL}`"),
            SnippetCheck("M260-B002-SPC-05", f"`{FAILURE_MODEL}`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M260-B002-META-01", "## M260 runtime-backed storage ownership legality metadata anchors (B002)"),
            SnippetCheck("M260-B002-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B002-META-03", "tmp/artifacts/compilation/objc3c-native/m260/b002/positive/module.manifest.json"),
            SnippetCheck("M260-B002-META-04", "`M260-B003` is the next issue"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.sema_source,
        (
            SnippetCheck("M260-B002-SEMA-01", "M260-B002 runtime-backed storage ownership legality anchor: explicit"),
            SnippetCheck("M260-B002-SEMA-02", "`__weak`, `__unsafe_unretained`, and `__strong` qualifiers now participate"),
            SnippetCheck("M260-B002-SEMA-03", "property ownership qualifier '"),
            SnippetCheck("M260-B002-SEMA-04", "@property modifier 'assign' or no explicit ownership modifier"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.ir_source,
        (
            SnippetCheck("M260-B002-IR-01", "M260-B002 runtime-backed storage ownership legality anchor: explicit"),
            SnippetCheck("M260-B002-IR-02", "runtime_backed_storage_ownership_legality"),
            SnippetCheck("M260-B002-IR-03", "owned/weak/"),
            SnippetCheck("M260-B002-IR-04", "Objc3RuntimeBackedStorageOwnershipLegalitySummary()"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_header,
        (
            SnippetCheck("M260-B002-LHDR-01", "kObjc3RuntimeBackedStorageOwnershipLegalityContractId"),
            SnippetCheck("M260-B002-LHDR-02", OWNED_STORAGE_MODEL),
            SnippetCheck("M260-B002-LHDR-03", WEAK_UNOWNED_MODEL),
            SnippetCheck("M260-B002-LHDR-04", FAILURE_MODEL),
            SnippetCheck("M260-B002-LHDR-05", "Objc3RuntimeBackedStorageOwnershipLegalitySummary"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.lowering_source,
        (
            SnippetCheck("M260-B002-LOW-01", "M260-B002 runtime-backed storage ownership legality anchor: explicit"),
            SnippetCheck("M260-B002-LOW-02", "ownership qualifiers on Objective-C object properties now participate"),
            SnippetCheck("M260-B002-LOW-03", "Weak and unsafe-unretained qualifiers must agree"),
            SnippetCheck("M260-B002-LOW-04", "kObjc3RuntimeBackedStorageOwnershipFailClosedModel"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M260-B002-PKG-01", '"check:objc3c:m260-b002-runtime-backed-storage-ownership-legality":'),
            SnippetCheck("M260-B002-PKG-02", '"test:tooling:m260-b002-runtime-backed-storage-ownership-legality":'),
            SnippetCheck("M260-B002-PKG-03", '"check:objc3c:m260-b002-lane-b-readiness":'),
            SnippetCheck("M260-B002-PKG-04", 'python scripts/run_m260_b002_lane_b_readiness.py'),
            SnippetCheck("M260-B002-PKG-05", '"compile:objc3c":'),
        ),
        failures,
    )

    if not args.skip_dynamic_probes:
        dynamic_probe_summary["executed"] = True

        positive_result = run_compile(args.compile_script, args.positive_fixture, args.positive_out_dir)
        positive_manifest_path = args.positive_out_dir / "module.manifest.json"
        positive_ir_path = args.positive_out_dir / "module.ll"

        negative_weak_result = run_compile(args.compile_script, args.negative_weak_fixture, args.negative_weak_out_dir)
        negative_unowned_result = run_compile(args.compile_script, args.negative_unowned_fixture, args.negative_unowned_out_dir)

        dynamic_probe_summary["positive"] = {
            "returncode": positive_result.returncode,
            "stdout": positive_result.stdout.strip(),
            "stderr": positive_result.stderr.strip(),
            "manifest": display_path(positive_manifest_path),
            "ir": display_path(positive_ir_path),
        }
        dynamic_probe_summary["negative_weak"] = {
            "returncode": negative_weak_result.returncode,
            "stdout": negative_weak_result.stdout.strip(),
            "stderr": negative_weak_result.stderr.strip(),
            "diagnostics": display_path(args.negative_weak_out_dir / "module.diagnostics.txt"),
        }
        dynamic_probe_summary["negative_unowned"] = {
            "returncode": negative_unowned_result.returncode,
            "stdout": negative_unowned_result.stdout.strip(),
            "stderr": negative_unowned_result.stderr.strip(),
            "diagnostics": display_path(args.negative_unowned_out_dir / "module.diagnostics.txt"),
        }

        checks_total += 16
        checks_passed += require(positive_result.returncode == 0, display_path(args.compile_script), "M260-B002-DYN-01", "positive runtime-backed storage legality fixture compile failed", failures)
        checks_passed += require(positive_manifest_path.exists(), display_path(positive_manifest_path), "M260-B002-DYN-02", "positive manifest artifact missing", failures)
        checks_passed += require(positive_ir_path.exists(), display_path(positive_ir_path), "M260-B002-DYN-03", "positive IR artifact missing", failures)

        manifest_payload = json.loads(read_text(positive_manifest_path)) if positive_manifest_path.exists() else {}
        ir_text = read_text(positive_ir_path) if positive_ir_path.exists() else ""
        property_records = (((manifest_payload.get("runtime_metadata_source_records") or {}).get("properties")) or [])

        def has_property(name: str, lifetime: str, runtime_hook: str | None = None) -> bool:
            for record in property_records:
                if record.get("property_name") != name:
                    continue
                if record.get("ownership_lifetime_profile") != lifetime:
                    continue
                if runtime_hook is not None and record.get("ownership_runtime_hook_profile") != runtime_hook:
                    continue
                return True
            return False

        checks_passed += require(has_property("currentValue", "strong-owned"), display_path(positive_manifest_path), "M260-B002-DYN-04", "strong-owned runtime-backed property metadata missing", failures)
        checks_passed += require(has_property("copiedValue", "strong-owned"), display_path(positive_manifest_path), "M260-B002-DYN-05", "copy-backed owned property metadata missing", failures)
        checks_passed += require(has_property("weakValue", "weak", "objc-weak-side-table"), display_path(positive_manifest_path), "M260-B002-DYN-06", "weak runtime-backed property metadata missing", failures)
        checks_passed += require(has_property("borrowedValue", "unowned-unsafe", "objc-unowned-unsafe-direct"), display_path(positive_manifest_path), "M260-B002-DYN-07", "unsafe-unretained runtime-backed property metadata missing", failures)
        checks_passed += require(has_property("guardedValue", "unowned-safe", "objc-unowned-safe-guard"), display_path(positive_manifest_path), "M260-B002-DYN-08", "safe unowned runtime-backed property metadata missing", failures)
        checks_passed += require(
            f"; runtime_backed_storage_ownership_legality = contract={CONTRACT_ID};owned_storage_model={OWNED_STORAGE_MODEL};weak_unowned_model={WEAK_UNOWNED_MODEL};failure_model={FAILURE_MODEL}" in ir_text,
            display_path(positive_ir_path),
            "M260-B002-DYN-09",
            "IR runtime-backed storage ownership legality summary missing",
            failures,
        )

        negative_weak_output = diagnostics_text(args.negative_weak_out_dir) or combined_output(negative_weak_result)
        negative_unowned_output = diagnostics_text(args.negative_unowned_out_dir) or combined_output(negative_unowned_result)
        checks_passed += require(negative_weak_result.returncode != 0, display_path(args.negative_weak_fixture), "M260-B002-DYN-10", "weak mismatch negative fixture unexpectedly compiled", failures)
        checks_passed += require("O3S206" in negative_weak_output, display_path(args.negative_weak_fixture), "M260-B002-DYN-11", "weak mismatch diagnostic code missing", failures)
        checks_passed += require(
            "property ownership qualifier '__weak' conflicts with @property ownership modifier 'assign'" in negative_weak_output,
            display_path(args.negative_weak_fixture),
            "M260-B002-DYN-12",
            "weak mismatch diagnostic message missing",
            failures,
        )
        checks_passed += require(negative_unowned_result.returncode != 0, display_path(args.negative_unowned_fixture), "M260-B002-DYN-13", "unowned mismatch negative fixture unexpectedly compiled", failures)
        checks_passed += require("O3S206" in negative_unowned_output, display_path(args.negative_unowned_fixture), "M260-B002-DYN-14", "unowned mismatch diagnostic code missing", failures)
        checks_passed += require(
            "property ownership qualifier '__unsafe_unretained' conflicts with @property ownership modifier 'unowned'" in negative_unowned_output,
            display_path(args.negative_unowned_fixture),
            "M260-B002-DYN-15",
            "unowned mismatch diagnostic message missing",
            failures,
        )
        checks_passed += require(
            "guardedValue" in read_text(positive_manifest_path) if positive_manifest_path.exists() else False,
            display_path(positive_manifest_path),
            "M260-B002-DYN-16",
            "positive manifest does not retain the safe-unowned proof property",
            failures,
        )
    else:
        dynamic_probe_summary["skipped"] = True

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "owned_storage_model": OWNED_STORAGE_MODEL,
        "weak_unowned_model": WEAK_UNOWNED_MODEL,
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
