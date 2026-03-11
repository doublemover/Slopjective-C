#!/usr/bin/env python3
"""Validate M260-B003 autoreleasepool/destruction-order semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-b003-autoreleasepool-destruction-order-semantics-v1"
CONTRACT_ID = "objc3c-runtime-backed-autoreleasepool-destruction-order-semantics/m260-b003-v1"
AUTORELEASEPOOL_MODEL = (
    "autoreleasepool-scopes-remain-fail-closed-while-owned-runtime-backed-object-storage-publishes-destruction-order-edge-diagnostics"
)
DESTRUCTION_MODEL = (
    "owned-runtime-backed-object-or-synthesized-property-storage-inside-autoreleasepool-requires-deferred-destruction-order-runtime-support"
)
FAILURE_MODEL = (
    "fail-closed-on-autoreleasepool-destruction-order-semantic-drift-for-owned-runtime-backed-storage"
)
NEXT_ISSUE = "M260-C001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-B003" / "autoreleasepool_destruction_order_semantics_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion_packet.md"
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
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_storage_ownership_legality_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_autoreleasepool_owned_storage_destruction_order_negative.objc3"
POSITIVE_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "b003" / "positive"
NEGATIVE_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "b003" / "negative"


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
    parser.add_argument("--positive-fixture", type=Path, default=POSITIVE_FIXTURE)
    parser.add_argument("--negative-fixture", type=Path, default=NEGATIVE_FIXTURE)
    parser.add_argument("--positive-out-dir", type=Path, default=POSITIVE_OUT_DIR)
    parser.add_argument("--negative-out-dir", type=Path, default=NEGATIVE_OUT_DIR)
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


def diagnostics_text(out_dir: Path) -> str:
    diagnostics_txt = out_dir / "module.diagnostics.txt"
    if not diagnostics_txt.exists():
        return ""
    return read_text(diagnostics_txt)


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
            SnippetCheck("M260-B003-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M260-B003-DOC-02", "Issue: `#7172`"),
            SnippetCheck("M260-B003-DOC-03", "`tests/tooling/fixtures/native/m260_autoreleasepool_owned_storage_destruction_order_negative.objc3`"),
            SnippetCheck("M260-B003-DOC-04", "plain `@autoreleasepool` still fails closed"),
            SnippetCheck("M260-B003-DOC-05", "`M260-C001`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M260-B003-PKT-01", "Packet: `M260-B003`"),
            SnippetCheck("M260-B003-PKT-02", "Issue: `#7172`"),
            SnippetCheck("M260-B003-PKT-03", "Dependencies: `M260-B002`"),
            SnippetCheck("M260-B003-PKT-04", "`M260-C001`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M260-B003-SRC-01", "## M260 autoreleasepool and destruction-order semantics (B003)"),
            SnippetCheck("M260-B003-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B003-SRC-03", "additional destruction-order diagnostic"),
            SnippetCheck("M260-B003-SRC-04", "`M260-C001` is the next issue"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M260-B003-NDOC-01", "## M260 autoreleasepool and destruction-order semantics (B003)"),
            SnippetCheck("M260-B003-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B003-NDOC-03", "additional destruction-order diagnostic"),
            SnippetCheck("M260-B003-NDOC-04", "`M260-C001` is the next issue"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.architecture_doc,
        (
            SnippetCheck("M260-B003-ARCH-01", "## M260 Autoreleasepool And Destruction-Order Semantics (B003)"),
            SnippetCheck("M260-B003-ARCH-02", "additional destruction-order diagnostic"),
            SnippetCheck("M260-B003-ARCH-03", "semantic admission rule only"),
            SnippetCheck("M260-B003-ARCH-04", "the next issue is `M260-C001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M260-B003-SPC-01", "## M260 autoreleasepool and destruction-order semantics (B003)"),
            SnippetCheck("M260-B003-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B003-SPC-03", f"`{AUTORELEASEPOOL_MODEL}`"),
            SnippetCheck("M260-B003-SPC-04", f"`{DESTRUCTION_MODEL}`"),
            SnippetCheck("M260-B003-SPC-05", f"`{FAILURE_MODEL}`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M260-B003-META-01", "## M260 autoreleasepool and destruction-order semantic metadata anchors (B003)"),
            SnippetCheck("M260-B003-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-B003-META-03", "tmp/artifacts/compilation/objc3c-native/m260/b003/positive/module.manifest.json"),
            SnippetCheck("M260-B003-META-04", "`M260-C001` is the next issue"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.sema_source,
        (
            SnippetCheck("M260-B003-SEMA-01", "M260-B003 autoreleasepool/destruction-order semantic expansion anchor:"),
            SnippetCheck("M260-B003-SEMA-02", "lane-B identified the ownership-sensitive destruction-order surface so"),
            SnippetCheck("M260-B003-SEMA-03", "later runtime work would not need to recover it from source"),
            SnippetCheck("M260-B003-SEMA-04", "BuildUnsupportedFeatureClaimContext"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.ir_source,
        (
            SnippetCheck("M260-B003-IR-01", "M260-B003 autoreleasepool/destruction-order semantic expansion anchor:"),
            SnippetCheck("M260-B003-IR-02", "runtime_backed_autoreleasepool_destruction_order"),
            SnippetCheck("M260-B003-IR-03", "ownership-sensitive destruction-order contract"),
            SnippetCheck("M260-B003-IR-04", "Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary()"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_header,
        (
            SnippetCheck("M260-B003-LHDR-01", "kObjc3RuntimeBackedAutoreleasepoolDestructionOrderContractId"),
            SnippetCheck("M260-B003-LHDR-02", AUTORELEASEPOOL_MODEL),
            SnippetCheck("M260-B003-LHDR-03", DESTRUCTION_MODEL),
            SnippetCheck("M260-B003-LHDR-04", FAILURE_MODEL),
            SnippetCheck("M260-B003-LHDR-05", "Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.lowering_source,
        (
            SnippetCheck("M260-B003-LOW-01", "M260-B003 autoreleasepool/destruction-order semantic expansion anchor:"),
            SnippetCheck("M260-B003-LOW-02", "autoreleasepool scopes still fail closed"),
            SnippetCheck("M260-B003-LOW-03", "owned runtime-backed object storage now upgrades"),
            SnippetCheck("M260-B003-LOW-04", "plain autoreleasepool parse-only probe"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M260-B003-PKG-01", '"check:objc3c:m260-b003-autoreleasepool-destruction-order-semantics":'),
            SnippetCheck("M260-B003-PKG-02", '"test:tooling:m260-b003-autoreleasepool-destruction-order-semantics":'),
            SnippetCheck("M260-B003-PKG-03", '"check:objc3c:m260-b003-lane-b-readiness":'),
            SnippetCheck("M260-B003-PKG-04", 'python scripts/run_m260_b003_lane_b_readiness.py'),
            SnippetCheck("M260-B003-PKG-05", '"compile:objc3c":'),
        ),
        failures,
    )

    if not args.skip_dynamic_probes:
        dynamic_probe_summary["executed"] = True
        positive_result = run_compile(args.compile_script, args.positive_fixture, args.positive_out_dir)
        negative_result = run_compile(args.compile_script, args.negative_fixture, args.negative_out_dir)
        positive_manifest_path = args.positive_out_dir / "module.manifest.json"
        positive_ir_path = args.positive_out_dir / "module.ll"
        negative_diagnostics = diagnostics_text(args.negative_out_dir)

        dynamic_probe_summary["positive"] = {
            "returncode": positive_result.returncode,
            "stdout": positive_result.stdout.strip(),
            "stderr": positive_result.stderr.strip(),
            "manifest": display_path(positive_manifest_path),
            "ir": display_path(positive_ir_path),
        }
        dynamic_probe_summary["negative"] = {
            "returncode": negative_result.returncode,
            "stdout": negative_result.stdout.strip(),
            "stderr": negative_result.stderr.strip(),
            "diagnostics": display_path(args.negative_out_dir / "module.diagnostics.txt"),
        }

        checks_total += 9
        checks_passed += require(positive_result.returncode == 0, display_path(args.positive_fixture), "M260-B003-DYN-01", "positive ownership fixture compile failed", failures)
        checks_passed += require(positive_manifest_path.exists(), display_path(positive_manifest_path), "M260-B003-DYN-02", "positive manifest artifact missing", failures)
        checks_passed += require(positive_ir_path.exists(), display_path(positive_ir_path), "M260-B003-DYN-03", "positive IR artifact missing", failures)
        ir_text = read_text(positive_ir_path) if positive_ir_path.exists() else ""
        checks_passed += require(
            f"; runtime_backed_autoreleasepool_destruction_order = contract={CONTRACT_ID};autoreleasepool_model={AUTORELEASEPOOL_MODEL};destruction_model={DESTRUCTION_MODEL};failure_model={FAILURE_MODEL}" in ir_text,
            display_path(positive_ir_path),
            "M260-B003-DYN-04",
            "IR autoreleasepool/destruction-order summary missing",
            failures,
        )
        checks_passed += require(negative_result.returncode != 0, display_path(args.negative_fixture), "M260-B003-DYN-05", "negative autoreleasepool fixture unexpectedly compiled", failures)
        checks_passed += require(
            "unsupported feature claim: '@autoreleasepool' is not yet runnable in Objective-C 3 native mode" in negative_diagnostics,
            display_path(args.negative_fixture),
            "M260-B003-DYN-06",
            "generic autoreleasepool rejection missing",
            failures,
        )
        checks_passed += require(
            "unsupported feature claim: '@autoreleasepool' with owned runtime-backed object or synthesized property storage requires destruction-order semantics that are not yet runnable in Objective-C 3 native mode" in negative_diagnostics,
            display_path(args.negative_fixture),
            "M260-B003-DYN-07",
            "ownership-sensitive destruction-order rejection missing",
            failures,
        )
        checks_passed += require("O3S221" in negative_diagnostics, display_path(args.negative_fixture), "M260-B003-DYN-08", "autoreleasepool negative diagnostics missing O3S221", failures)
        manifest_text = read_text(positive_manifest_path) if positive_manifest_path.exists() else ""
        checks_passed += require("currentValue" in manifest_text, display_path(positive_manifest_path), "M260-B003-DYN-09", "positive manifest lost owned runtime-backed property evidence", failures)
    else:
        dynamic_probe_summary["skipped"] = True

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "autoreleasepool_model": AUTORELEASEPOOL_MODEL,
        "destruction_model": DESTRUCTION_MODEL,
        "failure_model": FAILURE_MODEL,
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
