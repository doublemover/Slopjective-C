#!/usr/bin/env python3
"""Fail-closed checker for M257-B003 property accessor legality interactions."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-b003-accessor-legality-ownership-or-atomicity-attribute-interactions-v1"
CONTRACT_ID = "objc3c-property-accessor-attribute-interactions/m257-b003-v1"
PREVIOUS_CONTRACT_ID = "objc3c-property-default-ivar-binding-semantics/m257-b002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m257" / "M257-B003" / "accessor_legality_attribute_interactions_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m257" / "b003-accessor-legality-attribute-interactions"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m257_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m257" / "m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
NATIVE_DOC_SRC = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m257_b003_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_accessor_attribute_interactions_positive.objc3"
GETTER_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_accessor_duplicate_getter_negative.objc3"
SETTER_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_accessor_duplicate_setter_negative.objc3"
SCALAR_OWNERSHIP_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_scalar_ownership_negative.objc3"
ATOMIC_OWNERSHIP_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_atomic_ownership_negative.objc3"
ACCESSOR_SELECTOR_MODEL = "effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding"
OWNERSHIP_ATOMICITY_MODEL = "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land"


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
        SnippetCheck("M257-B003-DOC-EXP-01", "# M257 Accessor Legality and Ownership or Atomicity Attribute Interactions Core Feature Expansion Expectations (B003)"),
        SnippetCheck("M257-B003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M257-B003-DOC-EXP-03", "Issue: `#7149`"),
        SnippetCheck("M257-B003-DOC-EXP-04", "m257_accessor_duplicate_getter_negative.objc3"),
        SnippetCheck("M257-B003-DOC-EXP-05", "Atomic ownership-aware property => `O3S206`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M257-B003-DOC-PKT-01", "# M257-B003 Accessor Legality and Ownership or Atomicity Attribute Interactions Core Feature Expansion Packet"),
        SnippetCheck("M257-B003-DOC-PKT-02", "Packet: `M257-B003`"),
        SnippetCheck("M257-B003-DOC-PKT-03", "Dependencies: `M257-B001`, `M257-B002`"),
        SnippetCheck("M257-B003-DOC-PKT-04", "Next issue: `M257-B004`"),
        SnippetCheck("M257-B003-DOC-PKT-05", f"Contract ID: `{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M257-B003-NDOC-01", "## Accessor legality and ownership or atomicity attribute interactions (M257-B003)"),
        SnippetCheck("M257-B003-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B003-NDOC-03", "duplicate effective getter and setter selectors now fail closed per property container"),
        SnippetCheck("M257-B003-NDOC-04", "ownership modifiers on non-object properties and atomic ownership-aware properties fail closed"),
    ),
    NATIVE_DOC_SRC: (
        SnippetCheck("M257-B003-NSRC-01", "## Accessor legality and ownership or atomicity attribute interactions (M257-B003)"),
        SnippetCheck("M257-B003-NSRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B003-NSRC-03", "duplicate effective getter and setter selectors now fail closed per property container"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M257-B003-SPC-01", "## M257 accessor legality and ownership or atomicity attribute interactions (B003)"),
        SnippetCheck("M257-B003-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B003-SPC-03", "lane-B rejects duplicate effective accessor selectors before executable accessor binding"),
        SnippetCheck("M257-B003-SPC-04", "atomic ownership-aware properties remain fail-closed"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M257-B003-META-01", "## M257 property accessor legality metadata anchors (B003)"),
        SnippetCheck("M257-B003-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-B003-META-03", "`Objc3PropertyInfo.has_accessor_selector_contract_violation`"),
        SnippetCheck("M257-B003-META-04", "`frontend.pipeline.semantic_surface.objc_property_attribute_surface`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M257-B003-ARCH-01", "## M257 accessor legality and ownership or atomicity attribute interactions (B003)"),
        SnippetCheck("M257-B003-ARCH-02", "duplicate effective accessor selectors are rejected before runtime accessor realization"),
        SnippetCheck("M257-B003-ARCH-03", "check:objc3c:m257-b003-lane-b-readiness"),
    ),
    AST_HEADER: (
        SnippetCheck("M257-B003-AST-01", "kObjc3ExecutablePropertyAccessorSelectorUniquenessModel"),
        SnippetCheck("M257-B003-AST-02", ACCESSOR_SELECTOR_MODEL),
        SnippetCheck("M257-B003-AST-03", "kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel"),
        SnippetCheck("M257-B003-AST-04", OWNERSHIP_ATOMICITY_MODEL),
    ),
    SEMA_CPP: (
        SnippetCheck("M257-B003-SEMA-01", "M257-B003 accessor/ownership legality anchor"),
        SnippetCheck("M257-B003-SEMA-02", "ValidatePropertyAccessorSelectorUniqueness("),
        SnippetCheck("M257-B003-SEMA-03", "requires an Objective-C object property"),
        SnippetCheck("M257-B003-SEMA-04", "atomic ownership-aware property '"),
    ),
    IR_CPP: (
        SnippetCheck("M257-B003-IR-01", "M257-B003 accessor legality expansion anchor"),
        SnippetCheck("M257-B003-IR-02", "storage interaction diagnostics have already fail-closed source."),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M257-B003-PKG-01", '"check:objc3c:m257-b003-accessor-legality-and-ownership-or-atomicity-attribute-interactions"'),
        SnippetCheck("M257-B003-PKG-02", '"test:tooling:m257-b003-accessor-legality-and-ownership-or-atomicity-attribute-interactions"'),
        SnippetCheck("M257-B003-PKG-03", '"check:objc3c:m257-b003-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M257-B003-RUN-01", "M257-B003"),
        SnippetCheck("M257-B003-RUN-02", "check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py"),
        SnippetCheck("M257-B003-RUN-03", "check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M257-B003-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M257-B003-TEST-02", CONTRACT_ID),
    ),
    POSITIVE_FIXTURE: (
        SnippetCheck("M257-B003-FIX-01", "module propertyAccessorAttributeInteractionsPositive;"),
        SnippetCheck("M257-B003-FIX-02", "@property (nonatomic, getter=tokenValue, setter=setTokenValue:, strong) id token;"),
        SnippetCheck("M257-B003-FIX-03", "@property (assign, getter=countValue, setter=setCountValue:) i32 count;"),
    ),
    GETTER_NEGATIVE_FIXTURE: (
        SnippetCheck("M257-B003-FIX-04", "module propertyAccessorDuplicateGetterNegative;"),
        SnippetCheck("M257-B003-FIX-05", "@property (getter=value) id token;"),
    ),
    SETTER_NEGATIVE_FIXTURE: (
        SnippetCheck("M257-B003-FIX-06", "module propertyAccessorDuplicateSetterNegative;"),
        SnippetCheck("M257-B003-FIX-07", "@property (setter=setValue:) id token;"),
    ),
    SCALAR_OWNERSHIP_NEGATIVE_FIXTURE: (
        SnippetCheck("M257-B003-FIX-08", "module propertyScalarOwnershipNegative;"),
        SnippetCheck("M257-B003-FIX-09", "@property (strong) i32 value;"),
    ),
    ATOMIC_OWNERSHIP_NEGATIVE_FIXTURE: (
        SnippetCheck("M257-B003-FIX-10", "module propertyAtomicOwnershipNegative;"),
        SnippetCheck("M257-B003-FIX-11", "@property (atomic, copy) id value;"),
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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


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
    return (
        completed,
        out_dir / "module.manifest.json",
        out_dir / "module.object-backend.txt",
        out_dir / "module.diagnostics.txt",
        out_dir / "module.diagnostics.json",
    )


def validate_positive_case(failures: list[Finding]) -> tuple[int, dict[str, object]]:
    checks_total = 0
    out_dir = PROBE_ROOT / "positive"
    completed, manifest_path, backend_path, diagnostics_text_path, diagnostics_json_path = compile_fixture(POSITIVE_FIXTURE, out_dir)
    diagnostics_text = diagnostics_text_path.read_text(encoding="utf-8") if diagnostics_text_path.exists() else ""
    checks_total += require(completed.returncode == 0, display_path(out_dir), "POS-COMPILE", completed.stdout + completed.stderr + diagnostics_text, failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "POS-MANIFEST", "manifest is missing", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), "POS-BACKEND", "backend marker is missing", failures)
    checks_total += require(diagnostics_json_path.exists(), display_path(diagnostics_json_path), "POS-DIAGNOSTICS", "diagnostics json is missing", failures)
    if completed.returncode != 0 or not manifest_path.exists() or not backend_path.exists() or not diagnostics_json_path.exists():
        return checks_total, {}

    manifest = load_json(manifest_path)
    backend_text = backend_path.read_text(encoding="utf-8").strip()
    diagnostics_json = load_json(diagnostics_json_path)
    attribute_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_property_attribute_surface", {})
    expected_surface = {
        "property_declaration_entries": 3,
        "property_attribute_entries": 11,
        "property_attribute_value_entries": 6,
        "property_accessor_modifier_entries": 11,
        "property_getter_selector_entries": 3,
        "property_setter_selector_entries": 3,
    }

    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "POS-LLVM-DIRECT", "probe must stay on llvm-direct", failures)
    checks_total += require(diagnostics_json.get("diagnostics") == [], display_path(diagnostics_json_path), "POS-DIAG-CLEAN", "probe must remain diagnostics-clean", failures)
    checks_total += require(attribute_surface.get("deterministic_handoff") is True, display_path(manifest_path), "POS-DETERMINISTIC", "property attribute surface must stay deterministic", failures)
    for key, expected in expected_surface.items():
        checks_total += require(attribute_surface.get(key) == expected, display_path(manifest_path), f"POS-{key}", f"expected {key}={expected}, observed {attribute_surface.get(key)}", failures)

    return checks_total, {
        "fixture": display_path(POSITIVE_FIXTURE),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "backend_text": backend_text,
        "attribute_surface": attribute_surface,
    }


def validate_negative_case(fixture: Path, probe_name: str, expected_snippets: Sequence[str], failures: list[Finding]) -> tuple[int, dict[str, object]]:
    checks_total = 0
    out_dir = PROBE_ROOT / probe_name
    completed, manifest_path, backend_path, diagnostics_text_path, diagnostics_json_path = compile_fixture(fixture, out_dir)
    diagnostics_text = diagnostics_text_path.read_text(encoding="utf-8") if diagnostics_text_path.exists() else ""
    combined = (completed.stdout or "") + "\n" + (completed.stderr or "") + "\n" + diagnostics_text
    checks_total += require(completed.returncode != 0, display_path(out_dir), f"{probe_name}-FAIL", "negative fixture must fail compilation", failures)
    for snippet in expected_snippets:
        checks_total += require(snippet in combined, display_path(out_dir), f"{probe_name}-{snippet}", f"expected diagnostic snippet not found: {snippet}", failures)
    return checks_total, {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "manifest_exists": manifest_path.exists(),
        "backend_exists": backend_path.exists(),
        "diagnostics_exists": diagnostics_json_path.exists(),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
        "diagnostics_text": diagnostics_text,
    }


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    args = parser.parse_args(argv)

    findings: list[Finding] = []
    checks_total = 0
    dynamic_checks_total = 0
    positive_payload: dict[str, object] = {}
    negative_payloads: dict[str, object] = {}

    for path, snippets in STATIC_SNIPPETS.items():
        file_checks, file_findings = check_static_contract(path, snippets)
        checks_total += file_checks
        findings.extend(file_findings)

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "NATIVE-EXISTS", "native compiler executable is missing; run npm run build:objc3c-native", findings)

    if not args.skip_dynamic_probes and not findings:
        probe_checks, positive_payload = validate_positive_case(findings)
        dynamic_checks_total += probe_checks

        getter_checks, getter_payload = validate_negative_case(
            GETTER_NEGATIVE_FIXTURE,
            "duplicate-getter",
            (
                "duplicate effective getter selector 'value'",
                "[O3S206]",
            ),
            findings,
        )
        dynamic_checks_total += getter_checks
        negative_payloads["duplicate_getter"] = getter_payload

        setter_checks, setter_payload = validate_negative_case(
            SETTER_NEGATIVE_FIXTURE,
            "duplicate-setter",
            (
                "duplicate effective setter selector 'setValue:'",
                "[O3S206]",
            ),
            findings,
        )
        dynamic_checks_total += setter_checks
        negative_payloads["duplicate_setter"] = setter_payload

        scalar_checks, scalar_payload = validate_negative_case(
            SCALAR_OWNERSHIP_NEGATIVE_FIXTURE,
            "scalar-ownership",
            (
                "ownership modifier 'strong' requires an Objective-C object property",
                "[O3S206]",
            ),
            findings,
        )
        dynamic_checks_total += scalar_checks
        negative_payloads["scalar_ownership"] = scalar_payload

        atomic_checks, atomic_payload = validate_negative_case(
            ATOMIC_OWNERSHIP_NEGATIVE_FIXTURE,
            "atomic-ownership",
            (
                "atomic ownership-aware property 'value' in interface 'Widget' is unsupported until executable accessor storage semantics land",
                "[O3S206]",
            ),
            findings,
        )
        dynamic_checks_total += atomic_checks
        negative_payloads["atomic_ownership"] = atomic_payload

    checks_total += dynamic_checks_total
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "previous_contract_id": PREVIOUS_CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "findings": [finding.__dict__ for finding in findings],
        "dynamic_probes_skipped": args.skip_dynamic_probes,
        "positive_probe": positive_payload,
        "negative_probes": negative_payloads,
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        for finding in findings:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {CONTRACT_ID} ({checks_total} checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
