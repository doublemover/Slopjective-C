#!/usr/bin/env python3
"""Checker for M271-C001 system-extension lowering contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m271-c001-part8-system-extension-lowering-contract-v1"
CONTRACT_ID = "objc3c-part8-system-extension-lowering-contract/m271-c001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m271" / "M271-C001" / "system_extension_lowering_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m271_system_extension_lowering_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m271" / "m271_c001_system_extension_lowering_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_c001_system_extension_lowering_positive.objc3"

EXPECTED_PACKET = {
    "contract_id": CONTRACT_ID,
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
    "; system_extension_lowering_contract = cleanup_hook_sites=2",
    "; frontend_objc_system_extension_lowering_profile = cleanup_hook_sites=2",
    "!objc3.objc_part8_system_extension_lowering_contract = !{!98}",
    "!98 = !{i64 2, i64 2, i64 2, i64 1, i64 2, i64 1, i64 0, i64 3, i64 2, i64 2, i64 2, i64 0, i64 0, i1 1}",
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
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M271-C001-MISSING", f"missing artifact: {display_path(path)}"))
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
        passed += require(payload.get(field) == expected, artifact, f"M271-C001-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M271-C001-PAYLOAD-30", "replay_key missing", failures)
    total += 1
    passed += require(payload.get("deterministic_handoff") is True, artifact, "M271-C001-PAYLOAD-31", "deterministic_handoff must stay true", failures)
    total += 1
    passed += require(payload.get("ready_for_ir_emission") is True, artifact, "M271-C001-PAYLOAD-32", "ready_for_ir_emission must stay true", failures)
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
        "m271-c001-readiness",
        "--summary-out",
        "tmp/reports/m271/M271-C001/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M271-C001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M271-C001-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m271" / "c001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    total += 1
    passed += require(run.returncode == 0, display_path(FIXTURE), "M271-C001-DYN-03", f"positive fixture failed: {output}", failures)
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M271-C001-DYN-04", "manifest missing", failures)
    total += 1
    passed += require(ir_path.exists(), display_path(ir_path), "M271-C001-DYN-05", "module.ll missing", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "manifest": display_path(manifest_path),
        "ir_path": display_path(ir_path),
    }
    if manifest_path.exists():
        payload = packet_payload(manifest_path)
        sub_total, sub_passed = validate_payload(payload, display_path(manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["system_extension_lowering_contract"] = payload
    if ir_path.exists():
        ir_text = read_text(ir_path)
        for index, snippet in enumerate(IR_SNIPPETS, start=40):
            total += 1
            passed += require(snippet in ir_text, display_path(ir_path), f"M271-C001-IR-{index}", f"missing IR snippet: {snippet}", failures)
    return total, passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M271-C001-EXP-01", "# M271 System Extension Lowering Contract And Architecture Freeze Expectations (C001)"),
            SnippetCheck("M271-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M271-C001-EXP-03", "frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract"),
        ],
        PACKET_DOC: [
            SnippetCheck("M271-C001-PKT-01", "# M271-C001 Packet: System Extension Lowering Contract - Contract And Architecture Freeze"),
            SnippetCheck("M271-C001-PKT-02", "Freeze one truthful Part 8 lowering contract"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M271-C001-GRM-01", "## M271 system-extension lowering contract"),
            SnippetCheck("M271-C001-GRM-02", "frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M271-C001-DOC-01", "## M271 system-extension lowering contract"),
            SnippetCheck("M271-C001-DOC-02", "frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract"),
        ],
        SPEC_AM: [
            SnippetCheck("M271-C001-AM-01", "M271-C001 lowering note:"),
            SnippetCheck("M271-C001-AM-02", "objc_part8_system_extension_lowering_contract"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M271-C001-ATTR-01", "Current implementation status (`M271-C001`):"),
            SnippetCheck("M271-C001-ATTR-02", "frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M271-C001-LWR-01", "M271-C001 lowering-freeze anchor:"),
            SnippetCheck("M271-C001-LWR-02", "kObjc3Part8SystemExtensionLoweringContractId"),
            SnippetCheck("M271-C001-LWR-03", "struct Objc3Part8SystemExtensionLoweringContract"),
        ],
        LOWERING_CONTRACT_CPP: [
            SnippetCheck("M271-C001-LWR-04", "IsValidObjc3Part8SystemExtensionLoweringContract"),
            SnippetCheck("M271-C001-LWR-05", "Objc3Part8SystemExtensionLoweringReplayKey"),
        ],
        IR_HEADER: [
            SnippetCheck("M271-C001-IRH-01", "lowering_part8_system_extension_replay_key"),
            SnippetCheck("M271-C001-IRH-02", "part8_system_extension_lowering_cleanup_hook_sites"),
        ],
        IR_EMITTER: [
            SnippetCheck("M271-C001-IR-01", "; system_extension_lowering_contract = "),
            SnippetCheck("M271-C001-IR-02", "; frontend_objc_system_extension_lowering_profile = cleanup_hook_sites="),
            SnippetCheck("M271-C001-IR-03", "!objc3.objc_part8_system_extension_lowering_contract = !{!98}"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M271-C001-ART-01", "BuildPart8SystemExtensionLoweringContract("),
            SnippetCheck("M271-C001-ART-02", "BuildPart8SystemExtensionLoweringContractJson("),
            SnippetCheck("M271-C001-ART-03", "objc_part8_system_extension_lowering_contract"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M271-C001-PKG-01", "check:objc3c:m271-c001-system-extension-lowering-contract-and-architecture-freeze"),
            SnippetCheck("M271-C001-PKG-02", "check:objc3c:m271-c001-lane-c-readiness"),
        ],
    }

    for path, path_snippets in snippets.items():
        total += len(path_snippets)
        passed += ensure_snippets(path, path_snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        total += dyn_total
        passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": total,
        "checks_passed": passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(summary, indent=2))
        return 1
    print(f"[ok] {MODE}: {passed}/{total} checks passed")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
