#!/usr/bin/env python3
"""Checker for M261-B003 byref mutation, copy-dispose eligibility, and object-capture ownership."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-b003-byref-mutation-copy-dispose-object-ownership-v1"
CONTRACT_ID = "objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1"
BYREF_MUTATION_MODEL = (
    "source-only-sema-rejects-weak-or-unowned-byref-mutation-before-runnable-block-ownership-lowering"
)
COPY_DISPOSE_ELIGIBILITY_MODEL = (
    "owned-object-captures-promote-copy-dispose-helper-eligibility-even-without-byref-cells"
)
OBJECT_CAPTURE_OWNERSHIP_MODEL = (
    "weak-and-unowned-object-captures-remain-non-owning-and-do-not-force-copy-dispose-helpers"
)
NEXT_ISSUE = "M261-C001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-B003" / "byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_b003_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
OWNED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_owned_object_capture_helper_positive.objc3"
NONOWNING_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_nonowning_object_capture_helper_elided_positive.objc3"
WEAK_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_weak_object_capture_mutation_negative.objc3"
UNOWNED_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_unowned_object_capture_mutation_negative.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "b003-capture-ownership"


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
        SnippetCheck("M261-B003-EXP-01", "# M261 Byref Mutation Copy-Dispose Eligibility And Object-Capture Ownership Core Feature Expansion Expectations (B003)"),
        SnippetCheck("M261-B003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-B003-EXP-03", "`O3S206`"),
        SnippetCheck("M261-B003-EXP-04", "`O3S221`"),
        SnippetCheck("M261-B003-EXP-05", "`M261-C001`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-B003-PKT-01", "# M261-B003 Byref Mutation Copy-Dispose Eligibility And Object-Capture Ownership Core Feature Expansion Packet"),
        SnippetCheck("M261-B003-PKT-02", "Issue: `#7184`"),
        SnippetCheck("M261-B003-PKT-03", "Packet: `M261-B003`"),
        SnippetCheck("M261-B003-PKT-04", "`M261-C001` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-B003-SRC-01", "## M261 byref mutation, copy-dispose eligibility, and object-capture ownership semantics (M261-B003)"),
        SnippetCheck("M261-B003-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B003-SRC-03", "owned object captures now promote copy/dispose helper eligibility"),
        SnippetCheck("M261-B003-SRC-04", "`M261-C001` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-B003-NDOC-01", "## M261 byref mutation, copy-dispose eligibility, and object-capture ownership semantics (M261-B003)"),
        SnippetCheck("M261-B003-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B003-NDOC-03", "`M261-C001` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-B003-P0-01", "(`M261-B003`)"),
        SnippetCheck("M261-B003-P0-02", "copy/dispose helpers only for owned object captures"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-B003-SPC-01", "## M261 byref mutation, copy-dispose eligibility, and object-capture ownership semantics (B003)"),
        SnippetCheck("M261-B003-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B003-SPC-03", "mutating a captured `__weak` or `__unsafe_unretained` object now fails"),
        SnippetCheck("M261-B003-SPC-04", "`M261-C001` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-B003-ARCH-01", "## M261 Byref Mutation, Copy-Dispose Eligibility, And Object-Capture Ownership Semantics (B003)"),
        SnippetCheck("M261-B003-ARCH-02", "owned object captures promote copy/dispose helper eligibility"),
        SnippetCheck("M261-B003-ARCH-03", "the next issue is `M261-C001`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-B003-PARSER-01", "M261-B003 byref/copy-dispose/object-ownership anchor"),
        SnippetCheck("M261-B003-PARSER-02", "ownership-sensitive helper eligibility"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-B003-AST-01", "kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId"),
        SnippetCheck("M261-B003-AST-02", "kObjc3ExecutableBlockCopyDisposeEligibilityModel"),
        SnippetCheck("M261-B003-AST-03", "block_runtime_owned_object_capture_count"),
        SnippetCheck("M261-B003-AST-04", "block_runtime_capture_ownership_profile"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-B003-SEMA-PM-01", "M261-B003 byref/copy-dispose/object-ownership anchor"),
        SnippetCheck("M261-B003-SEMA-PM-02", "ownership-sensitive helper eligibility"),
    ),
    SEMA_CPP: (
        SnippetCheck("M261-B003-SEMA-01", "M261-B003 byref/copy-dispose/object-ownership anchor"),
        SnippetCheck("M261-B003-SEMA-02", "block_runtime_owned_object_capture_count"),
        SnippetCheck("M261-B003-SEMA-03", "requires owned runtime-backed storage"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-B003-LOWER-01", "M261-B003 byref/copy-dispose/object-ownership anchor"),
        SnippetCheck("M261-B003-LOWER-02", "owned object captures even when byref slot totals remain zero"),
    ),
    IR_CPP: (
        SnippetCheck("M261-B003-IR-01", "M261-B003 byref/copy-dispose/object-ownership anchor"),
        SnippetCheck("M261-B003-IR-02", "owned object captures as well as byref cells"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-B003-PKG-01", '"check:objc3c:m261-b003-block-capture-ownership": "python scripts/check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py"'),
        SnippetCheck("M261-B003-PKG-02", '"test:tooling:m261-b003-block-capture-ownership": "python -m pytest tests/tooling/test_check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py -q"'),
        SnippetCheck("M261-B003-PKG-03", '"check:objc3c:m261-b003-lane-b-readiness": "python scripts/run_m261_b003_lane_b_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-B003-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M261-B003-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-B003-RUN-03", "check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py"),
        SnippetCheck("M261-B003-RUN-04", "check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-B003-TEST-01", "def test_m261_b003_checker_emits_summary() -> None:"),
        SnippetCheck("M261-B003-TEST-02", CONTRACT_ID),
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


def diagnostics_codes(path: Path) -> list[str]:
    payload = load_json(path)
    diagnostics = payload.get("diagnostics", [])
    if not isinstance(diagnostics, list):
        return []
    return [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def semantic_surface(manifest_path: Path, key: str) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    surfaces = pipeline.get("semantic_surface", {})
    surface = surfaces.get(key)
    if not isinstance(surface, dict):
        raise TypeError(f"missing {key} in {display_path(manifest_path)}")
    return surface


def run_source_only_probe(fixture: Path, out_dir: Path) -> tuple[subprocess.CompletedProcess[str], Path, Path, Path]:
    summary_path = out_dir / "runner-summary.json"
    completed = run_process([
        str(RUNNER_EXE),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--summary-out",
        str(summary_path),
        "--no-emit-ir",
        "--no-emit-object",
    ])
    return completed, summary_path, out_dir / "module.manifest.json", out_dir / "module.diagnostics.json"


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}
    owned_out = PROBE_ROOT / "owned-positive"
    nonowning_out = PROBE_ROOT / "nonowning-positive"
    weak_out = PROBE_ROOT / "weak-mutation-negative"
    unowned_out = PROBE_ROOT / "unowned-mutation-negative"
    native_out = PROBE_ROOT / "native-fail-closed"
    for out_dir in (owned_out, nonowning_out, weak_out, unowned_out, native_out):
        out_dir.mkdir(parents=True, exist_ok=True)

    checks_total += require(RUNNER_EXE.exists(), display_path(RUNNER_EXE), "M261-B003-DYN-01", "frontend runner binary is missing", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-B003-DYN-02", "native binary is missing", failures)
    for idx, fixture in enumerate((OWNED_FIXTURE, NONOWNING_FIXTURE, WEAK_NEGATIVE_FIXTURE, UNOWNED_NEGATIVE_FIXTURE), start=3):
        checks_total += require(fixture.exists(), display_path(fixture), f"M261-B003-DYN-{idx:02d}", "fixture is missing", failures)
    if failures:
        return checks_total, payload

    owned_completed, owned_summary, owned_manifest, owned_diag = run_source_only_probe(OWNED_FIXTURE, owned_out)
    owned_diag_codes: list[str] = []
    owned_copy_surface: dict[str, Any] = {}
    owned_storage_surface: dict[str, Any] = {}
    checks_total += require(owned_completed.returncode == 0, display_path(owned_out), "M261-B003-DYN-07", f"owned positive probe failed: {owned_completed.stdout}{owned_completed.stderr}", failures)
    checks_total += require(owned_summary.exists(), display_path(owned_summary), "M261-B003-DYN-08", "owned summary is missing", failures)
    checks_total += require(owned_manifest.exists(), display_path(owned_manifest), "M261-B003-DYN-09", "owned manifest is missing", failures)
    checks_total += require(owned_diag.exists(), display_path(owned_diag), "M261-B003-DYN-10", "owned diagnostics are missing", failures)
    if owned_summary.exists() and owned_manifest.exists() and owned_diag.exists():
        owned_summary_payload = load_json(owned_summary)
        owned_diag_codes = diagnostics_codes(owned_diag)
        owned_copy_surface = semantic_surface(owned_manifest, "objc_block_copy_dispose_lowering_surface")
        owned_storage_surface = semantic_surface(owned_manifest, "objc_block_storage_escape_lowering_surface")
        checks_total += require(owned_summary_payload.get("success") is True, display_path(owned_summary), "M261-B003-DYN-11", "owned summary must report success", failures)
        checks_total += require(owned_diag_codes == [], display_path(owned_diag), "M261-B003-DYN-12", f"owned diagnostics must stay empty, observed {owned_diag_codes}", failures)
        checks_total += require(owned_copy_surface.get("block_literal_sites") == 1, display_path(owned_manifest), "M261-B003-DYN-13", "unexpected owned block literal count", failures)
        checks_total += require(owned_copy_surface.get("byref_slot_count_total") == 0, display_path(owned_manifest), "M261-B003-DYN-14", "owned helper probe must not require byref slots", failures)
        checks_total += require(owned_copy_surface.get("copy_helper_required_sites") == 1, display_path(owned_manifest), "M261-B003-DYN-15", "owned object capture must require copy helper", failures)
        checks_total += require(owned_copy_surface.get("dispose_helper_required_sites") == 1, display_path(owned_manifest), "M261-B003-DYN-16", "owned object capture must require dispose helper", failures)
        checks_total += require(owned_copy_surface.get("copy_helper_symbolized_sites") == 1, display_path(owned_manifest), "M261-B003-DYN-17", "owned object capture must symbolize copy helper", failures)
        checks_total += require(owned_copy_surface.get("dispose_helper_symbolized_sites") == 1, display_path(owned_manifest), "M261-B003-DYN-18", "owned object capture must symbolize dispose helper", failures)
        checks_total += require(owned_storage_surface.get("requires_byref_cells_sites") == 0, display_path(owned_manifest), "M261-B003-DYN-19", "owned object capture alone must not require byref cells", failures)
        checks_total += require(owned_copy_surface.get("contract_violation_sites") == 0, display_path(owned_manifest), "M261-B003-DYN-20", "owned copy/dispose contract violations must stay zero", failures)

    nonowning_completed, nonowning_summary, nonowning_manifest, nonowning_diag = run_source_only_probe(NONOWNING_FIXTURE, nonowning_out)
    nonowning_diag_codes: list[str] = []
    nonowning_copy_surface: dict[str, Any] = {}
    checks_total += require(nonowning_completed.returncode == 0, display_path(nonowning_out), "M261-B003-DYN-21", f"nonowning positive probe failed: {nonowning_completed.stdout}{nonowning_completed.stderr}", failures)
    checks_total += require(nonowning_summary.exists(), display_path(nonowning_summary), "M261-B003-DYN-22", "nonowning summary is missing", failures)
    checks_total += require(nonowning_manifest.exists(), display_path(nonowning_manifest), "M261-B003-DYN-23", "nonowning manifest is missing", failures)
    checks_total += require(nonowning_diag.exists(), display_path(nonowning_diag), "M261-B003-DYN-24", "nonowning diagnostics are missing", failures)
    if nonowning_summary.exists() and nonowning_manifest.exists() and nonowning_diag.exists():
        nonowning_summary_payload = load_json(nonowning_summary)
        nonowning_diag_codes = diagnostics_codes(nonowning_diag)
        nonowning_copy_surface = semantic_surface(nonowning_manifest, "objc_block_copy_dispose_lowering_surface")
        checks_total += require(nonowning_summary_payload.get("success") is True, display_path(nonowning_summary), "M261-B003-DYN-25", "nonowning summary must report success", failures)
        checks_total += require(nonowning_diag_codes == [], display_path(nonowning_diag), "M261-B003-DYN-26", f"nonowning diagnostics must stay empty, observed {nonowning_diag_codes}", failures)
        checks_total += require(nonowning_copy_surface.get("byref_slot_count_total") == 0, display_path(nonowning_manifest), "M261-B003-DYN-27", "nonowning helper probe must not require byref slots", failures)
        checks_total += require(nonowning_copy_surface.get("copy_helper_required_sites") == 0, display_path(nonowning_manifest), "M261-B003-DYN-28", "weak/unowned captures must not force copy helpers", failures)
        checks_total += require(nonowning_copy_surface.get("dispose_helper_required_sites") == 0, display_path(nonowning_manifest), "M261-B003-DYN-29", "weak/unowned captures must not force dispose helpers", failures)
        checks_total += require(nonowning_copy_surface.get("contract_violation_sites") == 0, display_path(nonowning_manifest), "M261-B003-DYN-30", "nonowning copy/dispose contract violations must stay zero", failures)

    weak_completed, weak_summary, _, weak_diag = run_source_only_probe(WEAK_NEGATIVE_FIXTURE, weak_out)
    weak_diag_codes: list[str] = []
    checks_total += require(weak_completed.returncode != 0, display_path(weak_out), "M261-B003-DYN-31", "weak mutation probe must fail", failures)
    checks_total += require(weak_summary.exists(), display_path(weak_summary), "M261-B003-DYN-32", "weak mutation summary is missing", failures)
    checks_total += require(weak_diag.exists(), display_path(weak_diag), "M261-B003-DYN-33", "weak mutation diagnostics are missing", failures)
    if weak_diag.exists():
        weak_diag_codes = diagnostics_codes(weak_diag)
        checks_total += require("O3S206" in weak_diag_codes, display_path(weak_diag), "M261-B003-DYN-34", f"expected O3S206 in weak mutation diagnostics, observed {weak_diag_codes}", failures)

    unowned_completed, unowned_summary, _, unowned_diag = run_source_only_probe(UNOWNED_NEGATIVE_FIXTURE, unowned_out)
    unowned_diag_codes: list[str] = []
    checks_total += require(unowned_completed.returncode != 0, display_path(unowned_out), "M261-B003-DYN-35", "unowned mutation probe must fail", failures)
    checks_total += require(unowned_summary.exists(), display_path(unowned_summary), "M261-B003-DYN-36", "unowned mutation summary is missing", failures)
    checks_total += require(unowned_diag.exists(), display_path(unowned_diag), "M261-B003-DYN-37", "unowned mutation diagnostics are missing", failures)
    if unowned_diag.exists():
        unowned_diag_codes = diagnostics_codes(unowned_diag)
        checks_total += require("O3S206" in unowned_diag_codes, display_path(unowned_diag), "M261-B003-DYN-38", f"expected O3S206 in unowned mutation diagnostics, observed {unowned_diag_codes}", failures)

    native_completed = run_process([
        str(NATIVE_EXE),
        str(OWNED_FIXTURE),
        "--out-dir",
        str(native_out),
        "--emit-prefix",
        "module",
    ])
    native_diag = native_out / "module.diagnostics.json"
    native_diag_codes: list[str] = []
    checks_total += require(native_completed.returncode != 0, display_path(native_out), "M261-B003-DYN-39", "native block probe must still fail closed", failures)
    checks_total += require(native_diag.exists(), display_path(native_diag), "M261-B003-DYN-40", "native diagnostics are missing", failures)
    if native_diag.exists():
        native_diag_codes = diagnostics_codes(native_diag)
        checks_total += require("O3S221" in native_diag_codes, display_path(native_diag), "M261-B003-DYN-41", f"expected O3S221 in native diagnostics, observed {native_diag_codes}", failures)

    payload["owned_positive_case"] = {
        "out_dir": display_path(owned_out),
        "diagnostic_codes": owned_diag_codes,
        "copy_dispose_surface": owned_copy_surface,
        "storage_escape_surface": owned_storage_surface,
    }
    payload["nonowning_positive_case"] = {
        "out_dir": display_path(nonowning_out),
        "diagnostic_codes": nonowning_diag_codes,
        "copy_dispose_surface": nonowning_copy_surface,
    }
    payload["weak_mutation_negative_case"] = {
        "out_dir": display_path(weak_out),
        "diagnostic_codes": weak_diag_codes,
    }
    payload["unowned_mutation_negative_case"] = {
        "out_dir": display_path(unowned_out),
        "diagnostic_codes": unowned_diag_codes,
    }
    payload["native_fail_closed_case"] = {
        "out_dir": display_path(native_out),
        "diagnostic_codes": native_diag_codes,
    }
    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        added_checks, added_failures = check_static_contract(path, snippets)
        checks_total += added_checks
        failures.extend(added_failures)

    if args.skip_dynamic_probes:
        dynamic_payload = {
            "owned_positive_case": {"skipped": True},
            "nonowning_positive_case": {"skipped": True},
            "weak_mutation_negative_case": {"skipped": True},
            "unowned_mutation_negative_case": {"skipped": True},
            "native_fail_closed_case": {"skipped": True},
        }
    else:
        added_checks, dynamic_payload = run_dynamic_probes(failures)
        checks_total += added_checks

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "byref_mutation_model": BYREF_MUTATION_MODEL,
        "copy_dispose_eligibility_model": COPY_DISPOSE_ELIGIBILITY_MODEL,
        "object_capture_ownership_model": OBJECT_CAPTURE_OWNERSHIP_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        **dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}")
        print(f"wrote summary: {display_path(args.summary_out)}")
        return 1
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
