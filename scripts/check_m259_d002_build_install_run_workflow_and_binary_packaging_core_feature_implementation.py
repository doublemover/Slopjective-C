#!/usr/bin/env python3
"""Validate M259-D002 build/install/run workflow and binary packaging."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-d002-build-install-run-workflow-and-binary-packaging-v1"
CONTRACT_ID = "objc3c-runnable-build-install-run-package/m259-d002-v1"
PACKAGE_MODEL = "staged-runnable-toolchain-bundle-with-repo-relative-layout"
INSTALL_MODEL = "local-package-root-not-system-install"
NEXT_ISSUE = "M259-D003"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-D002" / "build_install_run_workflow_and_binary_packaging_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_build_install_run_workflow_and_binary_packaging_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_d002_build_install_run_workflow_and_binary_packaging_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
PACKAGE_SCRIPT = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
D001_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-D001" / "toolchain_and_runtime_operations_contract_summary.json"
PACKAGE_MANIFEST_RELATIVE = Path("artifacts/package/objc3c-runnable-toolchain-package.json")
CANONICAL_FIXTURE_RELATIVE = Path("tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3")
EXECUTION_SMOKE_RELATIVE = Path("scripts/check_objc3c_native_execution_smoke.ps1")
EXECUTION_REPLAY_RELATIVE = Path("scripts/check_objc3c_execution_replay_proof.ps1")
COMPILE_WRAPPER_RELATIVE = Path("scripts/objc3c_native_compile.ps1")
NATIVE_EXE_RELATIVE = Path("artifacts/bin/objc3c-native.exe")
RUNTIME_LIB_RELATIVE = Path("artifacts/lib/objc3_runtime.lib")


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
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=DOC_NATIVE)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--package-script", type=Path, default=PACKAGE_SCRIPT)
    parser.add_argument("--smoke-script", type=Path, default=SMOKE_SCRIPT)
    parser.add_argument("--replay-script", type=Path, default=REPLAY_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--d001-summary", type=Path, default=D001_SUMMARY)
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


def run_command(command: list[str], cwd: Path, log_path: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        command,
        cwd=str(cwd),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    log_path.parent.mkdir(parents=True, exist_ok=True)
    log_path.write_text(
        "[command]\n"
        + " ".join(command)
        + "\n\n[stdout]\n"
        + completed.stdout
        + "\n[stderr]\n"
        + completed.stderr,
        encoding="utf-8",
    )
    return completed


def parse_summary_path(stdout: str) -> str:
    for line in stdout.splitlines():
        if line.startswith("summary_path: "):
            return line.split(": ", 1)[1].strip()
    return ""


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks_total += 5
    checks_passed += ensure_snippets(
        args.expectations_doc,
        (
            SnippetCheck("M259-D002-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-D002-DOC-02", "Issue: `#7215`"),
            SnippetCheck("M259-D002-DOC-03", "`npm run package:objc3c-native:runnable-toolchain`"),
            SnippetCheck("M259-D002-DOC-04", "`artifacts/package/objc3c-runnable-toolchain-package.json`"),
            SnippetCheck("M259-D002-DOC-05", "The contract must explicitly hand off to `M259-D003`."),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M259-D002-PKT-01", "Packet: `M259-D002`"),
            SnippetCheck("M259-D002-PKT-02", "Issue: `#7215`"),
            SnippetCheck("M259-D002-PKT-03", "Dependencies: `M259-D001`"),
            SnippetCheck("M259-D002-PKT-04", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-D002-PKT-05", "`M259-D003`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M259-D002-SRC-01", "## M259 staged runnable toolchain package workflow (D002)"),
            SnippetCheck("M259-D002-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-D002-SRC-03", "`npm run package:objc3c-native:runnable-toolchain`"),
            SnippetCheck("M259-D002-SRC-04", "`artifacts/package/objc3c-runnable-toolchain-package.json`"),
            SnippetCheck("M259-D002-SRC-05", "staged local package root only"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M259-D002-NDOC-01", "## M259 staged runnable toolchain package workflow (D002)"),
            SnippetCheck("M259-D002-NDOC-02", "`npm run package:objc3c-native:runnable-toolchain`"),
            SnippetCheck("M259-D002-NDOC-03", "`scripts/check_objc3c_native_execution_smoke.ps1`"),
            SnippetCheck("M259-D002-NDOC-04", "no cross-platform packaging claim"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M259-D002-SPC-01", "## M259 staged runnable toolchain package workflow (D002)"),
            SnippetCheck("M259-D002-SPC-02", "`staged-runnable-toolchain-bundle-with-repo-relative-layout`"),
            SnippetCheck("M259-D002-SPC-03", "`local-package-root-not-system-install`"),
            SnippetCheck("M259-D002-SPC-04", "`M259-D003`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M259-D002-META-01", "## M259 staged toolchain package output anchors (D002)"),
            SnippetCheck("M259-D002-META-02", "`artifacts/package/objc3c-runnable-toolchain-package.json`"),
            SnippetCheck("M259-D002-META-03", "`tests/tooling/fixtures/native/execution`"),
            SnippetCheck("M259-D002-META-04", "`tests/tooling/runtime/objc3_msgsend_i32_shim.c`"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        args.package_script,
        (
            SnippetCheck("M259-D002-SCRIPT-01", 'contract_id = "objc3c-runnable-build-install-run-package/m259-d002-v1"'),
            SnippetCheck("M259-D002-SCRIPT-02", '"scripts/check_objc3c_execution_replay_proof.ps1"'),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        args.smoke_script,
        (
            SnippetCheck("M259-D002-SMOKE-01", "M259-D002 workflow-package anchor:"),
            SnippetCheck("M259-D002-SMOKE-02", "staged runnable toolchain bundle root"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        args.replay_script,
        (
            SnippetCheck("M259-D002-REPLAY-01", "M259-D002 workflow-package anchor:"),
            SnippetCheck("M259-D002-REPLAY-02", "staged runnable toolchain bundle root"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M259-D002-PKG-01", '"package:objc3c-native:runnable-toolchain": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/package_objc3c_runnable_toolchain.ps1"'),
            SnippetCheck("M259-D002-PKG-02", '"check:objc3c:m259-d002-build-install-run-workflow-and-binary-packaging"'),
            SnippetCheck("M259-D002-PKG-03", '"test:tooling:m259-d002-build-install-run-workflow-and-binary-packaging"'),
            SnippetCheck("M259-D002-PKG-04", '"check:objc3c:m259-d002-lane-d-readiness"'),
            SnippetCheck("M259-D002-PKG-05", '"check:objc3c:m259-d001-lane-d-readiness"'),
        ),
        failures,
    )

    d001_summary = load_json(args.d001_summary)
    checks_total += 3
    checks_passed += require(d001_summary.get("contract_id") == "objc3c-runnable-toolchain-runtime-operations-freeze/m259-d001-v1", display_path(args.d001_summary), "M259-D002-DEP-01", "M259-D001 contract drift", failures)
    checks_passed += require(d001_summary.get("ok") is True, display_path(args.d001_summary), "M259-D002-DEP-02", "M259-D001 summary must remain green", failures)
    checks_passed += require(d001_summary.get("next_issue") == "M259-D002", display_path(args.d001_summary), "M259-D002-DEP-03", "M259-D001 must hand off directly to M259-D002", failures)

    dynamic_details: dict[str, Any] = {"executed": False}
    if not args.skip_dynamic_probes:
        dynamic_details["executed"] = True
        run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
        package_root = ROOT / "tmp" / "pkg" / "m259d002" / run_id
        log_root = ROOT / "tmp" / "reports" / "m259" / "M259-D002" / "dynamic" / run_id

        package_completed = run_command(
            [
                "pwsh",
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(args.package_script),
                "-PackageRoot",
                str(package_root),
            ],
            ROOT,
            log_root / "package.log",
        )
        checks_total += 1
        checks_passed += require(package_completed.returncode == 0, display_path(args.package_script), "M259-D002-DYN-01", f"package script failed; see {display_path(log_root / 'package.log')}", failures)

        manifest_path = package_root / PACKAGE_MANIFEST_RELATIVE
        manifest_payload: dict[str, Any] = {}
        if manifest_path.exists():
            manifest_payload = load_json(manifest_path)
        checks_total += 10
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M259-D002-DYN-02", "package manifest missing", failures)
        if manifest_payload:
            checks_passed += require(manifest_payload.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M259-D002-DYN-03", "package manifest contract drift", failures)
            checks_passed += require(manifest_payload.get("package_model") == PACKAGE_MODEL, display_path(manifest_path), "M259-D002-DYN-04", "package model drift", failures)
            checks_passed += require(manifest_payload.get("install_model") == INSTALL_MODEL, display_path(manifest_path), "M259-D002-DYN-05", "install model drift", failures)
            checks_passed += require(manifest_payload.get("native_executable") == NATIVE_EXE_RELATIVE.as_posix(), display_path(manifest_path), "M259-D002-DYN-06", "native executable path drift", failures)
            checks_passed += require(manifest_payload.get("frontend_c_api_runner") == "artifacts/bin/objc3c-frontend-c-api-runner.exe", display_path(manifest_path), "M259-D002-DYN-07", "frontend c-api runner path drift", failures)
            checks_passed += require(manifest_payload.get("runtime_library") == RUNTIME_LIB_RELATIVE.as_posix(), display_path(manifest_path), "M259-D002-DYN-08", "runtime library path drift", failures)
            checks_passed += require(manifest_payload.get("compile_wrapper") == COMPILE_WRAPPER_RELATIVE.as_posix(), display_path(manifest_path), "M259-D002-DYN-09", "compile wrapper path drift", failures)
            checks_passed += require(manifest_payload.get("canonical_runnable_fixture") == CANONICAL_FIXTURE_RELATIVE.as_posix(), display_path(manifest_path), "M259-D002-DYN-10", "canonical fixture drift", failures)
            checks_passed += require(int(manifest_payload.get("copied_file_count", 0)) >= 21, display_path(manifest_path), "M259-D002-DYN-11A", "copied file inventory is unexpectedly small", failures)

        compile_script = package_root / COMPILE_WRAPPER_RELATIVE
        canonical_fixture = package_root / CANONICAL_FIXTURE_RELATIVE
        packaged_out_dir = package_root / "tmp" / "packaged-canonical-run"
        compile_completed = run_command(
            [
                "pwsh",
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(compile_script),
                str(canonical_fixture),
                "--out-dir",
                str(packaged_out_dir),
                "--emit-prefix",
                "module",
            ],
            ROOT,
            log_root / "packaged_compile.log",
        )
        checks_total += 4
        checks_passed += require(compile_completed.returncode == 0, display_path(compile_script), "M259-D002-DYN-12", f"packaged compile failed; see {display_path(log_root / 'packaged_compile.log')}", failures)
        backend_path = packaged_out_dir / "module.object-backend.txt"
        checks_passed += require(backend_path.exists(), display_path(backend_path), "M259-D002-DYN-13", "packaged compile backend marker missing", failures)
        if backend_path.exists():
            checks_passed += require(read_text(backend_path).strip() == "llvm-direct", display_path(backend_path), "M259-D002-DYN-14", "packaged compile backend drifted from llvm-direct", failures)
        checks_passed += require((packaged_out_dir / "module.runtime-registration-manifest.json").exists(), display_path(packaged_out_dir / "module.runtime-registration-manifest.json"), "M259-D002-DYN-15", "packaged compile registration manifest missing", failures)

        smoke_completed = run_command(
            [
                "pwsh",
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(package_root / EXECUTION_SMOKE_RELATIVE),
            ],
            ROOT,
            log_root / "packaged_smoke.log",
        )
        smoke_summary_relative = parse_summary_path(smoke_completed.stdout)
        smoke_summary_path = package_root / smoke_summary_relative if smoke_summary_relative else Path()
        checks_total += 4
        checks_passed += require(smoke_completed.returncode == 0, display_path(package_root / EXECUTION_SMOKE_RELATIVE), "M259-D002-DYN-16", f"packaged smoke failed; see {display_path(log_root / 'packaged_smoke.log')}", failures)
        checks_passed += require(bool(smoke_summary_relative), display_path(package_root / EXECUTION_SMOKE_RELATIVE), "M259-D002-DYN-17", "packaged smoke did not publish summary_path", failures)
        checks_passed += require(bool(smoke_summary_relative) and smoke_summary_path.exists(), display_path(smoke_summary_path) if smoke_summary_relative else display_path(package_root / EXECUTION_SMOKE_RELATIVE), "M259-D002-DYN-18", "packaged smoke summary missing", failures)
        if smoke_summary_relative and smoke_summary_path.exists():
            smoke_payload = load_json(smoke_summary_path)
            checks_passed += require(smoke_payload.get("status") == "PASS", display_path(smoke_summary_path), "M259-D002-DYN-19", "packaged smoke summary is not PASS", failures)
        else:
            smoke_payload = {}

        replay_completed = run_command(
            [
                "pwsh",
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(package_root / EXECUTION_REPLAY_RELATIVE),
            ],
            ROOT,
            log_root / "packaged_replay.log",
        )
        replay_summary_relative = parse_summary_path(replay_completed.stdout)
        replay_summary_path = package_root / replay_summary_relative if replay_summary_relative else Path()
        checks_total += 4
        checks_passed += require(replay_completed.returncode == 0, display_path(package_root / EXECUTION_REPLAY_RELATIVE), "M259-D002-DYN-20", f"packaged replay failed; see {display_path(log_root / 'packaged_replay.log')}", failures)
        checks_passed += require(bool(replay_summary_relative), display_path(package_root / EXECUTION_REPLAY_RELATIVE), "M259-D002-DYN-21", "packaged replay did not publish summary_path", failures)
        checks_passed += require(bool(replay_summary_relative) and replay_summary_path.exists(), display_path(replay_summary_path) if replay_summary_relative else display_path(package_root / EXECUTION_REPLAY_RELATIVE), "M259-D002-DYN-22", "packaged replay summary missing", failures)
        if replay_summary_relative and replay_summary_path.exists():
            replay_payload = load_json(replay_summary_path)
            checks_passed += require(replay_payload.get("status") == "PASS", display_path(replay_summary_path), "M259-D002-DYN-23", "packaged replay summary is not PASS", failures)
        else:
            replay_payload = {}

        dynamic_details.update(
            {
                "package_root": display_path(package_root),
                "package_manifest": display_path(manifest_path),
                "packaged_compile_out_dir": display_path(packaged_out_dir),
                "packaged_smoke_summary": display_path(smoke_summary_path) if smoke_summary_relative else "",
                "packaged_replay_summary": display_path(replay_summary_path) if replay_summary_relative else "",
                "logs": {
                    "package": display_path(log_root / "package.log"),
                    "compile": display_path(log_root / "packaged_compile.log"),
                    "smoke": display_path(log_root / "packaged_smoke.log"),
                    "replay": display_path(log_root / "packaged_replay.log"),
                },
                "manifest_contract_id": manifest_payload.get("contract_id", ""),
                "smoke_status": smoke_payload.get("status", ""),
                "replay_status": replay_payload.get("status", ""),
            }
        )

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "package_model": PACKAGE_MODEL,
        "install_model": INSTALL_MODEL,
        "next_issue": NEXT_ISSUE,
        "dynamic_probes_executed": dynamic_details.get("executed", False),
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "dependency": {
            "M259-D001": {
                "summary": display_path(args.d001_summary),
                "contract_id": d001_summary.get("contract_id"),
                "ok": d001_summary.get("ok"),
            }
        },
        "probe_details": dynamic_details,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
