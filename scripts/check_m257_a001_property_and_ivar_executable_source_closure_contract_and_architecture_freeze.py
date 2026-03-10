#!/usr/bin/env python3
"""Validate M257-A001 property/ivar executable source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-a001-property-ivar-executable-source-closure-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-executable-property-ivar-source-closure/m257-a001-v1"
SOURCE_SURFACE_MODEL = "property-ivar-executable-source-closure-freezes-decls-synthesis-bindings-and-accessor-selectors-before-storage-realization"
EVIDENCE_MODEL = "class-protocol-property-ivar-fixture-manifest-and-ir-replay-key"
FAILURE_MODEL = "fail-closed-on-property-ivar-source-surface-drift-before-layout-and-accessor-expansion"
NEXT_ISSUE = "M257-A002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m257" / "M257-A001" / "property_ivar_executable_source_closure_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m257" / "a001-property-ivar-source-closure"
EXPECTED_COUNTS = {
    "property_synthesis_sites": 2,
    "property_synthesis_explicit_ivar_bindings": 0,
    "property_synthesis_default_ivar_bindings": 2,
    "ivar_binding_sites": 2,
    "ivar_binding_resolved": 2,
    "ivar_binding_missing": 0,
    "ivar_binding_conflicts": 0,
}
EXPECTED_REPLAY_KEY = (
    "property_synthesis_sites=2;property_synthesis_explicit_ivar_bindings=0;"
    "property_synthesis_default_ivar_bindings=2;ivar_binding_sites=2;"
    "ivar_binding_resolved=2;ivar_binding_missing=0;ivar_binding_conflicts=0;"
    "deterministic=true;lane_contract=m154-property-synthesis-ivar-binding-v1"
)

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m257_property_and_ivar_executable_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m257" / "m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m257_a001_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py"


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
        SnippetCheck("M257-A001-DOC-EXP-01", "# M257 Property And Ivar Executable Source Closure Contract And Architecture Freeze Expectations (A001)"),
        SnippetCheck("M257-A001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M257-A001-DOC-EXP-03", "`frontend.pipeline.sema_pass_manager.lowering_property_synthesis_ivar_binding_replay_key`"),
        SnippetCheck("M257-A001-DOC-EXP-04", "The contract must explicitly hand off to `M257-A002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M257-A001-DOC-PKT-01", "# M257-A001 Property And Ivar Executable Source Closure Contract And Architecture Freeze Packet"),
        SnippetCheck("M257-A001-DOC-PKT-02", "Packet: `M257-A001`"),
        SnippetCheck("M257-A001-DOC-PKT-03", "Issue: `#7145`"),
        SnippetCheck("M257-A001-DOC-PKT-04", "- `frontend.pipeline.sema_pass_manager.lowering_property_synthesis_ivar_binding_replay_key`"),
        SnippetCheck("M257-A001-DOC-PKT-05", "`M257-A002` is the explicit next handoff after this freeze closes."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M257-A001-NDOC-01", "## Property and ivar executable source closure (M257-A001)"),
        SnippetCheck("M257-A001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-A001-NDOC-03", f"`{SOURCE_SURFACE_MODEL}`"),
        SnippetCheck("M257-A001-NDOC-04", "tmp/reports/m257/M257-A001/property_ivar_executable_source_closure_summary.json"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M257-A001-SPC-01", "## M257 property and ivar executable source closure (A001)"),
        SnippetCheck("M257-A001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-A001-SPC-03", f"`{FAILURE_MODEL}`"),
        SnippetCheck("M257-A001-SPC-04", "`M257-A002`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M257-A001-META-01", "## M257 property/ivar executable source closure metadata anchors (A001)"),
        SnippetCheck("M257-A001-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-A001-META-03", "`Objc3PropertyDecl.ivar_binding_symbol`"),
        SnippetCheck("M257-A001-META-04", "`tmp/reports/m257/M257-A001/property_ivar_executable_source_closure_summary.json`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M257-A001-ARCH-01", "## M257 property and ivar executable source closure (A001)"),
        SnippetCheck("M257-A001-ARCH-02", "`M257-A001` freezes the source-surface handoff for executable properties and ivars:"),
        SnippetCheck("M257-A001-ARCH-03", "check:objc3c:m257-a001-lane-a-readiness"),
    ),
    AST_HEADER: (
        SnippetCheck("M257-A001-AST-01", "kObjc3ExecutablePropertyIvarSourceClosureContractId"),
        SnippetCheck("M257-A001-AST-02", "kObjc3ExecutablePropertyIvarSourceSurfaceModel"),
        SnippetCheck("M257-A001-AST-03", "Objc3PropertyDecl.ivar_binding_symbol"),
    ),
    SEMA_CPP: (
        SnippetCheck("M257-A001-SEMA-01", "M257-A001 property-ivar executable-source-closure anchor"),
        SnippetCheck("M257-A001-SEMA-02", "BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff"),
    ),
    IR_CPP: (
        SnippetCheck("M257-A001-IR-01", "M257-A001 property-ivar executable-source-closure anchor"),
        SnippetCheck("M257-A001-IR-02", "property_synthesis_ivar_binding_lowering = "),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M257-A001-PKG-01", '"check:objc3c:m257-a001-property-and-ivar-executable-source-closure": "python scripts/check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py"'),
        SnippetCheck("M257-A001-PKG-02", '"test:tooling:m257-a001-property-and-ivar-executable-source-closure": "python -m pytest tests/tooling/test_check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M257-A001-PKG-03", '"check:objc3c:m257-a001-lane-a-readiness": "python scripts/run_m257_a001_lane_a_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M257-A001-RUN-01", "build:objc3c-native"),
        SnippetCheck("M257-A001-RUN-02", "test_check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py"),
        SnippetCheck("M257-A001-RUN-03", "check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M257-A001-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M257-A001-TEST-02", CONTRACT_ID),
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
    return parser.parse_args(argv)


def run_dynamic_probe(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}
    out_dir = PROBE_ROOT / "class_protocol_property_ivar"
    out_dir.mkdir(parents=True, exist_ok=True)

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M257-A001-DYN-01", "native binary is missing", failures)
    checks_total += require(FIXTURE.exists(), display_path(FIXTURE), "M257-A001-DYN-02", "fixture is missing", failures)
    if failures:
        return checks_total, payload

    completed = run_process([
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    backend_path = out_dir / "module.object-backend.txt"
    diagnostics_path = out_dir / "module.diagnostics.json"

    checks_total += require(completed.returncode == 0, display_path(out_dir), "M257-A001-DYN-03", f"compile failed: {completed.stdout}{completed.stderr}", failures)
    for check_id, path in (
        ("M257-A001-DYN-04", manifest_path),
        ("M257-A001-DYN-05", ir_path),
        ("M257-A001-DYN-06", backend_path),
        ("M257-A001-DYN-07", diagnostics_path),
    ):
        checks_total += require(path.exists(), display_path(path), check_id, f"missing dynamic artifact: {display_path(path)}", failures)
    if completed.returncode != 0 or not all(path.exists() for path in (manifest_path, ir_path, backend_path, diagnostics_path)):
        return checks_total, {
            "fixture": display_path(FIXTURE),
            "out_dir": display_path(out_dir),
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }

    manifest = load_json(manifest_path)
    diagnostics = load_json(diagnostics_path)
    ir_text = ir_path.read_text(encoding="utf-8")
    backend_text = backend_path.read_text(encoding="utf-8").strip()
    frontend = manifest.get("frontend", {}) if isinstance(manifest, dict) else {}
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    sema = pipeline.get("sema_pass_manager", {}) if isinstance(pipeline, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    property_surface = semantic_surface.get("objc_property_synthesis_ivar_binding_surface", {}) if isinstance(semantic_surface, dict) else {}

    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M257-A001-DYN-08", "property/ivar source closure must stay on llvm-direct", failures)
    checks_total += require(diagnostics.get("diagnostics") == [], display_path(diagnostics_path), "M257-A001-DYN-09", "fixture must remain diagnostics-clean", failures)
    checks_total += require(isinstance(sema, dict), display_path(manifest_path), "M257-A001-DYN-10", "manifest sema_pass_manager payload is missing", failures)
    checks_total += require(isinstance(property_surface, dict), display_path(manifest_path), "M257-A001-DYN-11", "manifest property/ivar semantic surface is missing", failures)
    if isinstance(sema, dict) and isinstance(property_surface, dict):
        for key, expected_value in EXPECTED_COUNTS.items():
            checks_total += require(sema.get(key) == expected_value, display_path(manifest_path), f"M257-A001-DYN-SEMA-{key.upper()}", f"sema field {key} mismatch: expected {expected_value}, observed {sema.get(key)}", failures)
            checks_total += require(property_surface.get(key) == expected_value, display_path(manifest_path), f"M257-A001-DYN-SURFACE-{key.upper()}", f"surface field {key} mismatch: expected {expected_value}, observed {property_surface.get(key)}", failures)
        checks_total += require(sema.get("deterministic_property_synthesis_ivar_binding_handoff") is True, display_path(manifest_path), "M257-A001-DYN-12", "sema deterministic property/ivar handoff must remain true", failures)
        checks_total += require(property_surface.get("deterministic_handoff") is True, display_path(manifest_path), "M257-A001-DYN-13", "semantic surface deterministic handoff must remain true", failures)
        checks_total += require(sema.get("lowering_property_synthesis_ivar_binding_replay_key") == EXPECTED_REPLAY_KEY, display_path(manifest_path), "M257-A001-DYN-14", "sema replay key drifted", failures)
        checks_total += require(property_surface.get("replay_key") == EXPECTED_REPLAY_KEY, display_path(manifest_path), "M257-A001-DYN-15", "semantic surface replay key drifted", failures)
        checks_total += require(sema.get("lowering_property_synthesis_ivar_binding_replay_key") == property_surface.get("replay_key"), display_path(manifest_path), "M257-A001-DYN-16", "sema and semantic surface replay keys must match", failures)
    checks_total += require(f"; property_synthesis_ivar_binding_lowering = {EXPECTED_REPLAY_KEY}" in ir_text, display_path(ir_path), "M257-A001-DYN-17", "IR replay key line drifted", failures)

    payload = {
        "fixture": display_path(FIXTURE),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "ir_path": display_path(ir_path),
        "backend_path": display_path(backend_path),
        "diagnostics_path": display_path(diagnostics_path),
        "backend_text": backend_text,
        "expected_counts": EXPECTED_COUNTS,
        "observed_replay_key": property_surface.get("replay_key") if isinstance(property_surface, dict) else None,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, snippets in STATIC_SNIPPETS.items():
        count, findings = check_static_contract(path, snippets)
        checks_total += count
        failures.extend(findings)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dynamic_count, dynamic_payload = run_dynamic_probe(failures)
        checks_total += dynamic_count

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "source_surface_model": SOURCE_SURFACE_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "dynamic_probe": dynamic_payload,
        "failures": [finding.__dict__ for finding in failures],
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        json.dump(summary_payload, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1

    json.dump(summary_payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
