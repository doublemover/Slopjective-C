"""Compile artifact reuse registry for runtime acceptance."""

from __future__ import annotations

import json
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable, Protocol


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
        self.entries: dict[str, dict[str, Any]] = {}
        self.reuse_events: list[dict[str, Any]] = []
        self.miss_events: list[dict[str, Any]] = []

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
        source_path = fixture.resolve()
        key_payload = {
            "contract_id": "objc3c.runtime.acceptance.artifact.registry.key.v1",
            "source_path": self._config.repo_display_path(source_path),
            "source_sha256": self._config.optional_file_sha256_hex(source_path),
            "extra_args": list(extra_args or []),
            "backend": backend,
            "emit_prefix": emit_prefix,
            "compiler_binary": self._config.repo_display_path(self._config.native_exe),
            "compiler_binary_sha256": self._config.optional_file_sha256_hex(
                self._config.native_exe
            ),
            "runtime_support_library": self._config.repo_display_path(
                self._config.runtime_lib
            ),
            "runtime_support_library_sha256": self._config.optional_file_sha256_hex(
                self._config.runtime_lib
            ),
            "environment": {
                "OBJC3C_RUNTIME_ACCEPTANCE_COMPILE_BACKEND": self._config.default_compile_backend,
            },
        }
        key = self._config.sha256_text_hex(json.dumps(key_payload, sort_keys=True))
        return key, key_payload

    def required_artifacts(self, emit_prefix: str) -> list[str]:
        return [
            f"{emit_prefix}.obj",
            f"{emit_prefix}.ll",
            f"{emit_prefix}.manifest.json",
            f"{emit_prefix}.runtime-registration-manifest.json",
            f"{emit_prefix}.runtime-registration-descriptor.json",
            f"{emit_prefix}.compile-provenance.json",
        ]

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
            self.miss_events.append(
                {
                    "cache_key_sha256": key,
                    "case": current_case,
                    "fixture": key_payload["source_path"],
                    "extra_args": key_payload["extra_args"],
                    "reuse_policy": reuse_policy,
                    "reason": "no-producer",
                }
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
        event = {
            "cache_key_sha256": key,
            "producer_case": entry.get("producer_case"),
            "consumer_case": current_case,
            "fixture": key_payload["source_path"],
            "extra_args": key_payload["extra_args"],
            "producer_dir": entry["producer_dir"],
            "consumer_dir": self._config.repo_display_path(out_dir),
            "reuse_policy": reuse_policy,
            "artifact_paths": copied_artifacts,
        }
        self.reuse_events.append(event)
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
        self.entries.setdefault(
            key,
            {
                "cache_key_sha256": key,
                "producer_case": producer_case,
                "fixture": key_payload["source_path"],
                "extra_args": key_payload["extra_args"],
                "producer_dir": self._config.repo_display_path(out_dir),
                "reuse_policy": reuse_policy,
                "artifact_paths": artifact_paths,
            },
        )

    def summary(self) -> dict[str, Any]:
        return {
            "contract_id": "objc3c.runtime.acceptance.artifact.registry.v1",
            "entry_count": len(self.entries),
            "reuse_count": len(self.reuse_events),
            "miss_count": len(self.miss_events),
            "entries": list(self.entries.values()),
            "reuse_events": self.reuse_events,
            "miss_events": self.miss_events,
            "reuse_model": (
                "only opt-in immutable direct-native compile artifacts are copied "
                "within one run; runtime-linked, negative, cache-mutating, and "
                "cold-warm cases stay isolated unless explicitly opted in"
            ),
        }


__all__ = [
    "ArtifactRegistryConfig",
    "ArtifactRegistryProgress",
    "RuntimeAcceptanceArtifactRegistry",
]
