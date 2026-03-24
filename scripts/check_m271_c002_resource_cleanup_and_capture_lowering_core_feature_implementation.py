#!/usr/bin/env python3
"""Checker for M271-C002 resource cleanup and capture lowering."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m271-c002-part8-resource-cleanup-and-capture-lowering-v1"
CONTRACT_ID = "objc3c-part8-resource-cleanup-and-capture-lowering/m271-c002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m271" / "M271-C002" / "resource_cleanup_and_capture_lowering_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m271_resource_cleanup_and_capture_lowering_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m271" / "m271_c002_resource_cleanup_and_capture_lowering_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_c002_resource_cleanup_capture_lowering_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_c002_resource_cleanup_capture_escaping_move_rejected.objc3"

EXPECTED_PACKET = {
    "contract_id": "objc3c-part8-system-extension-lowering-contract/m271-c001-v1",
    "surface_path": "frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract",
    "semantic_contract_id": "objc3c-part8-system-extension-semantic-model/m271-b001-v1",
    "resource_semantic_contract_id": "objc3c-part8-resource-move-use-after-move-semantics/m271-b002-v1",
    "borrowed_semantic_contract_id": "objc3c-part8-borrowed-pointer-escape-analysis/m271-b003-v1",
    "family_semantic_contract_id": "objc3c-part8-capture-list-retainable-family-legality/m271-b004-v1",
    "lane_contract_id": "m271-part8-system-extension-lowering-contract-v1",
    "cleanup_hook_sites": 2,
    "resource_local_sites": 2,
    "cleanup_owned_local_sites": 2,
    "resource_move_capture_sites": 1,
    "borrowed_parameter_sites": 2,
    "borrowed_return_callable_sites": 1,
    "borrowed_escape_candidate_sites": 0,
    "explicit_capture_item_sites": 3,
    "retainable_family_callable_sites": 2,
    "retainable_family_operation_callable_sites": 2,
    "retainable_family_alias_callable_sites": 2,
    "guard_blocked_sites": 0,
    "contract_violation_sites": 0,
}

IR_SNIPPETS = [
    "declare void @CloseFd(i32)",
    "declare void @ReleaseTemp(i32)",
    "define internal void @__objc3_block_dispose_helper_16_15_m0_b0_heap(ptr %block) {",
    "call void @CloseFd(i32 ",
    "call void @ReleaseTemp(i32 ",
]

NEGATIVE_DIAG = (
    "error:1:1: LLVM IR emission failed: lowering encountered unsupported fail-closed path: "
    "escaping move captures for cleanup/resource-backed locals still require later Part 8 runtime ownership transfer support [O3L300]"
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


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M271-C002-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def packet_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part8_system_extension_lowering_contract"]


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_PACKET.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M271-C002-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M271-C002-PAYLOAD-30", "replay_key missing", failures)
    total += 1
    passed += require(payload.get("deterministic_handoff") is True, artifact, "M271-C002-PAYLOAD-31", "deterministic_handoff must stay true", failures)
    total += 1
    passed += require(payload.get("ready_for_ir_emission") is True, artifact, "M271-C002-PAYLOAD-32", "ready_for_ir_emission must stay true", failures)
    return total, passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m271-c002-readiness",
        "--summary-out",
        "tmp/reports/m271/M271-C002/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M271-C002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.native_exe.exists(), display_path(args.native_exe), "M271-C002-DYN-02", "native compiler missing after build", failures)
    if ensure_build.returncode != 0 or not args.native_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m271" / "c002" / "positive"
    positive_out_dir.mkdir(parents=True, exist_ok=True)
    positive_run = run_command([
        str(args.native_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(positive_out_dir),
        "--emit-prefix",
        "module",
    ])
    positive_output = (positive_run.stdout or "") + (positive_run.stderr or "")
    positive_manifest = positive_out_dir / "module.manifest.json"
    positive_ir = positive_out_dir / "module.ll"
    positive_obj = positive_out_dir / "module.obj"
    positive_metadata = positive_out_dir / "module.runtime-metadata.bin"
    total += 1
    passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M271-C002-DYN-03", f"positive fixture failed: {positive_output}", failures)
    for offset, path in enumerate((positive_manifest, positive_ir, positive_obj, positive_metadata), start=4):
        total += 1
        passed += require(path.exists(), display_path(path), f"M271-C002-DYN-{offset:02d}", f"missing artifact: {display_path(path)}", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive_run.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(positive_manifest),
        "positive_ir": display_path(positive_ir),
        "positive_obj": display_path(positive_obj),
        "positive_runtime_metadata": display_path(positive_metadata),
    }
    if positive_manifest.exists():
        payload = packet_payload(positive_manifest)
        sub_total, sub_passed = validate_payload(payload, display_path(positive_manifest), failures)
        total += sub_total
        passed += sub_passed
        dynamic["system_extension_lowering_contract"] = payload
    if positive_ir.exists():
        ir_text = read_text(positive_ir)
        for index, snippet in enumerate(IR_SNIPPETS, start=40):
            total += 1
            passed += require(snippet in ir_text, display_path(positive_ir), f"M271-C002-IR-{index}", f"missing IR snippet: {snippet}", failures)

    negative_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m271" / "c002" / "negative"
    negative_out_dir.mkdir(parents=True, exist_ok=True)
    negative_run = run_command([
        str(args.native_exe),
        str(NEGATIVE_FIXTURE),
        "--out-dir",
        str(negative_out_dir),
        "--emit-prefix",
        "module",
    ])
    negative_output = (negative_run.stdout or "") + (negative_run.stderr or "")
    negative_diag_txt = negative_out_dir / "module.diagnostics.txt"
    total += 1
    passed += require(negative_run.returncode != 0, display_path(NEGATIVE_FIXTURE), "M271-C002-DYN-50", "negative fixture unexpectedly succeeded", failures)
    total += 1
    passed += require(negative_diag_txt.exists(), display_path(negative_diag_txt), "M271-C002-DYN-51", "negative diagnostics text missing", failures)
    if negative_diag_txt.exists():
        total += 1
        passed += require(NEGATIVE_DIAG in read_text(negative_diag_txt), display_path(negative_diag_txt), "M271-C002-DYN-52", "missing escaping move fail-closed diagnostic", failures)
    dynamic["negative_fixture"] = display_path(NEGATIVE_FIXTURE)
    dynamic["negative_returncode"] = negative_run.returncode
    dynamic["negative_output"] = negative_output.strip()
    dynamic["negative_diagnostics_txt"] = display_path(negative_diag_txt)

    return total, passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M271-C002-EXP-01", "# M271 Resource Cleanup And Capture Lowering Expectations (C002)"),
            SnippetCheck("M271-C002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M271-C002-EXP-03", "Escaping promotion of move-based cleanup/resource captures must remain fail-closed."),
        ],
        PACKET_DOC: [
            SnippetCheck("M271-C002-PKT-01", "# M271-C002 Packet: Resource Cleanup And Capture Lowering - Core Feature Implementation"),
            SnippetCheck("M271-C002-PKT-02", "Keep `M271-C001` as the only Part 8 lowering manifest surface."),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M271-C002-GRM-01", "## M271 resource cleanup and capture lowering"),
            SnippetCheck("M271-C002-GRM-02", "Current implementation status (`M271-C002`):"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M271-C002-DOC-01", "## M271 resource cleanup and capture lowering"),
            SnippetCheck("M271-C002-DOC-02", "Current implementation status (`M271-C002`):"),
        ],
        SPEC_AM: [
            SnippetCheck("M271-C002-AM-01", "M271-C002 lowering note:"),
            SnippetCheck("M271-C002-AM-02", "actual escaping promotion of move-based cleanup/resource captures remains"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M271-C002-ATTR-01", "Current implementation status (`M271-C002`):"),
            SnippetCheck("M271-C002-ATTR-02", "stack/local cleanup and resource lowering is now live in emitted native IR"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M271-C002-LWR-01", "M271-C002 lowering-implementation anchor:"),
            SnippetCheck("M271-C002-LWR-02", "Stack/local cleanup-resource capture lowering is implemented"),
        ],
        IR_EMITTER: [
            SnippetCheck("M271-C002-IRSRC-01", "M271-C002 lowering-implementation anchor: actual promotion of move-based"),
            SnippetCheck("M271-C002-IRSRC-02", "M271-C002 lowering-implementation anchor: cleanup/resource-backed move"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M271-C002-ART-01", "M271-C002 lowering-implementation anchor: the live cleanup/resource"),
            SnippetCheck("M271-C002-ART-02", "this issue does not mint a second manifest surface"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M271-C002-PKG-01", '"check:objc3c:m271-c002-resource-cleanup-and-capture-lowering-core-feature-implementation"'),
            SnippetCheck("M271-C002-PKG-02", '"check:objc3c:m271-c002-lane-c-readiness"'),
        ],
    }
    for path, checks in snippets.items():
        total += len(checks)
        passed += ensure_snippets(path, checks, failures)

    dynamic: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic["skipped"] = True
    else:
        sub_total, sub_passed, dynamic = run_dynamic_probes(args, failures)
        total += sub_total
        passed += sub_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": total,
        "checks_passed": passed,
        "checks_failed": len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "findings": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M271-C002 checks passed ({passed}/{total})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
