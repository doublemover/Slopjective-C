#!/usr/bin/env python3
"""Fail-closed checker for M257-B002 property synthesis semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-b002-property-synthesis-default-ivar-binding-full-semantics-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-property-default-ivar-binding-semantics/m257-b002-v1"
PREVIOUS_CONTRACT_ID = "objc3c-executable-property-ivar-semantics/m257-b001-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m257" / "M257-B002" / "property_synthesis_default_ivar_binding_full_semantics_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m257" / "b002-property-synthesis-default-ivar-binding"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m257_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m257" / "m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m257_b002_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py"
NO_REDECL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_synthesis_default_ivar_binding_no_redeclaration.objc3"
REDECL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_ivar_source_model_completion_positive.objc3"
INCOMPATIBLE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_synthesis_default_ivar_binding_incompatible_redeclaration.objc3"
DEFAULT_BINDING_MODEL = "matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration"


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
        SnippetCheck("M257-B002-DOC-EXP-01", "# M257 Property Synthesis and Default Ivar Binding Full Semantics Core Feature Implementation Expectations (B002)"),
        SnippetCheck("M257-B002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M257-B002-DOC-EXP-03", "Issue: `#7148`"),
        SnippetCheck("M257-B002-DOC-EXP-04", "m257_property_synthesis_default_ivar_binding_no_redeclaration.objc3"),
        SnippetCheck("M257-B002-DOC-EXP-05", "Incompatible implementation redeclaration => `O3S206`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M257-B002-DOC-PKT-01", "# M257-B002 Property Synthesis and Default Ivar Binding Full Semantics Core Feature Implementation Packet"),
        SnippetCheck("M257-B002-DOC-PKT-02", "Packet: `M257-B002`"),
        SnippetCheck("M257-B002-DOC-PKT-03", "Dependencies: `M257-B001`, `M257-A002`"),
        SnippetCheck("M257-B002-DOC-PKT-04", "Next issue: `M257-B003`"),
        SnippetCheck("M257-B002-DOC-PKT-05", f"Contract ID: `{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M257-B002-NDOC-01", "## Property synthesis and default ivar binding semantics (M257-B002)"),
        SnippetCheck("M257-B002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B002-NDOC-03", "implementation redeclarations are optional and must stay signature/binding compatible"),
        SnippetCheck("M257-B002-NDOC-04", "`M257-B003`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M257-B002-SPC-01", "## M257 property synthesis and default ivar binding full semantics (B002)"),
        SnippetCheck("M257-B002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B002-SPC-03", "matched class implementations synthesize from interface-declared properties first"),
        SnippetCheck("M257-B002-SPC-04", "implementation redeclarations remain optional compatibility overlays"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M257-B002-META-01", "## M257 property/default-ivar-binding implementation metadata anchors (B002)"),
        SnippetCheck("M257-B002-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B002-META-03", "`Objc3PropertyInfo.ivar_binding_symbol`"),
        SnippetCheck("M257-B002-META-04", "`frontend.pipeline.sema_pass_manager.interface_owned_property_synthesis_sites`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M257-B002-ARCH-01", "## M257 property synthesis and default ivar binding full semantics (B002)"),
        SnippetCheck("M257-B002-ARCH-02", "matched class-interface properties as the authoritative"),
        SnippetCheck("M257-B002-ARCH-03", "check:objc3c:m257-b002-lane-b-readiness"),
    ),
    AST_HEADER: (
        SnippetCheck("M257-B002-AST-01", "kObjc3ExecutablePropertyDefaultIvarBindingResolutionModel"),
        SnippetCheck("M257-B002-AST-02", DEFAULT_BINDING_MODEL),
    ),
    SEMA_CPP: (
        SnippetCheck("M257-B002-SEMA-01", "M257-B002 property-synthesis enforcement anchor"),
        SnippetCheck("M257-B002-SEMA-02", "lacks an authoritative default ivar binding"),
        SnippetCheck("M257-B002-SEMA-03", "drifted from the interface default ivar binding"),
        SnippetCheck("M257-B002-SEMA-04", "FindPropertyInImplementationInfo("),
    ),
    IR_CPP: (
        SnippetCheck("M257-B002-IR-01", "M257-B002 property-synthesis implementation anchor"),
        SnippetCheck("M257-B002-IR-02", "interface-declared class properties even when the implementation does"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M257-B002-PKG-01", '"check:objc3c:m257-b002-property-synthesis-and-default-ivar-binding-full-semantics"'),
        SnippetCheck("M257-B002-PKG-02", '"test:tooling:m257-b002-property-synthesis-and-default-ivar-binding-full-semantics"'),
        SnippetCheck("M257-B002-PKG-03", '"check:objc3c:m257-b002-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M257-B002-RUN-01", "check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py"),
        SnippetCheck("M257-B002-RUN-02", "test_check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py"),
        SnippetCheck("M257-B002-RUN-03", "check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M257-B002-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M257-B002-TEST-02", CONTRACT_ID),
    ),
    NO_REDECL_FIXTURE: (
        SnippetCheck("M257-B002-FIX-01", "module propertySynthesisDefaultIvarBindingNoRedeclaration;"),
        SnippetCheck("M257-B002-FIX-02", "@property (readonly, getter=tokenValue) id token;"),
        SnippetCheck("M257-B002-FIX-03", "@property (nonatomic, getter=currentValue, setter=setCurrentValue:, strong) id value;"),
        SnippetCheck("M257-B002-FIX-04", "@implementation Widget"),
    ),
    REDECL_FIXTURE: (
        SnippetCheck("M257-B002-FIX-05", "module propertyIvarSourceModelCompletion;"),
        SnippetCheck("M257-B002-FIX-06", "@property (assign, setter=setCount:) i32 count;"),
    ),
    INCOMPATIBLE_FIXTURE: (
        SnippetCheck("M257-B002-FIX-07", "module propertySynthesisDefaultIvarBindingIncompatibleRedeclaration;"),
        SnippetCheck("M257-B002-FIX-08", "@property (readonly, getter=tokenValue) i32 token;"),
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


def compile_fixture(fixture: Path, out_dir: Path) -> tuple[subprocess.CompletedProcess[str], Path, Path, Path, Path]:
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_process([
        str(NATIVE_EXE),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    manifest_path = out_dir / "module.manifest.json"
    backend_path = out_dir / "module.object-backend.txt"
    diagnostics_text_path = out_dir / "module.diagnostics.txt"
    diagnostics_json_path = out_dir / "module.diagnostics.json"
    return completed, manifest_path, backend_path, diagnostics_text_path, diagnostics_json_path


def validate_positive_case(
    *,
    fixture: Path,
    out_dir: Path,
    expected_counts: dict[str, int],
    failures: list[Finding],
    probe_id: str,
) -> tuple[int, dict[str, object]]:
    checks_total = 0
    completed, manifest_path, backend_path, diagnostics_text_path, diagnostics_json_path = compile_fixture(fixture, out_dir)
    diagnostics_text = diagnostics_text_path.read_text(encoding="utf-8") if diagnostics_text_path.exists() else ""
    diagnostics_json = load_json(diagnostics_json_path) if diagnostics_json_path.exists() else {}

    checks_total += require(completed.returncode == 0, display_path(out_dir), f"{probe_id}-COMPILE", completed.stdout + completed.stderr + diagnostics_text, failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), f"{probe_id}-MANIFEST", "manifest is missing", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), f"{probe_id}-BACKEND", "backend marker is missing", failures)
    checks_total += require(diagnostics_json_path.exists(), display_path(diagnostics_json_path), f"{probe_id}-DIAGNOSTICS", "diagnostics json is missing", failures)
    if completed.returncode != 0 or not manifest_path.exists() or not backend_path.exists() or not diagnostics_json_path.exists():
        return checks_total, {}

    backend_text = backend_path.read_text(encoding="utf-8").strip()
    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), f"{probe_id}-LLVM-DIRECT", "probe must stay on llvm-direct", failures)
    checks_total += require(diagnostics_json.get("diagnostics") == [], display_path(diagnostics_json_path), f"{probe_id}-DIAG-CLEAN", "probe must remain diagnostics-clean", failures)

    manifest = load_json(manifest_path)
    sema = manifest.get("frontend", {}).get("pipeline", {}).get("sema_pass_manager", {})
    surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_property_synthesis_ivar_binding_surface", {})

    for key, expected in expected_counts.items():
        checks_total += require(sema.get(key) == expected, display_path(manifest_path), f"{probe_id}-SEMA-{key}", f"expected sema {key}={expected}, observed {sema.get(key)}", failures)
        checks_total += require(surface.get(key) == expected, display_path(manifest_path), f"{probe_id}-SURFACE-{key}", f"expected semantic surface {key}={expected}, observed {surface.get(key)}", failures)

    checks_total += require(sema.get("lowering_property_synthesis_ivar_binding_replay_key") == surface.get("replay_key"), display_path(manifest_path), f"{probe_id}-REPLAY", "sema and semantic surface replay keys must match", failures)

    payload = {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "backend_path": display_path(backend_path),
        "diagnostics_path": display_path(diagnostics_json_path),
        "expected_counts": expected_counts,
        "observed_counts": {key: sema.get(key) for key in expected_counts},
        "backend_text": backend_text,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, payload


def validate_negative_case(*, fixture: Path, out_dir: Path, expected_snippets: Sequence[str], failures: list[Finding], probe_id: str) -> tuple[int, dict[str, object]]:
    checks_total = 0
    completed, manifest_path, backend_path, diagnostics_text_path, diagnostics_json_path = compile_fixture(fixture, out_dir)
    diagnostics_text = diagnostics_text_path.read_text(encoding="utf-8") if diagnostics_text_path.exists() else ""
    combined = (completed.stdout or "") + "\n" + (completed.stderr or "") + "\n" + diagnostics_text
    checks_total += require(completed.returncode != 0, display_path(out_dir), f"{probe_id}-FAIL", "negative fixture must fail compilation", failures)
    for snippet in expected_snippets:
        checks_total += require(snippet in combined, display_path(out_dir), f"{probe_id}-{snippet}", f"expected diagnostic snippet not found: {snippet}", failures)
    payload = {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "manifest_exists": manifest_path.exists(),
        "backend_exists": backend_path.exists(),
        "diagnostics_exists": diagnostics_json_path.exists(),
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
        "diagnostics_text": diagnostics_text,
    }
    return checks_total, payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
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
        checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M257-B002-NATIVE", "native binary is missing", failures)
        if NATIVE_EXE.exists():
            no_redecl_checks, no_redecl_payload = validate_positive_case(
                fixture=NO_REDECL_FIXTURE,
                out_dir=PROBE_ROOT / "no-redeclaration",
                expected_counts={
                    "property_synthesis_sites": 2,
                    "property_synthesis_default_ivar_bindings": 2,
                    "interface_owned_property_synthesis_sites": 2,
                    "implementation_property_redeclaration_sites": 0,
                    "ivar_binding_resolved": 2,
                    "ivar_binding_missing": 0,
                    "ivar_binding_conflicts": 0,
                },
                failures=failures,
                probe_id="M257-B002-NO-REDECL",
            )
            checks_total += no_redecl_checks

            redecl_checks, redecl_payload = validate_positive_case(
                fixture=REDECL_FIXTURE,
                out_dir=PROBE_ROOT / "with-redeclaration",
                expected_counts={
                    "property_synthesis_sites": 3,
                    "property_synthesis_default_ivar_bindings": 3,
                    "interface_owned_property_synthesis_sites": 3,
                    "implementation_property_redeclaration_sites": 3,
                    "ivar_binding_resolved": 3,
                    "ivar_binding_missing": 0,
                    "ivar_binding_conflicts": 0,
                },
                failures=failures,
                probe_id="M257-B002-WITH-REDECL",
            )
            checks_total += redecl_checks

            incompatible_checks, incompatible_payload = validate_negative_case(
                fixture=INCOMPATIBLE_FIXTURE,
                out_dir=PROBE_ROOT / "incompatible-redeclaration",
                expected_snippets=("O3S206", "incompatible property signature for 'token' in implementation 'Widget'"),
                failures=failures,
                probe_id="M257-B002-INCOMPATIBLE",
            )
            checks_total += incompatible_checks

            dynamic_payload = {
                "skipped": False,
                "no_redeclaration": no_redecl_payload,
                "with_redeclaration": redecl_payload,
                "incompatible_redeclaration": incompatible_payload,
            }

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "previous_contract_id": PREVIOUS_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "default_binding_resolution_model": DEFAULT_BINDING_MODEL,
        "dynamic_probes": dynamic_payload,
        "failures": [finding.__dict__ for finding in failures],
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        print(canonical_json(payload), file=sys.stderr, end="")
        return 1
    print(canonical_json(payload), end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
