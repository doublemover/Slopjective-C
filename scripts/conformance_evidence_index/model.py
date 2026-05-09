from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class ArtifactRecord:
    artifact_path: str
    file_sha256: str
    size_bytes: int
    media_type: str
    profile_id: str
    release_id: str
    artifact_id: str
    manifest_kind: str
    schema_ref: str | None
    source_generated_at: str | None
    issue_ref: str | None

    def as_dict(self) -> dict[str, Any]:
        return {
            "artifact_path": self.artifact_path,
            "file_sha256": self.file_sha256,
            "size_bytes": self.size_bytes,
            "media_type": self.media_type,
            "profile_id": self.profile_id,
            "release_id": self.release_id,
            "artifact_id": self.artifact_id,
            "manifest_kind": self.manifest_kind,
            "schema_ref": self.schema_ref,
            "source_generated_at": self.source_generated_at,
            "issue_ref": self.issue_ref,
        }
