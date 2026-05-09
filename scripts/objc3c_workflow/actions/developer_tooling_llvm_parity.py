"""Capability-routed source parity workflow action."""

from __future__ import annotations

import sys

from objc3c_tooling.json_io import load_json_object as load_json

from ..commands import run
from ..environment import ROOT
from .developer_tooling_paths import HOSTED_LLVM_CAPABILITIES_SUMMARY, LIBRARY_CLI_PARITY_PY


def _hosted_summary() -> dict[str, object]:
    return (
        load_json(HOSTED_LLVM_CAPABILITIES_SUMMARY)
        if HOSTED_LLVM_CAPABILITIES_SUMMARY.is_file()
        else {}
    )


def _summary_section(summary: dict[str, object], key: str) -> dict[str, object]:
    value = summary.get(key)
    return value if isinstance(value, dict) else {}


def action_test_capability_routed_source_parity(_: list[str]) -> int:
    summary = _hosted_summary()
    llc = _summary_section(summary, "llc")
    llc_features = _summary_section(summary, "llc_features")
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
