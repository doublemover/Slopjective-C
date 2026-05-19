from __future__ import annotations

from dataclasses import dataclass

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from objc3c_long_horizon_operations_evidence.model import LongHorizonEvidenceModel
from objc3c_long_horizon_operations_evidence.paths import LongHorizonEvidencePaths


@dataclass(frozen=True)
class PublishedLongHorizonEvidence:
    summary_path: str
    artifact_path: str


def publish_long_horizon_evidence(
    paths: LongHorizonEvidencePaths,
    model: LongHorizonEvidenceModel,
) -> PublishedLongHorizonEvidence:
    paths.artifact_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.artifact_path, model.artifact)

    paths.summary_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.summary_path, model.summary)

    return PublishedLongHorizonEvidence(
        summary_path=repo_rel(paths.summary_path, root=paths.root),
        artifact_path=repo_rel(paths.artifact_path, root=paths.root),
    )
