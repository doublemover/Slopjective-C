from __future__ import annotations

import difflib
import sys


def print_diff_preview(actual: bytes, expected: bytes) -> None:
    actual_text = actual.decode("utf-8", errors="replace")
    expected_text = expected.decode("utf-8", errors="replace")
    diff = list(
        difflib.unified_diff(
            actual_text.splitlines(),
            expected_text.splitlines(),
            fromfile="docs/objc3c-native.md (actual)",
            tofile="docs/objc3c-native.md (expected)",
            lineterm="",
        )
    )
    if not diff:
        return
    print("- Diff preview:", file=sys.stderr)
    for line in diff[:120]:
        print(line, file=sys.stderr)
