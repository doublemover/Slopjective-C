"""Execute a native trap witness without displaying a Windows crash dialog."""

from __future__ import annotations

import argparse
import ctypes
import json
from pathlib import Path
import subprocess
import sys


ILLEGAL_INSTRUCTION_EXIT = -1073741795
CASE_ARGUMENTS = {
    "dynamic-nil": ("nil",),
    "force-error": ("force", "error"),
    "mrc-dynamic-nil": ("mrc", "nil", "error"),
    "mrc-force-error": ("mrc", "force", "error", "trap"),
    "block-dynamic-nil": ("block", "nil", "error", "trap", "arc"),
    "block-force-error": ("block", "force", "error", "trap", "arc", "owned"),
    "mrc-block-dynamic-nil": ("block", "nil", "error", "trap", "mrc", "owned", "copy"),
    "mrc-block-force-error": ("block", "force", "error", "trap", "mrc", "owned", "copy", "dispose"),
}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path)
    parser.add_argument("case", choices=tuple(CASE_ARGUMENTS))
    args = parser.parse_args()
    if sys.platform != "win32":
        parser.error("this witness requires the admitted Windows runtime")
    executable = args.executable.resolve(strict=True)
    command = [str(executable), *CASE_ARGUMENTS[args.case]]
    kernel = ctypes.windll.kernel32
    kernel.SetErrorMode.argtypes = [ctypes.c_uint]
    kernel.SetErrorMode.restype = ctypes.c_uint
    previous = kernel.SetErrorMode(0x0001 | 0x0002)
    try:
        completed = subprocess.run(command, capture_output=True, text=True,
                                   timeout=30, creationflags=subprocess.CREATE_NO_WINDOW)
    finally:
        kernel.SetErrorMode(previous)
    exit_code = ctypes.c_int32(completed.returncode).value
    passed = exit_code == ILLEGAL_INSTRUCTION_EXIT
    print(json.dumps({"command": command, "exit_code": exit_code,
                      "expected_exit_code": ILLEGAL_INSTRUCTION_EXIT,
                      "status": "passed" if passed else "failed",
                      "stdout": completed.stdout, "stderr": completed.stderr}))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
