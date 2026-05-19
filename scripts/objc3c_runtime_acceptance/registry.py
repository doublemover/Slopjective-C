"""Runtime acceptance artifact registry ownership."""

from __future__ import annotations

from .artifacts import ArtifactRegistryConfig
from .artifacts import RuntimeAcceptanceArtifactRegistry
from .checksums import optional_file_sha256_hex
from .checksums import sha256_text_hex
from .compile_backends import DEFAULT_COMPILE_BACKEND
from .compile_backends import DIRECT_COMPILE_BACKEND
from .paths import NATIVE_EXE
from .paths import ROOT
from .paths import RUNTIME_LIB
from .progress_format import repo_display_path


def build_acceptance_artifact_registry() -> RuntimeAcceptanceArtifactRegistry:
    return RuntimeAcceptanceArtifactRegistry(
        ArtifactRegistryConfig(
            root=ROOT,
            native_exe=NATIVE_EXE,
            runtime_lib=RUNTIME_LIB,
            direct_compile_backend=DIRECT_COMPILE_BACKEND,
            default_compile_backend=DEFAULT_COMPILE_BACKEND,
            repo_display_path=repo_display_path,
            optional_file_sha256_hex=optional_file_sha256_hex,
            sha256_text_hex=sha256_text_hex,
        )
    )


ACCEPTANCE_ARTIFACT_REGISTRY = build_acceptance_artifact_registry()


__all__ = [
    "ACCEPTANCE_ARTIFACT_REGISTRY",
    "build_acceptance_artifact_registry",
]
