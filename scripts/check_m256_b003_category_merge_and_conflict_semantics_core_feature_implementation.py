#!/usr/bin/env python3
"""Fail-closed checker for M256-B003 category merge and conflict semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-b003-category-merge-and-conflict-semantics-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-category-merge-conflict-semantics/m256-b003-v1"
PREVIOUS_CONTRACT_ID = "objc3c-protocol-conformance-required-optional-enforcement/m256-b002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m256" / "M256-B003" / "category_merge_and_conflict_semantics_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m256" / "b003-category-merge"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m256_category_merge_and_conflict_semantics_core_feature_implementation_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_b003_category_merge_and_conflict_semantics_core_feature_implementation_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m256_b003_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_category_merge_positive.objc3"
CONFLICTING_METHOD_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_category_merge_conflicting_method.objc3"
CONFLICTING_PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_category_merge_conflicting_property.objc3"
MISSING_PAIR_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_category_merge_missing_pair.objc3"


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
        SnippetCheck("M256-B003-DOC-EXP-01", "# M256 Category Merge and Conflict Semantics Core Feature Implementation Expectations (B003)"),
        SnippetCheck("M256-B003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M256-B003-DOC-EXP-03", "Issue: `#7134`"),
        SnippetCheck("M256-B003-DOC-EXP-04", "Deterministic conflict reporting must preserve the previously attached category owner"),
        SnippetCheck("M256-B003-DOC-EXP-05", "Metadata-only orphan categories on unrealized classes remain non-blocking"),
    ),
    PACKET_DOC: (
        SnippetCheck("M256-B003-DOC-PKT-01", "# M256-B003 Category Merge and Conflict Semantics Core Feature Implementation Packet"),
        SnippetCheck("M256-B003-DOC-PKT-02", "Packet: `M256-B003`"),
        SnippetCheck("M256-B003-DOC-PKT-03", "Dependencies: `M256-B002`, `M256-A003`"),
        SnippetCheck("M256-B003-DOC-PKT-04", "Next issue: `M256-B004`"),
        SnippetCheck("M256-B003-DOC-PKT-05", f"Contract ID: `{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M256-B003-NDOC-01", "## Category merge and conflict semantics (M256-B003)"),
        SnippetCheck("M256-B003-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-B003-NDOC-03", "concrete message resolution now consults the merged category surface"),
        SnippetCheck("M256-B003-NDOC-04", "`O3S219`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M256-B003-SPC-01", "## M256 category merge and conflict semantics (B003)"),
        SnippetCheck("M256-B003-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-B003-SPC-03", "sema owns deterministic merge-surface construction for realized classes only"),
        SnippetCheck("M256-B003-SPC-04", "incompatible attached category members fail closed with `O3S219`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M256-B003-META-01", "## M256 category-merge metadata anchors (B003)"),
        SnippetCheck("M256-B003-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-B003-META-03", "missing interface/implementation category pairs fail closed"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M256-B003-ARCH-01", "## M256 category merge and conflict semantics (B003)"),
        SnippetCheck("M256-B003-ARCH-02", "check:objc3c:m256-b003-lane-b-readiness"),
        SnippetCheck("M256-B003-ARCH-03", "concrete message resolution through attached category members"),
    ),
    PARSER_CPP: (
        SnippetCheck("M256-B003-PARSE-01", "M256-B003 category-merge source-order anchor"),
        SnippetCheck("M256-B003-PARSE-02", "deterministic realized-class merge surface"),
    ),
    SEMA_CPP: (
        SnippetCheck("M256-B003-SEMA-01", "M256-B003 category-merge enforcement anchor"),
        SnippetCheck("M256-B003-SEMA-02", "ValidateAndBuildCategoryMergeSurfaces("),
        SnippetCheck("M256-B003-SEMA-03", "ResolveMethodInMergedCategorySurface("),
        SnippetCheck("M256-B003-SEMA-04", "ResolvePropertyInMergedCategorySurface("),
        SnippetCheck("M256-B003-SEMA-05", '"O3S219"'),
    ),
    IR_CPP: (
        SnippetCheck("M256-B003-IR-01", "M256-B003 category-merge implementation anchor"),
        SnippetCheck("M256-B003-IR-02", "realized-class category merge/conflict decision and must"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M256-B003-PKG-01", '"check:objc3c:m256-b003-category-merge-and-conflict-semantics"'),
        SnippetCheck("M256-B003-PKG-02", '"test:tooling:m256-b003-category-merge-and-conflict-semantics"'),
        SnippetCheck("M256-B003-PKG-03", '"check:objc3c:m256-b003-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M256-B003-RUN-01", "check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py"),
        SnippetCheck("M256-B003-RUN-02", "check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py"),
        SnippetCheck("M256-B003-RUN-03", "M256-A003 + M256-B001 + M256-B002"),
    ),
    TEST_FILE: (
        SnippetCheck("M256-B003-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M256-B003-TEST-02", CONTRACT_ID),
    ),
    POSITIVE_FIXTURE: (
        SnippetCheck("M256-B003-FIX-POS-01", "module categoryMergePositive;"),
        SnippetCheck("M256-B003-FIX-POS-02", "@interface Widget (Alpha)"),
        SnippetCheck("M256-B003-FIX-POS-03", "@interface Widget (Beta)"),
        SnippetCheck("M256-B003-FIX-POS-04", "return [Widget alphaValue] + [Widget baseValue];"),
    ),
    CONFLICTING_METHOD_FIXTURE: (
        SnippetCheck("M256-B003-FIX-METHOD-01", "module categoryMergeConflictingMethod;"),
        SnippetCheck("M256-B003-FIX-METHOD-02", "+ (bool) value;"),
    ),
    CONFLICTING_PROPERTY_FIXTURE: (
        SnippetCheck("M256-B003-FIX-PROP-01", "module categoryMergeConflictingProperty;"),
        SnippetCheck("M256-B003-FIX-PROP-02", "@property (readonly) i32 token;"),
    ),
    MISSING_PAIR_FIXTURE: (
        SnippetCheck("M256-B003-FIX-PAIR-01", "module categoryMergeMissingPair;"),
        SnippetCheck("M256-B003-FIX-PAIR-02", "@interface Widget (Debug)"),
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
    extra_positive_checks: bool = False,
) -> tuple[int, list[Finding], dict[str, object]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M256-B003-NATIVE-EXISTS", "native binary is missing", failures)
    checks_total += require(fixture.exists(), display_path(fixture), "M256-B003-FIXTURE-EXISTS", "fixture is missing", failures)
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
        checks_total += require(completed.returncode == 0, display_path(out_dir), "M256-B003-COMPILE-SUCCESS", "positive fixture must compile successfully", failures)
        checks_total += require(manifest_path.exists(), display_path(manifest_path), "M256-B003-MANIFEST", "positive fixture manifest is missing", failures)
        checks_total += require(backend_path.exists(), display_path(backend_path), "M256-B003-BACKEND", "positive fixture backend marker is missing", failures)
        if backend_path.exists():
            checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M256-B003-BACKEND-TEXT", "positive fixture must stay on llvm-direct", failures)
        checks_total += require("O3S219" not in combined, display_path(out_dir), "M256-B003-NO-MERGE-DIAG", "positive fixture emitted category-merge diagnostics", failures)
        if extra_positive_checks:
            category_records = manifest.get("categories", []) if isinstance(manifest, dict) else []
            checks_total += require(len(category_records) == 4, display_path(manifest_path), "M256-B003-CATEGORY-COUNT", "positive fixture must preserve two category interface/implementation pairs", failures)
            runtime_methods = (
                manifest.get("runtime_metadata_source_records", {}).get("methods", [])
                if isinstance(manifest, dict)
                else []
            )
            selectors = sorted(method["selector"] for method in runtime_methods if isinstance(method, dict) and method.get("owner_name") in {"Widget(Alpha)", "Widget(Beta)"})
            checks_total += require(selectors == ["alphaValue", "alphaValue", "betaValue", "betaValue"], display_path(manifest_path), "M256-B003-MERGED-METHODS", "positive fixture must expose deterministic attached-category method records", failures)
    else:
        checks_total += require(completed.returncode != 0, display_path(out_dir), "M256-B003-COMPILE-FAIL", "negative fixture must fail compilation", failures)
        for snippet in expected_snippets:
            checks_total += require(snippet in combined, display_path(out_dir), f"M256-B003-DIAG-{snippet}", f"expected diagnostic snippet not found: {snippet}", failures)

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
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--parser-cpp", type=Path, default=PARSER_CPP)
    parser.add_argument("--sema-cpp", type=Path, default=SEMA_CPP)
    parser.add_argument("--ir-cpp", type=Path, default=IR_CPP)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--readiness-runner", type=Path, default=READINESS_RUNNER)
    parser.add_argument("--test-file", type=Path, default=TEST_FILE)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    checks_total = 0
    failures: list[Finding] = []

    for path, snippets in {
        args.expectations_doc: STATIC_SNIPPETS[EXPECTATIONS_DOC],
        args.packet_doc: STATIC_SNIPPETS[PACKET_DOC],
        args.native_doc: STATIC_SNIPPETS[NATIVE_DOC],
        args.lowering_spec: STATIC_SNIPPETS[LOWERING_SPEC],
        args.metadata_spec: STATIC_SNIPPETS[METADATA_SPEC],
        args.architecture_doc: STATIC_SNIPPETS[ARCHITECTURE_DOC],
        args.parser_cpp: STATIC_SNIPPETS[PARSER_CPP],
        args.sema_cpp: STATIC_SNIPPETS[SEMA_CPP],
        args.ir_cpp: STATIC_SNIPPETS[IR_CPP],
        args.package_json: STATIC_SNIPPETS[PACKAGE_JSON],
        args.readiness_runner: STATIC_SNIPPETS[READINESS_RUNNER],
        args.test_file: STATIC_SNIPPETS[TEST_FILE],
        POSITIVE_FIXTURE: STATIC_SNIPPETS[POSITIVE_FIXTURE],
        CONFLICTING_METHOD_FIXTURE: STATIC_SNIPPETS[CONFLICTING_METHOD_FIXTURE],
        CONFLICTING_PROPERTY_FIXTURE: STATIC_SNIPPETS[CONFLICTING_PROPERTY_FIXTURE],
        MISSING_PAIR_FIXTURE: STATIC_SNIPPETS[MISSING_PAIR_FIXTURE],
    }.items():
        count, found = check_static_contract(path, snippets)
        checks_total += count
        failures.extend(found)

    dynamic_payload: dict[str, object] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        positive_checks, positive_failures, positive_payload = run_compile_case(
            fixture=POSITIVE_FIXTURE,
            out_dir=PROBE_ROOT / "positive",
            expect_success=True,
            expected_snippets=(),
            extra_positive_checks=True,
        )
        checks_total += positive_checks
        failures.extend(positive_failures)

        method_checks, method_failures, method_payload = run_compile_case(
            fixture=CONFLICTING_METHOD_FIXTURE,
            out_dir=PROBE_ROOT / "conflicting-method",
            expect_success=False,
            expected_snippets=("O3S219", "selector '+value'", "conflicts with attached category 'Widget(Alpha)'"),
        )
        checks_total += method_checks
        failures.extend(method_failures)

        property_checks, property_failures, property_payload = run_compile_case(
            fixture=CONFLICTING_PROPERTY_FIXTURE,
            out_dir=PROBE_ROOT / "conflicting-property",
            expect_success=False,
            expected_snippets=("O3S219", "property 'token'", "conflicts with attached category 'Widget(Alpha)'"),
        )
        checks_total += property_checks
        failures.extend(property_failures)

        pair_checks, pair_failures, pair_payload = run_compile_case(
            fixture=MISSING_PAIR_FIXTURE,
            out_dir=PROBE_ROOT / "missing-pair",
            expect_success=False,
            expected_snippets=("O3S219", "missing category implementation", "realized class 'Widget'"),
        )
        checks_total += pair_checks
        failures.extend(pair_failures)

        dynamic_payload = {
            "skipped": False,
            "positive": positive_payload,
            "conflicting_method": method_payload,
            "conflicting_property": property_payload,
            "missing_pair": pair_payload,
        }

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "previous_contract_id": PREVIOUS_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "merge_model": "realized-class category surfaces built in parser declaration order",
        "concrete_resolution_model": "known-owner message resolution consults the merged category surface before base fallback",
        "conformance_model": "declared protocol conformance consults the merged category surface",
        "diagnostic_code": "O3S219",
        "dynamic_probes": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        print(canonical_json(payload), file=sys.stderr, end="")
        return 1
    print(canonical_json(payload), end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
