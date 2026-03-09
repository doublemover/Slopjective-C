#!/usr/bin/env python3
"""Fail-closed checker for M255-B002 selector resolution semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-b002-selector-resolution-ambiguity-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-selector-resolution-ambiguity/m255-b002-v1"
PREVIOUS_CONTRACT_ID = "objc3c-dispatch-legality-selector-resolution/m255-b001-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-B002" / "selector_resolution_ambiguity_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_selector_resolution_positive.objc3"
MISSING_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_selector_resolution_missing_selector.objc3"
AMBIGUOUS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_selector_resolution_ambiguous_signature.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "b002-selector-resolution"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m255_selector_resolution_ambiguity_and_overload_rules_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_b002_selector_resolution_ambiguity_and_overload_rules_core_feature_implementation_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
SEMA = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m255_b002_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m255_b002_selector_resolution_ambiguity_and_overload_rules_core_feature_implementation.py"


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
        SnippetCheck("M255-B002-DOC-EXP-01", "# M255 Selector Resolution, Ambiguity, And Overload Rules Core Feature Implementation Expectations (B002)"),
        SnippetCheck("M255-B002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M255-B002-DOC-EXP-03", "missing concrete selector => `O3S216`"),
        SnippetCheck("M255-B002-DOC-EXP-04", "ambiguous concrete selector / incompatible concrete declarations => `O3S217`"),
        SnippetCheck("M255-B002-DOC-EXP-05", "m255_selector_resolution_positive.objc3"),
    ),
    PACKET_DOC: (
        SnippetCheck("M255-B002-DOC-PKT-01", "# M255-B002 Selector Resolution, Ambiguity, And Overload Rules Core Feature Implementation Packet"),
        SnippetCheck("M255-B002-DOC-PKT-02", "Dependencies: `M255-B001`, `M255-A002`"),
        SnippetCheck("M255-B002-DOC-PKT-03", "Missing concrete selectors must fail closed with"),
        SnippetCheck("M255-B002-DOC-PKT-04", f"Contract ID: `{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M255-B002-NDOC-01", "## Selector resolution and ambiguity rejection (M255-B002)"),
        SnippetCheck("M255-B002-NDOC-02", f"contract id `{CONTRACT_ID}`"),
        SnippetCheck("M255-B002-NDOC-03", "self-super-known-class-receivers-resolve-concretely"),
        SnippetCheck("M255-B002-NDOC-04", "m255_selector_resolution_ambiguous_signature.objc3"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M255-B002-SPC-01", "## M255 selector resolution and ambiguity implementation (B002)"),
        SnippetCheck("M255-B002-SPC-02", f"contract id `{CONTRACT_ID}`"),
        SnippetCheck("M255-B002-SPC-03", "non-concrete-receivers-remain-runtime-dynamic"),
        SnippetCheck("M255-B002-SPC-04", "`O3S217`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M255-B002-META-01", "## M255 selector-resolution metadata anchors (B002)"),
        SnippetCheck("M255-B002-META-02", f"contract id `{CONTRACT_ID}`"),
        SnippetCheck("M255-B002-META-03", "diagnostic anchors `O3S216` and `O3S217`"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M255-B002-HDR-01", "kObjc3SelectorResolutionImplementationContractId"),
        SnippetCheck("M255-B002-HDR-02", '"self-super-known-class-receivers-resolve-concretely"'),
        SnippetCheck("M255-B002-HDR-03", '"no-overload-recovery-exact-signature-or-fail-closed"'),
        SnippetCheck("M255-B002-HDR-04", '"O3S217"'),
    ),
    PARSER: (
        SnippetCheck("M255-B002-PARSE-01", "M255-B002 selector-resolution implementation anchor"),
        SnippetCheck("M255-B002-PARSE-02", "exact concrete self/super/known-class"),
    ),
    IR_CPP: (
        SnippetCheck("M255-B002-IR-01", "M255-B002 selector-resolution implementation anchor"),
        SnippetCheck("M255-B002-IR-02", "fail-closed exact-signature result"),
    ),
    RUNTIME_SHIM: (
        SnippetCheck("M255-B002-SHIM-01", "M255-B002 selector-resolution implementation"),
        SnippetCheck("M255-B002-SHIM-02", "does not perform selector lookup, ambiguity recovery, or overload"),
    ),
    SEMA: (
        SnippetCheck("M255-B002-SEMA-01", "ResolveConcreteMessageSendMethod("),
        SnippetCheck("M255-B002-SEMA-02", '"O3S216"'),
        SnippetCheck("M255-B002-SEMA-03", '"O3S217"'),
        SnippetCheck("M255-B002-SEMA-04", "ResolveConcreteOwnerMethod("),
    ),
    POSITIVE_FIXTURE: (
        SnippetCheck("M255-B002-FIX-POS-01", "module selectorResolutionPositive;"),
        SnippetCheck("M255-B002-FIX-POS-02", "return [self ping];"),
        SnippetCheck("M255-B002-FIX-POS-03", "return [super ping];"),
        SnippetCheck("M255-B002-FIX-POS-04", "return [Widget shared];"),
        SnippetCheck("M255-B002-FIX-POS-05", "[local ping];"),
    ),
    MISSING_FIXTURE: (
        SnippetCheck("M255-B002-FIX-MISS-01", "module selectorResolutionMissingSelector;"),
        SnippetCheck("M255-B002-FIX-MISS-02", "return [self missing];"),
    ),
    AMBIGUOUS_FIXTURE: (
        SnippetCheck("M255-B002-FIX-AMB-01", "module selectorResolutionAmbiguousSignature;"),
        SnippetCheck("M255-B002-FIX-AMB-02", "- (bool) ping"),
        SnippetCheck("M255-B002-FIX-AMB-03", "return [self ping];"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M255-B002-PKG-01", '"check:objc3c:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation"'),
        SnippetCheck("M255-B002-PKG-02", '"test:tooling:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation"'),
        SnippetCheck("M255-B002-PKG-03", '"check:objc3c:m255-b002-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M255-B002-RUN-01", "check:objc3c:m255-b001-lane-b-readiness"),
        SnippetCheck("M255-B002-RUN-02", "check:objc3c:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation"),
    ),
    TEST_FILE: (
        SnippetCheck("M255-B002-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M255-B002-TEST-02", CONTRACT_ID),
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
    )


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run_compile_case(*, native_exe: Path, fixture: Path, out_dir: Path, expect_success: bool, required_diagnostics: Sequence[str]) -> tuple[int, list[Finding], dict[str, object]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(native_exe.exists(), display_path(native_exe), "M255-B002-NATIVE-EXISTS", "native binary is missing", failures)
    checks_total += require(fixture.exists(), display_path(fixture), "M255-B002-FIXTURE-EXISTS", "fixture is missing", failures)
    if failures:
        return checks_total, failures, {}

    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_process([
        str(native_exe),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    combined = completed.stdout + "\n" + completed.stderr
    manifest_path = out_dir / "module.manifest.json"
    backend_path = out_dir / "module.object-backend.txt"
    diagnostics_text_path = out_dir / "module.diagnostics.txt"
    diagnostics_json_path = out_dir / "module.diagnostics.json"
    diagnostics_text = (
        diagnostics_text_path.read_text(encoding="utf-8")
        if diagnostics_text_path.exists()
        else ""
    )
    diagnostics_json = (
        json.loads(diagnostics_json_path.read_text(encoding="utf-8"))
        if diagnostics_json_path.exists()
        else None
    )
    combined = combined + "\n" + diagnostics_text
    if expect_success:
        checks_total += require(completed.returncode == 0, display_path(out_dir), "M255-B002-COMPILE-SUCCESS", "positive fixture must compile successfully", failures)
        checks_total += require(manifest_path.exists(), display_path(manifest_path), "M255-B002-MANIFEST", "positive fixture manifest is missing", failures)
        checks_total += require(backend_path.exists(), display_path(backend_path), "M255-B002-BACKEND", "positive fixture backend marker is missing", failures)
        if backend_path.exists():
            backend_text = backend_path.read_text(encoding="utf-8").strip()
            checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M255-B002-BACKEND-TEXT", "positive fixture must stay on llvm-direct", failures)
        checks_total += require("O3S216" not in combined and "O3S217" not in combined, display_path(out_dir), "M255-B002-NO-RESOLUTION-DIAGS", "positive fixture emitted selector-resolution diagnostics", failures)
    else:
        checks_total += require(completed.returncode != 0, display_path(out_dir), "M255-B002-COMPILE-FAIL", "negative fixture must fail compilation", failures)
        for diagnostic in required_diagnostics:
            checks_total += require(diagnostic in combined, display_path(out_dir), f"M255-B002-DIAG-{diagnostic}", f"expected diagnostic {diagnostic} not found", failures)

    payload = {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "returncode": completed.returncode,
        "manifest_exists": manifest_path.exists(),
        "backend_exists": backend_path.exists(),
        "diagnostics_text": diagnostics_text,
        "diagnostics_json": diagnostics_json,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, failures, payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    checks_total = 0
    failures: list[Finding] = []

    for path, snippets in STATIC_SNIPPETS.items():
        count, found = check_static_contract(path, snippets)
        checks_total += count
        failures.extend(found)

    dynamic_payload: dict[str, object] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        positive_checks, positive_failures, positive_payload = run_compile_case(
            native_exe=NATIVE_EXE,
            fixture=POSITIVE_FIXTURE,
            out_dir=PROBE_ROOT / "positive",
            expect_success=True,
            required_diagnostics=(),
        )
        checks_total += positive_checks
        failures.extend(positive_failures)

        missing_checks, missing_failures, missing_payload = run_compile_case(
            native_exe=NATIVE_EXE,
            fixture=MISSING_FIXTURE,
            out_dir=PROBE_ROOT / "missing-selector",
            expect_success=False,
            required_diagnostics=("O3S216",),
        )
        checks_total += missing_checks
        failures.extend(missing_failures)

        ambiguous_checks, ambiguous_failures, ambiguous_payload = run_compile_case(
            native_exe=NATIVE_EXE,
            fixture=AMBIGUOUS_FIXTURE,
            out_dir=PROBE_ROOT / "ambiguous-signature",
            expect_success=False,
            required_diagnostics=("O3S217",),
        )
        checks_total += ambiguous_checks
        failures.extend(ambiguous_failures)

        dynamic_payload = {
            "skipped": False,
            "positive": positive_payload,
            "missing_selector": missing_payload,
            "ambiguous_signature": ambiguous_payload,
        }

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "previous_contract_id": PREVIOUS_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "findings": [finding.__dict__ for finding in failures],
        "concrete_receiver_policy": "self-super-known-class-receivers-resolve-concretely",
        "dynamic_fallback_policy": "non-concrete-receivers-remain-runtime-dynamic",
        "overload_policy": "no-overload-recovery-exact-signature-or-fail-closed",
        "diagnostics": {
            "missing_selector": "O3S216",
            "ambiguous_selector": "O3S217",
        },
        "dynamic_probes": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        print(canonical_json(summary), file=sys.stderr, end="")
        return 1
    print(canonical_json(summary), end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
