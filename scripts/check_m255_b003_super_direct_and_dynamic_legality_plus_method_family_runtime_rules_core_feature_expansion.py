#!/usr/bin/env python3
"""Fail-closed checker for M255-B003 super/direct/dynamic legality expansion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-b003-super-direct-dynamic-method-family-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-super-dynamic-method-family/m255-b003-v1"
PREVIOUS_CONTRACT_ID = "objc3c-selector-resolution-ambiguity/m255-b002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-B003" / "super_direct_dynamic_method_family_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_super_dynamic_method_family_edges.objc3"
SUPER_OUTSIDE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_super_outside_method.objc3"
SUPER_ROOT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_super_root_dispatch.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "b003-super-direct-dynamic"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m255_super_direct_and_dynamic_legality_plus_method_family_runtime_rules_core_feature_expansion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_b003_super_direct_and_dynamic_legality_plus_method_family_runtime_rules_core_feature_expansion_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
SEMA = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m255_b003_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m255_b003_super_direct_and_dynamic_legality_plus_method_family_runtime_rules_core_feature_expansion.py"

EXPECTED_DISPATCH_SURFACE = {
    "super_dispatch_sites": 4,
    "dynamic_dispatch_sites": 3,
    "direct_dispatch_sites": 0,
    "direct_entrypoint_family": "reserved-non-goal",
}
EXPECTED_METHOD_FAMILY = {
    "message_send_sites": 7,
    "receiver_super_identifier_sites": 4,
    "super_dispatch_enabled_sites": 4,
    "super_dispatch_requires_class_context_sites": 4,
    "method_family_init_sites": 1,
    "method_family_copy_sites": 2,
    "method_family_mutable_copy_sites": 1,
    "method_family_new_sites": 2,
    "method_family_none_sites": 1,
    "method_family_returns_retained_result_sites": 6,
    "method_family_returns_related_result_sites": 1,
    "contract_violation_sites": 0,
}
EXPECTED_RUNTIME_SHIM = {
    "message_send_sites": 7,
    "runtime_shim_required_sites": 7,
    "runtime_shim_elided_sites": 0,
    "runtime_dispatch_arg_slots": 4,
    "runtime_dispatch_declaration_parameter_count": 6,
    "runtime_dispatch_symbol": "objc3_msgsend_i32",
    "default_runtime_dispatch_symbol_binding": True,
    "contract_violation_sites": 0,
}


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
        SnippetCheck("M255-B003-DOC-EXP-01", "# M255 Super, Direct, And Dynamic Legality Plus Method-Family Runtime Rules Core Feature Expansion Expectations (B003)"),
        SnippetCheck("M255-B003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M255-B003-DOC-EXP-03", "`super` dispatch sites `4`"),
        SnippetCheck("M255-B003-DOC-EXP-04", "direct dispatch remains reserved/non-goal"),
    ),
    PACKET_DOC: (
        SnippetCheck("M255-B003-DOC-PKT-01", "# M255-B003 Super, Direct, And Dynamic Legality Plus Method-Family Runtime Rules Core Feature Expansion Packet"),
        SnippetCheck("M255-B003-DOC-PKT-02", "Dependencies: `M255-B002`"),
        SnippetCheck("M255-B003-DOC-PKT-03", "`tests/tooling/fixtures/native/m255_super_root_dispatch.objc3`"),
        SnippetCheck("M255-B003-DOC-PKT-04", f"Contract ID: `{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M255-B003-NDOC-01", "## Super, dynamic, and method-family legality expansion (M255-B003)"),
        SnippetCheck("M255-B003-NDOC-02", f"contract id `{CONTRACT_ID}`"),
        SnippetCheck("M255-B003-NDOC-03", "`super-requires-enclosing-method-and-real-superclass`"),
        SnippetCheck("M255-B003-NDOC-04", "`tests/tooling/fixtures/native/m255_super_root_dispatch.objc3`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M255-B003-SPC-01", "## M255 super/direct/dynamic legality and method-family expansion (B003)"),
        SnippetCheck("M255-B003-SPC-02", f"contract id `{CONTRACT_ID}`"),
        SnippetCheck("M255-B003-SPC-03", "`direct-dispatch-remains-reserved-non-goal`"),
        SnippetCheck("M255-B003-SPC-04", "root-class `super` dispatch also fails closed with `O3S216`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M255-B003-META-01", "## M255 super/direct/dynamic legality metadata anchors (B003)"),
        SnippetCheck("M255-B003-META-02", f"contract id `{CONTRACT_ID}`"),
        SnippetCheck("M255-B003-META-03", "`super-and-dynamic-sites-preserve-method-family-runtime-visibility`"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M255-B003-HDR-01", "kObjc3SuperDynamicMethodFamilyContractId"),
        SnippetCheck("M255-B003-HDR-02", '"super-requires-enclosing-method-and-real-superclass"'),
        SnippetCheck("M255-B003-HDR-03", '"direct-dispatch-remains-reserved-non-goal"'),
        SnippetCheck("M255-B003-HDR-04", '"dynamic-dispatch-preserves-runtime-resolution-and-method-family-accounting"'),
    ),
    PARSER: (
        SnippetCheck("M255-B003-PARSE-01", "M255-B003 super/direct/dynamic legality expansion anchor"),
        SnippetCheck("M255-B003-PARSE-02", "same raw dynamic receiver spellings for runtime method-family accounting"),
    ),
    IR_CPP: (
        SnippetCheck("M255-B003-IR-01", "M255-B003 super/direct/dynamic legality expansion anchor"),
        SnippetCheck("M255-B003-IR-02", "never synthesizes a reserved direct-dispatch entrypoint"),
    ),
    RUNTIME_SHIM: (
        SnippetCheck("M255-B003-SHIM-01", "M255-B003 super/direct/dynamic legality expansion"),
        SnippetCheck("M255-B003-SHIM-02", "method-family accounting"),
    ),
    SEMA: (
        SnippetCheck("M255-B003-SEMA-01", "M255-B003 super/direct/dynamic legality expansion anchor"),
        SnippetCheck("M255-B003-SEMA-02", "selector resolution failed: 'super' dispatch requires an enclosing implementation method"),
        SnippetCheck("M255-B003-SEMA-03", "selector resolution failed: implementation '"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M255-B003-FRONTEND-01", "M255-B003 parity-expansion anchor"),
        SnippetCheck("M255-B003-FRONTEND-02", "runtime-shim host-link validation stays in"),
    ),
    POSITIVE_FIXTURE: (
        SnippetCheck("M255-B003-FIX-POS-01", "module superDynamicMethodFamilyEdges;"),
        SnippetCheck("M255-B003-FIX-POS-02", "[super init];"),
        SnippetCheck("M255-B003-FIX-POS-03", "[(toggle ? local : 0) marker];"),
        SnippetCheck("M255-B003-FIX-POS-04", "return [super new];"),
    ),
    SUPER_OUTSIDE_FIXTURE: (
        SnippetCheck("M255-B003-FIX-OUT-01", "module superOutsideMethod;"),
        SnippetCheck("M255-B003-FIX-OUT-02", "[super ping];"),
    ),
    SUPER_ROOT_FIXTURE: (
        SnippetCheck("M255-B003-FIX-ROOT-01", "module superRootDispatch;"),
        SnippetCheck("M255-B003-FIX-ROOT-02", "[super ping];"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M255-B003-PKG-01", '"check:objc3c:m255-b003-super-direct-and-dynamic-legality-plus-method-family-runtime-rules-core-feature-expansion"'),
        SnippetCheck("M255-B003-PKG-02", '"test:tooling:m255-b003-super-direct-and-dynamic-legality-plus-method-family-runtime-rules-core-feature-expansion"'),
        SnippetCheck("M255-B003-PKG-03", '"check:objc3c:m255-b003-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M255-B003-RUN-01", "check:objc3c:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation"),
        SnippetCheck("M255-B003-RUN-02", "test:tooling:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation"),
        SnippetCheck("M255-B003-RUN-03", "check:objc3c:m255-b003-super-direct-and-dynamic-legality-plus-method-family-runtime-rules-core-feature-expansion"),
    ),
    TEST_FILE: (
        SnippetCheck("M255-B003-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M255-B003-TEST-02", CONTRACT_ID),
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


def load_manifest_payload(out_dir: Path) -> dict[str, object]:
    manifest_path = out_dir / "module.manifest.json"
    if not manifest_path.exists():
        return {}
    return json.loads(manifest_path.read_text(encoding="utf-8"))


def run_compile_case(*, fixture: Path, out_dir: Path) -> tuple[subprocess.CompletedProcess[str], str, dict[str, object]]:
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_process([
        str(NATIVE_EXE),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    diagnostics_path = out_dir / "module.diagnostics.txt"
    diagnostics_text = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.exists() else ""
    return completed, diagnostics_text, load_manifest_payload(out_dir)


def check_positive_probe(failures: list[Finding]) -> tuple[int, dict[str, object]]:
    checks_total = 0
    out_dir = PROBE_ROOT / "positive"
    completed, diagnostics_text, manifest = run_compile_case(fixture=POSITIVE_FIXTURE, out_dir=out_dir)
    combined = completed.stdout + "\n" + completed.stderr + "\n" + diagnostics_text
    backend_path = out_dir / "module.object-backend.txt"
    checks_total += require(completed.returncode == 0, display_path(out_dir), "M255-B003-POS-RC", "positive fixture must compile successfully", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), "M255-B003-POS-BACKEND", "positive fixture backend marker is missing", failures)
    if backend_path.exists():
        backend_text = backend_path.read_text(encoding="utf-8").strip()
        checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M255-B003-POS-BACKEND-TEXT", "positive fixture must stay on llvm-direct", failures)
    checks_total += require("O3S216" not in combined and "O3S217" not in combined, display_path(out_dir), "M255-B003-POS-DIAGS", "positive fixture emitted selector-resolution diagnostics", failures)

    surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}) if manifest else {}
    dispatch_surface = surface.get("objc_dispatch_surface_classification_surface", {})
    method_family_surface = surface.get("objc_super_dispatch_method_family_surface", {})
    runtime_shim_surface = surface.get("objc_runtime_shim_host_link_surface", {})

    for key, expected in EXPECTED_DISPATCH_SURFACE.items():
        checks_total += require(dispatch_surface.get(key) == expected, display_path(out_dir), f"M255-B003-DISPATCH-{key}", f"dispatch surface {key} expected {expected!r} got {dispatch_surface.get(key)!r}", failures)
    for key, expected in EXPECTED_METHOD_FAMILY.items():
        checks_total += require(method_family_surface.get(key) == expected, display_path(out_dir), f"M255-B003-METHOD-{key}", f"method-family surface {key} expected {expected!r} got {method_family_surface.get(key)!r}", failures)
    for key, expected in EXPECTED_RUNTIME_SHIM.items():
        checks_total += require(runtime_shim_surface.get(key) == expected, display_path(out_dir), f"M255-B003-SHIM-{key}", f"runtime-shim surface {key} expected {expected!r} got {runtime_shim_surface.get(key)!r}", failures)

    return checks_total, {
        "fixture": display_path(POSITIVE_FIXTURE),
        "out_dir": display_path(out_dir),
        "returncode": completed.returncode,
        "dispatch_surface": dispatch_surface,
        "method_family_surface": method_family_surface,
        "runtime_shim_surface": runtime_shim_surface,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
        "diagnostics_text": diagnostics_text,
    }


def check_negative_probe(name: str, fixture: Path, failures: list[Finding]) -> tuple[int, dict[str, object]]:
    checks_total = 0
    out_dir = PROBE_ROOT / name
    completed, diagnostics_text, manifest = run_compile_case(fixture=fixture, out_dir=out_dir)
    combined = completed.stdout + "\n" + completed.stderr + "\n" + diagnostics_text
    checks_total += require(completed.returncode != 0, display_path(out_dir), f"M255-B003-{name}-RC", f"{name} fixture must fail compilation", failures)
    checks_total += require("O3S216" in combined, display_path(out_dir), f"M255-B003-{name}-O3S216", f"{name} fixture must fail with O3S216", failures)
    return checks_total, {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
        "diagnostics_text": diagnostics_text,
        "manifest_exists": bool(manifest),
    }


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
        positive_checks, positive_payload = check_positive_probe(failures)
        checks_total += positive_checks
        outside_checks, outside_payload = check_negative_probe("super_outside_method", SUPER_OUTSIDE_FIXTURE, failures)
        checks_total += outside_checks
        root_checks, root_payload = check_negative_probe("super_root_dispatch", SUPER_ROOT_FIXTURE, failures)
        checks_total += root_checks
        dynamic_payload = {
            "skipped": False,
            "positive": positive_payload,
            "super_outside_method": outside_payload,
            "super_root_dispatch": root_payload,
        }

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "previous_contract_id": PREVIOUS_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "findings": [finding.__dict__ for finding in failures],
        "policies": {
            "super_legality": "super-requires-enclosing-method-and-real-superclass",
            "direct_dispatch": "direct-dispatch-remains-reserved-non-goal",
            "dynamic_dispatch": "dynamic-dispatch-preserves-runtime-resolution-and-method-family-accounting",
            "runtime_visible_method_family": "super-and-dynamic-sites-preserve-method-family-runtime-visibility",
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
