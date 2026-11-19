from __future__ import annotations

import importlib.util
import io
import json
import signal
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

import pytest

SCRIPTS_ROOT = Path(__file__).resolve().parents[2] / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

SCRIPT_PATH = SCRIPTS_ROOT / "run_objc3c_fuzz_safety.py"
SPEC = importlib.util.spec_from_file_location("run_objc3c_fuzz_safety", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/run_objc3c_fuzz_safety.py")
run_objc3c_fuzz_safety = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = run_objc3c_fuzz_safety
SPEC.loader.exec_module(run_objc3c_fuzz_safety)


def run_main(argv: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = run_objc3c_fuzz_safety.main(argv)
    return code, stdout.getvalue(), stderr.getvalue()


def write_fake_compiler(path: Path, *, mode: str) -> None:
    modes = {
        "safe_fail": (1, "error: synthetic diagnostic O3P901 digest=", 0),
        "unsafe_zero": (0, "error: synthetic diagnostic O3P901 digest=", 0),
        "silent_one": (1, "", 0),
        "silent_two": (2, "", 0),
        "exit_86": (86, "error: synthetic diagnostic O3P901 digest=", 0),
        "sanitizer_one": (1, "error: synthetic diagnostic O3P901\n==42==ERROR: AddressSanitizer: heap-buffer-overflow", 0),
        "cli_one": (1, "error: unknown argument '--pretend-source-error'", 0),
        "signal_abrt": (-signal.SIGABRT, "", 0),
        "signal_segv": (-signal.SIGSEGV, "", 0),
        "timeout": (1, "error: too late", 1),
    }
    if mode not in modes:
        raise ValueError(f"unsupported fake compiler mode: {mode}")

    exit_code, diagnostic, delay = modes[mode]
    body = """#!/usr/bin/env python3
import argparse
import hashlib
from pathlib import Path
import sys

def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("source")
    parser.add_argument("--out-dir", required=True)
    parser.add_argument("--emit-prefix", required=True)
    args = parser.parse_args()

    source_path = Path(args.source)
    text = source_path.read_text(encoding="utf-8")
    digest = hashlib.sha256(text.encode("utf-8")).hexdigest()[:10]
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "module.ll").write_text("; fake ir " + digest + "\\n", encoding="utf-8")
    diagnostic = {diagnostic!r}
    delay = {delay}
    if delay:
        import time
        time.sleep(delay)
    if diagnostic:
        print(diagnostic + (digest if diagnostic.endswith("digest=") else ""), file=sys.stderr)
    exit_code = {exit_code}
    if exit_code < 0:
        import os, signal
        os.kill(os.getpid(), -exit_code)
    return exit_code

if __name__ == "__main__":
    raise SystemExit(main())
""".format(exit_code=exit_code, diagnostic=diagnostic, delay=delay)
    path.write_text(body, encoding="utf-8")


def test_corpus_strategy_is_deterministic_and_contains_parser_and_semantic_cases() -> None:
    first = run_objc3c_fuzz_safety.build_corpus(max_cases=None)
    second = run_objc3c_fuzz_safety.build_corpus(max_cases=None)

    assert first == second
    assert len(first) >= 31

    case_ids = [item.case_id for item in first]
    assert case_ids == sorted(case_ids)
    assert len(set(case_ids)) == len(case_ids)
    assert any(item.subsystem == "parser" for item in first)
    assert any(item.subsystem == "semantic" for item in first)
    assert all(item.source.strip() for item in first)
    assert "parser_reject_trailing_comma_param" in case_ids
    assert "sema_break_outside_control_flow" in case_ids


def test_manifest_cases_are_loaded_from_checked_in_sources() -> None:
    manifest_cases = run_objc3c_fuzz_safety.load_manifest_cases(
        run_objc3c_fuzz_safety.DEFAULT_MANIFEST
    )
    case_ids = [case.case_id for case in manifest_cases]

    assert case_ids == [
        "parser_reject_trailing_comma_param",
        "parser_reject_post_comma_nondecl",
        "parser_reject_i32_pointer_param",
        "sema_break_outside_control_flow",
        "sema_continue_outside_loop",
        "sema_guard_binding_missing_exit",
        "parser_message_receiver_qualifier_extern",
    ]
    assert all(case.source.strip() for case in manifest_cases)


def test_contract_mode_happy_path_is_deterministic(tmp_path: Path) -> None:
    fake_compiler = tmp_path / "fake_safe_compiler.py"
    write_fake_compiler(fake_compiler, mode="safe_fail")

    args = [
        "--compiler",
        str(fake_compiler),
        "--python-launcher",
        sys.executable,
        "--out-root",
        str(tmp_path / "fuzz_out"),
        "--max-cases",
        "8",
        "--generated-at-utc",
        "2026-02-27T00:00:00Z",
        "--contract-mode",
    ]

    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout

    payload = json.loads(first_stdout)
    assert payload["mode"] == run_objc3c_fuzz_safety.MODE
    assert payload["schema_version"] == run_objc3c_fuzz_safety.SCHEMA_VERSION
    assert payload["schema_version"] == "objc3c-fuzz-safety-report.v2"
    assert payload["status"] == "PASS"
    assert payload["violation_count"] == 0
    assert payload["config"]["case_count"] == 8
    assert payload["corpus_strategy"]["strategy_id"] == "objc3c-malformed-corpus-with-manifest-v2"
    assert payload["corpus_strategy"]["manifest_path"] == "tests/tooling/fixtures/stress/parser_sema_fuzz_manifest.json"
    assert payload["corpus_strategy"]["manifest_case_count"] == 7
    assert all(result["run1_termination"] == "diagnostic-exit" for result in payload["results"])
    assert all(result["run1_diagnostic_class"] == "ordinary-source-diagnostic"
               for result in payload["results"])


def test_contract_mode_fails_on_zero_exit_for_malformed_inputs(tmp_path: Path) -> None:
    fake_compiler = tmp_path / "fake_unsafe_compiler.py"
    write_fake_compiler(fake_compiler, mode="unsafe_zero")

    code, stdout, stderr = run_main(
        [
            "--compiler",
            str(fake_compiler),
            "--python-launcher",
            sys.executable,
            "--out-root",
            str(tmp_path / "fuzz_out"),
            "--max-cases",
            "5",
            "--generated-at-utc",
            "2026-02-27T00:00:00Z",
            "--contract-mode",
        ]
    )

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["status"] == "FAIL"
    assert payload["violation_count"] > 0
    violation_ids = {item["check_id"] for item in payload["violations"]}
    assert "SAFE-001" in violation_ids


@pytest.mark.parametrize(
    ("mode", "required_check"),
    [
        ("silent_one", "SAFE-002"),
        ("silent_two", "SAFE-001"),
        ("exit_86", "SAFE-001"),
        ("sanitizer_one", "SAFE-005"),
        ("cli_one", "SAFE-005"),
        ("signal_abrt", "SAFE-004"),
        ("signal_segv", "SAFE-004"),
        ("timeout", "SAFE-003"),
    ],
)
def test_contract_mode_rejects_abnormal_statuses_and_diagnostics(
    tmp_path: Path, mode: str, required_check: str
) -> None:
    fake_compiler = tmp_path / (mode + ".py")
    write_fake_compiler(fake_compiler, mode=mode)

    code, stdout, stderr = run_main(
        [
            "--compiler", str(fake_compiler),
            "--python-launcher", sys.executable,
            "--out-root", str(tmp_path / (mode + "-out")),
            "--max-cases", "1",
            "--timeout-sec", "0.05" if mode == "timeout" else "5",
            "--generated-at-utc", "2026-02-27T00:00:00Z",
            "--contract-mode",
        ]
    )

    assert code == 1 and stderr == ""
    payload = json.loads(stdout)
    assert payload["status"] == "FAIL"
    assert required_check in {row["check_id"] for row in payload["violations"]}
    result = payload["results"][0]
    assert result["run1_exit_code"] == result["run2_exit_code"]
    assert result["run1_termination"] == result["run2_termination"]
    assert result["run1_diagnostic_class"] == result["run2_diagnostic_class"]


def test_missing_compiler_returns_input_error(tmp_path: Path) -> None:
    code, stdout, stderr = run_main(
        [
            "--compiler",
            str(tmp_path / "missing_compiler.exe"),
            "--contract-mode",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "compiler not found" in stderr
