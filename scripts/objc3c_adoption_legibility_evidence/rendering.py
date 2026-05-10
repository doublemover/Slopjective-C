from __future__ import annotations

from objc3c_adoption_legibility_evidence.model import AdoptionLegibilityEvidenceModel
from objc3c_adoption_legibility_evidence.publication import PublishedAdoptionLegibilityEvidence


def render_console_lines(
    model: AdoptionLegibilityEvidenceModel,
    published: PublishedAdoptionLegibilityEvidence,
) -> list[str]:
    status_line = (
        "objc3c-adoption-legibility-evidence: PASS"
        if model.passed
        else "objc3c-adoption-legibility-evidence: FAIL"
    )
    return [
        f"summary_path: {published.summary_path}",
        f"artifact_path: {published.artifact_path}",
        f"publication_path: {published.publication_path}",
        status_line,
    ]
