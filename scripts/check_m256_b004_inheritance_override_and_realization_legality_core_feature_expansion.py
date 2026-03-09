#!/usr/bin/env python3
"""Fail-closed checker for M256-B004 inheritance/override/realization legality."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-b004-inheritance-override-realization-legality-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-inheritance-override-realization-legality/m256-b004-v1"
PREVIOUS_CONTRACT_ID = "objc3c-category-merge-conflict-semantics/m256-b003-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m256" / "M256-B004" / "inheritance_override_realization_legality_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m256" / "b004-inheritance-override-realization"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m256_inheritance_override_and_realization_legality_core_feature_expansion_b004_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_b004_inheritance_override_and_realization_legality_core_feature_expansion_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m256_b004_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_realization_positive.objc3"
MISSING_SUPER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_missing_superclass.objc3"
CYCLE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_cycle.objc3"
BAD_SIGNATURE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_bad_signature.objc3"
BAD_KIND_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_bad_kind.objc3"
UNREALIZED_SUPER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_unrealized_superclass.objc3"
BAD_PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_bad_property.objc3"


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
        SnippetCheck("M256-B004-DOC-EXP-01", "# M256 Inheritance, Override, and Realization Legality Core Feature Expansion Expectations (B004)"),
        SnippetCheck("M256-B004-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M256-B004-DOC-EXP-03", "Issue: `#7135`"),
        SnippetCheck("M256-B004-DOC-EXP-04", "`O3S220`"),
        SnippetCheck("M256-B004-DOC-EXP-05", "selector-kind drift"),
    ),
    PACKET_DOC: (
        SnippetCheck("M256-B004-DOC-PKT-01", "# M256-B004 Inheritance, Override, and Realization Legality Core Feature Expansion Packet"),
        SnippetCheck("M256-B004-DOC-PKT-02", "Packet: `M256-B004`"),
        SnippetCheck("M256-B004-DOC-PKT-03", "Dependencies: `M256-B003`, `M256-A002`"),
        SnippetCheck("M256-B004-DOC-PKT-04", "Next issue: `M256-B005`"),
        SnippetCheck("M256-B004-DOC-PKT-05", f"Contract ID: `{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M256-B004-NDOC-01", "## Inheritance, override, and realization legality (M256-B004)"),
        SnippetCheck("M256-B004-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-B004-NDOC-03", "realized superclass implementation"),
        SnippetCheck("M256-B004-NDOC-04", "`O3S220`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M256-B004-SPC-01", "## M256 inheritance, override, and realization legality (B004)"),
        SnippetCheck("M256-B004-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-B004-SPC-03", "selector-kind drift"),
        SnippetCheck("M256-B004-SPC-04", "`O3S220`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M256-B004-META-01", "## M256 inheritance/override realization metadata anchors (B004)"),
        SnippetCheck("M256-B004-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-B004-META-03", "missing superclass interfaces"),
        SnippetCheck("M256-B004-META-04", "`O3S220`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M256-B004-ARCH-01", "## M256 inheritance, override, and realization legality (B004)"),
        SnippetCheck("M256-B004-ARCH-02", "check:objc3c:m256-b004-lane-b-readiness"),
        SnippetCheck("M256-B004-ARCH-03", "realized-class inheritance legality"),
    ),
    PARSER_CPP: (
        SnippetCheck("M256-B004-PARSE-01", "M256-B004 inheritance-legality source anchor"),
    ),
    SEMA_CPP: (
        SnippetCheck("M256-B004-SEMA-01", "M256-B004 inheritance-override-realization anchor"),
        SnippetCheck("M256-B004-SEMA-02", "ValidateInheritanceOverrideAndRealizationLegality("),
        SnippetCheck("M256-B004-SEMA-03", '"O3S220"'),
    ),
    IR_CPP: (
        SnippetCheck("M256-B004-IR-01", "M256-B004 inheritance/override legality anchor"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M256-B004-PKG-01", '"check:objc3c:m256-b004-inheritance-override-and-realization-legality"'),
        SnippetCheck("M256-B004-PKG-02", '"test:tooling:m256-b004-inheritance-override-and-realization-legality"'),
        SnippetCheck("M256-B004-PKG-03", '"check:objc3c:m256-b004-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M256-B004-RUN-01", "check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py"),
        SnippetCheck("M256-B004-RUN-02", "check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py"),
        SnippetCheck("M256-B004-RUN-03", "M256-A003 + M256-B001 + M256-B002 + M256-B003"),
    ),
    TEST_FILE: (
        SnippetCheck("M256-B004-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M256-B004-TEST-02", CONTRACT_ID),
    ),
    POSITIVE_FIXTURE: (
        SnippetCheck("M256-B004-FIX-POS-01", "module inheritanceOverrideRealizationPositive;"),
        SnippetCheck("M256-B004-FIX-POS-02", "@interface Widget : Root"),
        SnippetCheck("M256-B004-FIX-POS-03", "return [super ping] + 3;"),
    ),
    MISSING_SUPER_FIXTURE: (
        SnippetCheck("M256-B004-FIX-MISS-01", "module inheritanceOverrideMissingSuperclass;"),
        SnippetCheck("M256-B004-FIX-MISS-02", "@interface Widget : MissingRoot"),
    ),
    CYCLE_FIXTURE: (
        SnippetCheck("M256-B004-FIX-CYCLE-01", "module inheritanceOverrideCycle;"),
        SnippetCheck("M256-B004-FIX-CYCLE-02", "@interface Alpha : Beta"),
        SnippetCheck("M256-B004-FIX-CYCLE-03", "@interface Beta : Alpha"),
    ),
    BAD_SIGNATURE_FIXTURE: (
        SnippetCheck("M256-B004-FIX-SIG-01", "module inheritanceOverrideBadSignature;"),
        SnippetCheck("M256-B004-FIX-SIG-02", "- (bool) ping;"),
    ),
    BAD_KIND_FIXTURE: (
        SnippetCheck("M256-B004-FIX-KIND-01", "module inheritanceOverrideBadKind;"),
        SnippetCheck("M256-B004-FIX-KIND-02", "+ (i32) ping;"),
    ),
    UNREALIZED_SUPER_FIXTURE: (
        SnippetCheck("M256-B004-FIX-REAL-01", "module inheritanceOverrideUnrealizedSuperclass;"),
        SnippetCheck("M256-B004-FIX-REAL-02", "@interface Widget : Root"),
    ),
    BAD_PROPERTY_FIXTURE: (
        SnippetCheck("M256-B004-FIX-PROP-01", "module inheritanceOverrideBadProperty;"),
        SnippetCheck("M256-B004-FIX-PROP-02", "@property (readonly) bool token;"),
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


def run_compile_case(
    *,
    fixture: Path,
    out_dir: Path,
    expect_success: bool,
    expected_snippets: Sequence[str],
) -> tuple[int, list[Finding], dict[str, object]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M256-B004-NATIVE-EXISTS", "native binary is missing", failures)
    checks_total += require(fixture.exists(), display_path(fixture), "M256-B004-FIXTURE-EXISTS", "fixture is missing", failures)
    if failures:
        return checks_total, failures, {}

    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_process(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
    )
    manifest_path = out_dir / "module.manifest.json"
    backend_path = out_dir / "module.object-backend.txt"
    diagnostics_text_path = out_dir / "module.diagnostics.txt"
    diagnostics_json_path = out_dir / "module.diagnostics.json"
    diagnostics_text = diagnostics_text_path.read_text(encoding="utf-8") if diagnostics_text_path.exists() else ""
    diagnostics_json = json.loads(diagnostics_json_path.read_text(encoding="utf-8")) if diagnostics_json_path.exists() else None
    manifest = json.loads(manifest_path.read_text(encoding="utf-8")) if manifest_path.exists() else None
    backend_text = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
    combined = (completed.stdout or "") + "\n" + (completed.stderr or "") + "\n" + diagnostics_text

    if expect_success:
        checks_total += require(completed.returncode == 0, display_path(out_dir), "M256-B004-COMPILE-SUCCESS", "positive fixture must compile successfully", failures)
        checks_total += require(manifest_path.exists(), display_path(manifest_path), "M256-B004-MANIFEST", "positive fixture manifest is missing", failures)
        checks_total += require(backend_path.exists(), display_path(backend_path), "M256-B004-BACKEND", "positive fixture backend marker is missing", failures)
        if backend_path.exists():
            checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M256-B004-BACKEND-TEXT", "positive fixture must stay on llvm-direct", failures)
        checks_total += require("O3S220" not in combined, display_path(out_dir), "M256-B004-NO-LEGality-DIAG", "positive fixture emitted inheritance/override/realization diagnostics", failures)
    else:
        checks_total += require(completed.returncode != 0, display_path(out_dir), "M256-B004-COMPILE-FAIL", "negative fixture must fail compilation", failures)
        for snippet in expected_snippets:
            checks_total += require(snippet in combined, display_path(out_dir), f"M256-B004-DIAG-{snippet}", f"expected diagnostic snippet not found: {snippet}", failures)

    payload = {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "returncode": completed.returncode,
        "manifest_exists": manifest_path.exists(),
        "backend_exists": backend_path.exists(),
        "backend_text": backend_text,
        "diagnostics_text": diagnostics_text,
        "diagnostics_json": diagnostics_json,
        "manifest": manifest,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, failures, payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, snippets in STATIC_SNIPPETS.items():
        count, findings = check_static_contract(path, snippets)
        checks_total += count
        failures.extend(findings)

    positive_case: dict[str, object] = {}
    missing_super_case: dict[str, object] = {}
    cycle_case: dict[str, object] = {}
    bad_signature_case: dict[str, object] = {}
    bad_kind_case: dict[str, object] = {}
    unrealized_super_case: dict[str, object] = {}
    bad_property_case: dict[str, object] = {}

    if not args.skip_dynamic_probes:
        count, findings, positive_case = run_compile_case(
            fixture=POSITIVE_FIXTURE,
            out_dir=PROBE_ROOT / "positive",
            expect_success=True,
            expected_snippets=(),
        )
        checks_total += count
        failures.extend(findings)

        negative_cases = (
            (MISSING_SUPER_FIXTURE, PROBE_ROOT / "missing-superclass", ("O3S220", "inherits from missing superclass 'MissingRoot'")),
            (CYCLE_FIXTURE, PROBE_ROOT / "cycle", ("O3S220", "inheritance cycle detected")),
            (BAD_SIGNATURE_FIXTURE, PROBE_ROOT / "bad-signature", ("O3S220", "incompatible override signature for selector '-ping'")),
            (BAD_KIND_FIXTURE, PROBE_ROOT / "bad-kind", ("O3S220", "changes method kind relative to inherited selector")),
            (UNREALIZED_SUPER_FIXTURE, PROBE_ROOT / "unrealized-superclass", ("O3S220", "requires realized superclass implementation 'Root'")),
            (BAD_PROPERTY_FIXTURE, PROBE_ROOT / "bad-property", ("O3S220", "incompatible inherited property 'token'")),
        )
        payload_targets = []
        for fixture, out_dir, expected_snippets in negative_cases:
            count, findings, payload = run_compile_case(
                fixture=fixture,
                out_dir=out_dir,
                expect_success=False,
                expected_snippets=expected_snippets,
            )
            checks_total += count
            failures.extend(findings)
            payload_targets.append(payload)
        (
            missing_super_case,
            cycle_case,
            bad_signature_case,
            bad_kind_case,
            unrealized_super_case,
            bad_property_case,
        ) = payload_targets

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "previous_contract_id": PREVIOUS_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "positive_case": positive_case,
        "missing_superclass_case": missing_super_case,
        "cycle_case": cycle_case,
        "bad_signature_case": bad_signature_case,
        "bad_kind_case": bad_kind_case,
        "unrealized_superclass_case": unrealized_super_case,
        "bad_property_case": bad_property_case,
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
