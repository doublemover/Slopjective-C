#!/usr/bin/env python3
"""Run the M210 performance-regression gate without deep recursive npm nesting.

This preserves the historical coverage envelope of the m210->m211->...->m225
chain while avoiding command-recursion brittleness in Windows shells.
"""

from __future__ import annotations

import os
import subprocess
import sys
from typing import Sequence


def run(command: Sequence[str]) -> None:
    printable = " ".join(command)
    print(f"[m210-chain] run: {printable}")
    completed = subprocess.run(command, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def run_npm_script(script_name: str) -> None:
    npm_execpath = os.environ.get("npm_execpath")
    npm_node_execpath = os.environ.get("npm_node_execpath")
    if npm_execpath and npm_node_execpath:
        run([npm_node_execpath, npm_execpath, "run", "-s", script_name])
        return
    run(["npm", "run", "-s", script_name])


def run_pytest(files: Sequence[str]) -> None:
    run([sys.executable, "-m", "pytest", *files, "-q"])


def main() -> None:
    # m223/m224 base prerequisites.
    run_npm_script("test:objc3c:m222-compatibility-migration")
    run([sys.executable, "scripts/build_objc3c_native_docs.py", "--check"])
    run_npm_script("check:objc3c:library-cli-parity:golden")

    milestone_groups = [
        (
            "m224",
            [
                "tests/tooling/test_objc3c_m224_frontend_release_contract.py",
                "tests/tooling/test_objc3c_m224_sema_release_contract.py",
                "tests/tooling/test_objc3c_m224_lowering_release_contract.py",
                "tests/tooling/test_objc3c_m224_integration_release_contract.py",
            ],
        ),
        (
            "m225",
            [
                "tests/tooling/test_objc3c_m225_frontend_roadmap_seed_contract.py",
                "tests/tooling/test_objc3c_m225_sema_roadmap_seed_contract.py",
                "tests/tooling/test_objc3c_m225_lowering_roadmap_seed_contract.py",
                "tests/tooling/test_objc3c_m225_validation_roadmap_seed_contract.py",
                "tests/tooling/test_objc3c_m225_integration_roadmap_seed_contract.py",
            ],
        ),
        (
            "m221",
            [
                "tests/tooling/test_objc3c_m221_frontend_ga_blocker_contract.py",
                "tests/tooling/test_objc3c_m221_sema_ga_blocker_contract.py",
                "tests/tooling/test_objc3c_m221_lowering_ga_blocker_contract.py",
                "tests/tooling/test_objc3c_m221_validation_ga_blocker_contract.py",
                "tests/tooling/test_objc3c_m221_integration_ga_blocker_contract.py",
            ],
        ),
        (
            "m220",
            [
                "tests/tooling/test_objc3c_m220_frontend_public_beta_contract.py",
                "tests/tooling/test_objc3c_m220_sema_public_beta_contract.py",
                "tests/tooling/test_objc3c_m220_lowering_public_beta_contract.py",
                "tests/tooling/test_objc3c_m220_validation_public_beta_contract.py",
                "tests/tooling/test_objc3c_m220_integration_public_beta_contract.py",
            ],
        ),
        (
            "m219",
            [
                "tests/tooling/test_objc3c_m219_frontend_cross_platform_contract.py",
                "tests/tooling/test_objc3c_m219_sema_cross_platform_contract.py",
                "tests/tooling/test_objc3c_m219_lowering_cross_platform_contract.py",
                "tests/tooling/test_objc3c_m219_validation_cross_platform_contract.py",
                "tests/tooling/test_objc3c_m219_integration_cross_platform_contract.py",
            ],
        ),
        (
            "m218",
            [
                "tests/tooling/test_objc3c_m218_frontend_rc_provenance_contract.py",
                "tests/tooling/test_objc3c_m218_sema_rc_provenance_contract.py",
                "tests/tooling/test_objc3c_m218_lowering_rc_provenance_contract.py",
                "tests/tooling/test_objc3c_m218_validation_rc_provenance_contract.py",
                "tests/tooling/test_objc3c_m218_integration_rc_provenance_contract.py",
            ],
        ),
        (
            "m217",
            [
                "tests/tooling/test_objc3c_m217_frontend_differential_contract.py",
                "tests/tooling/test_objc3c_m217_sema_differential_contract.py",
                "tests/tooling/test_objc3c_m217_lowering_differential_contract.py",
                "tests/tooling/test_objc3c_m217_validation_differential_contract.py",
                "tests/tooling/test_objc3c_m217_integration_differential_contract.py",
            ],
        ),
        (
            "m216",
            [
                "tests/tooling/test_objc3c_m216_frontend_conformance_contract.py",
                "tests/tooling/test_objc3c_m216_sema_conformance_contract.py",
                "tests/tooling/test_objc3c_m216_lowering_conformance_contract.py",
                "tests/tooling/test_objc3c_m216_validation_conformance_contract.py",
                "tests/tooling/test_objc3c_m216_integration_conformance_contract.py",
            ],
        ),
        (
            "m215",
            [
                "tests/tooling/test_objc3c_m215_frontend_sdk_packaging_contract.py",
                "tests/tooling/test_objc3c_m215_sema_sdk_packaging_contract.py",
                "tests/tooling/test_objc3c_m215_lowering_sdk_packaging_contract.py",
                "tests/tooling/test_objc3c_m215_validation_sdk_packaging_contract.py",
                "tests/tooling/test_objc3c_m215_integration_sdk_packaging_contract.py",
            ],
        ),
        (
            "m214",
            [
                "tests/tooling/test_objc3c_m214_frontend_daemonized_contract.py",
                "tests/tooling/test_objc3c_m214_sema_daemonized_contract.py",
                "tests/tooling/test_objc3c_m214_lowering_daemonized_contract.py",
                "tests/tooling/test_objc3c_m214_validation_daemonized_contract.py",
                "tests/tooling/test_objc3c_m214_integration_daemonized_contract.py",
            ],
        ),
        (
            "m213",
            [
                "tests/tooling/test_objc3c_m213_frontend_debug_fidelity_contract.py",
                "tests/tooling/test_objc3c_m213_sema_debug_fidelity_contract.py",
                "tests/tooling/test_objc3c_m213_lowering_debug_fidelity_contract.py",
                "tests/tooling/test_objc3c_m213_validation_debug_fidelity_contract.py",
                "tests/tooling/test_objc3c_m213_integration_debug_fidelity_contract.py",
            ],
        ),
        (
            "m212",
            [
                "tests/tooling/test_objc3c_m212_frontend_code_action_contract.py",
                "tests/tooling/test_objc3c_m212_sema_code_action_contract.py",
                "tests/tooling/test_objc3c_m212_lowering_code_action_contract.py",
                "tests/tooling/test_objc3c_m212_validation_code_action_contract.py",
                "tests/tooling/test_objc3c_m212_integration_code_action_contract.py",
            ],
        ),
        (
            "m211",
            [
                "tests/tooling/test_objc3c_m211_frontend_lsp_contract.py",
                "tests/tooling/test_objc3c_m211_sema_lsp_contract.py",
                "tests/tooling/test_objc3c_m211_lowering_lsp_contract.py",
                "tests/tooling/test_objc3c_m211_validation_lsp_contract.py",
                "tests/tooling/test_objc3c_m211_integration_lsp_contract.py",
            ],
        ),
        (
            "m210",
            [
                "tests/tooling/test_objc3c_m210_frontend_perf_regression_contract.py",
                "tests/tooling/test_objc3c_m210_sema_perf_regression_contract.py",
                "tests/tooling/test_objc3c_m210_lowering_perf_regression_contract.py",
                "tests/tooling/test_objc3c_m210_validation_perf_regression_contract.py",
                "tests/tooling/test_objc3c_m210_integration_perf_regression_contract.py",
            ],
        ),
    ]

    for milestone, files in milestone_groups:
        print(f"[m210-chain] pytest group: {milestone}")
        run_pytest(files)

    print("[m210-chain] status: PASS")


if __name__ == "__main__":
    main()
