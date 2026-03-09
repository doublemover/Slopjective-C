#!/usr/bin/env python3
"""Fail-closed checker for M255-E002 live-dispatch smoke/replay closeout."""

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
MODE = "m255-e002-live-dispatch-smoke-replay-closeout-v1"
CONTRACT_ID = "objc3c-runtime-live-dispatch-smoke-replay-closeout/m255-e002-v1"
CLOSEOUT_MODEL = "live-runtime-dispatch-smoke-and-replay-authoritative-shim-evidence-non-authoritative"
CANONICAL_SYMBOL = "objc3_runtime_dispatch_i32"
COMPATIBILITY_SYMBOL = "objc3_msgsend_i32"
SMOKE_RUN_ID = "m255_e002_live_dispatch_smoke"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m255_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync_e002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_POSITIVE_README = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive" / "README.md"
DEFAULT_NEGATIVE_README = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative" / "README.md"
DEFAULT_SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
DEFAULT_REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SHIM_C = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m255_e002_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_E001_SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-E001" / "live_dispatch_gate_summary.json"
DEFAULT_SMOKE_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke"
DEFAULT_REPLAY_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-replay-proof"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-E002" / "live_dispatch_smoke_replay_closeout_summary.json"


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
    SnippetCheck("M255-E002-DOC-EXP-01", "# M255 Replace Shim-Based Execution Smoke With Live Dispatch Evidence Cross-Lane Integration Sync Expectations (E002)"),
    SnippetCheck("M255-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-E002-DOC-EXP-03", "requires_live_runtime_dispatch"),
    SnippetCheck("M255-E002-DOC-EXP-04", "tmp/reports/m255/M255-E002/live_dispatch_smoke_replay_closeout_summary.json"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-E002-DOC-PKT-01", "# M255-E002 Replace Shim-Based Execution Smoke With Live Dispatch Evidence Cross-Lane Integration Sync Packet"),
    SnippetCheck("M255-E002-DOC-PKT-02", "Packet: `M255-E002`"),
    SnippetCheck("M255-E002-DOC-PKT-03", "execution.requires_live_runtime_dispatch"),
    SnippetCheck("M255-E002-DOC-PKT-04", "objc3_runtime_dispatch_i32"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-E002-NDOC-01", "## Live dispatch smoke and replay closeout (M255-E002)"),
    SnippetCheck("M255-E002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-E002-NDOC-03", "`requires_live_runtime_dispatch`"),
    SnippetCheck("M255-E002-NDOC-04", "tmp/artifacts/objc3c-native/execution-smoke/m255_e002_live_dispatch_smoke/summary.json"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-E002-SPC-01", "## M255 live dispatch smoke and replay closeout (E002)"),
    SnippetCheck("M255-E002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-E002-SPC-03", "`objc3_runtime_dispatch_i32`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-E002-META-01", "## M255 live dispatch smoke/replay metadata anchors (E002)"),
    SnippetCheck("M255-E002-META-02", "`requires_live_runtime_dispatch`"),
    SnippetCheck("M255-E002-META-03", "`compatibility_runtime_shim`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M255-E002-ARCH-01", "## M255 live dispatch smoke and replay closeout (E002)"),
    SnippetCheck("M255-E002-ARCH-02", "execution smoke and replay now treat the live runtime path as authoritative"),
)
POSITIVE_README_SNIPPETS = (
    SnippetCheck("M255-E002-POS-01", "execution.requires_live_runtime_dispatch"),
    SnippetCheck("M255-E002-POS-02", "objc3_runtime_dispatch_i32"),
    SnippetCheck("M255-E002-POS-03", "compatibility shim remains test-only evidence"),
)
NEGATIVE_README_SNIPPETS = (
    SnippetCheck("M255-E002-NEG-01", "execution.requires_live_runtime_dispatch"),
    SnippetCheck("M255-E002-NEG-02", "unresolved symbol diagnostics for `objc3_runtime_dispatch_i32`"),
)
SMOKE_SCRIPT_SNIPPETS = (
    SnippetCheck("M255-E002-SMK-01", "requires_live_runtime_dispatch"),
    SnippetCheck("M255-E002-SMK-02", "compatibility_runtime_shim"),
    SnippetCheck("M255-E002-SMK-03", 'live_runtime_dispatch_default_symbol = "objc3_runtime_dispatch_i32"'),
)
REPLAY_SCRIPT_SNIPPETS = (
    SnippetCheck("M255-E002-RPY-01", "requires_live_runtime_dispatch"),
    SnippetCheck("M255-E002-RPY-02", "compatibility_runtime_shim"),
    SnippetCheck("M255-E002-RPY-03", "live_runtime_dispatch_default_symbol"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-E002-HDR-01", "kObjc3RuntimeLiveDispatchSmokeReplayCloseoutContractId"),
    SnippetCheck("M255-E002-HDR-02", f'"{CONTRACT_ID}"'),
)
IR_SNIPPETS = (
    SnippetCheck("M255-E002-IR-01", "M255-E002 live-dispatch smoke/replay closeout anchor"),
    SnippetCheck("M255-E002-IR-02", "canonical runtime dispatch evidence as authoritative"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-E002-PARSE-01", "M255-E002 live-dispatch smoke/replay closeout anchor"),
    SnippetCheck("M255-E002-PARSE-02", "proof consumes the canonical runtime dispatch symbol"),
)
SHIM_SNIPPETS = (
    SnippetCheck("M255-E002-SHIM-01", "M255-E002 smoke/replay closeout"),
    SnippetCheck("M255-E002-SHIM-02", "non-authoritative evidence for live smoke/replay proof"),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M255-E002-RUN-01", "check:objc3c:m255-e001-lane-e-readiness"),
    SnippetCheck("M255-E002-RUN-02", "check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py"),
    SnippetCheck("M255-E002-RUN-03", "test_check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-E002-PKG-01", '"check:objc3c:m255-e002-replace-shim-based-execution-smoke-with-live-dispatch-evidence": "python scripts/check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py"'),
    SnippetCheck("M255-E002-PKG-02", '"test:tooling:m255-e002-replace-shim-based-execution-smoke-with-live-dispatch-evidence": "python -m pytest tests/tooling/test_check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py -q"'),
    SnippetCheck("M255-E002-PKG-03", '"check:objc3c:m255-e002-lane-e-readiness": "python scripts/run_m255_e002_lane_e_readiness.py"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--positive-readme", type=Path, default=DEFAULT_POSITIVE_README)
    parser.add_argument("--negative-readme", type=Path, default=DEFAULT_NEGATIVE_README)
    parser.add_argument("--smoke-script", type=Path, default=DEFAULT_SMOKE_SCRIPT)
    parser.add_argument("--replay-script", type=Path, default=DEFAULT_REPLAY_SCRIPT)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--ir-cpp", type=Path, default=DEFAULT_IR_CPP)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--shim-c", type=Path, default=DEFAULT_SHIM_C)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--e001-summary", type=Path, default=DEFAULT_E001_SUMMARY)
    parser.add_argument("--smoke-root", type=Path, default=DEFAULT_SMOKE_ROOT)
    parser.add_argument("--replay-root", type=Path, default=DEFAULT_REPLAY_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M255-E002-MISSING", f"required artifact is missing: {display_path(path)}"))
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


def resolve_pwsh() -> str | None:
    return shutil.which("pwsh") or shutil.which("pwsh.exe") or shutil.which("powershell") or shutil.which("powershell.exe")


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


def latest_summary(root: Path) -> Path | None:
    if not root.exists():
        return None
    candidates = sorted(root.glob("*/summary.json"), key=lambda path: path.stat().st_mtime)
    return candidates[-1] if candidates else None


def validate_e001(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M255-E002-E001-01", "E001 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M255-E002-E001-02", "E001 summary must report full check coverage", failures)
    dynamic_probes_executed = payload.get("dynamic_probes_executed")
    if dynamic_probes_executed is None:
        dynamic_probes_executed = payload.get("checks_passed") == payload.get("checks_total")
    checks_total += 1
    checks_passed += require(dynamic_probes_executed is True, artifact, "M255-E002-E001-03", "E001 dynamic probes must execute", failures)
    checks_total += 1
    checks_passed += require(payload.get("next_closeout_issue") == "M255-E002", artifact, "M255-E002-E001-04", "E001 must hand off to M255-E002", failures)
    upstream = payload.get("upstream_evidence", {}) if isinstance(payload.get("upstream_evidence"), dict) else {}
    c004 = upstream.get("m255_c004", {}) if isinstance(upstream.get("m255_c004"), dict) else {}
    d004 = upstream.get("m255_d004", {}) if isinstance(upstream.get("m255_d004"), dict) else {}
    checks_total += 1
    checks_passed += require(c004.get("canonical_symbol") == CANONICAL_SYMBOL, artifact, "M255-E002-E001-05", "E001 must preserve canonical C004 symbol", failures)
    checks_total += 1
    checks_passed += require(c004.get("compatibility_call_count_zero") is True, artifact, "M255-E002-E001-06", "E001 must preserve zero compatibility call count", failures)
    checks_total += 1
    checks_passed += require(d004.get("fallback_protocol_probe_count") == 2, artifact, "M255-E002-E001-07", "E001 must preserve D004 negative protocol lookup evidence", failures)
    return checks_total, checks_passed, {
        "ok": True,
        "canonical_symbol": c004.get("canonical_symbol"),
        "compatibility_call_count_zero": c004.get("compatibility_call_count_zero"),
        "fallback_protocol_probe_count": d004.get("fallback_protocol_probe_count"),
    }


def validate_smoke_summary(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(payload.get("status") == "PASS", artifact, "M255-E002-SMK-01", "smoke summary must report PASS", failures)
    checks_total += 1
    checks_passed += require(payload.get("failed") == 0, artifact, "M255-E002-SMK-02", "smoke summary must report zero failures", failures)
    checks_total += 1
    checks_passed += require(payload.get("runtime_library") == "artifacts/lib/objc3_runtime.lib", artifact, "M255-E002-SMK-03", "smoke summary runtime library drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("compatibility_runtime_shim") == "tests/tooling/runtime/objc3_msgsend_i32_shim.c", artifact, "M255-E002-SMK-04", "smoke summary compatibility shim path drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("live_runtime_dispatch_default_symbol") == CANONICAL_SYMBOL, artifact, "M255-E002-SMK-05", "smoke summary canonical symbol drifted", failures)
    results = payload.get("results", []) if isinstance(payload.get("results"), list) else []
    positive_message_send = [case for case in results if isinstance(case, dict) and case.get("kind") == "positive" and str(case.get("fixture", "")).split("/")[-1].startswith("message_send")]
    negative_runtime_dispatch = [case for case in results if isinstance(case, dict) and case.get("kind") == "negative" and "runtime_dispatch_unresolved_symbol" in str(case.get("fixture", ""))]
    checks_total += 1
    checks_passed += require(len(positive_message_send) >= 10, artifact, "M255-E002-SMK-06", "expected message-send positive corpus in smoke summary", failures)
    checks_total += 1
    checks_passed += require(len(negative_runtime_dispatch) >= 4, artifact, "M255-E002-SMK-07", "expected runtime-dispatch negative corpus in smoke summary", failures)
    for index, case in enumerate(positive_message_send, start=1):
        checks_total += 1
        checks_passed += require(case.get("requires_live_runtime_dispatch") is True, artifact, f"M255-E002-SMK-POS-{index:02d}", f"positive message-send fixture must require live runtime dispatch: {case.get('fixture')}", failures)
        checks_total += 1
        checks_passed += require(case.get("runtime_dispatch_symbol") == CANONICAL_SYMBOL, artifact, f"M255-E002-SMK-POS-SYM-{index:02d}", f"positive message-send fixture must use canonical symbol: {case.get('fixture')}", failures)
    for index, case in enumerate(negative_runtime_dispatch, start=1):
        checks_total += 1
        checks_passed += require(case.get("requires_live_runtime_dispatch") is True, artifact, f"M255-E002-SMK-NEG-{index:02d}", f"runtime-dispatch negative must require live runtime dispatch: {case.get('fixture')}", failures)
        checks_total += 1
        checks_passed += require(case.get("runtime_dispatch_symbol") == CANONICAL_SYMBOL, artifact, f"M255-E002-SMK-NEG-SYM-{index:02d}", f"runtime-dispatch negative must use canonical symbol: {case.get('fixture')}", failures)
    return checks_total, checks_passed, {
        "ok": True,
        "runtime_library": payload.get("runtime_library"),
        "compatibility_runtime_shim": payload.get("compatibility_runtime_shim"),
        "live_runtime_dispatch_default_symbol": payload.get("live_runtime_dispatch_default_symbol"),
        "positive_message_send_cases": len(positive_message_send),
        "negative_runtime_dispatch_cases": len(negative_runtime_dispatch),
    }


def validate_replay_summary(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(payload.get("status") == "PASS", artifact, "M255-E002-RPY-01", "replay summary must report PASS", failures)
    checks_total += 1
    checks_passed += require(payload.get("run1_sha256") == payload.get("run2_sha256"), artifact, "M255-E002-RPY-02", "replay summary must keep canonical hashes identical", failures)
    run1_summary = ROOT / str(payload.get("run1_summary", ""))
    run2_summary = ROOT / str(payload.get("run2_summary", ""))
    checks_total += 1
    checks_passed += require(run1_summary.exists(), artifact, "M255-E002-RPY-03", "run1 summary path must exist", failures)
    checks_total += 1
    checks_passed += require(run2_summary.exists(), artifact, "M255-E002-RPY-04", "run2 summary path must exist", failures)
    for index, summary_path in enumerate((run1_summary, run2_summary), start=1):
        if summary_path.exists():
            summary = load_json(summary_path)
            checks_total += 1
            checks_passed += require(summary.get("runtime_library") == "artifacts/lib/objc3_runtime.lib", display_path(summary_path), f"M255-E002-RPY-SUM-{index}-01", "canonical replay smoke summary runtime library drifted", failures)
            checks_total += 1
            checks_passed += require(summary.get("live_runtime_dispatch_default_symbol") == CANONICAL_SYMBOL, display_path(summary_path), f"M255-E002-RPY-SUM-{index}-02", "canonical replay smoke summary symbol drifted", failures)
    return checks_total, checks_passed, {
        "ok": True,
        "run1_sha256": payload.get("run1_sha256"),
        "run2_sha256": payload.get("run2_sha256"),
    }


def dynamic_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    pwsh = resolve_pwsh()
    checks_total = 0
    checks_passed = 0
    if pwsh is None:
        failures.append(Finding(display_path(args.smoke_script), "M255-E002-DYN-00", "unable to resolve pwsh/powershell for dynamic probes"))
        return 1, 0, {}
    env = os.environ.copy()
    env["OBJC3C_NATIVE_EXECUTION_RUN_ID"] = SMOKE_RUN_ID
    before = {path.parent for path in args.replay_root.glob("*/summary.json")} if args.replay_root.exists() else set()
    smoke_result = run_command((pwsh, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(args.smoke_script)), env=env)
    smoke_summary_path = args.smoke_root / SMOKE_RUN_ID / "summary.json"
    checks_total += 1
    checks_passed += require(smoke_result.returncode == 0, display_path(args.smoke_script), "M255-E002-DYN-01", f"smoke script failed with exit {smoke_result.returncode}: {smoke_result.stderr.strip() or smoke_result.stdout.strip()}", failures)
    checks_total += 1
    checks_passed += require(smoke_summary_path.exists(), display_path(args.smoke_root), "M255-E002-DYN-02", "stable smoke summary path was not produced", failures)
    replay_result = run_command((pwsh, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(args.replay_script)))
    after = {path.parent for path in args.replay_root.glob("*/summary.json")} if args.replay_root.exists() else set()
    new_dirs = sorted(after - before, key=lambda path: path.stat().st_mtime)
    replay_summary_path = (new_dirs[-1] / "summary.json") if new_dirs else latest_summary(args.replay_root)
    checks_total += 1
    checks_passed += require(replay_result.returncode == 0, display_path(args.replay_script), "M255-E002-DYN-03", f"replay script failed with exit {replay_result.returncode}: {replay_result.stderr.strip() or replay_result.stdout.strip()}", failures)
    checks_total += 1
    checks_passed += require(replay_summary_path is not None and replay_summary_path.exists(), display_path(args.replay_root), "M255-E002-DYN-04", "replay summary was not produced", failures)
    return checks_total, checks_passed, {
        "smoke_summary_path": display_path(smoke_summary_path) if smoke_summary_path.exists() else None,
        "replay_summary_path": display_path(replay_summary_path) if replay_summary_path is not None and replay_summary_path.exists() else None,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_pairs = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.positive_readme, POSITIVE_README_SNIPPETS),
        (args.negative_readme, NEGATIVE_README_SNIPPETS),
        (args.smoke_script, SMOKE_SCRIPT_SNIPPETS),
        (args.replay_script, REPLAY_SCRIPT_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.ir_cpp, IR_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.shim_c, SHIM_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_pairs:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    e001_total, e001_passed, e001_distilled = validate_e001(args.e001_summary, failures)
    checks_total += e001_total
    checks_passed += e001_passed

    dynamic_probes_executed = False
    smoke_summary_path = args.smoke_root / SMOKE_RUN_ID / "summary.json"
    replay_summary_path: Path | None = latest_summary(args.replay_root)
    dynamic_distilled: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dynamic_probes_executed = True
        dyn_total, dyn_passed, dynamic_distilled = dynamic_probe(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed
        replay_summary_path = Path(ROOT / dynamic_distilled["replay_summary_path"]) if dynamic_distilled.get("replay_summary_path") else replay_summary_path

    if smoke_summary_path.exists():
        smoke_total, smoke_passed, smoke_distilled = validate_smoke_summary(smoke_summary_path, failures)
        checks_total += smoke_total
        checks_passed += smoke_passed
    else:
        smoke_distilled = {}

    if replay_summary_path is not None and replay_summary_path.exists():
        replay_total, replay_passed, replay_distilled = validate_replay_summary(replay_summary_path, failures)
        checks_total += replay_total
        checks_passed += replay_passed
    else:
        replay_distilled = {}

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "closeout_model": CLOSEOUT_MODEL,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "findings": [finding.__dict__ for finding in failures],
        "upstream_evidence": {
            "M255-E001": e001_distilled,
            "execution_smoke": smoke_distilled,
            "execution_replay": replay_distilled,
        },
        "dynamic_probe_outputs": dynamic_distilled,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        print(f"summary_path: {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"summary_path: {display_path(args.summary_out)}")
    print("status: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
