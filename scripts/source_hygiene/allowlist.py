from __future__ import annotations

import fnmatch
import json
from pathlib import Path
from typing import Any


def load_allowlist(path: Path | None) -> dict[str, Any]:
    if path is None or not path.is_file():
        return {"allowed": [], "allow_tracked_generated_reports": False}
    payload = json.loads(path.read_text(encoding="utf-8"))
    allowed = payload.get("allowed", [])
    if not isinstance(allowed, list):
        raise ValueError(f"allowlist {path} must contain an 'allowed' list")
    return {
        "allowed": allowed,
        "allow_tracked_generated_reports": bool(
            payload.get("allow_tracked_generated_reports", False)
        ),
    }


def allowlist_matches(finding: dict[str, Any], allowlist_entry: Any) -> bool:
    if isinstance(allowlist_entry, str):
        return fnmatch.fnmatch(finding["path"], allowlist_entry)
    if not isinstance(allowlist_entry, dict):
        return False
    pattern_id = allowlist_entry.get("pattern_id", "*")
    if pattern_id != "*" and pattern_id != finding["pattern_id"]:
        return False
    path_glob = allowlist_entry.get("path_glob") or allowlist_entry.get("path")
    if path_glob and not fnmatch.fnmatch(finding["path"], str(path_glob)):
        return False
    line = allowlist_entry.get("line")
    return line is None or int(line) == int(finding["line"])


def is_allowed(finding: dict[str, Any], allowlist: dict[str, Any]) -> bool:
    return any(allowlist_matches(finding, entry) for entry in allowlist.get("allowed", []))
