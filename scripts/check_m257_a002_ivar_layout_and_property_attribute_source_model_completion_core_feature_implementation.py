#!/usr/bin/env python3
"""Validate M257-A002 ivar layout and property attribute source-model completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-a002-ivar-layout-and-property-attribute-source-model-completion-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-executable-property-ivar-source-model-completion/m257-a002-v1"
LAYOUT_MODEL = "property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization"
ATTRIBUTE_MODEL = "property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles"
EVIDENCE_MODEL = "property-layout-fixture-manifest-and-ir-replay-key"
FAILURE_MODEL = "fail-closed-on-property-attribute-accessor-ownership-or-layout-drift-before-storage-realization"
NEXT_ISSUE = "M257-B001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m257" / "M257-A002" / "property_ivar_source_model_completion_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_ivar_source_model_completion_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m257" / "a002-property-ivar-source-model-completion"
EXPECTED_REPLAY_KEY = (
    "property_attribute_profiles=6;accessor_ownership_profiles=6;"
    "synthesized_bindings=6;ivar_layout_entries=3;deterministic=true;"
    "lane_contract=m257-property-ivar-source-model-v1"
)
EXPECTED_PROPERTY_COUNTS = {
    "property_declaration_entries": 6,
    "property_attribute_entries": 16,
    "property_getter_selector_entries": 4,
    "property_setter_selector_entries": 4,
    "runtime_metadata_property_record_count": 6,
    "runtime_metadata_ivar_record_count": 3,
}
EXPECTED_PROPERTY_RECORDS: dict[tuple[str, str], dict[str, Any]] = {
    ("class-interface", "token"): {
        "property_attribute_profile": "readonly=1;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=0;assign=0;attributes=getter=tokenValue,readonly",
        "effective_getter_selector": "tokenValue",
        "effective_setter_available": False,
        "effective_setter_selector": "",
        "accessor_ownership_profile": "getter=tokenValue;setter_available=0;setter=<none>;ownership_lifetime=;runtime_hook=",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:token",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:token",
        "executable_ivar_layout_slot_index": 0,
        "executable_ivar_layout_size_bytes": 8,
        "executable_ivar_layout_alignment_bytes": 8,
    },
    ("class-interface", "value"): {
        "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;strong=1;weak=0;unowned=0;assign=0;attributes=getter=currentValue,nonatomic,setter=setCurrentValue:,strong",
        "effective_getter_selector": "currentValue",
        "effective_setter_available": True,
        "effective_setter_selector": "setCurrentValue:",
        "accessor_ownership_profile": "getter=currentValue;setter_available=1;setter=setCurrentValue:;ownership_lifetime=;runtime_hook=",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:value",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:value",
        "executable_ivar_layout_slot_index": 1,
        "executable_ivar_layout_size_bytes": 8,
        "executable_ivar_layout_alignment_bytes": 8,
    },
    ("class-interface", "count"): {
        "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=0;assign=1;attributes=assign,setter=setCount:",
        "effective_getter_selector": "count",
        "effective_setter_available": True,
        "effective_setter_selector": "setCount:",
        "accessor_ownership_profile": "getter=count;setter_available=1;setter=setCount:;ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:count",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:count",
        "executable_ivar_layout_slot_index": 2,
        "executable_ivar_layout_size_bytes": 4,
        "executable_ivar_layout_alignment_bytes": 4,
    },
    ("class-implementation", "token"): {
        "property_attribute_profile": "readonly=1;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=0;assign=0;attributes=getter=tokenValue,readonly",
        "effective_getter_selector": "tokenValue",
        "effective_setter_available": False,
        "effective_setter_selector": "",
        "accessor_ownership_profile": "getter=tokenValue;setter_available=0;setter=<none>;ownership_lifetime=;runtime_hook=",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:token",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:token",
        "executable_ivar_layout_slot_index": 0,
        "executable_ivar_layout_size_bytes": 8,
        "executable_ivar_layout_alignment_bytes": 8,
    },
    ("class-implementation", "value"): {
        "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;strong=1;weak=0;unowned=0;assign=0;attributes=getter=currentValue,nonatomic,setter=setCurrentValue:,strong",
        "effective_getter_selector": "currentValue",
        "effective_setter_available": True,
        "effective_setter_selector": "setCurrentValue:",
        "accessor_ownership_profile": "getter=currentValue;setter_available=1;setter=setCurrentValue:;ownership_lifetime=;runtime_hook=",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:value",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:value",
        "executable_ivar_layout_slot_index": 1,
        "executable_ivar_layout_size_bytes": 8,
        "executable_ivar_layout_alignment_bytes": 8,
    },
    ("class-implementation", "count"): {
        "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=0;assign=1;attributes=assign,setter=setCount:",
        "effective_getter_selector": "count",
        "effective_setter_available": True,
        "effective_setter_selector": "setCount:",
        "accessor_ownership_profile": "getter=count;setter_available=1;setter=setCount:;ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:count",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:count",
        "executable_ivar_layout_slot_index": 2,
        "executable_ivar_layout_size_bytes": 4,
        "executable_ivar_layout_alignment_bytes": 4,
    },
}
EXPECTED_IVAR_RECORDS: dict[str, dict[str, Any]] = {
    "token": {
        "ivar_binding_symbol": "interface:Widget::ivar_binding:_token",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:token",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:token",
        "executable_ivar_layout_slot_index": 0,
        "executable_ivar_layout_size_bytes": 8,
        "executable_ivar_layout_alignment_bytes": 8,
    },
    "value": {
        "ivar_binding_symbol": "interface:Widget::ivar_binding:_value",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:value",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:value",
        "executable_ivar_layout_slot_index": 1,
        "executable_ivar_layout_size_bytes": 8,
        "executable_ivar_layout_alignment_bytes": 8,
    },
    "count": {
        "ivar_binding_symbol": "interface:Widget::ivar_binding:_count",
        "executable_synthesized_binding_kind": "implicit-ivar",
        "executable_synthesized_binding_symbol": "interface:Widget::property_synthesis:count",
        "executable_ivar_layout_symbol": "interface:Widget::objc_property_layout:count",
        "executable_ivar_layout_slot_index": 2,
        "executable_ivar_layout_size_bytes": 4,
        "executable_ivar_layout_alignment_bytes": 4,
    },
}

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m257_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m257" / "m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m257_a002_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py"


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
        SnippetCheck("M257-A002-DOC-EXP-01", "# M257 Ivar Layout And Property Attribute Source-Model Completion Core Feature Implementation Expectations (A002)"),
        SnippetCheck("M257-A002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M257-A002-DOC-EXP-03", "`frontend.lowering.executable_property_ivar_source_model_replay_key`"),
        SnippetCheck("M257-A002-DOC-EXP-04", "The contract must explicitly hand off to `M257-B001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M257-A002-DOC-PKT-01", "# M257-A002 Ivar Layout And Property Attribute Source-Model Completion Core Feature Implementation Packet"),
        SnippetCheck("M257-A002-DOC-PKT-02", "Packet: `M257-A002`"),
        SnippetCheck("M257-A002-DOC-PKT-03", "Issue: `#7146`"),
        SnippetCheck("M257-A002-DOC-PKT-04", "- `Objc3PropertyDecl.property_attribute_profile`"),
        SnippetCheck("M257-A002-DOC-PKT-05", "`M257-B001` is the explicit next handoff after this implementation closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M257-A002-DOCSRC-01", "## Ivar layout and property attribute source-model completion (M257-A002)"),
        SnippetCheck("M257-A002-DOCSRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-A002-DOCSRC-03", "`tmp/reports/m257/M257-A002/property_ivar_source_model_completion_summary.json`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M257-A002-NDOC-01", "## Ivar layout and property attribute source-model completion (M257-A002)"),
        SnippetCheck("M257-A002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-A002-NDOC-03", "`tmp/reports/m257/M257-A002/property_ivar_source_model_completion_summary.json`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M257-A002-SPC-01", "## M257 ivar layout and property attribute source-model completion (A002)"),
        SnippetCheck("M257-A002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-A002-SPC-03", f"`{FAILURE_MODEL}`"),
        SnippetCheck("M257-A002-SPC-04", "`M257-B001`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M257-A002-META-01", "## M257 property/ivar source-model completion metadata anchors (A002)"),
        SnippetCheck("M257-A002-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-A002-META-03", "`Objc3PropertyDecl.executable_ivar_layout_symbol`"),
        SnippetCheck("M257-A002-META-04", "`tmp/reports/m257/M257-A002/property_ivar_source_model_completion_summary.json`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M257-A002-ARCH-01", "## M257 ivar layout and property attribute source-model completion (A002)"),
        SnippetCheck("M257-A002-ARCH-02", "`M257-A002` broadens the frozen `M257-A001` source surface into one deterministic source-model completion step:"),
        SnippetCheck("M257-A002-ARCH-03", "check:objc3c:m257-a002-lane-a-readiness"),
    ),
    AST_HEADER: (
        SnippetCheck("M257-A002-AST-01", "kObjc3ExecutablePropertyIvarSourceModelCompletionContractId"),
        SnippetCheck("M257-A002-AST-02", "kObjc3ExecutablePropertyIvarLayoutModel"),
        SnippetCheck("M257-A002-AST-03", "executable_ivar_layout_symbol"),
    ),
    SEMA_CPP: (
        SnippetCheck("M257-A002-SEMA-01", "M257-A002 property-ivar source-model completion anchor"),
        SnippetCheck("M257-A002-SEMA-02", "property attribute/accessor ownership/layout fields belong to declaration compatibility"),
    ),
    IR_CPP: (
        SnippetCheck("M257-A002-IR-01", "M257-A002 property-ivar source-model completion anchor"),
        SnippetCheck("M257-A002-IR-02", "property_ivar_source_model_completion = "),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M257-A002-PKG-01", '"check:objc3c:m257-a002-ivar-layout-and-property-attribute-source-model-completion": "python scripts/check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py"'),
        SnippetCheck("M257-A002-PKG-02", '"test:tooling:m257-a002-ivar-layout-and-property-attribute-source-model-completion": "python -m pytest tests/tooling/test_check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py -q"'),
        SnippetCheck("M257-A002-PKG-03", '"check:objc3c:m257-a002-lane-a-readiness": "python scripts/run_m257_a002_lane_a_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M257-A002-RUN-01", "build:objc3c-native"),
        SnippetCheck("M257-A002-RUN-02", "test_check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py"),
        SnippetCheck("M257-A002-RUN-03", "check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M257-A002-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M257-A002-TEST-02", CONTRACT_ID),
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
    out_dir = PROBE_ROOT / "property_ivar_source_model_completion"
    out_dir.mkdir(parents=True, exist_ok=True)

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M257-A002-DYN-01", "native binary is missing", failures)
    checks_total += require(FIXTURE.exists(), display_path(FIXTURE), "M257-A002-DYN-02", "fixture is missing", failures)
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

    checks_total += require(completed.returncode == 0, display_path(out_dir), "M257-A002-DYN-03", f"compile failed: {completed.stdout}{completed.stderr}", failures)
    for check_id, path in (
        ("M257-A002-DYN-04", manifest_path),
        ("M257-A002-DYN-05", ir_path),
        ("M257-A002-DYN-06", backend_path),
        ("M257-A002-DYN-07", diagnostics_path),
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
    records = manifest.get("runtime_metadata_source_records", {}) if isinstance(manifest, dict) else {}
    property_records = records.get("properties", []) if isinstance(records, dict) else []
    ivar_records = records.get("ivars", []) if isinstance(records, dict) else []

    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M257-A002-DYN-08", "source-model completion must stay on llvm-direct", failures)
    checks_total += require(diagnostics.get("diagnostics") == [], display_path(diagnostics_path), "M257-A002-DYN-09", "fixture must remain diagnostics-clean", failures)
    checks_total += require(isinstance(sema, dict), display_path(manifest_path), "M257-A002-DYN-10", "manifest sema_pass_manager payload is missing", failures)
    checks_total += require(isinstance(records, dict), display_path(manifest_path), "M257-A002-DYN-11", "runtime metadata source records payload is missing", failures)
    checks_total += require(records.get("deterministic") is True, display_path(manifest_path), "M257-A002-DYN-12", "runtime metadata source records must remain deterministic", failures)
    checks_total += require(len(property_records) == 6, display_path(manifest_path), "M257-A002-DYN-13", f"expected 6 property records, observed {len(property_records)}", failures)
    checks_total += require(len(ivar_records) == 3, display_path(manifest_path), "M257-A002-DYN-14", f"expected 3 ivar records, observed {len(ivar_records)}", failures)
    if isinstance(sema, dict):
        for key, expected_value in EXPECTED_PROPERTY_COUNTS.items():
            checks_total += require(sema.get(key) == expected_value, display_path(manifest_path), f"M257-A002-DYN-SEMA-{key.upper()}", f"sema field {key} mismatch: expected {expected_value}, observed {sema.get(key)}", failures)

    property_index = {
        (str(record.get("owner_kind", "")), str(record.get("property_name", ""))): record
        for record in property_records
        if isinstance(record, dict)
    }
    for key, expected in EXPECTED_PROPERTY_RECORDS.items():
        record = property_index.get(key)
        checks_total += require(record is not None, display_path(manifest_path), f"M257-A002-DYN-PROP-{key[0]}-{key[1]}-EXISTS", f"missing property record for {key}", failures)
        if not isinstance(record, dict):
            continue
        for field_name, expected_value in expected.items():
            checks_total += require(record.get(field_name) == expected_value, display_path(manifest_path), f"M257-A002-DYN-PROP-{key[0]}-{key[1]}-{field_name}", f"property record {key} field {field_name} mismatch: expected {expected_value!r}, observed {record.get(field_name)!r}", failures)

    ivar_index = {
        str(record.get("property_name", "")): record
        for record in ivar_records
        if isinstance(record, dict)
    }
    for property_name, expected in EXPECTED_IVAR_RECORDS.items():
        record = ivar_index.get(property_name)
        checks_total += require(record is not None, display_path(manifest_path), f"M257-A002-DYN-IVAR-{property_name}-EXISTS", f"missing ivar record for {property_name}", failures)
        if not isinstance(record, dict):
            continue
        for field_name, expected_value in expected.items():
            checks_total += require(record.get(field_name) == expected_value, display_path(manifest_path), f"M257-A002-DYN-IVAR-{property_name}-{field_name}", f"ivar record {property_name} field {field_name} mismatch: expected {expected_value!r}, observed {record.get(field_name)!r}", failures)

    checks_total += require(f"; property_ivar_source_model_completion = {EXPECTED_REPLAY_KEY}" in ir_text, display_path(ir_path), "M257-A002-DYN-15", "IR replay key line drifted", failures)

    payload = {
        "fixture": display_path(FIXTURE),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "ir_path": display_path(ir_path),
        "backend_path": display_path(backend_path),
        "diagnostics_path": display_path(diagnostics_path),
        "backend_text": backend_text,
        "expected_replay_key": EXPECTED_REPLAY_KEY,
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
        "layout_model": LAYOUT_MODEL,
        "attribute_model": ATTRIBUTE_MODEL,
        "evidence_model": EVIDENCE_MODEL,
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
