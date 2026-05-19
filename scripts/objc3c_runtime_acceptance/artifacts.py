"""Compile artifact reuse registry for runtime acceptance."""

from __future__ import annotations

import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable, Protocol

from .artifact_registry_evidence import ArtifactRegistryEvidence
from .artifact_registry_keys import artifact_registry_key_digest
from .artifact_registry_keys import build_artifact_registry_key_payload
from .artifact_registry_keys import required_compile_artifacts


class ArtifactRegistryProgress(Protocol):
    current_case: dict[str, Any] | None

    def emit(self, message: str) -> None:
        ...


@dataclass(frozen=True)
class ArtifactRegistryConfig:
    root: Path
    native_exe: Path
    runtime_lib: Path
    direct_compile_backend: str
    default_compile_backend: str
    repo_display_path: Callable[[Path], str]
    optional_file_sha256_hex: Callable[[Path | None], str]
    sha256_text_hex: Callable[[str], str]


class RuntimeAcceptanceArtifactRegistry:
    def __init__(self, config: ArtifactRegistryConfig) -> None:
        self._config = config
        self._evidence = ArtifactRegistryEvidence()
        self.entries = self._evidence.entries
        self.reuse_events = self._evidence.reuse_events
        self.miss_events = self._evidence.miss_events

    def _current_case(self, progress: ArtifactRegistryProgress | None) -> str | None:
        if progress and progress.current_case:
            label = progress.current_case.get("label")
            return str(label) if label is not None else None
        return None

    def cache_key(
        self,
        fixture: Path,
        *,
        extra_args: list[str] | None,
        backend: str,
        emit_prefix: str,
    ) -> tuple[str, dict[str, Any]]:
        key_payload = build_artifact_registry_key_payload(
            fixture=fixture,
            extra_args=extra_args,
            backend=backend,
            emit_prefix=emit_prefix,
            native_exe=self._config.native_exe,
            runtime_lib=self._config.runtime_lib,
            default_compile_backend=self._config.default_compile_backend,
            repo_display_path=self._config.repo_display_path,
            optional_file_sha256_hex=self._config.optional_file_sha256_hex,
        )
        key = artifact_registry_key_digest(
            key_payload,
            self._config.sha256_text_hex,
        )
        return key, key_payload

    def required_artifacts(self, emit_prefix: str) -> list[str]:
        return required_compile_artifacts(emit_prefix)

    def validate_artifacts(self, directory: Path, emit_prefix: str) -> list[str]:
        missing = [
            artifact
            for artifact in self.required_artifacts(emit_prefix)
            if not (directory / artifact).is_file()
        ]
        if missing:
            raise RuntimeError(
                "runtime acceptance artifact registry missing required artifacts "
                f"in {directory}: {', '.join(missing)}"
            )
        return self.required_artifacts(emit_prefix)

    def try_reuse(
        self,
        *,
        fixture: Path,
        out_dir: Path,
        extra_args: list[str] | None,
        backend: str,
        emit_prefix: str,
        reuse_policy: str,
        progress: ArtifactRegistryProgress | None = None,
    ) -> bool:
        if (
            reuse_policy == "none"
            or backend != self._config.direct_compile_backend
        ):
            return False
        key, key_payload = self.cache_key(
            fixture,
            extra_args=extra_args,
            backend=backend,
            emit_prefix=emit_prefix,
        )
        entry = self.entries.get(key)
        current_case = self._current_case(progress)
        if entry is None:
            self._evidence.record_miss(
                cache_key_sha256=key,
                current_case=current_case,
                fixture=key_payload["source_path"],
                extra_args=key_payload["extra_args"],
                reuse_policy=reuse_policy,
                reason="no-producer",
            )
            return False
        producer_dir = self._config.root / str(entry["producer_dir"])
        self.validate_artifacts(producer_dir, emit_prefix)
        out_dir.mkdir(parents=True, exist_ok=True)
        copied_artifacts: list[str] = []
        for artifact in sorted(producer_dir.iterdir(), key=lambda item: item.name):
            if artifact.is_file():
                shutil.copy2(artifact, out_dir / artifact.name)
                copied_artifacts.append(artifact.name)
        self.validate_artifacts(out_dir, emit_prefix)
        self._evidence.record_reuse(
            cache_key_sha256=key,
            producer_case=entry.get("producer_case"),
            consumer_case=current_case,
            fixture=key_payload["source_path"],
            extra_args=key_payload["extra_args"],
            producer_dir=entry["producer_dir"],
            consumer_dir=self._config.repo_display_path(out_dir),
            reuse_policy=reuse_policy,
            artifact_paths=copied_artifacts,
        )
        if progress:
            progress.emit(
                "ARTIFACT reuse "
                f"case={current_case} fixture={key_payload['source_path']} "
                f"producer={entry.get('producer_case')} key={key[:12]}"
            )
        return True

    def register(
        self,
        *,
        fixture: Path,
        out_dir: Path,
        extra_args: list[str] | None,
        backend: str,
        emit_prefix: str,
        reuse_policy: str,
        progress: ArtifactRegistryProgress | None = None,
    ) -> None:
        if (
            reuse_policy == "none"
            or backend != self._config.direct_compile_backend
        ):
            return
        artifact_paths = self.validate_artifacts(out_dir, emit_prefix)
        key, key_payload = self.cache_key(
            fixture,
            extra_args=extra_args,
            backend=backend,
            emit_prefix=emit_prefix,
        )
        producer_case = self._current_case(progress)
        self._evidence.register_entry(
            cache_key_sha256=key,
            producer_case=producer_case,
            fixture=key_payload["source_path"],
            extra_args=key_payload["extra_args"],
            producer_dir=self._config.repo_display_path(out_dir),
            reuse_policy=reuse_policy,
            artifact_paths=artifact_paths,
        )

    def summary(self) -> dict[str, Any]:
        return self._evidence.summary()


__all__ = [
    "ArtifactRegistryConfig",
    "ArtifactRegistryProgress",
    "RuntimeAcceptanceArtifactRegistry",
]
