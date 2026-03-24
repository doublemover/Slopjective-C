#!/usr/bin/env python3
"""Checker for M271-C003 borrowed/retainable ABI completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m271-c003-part8-borrowed-retainable-abi-completion-v1"
CONTRACT_ID = "objc3c-part8-borrowed-retainable-family-abi-completion/m271-c003-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m271" / "M271-C003" / "borrowed_retainable_abi_completion_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m271_borrowed_pointer_and_retainable_family_abi_completion_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m271" / "m271_c003_borrowed_pointer_and_retainable_family_abi_completion_core_feature_expansion_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_c003_borrowed_retainable_abi_completion_positive.objc3"

EXPECTED_PACKET = {
    "contract_id": CONTRACT_ID,
    "surface_path": "frontend.pipeline.semantic_surface.objc_part8_borrowed_pointer_and_retainable_family_abi_completion",
    "lowering_contract_id": "objc3c-part8-system-extension-lowering-contract/m271-c001-v1",
    "borrowed_parameter_sites": 2,
    "borrowed_return_callable_sites": 1,
    "returns_borrowed_attribute_sites": 1,
    "retainable_family_callable_sites": 3,
    "retainable_family_operation_callable_sites": 3,
    "retainable_family_alias_callable_sites": 3,
    "family_retain_sites": 1,
    "family_release_sites": 1,
    "family_autorelease_sites": 1,
    "compatibility_returns_retained_sites": 1,
    "compatibility_returns_not_retained_sites": 1,
    "compatibility_consumed_sites": 1,
}

IR_SNIPPETS = [
    "; part8_borrowed_retainable_abi_completion = contract=objc3c-part8-borrowed-retainable-family-abi-completion/m271-c003-v1",
    "!objc3.objc_part8_borrowed_pointer_and_retainable_family_abi_completion = !{!99}",
    "declare void @ConsumeBorrowed(i32)",
    "declare i32 @BorrowWindow(i32)",
    "declare i32 @CFRetain(i32)",
    "declare void @CFRelease(i32)",
    "declare i32 @CFAutorelease(i32)",
    "call i32 @BorrowWindow(i32",
    "call void @ConsumeBorrowed(i32",
    "call i32 @CFRetain(i32",
    "call i32 @CFAutorelease(i32",
    "call void @CFRelease(i32",
]


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
        failures.append(Finding(display_path(path), "M271-C003-MISSING", f"missing artifact: {display_path(path)}"))
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
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part8_borrowed_pointer_and_retainable_family_abi_completion"]


def lowering_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part8_system_extension_lowering_contract"]


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_PACKET.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M271-C003-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    total += 1
    passed += require(bool(payload.get("lowering_replay_key")), artifact, "M271-C003-PAYLOAD-20", "lowering_replay_key missing", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M271-C003-PAYLOAD-21", "replay_key missing", failures)
    total += 1
    passed += require(payload.get("deterministic_handoff") is True, artifact, "M271-C003-PAYLOAD-22", "deterministic_handoff must stay true", failures)
    total += 1
    passed += require(payload.get("ready_for_ir_emission") is True, artifact, "M271-C003-PAYLOAD-23", "ready_for_ir_emission must stay true", failures)
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
        "m271-c003-readiness",
        "--summary-out",
        "tmp/reports/m271/M271-C003/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M271-C003-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.native_exe.exists(), display_path(args.native_exe), "M271-C003-DYN-02", "native compiler missing after build", failures)
    if ensure_build.returncode != 0 or not args.native_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m271" / "c003" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.native_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    metadata_path = out_dir / "module.runtime-metadata.bin"
    total += 1
    passed += require(run.returncode == 0, display_path(FIXTURE), "M271-C003-DYN-03", f"positive fixture failed: {output}", failures)
    for offset, path in enumerate((manifest_path, ir_path, obj_path, backend_path, metadata_path), start=4):
        total += 1
        passed += require(path.exists(), display_path(path), f"M271-C003-DYN-{offset:02d}", f"missing artifact: {display_path(path)}", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "manifest": display_path(manifest_path),
        "ir_path": display_path(ir_path),
        "obj_path": display_path(obj_path),
        "backend_path": display_path(backend_path),
        "runtime_metadata_path": display_path(metadata_path),
    }
    if manifest_path.exists():
        payload = packet_payload(manifest_path)
        lowering = lowering_payload(manifest_path)
        sub_total, sub_passed = validate_payload(payload, display_path(manifest_path), failures)
        total += sub_total
        passed += sub_passed
        total += 1
        passed += require(payload.get("lowering_replay_key") == lowering.get("replay_key"), display_path(manifest_path), "M271-C003-DYN-20", "lowering replay key drifted from Part 8 lowering contract", failures)
        dynamic["abi_completion_packet"] = payload
        dynamic["lowering_contract"] = lowering
    if backend_path.exists():
        total += 1
        passed += require(read_text(backend_path).strip() == "llvm-direct", display_path(backend_path), "M271-C003-DYN-21", "object backend drifted from llvm-direct", failures)
    if ir_path.exists():
        ir_text = read_text(ir_path)
        for index, snippet in enumerate(IR_SNIPPETS, start=30):
            total += 1
            passed += require(snippet in ir_text, display_path(ir_path), f"M271-C003-IR-{index}", f"missing IR snippet: {snippet}", failures)

    return total, passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M271-C003-EXP-01", "# M271 Borrowed-Pointer And Retainable-Family ABI Completion Expectations (C003)"),
            SnippetCheck("M271-C003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M271-C003-EXP-03", "keep `frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract` as the single Part 8 lowering boundary"),
        ],
        PACKET_DOC: [
            SnippetCheck("M271-C003-PKT-01", "# M271-C003 Packet: Borrowed-Pointer And Retainable-Family ABI Completion - Core Feature Expansion"),
            SnippetCheck("M271-C003-PKT-02", "keep `M271-C001` as the only Part 8 lowering contract"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M271-C003-GRM-01", "## M271 borrowed-pointer and retainable-family ABI completion"),
            SnippetCheck("M271-C003-GRM-02", "Current implementation status (`M271-C003`):"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M271-C003-DOC-01", "## M271 borrowed-pointer and retainable-family ABI completion"),
            SnippetCheck("M271-C003-DOC-02", "objc_part8_borrowed_pointer_and_retainable_family_abi_completion"),
        ],
        SPEC_AM: [
            SnippetCheck("M271-C003-AM-01", "M271-C003 Part 8 ABI completion note:"),
            SnippetCheck("M271-C003-AM-02", "objc_part8_borrowed_pointer_and_retainable_family_abi_completion"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M271-C003-ATTR-01", "Current implementation status (`M271-C003`):"),
            SnippetCheck("M271-C003-ATTR-02", "objc_part8_borrowed_pointer_and_retainable_family_abi_completion"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M271-C003-H-01", "// M271-C003 ABI/artifact completion anchor:"),
            SnippetCheck("M271-C003-H-02", "kObjc3Part8BorrowedRetainableAbiCompletionContractId"),
        ],
        IR_EMITTER_H: [
            SnippetCheck("M271-C003-IH-01", "part8_borrowed_retainable_abi_completion_replay_key"),
            SnippetCheck("M271-C003-IH-02", "deterministic_part8_borrowed_retainable_abi_completion_handoff"),
        ],
        IR_EMITTER: [
            SnippetCheck("M271-C003-IRSRC-01", "part8_borrowed_retainable_abi_completion = contract="),
            SnippetCheck("M271-C003-IRSRC-02", "!objc3.objc_part8_borrowed_pointer_and_retainable_family_abi_completion = !{!99}"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M271-C003-ART-01", "BuildPart8BorrowedRetainableAbiCompletionJson"),
            SnippetCheck("M271-C003-ART-02", "objc_part8_borrowed_pointer_and_retainable_family_abi_completion"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M271-C003-PKG-01", '"check:objc3c:m271-c003-borrowed-pointer-and-retainable-family-abi-completion-core-feature-expansion"'),
            SnippetCheck("M271-C003-PKG-02", '"check:objc3c:m271-c003-lane-c-readiness"'),
        ],
        FIXTURE: [
            SnippetCheck("M271-C003-FIX-01", "extern fn BorrowWindow(owner: id) -> borrowed id *"),
            SnippetCheck("M271-C003-FIX-02", "__attribute__((objc_family_autorelease(CoreThing), os_returns_not_retained));"),
        ],
    }
    for path, checks in snippets.items():
        total += len(checks)
        passed += ensure_snippets(path, checks, failures)

    dynamic: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        sub_total, sub_passed, dynamic = run_dynamic_probes(args, failures)
        total += sub_total
        passed += sub_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "total_checks": total,
        "passed_checks": passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "findings": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}")
        print(f"[info] wrote summary to {display_path(args.summary_out)}")
        return 1
    print(f"[ok] M271-C003 borrowed/retainable ABI checks passed ({passed}/{total})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
