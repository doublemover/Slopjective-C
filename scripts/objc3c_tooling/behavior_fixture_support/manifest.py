from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.paths import repo_rel


def load_manifest_fixture_entries(manifest_path: Path) -> list[dict[str, Any]]:
    manifest = load_json_object(manifest_path)
    raw_fixtures = manifest.get("fixtures", [])
    if not isinstance(raw_fixtures, list):
        raise RuntimeError(f"fixtures must be a list in {repo_rel(manifest_path)}")
    entries: list[dict[str, Any]] = []
    seen_paths: set[str] = set()
    for entry in raw_fixtures:
        if not isinstance(entry, dict):
            raise RuntimeError(
                f"manifest fixture entries must be objects in {repo_rel(manifest_path)}"
            )
        path = entry.get("path")
        if not isinstance(path, str) or not path:
            raise RuntimeError(
                f"manifest fixture entry missing path in {repo_rel(manifest_path)}"
            )
        if path in seen_paths:
            raise RuntimeError(
                f"manifest fixture entry duplicates path {path!r} in "
                f"{repo_rel(manifest_path)}"
            )
        seen_paths.add(path)
        entries.append(entry)
    return entries


def load_manifest_fixture_paths(manifest_path: Path) -> set[str]:
    return {entry["path"] for entry in load_manifest_fixture_entries(manifest_path)}
