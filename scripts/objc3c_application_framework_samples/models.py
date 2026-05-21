from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class FrameworkSample:
    sample_id: str
    kind: str
    package_id: str
    module_name: str
    source: str
    workspace_manifest: str
    tutorial: str
    artifact_root: str
    capabilities: tuple[str, ...]
    support_claims: tuple[str, ...]
    package_dependencies: tuple[str, ...]
    public_compile_command: str

    @classmethod
    def from_payload(cls, payload: dict[str, Any]) -> "FrameworkSample":
        return cls(
            sample_id=str(payload["id"]),
            kind=str(payload["kind"]),
            package_id=str(payload["package_id"]),
            module_name=str(payload["module_name"]),
            source=str(payload["source"]),
            workspace_manifest=str(payload["workspace_manifest"]),
            tutorial=str(payload["tutorial"]),
            artifact_root=str(payload["artifact_root"]),
            capabilities=tuple(str(item) for item in payload.get("capabilities", [])),
            support_claims=tuple(str(item) for item in payload.get("support_claims", [])),
            package_dependencies=tuple(
                str(item) for item in payload.get("package_dependencies", [])
            ),
            public_compile_command=str(payload["public_compile_command"]),
        )

    def artifact_path(self, root: Path) -> Path:
        return root / self.artifact_root

    def source_path(self, root: Path) -> Path:
        return root / self.source

    def workspace_path(self, root: Path) -> Path:
        return root / self.workspace_manifest

    def tutorial_path(self, root: Path) -> Path:
        return root / self.tutorial
