#!/usr/bin/env python3
"""Fast checks for shared subprocess helper semantics."""

from __future__ import annotations

import sys
import tempfile
from pathlib import Path
from typing import Sequence

from objc3c_tooling.subprocesses import (
    MISSING_EXECUTABLE_EXIT_CODE,
    TIMEOUT_EXIT_CODE,
    bounded_text,
    failure_snippet,
    raise_if_failed,
    run_capture,
    run_timed,
)


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main(argv: Sequence[str] | None = None) -> int:
    _ = argv

    success = run_capture([sys.executable, "-c", "print('ok')"], echo=False)
    expect(success.returncode == 0, "success command should return zero")
    expect(success.stdout.strip() == "ok", "success command should capture stdout")

    streamed = run_capture([sys.executable, "-c", ""], capture_output=False, echo=False)
    expect(streamed.returncode == 0, "non-capturing command should return zero")

    failure = run_capture([sys.executable, "-c", "import sys; print('bad'); print('err', file=sys.stderr); sys.exit(7)"], echo=False)
    expect(failure.returncode == 7, "nonzero command should preserve return code")
    expect("bad" in failure.stdout and "err" in failure.stderr, "nonzero command should capture both streams")
    expect("stdout:" in failure_snippet(failure), "failure snippets should include stdout")

    missing = run_capture(["definitely-not-a-real-objc3c-helper-binary"], echo=False)
    expect(missing.returncode == MISSING_EXECUTABLE_EXIT_CODE, "missing executable should map to stable return code")
    expect("executable not found" in missing.stderr, "missing executable should explain launch failure")

    empty = run_capture([], echo=False)
    expect(empty.returncode != 0 and "empty command" in empty.stderr, "empty commands should fail without crashing")

    timed_out = run_capture([sys.executable, "-c", "import time; time.sleep(2)"], timeout=0.1, echo=False)
    expect(timed_out.returncode == TIMEOUT_EXIT_CODE, "timeout should map to stable return code")
    expect("timed out" in timed_out.stderr, "timeout should include bounded diagnostic")

    with tempfile.TemporaryDirectory() as raw_tmp:
        tmp_dir = Path(raw_tmp)
        cwd_result = run_capture([sys.executable, "-c", "from pathlib import Path; print(Path.cwd().name)"], cwd=tmp_dir, echo=False)
        expect(cwd_result.stdout.strip() == tmp_dir.name, "cwd override should be honored")
        env_result = run_capture([sys.executable, "-c", "import os; print(os.environ.get('OBJC3C_HELPER_TEST'))"], env_overlay={"OBJC3C_HELPER_TEST": "yes"}, echo=False)
        expect(env_result.stdout.strip() == "yes", "env overlay should be applied")

    timed = run_timed([sys.executable, "-c", "print('timed')"], echo=False, metadata={"case": "unit"})
    expect(timed.returncode == 0, "timed command should preserve return code")
    expect(timed.stdout.strip() == "timed", "timed command should capture stdout")
    expect(timed.duration_ms >= 0.0, "timed command should record non-negative duration")
    expect(timed.to_dict(include_output=False)["metadata"] == {"case": "unit"}, "timed metadata should be preserved")

    try:
        raise_if_failed(failure, context="helper failure smoke", limit=3)
    except RuntimeError as exc:
        message = str(exc)
        expect("helper failure smoke failed with exit code 7" in message, "raise_if_failed should include context")
        expect("truncated" in bounded_text("abcdef", 3), "bounded_text should mark truncation")
    else:
        raise RuntimeError("raise_if_failed should raise for nonzero results")

    print("objc3c-subprocess-helpers: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
