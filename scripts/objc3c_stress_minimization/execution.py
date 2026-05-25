"""Compiler process execution for stress minimization candidates."""

from __future__ import annotations

import hashlib
import json
import os
import subprocess
from pathlib import Path
from typing import Any

from objc3c_tooling.llvm_discovery import find_llvm_tool_path

from .paths import ROOT


def compile_source(
    compiler: Path,
    source_text: str,
    work_dir: Path,
    timeout_sec: float,
) -> dict[str, Any]:
    work_dir.mkdir(parents=True, exist_ok=True)
    source_path = work_dir / "candidate.objc3"
    out_dir = work_dir / "out"
    source_path.write_text(source_text, encoding="utf-8")
    completed = subprocess.run(
        [
            str(compiler),
            str(source_path),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=timeout_sec,
    )
    stdout_text = completed.stdout.replace("\r\n", "\n")
    stderr_text = completed.stderr.replace("\r\n", "\n")
    combined_output = f"{stdout_text}\n{stderr_text}"
    diagnostic_lines = [
        line.strip()
        for line in combined_output.splitlines()
        if line.strip() and ("error" in line.lower() or "o3" in line.lower())
    ]
    if not diagnostic_lines:
        diagnostic_lines = [line.strip() for line in combined_output.splitlines() if line.strip()][:5]
    if not diagnostic_lines:
        diagnostic_lines = [f"returncode:{completed.returncode}"]
    signature_payload = {
        "returncode": completed.returncode,
        "diagnostic_lines": diagnostic_lines,
    }
    signature_text = json.dumps(signature_payload, sort_keys=True)
    return {
        "returncode": completed.returncode,
        "stdout": stdout_text,
        "stderr": stderr_text,
        "diagnostic_lines": diagnostic_lines,
        "signature_sha256": hashlib.sha256(signature_text.encode("utf-8")).hexdigest(),
        "signature_payload": signature_payload,
    }


def _diagnostic_lines(stdout_text: str, stderr_text: str, returncode: int) -> list[str]:
    combined_output = f"{stdout_text}\n{stderr_text}"
    diagnostic_lines = [
        line.strip()
        for line in combined_output.splitlines()
        if line.strip()
        and (
            "error" in line.lower()
            or "o3" in line.lower()
            or "runtime dispatch failed" in line.lower()
            or "link." in line.lower()
        )
    ]
    if not diagnostic_lines:
        diagnostic_lines = [line.strip() for line in combined_output.splitlines() if line.strip()][:5]
    if not diagnostic_lines:
        diagnostic_lines = [f"returncode:{returncode}"]
    return diagnostic_lines


def _signature_result(
    *,
    failure_stage: str,
    returncode: int,
    stdout_text: str,
    stderr_text: str,
) -> dict[str, Any]:
    diagnostic_lines = _diagnostic_lines(stdout_text, stderr_text, returncode)
    signature_payload = {
        "failure_stage": failure_stage,
        "returncode": returncode,
        "diagnostic_lines": diagnostic_lines,
    }
    signature_text = json.dumps(signature_payload, sort_keys=True)
    return {
        "returncode": returncode,
        "stdout": stdout_text,
        "stderr": stderr_text,
        "diagnostic_lines": diagnostic_lines,
        "failure_stage": failure_stage,
        "signature_sha256": hashlib.sha256(signature_text.encode("utf-8")).hexdigest(),
        "signature_payload": signature_payload,
    }


def _execution_expectation(source_path: Path) -> dict[str, Any]:
    meta_path = source_path.with_suffix(".meta.json")
    if not meta_path.is_file():
        raise RuntimeError(f"runtime/execution minimization case missing metadata: {meta_path}")
    payload = json.loads(meta_path.read_text(encoding="utf-8"))
    expected = payload.get("expect_failure") or payload.get("expected")
    if not isinstance(expected, dict):
        raise RuntimeError(f"runtime/execution minimization case missing failure expectation: {meta_path}")
    stage = expected.get("stage")
    if stage not in {"compile", "link", "run"}:
        raise RuntimeError(f"runtime/execution minimization case has invalid failure stage: {meta_path}")
    required_tokens = expected.get("required_diagnostic_tokens", expected.get("required_tokens", []))
    if not isinstance(required_tokens, list) or not all(
        isinstance(token, str) and token for token in required_tokens
    ):
        raise RuntimeError(f"runtime/execution minimization case missing required diagnostics: {meta_path}")
    return {"stage": stage, "required_tokens": required_tokens}


def _clangxx() -> str:
    configured = os.environ.get("OBJC3C_NATIVE_EXECUTION_CLANG_PATH")
    if configured:
        return configured
    discovered = find_llvm_tool_path("clang++")
    return str(discovered) if discovered else "clang++"


def _link_driver_args() -> list[str]:
    args = ["-std=c++20"]
    if os.name == "nt":
        args.extend(
            [
                "-fms-runtime-lib=dll",
                "-fuse-ld=lld",
                "-Xlinker",
                "/MANIFEST:EMBED",
                "-Xlinker",
                "/MANIFESTUAC:level='asInvoker' uiAccess='false'",
            ]
        )
    return args


def _runtime_launch_contract(compile_dir: Path) -> dict[str, Any]:
    manifest_path = compile_dir / "module.runtime-registration-manifest.json"
    if not manifest_path.is_file():
        raise RuntimeError(f"runtime/execution minimization missing runtime registration manifest: {manifest_path}")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    runtime_library_relative_path = manifest.get("runtime_support_library_archive_relative_path")
    driver_linker_flags = manifest.get("driver_linker_flags")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path:
        raise RuntimeError("runtime/execution minimization missing runtime library path")
    if not isinstance(driver_linker_flags, list) or not all(
        isinstance(flag, str) and flag for flag in driver_linker_flags
    ):
        raise RuntimeError("runtime/execution minimization missing driver linker flags")
    runtime_library = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library.is_file():
        raise RuntimeError(f"runtime/execution minimization runtime library missing: {runtime_library}")
    return {
        "runtime_library": runtime_library,
        "driver_linker_flags": driver_linker_flags,
    }


def _require_expected_tokens(result: dict[str, Any], expectation: dict[str, Any], source_path: Path) -> None:
    diagnostic_text = "\n".join(result["diagnostic_lines"])
    missing = [token for token in expectation["required_tokens"] if token not in diagnostic_text]
    if missing:
        raise RuntimeError(
            f"runtime/execution minimization case {source_path} missed expected diagnostics: {', '.join(missing)}"
        )


def run_runtime_or_execution_source(
    compiler: Path,
    source_path: Path,
    source_text: str,
    work_dir: Path,
    timeout_sec: float,
) -> dict[str, Any]:
    expectation = _execution_expectation(source_path)
    compile_result = compile_source(compiler, source_text, work_dir / "compile", timeout_sec)
    if compile_result["returncode"] != 0:
        compile_result = {
            **compile_result,
            "failure_stage": "compile",
            "signature_payload": {
                "failure_stage": "compile",
                **compile_result["signature_payload"],
            },
        }
        if expectation["stage"] == "compile":
            _require_expected_tokens(compile_result, expectation, source_path)
        return compile_result
    if expectation["stage"] == "compile":
        return compile_result

    obj_path = work_dir / "compile" / "out" / "module.obj"
    if not obj_path.is_file():
        raise RuntimeError(f"runtime/execution minimization missing object output: {obj_path}")
    exe_path = work_dir / "module.exe"
    link_args = [*_link_driver_args(), str(obj_path)]
    if expectation["stage"] == "run":
        contract = _runtime_launch_contract(work_dir / "compile" / "out")
        link_args.extend([str(contract["runtime_library"]), *contract["driver_linker_flags"]])
    link_args.extend(["-o", str(exe_path), "-fno-color-diagnostics"])
    link_completed = subprocess.run(
        [_clangxx(), *link_args],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=timeout_sec,
    )
    link_result = _signature_result(
        failure_stage="link",
        returncode=link_completed.returncode,
        stdout_text=link_completed.stdout.replace("\r\n", "\n"),
        stderr_text=link_completed.stderr.replace("\r\n", "\n"),
    )
    if link_completed.returncode != 0:
        if expectation["stage"] == "link":
            _require_expected_tokens(link_result, expectation, source_path)
        return link_result
    if expectation["stage"] == "link":
        return link_result

    run_completed = subprocess.run(
        [str(exe_path)],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=timeout_sec,
    )
    run_result = _signature_result(
        failure_stage="run",
        returncode=run_completed.returncode,
        stdout_text=run_completed.stdout.replace("\r\n", "\n"),
        stderr_text=run_completed.stderr.replace("\r\n", "\n"),
    )
    if run_completed.returncode != 0:
        _require_expected_tokens(run_result, expectation, source_path)
    return run_result


__all__ = ["compile_source", "run_runtime_or_execution_source"]
