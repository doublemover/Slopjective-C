#!/usr/bin/env python3
"""Validate M260-C001 ownership lowering baseline freeze."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-c001-ownership-lowering-baseline-freeze-v1"
CONTRACT_ID = "objc3c-ownership-lowering-baseline-freeze/m260-c001-v1"
QUALIFIER_MODEL = (
    "ownership-qualifier-lowering-remains-legacy-summary-driven-for-runtime-backed-object-metadata"
)
RUNTIME_HOOK_MODEL = (
    "retain-release-autorelease-and-weak-lowering-stays-summary-only-without-live-runtime-hook-emission"
)
AUTORELEASEPOOL_MODEL = (
    "autoreleasepool-lowering-remains-summary-only-without-emitted-push-pop-hooks"
)
FAIL_CLOSED_MODEL = (
    "no-live-ownership-runtime-hooks-no-arc-weak-side-table-entrypoints-no-destruction-lowering-yet"
)
NEXT_ISSUE = "M260-C002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-C001" / "ownership_lowering_baseline_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_ownership_lowering_baseline_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_c001_ownership_lowering_baseline_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PACKAGE_JSON = ROOT / "package.json"
COMPILE_SCRIPT = ROOT / "scripts" / "objc3c_native_compile.ps1"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_storage_ownership_legality_positive.objc3"
PROBE_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "c001" / "positive"
EXPECTED_OWNERSHIP_REPLAY_KEY = (
    "ownership_qualifier_sites=8;invalid_ownership_qualifier_sites=0;"
    "object_pointer_type_annotation_sites=10;deterministic=true;"
    "lane_contract=m161-ownership-qualifier-lowering-v1"
)
EXPECTED_RETAIN_RELEASE_REPLAY_KEY = (
    "ownership_qualified_sites=8;retain_insertion_sites=4;release_insertion_sites=4;"
    "autorelease_insertion_sites=0;contract_violation_sites=0;deterministic=true;"
    "lane_contract=m162-retain-release-operation-lowering-v1"
)
EXPECTED_AUTORELEASEPOOL_REPLAY_KEY = (
    "scope_sites=0;scope_symbolized_sites=0;max_scope_depth=0;"
    "scope_entry_transition_sites=0;scope_exit_transition_sites=0;"
    "contract_violation_sites=0;deterministic=true;"
    "lane_contract=m163-autoreleasepool-scope-lowering-v1"
)
EXPECTED_WEAK_UNOWNED_REPLAY_KEY = (
    "ownership_candidate_sites=10;weak_reference_sites=2;unowned_reference_sites=4;"
    "unowned_safe_reference_sites=2;weak_unowned_conflict_sites=0;"
    "contract_violation_sites=0;deterministic=true;"
    "lane_contract=m164-weak-unowned-semantics-lowering-v1"
)
HOOK_SYMBOL_PATTERNS = (
    r"@objc_(?:retain|release|autorelease)\b",
    r"@objc_autoreleasePool(?:Push|Pop)\b",
    r"@objc_(?:storeWeak|loadWeakRetained)\b",
    r"@objc3_runtime_(?:retain|release|autorelease|storeWeak|loadWeakRetained)\b",
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=DOC_NATIVE)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--sema-source", type=Path, default=SEMA_SOURCE)
    parser.add_argument("--ir-source", type=Path, default=IR_SOURCE)
    parser.add_argument("--lowering-header", type=Path, default=LOWERING_HEADER)
    parser.add_argument("--lowering-source", type=Path, default=LOWERING_SOURCE)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--compile-script", type=Path, default=COMPILE_SCRIPT)
    parser.add_argument("--fixture", type=Path, default=FIXTURE)
    parser.add_argument("--probe-out-dir", type=Path, default=PROBE_OUT_DIR)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_compile(compile_script: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    cmd = [
        "pwsh",
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(compile_script),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    return subprocess.run(
        cmd,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        check=False,
    )


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    dynamic_probe_summary: dict[str, Any] = {"executed": False}

    checks_total += 5
    checks_passed += ensure_snippets(
        args.expectations_doc,
        (
            SnippetCheck("M260-C001-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M260-C001-DOC-02", "Issue: `#7173`"),
            SnippetCheck("M260-C001-DOC-03", "`tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_legality_positive.objc3`"),
            SnippetCheck("M260-C001-DOC-04", "No live retain/release/autorelease runtime hook emission yet."),
            SnippetCheck("M260-C001-DOC-05", "`M260-C002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M260-C001-PKT-01", "Packet: `M260-C001`"),
            SnippetCheck("M260-C001-PKT-02", "Issue: `#7173`"),
            SnippetCheck("M260-C001-PKT-03", "Dependencies: none"),
            SnippetCheck("M260-C001-PKT-04", "No live weak side-table runtime hook emission."),
            SnippetCheck("M260-C001-PKT-05", "`M260-C002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M260-C001-SRC-01", "## M260 ownership lowering baseline freeze (C001)"),
            SnippetCheck("M260-C001-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-C001-SRC-03", "`ownership_qualifier_lowering`"),
            SnippetCheck("M260-C001-SRC-04", "no live weak side-table runtime entrypoints are emitted yet"),
            SnippetCheck("M260-C001-SRC-05", "`M260-C002` is the next issue"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M260-C001-NDOC-01", "## M260 ownership lowering baseline freeze (C001)"),
            SnippetCheck("M260-C001-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-C001-NDOC-03", "retain/release lowering remains published through the legacy"),
            SnippetCheck("M260-C001-NDOC-04", "no live weak side-table runtime entrypoints are emitted yet"),
            SnippetCheck("M260-C001-NDOC-05", "`M260-C002` is the next issue"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.architecture_doc,
        (
            SnippetCheck("M260-C001-ARCH-01", "## M260 Ownership Lowering Baseline Freeze (C001)"),
            SnippetCheck("M260-C001-ARCH-02", "ownership qualifier, retain/release, autoreleasepool, and weak/unowned"),
            SnippetCheck("M260-C001-ARCH-03", "no live retain/release/autorelease runtime hook calls are emitted yet"),
            SnippetCheck("M260-C001-ARCH-04", "no live weak side-table runtime entrypoints are emitted yet"),
            SnippetCheck("M260-C001-ARCH-05", "`M260-C002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M260-C001-SPC-01", "## M260 ownership lowering baseline freeze (C001)"),
            SnippetCheck("M260-C001-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-C001-SPC-03", f"`{QUALIFIER_MODEL}`"),
            SnippetCheck("M260-C001-SPC-04", f"`{RUNTIME_HOOK_MODEL}`"),
            SnippetCheck("M260-C001-SPC-05", f"`{FAIL_CLOSED_MODEL}`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M260-C001-META-01", "## M260 ownership lowering baseline metadata anchors (C001)"),
            SnippetCheck("M260-C001-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-C001-META-03", "tmp/artifacts/compilation/objc3c-native/m260/c001/positive/module.manifest.json"),
            SnippetCheck("M260-C001-META-04", "`M260-C002` is the next issue"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.sema_source,
        (
            SnippetCheck("M260-C001-SEMA-01", "M260-C001 ownership-lowering baseline freeze anchor:"),
            SnippetCheck("M260-C001-SEMA-02", "legacy ownership lowering summaries"),
            SnippetCheck("M260-C001-SEMA-03", "no executable retain/release/autorelease/weak runtime hooks"),
            SnippetCheck("M260-C001-SEMA-04", "ownership hook surface available to IR closeout"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.ir_source,
        (
            SnippetCheck("M260-C001-IR-01", "M260-C001 ownership-lowering baseline freeze anchor:"),
            SnippetCheck("M260-C001-IR-02", 'out << "; ownership_lowering_baseline = "'),
            SnippetCheck("M260-C001-IR-03", "No live runtime ownership hooks are emitted here before M260-C002."),
            SnippetCheck("M260-C001-IR-04", "Objc3OwnershipLoweringBaselineSummary()"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_header,
        (
            SnippetCheck("M260-C001-LHDR-01", "kObjc3OwnershipLoweringBaselineContractId"),
            SnippetCheck("M260-C001-LHDR-02", QUALIFIER_MODEL),
            SnippetCheck("M260-C001-LHDR-03", RUNTIME_HOOK_MODEL),
            SnippetCheck("M260-C001-LHDR-04", FAIL_CLOSED_MODEL),
            SnippetCheck("M260-C001-LHDR-05", "Objc3OwnershipLoweringBaselineSummary"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.lowering_source,
        (
            SnippetCheck("M260-C001-LOW-01", "M260-C001 ownership-lowering baseline freeze anchor:"),
            SnippetCheck("M260-C001-LOW-02", "retain/release,"),
            SnippetCheck("M260-C001-LOW-03", "summary-only-without-live-runtime-hook-emission"),
            SnippetCheck("M260-C001-LOW-04", "before M260-C002"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M260-C001-PKG-01", '"check:objc3c:m260-c001-ownership-lowering-baseline-contract":'),
            SnippetCheck("M260-C001-PKG-02", '"test:tooling:m260-c001-ownership-lowering-baseline-contract":'),
            SnippetCheck("M260-C001-PKG-03", '"check:objc3c:m260-c001-lane-c-readiness":'),
            SnippetCheck("M260-C001-PKG-04", 'python scripts/run_m260_c001_lane_c_readiness.py'),
            SnippetCheck("M260-C001-PKG-05", '"compile:objc3c":'),
        ),
        failures,
    )

    if not args.skip_dynamic_probes:
        dynamic_probe_summary["executed"] = True
        result = run_compile(args.compile_script, args.fixture, args.probe_out_dir)
        dynamic_probe_summary["returncode"] = result.returncode
        dynamic_probe_summary["stdout"] = result.stdout.strip()
        dynamic_probe_summary["stderr"] = result.stderr.strip()

        manifest_path = args.probe_out_dir / "module.manifest.json"
        ir_path = args.probe_out_dir / "module.ll"
        obj_path = args.probe_out_dir / "module.obj"

        checks_total += 18
        checks_passed += require(result.returncode == 0, display_path(args.compile_script), "M260-C001-DYN-01", "ownership lowering baseline fixture compile failed", failures)
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M260-C001-DYN-02", "manifest artifact missing", failures)
        checks_passed += require(ir_path.exists(), display_path(ir_path), "M260-C001-DYN-03", "IR artifact missing", failures)
        checks_passed += require(obj_path.exists(), display_path(obj_path), "M260-C001-DYN-04", "object artifact missing", failures)

        manifest_payload = json.loads(read_text(manifest_path)) if manifest_path.exists() else {}
        ir_text = read_text(ir_path) if ir_path.exists() else ""
        records = (((manifest_payload.get("runtime_metadata_source_records") or {}).get("properties")) or [])
        boundary_line = (
            f"; ownership_lowering_baseline = contract={CONTRACT_ID};"
            f"ownership_qualifier_model={QUALIFIER_MODEL};"
            f"runtime_hook_model={RUNTIME_HOOK_MODEL};"
            f"autoreleasepool_model={AUTORELEASEPOOL_MODEL};"
            f"fail_closed_model={FAIL_CLOSED_MODEL};"
            "ownership_qualifier_lane=m161-ownership-qualifier-lowering-v1;"
            "retain_release_lane=m162-retain-release-operation-lowering-v1;"
            "autoreleasepool_lane=m163-autoreleasepool-scope-lowering-v1;"
            "weak_unowned_lane=m164-weak-unowned-semantics-lowering-v1"
        )

        checks_passed += require(
            (manifest_payload.get("lowering_ownership_qualifier") or {}).get("replay_key") == EXPECTED_OWNERSHIP_REPLAY_KEY,
            display_path(manifest_path),
            "M260-C001-DYN-05",
            "ownership qualifier lowering replay key drifted",
            failures,
        )
        checks_passed += require(
            (manifest_payload.get("lowering_retain_release_operation") or {}).get("replay_key") == EXPECTED_RETAIN_RELEASE_REPLAY_KEY,
            display_path(manifest_path),
            "M260-C001-DYN-06",
            "retain/release lowering replay key drifted",
            failures,
        )
        checks_passed += require(
            (manifest_payload.get("lowering_autoreleasepool_scope") or {}).get("replay_key") == EXPECTED_AUTORELEASEPOOL_REPLAY_KEY,
            display_path(manifest_path),
            "M260-C001-DYN-07",
            "autoreleasepool lowering replay key drifted",
            failures,
        )
        checks_passed += require(
            (manifest_payload.get("lowering_weak_unowned_semantics") or {}).get("replay_key") == EXPECTED_WEAK_UNOWNED_REPLAY_KEY,
            display_path(manifest_path),
            "M260-C001-DYN-08",
            "weak/unowned lowering replay key drifted",
            failures,
        )
        checks_passed += require(
            any(record.get("property_name") == "weakValue" and record.get("ownership_runtime_hook_profile") == "objc-weak-side-table" for record in records),
            display_path(manifest_path),
            "M260-C001-DYN-09",
            "weak runtime-backed ownership metadata missing",
            failures,
        )
        checks_passed += require(
            any(record.get("property_name") == "guardedValue" and record.get("ownership_runtime_hook_profile") == "objc-unowned-safe-guard" for record in records),
            display_path(manifest_path),
            "M260-C001-DYN-10",
            "unowned-safe runtime-backed ownership metadata missing",
            failures,
        )
        checks_passed += require(boundary_line in ir_text, display_path(ir_path), "M260-C001-DYN-11", "IR ownership lowering baseline summary missing", failures)
        checks_passed += require(
            "; frontend_objc_ownership_qualifier_lowering_profile = ownership_qualifier_sites=8, invalid_ownership_qualifier_sites=0, object_pointer_type_annotation_sites=10, deterministic_ownership_qualifier_lowering_handoff=true" in ir_text,
            display_path(ir_path),
            "M260-C001-DYN-12",
            "IR ownership qualifier lowering profile drifted",
            failures,
        )
        checks_passed += require(
            "; frontend_objc_retain_release_operation_lowering_profile = ownership_qualified_sites=8, retain_insertion_sites=4, release_insertion_sites=4, autorelease_insertion_sites=0, contract_violation_sites=0, deterministic_retain_release_operation_lowering_handoff=true" in ir_text,
            display_path(ir_path),
            "M260-C001-DYN-13",
            "IR retain/release lowering profile drifted",
            failures,
        )
        checks_passed += require(
            "; frontend_objc_autoreleasepool_scope_lowering_profile = scope_sites=0, scope_symbolized_sites=0, max_scope_depth=0, scope_entry_transition_sites=0, scope_exit_transition_sites=0, contract_violation_sites=0, deterministic_autoreleasepool_scope_lowering_handoff=true" in ir_text,
            display_path(ir_path),
            "M260-C001-DYN-14",
            "IR autoreleasepool lowering profile drifted",
            failures,
        )
        checks_passed += require(
            "; frontend_objc_weak_unowned_semantics_lowering_profile = ownership_candidate_sites=10, weak_reference_sites=2, unowned_reference_sites=4, unowned_safe_reference_sites=2, weak_unowned_conflict_sites=0, contract_violation_sites=0, deterministic_weak_unowned_semantics_lowering_handoff=true" in ir_text,
            display_path(ir_path),
            "M260-C001-DYN-15",
            "IR weak/unowned lowering profile drifted",
            failures,
        )
        checks_passed += require(
            re.search(r"!objc3\.objc_retain_release_operation_lowering = !\{![0-9]+\}", ir_text) is not None,
            display_path(ir_path),
            "M260-C001-DYN-16",
            "IR retain/release named metadata missing",
            failures,
        )
        checks_passed += require(
            re.search(r"!objc3\.objc_autoreleasepool_scope_lowering = !\{![0-9]+\}", ir_text) is not None,
            display_path(ir_path),
            "M260-C001-DYN-17",
            "IR autoreleasepool named metadata missing",
            failures,
        )
        checks_passed += require(
            re.search(r"!objc3\.objc_weak_unowned_semantics_lowering = !\{![0-9]+\}", ir_text) is not None,
            display_path(ir_path),
            "M260-C001-DYN-18",
            "IR weak/unowned named metadata missing",
            failures,
        )
        for index, pattern in enumerate(HOOK_SYMBOL_PATTERNS, start=19):
            checks_total += 1
            checks_passed += require(
                re.search(pattern, ir_text) is None,
                display_path(ir_path),
                f"M260-C001-DYN-{index}",
                f"unexpected live ownership runtime hook symbol matched pattern: {pattern}",
                failures,
            )

        dynamic_probe_summary["manifest"] = display_path(manifest_path)
        dynamic_probe_summary["ir"] = display_path(ir_path)
        dynamic_probe_summary["object"] = display_path(obj_path)
    else:
        dynamic_probe_summary["skipped"] = True

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "qualifier_model": QUALIFIER_MODEL,
        "runtime_hook_model": RUNTIME_HOOK_MODEL,
        "autoreleasepool_model": AUTORELEASEPOOL_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probe": dynamic_probe_summary,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
