#!/usr/bin/env python3
"""Unified public workflow runner for the live objc3c command surface."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Callable, Sequence

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
NPX = shutil.which("npx.cmd") or shutil.which("npx") or "npx"

BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_PS1 = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
RECOVERY_PS1 = ROOT / "scripts" / "check_objc3c_native_recovery_contract.ps1"
MATRIX_PS1 = ROOT / "scripts" / "run_objc3c_native_fixture_matrix.ps1"
NEGATIVE_EXPECTATIONS_PS1 = ROOT / "scripts" / "check_objc3c_negative_fixture_expectations.ps1"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PROOF_PS1 = ROOT / "scripts" / "run_objc3c_native_compile_proof.ps1"
SITE_PY = ROOT / "scripts" / "build_site_index.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"


@dataclass(frozen=True)
class ActionSpec:
    action: str
    summary: str
    backend: str
    public_scripts: tuple[str, ...]
    validation_tier: str = ""
    guarantee_owner: str = ""
    pass_through_args: bool = False


ActionHandler = Callable[[list[str]], int]


def run(command: Sequence[str]) -> int:
    return subprocess.run(list(command), cwd=ROOT, check=False).returncode


def pwsh_file(script: Path, *args: str) -> int:
    return run([PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(script), *args])


def run_steps(actions: Sequence[str]) -> int:
    for action in actions:
        rc = execute_registered_action(action, [])
        if rc != 0:
            return rc
    return 0


def action_build_default(_: list[str]) -> int:
    return run_steps(["build-native-binaries"])


def action_build_native_binaries(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only")


def action_build_native_contracts(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "packets-binary")


def action_build_native_full(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "full")


def action_build_native_reconfigure(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only", "-ForceReconfigure")


def action_build_spec(_: list[str]) -> int:
    rc = run([sys.executable, str(SITE_PY)])
    if rc != 0:
        return rc
    return run([NPX, "prettier", "--write", "site/index.md"])


def action_compile_objc3c(rest: list[str]) -> int:
    return pwsh_file(COMPILE_PS1, *rest)


def action_lint_spec(_: list[str]) -> int:
    return run([sys.executable, str(SPEC_LINT_PY)])


def action_test_default(_: list[str]) -> int:
    return run_steps(["test-fast"])


def action_test_fast(_: list[str]) -> int:
    rc = pwsh_file(SMOKE_PS1, "-Limit", "12")
    if rc != 0:
        return rc
    rc = run_steps(["test-runtime-acceptance"])
    if rc != 0:
        return rc
    return run_steps(["test-execution-replay"])


def action_test_smoke(_: list[str]) -> int:
    return run_steps(["test-execution-smoke"])


def action_test_ci(_: list[str]) -> int:
    rc = run([sys.executable, str(TASK_HYGIENE_PY)])
    if rc != 0:
        return rc
    return run_steps(["test-full"])


def action_test_recovery(rest: list[str]) -> int:
    return pwsh_file(RECOVERY_PS1, *rest)


def action_test_execution_smoke(rest: list[str]) -> int:
    return pwsh_file(SMOKE_PS1, *rest)


def action_test_execution_replay(rest: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, *rest)


def action_test_runtime_acceptance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY)])


def action_test_fixture_matrix(rest: list[str]) -> int:
    return pwsh_file(MATRIX_PS1, *rest)


def action_test_negative_expectations(rest: list[str]) -> int:
    return pwsh_file(NEGATIVE_EXPECTATIONS_PS1, *rest)


def action_test_full(_: list[str]) -> int:
    rc = run_steps(["test-smoke", "test-runtime-acceptance"])
    if rc != 0:
        return rc
    return run_steps(["test-execution-replay"])


def action_test_nightly(_: list[str]) -> int:
    rc = run_steps(["test-full", "test-recovery", "test-fixture-matrix", "test-negative-expectations"])
    if rc != 0:
        return rc
    return 0


def action_package_runnable_toolchain(_: list[str]) -> int:
    return pwsh_file(PACKAGE_PS1)


def action_proof_objc3c(_: list[str]) -> int:
    return pwsh_file(PROOF_PS1)


ACTION_SPECS: dict[str, ActionSpec] = {
    "build-default": ActionSpec("build-default", "default public build entrypoint", "runner-internal", ("build",)),
    "build-native-binaries": ActionSpec("build-native-binaries", "build native binaries", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native",)),
    "build-native-contracts": ActionSpec("build-native-contracts", "build native contracts/binaries packet surface", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:contracts",)),
    "build-native-full": ActionSpec("build-native-full", "run full native build", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:full",)),
    "build-native-reconfigure": ActionSpec("build-native-reconfigure", "force native reconfigure build", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:reconfigure",)),
    "build-spec": ActionSpec("build-spec", "build site/spec output and format it", "python:scripts/build_site_index.py + npx prettier", ("build:spec",)),
    "compile-objc3c": ActionSpec("compile-objc3c", "compile one Objective-C 3 fixture through the native compiler", "pwsh:scripts/objc3c_native_compile.ps1", ("compile:objc3c",), pass_through_args=True),
    "lint-spec": ActionSpec("lint-spec", "run spec lint", "python:scripts/spec_lint.py", ("lint:spec",)),
    "test-default": ActionSpec("test-default", "default public test entrypoint", "runner-internal", ("test",)),
    "test-fast": ActionSpec("test-fast", "fast public validation entrypoint", "runner-internal + targeted smoke slice", ("test:fast",), validation_tier="fast", guarantee_owner="runtime acceptance, canonical replay, and a bounded smoke slice"),
    "test-smoke": ActionSpec("test-smoke", "developer smoke validation entrypoint", "runner-internal", ("test:smoke",), validation_tier="smoke", guarantee_owner="full execution smoke corpus"),
    "test-ci": ActionSpec("test-ci", "CI-oriented public validation entrypoint", "runner-internal + direct task hygiene", ("test:ci",), validation_tier="ci", guarantee_owner="task hygiene plus full developer validation"),
    "test-recovery": ActionSpec("test-recovery", "native recovery contract suite", "pwsh:scripts/check_objc3c_native_recovery_contract.ps1", ("test:objc3c",), validation_tier="recovery", guarantee_owner="recovery compile success and deterministic recovery diagnostics", pass_through_args=True),
    "test-execution-smoke": ActionSpec("test-execution-smoke", "native execution smoke suite", "pwsh:scripts/check_objc3c_native_execution_smoke.ps1", ("test:objc3c:execution-smoke",), validation_tier="smoke", guarantee_owner="compile/link/run execution behavior", pass_through_args=True),
    "test-execution-replay": ActionSpec("test-execution-replay", "native execution replay proof suite", "pwsh:scripts/check_objc3c_execution_replay_proof.ps1", ("test:objc3c:execution-replay-proof",), validation_tier="full", guarantee_owner="replay and native-output truth", pass_through_args=True),
    "test-runtime-acceptance": ActionSpec("test-runtime-acceptance", "runtime acceptance suite", "python:scripts/check_objc3c_runtime_acceptance.py", ("test:objc3c:runtime-acceptance",), validation_tier="fast", guarantee_owner="runtime acceptance and ABI/accessor proof"),
    "test-fixture-matrix": ActionSpec("test-fixture-matrix", "broad positive recovery fixture matrix sweep", "pwsh:scripts/run_objc3c_native_fixture_matrix.ps1", ("test:objc3c:fixture-matrix",), validation_tier="nightly", guarantee_owner="broad positive corpus artifact sanity", pass_through_args=True),
    "test-negative-expectations": ActionSpec("test-negative-expectations", "static negative fixture expectation enforcement", "pwsh:scripts/check_objc3c_negative_fixture_expectations.ps1", ("test:objc3c:negative-expectations",), validation_tier="nightly", guarantee_owner="negative expectation header and token enforcement", pass_through_args=True),
    "test-full": ActionSpec("test-full", "full developer validation entrypoint", "runner-internal + direct PowerShell suites", ("test:objc3c:full",), validation_tier="full", guarantee_owner="smoke, runtime acceptance, and replay without full recovery fan-out"),
    "test-nightly": ActionSpec("test-nightly", "exhaustive validation entrypoint", "runner-internal + direct PowerShell suites", ("test:objc3c:nightly",), validation_tier="nightly", guarantee_owner="full validation plus recovery and broad corpus sweeps"),
    "package-runnable-toolchain": ActionSpec("package-runnable-toolchain", "package the runnable native toolchain", "pwsh:scripts/package_objc3c_runnable_toolchain.ps1", ("package:objc3c-native:runnable-toolchain",)),
    "proof-objc3c": ActionSpec("proof-objc3c", "run the native compile proof workflow", "pwsh:scripts/run_objc3c_native_compile_proof.ps1", ("proof:objc3c",)),
}


ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-default": action_build_default,
    "build-native-binaries": action_build_native_binaries,
    "build-native-contracts": action_build_native_contracts,
    "build-native-full": action_build_native_full,
    "build-native-reconfigure": action_build_native_reconfigure,
    "build-spec": action_build_spec,
    "compile-objc3c": action_compile_objc3c,
    "lint-spec": action_lint_spec,
    "test-default": action_test_default,
    "test-fast": action_test_fast,
    "test-smoke": action_test_smoke,
    "test-ci": action_test_ci,
    "test-recovery": action_test_recovery,
    "test-execution-smoke": action_test_execution_smoke,
    "test-execution-replay": action_test_execution_replay,
    "test-runtime-acceptance": action_test_runtime_acceptance,
    "test-fixture-matrix": action_test_fixture_matrix,
    "test-negative-expectations": action_test_negative_expectations,
    "test-full": action_test_full,
    "test-nightly": action_test_nightly,
    "package-runnable-toolchain": action_package_runnable_toolchain,
    "proof-objc3c": action_proof_objc3c,
}


def emit_json(payload: object) -> int:
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


def list_actions_payload() -> dict[str, object]:
    return {
        "mode": "m314-c002-parameterized-task-runner-v1",
        "runner_path": "scripts/objc3c_public_workflow_runner.py",
        "action_count": len(ACTION_SPECS),
        "actions": [asdict(spec) for spec in ACTION_SPECS.values()],
    }


def describe_action_payload(action: str) -> dict[str, object]:
    spec = ACTION_SPECS[action]
    payload = asdict(spec)
    payload["mode"] = "m314-c002-parameterized-task-runner-v1"
    payload["runner_path"] = "scripts/objc3c_public_workflow_runner.py"
    return payload


def execute_registered_action(action: str, rest: list[str]) -> int:
    spec = ACTION_SPECS.get(action)
    handler = ACTION_HANDLERS.get(action)
    if spec is None or handler is None:
        print(f"unknown action: {action}", file=sys.stderr)
        return 2
    if rest and not spec.pass_through_args:
        print(f"action does not accept extra arguments: {action}", file=sys.stderr)
        return 2
    return handler(rest)


def main(argv: Sequence[str]) -> int:
    if not argv:
        print(
            "usage: objc3c_public_workflow_runner.py <action> [args...]\n"
            "       objc3c_public_workflow_runner.py --list-json\n"
            "       objc3c_public_workflow_runner.py --describe <action>",
            file=sys.stderr,
        )
        return 2

    action, *rest = argv
    if action == "--list-json":
        return emit_json(list_actions_payload())
    if action == "--describe":
        if len(rest) != 1:
            print("usage: objc3c_public_workflow_runner.py --describe <action>", file=sys.stderr)
            return 2
        describe_action = rest[0]
        if describe_action not in ACTION_SPECS:
            print(f"unknown action: {describe_action}", file=sys.stderr)
            return 2
        return emit_json(describe_action_payload(describe_action))
    return execute_registered_action(action, rest)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
