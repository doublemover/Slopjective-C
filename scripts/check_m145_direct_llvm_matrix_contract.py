#!/usr/bin/env python3
"""Fail-closed contract checks for direct LLVM object emission matrix (M145)."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m145-direct-llvm-matrix-contract-v1"

DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_CLI_OPTIONS_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--cli-options-cpp", type=Path, default=DEFAULT_CLI_OPTIONS_CPP)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/m145_direct_llvm_matrix_contract_summary.json"),
    )
    return parser.parse_args(argv)


def require_file(path: Path, *, label: str) -> None:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{label} must be a file: {display_path(path)}")


def extract_function_body(text: str, signature: str) -> str:
    start = text.find(signature)
    if start == -1:
        raise ValueError(f"missing function signature: {signature}")
    brace_start = text.find("{", start)
    if brace_start == -1:
        raise ValueError(f"missing function body for: {signature}")

    depth = 0
    for idx in range(brace_start, len(text)):
        ch = text[idx]
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return text[brace_start : idx + 1]
    raise ValueError(f"unterminated function body for: {signature}")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    require_file(args.process_cpp, label="process-cpp")
    require_file(args.driver_cpp, label="driver-cpp")
    require_file(args.cli_options_cpp, label="cli-options-cpp")

    process_cpp = args.process_cpp.read_text(encoding="utf-8")
    driver_cpp = args.driver_cpp.read_text(encoding="utf-8")
    cli_options_cpp = args.cli_options_cpp.read_text(encoding="utf-8")

    checks: list[tuple[str, bool, str]] = []

    # Process layer: llvm-direct path must be explicit and fail-closed.
    checks.append(
        (
            "process-m145-01",
            "RunProcess(llc_path.string(), {\"-filetype=obj\"" in process_cpp,
            "RunIRCompileLLVMDirect must invoke llc with -filetype=obj",
        )
    )
    checks.append(
        (
            "process-m145-02",
            "if (llc_status == 127)" in process_cpp,
            "RunIRCompileLLVMDirect must map missing llc to explicit status branch",
        )
    )
    checks.append(
        (
            "process-m145-03",
            "return 125;" in process_cpp,
            "RunIRCompileLLVMDirect must fail-closed with status 125 when backend unavailable",
        )
    )

    llvm_direct_body = extract_function_body(
        process_cpp,
        "int RunIRCompileLLVMDirect(",
    )
    checks.append(
        (
            "process-m145-04",
            "RunIRCompile(" not in llvm_direct_body,
            "RunIRCompileLLVMDirect must not fallback to clang RunIRCompile",
        )
    )

    # Driver layer: backend routing must be explicit and no hidden fallback.
    checks.append(
        (
            "driver-m145-01",
            "if (cli_options.ir_object_backend == Objc3IrObjectBackend::kClang)" in driver_cpp,
            "driver must branch explicitly on selected object backend",
        )
    )
    checks.append(
        (
            "driver-m145-02",
            "compile_status = RunIRCompile(cli_options.clang_path, ir_out, object_out);" in driver_cpp,
            "driver clang backend branch must call RunIRCompile",
        )
    )
    checks.append(
        (
            "driver-m145-03",
            "compile_status = RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error);"
            in driver_cpp,
            "driver llvm-direct backend branch must call RunIRCompileLLVMDirect",
        )
    )

    # CLI surface must continue exposing explicit backend selection and values.
    checks.append(
        (
            "cli-m145-01",
            "[--objc3-ir-object-backend <clang|llvm-direct>]" in cli_options_cpp,
            "usage text must advertise explicit clang|llvm-direct backend values",
        )
    )
    checks.append(
        (
            "cli-m145-02",
            "if (value == \"clang\")" in cli_options_cpp and "if (value == \"llvm-direct\")" in cli_options_cpp,
            "CLI parser must recognize both backend spellings",
        )
    )
    checks.append(
        (
            "cli-m145-03",
            re.search(r"invalid --objc3-ir-object-backend.*clang\|llvm-direct", cli_options_cpp)
            is not None,
            "CLI parser must fail with deterministic backend validation diagnostic",
        )
    )

    failed = [check for check in checks if not check[1]]
    summary = {
        "mode": MODE,
        "process_cpp": display_path(args.process_cpp),
        "driver_cpp": display_path(args.driver_cpp),
        "cli_options_cpp": display_path(args.cli_options_cpp),
        "checks_passed": len(checks) - len(failed),
        "checks_total": len(checks),
        "failures": [{"id": check_id, "message": message} for check_id, _, message in failed],
        "ok": not failed,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failed:
        for check_id, _, message in failed:
            print(f"M145-DIRECT-LLVM-FAIL [{check_id}] {message}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m145-direct-llvm-matrix-contract: OK")
    print(f"- checks_passed={summary['checks_passed']}")
    print(f"- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()
