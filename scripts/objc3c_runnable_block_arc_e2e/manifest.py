"""Package manifest validation and path extraction."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import normalize_rel_path

from .assertions import expect
from .paths import PACKAGE_CONTRACT_ID

REQUIRED_FILE_MANIFEST_KEYS = (
    "compile_wrapper",
    "runtime_library",
    "execution_smoke_script",
    "execution_replay_script",
    "runtime_public_header",
    "runtime_internal_header",
    "block_arc_fixture",
    "block_arc_runtime_abi_probe",
    "block_arc_byref_forwarding_probe",
)
REQUIRED_DIRECTORY_MANIFEST_KEYS = ("execution_fixture_root",)
REQUIRED_MANIFEST_KEYS = (
    "compile_wrapper",
    "runtime_library",
    "execution_smoke_script",
    "execution_replay_script",
    "execution_fixture_root",
    "runtime_public_header",
    "runtime_internal_header",
    "block_arc_fixture",
    "block_arc_runtime_abi_probe",
    "block_arc_byref_forwarding_probe",
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
        if manifest_key in REQUIRED_DIRECTORY_MANIFEST_KEYS:
            expect(candidate.is_dir(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")
        else:
            expect(candidate.is_file(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")


def load_and_validate_manifest(manifest_json_path: Path, *, package_root: Path) -> dict[str, object]:
    manifest = load_json(manifest_json_path)
    validate_package_manifest(manifest, package_root=package_root)
    return manifest


def manifest_path(manifest: dict[str, object], key: str, *, package_root: Path) -> Path:
    return package_root / normalize_rel_path(str(manifest[key]))


__all__ = [
    "REQUIRED_DIRECTORY_MANIFEST_KEYS",
    "REQUIRED_FILE_MANIFEST_KEYS",
    "REQUIRED_MANIFEST_KEYS",
    "load_and_validate_manifest",
    "manifest_path",
    "validate_package_manifest",
]
