"""Runtime acceptance compile artifact registry instance."""

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


ACCEPTANCE_ARTIFACT_REGISTRY = RuntimeAcceptanceArtifactRegistry(
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


__all__ = ["ACCEPTANCE_ARTIFACT_REGISTRY"]
