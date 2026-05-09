"""Capability-routed source parity workflow action."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT
from .developer_tooling_paths import HOSTED_LLVM_CAPABILITIES_SUMMARY, LIBRARY_CLI_PARITY_PY
from .hosted_llvm_summary import hosted_llc_object_emission_available


def action_test_capability_routed_source_parity(_: list[str]) -> int:
    if not hosted_llc_object_emission_available():
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
