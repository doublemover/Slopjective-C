#!/usr/bin/env python3
"""Checker for M269-C001 task runtime lowering contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m269-c001-part7-task-runtime-lowering-contract-v1"
CONTRACT_ID = "objc3c-part7-task-runtime-lowering-contract/m269-c001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m269" / "M269-C001" / "task_runtime_lowering_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_task_runtime_lowering_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_c001_task_runtime_lowering_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_c001_task_runtime_lowering_positive.objc3"

EXPECTED_PACKET = {
    "contract_id": CONTRACT_ID,
    "surface_path": "frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract",
    "semantic_contract_id": "objc3c-part7-task-executor-cancellation-semantic-model/m269-b001-v1",
    "structured_semantic_contract_id": "objc3c-part7-structured-task-cancellation-semantics/m269-b002-v1",
    "executor_semantic_contract_id": "objc3c-part7-executor-hop-affinity-compatibility/m269-b003-v1",
    "actor_lane_contract_id": "m188-actor-isolation-sendability-lowering-v1",
    "task_runtime_lane_contract_id": "m189-task-runtime-interop-cancellation-lowering-v1",
    "concurrency_lane_contract_id": "m190-concurrency-replay-race-guard-lowering-v1",
    "task_creation_sites": 2,
    "task_group_scope_sites": 1,
    "task_group_add_task_sites": 1,
    "task_group_wait_next_sites": 1,
    "task_group_cancel_all_sites": 1,
    "detached_task_creation_sites": 1,
    "actor_isolation_sites": 2,
    "cross_actor_hop_sites": 1,
    "sendability_check_sites": 2,
    "task_runtime_sites": 14,
    "task_runtime_interop_sites": 14,
    "cancellation_probe_sites": 4,
    "cancellation_handler_sites": 1,
    "runtime_resume_sites": 1,
    "runtime_cancel_sites": 1,
    "task_runtime_normalized_sites": 14,
    "task_runtime_guard_blocked_sites": 0,
    "concurrency_replay_sites": 14,
    "replay_proof_sites": 3,
    "race_guard_sites": 5,
    "task_handoff_sites": 4,
    "deterministic_schedule_sites": 14,
    "concurrency_guard_blocked_sites": 0,
}

IR_SNIPPETS = [
    "; actor_isolation_sendability_lowering = actor_isolation_sites=2",
    "; task_runtime_interop_cancellation_lowering = task_runtime_sites=14",
    "; concurrency_replay_race_guard_lowering = concurrency_replay_sites=14",
    "; frontend_objc_actor_isolation_sendability_lowering_profile = actor_isolation_sites=2",
    "; frontend_objc_task_runtime_interop_cancellation_lowering_profile = task_runtime_sites=14",
    "; frontend_objc_concurrency_replay_race_guard_lowering_profile = concurrency_replay_sites=14",
    "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}",
    "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}",
    "!objc3.objc_actor_isolation_sendability_lowering = !{!41}",
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
        failures.append(Finding(display_path(path), "M269-C001-MISSING", f"missing artifact: {display_path(path)}"))
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
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def packet_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_task_runtime_lowering_contract"]


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_PACKET.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M269-C001-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(["actor_replay_key", "task_runtime_replay_key", "concurrency_replay_key"], start=40):
        total += 1
        passed += require(bool(payload.get(field)), artifact, f"M269-C001-PAYLOAD-{index:02d}", f"{field} missing", failures)
    for index, field in enumerate(["deterministic_handoff", "ready_for_ir_emission"], start=50):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M269-C001-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
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
        "m269-c001-readiness",
        "--summary-out",
        "tmp/reports/m269/M269-C001/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M269-C001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M269-C001-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "c001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    manifest_path = out_dir / "module.manifest.json"
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M269-C001-DYN-03", f"manifest missing after probe: {output}", failures)

    payload: dict[str, Any] = {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "manifest": display_path(manifest_path),
    }
    if manifest_path.exists():
        surface = packet_payload(manifest_path)
        sub_total, sub_passed = validate_payload(surface, display_path(manifest_path), failures)
        total += sub_total
        passed += sub_passed
        payload["task_runtime_lowering_contract"] = surface
    return total, passed, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M269-C001-EXP-01", "# M269 Task Runtime Lowering Contract And Architecture Freeze Expectations (C001)"),
            SnippetCheck("M269-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M269-C001-EXP-03", "frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract"),
        ],
        PACKET_DOC: [
            SnippetCheck("M269-C001-PKT-01", "# M269-C001 Packet: Task Runtime Lowering Contract - Contract And Architecture Freeze"),
            SnippetCheck("M269-C001-PKT-02", "thread replay-stable actor-isolation, task-runtime-interop, and concurrency-replay lowering metadata into emitted LLVM IR frontend metadata"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M269-C001-GRM-01", "## M269 task runtime lowering contract"),
            SnippetCheck("M269-C001-GRM-02", "frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M269-C001-DOC-01", "## M269 task runtime lowering contract"),
            SnippetCheck("M269-C001-DOC-02", "frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract"),
        ],
        SPEC_AM: [
            SnippetCheck("M269-C001-AM-01", "M269-C001 task-runtime lowering note:"),
            SnippetCheck("M269-C001-AM-02", "objc_part7_task_runtime_lowering_contract"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M269-C001-ATTR-01", "Current implementation status (`M269-C001`):"),
            SnippetCheck("M269-C001-ATTR-02", "frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract"),
        ],
        LOWERING_CONTRACT: [
            SnippetCheck("M269-C001-LWR-01", "M269-C001 lowering-freeze anchor:"),
            SnippetCheck("M269-C001-LWR-02", "kObjc3Part7TaskRuntimeLoweringContractId"),
            SnippetCheck("M269-C001-LWR-03", "kObjc3Part7TaskRuntimeLoweringSurfacePath"),
        ],
        IR_EMITTER: [
            SnippetCheck("M269-C001-IR-01", "; actor_isolation_sendability_lowering = "),
            SnippetCheck("M269-C001-IR-02", "; task_runtime_interop_cancellation_lowering = "),
            SnippetCheck("M269-C001-IR-03", "; concurrency_replay_race_guard_lowering = "),
            SnippetCheck("M269-C001-IR-04", "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}"),
            SnippetCheck("M269-C001-IR-05", "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}"),
            SnippetCheck("M269-C001-IR-06", "!objc3.objc_actor_isolation_sendability_lowering = !{!41}"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M269-C001-ART-01", "kObjc3Part7TaskRuntimeLoweringContractId"),
            SnippetCheck("M269-C001-ART-02", "BuildPart7TaskRuntimeLoweringContractJson("),
            SnippetCheck("M269-C001-ART-03", '"objc_part7_task_runtime_lowering_contract"'),
            SnippetCheck("M269-C001-ART-04", "part7_task_runtime_interop_cancellation_lowering_replay_key"),
            SnippetCheck("M269-C001-ART-05", "part7_concurrency_replay_race_guard_lowering_replay_key"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M269-C001-PKG-01", '"check:objc3c:m269-c001-task-runtime-lowering-contract-and-architecture-freeze"'),
            SnippetCheck("M269-C001-PKG-02", '"test:tooling:m269-c001-task-runtime-lowering-contract-and-architecture-freeze"'),
            SnippetCheck("M269-C001-PKG-03", '"check:objc3c:m269-c001-lane-c-readiness"'),
        ],
    }
    for path, path_snippets in snippets.items():
        total += len(path_snippets)
        passed += ensure_snippets(path, path_snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic_executed = False
    else:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        total += dyn_total
        passed += dyn_passed

    ok = not failures
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": total,
        "checks_passed": passed,
        "checks_failed": total - passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M269-C001 check passed ({passed}/{total} checks)")
        return 0
    print(f"[fail] M269-C001 check failed ({passed}/{total} checks)", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
