"""Shared report I/O bindings."""

from __future__ import annotations

from .import_paths import ensure_import_paths

ensure_import_paths()

from objc3c_tooling.json_io import load_optional_json_object as load_json  # noqa: E402
from objc3c_tooling.json_io import write_json_file  # noqa: E402
from objc3c_tooling.paths import repo_rel  # noqa: E402


__all__ = ["load_json", "repo_rel", "write_json_file"]
