#!/usr/bin/env python3
"""Checker for M265-C001 Part 3 lowering freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m265-c001-part3-lowering-freeze-v1"
CONTRACT_ID = "objc3c-part3-optional-keypath-lowering/m265-c001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part3_optional_keypath_lowering_contract"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-C001" / "optional_and_key_path_lowering_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_optional_and_key_path_lowering_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_c001_optional_and_key_path_lowering_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_PART3 = ROOT / "spec" / "PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
OPTIONAL_FLOW_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_flow_binding_refinement_positive.objc3"
OPTIONAL_SHORT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_send_argument_short_circuit_positive.objc3"
KEYPATH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_class_root_positive.objc3"


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
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M265-C001-MISSING", f"missing artifact: {display_path(path)}"))
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
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def packet_from_manifest(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    packet = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_part3_optional_keypath_lowering_contract")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return packet


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-C001-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-C001-DIAG-empty", "expected zero diagnostics", failures)
    return checks_total, checks_passed


def validate_packet(packet: dict[str, Any], artifact: str, expected: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    shared = {
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "ready_for_native_optional_lowering": True,
        "ready_for_typed_keypath_artifact_emission": True,
        "contract_violation_sites": 0,
    }
    for key, value in {**shared, **expected}.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == value, artifact, f"M265-C001-PKT-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(packet.get("deterministic") is True, artifact, "M265-C001-PKT-deterministic", "deterministic must be true", failures)
    checks_total += 1
    checks_passed += require("lane_contract=m265-part3-optional-keypath-lowering-v1" in str(packet.get("replay_key", "")), artifact, "M265-C001-PKT-replay", "replay key missing lane contract", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m265-c001-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-C001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-C001-DYN-build", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M265-C001-DYN-native", "native driver missing after build", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M265-C001-DYN-runner", "frontend runner missing after build", failures)

    native_flow_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "c001" / "native-optional-flow"
    native_flow_out.mkdir(parents=True, exist_ok=True)
    native_flow = run_command([str(args.native_exe), str(OPTIONAL_FLOW_FIXTURE), "--out-dir", str(native_flow_out), "--emit-prefix", "module"])
    checks_total += 1
    checks_passed += require(native_flow.returncode == 0, display_path(OPTIONAL_FLOW_FIXTURE), "M265-C001-DYN-native-flow-rc", f"native optional-flow probe failed: {native_flow.stderr or native_flow.stdout}", failures)
    flow_diag = native_flow_out / "module.diagnostics.json"
    flow_manifest = native_flow_out / "module.manifest.json"
    flow_ir = native_flow_out / "module.ll"
    flow_backend = native_flow_out / "module.object-backend.txt"
    checks_total += 1
    checks_passed += require(flow_manifest.exists(), display_path(flow_manifest), "M265-C001-DYN-native-flow-manifest", "native optional-flow manifest missing", failures)
    checks_total += 1
    checks_passed += require(flow_ir.exists(), display_path(flow_ir), "M265-C001-DYN-native-flow-ir", "native optional-flow IR missing", failures)
    checks_total += 1
    checks_passed += require(flow_backend.exists(), display_path(flow_backend), "M265-C001-DYN-native-flow-backend", "native optional-flow backend note missing", failures)
    if flow_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(flow_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    if flow_manifest.exists():
        flow_packet = packet_from_manifest(flow_manifest)
        sub_total, sub_passed = validate_packet(flow_packet, display_path(flow_manifest), {
            "optional_binding_sites": 2,
            "optional_binding_clause_sites": 2,
            "optional_send_sites": 1,
            "nil_coalescing_sites": 2,
            "typed_keypath_literal_sites": 0,
            "typed_keypath_self_root_sites": 0,
            "typed_keypath_class_root_sites": 0,
            "live_optional_lowering_sites": 5,
            "single_evaluation_nil_short_circuit_sites": 5,
            "deferred_typed_keypath_sites": 0,
        }, failures)
        checks_total += sub_total
        checks_passed += sub_passed
        evidence["native_optional_flow_packet"] = {k: flow_packet[k] for k in [
            "optional_binding_sites",
            "optional_binding_clause_sites",
            "optional_send_sites",
            "nil_coalescing_sites",
            "live_optional_lowering_sites",
            "single_evaluation_nil_short_circuit_sites",
        ]}
    if flow_backend.exists():
        checks_total += 1
        checks_passed += require(flow_backend.read_text(encoding="utf-8").strip() == "llvm-direct", display_path(flow_backend), "M265-C001-DYN-native-flow-backend-value", "expected llvm-direct object backend", failures)
    if flow_ir.exists():
        checks_total += 1
        checks_passed += require("part3_optional_keypath_lowering = contract_id=objc3c-part3-optional-keypath-lowering/m265-c001-v1" in flow_ir.read_text(encoding="utf-8"), display_path(flow_ir), "M265-C001-DYN-native-flow-ir-marker", "IR marker missing", failures)

    short_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "c001" / "optional-short-circuit"
    short_out.mkdir(parents=True, exist_ok=True)
    short_build = run_command([str(args.native_exe), str(OPTIONAL_SHORT_FIXTURE), "--out-dir", str(short_out), "--emit-prefix", "module"])
    checks_total += 1
    checks_passed += require(short_build.returncode == 0, display_path(OPTIONAL_SHORT_FIXTURE), "M265-C001-DYN-short-rc", f"optional short-circuit build failed: {short_build.stderr or short_build.stdout}", failures)
    short_diag = short_out / "module.diagnostics.json"
    short_exe = short_out / "module.exe"
    short_manifest = short_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(short_exe.exists(), display_path(short_exe), "M265-C001-DYN-short-exe", "optional short-circuit executable missing", failures)
    if short_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(short_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    if short_manifest.exists():
        short_packet = packet_from_manifest(short_manifest)
        sub_total, sub_passed = validate_packet(short_packet, display_path(short_manifest), {
            "optional_binding_sites": 0,
            "optional_binding_clause_sites": 0,
            "optional_send_sites": 1,
            "nil_coalescing_sites": 0,
            "typed_keypath_literal_sites": 0,
            "typed_keypath_self_root_sites": 0,
            "typed_keypath_class_root_sites": 0,
            "live_optional_lowering_sites": 1,
            "single_evaluation_nil_short_circuit_sites": 1,
            "deferred_typed_keypath_sites": 0,
        }, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    if short_exe.exists():
        short_run = run_command([str(short_exe)])
        checks_total += 1
        checks_passed += require(short_run.returncode == 0, display_path(short_exe), "M265-C001-DYN-short-run", f"optional short-circuit program exited {short_run.returncode}", failures)
        evidence["optional_short_circuit_exit_code"] = short_run.returncode

    keypath_source_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "c001" / "keypath-source-only"
    keypath_source_out.mkdir(parents=True, exist_ok=True)
    keypath_source = run_command([str(args.runner_exe), str(KEYPATH_FIXTURE), "--out-dir", str(keypath_source_out), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    checks_total += 1
    checks_passed += require(keypath_source.returncode == 0, display_path(KEYPATH_FIXTURE), "M265-C001-DYN-keypath-source-rc", f"keypath source-only probe failed: {keypath_source.stderr or keypath_source.stdout}", failures)
    keypath_source_manifest = keypath_source_out / "module.manifest.json"
    keypath_source_diag = keypath_source_out / "module.diagnostics.json"
    if keypath_source_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(keypath_source_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    if keypath_source_manifest.exists():
        keypath_packet = packet_from_manifest(keypath_source_manifest)
        sub_total, sub_passed = validate_packet(keypath_packet, display_path(keypath_source_manifest), {
            "optional_binding_sites": 0,
            "optional_binding_clause_sites": 0,
            "optional_send_sites": 0,
            "nil_coalescing_sites": 0,
            "typed_keypath_literal_sites": 1,
            "typed_keypath_self_root_sites": 0,
            "typed_keypath_class_root_sites": 1,
            "live_optional_lowering_sites": 0,
            "single_evaluation_nil_short_circuit_sites": 0,
            "live_typed_keypath_artifact_sites": 1,
            "deferred_typed_keypath_sites": 0,
        }, failures)
        checks_total += sub_total
        checks_passed += sub_passed
        evidence["typed_keypath_source_packet"] = {k: keypath_packet[k] for k in [
            "typed_keypath_literal_sites",
            "typed_keypath_class_root_sites",
            "live_typed_keypath_artifact_sites",
            "deferred_typed_keypath_sites",
            "ready_for_typed_keypath_artifact_emission",
        ]}

    keypath_native_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "c001" / "native-keypath-positive"
    keypath_native_out.mkdir(parents=True, exist_ok=True)
    keypath_native = run_command([str(args.native_exe), str(KEYPATH_FIXTURE), "--out-dir", str(keypath_native_out), "--emit-prefix", "module"])
    checks_total += 1
    checks_passed += require(keypath_native.returncode == 0, display_path(KEYPATH_FIXTURE), "M265-C001-DYN-keypath-native-rc", f"native keypath probe failed: {keypath_native.stderr or keypath_native.stdout}", failures)
    keypath_native_manifest = keypath_native_out / "module.manifest.json"
    keypath_native_ir = keypath_native_out / "module.ll"
    keypath_native_obj = keypath_native_out / "module.obj"
    keypath_native_diag = keypath_native_out / "module.diagnostics.json"
    if keypath_native_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(keypath_native_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    checks_total += 1
    checks_passed += require(keypath_native_ir.exists(), display_path(keypath_native_ir), "M265-C001-DYN-keypath-native-ir", "native keypath IR missing", failures)
    if keypath_native_ir.exists():
        ir_text = keypath_native_ir.read_text(encoding="utf-8")
        checks_total += 1
        checks_passed += require("@__objc3_sec_keypath_descriptors" in ir_text, display_path(keypath_native_ir), "M265-C001-DYN-keypath-native-section", "keypath descriptor aggregate missing", failures)
        checks_total += 1
        checks_passed += require("typed_keypath_artifact_emission = " in ir_text, display_path(keypath_native_ir), "M265-C001-DYN-keypath-native-comment", "typed keypath artifact emission comment missing", failures)
    checks_total += 1
    checks_passed += require(keypath_native_obj.exists(), display_path(keypath_native_obj), "M265-C001-DYN-keypath-native-obj", "native keypath object missing", failures)
    if keypath_native_manifest.exists():
        keypath_packet = packet_from_manifest(keypath_native_manifest)
        sub_total, sub_passed = validate_packet(keypath_packet, display_path(keypath_native_manifest), {
            "optional_binding_sites": 0,
            "optional_binding_clause_sites": 0,
            "optional_send_sites": 0,
            "nil_coalescing_sites": 0,
            "typed_keypath_literal_sites": 1,
            "typed_keypath_self_root_sites": 0,
            "typed_keypath_class_root_sites": 1,
            "live_optional_lowering_sites": 0,
            "single_evaluation_nil_short_circuit_sites": 0,
            "live_typed_keypath_artifact_sites": 1,
            "deferred_typed_keypath_sites": 0,
        }, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    evidence["typed_keypath_native_artifact_emission"] = {
        "returncode": keypath_native.returncode,
        "manifest": display_path(keypath_native_manifest),
        "ir": display_path(keypath_native_ir),
        "object": display_path(keypath_native_obj),
    }

    return checks_total, checks_passed, evidence


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_snippets: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M265-C001-EXPECT-01", "# M265-C001 Expectations"),
            SnippetCheck("M265-C001-EXPECT-02", CONTRACT_ID),
            SnippetCheck("M265-C001-EXPECT-03", SURFACE_PATH),
        ],
        PACKET_DOC: [
            SnippetCheck("M265-C001-PACKET-01", "# M265-C001 - Optional and key-path lowering contract"),
            SnippetCheck("M265-C001-PACKET-02", "optional-send argument short-circuit on a nil"),
            SnippetCheck("M265-C001-PACKET-03", "validated typed key-path literals now lower into"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M265-C001-H-01", "kObjc3Part3OptionalKeypathLoweringContractId"),
            SnippetCheck("M265-C001-H-02", "struct Objc3Part3OptionalKeypathLoweringContract"),
            SnippetCheck("M265-C001-H-03", "Objc3Part3OptionalKeypathLoweringReplayKey"),
        ],
        LOWERING_CONTRACT_CPP: [
            SnippetCheck("M265-C001-CPP-01", "Objc3Part3OptionalKeypathLoweringSummary"),
            SnippetCheck("M265-C001-CPP-02", "contract.live_typed_keypath_artifact_sites +"),
            SnippetCheck("M265-C001-CPP-03", "single_evaluation_nil_short_circuit_sites"),
        ],
        IR_EMITTER_CPP: [
            SnippetCheck("M265-C001-IR-01", "MaterializeMessageSendArgs"),
            SnippetCheck("M265-C001-IR-02", "optional_send_enabled"),
            SnippetCheck("M265-C001-IR-03", "part3_optional_keypath_lowering = "),
        ],
        FRONTEND_ARTIFACTS_CPP: [
            SnippetCheck("M265-C001-FA-01", "objc_part3_optional_keypath_lowering_contract"),
            SnippetCheck("M265-C001-FA-02", "BuildPart3OptionalKeypathLoweringContractJson"),
            SnippetCheck("M265-C001-FA-03", "ready_for_typed_keypath_artifact_emission"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M265-C001-DOCSRC-01", "Current implementation status (`M265-C001`)"),
            SnippetCheck("M265-C001-DOCSRC-02", "optional sends now lower natively with single-evaluation nil"),
            SnippetCheck("M265-C001-DOCSRC-03", "typed key-path literals now lower natively for the validated"),
        ],
        SPEC_AM: [
            SnippetCheck("M265-C001-SPECAM-01", "Lane C now carries the first lowering-owned Part 3 packet"),
            SnippetCheck("M265-C001-SPECAM-02", "Optional sends now lower natively"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M265-C001-SPECATTR-01", "Current implementation status (`M265-C001`)"),
            SnippetCheck("M265-C001-SPECATTR-02", "optional sends written as `[receiver? selector]`"),
        ],
        SPEC_PART3: [
            SnippetCheck("M265-C001-SPECP3-01", "Implementation note (`M265-C001`)"),
            SnippetCheck("M265-C001-SPECP3-02", "Typed key-path literals now lower on the native path"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M265-C001-PKG-01", "check:objc3c:m265-c001-optional-and-key-path-lowering-contract-and-architecture-freeze"),
            SnippetCheck("M265-C001-PKG-02", "test:tooling:m265-c001-optional-and-key-path-lowering-contract-and-architecture-freeze"),
            SnippetCheck("M265-C001-PKG-03", "check:objc3c:m265-c001-lane-c-readiness"),
        ],
    }

    for path, snippets in static_snippets.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_evidence: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        sub_total, sub_passed, dynamic_evidence = run_dynamic_probes(args, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    summary = {
        "ok": not failures,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_evidence": dynamic_evidence,
        "findings": [asdict(f) for f in failures],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        json.dump(summary, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
