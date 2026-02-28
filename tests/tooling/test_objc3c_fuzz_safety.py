from __future__ import annotations

import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "run_objc3c_fuzz_safety.py"
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
    if mode not in {"safe_fail", "unsafe_zero"}:
        raise ValueError(f"unsupported fake compiler mode: {mode}")

    exit_code = "1" if mode == "safe_fail" else "0"
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
    print("error: synthetic diagnostic O3P901 digest=" + digest, file=sys.stderr)
    return {exit_code}

if __name__ == "__main__":
    raise SystemExit(main())
""".format(exit_code=exit_code)
    path.write_text(body, encoding="utf-8")


def test_corpus_strategy_is_deterministic_and_contains_parser_and_semantic_cases() -> None:
    first = run_objc3c_fuzz_safety.build_corpus(max_cases=None)
    second = run_objc3c_fuzz_safety.build_corpus(max_cases=None)

    assert first == second
    assert len(first) >= 24

    case_ids = [item.case_id for item in first]
    assert case_ids == sorted(case_ids)
    assert len(set(case_ids)) == len(case_ids)
    assert any(item.subsystem == "parser" for item in first)
    assert any(item.subsystem == "semantic" for item in first)
    assert all(item.source.strip() for item in first)


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
    assert payload["status"] == "PASS"
    assert payload["violation_count"] == 0
    assert payload["config"]["case_count"] == 8
    assert payload["corpus_strategy"]["strategy_id"] == "objc3c-malformed-corpus-v1"


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
