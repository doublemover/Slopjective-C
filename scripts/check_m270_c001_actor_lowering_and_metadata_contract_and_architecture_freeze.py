#!/usr/bin/env python3
"""Checker for M270-C001 actor lowering and metadata contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m270-c001-part7-actor-lowering-metadata-contract-v1"
CONTRACT_ID = "objc3c-part7-actor-lowering-and-metadata-contract/m270-c001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m270" / "M270-C001" / "actor_lowering_metadata_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m270_actor_lowering_and_metadata_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m270" / "m270_c001_actor_lowering_and_metadata_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_c001_actor_lowering_metadata_positive.objc3"

EXPECTED_PACKET = {
    "contract_id": CONTRACT_ID,
    "surface_path": "frontend.pipeline.semantic_surface.objc_part7_actor_lowering_and_metadata_contract",
    "source_contract_id": "objc3c-part7-actor-member-isolation-source-closure/m270-a002-v1",
    "semantic_contract_id": "objc3c-part7-actor-isolation-sendability-enforcement/m270-b002-v1",
    "hazard_contract_id": "objc3c-part7-actor-race-hazard-escape-diagnostics/m270-b003-v1",
    "lane_contract_id": "m270-actor-lowering-metadata-contract-v1",
    "actor_interface_sites": 1,
    "actor_method_sites": 2,
    "actor_metadata_record_sites": 3,
    "nonisolated_entry_sites": 2,
    "executor_affinity_sites": 1,
    "actor_hop_artifact_sites": 0,
    "actor_isolation_thunk_sites": 1,
    "replay_proof_dependency_sites": 1,
    "race_guard_dependency_sites": 1,
    "task_handoff_sites": 1,
    "guard_blocked_sites": 0,
    "contract_violation_sites": 0,
}

IR_SNIPPETS = [
    "; actor_lowering_metadata_contract = actor_interface_sites=1",
    "; frontend_objc_actor_lowering_metadata_profile = actor_interface_sites=1",
    "!objc3.objc_part7_actor_lowering_and_metadata = !{!97}",
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
        failures.append(Finding(display_path(path), "M270-C001-MISSING", f"missing artifact: {display_path(path)}"))
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
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_actor_lowering_and_metadata_contract"]


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_PACKET.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M270-C001-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M270-C001-PAYLOAD-40", "replay_key missing", failures)
    total += 1
    passed += require(payload.get("deterministic_handoff") is True, artifact, "M270-C001-PAYLOAD-41", "deterministic_handoff must stay true", failures)
    total += 1
    passed += require(payload.get("ready_for_ir_emission") is True, artifact, "M270-C001-PAYLOAD-42", "ready_for_ir_emission must stay true", failures)
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
        "m270-c001-readiness",
        "--summary-out",
        "tmp/reports/m270/M270-C001/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M270-C001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M270-C001-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m270" / "c001" / "positive"
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
    passed += require(run.returncode == 0, display_path(FIXTURE), "M270-C001-DYN-03", f"positive fixture failed: {output}", failures)
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M270-C001-DYN-04", "manifest missing", failures)
    total += 1
    passed += require(ir_path.exists(), display_path(ir_path), "M270-C001-DYN-05", "module.ll missing", failures)

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
        dynamic["actor_lowering_metadata_contract"] = payload
    if ir_path.exists():
        ir_text = read_text(ir_path)
        for index, snippet in enumerate(IR_SNIPPETS, start=60):
            total += 1
            passed += require(snippet in ir_text, display_path(ir_path), f"M270-C001-IR-{index}", f"missing IR snippet: {snippet}", failures)
    return total, passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M270-C001-EXP-01", "# M270 Actor Lowering And Metadata Contract And Architecture Freeze Expectations (C001)"),
            SnippetCheck("M270-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M270-C001-EXP-03", "frontend.pipeline.semantic_surface.objc_part7_actor_lowering_and_metadata_contract"),
        ],
        PACKET_DOC: [
            SnippetCheck("M270-C001-PKT-01", "# M270-C001 Packet: Actor Lowering And Metadata Contract - Contract And Architecture Freeze"),
            SnippetCheck("M270-C001-PKT-02", "thread the replay-stable actor lowering metadata into emitted LLVM IR frontend metadata"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M270-C001-GRM-01", "## M270 actor lowering and metadata contract"),
            SnippetCheck("M270-C001-GRM-02", "frontend.pipeline.semantic_surface.objc_part7_actor_lowering_and_metadata_contract"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M270-C001-DOC-01", "## M270 actor lowering and metadata contract"),
            SnippetCheck("M270-C001-DOC-02", "frontend.pipeline.semantic_surface.objc_part7_actor_lowering_and_metadata_contract"),
        ],
        SPEC_AM: [
            SnippetCheck("M270-C001-AM-01", "M270-C001 actor lowering note:"),
            SnippetCheck("M270-C001-AM-02", "objc_part7_actor_lowering_and_metadata_contract"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M270-C001-CHK-01", "M270-C001 lowering note:"),
            SnippetCheck("M270-C001-CHK-02", "objc_part7_actor_lowering_and_metadata_contract"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M270-C001-LWR-01", "M270-C001 lowering-freeze anchor:"),
            SnippetCheck("M270-C001-LWR-02", "kObjc3Part7ActorLoweringMetadataContractId"),
            SnippetCheck("M270-C001-LWR-03", "kObjc3Part7ActorLoweringMetadataSurfacePath"),
            SnippetCheck("M270-C001-LWR-04", "struct Objc3ActorLoweringMetadataContract"),
        ],
        LOWERING_CONTRACT_CPP: [
            SnippetCheck("M270-C001-LWR-05", "IsValidObjc3ActorLoweringMetadataContract"),
            SnippetCheck("M270-C001-LWR-06", "Objc3ActorLoweringMetadataReplayKey"),
        ],
        IR_HEADER: [
            SnippetCheck("M270-C001-IRH-01", "lowering_actor_lowering_metadata_replay_key"),
            SnippetCheck("M270-C001-IRH-02", "actor_lowering_metadata_actor_metadata_record_sites"),
        ],
        IR_EMITTER: [
            SnippetCheck("M270-C001-IR-01", "; actor_lowering_metadata_contract = "),
            SnippetCheck("M270-C001-IR-02", "; frontend_objc_actor_lowering_metadata_profile = actor_interface_sites="),
            SnippetCheck("M270-C001-IR-03", "!objc3.objc_part7_actor_lowering_and_metadata = !{!97}"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M270-C001-ART-01", "BuildPart7ActorLoweringMetadataContract"),
            SnippetCheck("M270-C001-ART-02", "BuildPart7ActorLoweringMetadataContractJson"),
            SnippetCheck("M270-C001-ART-03", '"objc_part7_actor_lowering_and_metadata_contract"'),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M270-C001-PKG-01", '"check:objc3c:m270-c001-actor-lowering-and-metadata-contract-and-architecture-freeze"'),
            SnippetCheck("M270-C001-PKG-02", '"test:tooling:m270-c001-actor-lowering-and-metadata-contract-and-architecture-freeze"'),
            SnippetCheck("M270-C001-PKG-03", '"check:objc3c:m270-c001-lane-c-readiness"'),
        ],
    }
    for path, checks in snippets.items():
        total += len(checks)
        passed += ensure_snippets(path, checks, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        total += dyn_total
        passed += dyn_passed

    ok = not failures
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "total_checks": total,
        "passed_checks": passed,
        "failed_checks": total - passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M270-C001 checker passed ({passed}/{total} checks)")
        return 0
    print(json.dumps(payload, indent=2))
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
