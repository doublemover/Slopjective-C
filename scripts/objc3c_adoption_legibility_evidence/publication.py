from __future__ import annotations

from dataclasses import dataclass

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from objc3c_adoption_legibility_evidence.model import AdoptionLegibilityEvidenceModel
from objc3c_adoption_legibility_evidence.paths import AdoptionLegibilityEvidencePaths


@dataclass(frozen=True)
class PublishedAdoptionLegibilityEvidence:
    summary_path: str
    artifact_path: str
    publication_path: str


def publish_adoption_legibility_evidence(
    paths: AdoptionLegibilityEvidencePaths,
    model: AdoptionLegibilityEvidenceModel,
) -> PublishedAdoptionLegibilityEvidence:
    paths.artifact_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.artifact_path, model.artifact)

    paths.publication_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.publication_path, model.publication)

    paths.summary_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.summary_path, model.summary)

    return PublishedAdoptionLegibilityEvidence(
        summary_path=repo_rel(paths.summary_path, root=paths.root),
        artifact_path=repo_rel(paths.artifact_path, root=paths.root),
        publication_path=repo_rel(paths.publication_path, root=paths.root),
    )
