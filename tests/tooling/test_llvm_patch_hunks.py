from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
HUNK = re.compile(
    r"^@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@"
)


def test_locked_llvm_patch_hunk_counts_are_exact() -> None:
    """Reject stale hunk metadata before a maintained patch reaches a build."""

    for patch in sorted((ROOT / "toolchains/llvm/patches").glob("*.patch")):
        lines = patch.read_text(encoding="utf-8").splitlines()
        index = 0
        while index < len(lines):
            match = HUNK.match(lines[index])
            if match is None:
                index += 1
                continue

            declared_old = int(match.group(2) or 1)
            declared_new = int(match.group(4) or 1)
            actual_old = 0
            actual_new = 0
            cursor = index + 1
            while cursor < len(lines):
                line = lines[cursor]
                if line.startswith("@@ ") or line.startswith("diff --git "):
                    break
                if line.startswith(" "):
                    actual_old += 1
                    actual_new += 1
                elif line.startswith("-"):
                    actual_old += 1
                elif line.startswith("+"):
                    actual_new += 1
                elif line == r"\ No newline at end of file":
                    pass
                else:
                    break
                cursor += 1

            assert (actual_old, actual_new) == (declared_old, declared_new), (
                f"{patch.relative_to(ROOT)}:{index + 1} declares "
                f"-{declared_old}/+{declared_new} lines but contains "
                f"-{actual_old}/+{actual_new}"
            )
            index = cursor
