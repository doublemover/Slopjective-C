from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path


def count_items(root: Any, *, path: Path, label: str) -> int:
    if isinstance(root, list):
        return len(root)
    if isinstance(root, dict):
        items = root.get("items")
        if not isinstance(items, list):
            raise ValueError(
                f"error: {label} snapshot {display_path(path)} must be a JSON "
                "array or object with list field 'items'"
            )
        declared_count = root.get("count")
        if declared_count is not None:
            if isinstance(declared_count, bool) or not isinstance(declared_count, int):
                raise ValueError(
                    f"error: {label} snapshot field 'count' must be an integer in "
                    f"{display_path(path)}"
                )
            if declared_count != len(items):
                raise ValueError(
                    f"error: {label} snapshot field 'count'={declared_count} "
                    f"does not match item count {len(items)} in {display_path(path)}"
                )
        return len(items)
    raise ValueError(
        f"error: {label} snapshot {display_path(path)} must be a JSON array or "
        "object with list field 'items'"
    )
