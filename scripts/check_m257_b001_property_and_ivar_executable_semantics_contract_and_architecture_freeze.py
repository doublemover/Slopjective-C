#!/usr/bin/env python3
"""Validate M257-B001 property and ivar executable semantics freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-b001-property-and-ivar-executable-semantics-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-executable-property-ivar-semantics/m257-b001-v1"
SYNTHESIS_MODEL = "non-category-class-interface-properties-own-deterministic-implicit-ivar-and-synthesized-binding-identities-until-explicit-synthesize-lands"
ACCESSOR_MODEL = "readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission"
STORAGE_MODEL = "interface-owned-property-layout-slots-sizes-and-alignment-remain-deterministic-before-runtime-allocation"
COMPATIBILITY_MODEL = "protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols"
FAILURE_MODEL = "fail-closed-on-property-runtime-semantic-boundary-drift-before-accessor-body-or-storage-realization"
NEXT_ISSUE = "M257-B002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m257" / "M257-B001" / "property_ivar_executable_semantics_contract_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_ivar_source_model_completion_positive.objc3"
PROTOCOL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m257" / "b001-property-ivar-executable-semantics"
EXPECTED_REPLAY_KEY = (
    "property_attribute_profiles=6;accessor_ownership_profiles=6;"
    "synthesized_bindings=6;ivar_layout_entries=3;deterministic=true;"
    "lane_contract=m257-property-ivar-source-model-v1"
)

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m257_property_and_ivar_executable_semantics_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m257" / "m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m257_b001_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py"


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
        SnippetCheck("M257-B001-DOC-EXP-01", "# M257 Property And Ivar Executable Semantics Contract And Architecture Freeze Expectations (B001)"),
        SnippetCheck("M257-B001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M257-B001-DOC-EXP-03", "The second probe must stay protocol-compatible after the `M257-A002`"),
        SnippetCheck("M257-B001-DOC-EXP-04", "The contract must explicitly hand off to `M257-B002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M257-B001-DOC-PKT-01", "# M257-B001 Property And Ivar Executable Semantics Contract And Architecture Freeze Packet"),
        SnippetCheck("M257-B001-DOC-PKT-02", "Packet: `M257-B001`"),
        SnippetCheck("M257-B001-DOC-PKT-03", "Issue: `#7147`"),
        SnippetCheck("M257-B001-DOC-PKT-04", "- `kObjc3ExecutablePropertyCompatibilitySemanticsModel`"),
        SnippetCheck("M257-B001-DOC-PKT-05", "`M257-B002` is the explicit next handoff after this freeze closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M257-B001-DOCSRC-01", "## Property and ivar executable semantics (M257-B001)"),
        SnippetCheck("M257-B001-DOCSRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B001-DOCSRC-03", "`tmp/reports/m257/M257-B001/property_ivar_executable_semantics_contract_summary.json`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M257-B001-NDOC-01", "## Property and ivar executable semantics (M257-B001)"),
        SnippetCheck("M257-B001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B001-NDOC-03", "`tmp/reports/m257/M257-B001/property_ivar_executable_semantics_contract_summary.json`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M257-B001-SPC-01", "## M257 property and ivar executable semantics (B001)"),
        SnippetCheck("M257-B001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B001-SPC-03", f"`{FAILURE_MODEL}`"),
        SnippetCheck("M257-B001-SPC-04", "`M257-B002`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M257-B001-META-01", "## M257 property/ivar executable semantics metadata anchors (B001)"),
        SnippetCheck("M257-B001-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B001-META-03", "`kObjc3ExecutablePropertyCompatibilitySemanticsModel`"),
        SnippetCheck("M257-B001-META-04", "`tmp/reports/m257/M257-B001/property_ivar_executable_semantics_contract_summary.json`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M257-B001-ARCH-01", "## M257 property and ivar executable semantics (B001)"),
        SnippetCheck("M257-B001-ARCH-02", "`M257-B001` freezes the runtime-meaningful semantic rules above the completed `M257-A002` source model:"),
        SnippetCheck("M257-B001-ARCH-03", "check:objc3c:m257-b001-lane-b-readiness"),
    ),
    AST_HEADER: (
        SnippetCheck("M257-B001-AST-01", "kObjc3ExecutablePropertyIvarSemanticsContractId"),
        SnippetCheck("M257-B001-AST-02", "kObjc3ExecutablePropertySynthesisSemanticsModel"),
        SnippetCheck("M257-B001-AST-03", "kObjc3ExecutablePropertyCompatibilitySemanticsModel"),
    ),
    SEMA_CPP: (
        SnippetCheck("M257-B001-SEMA-01", "M257-B001 property-ivar executable semantics anchor"),
        SnippetCheck("M257-B001-SEMA-02", "runtime-meaningful property compatibility preserves declaration-level attribute"),
    ),
    IR_CPP: (
        SnippetCheck("M257-B001-IR-01", "M257-B001 property-ivar executable semantics anchor"),
        SnippetCheck("M257-B001-IR-02", "property_ivar_source_model_completion = "),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M257-B001-PKG-01", '"check:objc3c:m257-b001-property-and-ivar-executable-semantics": "python scripts/check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py"'),
        SnippetCheck("M257-B001-PKG-02", '"test:tooling:m257-b001-property-and-ivar-executable-semantics": "python -m pytest tests/tooling/test_check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M257-B001-PKG-03", '"check:objc3c:m257-b001-lane-b-readiness": "python scripts/run_m257_b001_lane_b_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M257-B001-RUN-01", "build:objc3c-native"),
        SnippetCheck("M257-B001-RUN-02", "test_check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py"),
        SnippetCheck("M257-B001-RUN-03", "check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M257-B001-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M257-B001-TEST-02", CONTRACT_ID),
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


def probe_fixture(fixture: Path, out_dir: Path, failures: list[Finding], probe_id: str) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}
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
    ir_path = out_dir / "module.ll"
    backend_path = out_dir / "module.object-backend.txt"
    diagnostics_path = out_dir / "module.diagnostics.json"

    checks_total += require(completed.returncode == 0, display_path(out_dir), f"{probe_id}-COMPILE", f"compile failed: {completed.stdout}{completed.stderr}", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), f"{probe_id}-MANIFEST", "manifest is missing", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), f"{probe_id}-BACKEND", "backend marker is missing", failures)
    checks_total += require(diagnostics_path.exists(), display_path(diagnostics_path), f"{probe_id}-DIAGNOSTICS", "diagnostics json is missing", failures)
    if completed.returncode != 0 or not all(path.exists() for path in (manifest_path, backend_path, diagnostics_path)):
        return checks_total, payload

    backend_text = backend_path.read_text(encoding="utf-8").strip()
    diagnostics = load_json(diagnostics_path)
    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), f"{probe_id}-LLVM-DIRECT", "probe must stay on llvm-direct", failures)
    checks_total += require(diagnostics.get("diagnostics") == [], display_path(diagnostics_path), f"{probe_id}-DIAG-CLEAN", "probe must remain diagnostics-clean", failures)

    payload = {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "ir_path": display_path(ir_path) if ir_path.exists() else None,
        "backend_path": display_path(backend_path),
        "diagnostics_path": display_path(diagnostics_path),
        "backend_text": backend_text,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, payload


def run_dynamic_probe(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M257-B001-DYN-01", "native binary is missing", failures)
    checks_total += require(POSITIVE_FIXTURE.exists(), display_path(POSITIVE_FIXTURE), "M257-B001-DYN-02", "positive fixture is missing", failures)
    checks_total += require(PROTOCOL_FIXTURE.exists(), display_path(PROTOCOL_FIXTURE), "M257-B001-DYN-03", "protocol continuity fixture is missing", failures)
    if failures:
        return checks_total, payload

    positive_count, positive_payload = probe_fixture(
        POSITIVE_FIXTURE,
        PROBE_ROOT / "positive",
        failures,
        "M257-B001-POS",
    )
    checks_total += positive_count
    protocol_count, protocol_payload = probe_fixture(
        PROTOCOL_FIXTURE,
        PROBE_ROOT / "protocol-continuity",
        failures,
        "M257-B001-PRO",
    )
    checks_total += protocol_count

    if positive_payload:
        positive_manifest = load_json(ROOT / positive_payload["manifest_path"])
        positive_ir_path = ROOT / positive_payload["ir_path"] if positive_payload.get("ir_path") else None
        frontend = positive_manifest.get("frontend", {}) if isinstance(positive_manifest, dict) else {}
        pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
        sema = pipeline.get("sema_pass_manager", {}) if isinstance(pipeline, dict) else {}
        records = positive_manifest.get("runtime_metadata_source_records", {}) if isinstance(positive_manifest, dict) else {}
        property_records = records.get("properties", []) if isinstance(records, dict) else []
        ivar_records = records.get("ivars", []) if isinstance(records, dict) else []

        checks_total += require(sema.get("runtime_metadata_property_record_count") == 6, positive_payload["manifest_path"], "M257-B001-DYN-04", f"expected 6 property records, observed {sema.get('runtime_metadata_property_record_count')}", failures)
        checks_total += require(sema.get("runtime_metadata_ivar_record_count") == 3, positive_payload["manifest_path"], "M257-B001-DYN-05", f"expected 3 ivar records, observed {sema.get('runtime_metadata_ivar_record_count')}", failures)
        checks_total += require(len(property_records) == 6, positive_payload["manifest_path"], "M257-B001-DYN-06", f"expected 6 runtime property records, observed {len(property_records)}", failures)
        checks_total += require(len(ivar_records) == 3, positive_payload["manifest_path"], "M257-B001-DYN-07", f"expected 3 runtime ivar records, observed {len(ivar_records)}", failures)
        if positive_ir_path and positive_ir_path.exists():
            ir_text = positive_ir_path.read_text(encoding="utf-8")
            checks_total += require(f"; property_ivar_source_model_completion = {EXPECTED_REPLAY_KEY}" in ir_text, positive_payload["ir_path"], "M257-B001-DYN-08", "positive probe replay key drifted", failures)

    payload = {
        "positive_probe": positive_payload,
        "protocol_continuity_probe": protocol_payload,
        "expected_replay_key": EXPECTED_REPLAY_KEY,
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
        "synthesis_model": SYNTHESIS_MODEL,
        "accessor_model": ACCESSOR_MODEL,
        "storage_model": STORAGE_MODEL,
        "compatibility_model": COMPATIBILITY_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "dynamic_probe": dynamic_payload,
        "failures": [finding.__dict__ for finding in failures],
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary_payload), encoding="utf-8")

    json.dump(summary_payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
