"""Hosted LLVM and capability explorer developer-tooling actions."""

from __future__ import annotations

import subprocess
import sys

from objc3c_tooling.json_io import load_json_object as load_json

from ..commands import run
from ..environment import ROOT
from .developer_tooling_paths import (
    HOSTED_LLVM_CAPABILITIES_SUMMARY,
    LIBRARY_CLI_PARITY_PY,
    LLVM_CAPABILITIES_PROBE_PY,
    PUBLIC_WORKFLOW_REPORT_ROOT,
)


def action_check_llvm_capabilities(_: list[str]) -> int:
    return run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            "tmp/artifacts/objc3c-native/llvm_capabilities/summary.json",
        ]
    )


def _run_hosted_llvm_probe() -> tuple[int, dict[str, object]]:
    result = subprocess.run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            str(HOSTED_LLVM_CAPABILITIES_SUMMARY.relative_to(ROOT).as_posix()),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    summary = (
        load_json(HOSTED_LLVM_CAPABILITIES_SUMMARY)
        if HOSTED_LLVM_CAPABILITIES_SUMMARY.is_file()
        else {}
    )
    return result.returncode, summary


def action_check_hosted_llvm_capabilities(_: list[str]) -> int:
    probe_exit, summary = _run_hosted_llvm_probe()
    if probe_exit == 0:
        print("Hosted runner exposes clang and llc object-emission capability.")
        return 0

    clang = summary.get("clang") if isinstance(summary.get("clang"), dict) else {}
    llc = summary.get("llc") if isinstance(summary.get("llc"), dict) else {}
    llc_features = summary.get("llc_features")
    llc_features = llc_features if isinstance(llc_features, dict) else {}
    if not bool(clang.get("found")):
        print(
            "Hosted runner capability probe failed without clang availability.",
            file=sys.stderr,
        )
        return probe_exit or 1
    if bool(llc.get("found")) or bool(llc_features.get("supports_filetype_obj")):
        print(
            "Hosted runner capability probe reported an unexpected llc failure mode.",
            file=sys.stderr,
        )
        return probe_exit or 1
    print(
        "Hosted runner does not provide llc --filetype=obj capability; "
        "continuing via capability-routed parity validation."
    )
    return 0


def action_test_capability_routed_source_parity(_: list[str]) -> int:
    summary = (
        load_json(HOSTED_LLVM_CAPABILITIES_SUMMARY)
        if HOSTED_LLVM_CAPABILITIES_SUMMARY.is_file()
        else {}
    )
    llc = summary.get("llc") if isinstance(summary.get("llc"), dict) else {}
    llc_features = summary.get("llc_features")
    llc_features = llc_features if isinstance(llc_features, dict) else {}
    if not bool(llc.get("found")) or not bool(llc_features.get("supports_filetype_obj")):
        print(
            "Skipping live source parity: hosted runner does not provide "
            "llc --filetype=obj capability."
        )
        return 0
    return run(
        [
            sys.executable,
            str(LIBRARY_CLI_PARITY_PY),
            "--source",
            "tests/tooling/fixtures/native/hello.objc3",
            "--cli-bin",
            "artifacts/bin/objc3c-native.exe",
            "--c-api-bin",
            "artifacts/bin/objc3c-frontend-c-api-runner.exe",
            "--work-dir",
            "tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work",
            "--summary-out",
            "tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/summary.json",
            "--llvm-capabilities-summary",
            str(HOSTED_LLVM_CAPABILITIES_SUMMARY.relative_to(ROOT).as_posix()),
            "--route-cli-backend-from-capabilities",
        ]
    )


def action_inspect_capability_explorer(rest: list[str]) -> int:
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / "capability-explorer.json"
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    rc = run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            str(dump_path),
            *rest,
        ]
    )
    if rc == 0:
        print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
        print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return rc
