"""Artifact registry key and report contracts."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Callable


ARTIFACT_REGISTRY_KEY_CONTRACT_ID = "objc3c.runtime.acceptance.artifact.registry.key.v1"
ARTIFACT_REGISTRY_SUMMARY_CONTRACT_ID = "objc3c.runtime.acceptance.artifact.registry.v1"
ARTIFACT_REGISTRY_REUSE_MODEL = (
    "only opt-in immutable direct-native compile artifacts are copied "
    "within one run; runtime-linked, negative, cache-mutating, and "
    "cold-warm cases stay isolated unless explicitly opted in"
)


def required_compile_artifacts(emit_prefix: str) -> list[str]:
    return [
        f"{emit_prefix}.obj",
        f"{emit_prefix}.ll",
        f"{emit_prefix}.manifest.json",
        f"{emit_prefix}.runtime-registration-manifest.json",
        f"{emit_prefix}.runtime-registration-descriptor.json",
        f"{emit_prefix}.compile-provenance.json",
    ]


def build_artifact_registry_key_payload(
    *,
    fixture: Path,
    extra_args: list[str] | None,
    backend: str,
    emit_prefix: str,
    native_exe: Path,
    runtime_lib: Path,
    default_compile_backend: str,
    repo_display_path: Callable[[Path], str],
    optional_file_sha256_hex: Callable[[Path | None], str],
) -> dict[str, Any]:
    source_path = fixture.resolve()
    return {
        "contract_id": ARTIFACT_REGISTRY_KEY_CONTRACT_ID,
        "source_path": repo_display_path(source_path),
        "source_sha256": optional_file_sha256_hex(source_path),
        "extra_args": list(extra_args or []),
        "backend": backend,
        "emit_prefix": emit_prefix,
        "compiler_binary": repo_display_path(native_exe),
        "compiler_binary_sha256": optional_file_sha256_hex(native_exe),
        "runtime_support_library": repo_display_path(runtime_lib),
        "runtime_support_library_sha256": optional_file_sha256_hex(runtime_lib),
        "environment": {
            "OBJC3C_RUNTIME_ACCEPTANCE_COMPILE_BACKEND": default_compile_backend,
        },
    }


def artifact_registry_key_digest(
    payload: dict[str, Any],
    sha256_text_hex: Callable[[str], str],
) -> str:
    return sha256_text_hex(json.dumps(payload, sort_keys=True))


__all__ = [
    "ARTIFACT_REGISTRY_KEY_CONTRACT_ID",
    "ARTIFACT_REGISTRY_REUSE_MODEL",
    "ARTIFACT_REGISTRY_SUMMARY_CONTRACT_ID",
    "artifact_registry_key_digest",
    "build_artifact_registry_key_payload",
    "required_compile_artifacts",
]
