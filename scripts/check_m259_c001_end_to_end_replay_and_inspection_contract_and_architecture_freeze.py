#!/usr/bin/env python3
"""Validate M259-C001 runnable replay/inspection freeze."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-c001-end-to-end-replay-and-inspection-freeze-v1"


def host_path_str(path: Path) -> str:
    text = str(path)
    if sys.platform.startswith("win") and text.startswith("\\\\?\\"):
        return text[4:]
    return text


def host_arg(arg: str) -> str:
    if sys.platform.startswith("win") and arg.startswith("\\\\?\\"):
        return arg[4:]
    return arg
CONTRACT_ID = "objc3c-runnable-replay-and-inspection-evidence-freeze/m259-c001-v1"
FREEZE_MODEL = "runnable-slice-replay-proof-and-single-sample-object-inspection-boundary"
EVIDENCE_MODEL = "execution-smoke-plus-replay-proof-plus-a002-object-section-anchor"
FAILURE_MODEL = "fail-closed-on-runnable-replay-or-object-inspection-boundary-drift"
NEXT_ISSUE = "M259-C002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-C001" / "end_to_end_replay_and_inspection_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_c001_end_to_end_replay_and_inspection_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_c001_end_to_end_replay_and_inspection_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
EXECUTION_SMOKE = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
EXECUTION_REPLAY = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
REPLAY_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-replay-proof"
A002_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_a002_canonical_runnable_sample_set.objc3"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-A002" / "canonical_runnable_sample_set_summary.json"
EXPECTED_SECTIONS = {
    "objc3.runtime.image_info",
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
    "objc3.runtime.selector_pool",
    "objc3.runtime.string_pool",
    "objc3.runtime.discovery_root",
    "objc3.runtime.linker_anchor",
    "objc3.runtime.image_root",
    "objc3.runtime.registration_descriptor",
}


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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--llvm-readobj", default=shutil.which("llvm-readobj") or "llvm-readobj")
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


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    normalized_command = [host_arg(part) for part in command]
    return subprocess.run(
        normalized_command,
        cwd=host_path_str(ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def latest_summary(root: Path) -> Path | None:
    if not root.exists():
        return None
    summaries = list(root.glob("*/summary.json"))
    if not summaries:
        return None
    return max(summaries, key=lambda path: path.stat().st_mtime)


def parse_section_names(readobj_stdout: str) -> list[str]:
    names: list[str] = []
    for line in readobj_stdout.splitlines():
        stripped = line.strip()
        if stripped.startswith("Name: "):
            name = stripped.split("Name: ", 1)[1].split(" (", 1)[0].strip()
            names.append(name)
    return names


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    build = run_command(["npm.cmd", "run", "build:objc3c-native"])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "npm run build:objc3c-native", "M259-C001-DYN-build", f"native build failed: {build.stderr or build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M259-C001-DYN-native", "native executable missing after build", failures)

    replay = run_command(["pwsh", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", host_path_str(EXECUTION_REPLAY)])
    checks_total += 1
    checks_passed += require(replay.returncode == 0, display_path(EXECUTION_REPLAY), "M259-C001-DYN-replay", f"execution replay proof failed: {replay.stderr or replay.stdout}", failures)

    replay_summary_path = latest_summary(REPLAY_ROOT)
    checks_total += 1
    checks_passed += require(replay_summary_path is not None and replay_summary_path.exists(), display_path(REPLAY_ROOT), "M259-C001-DYN-replay-summary", "replay summary missing", failures)
    replay_payload: dict[str, Any] = {}
    run1_payload: dict[str, Any] = {}
    if replay_summary_path is not None and replay_summary_path.exists():
        replay_payload = load_json(replay_summary_path)
        checks_total += 3
        checks_passed += require(replay_payload.get("status") == "PASS", display_path(replay_summary_path), "M259-C001-DYN-replay-status", "replay status must be PASS", failures)
        checks_passed += require(replay_payload.get("run1_sha256") == replay_payload.get("run2_sha256"), display_path(replay_summary_path), "M259-C001-DYN-replay-hash", "replay hashes must match", failures)
        checks_passed += require(bool(replay_payload.get("run1_summary")) and bool(replay_payload.get("run2_summary")), display_path(replay_summary_path), "M259-C001-DYN-replay-summaries", "run1/run2 summaries must be present", failures)
        run1_summary = ROOT / str(replay_payload.get("run1_summary", ""))
        checks_total += 1
        checks_passed += require(run1_summary.exists(), display_path(run1_summary), "M259-C001-DYN-run1-summary", "run1 smoke summary missing", failures)
        if run1_summary.exists():
            run1_payload = load_json(run1_summary)
            checks_total += 3
            checks_passed += require(run1_payload.get("status") == "PASS", display_path(run1_summary), "M259-C001-DYN-run1-status", "run1 smoke summary must be PASS", failures)
            checks_passed += require(run1_payload.get("failed") == 0, display_path(run1_summary), "M259-C001-DYN-run1-failed", "run1 smoke summary must have zero failures", failures)
            checks_passed += require(int(run1_payload.get("passed", 0)) == int(run1_payload.get("total", -1)) and int(run1_payload.get("total", 0)) > 0, display_path(run1_summary), "M259-C001-DYN-run1-total", "run1 smoke summary must pass every case", failures)

    probe_root = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m259" / "c001" / f"probe-{uuid.uuid4().hex}"
    probe_root.mkdir(parents=True, exist_ok=True)
    out_dir = probe_root / "runnable-slice-a002"
    out_dir.mkdir(parents=True, exist_ok=True)
    compile_run = run_command([
        host_path_str(args.native_exe),
        host_path_str(A002_FIXTURE),
        "--out-dir",
        host_path_str(out_dir),
        "--emit-prefix",
        "module",
    ])
    checks_total += 1
    checks_passed += require(compile_run.returncode == 0, display_path(A002_FIXTURE), "M259-C001-DYN-a002-compile", f"A002 compile failed: {compile_run.stderr or compile_run.stdout}", failures)
    backend_path = out_dir / "module.object-backend.txt"
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    for artifact_id, artifact_path in (("backend", backend_path), ("manifest", manifest_path), ("ir", ir_path), ("obj", obj_path)):
        checks_total += 1
        checks_passed += require(artifact_path.exists(), display_path(artifact_path), f"M259-C001-DYN-a002-{artifact_id}", f"missing {artifact_id} artifact", failures)
    backend_text = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M259-C001-DYN-a002-backend", f"backend must be llvm-direct, saw {backend_text!r}", failures)

    readobj_result = run_command([args.llvm_readobj, "--sections", host_path_str(obj_path)])
    checks_total += 1
    checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), "M259-C001-DYN-readobj", f"llvm-readobj failed: {readobj_result.stderr or readobj_result.stdout}", failures)
    section_names = parse_section_names(readobj_result.stdout) if readobj_result.returncode == 0 else []
    checks_total += 1
    checks_passed += require(EXPECTED_SECTIONS.issubset(set(section_names)), display_path(obj_path), "M259-C001-DYN-sections", "A002 object is missing required runtime sections", failures)

    return checks_total, checks_passed, {
        "replay_summary": display_path(replay_summary_path) if replay_summary_path else "",
        "replay_status": replay_payload.get("status", ""),
        "run1_summary": replay_payload.get("run1_summary", ""),
        "run1_total": run1_payload.get("total"),
        "run1_passed": run1_payload.get("passed"),
        "binary_inspection": {
            "fixture": display_path(A002_FIXTURE),
            "out_dir": display_path(out_dir),
            "backend": backend_text,
            "sections": section_names,
            "manifest": display_path(manifest_path),
            "ir": display_path(ir_path),
            "object": display_path(obj_path),
        },
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks_total += 4
    checks_passed += ensure_snippets(
        EXPECTATIONS_DOC,
        (
            SnippetCheck("M259-C001-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-C001-DOC-02", "Freeze the current replay-proof and binary-inspection evidence boundary"),
            SnippetCheck("M259-C001-DOC-03", "The contract must explicitly hand off to `M259-C002`."),
            SnippetCheck("M259-C001-DOC-04", "tmp/reports/m259/M259-C001/end_to_end_replay_and_inspection_summary.json"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        PACKET_DOC,
        (
            SnippetCheck("M259-C001-PKT-01", "Packet: `M259-C001`"),
            SnippetCheck("M259-C001-PKT-02", "Issue: `#7212`"),
            SnippetCheck("M259-C001-PKT-03", "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3"),
            SnippetCheck("M259-C001-PKT-04", "Next issue: `M259-C002`."),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_SOURCE,
        (
            SnippetCheck("M259-C001-SRC-01", "## M259 end-to-end replay and inspection freeze (C001)"),
            SnippetCheck("M259-C001-SRC-02", "execution-smoke-plus-replay-proof-plus-a002-object-section-anchor"),
            SnippetCheck("M259-C001-SRC-03", "`M259-C002`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_NATIVE,
        (
            SnippetCheck("M259-C001-NDOC-01", "## M259 end-to-end replay and inspection freeze (C001)"),
            SnippetCheck("M259-C001-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-C001-NDOC-03", "`tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        LOWERING_SPEC,
        (
            SnippetCheck("M259-C001-SPC-01", "## M259 end-to-end replay and inspection freeze (C001)"),
            SnippetCheck("M259-C001-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-C001-SPC-03", f"`{FREEZE_MODEL}`"),
            SnippetCheck("M259-C001-SPC-04", "`M259-C002`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        METADATA_SPEC,
        (
            SnippetCheck("M259-C001-META-01", "## M259 replay and inspection metadata anchors (C001)"),
            SnippetCheck("M259-C001-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-C001-META-03", "`objc3.runtime.class_descriptors`"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        EXECUTION_SMOKE,
        (
            SnippetCheck("M259-C001-SMOKE-01", "M259-C001 replay-inspection-freeze anchor:"),
            SnippetCheck("M259-C001-SMOKE-02", "canonical runnable replay corpus boundary"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        EXECUTION_REPLAY,
        (
            SnippetCheck("M259-C001-REPLAY-01", "M259-C001 replay-inspection-freeze anchor:"),
            SnippetCheck("M259-C001-REPLAY-02", "canonical replay-proof artifact boundary"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        PACKAGE_JSON,
        (
            SnippetCheck("M259-C001-PKG-01", '"check:objc3c:m259-c001-end-to-end-replay-and-inspection-freeze"'),
            SnippetCheck("M259-C001-PKG-02", '"test:tooling:m259-c001-end-to-end-replay-and-inspection-freeze"'),
            SnippetCheck("M259-C001-PKG-03", '"check:objc3c:m259-c001-lane-c-readiness"'),
        ),
        failures,
    )

    a002_summary = load_json(A002_SUMMARY)
    checks_total += 2
    checks_passed += require(a002_summary.get("contract_id") == "objc3c-canonical-runnable-sample-set/m259-a002-v1", display_path(A002_SUMMARY), "M259-C001-A002-contract", "A002 contract drift", failures)
    checks_passed += require(a002_summary.get("ok") is True, display_path(A002_SUMMARY), "M259-C001-A002-ok", "A002 summary must remain green", failures)

    probe_details: dict[str, Any] = {"binary_inspection": {}}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, probe_details = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "freeze_model": FREEZE_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "dependency": {
            "M259-A002": {
                "summary": display_path(A002_SUMMARY),
                "contract_id": a002_summary.get("contract_id"),
                "ok": a002_summary.get("ok"),
            }
        },
        "probe_details": probe_details,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
