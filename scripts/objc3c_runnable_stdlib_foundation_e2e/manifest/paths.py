from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import normalize_rel_path

from ..assertions import expect


def package_path(package_root: Path, manifest_value: object) -> Path:
    return package_root / normalize_rel_path(str(manifest_value))


def require_packaged_file(path: Path) -> None:
    expect(path.is_file(), f"packaged runnable toolchain missing required stdlib file {path}")
