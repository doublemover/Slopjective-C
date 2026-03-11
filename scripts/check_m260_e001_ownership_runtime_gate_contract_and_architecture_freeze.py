#!/usr/bin/env python3
"""Fail-closed checker for M260-E001 ownership runtime gate freeze."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-e001-ownership-runtime-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-ownership-runtime-gate-freeze/m260-e001-v1"
SUPPORTED_MODEL = (
    "runtime-backed-object-baseline-proves-strong-weak-and-autoreleasepool-behavior-through-private-runtime-hooks"
)
EVIDENCE_MODEL = (
    "gate-consumes-m260-c002-d001-d002-contract-summaries-and-runtime-probe-evidence"
)
NON_GOAL_MODEL = "no-arc-automation-no-block-ownership-runtime-no-public-ownership-api-widening"
FAIL_CLOSED_MODEL = (
    "integration-gate-must-not-claim-more-than-the-supported-runtime-backed-ownership-baseline"
)
NEXT_CLOSEOUT_ISSUE = "M260-E002"

M260_C002_CONTRACT_ID = "objc3c-ownership-runtime-hook-emission/m260-c002-v1"
M260_D001_CONTRACT_ID = "objc3c-runtime-memory-management-api-freeze/m260-d001-v1"
M260_D002_CONTRACT_ID = "objc3c-runtime-memory-management-implementation/m260-d002-v1"

BOUNDARY_PREFIX = "; ownership_runtime_gate = "
NAMED_METADATA_PREFIX = "!objc3.objc_ownership_runtime_gate = !{!72}"

EXPECTATIONS_DOC = (
    ROOT / "docs" / "contracts" / "m260_ownership_runtime_gate_contract_and_architecture_freeze_e001_expectations.md"
)
PACKET_DOC = (
    ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_e001_ownership_runtime_gate_contract_and_architecture_freeze_packet.md"
)
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m260_d002_reference_counting_weak_autoreleasepool_positive.objc3"
)
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "e001"
C002_SUMMARY = ROOT / "tmp" / "reports" / "m260" / "M260-C002" / "ownership_runtime_hook_emission_summary.json"
D001_SUMMARY = ROOT / "tmp" / "reports" / "m260" / "M260-D001" / "runtime_memory_management_api_contract_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m260" / "M260-D002" / "reference_counting_weak_autoreleasepool_summary.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-E001" / "ownership_runtime_gate_contract_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck(
        "M260-E001-EXP-01",
        "# M260 Ownership Runtime Gate Contract And Architecture Freeze Expectations (E001)",
    ),
    SnippetCheck("M260-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck(
        "M260-E001-EXP-03",
        "tmp/reports/m260/M260-D002/reference_counting_weak_autoreleasepool_summary.json",
    ),
    SnippetCheck("M260-E001-EXP-04", "`M260-E002` must exercise this exact baseline"),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M260-E001-PKT-01",
        "# M260-E001 Ownership Runtime Gate Contract And Architecture Freeze Packet",
    ),
    SnippetCheck("M260-E001-PKT-02", "Issue: `#7177`"),
    SnippetCheck("M260-E001-PKT-03", "Packet: `M260-E001`"),
    SnippetCheck("M260-E001-PKT-04", "- `M260-C002`"),
    SnippetCheck("M260-E001-PKT-05", "- `M260-D002`"),
    SnippetCheck("M260-E001-PKT-06", "`M260-E002` is the first issue allowed"),
)
DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M260-E001-SRC-01", "## M260 ownership runtime gate freeze (E001)"),
    SnippetCheck("M260-E001-SRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M260-E001-SRC-03",
        "`tmp/reports/m260/M260-D002/reference_counting_weak_autoreleasepool_summary.json`",
    ),
    SnippetCheck("M260-E001-SRC-04", "`check:objc3c:m260-e001-lane-e-readiness`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M260-E001-NDOC-01", "## M260 ownership runtime gate freeze (E001)"),
    SnippetCheck("M260-E001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-E001-NDOC-03", "`M260-E002` must prove this exact baseline and no more"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M260-E001-SPC-01", "## M260 ownership runtime gate freeze (E001)"),
    SnippetCheck("M260-E001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-E001-SPC-03", f"`{SUPPORTED_MODEL}`"),
    SnippetCheck("M260-E001-SPC-04", f"`{EVIDENCE_MODEL}`"),
    SnippetCheck("M260-E001-SPC-05", f"`{FAIL_CLOSED_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck(
        "M260-E001-META-01",
        "## M260 ownership runtime gate metadata anchors (E001)",
    ),
    SnippetCheck("M260-E001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-E001-META-03", "`!objc3.objc_ownership_runtime_gate`"),
    SnippetCheck("M260-E001-META-04", "`M260-E002` must preserve and exercise this exact gate contract"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck(
        "M260-E001-ARCH-01",
        "M260-E001` freezes the supported ownership runtime slice for the lane-E gate.",
    ),
    SnippetCheck("M260-E001-ARCH-02", "`!objc3.objc_ownership_runtime_gate`"),
    SnippetCheck("M260-E001-ARCH-03", "the next issue is `M260-E002`"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M260-E001-SEMA-01", "M260-E001 ownership-runtime-gate freeze anchor"),
    SnippetCheck(
        "M260-E001-SEMA-02",
        "runtime baseline and leaves ARC/block/public-ABI widening as explicit",
    ),
)
IR_SNIPPETS = (
    SnippetCheck("M260-E001-IR-01", "M260-E001 ownership-runtime-gate freeze anchor"),
    SnippetCheck("M260-E001-IR-02", 'out << "; ownership_runtime_gate = "'),
    SnippetCheck("M260-E001-IR-03", '!objc3.objc_ownership_runtime_gate = !{!72}'),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M260-E001-LHDR-01", "kObjc3OwnershipRuntimeGateContractId"),
    SnippetCheck("M260-E001-LHDR-02", "kObjc3OwnershipRuntimeGateSupportedModel"),
    SnippetCheck("M260-E001-LHDR-03", "kObjc3OwnershipRuntimeGateEvidenceModel"),
    SnippetCheck("M260-E001-LHDR-04", "Objc3OwnershipRuntimeGateSummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M260-E001-LCPP-01", "Objc3OwnershipRuntimeGateSummary()"),
    SnippetCheck("M260-E001-LCPP-02", "M260-E001 ownership-runtime-gate freeze anchor"),
    SnippetCheck("M260-E001-LCPP-03", "ownership_hook_contract="),
    SnippetCheck("M260-E001-LCPP-04", "memory_implementation_contract="),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M260-E001-PKG-01", '"check:objc3c:m260-e001-ownership-runtime-gate-contract"'),
    SnippetCheck("M260-E001-PKG-02", '"test:tooling:m260-e001-ownership-runtime-gate-contract"'),
    SnippetCheck("M260-E001-PKG-03", '"check:objc3c:m260-e001-lane-e-readiness"'),
)


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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    artifact = display_path(path)
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(artifact, snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def boundary_line(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(BOUNDARY_PREFIX + "contract="):
            return line
    return ""


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--sema-cpp", type=Path, default=SEMA_CPP)
    parser.add_argument("--ir-cpp", type=Path, default=IR_CPP)
    parser.add_argument("--lowering-header", type=Path, default=LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=LOWERING_CPP)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--fixture", type=Path, default=FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=PROBE_ROOT)
    parser.add_argument("--c002-summary", type=Path, default=C002_SUMMARY)
    parser.add_argument("--d001-summary", type=Path, default=D001_SUMMARY)
    parser.add_argument("--d002-summary", type=Path, default=D002_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def validate_summary(
    name: str,
    path: Path,
    expected_contract_id: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(path.exists(), artifact, f"{name}-00", "summary must exist", failures)
    total += 1
    passed += require(payload.get("contract_id") == expected_contract_id, artifact, f"{name}-01", "contract id drifted", failures)
    total += 1
    if "ok" in payload:
        passed += require(payload.get("ok") is True, artifact, f"{name}-02", "summary must report ok=true", failures)
    else:
        passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{name}-02", "summary must report full check coverage", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{name}-03", "checks_passed must equal checks_total", failures)
    distilled: dict[str, Any] = {
        "contract_id": payload.get("contract_id"),
        "checks_passed": payload.get("checks_passed"),
        "checks_total": payload.get("checks_total"),
    }
    if "ok" in payload:
        distilled["ok"] = payload.get("ok")
    if "dynamic" in payload:
        distilled["dynamic"] = payload.get("dynamic")
    if "dynamic_case" in payload:
        distilled["dynamic_case"] = payload.get("dynamic_case")
    return total, passed, distilled


def validate_d002_runtime_evidence(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    dynamic = payload.get("dynamic")
    total = 0
    passed = 0
    total += 1
    passed += require(isinstance(dynamic, dict), artifact, "M260-E001-D002-01", "D002 must publish a dynamic evidence object", failures)
    if not isinstance(dynamic, dict):
        return total, passed, {"contract_id": payload.get("contract_id")}
    total += 1
    passed += require(dynamic.get("skipped") is False, artifact, "M260-E001-D002-02", "D002 dynamic evidence must remain live, not skipped", failures)
    boundary = str(dynamic.get("boundary", ""))
    total += 1
    passed += require(f"contract={M260_D002_CONTRACT_ID}" in boundary, artifact, "M260-E001-D002-03", "D002 boundary must publish the D002 contract id", failures)
    total += 1
    passed += require("push_autoreleasepool_symbol=objc3_runtime_push_autoreleasepool_scope" in boundary, artifact, "M260-E001-D002-04", "D002 boundary must publish the push helper symbol", failures)
    total += 1
    passed += require("pop_autoreleasepool_symbol=objc3_runtime_pop_autoreleasepool_scope" in boundary, artifact, "M260-E001-D002-05", "D002 boundary must publish the pop helper symbol", failures)
    probe_payload = dynamic.get("probe_payload", {})
    total += 1
    passed += require(isinstance(probe_payload, dict), artifact, "M260-E001-D002-06", "D002 must publish the runtime probe payload", failures)
    total += 1
    passed += require(
        isinstance(probe_payload.get("memory_after_pool"), dict)
        and probe_payload["memory_after_pool"].get("drained_autorelease_value_count") == 1,
        artifact,
        "M260-E001-D002-07",
        "D002 probe must prove one autoreleased value drains on pool pop",
        failures,
    )
    total += 1
    passed += require(
        isinstance(probe_payload.get("memory_after_parent_release"), dict)
        and probe_payload["memory_after_parent_release"].get("live_runtime_instance_count") == 0,
        artifact,
        "M260-E001-D002-08",
        "D002 probe must prove no live runtime instances remain after final release",
        failures,
    )
    return total, passed, {
        "contract_id": payload.get("contract_id"),
        "dynamic": {
            "skipped": dynamic.get("skipped"),
            "boundary": boundary,
            "probe_payload": probe_payload,
        },
    }


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    artifact = "dynamic_case"
    total = 0
    passed = 0

    total += 1
    passed += require(args.native_exe.exists(), artifact, "M260-E001-DYN-01", f"missing native executable: {display_path(args.native_exe)}", failures)
    total += 1
    passed += require(args.fixture.exists(), artifact, "M260-E001-DYN-02", f"missing fixture: {display_path(args.fixture)}", failures)
    if failures:
        return total, passed, {"skipped": False}

    probe_dir = args.probe_root / "gate"
    probe_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_command(
        [str(args.native_exe), str(args.fixture), "--out-dir", str(probe_dir), "--emit-prefix", "module"],
        ROOT,
    )
    module_ir = probe_dir / "module.ll"
    total += 1
    passed += require(
        compile_result.returncode == 0,
        artifact,
        "M260-E001-DYN-03",
        f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}",
        failures,
    )
    total += 1
    passed += require(module_ir.exists(), artifact, "M260-E001-DYN-04", f"missing emitted IR: {display_path(module_ir)}", failures)
    if compile_result.returncode != 0 or not module_ir.exists():
        return total, passed, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    ir_text = read_text(module_ir)
    boundary = boundary_line(ir_text)
    total += 1
    passed += require(bool(boundary), artifact, "M260-E001-DYN-05", "IR must publish the ownership runtime gate boundary", failures)
    total += 1
    passed += require(NAMED_METADATA_PREFIX in ir_text, artifact, "M260-E001-DYN-06", "IR must publish !objc3.objc_ownership_runtime_gate", failures)
    total += 1
    passed += require(f"contract={CONTRACT_ID}" in boundary, artifact, "M260-E001-DYN-07", "boundary must publish the E001 contract id", failures)
    total += 1
    passed += require(f"supported_model={SUPPORTED_MODEL}" in boundary, artifact, "M260-E001-DYN-08", "boundary must publish the supported model", failures)
    total += 1
    passed += require(f"evidence_model={EVIDENCE_MODEL}" in boundary, artifact, "M260-E001-DYN-09", "boundary must publish the evidence model", failures)
    total += 1
    passed += require(f"non_goal_model={NON_GOAL_MODEL}" in boundary, artifact, "M260-E001-DYN-10", "boundary must publish the non-goal model", failures)
    total += 1
    passed += require(f"fail_closed_model={FAIL_CLOSED_MODEL}" in boundary, artifact, "M260-E001-DYN-11", "boundary must publish the fail-closed model", failures)
    total += 1
    passed += require(f"ownership_hook_contract={M260_C002_CONTRACT_ID}" in boundary, artifact, "M260-E001-DYN-12", "boundary must publish the C002 dependency contract id", failures)
    total += 1
    passed += require(f"memory_api_contract={M260_D001_CONTRACT_ID}" in boundary, artifact, "M260-E001-DYN-13", "boundary must publish the D001 dependency contract id", failures)
    total += 1
    passed += require(f"memory_implementation_contract={M260_D002_CONTRACT_ID}" in boundary, artifact, "M260-E001-DYN-14", "boundary must publish the D002 dependency contract id", failures)

    return total, passed, {
        "skipped": False,
        "probe_dir": display_path(probe_dir),
        "module_ir": display_path(module_ir),
        "boundary": boundary,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    file_checks: tuple[tuple[Path, Sequence[SnippetCheck]], ...] = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.doc_source, DOC_SOURCE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_cpp, IR_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in file_checks:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream_summaries: dict[str, Any] = {}
    for name, path, contract_id in (
        ("M260-C002", args.c002_summary, M260_C002_CONTRACT_ID),
        ("M260-D001", args.d001_summary, M260_D001_CONTRACT_ID),
    ):
        total, passed, distilled = validate_summary(name, path, contract_id, failures)
        checks_total += total
        checks_passed += passed
        upstream_summaries[name] = distilled

    total, passed, distilled = validate_summary("M260-D002", args.d002_summary, M260_D002_CONTRACT_ID, failures)
    checks_total += total
    checks_passed += passed
    upstream_summaries["M260-D002"] = distilled

    total, passed, runtime_distilled = validate_d002_runtime_evidence(args.d002_summary, failures)
    checks_total += total
    checks_passed += passed
    upstream_summaries["M260-D002"] = runtime_distilled

    if args.skip_dynamic_probes:
        dynamic_case: dict[str, Any] = {"skipped": True}
        dynamic_probes_executed = False
    else:
        total, passed, dynamic_case = run_dynamic_case(args, failures)
        checks_total += total
        checks_passed += passed
        dynamic_probes_executed = True

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "upstream_summaries": upstream_summaries,
        "dynamic_case": dynamic_case,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(
        f"[ok] {CONTRACT_ID} passed {checks_passed}/{checks_total} checks"
        + (" (dynamic skipped)" if args.skip_dynamic_probes else ""),
        flush=True,
    )
    print(f"[ok] summary: {display_path(args.summary_out)}", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
