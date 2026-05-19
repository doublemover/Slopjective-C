"""Package manifest validation and path extraction."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import normalize_rel_path

from .assertions import expect
from .paths import PACKAGE_CONTRACT_ID

REQUIRED_MANIFEST_KEYS = (
    "compile_wrapper",
    "runtime_library",
    "execution_smoke_script",
    "execution_replay_script",
    "runtime_public_header",
    "runtime_internal_header",
    "interop_runtime_fixture",
    "interop_runtime_consumer_fixture",
    "interop_header_bridge_fixture",
    "interop_header_bridge_consumer_fixture",
    "interop_packaging_probe",
    "interop_bridge_generation_probe",
)


def validate_package_manifest(manifest: dict[str, object], *, package_root: Path) -> None:
    expect(
        manifest.get("contract_id") == PACKAGE_CONTRACT_ID,
        "runnable toolchain package manifest published the wrong contract id",
    )

    for manifest_key in REQUIRED_MANIFEST_KEYS:
        relative_path = manifest.get(manifest_key)
        expect(isinstance(relative_path, str) and relative_path, f"package manifest did not publish {manifest_key}")
        candidate = package_root / normalize_rel_path(relative_path)
        expect(candidate.is_file(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")


def manifest_path(manifest: dict[str, object], key: str, *, package_root: Path) -> Path:
    return package_root / normalize_rel_path(str(manifest[key]))


__all__ = ["REQUIRED_MANIFEST_KEYS", "manifest_path", "validate_package_manifest"]
