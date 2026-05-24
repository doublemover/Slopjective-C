"""Focused native execution and fixture test orchestration actions."""

from __future__ import annotations

import json
import os
from pathlib import Path
import sys

from ..commands import pwsh_file, run
from .hosted_llvm_summary import (
    hosted_llc_object_emission_available,
    hosted_native_object_emission_status,
)
from .test_orchestration_paths import (
    BEHAVIOR_MATRIX_PY,
    COMPILE_WRAPPER_SELF_AUDIT_PY,
    LLVM_CAPABILITY_ROUTING_TESTS,
    MATRIX_PS1,
    NEGATIVE_EXPECTATIONS_PS1,
    RECOVERY_PS1,
    REPLAY_PS1,
    SMOKE_PS1,
)

ROOT = Path(__file__).resolve().parents[3]
HOSTED_EXECUTION_SMOKE_SUMMARY = (
    ROOT / "tmp" / "reports" / "hosted-execution-smoke" / "summary.json"
)
NATIVE_EXECUTION_SMOKE_REPORT_SUMMARY = (
    ROOT / "tmp" / "reports" / "objc3c-native-execution-smoke" / "summary.json"
)
NATIVE_EXECUTION_SMOKE_ARTIFACT_ROOT = (
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke"
)


def repo_rel(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def write_json(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def hosted_execution_run_id() -> str:
    configured = os.environ.get("OBJC3C_NATIVE_EXECUTION_RUN_ID", "").strip()
    if configured:
        return configured
    run_id = "hosted-execution-smoke"
    os.environ["OBJC3C_NATIVE_EXECUTION_RUN_ID"] = run_id
    return run_id


def native_execution_artifact_summary(run_id: str) -> Path:
    return NATIVE_EXECUTION_SMOKE_ARTIFACT_ROOT / run_id / "summary.json"


def normalize_native_execution_summary(run_id: str, exit_code: int, status: str) -> dict:
    artifact_summary = native_execution_artifact_summary(run_id)
    if artifact_summary.is_file():
        payload = json.loads(artifact_summary.read_text(encoding="utf-8"))
        if not isinstance(payload, dict):
            payload = {}
    else:
        payload = {}

    results = payload.get("results", [])
    if not isinstance(results, list):
        results = []
    runtime_libraries = sorted(
        {
            str(result.get("runtime_library", ""))
            for result in results
            if isinstance(result, dict) and result.get("runtime_library")
        }
    )
    linker_flags: set[str] = set()
    for result in results:
        if not isinstance(result, dict):
            continue
        raw_flags = result.get("driver_linker_flags", [])
        if not isinstance(raw_flags, list):
            continue
        linker_flags.update(flag for flag in raw_flags if isinstance(flag, str) and flag)

    payload.setdefault("contract_id", "objc3c.native_execution_smoke.summary.v1")
    payload["status"] = status if exit_code != 0 else str(payload.get("status") or status)
    payload["hosted_runner_summary"] = True
    payload["native_object_emission"] = exit_code == 0 and artifact_summary.is_file()
    payload["skip_reason"] = "" if artifact_summary.is_file() else "native-execution-smoke-summary-missing"
    payload["support_claim_published"] = False
    payload["fallback_success_path"] = False
    payload["source_summary_path"] = repo_rel(artifact_summary)
    payload["results"] = results
    payload.setdefault("runtime_library", runtime_libraries[0] if runtime_libraries else "")
    payload.setdefault("link_command", "clang++ plus per-fixture object, runtime library, and driver_linker_flags")
    payload.setdefault("load_path", runtime_libraries)
    payload.setdefault("driver_linker_flags", sorted(linker_flags))
    payload["exit_code"] = exit_code
    return payload


def write_hosted_execution_skip_summary(status: str) -> None:
    native_summary = {
        "contract_id": "objc3c.native_execution_smoke.summary.v1",
        "status": "UNAVAILABLE",
        "hosted_runner_summary": True,
        "native_object_emission": False,
        "native_object_emission_status": status,
        "skip_reason": status,
        "support_claim_published": False,
        "fallback_success_path": False,
        "results": [],
        "runtime_library": "",
        "link_command": "",
        "load_path": [],
        "driver_linker_flags": [],
        "exit_code": 0,
    }
    write_json(NATIVE_EXECUTION_SMOKE_REPORT_SUMMARY, native_summary)
    write_json(
        HOSTED_EXECUTION_SMOKE_SUMMARY,
        {
            "contract_id": "objc3c.hosted_execution_smoke.summary.v1",
            "status": "UNAVAILABLE",
            "native_object_emission": False,
            "native_object_emission_status": status,
            "skip_reason": status,
            "native_execution_summary": repo_rel(NATIVE_EXECUTION_SMOKE_REPORT_SUMMARY),
            "support_claim_published": False,
            "fallback_success_path": False,
        },
    )


def write_hosted_execution_run_summary(run_id: str, exit_code: int) -> None:
    status = "PASS" if exit_code == 0 else "FAIL"
    native_summary = normalize_native_execution_summary(run_id, exit_code, status)
    write_json(NATIVE_EXECUTION_SMOKE_REPORT_SUMMARY, native_summary)
    write_json(
        HOSTED_EXECUTION_SMOKE_SUMMARY,
        {
            "contract_id": "objc3c.hosted_execution_smoke.summary.v1",
            "status": status,
            "native_object_emission": native_summary["native_object_emission"],
            "native_object_emission_status": "native_object_emission_supported"
            if native_summary["native_object_emission"]
            else "native_execution_summary_missing",
            "skip_reason": native_summary["skip_reason"],
            "native_execution_summary": repo_rel(NATIVE_EXECUTION_SMOKE_REPORT_SUMMARY),
            "source_summary_path": native_summary["source_summary_path"],
            "support_claim_published": False,
            "fallback_success_path": False,
            "exit_code": exit_code,
        },
    )


def hosted_execution_effective_exit_code(run_id: str, smoke_exit_code: int) -> int:
    if smoke_exit_code != 0:
        return smoke_exit_code
    if native_execution_artifact_summary(run_id).is_file():
        return 0
    return 1


def action_test_behavior_matrix(_: list[str]) -> int:
    return run([sys.executable, str(BEHAVIOR_MATRIX_PY)])


def action_test_recovery(rest: list[str]) -> int:
    return pwsh_file(RECOVERY_PS1, *rest)


def action_test_execution_smoke(rest: list[str]) -> int:
    return pwsh_file(SMOKE_PS1, *rest)


def action_test_hosted_execution_smoke(_: list[str]) -> int:
    if not hosted_llc_object_emission_available():
        status = hosted_native_object_emission_status()
        write_hosted_execution_skip_summary(status)
        print(
            "Skipping execution smoke: hosted runner does not provide "
            f"llc --filetype=obj capability; {status}."
        )
        return 0
    run_id = hosted_execution_run_id()
    smoke_exit_code = pwsh_file(SMOKE_PS1)
    exit_code = hosted_execution_effective_exit_code(run_id, smoke_exit_code)
    write_hosted_execution_run_summary(run_id, exit_code)
    return exit_code


def action_test_execution_replay(rest: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, *rest)


def action_test_execution_replay_focused(_: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, "-Limit", "1")


def action_test_compile_wrapper_self_audit(_: list[str]) -> int:
    return run([sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)])


def action_test_llvm_capability_routing(_: list[str]) -> int:
    return run([sys.executable, "-m", "pytest", *LLVM_CAPABILITY_ROUTING_TESTS, "-q"])


def action_test_fixture_matrix(rest: list[str]) -> int:
    return pwsh_file(MATRIX_PS1, *rest)


def action_test_negative_expectations(rest: list[str]) -> int:
    return pwsh_file(NEGATIVE_EXPECTATIONS_PS1, *rest)
