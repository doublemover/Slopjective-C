#!/usr/bin/env python3
"""Fail-closed checker for M254-E002 replay/bootstrap runbook closeout."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-e002-replay-bootstrap-runbook-closeout-v1"
CONTRACT_ID = "objc3c-runtime-replay-bootstrap-closeout/m254-e002-v1"
CLOSEOUT_MODEL = "e001-gate-plus-live-operator-runbook-smoke"
RUNBOOK_RUN_ID = "m254_e002_bootstrap_runbook_closeout"
E001_CONTRACT_ID = "objc3c-runtime-startup-registration-gate/m254-e001-v1"
E001_EVIDENCE_MODEL = "a002-b002-c003-d003-d004-summary-chain"
D004_CONTRACT_ID = "objc3c-runtime-launch-integration/m254-d004-v1"
EXPECTED_COMPILE_WRAPPER = "scripts/objc3c_native_compile.ps1"
EXPECTED_RUNTIME_LAUNCH_SCRIPT = "scripts/objc3c_runtime_launch_contract.ps1"
EXPECTED_RUNTIME_LIBRARY = "artifacts/lib/objc3_runtime.lib"
EXPECTED_NATIVE_EXE = "artifacts/bin/objc3c-native.exe"
EXPECTED_LLC_PATH = Path(r"C:\Program Files\LLVM\bin\llc.exe")

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_replay_and_bootstrap_proof_plus_runbook_closeout_e002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout_packet.md"
DEFAULT_RUNBOOK_DOC = ROOT / "docs" / "runbooks" / "m254_bootstrap_replay_operator_runbook.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m254_e002_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_E001_SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-E001" / "startup_registration_gate_summary.json"
DEFAULT_D004_SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-D004" / "runtime_launch_integration_summary.json"
DEFAULT_SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m254" / "M254-E002" / "replay_bootstrap_runbook_closeout_summary.json"


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
    SnippetCheck("M254-E002-DOC-EXP-01", "# M254 Replay And Bootstrap Proof Plus Runbook Closeout Expectations (E002)"),
    SnippetCheck("M254-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-E002-DOC-EXP-03", "docs/runbooks/m254_bootstrap_replay_operator_runbook.md"),
    SnippetCheck("M254-E002-DOC-EXP-04", "scripts/check_objc3c_native_execution_smoke.ps1"),
    SnippetCheck("M254-E002-DOC-EXP-05", "tmp/reports/m254/M254-E002/replay_bootstrap_runbook_closeout_summary.json"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M254-E002-DOC-PKT-01", "# M254-E002 Replay And Bootstrap Proof Plus Runbook Closeout Packet"),
    SnippetCheck("M254-E002-DOC-PKT-02", "Packet: `M254-E002`"),
    SnippetCheck("M254-E002-DOC-PKT-03", "- `M254-E001`"),
    SnippetCheck("M254-E002-DOC-PKT-04", "- `M254-D004`"),
    SnippetCheck("M254-E002-DOC-PKT-05", "`check:objc3c:m254-e002-lane-e-readiness`"),
)
RUNBOOK_SNIPPETS = (
    SnippetCheck("M254-E002-RUNBOOK-01", "# M254 Bootstrap Replay Operator Runbook"),
    SnippetCheck("M254-E002-RUNBOOK-02", "npm run check:objc3c:m254-e001-lane-e-readiness"),
    SnippetCheck("M254-E002-RUNBOOK-03", "$env:OBJC3C_NATIVE_EXECUTION_RUN_ID='m254_e002_bootstrap_runbook_closeout'"),
    SnippetCheck("M254-E002-RUNBOOK-04", r"$env:OBJC3C_NATIVE_EXECUTION_LLC_PATH='C:\Program Files\LLVM\bin\llc.exe'"),
    SnippetCheck("M254-E002-RUNBOOK-05", r"& 'C:\Program Files\PowerShell\7\pwsh.exe' -NoProfile -ExecutionPolicy Bypass -File .\scripts\check_objc3c_native_execution_smoke.ps1"),
    SnippetCheck("M254-E002-RUNBOOK-06", "tmp/artifacts/objc3c-native/execution-smoke/m254_e002_bootstrap_runbook_closeout/summary.json"),
    SnippetCheck("M254-E002-RUNBOOK-07", "npm run check:objc3c:m254-e002-lane-e-readiness"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M254-E002-NDOC-01", "## Replay and bootstrap proof plus runbook closeout (M254-E002)"),
    SnippetCheck("M254-E002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-E002-NDOC-03", f"`{CLOSEOUT_MODEL}`"),
    SnippetCheck("M254-E002-NDOC-04", "docs/runbooks/m254_bootstrap_replay_operator_runbook.md"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M254-E002-SPC-01", "## M254 replay and bootstrap proof plus runbook closeout (E002)"),
    SnippetCheck("M254-E002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-E002-SPC-03", f"`{CLOSEOUT_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M254-E002-META-01", "## M254 replay/bootstrap closeout metadata anchors (E002)"),
    SnippetCheck("M254-E002-META-02", "`module.runtime-registration-manifest.json`"),
    SnippetCheck("M254-E002-META-03", "`tmp/artifacts/objc3c-native/execution-smoke/m254_e002_bootstrap_runbook_closeout/summary.json`"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-E002-RTDOC-01", "`M254-E002` closes the milestone on top of the same replay/bootstrap evidence"),
    SnippetCheck("M254-E002-RTDOC-02", "`docs/runbooks/m254_bootstrap_replay_operator_runbook.md`"),
    SnippetCheck("M254-E002-RTDOC-03", "`scripts/check_objc3c_native_execution_smoke.ps1`"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-E002-DRV-01", "M254-E002 runbook-closeout anchor"),
    SnippetCheck("M254-E002-DRV-02", "published operator runbook"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-E002-ART-01", "M254-E002 runbook-closeout anchor"),
    SnippetCheck("M254-E002-ART-02", "registration manifest summary stays authoritative for the published runbook"),
)
PROCESS_SNIPPETS = (
    SnippetCheck("M254-E002-PROC-01", "M254-E002 runbook-closeout anchor"),
    SnippetCheck("M254-E002-PROC-02", "operator runbook must stay bound to this emitted launch contract"),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M254-E002-RUN-01", "check:objc3c:m254-e001-lane-e-readiness"),
    SnippetCheck("M254-E002-RUN-02", "check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py"),
    SnippetCheck("M254-E002-RUN-03", "test_check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M254-E002-PKG-01", '"check:objc3c:m254-e002-replay-and-bootstrap-proof-plus-runbook-closeout": "python scripts/check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py"'),
    SnippetCheck("M254-E002-PKG-02", '"test:tooling:m254-e002-replay-and-bootstrap-proof-plus-runbook-closeout": "python -m pytest tests/tooling/test_check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py -q"'),
    SnippetCheck("M254-E002-PKG-03", '"check:objc3c:m254-e002-lane-e-readiness": "python scripts/run_m254_e002_lane_e_readiness.py"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--runbook-doc", type=Path, default=DEFAULT_RUNBOOK_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--frontend-artifacts-cpp", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_CPP)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--e001-summary", type=Path, default=DEFAULT_E001_SUMMARY)
    parser.add_argument("--d004-summary", type=Path, default=DEFAULT_D004_SUMMARY)
    parser.add_argument("--smoke-script", type=Path, default=DEFAULT_SMOKE_SCRIPT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--runbook-run-id", default=RUNBOOK_RUN_ID)
    parser.add_argument("--llc-path", type=Path)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


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


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M254-E002-MISSING", f"required artifact is missing: {display_path(path)}"))
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def resolve_pwsh() -> str | None:
    return shutil.which("pwsh") or shutil.which("pwsh.exe") or shutil.which("powershell") or shutil.which("powershell.exe")


def resolve_llc_path(explicit: Path | None) -> Path | None:
    if explicit is not None:
        return explicit.resolve()
    if EXPECTED_LLC_PATH.exists():
        return EXPECTED_LLC_PATH.resolve()
    located = shutil.which("llc") or shutil.which("llc.exe")
    return Path(located).resolve() if located else None


def run_command(command: Sequence[str], env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        env=env,
        check=False,
    )


def validate_e001(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M254-E002-E001-01", "E001 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M254-E002-E001-02", "E001 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M254-E002-E001-03", "E001 summary must report dynamic probes executed", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == E001_CONTRACT_ID, artifact, "M254-E002-E001-04", "E001 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("evidence_model") == E001_EVIDENCE_MODEL, artifact, "M254-E002-E001-05", "E001 evidence model drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("next_closeout_issue") == "M254-E002", artifact, "M254-E002-E001-06", "E001 handoff issue drifted", failures)

    upstream = payload.get("upstream_evidence", {})
    d004 = upstream.get("M254-D004", {}) if isinstance(upstream, dict) else {}
    b002 = upstream.get("M254-B002", {}) if isinstance(upstream, dict) else {}
    c003 = upstream.get("M254-C003", {}) if isinstance(upstream, dict) else {}

    checks_total += 1
    checks_passed += require(d004.get("ok") is True, artifact, "M254-E002-E001-07", "E001 must retain passing D004 evidence", failures)
    checks_total += 1
    checks_passed += require(d004.get("smoke_status") == "PASS", artifact, "M254-E002-E001-08", "E001 must retain D004 smoke PASS", failures)
    checks_total += 1
    checks_passed += require(b002.get("duplicate_status") == -2, artifact, "M254-E002-E001-09", "E001 must retain duplicate-registration status -2", failures)
    checks_total += 1
    checks_passed += require(b002.get("out_of_order_status") == -3, artifact, "M254-E002-E001-10", "E001 must retain out-of-order status -3", failures)
    case_ids = c003.get("case_ids", []) if isinstance(c003, dict) else []
    checks_total += 1
    checks_passed += require("metadata-library" in case_ids and "category-library" in case_ids, artifact, "M254-E002-E001-11", "E001 must retain both C003 case ids", failures)

    distilled = {
        "ok": True,
        "contract_id": payload.get("contract_id"),
        "evidence_model": payload.get("evidence_model"),
        "next_closeout_issue": payload.get("next_closeout_issue"),
        "d004_smoke_status": d004.get("smoke_status"),
    }
    return checks_total, checks_passed, distilled


def validate_d004(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M254-E002-D004-01", "D004 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M254-E002-D004-02", "D004 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M254-E002-D004-03", "D004 summary must report dynamic probes executed", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == D004_CONTRACT_ID, artifact, "M254-E002-D004-04", "D004 contract id drifted", failures)

    smoke_case = payload.get("smoke_case", {})
    smoke_summary_path = ROOT / str(smoke_case.get("summary", ""))
    checks_total += 1
    checks_passed += require(smoke_summary_path.exists(), artifact, "M254-E002-D004-05", "D004 smoke summary path is missing", failures)

    smoke_summary = load_json(smoke_summary_path) if smoke_summary_path.exists() else {}
    smoke_artifact = display_path(smoke_summary_path) if smoke_summary_path.exists() else artifact
    checks_total += 1
    checks_passed += require(smoke_summary.get("status") == "PASS", smoke_artifact, "M254-E002-D004-06", "D004 smoke summary must report PASS", failures)
    checks_total += 1
    checks_passed += require(smoke_summary.get("compile_wrapper") == EXPECTED_COMPILE_WRAPPER, smoke_artifact, "M254-E002-D004-07", "D004 smoke compile wrapper drifted", failures)
    checks_total += 1
    checks_passed += require(smoke_summary.get("runtime_launch_contract_script") == EXPECTED_RUNTIME_LAUNCH_SCRIPT, smoke_artifact, "M254-E002-D004-08", "D004 smoke launch script drifted", failures)
    checks_total += 1
    checks_passed += require(smoke_summary.get("runtime_library") == EXPECTED_RUNTIME_LIBRARY, smoke_artifact, "M254-E002-D004-09", "D004 smoke runtime library drifted", failures)
    checks_total += 1
    checks_passed += require(smoke_summary.get("native_exe") == EXPECTED_NATIVE_EXE, smoke_artifact, "M254-E002-D004-10", "D004 smoke native exe drifted", failures)
    checks_total += 1
    checks_passed += require(int(smoke_summary.get("total", 0)) > 0, smoke_artifact, "M254-E002-D004-11", "D004 smoke must exercise at least one case", failures)
    checks_total += 1
    checks_passed += require(smoke_summary.get("failed") == 0, smoke_artifact, "M254-E002-D004-12", "D004 smoke must retain zero failures", failures)

    distilled = {
        "ok": True,
        "contract_id": payload.get("contract_id"),
        "smoke_summary": display_path(smoke_summary_path),
        "compile_wrapper": smoke_summary.get("compile_wrapper"),
        "runtime_launch_contract_script": smoke_summary.get("runtime_launch_contract_script"),
        "runtime_library": smoke_summary.get("runtime_library"),
        "native_exe": smoke_summary.get("native_exe"),
        "status": smoke_summary.get("status"),
    }
    return checks_total, checks_passed, distilled


def run_runbook_smoke(
    smoke_script: Path,
    run_id: str,
    llc_path: Path | None,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = display_path(smoke_script)

    pwsh = resolve_pwsh()
    checks_total += 1
    checks_passed += require(pwsh is not None, artifact, "M254-E002-DYN-01", "PowerShell executable is unavailable", failures)

    llc = llc_path
    checks_total += 1
    checks_passed += require(llc is not None and llc.exists(), artifact, "M254-E002-DYN-02", "llc executable is unavailable", failures)

    if pwsh is None or llc is None or not llc.exists():
        return checks_total, checks_passed, {}

    env = os.environ.copy()
    env["OBJC3C_NATIVE_EXECUTION_RUN_ID"] = run_id
    env["OBJC3C_NATIVE_EXECUTION_LLC_PATH"] = str(llc)

    completed = run_command(
        [pwsh, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(smoke_script)],
        env=env,
    )

    checks_total += 1
    checks_passed += require(completed.returncode == 0, artifact, "M254-E002-DYN-03", f"runbook smoke command failed with exit code {completed.returncode}", failures)

    smoke_summary_path = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke" / run_id / "summary.json"
    checks_total += 1
    checks_passed += require(smoke_summary_path.exists(), artifact, "M254-E002-DYN-04", "runbook smoke summary is missing", failures)
    if not smoke_summary_path.exists():
        return checks_total, checks_passed, {
            "run_id": run_id,
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }

    summary = load_json(smoke_summary_path)
    smoke_artifact = display_path(smoke_summary_path)
    checks_total += 1
    checks_passed += require(summary.get("status") == "PASS", smoke_artifact, "M254-E002-DYN-05", "runbook smoke summary must report PASS", failures)
    checks_total += 1
    checks_passed += require(summary.get("compile_wrapper") == EXPECTED_COMPILE_WRAPPER, smoke_artifact, "M254-E002-DYN-06", "runbook smoke compile wrapper drifted", failures)
    checks_total += 1
    checks_passed += require(summary.get("runtime_launch_contract_script") == EXPECTED_RUNTIME_LAUNCH_SCRIPT, smoke_artifact, "M254-E002-DYN-07", "runbook smoke launch script drifted", failures)
    checks_total += 1
    checks_passed += require(summary.get("runtime_library") == EXPECTED_RUNTIME_LIBRARY, smoke_artifact, "M254-E002-DYN-08", "runbook smoke runtime library drifted", failures)
    checks_total += 1
    checks_passed += require(summary.get("native_exe") == EXPECTED_NATIVE_EXE, smoke_artifact, "M254-E002-DYN-09", "runbook smoke native exe drifted", failures)
    checks_total += 1
    checks_passed += require(int(summary.get("passed", 0)) == int(summary.get("total", -1)), smoke_artifact, "M254-E002-DYN-10", "runbook smoke must pass every case", failures)
    checks_total += 1
    checks_passed += require(int(summary.get("failed", 0)) == 0, smoke_artifact, "M254-E002-DYN-11", "runbook smoke must retain zero failures", failures)
    checks_total += 1
    checks_passed += require(
        str(summary.get("llc_source", "")).lower().endswith("llvm\\bin\\llc.exe"),
        smoke_artifact,
        "M254-E002-DYN-12",
        "runbook smoke llc source must stay rooted on llvm/bin/llc.exe",
        failures,
    )

    distilled = {
        "run_id": run_id,
        "summary": display_path(smoke_summary_path),
        "status": summary.get("status"),
        "total": summary.get("total"),
        "passed": summary.get("passed"),
        "failed": summary.get("failed"),
        "compile_wrapper": summary.get("compile_wrapper"),
        "runtime_launch_contract_script": summary.get("runtime_launch_contract_script"),
        "runtime_library": summary.get("runtime_library"),
        "native_exe": summary.get("native_exe"),
        "llc_source": summary.get("llc_source"),
    }
    return checks_total, checks_passed, distilled


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.runbook_doc, RUNBOOK_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.frontend_artifacts_cpp, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.process_cpp, PROCESS_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    extra_total, extra_passed, e001_distilled = validate_e001(args.e001_summary, failures)
    checks_total += extra_total
    checks_passed += extra_passed

    extra_total, extra_passed, d004_distilled = validate_d004(args.d004_summary, failures)
    checks_total += extra_total
    checks_passed += extra_passed

    dynamic_probes_executed = not args.skip_dynamic_probes
    runbook_probe: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        llc_path = resolve_llc_path(args.llc_path)
        extra_total, extra_passed, runbook_probe = run_runbook_smoke(args.smoke_script, args.runbook_run_id, llc_path, failures)
        checks_total += extra_total
        checks_passed += extra_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "closeout_model": CLOSEOUT_MODEL,
        "runbook": display_path(args.runbook_doc),
        "upstream_evidence": {
            "M254-E001": e001_distilled,
            "M254-D004": d004_distilled,
        },
        "runbook_probe": runbook_probe,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[FAIL] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
