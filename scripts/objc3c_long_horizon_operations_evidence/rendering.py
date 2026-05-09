from __future__ import annotations

from objc3c_long_horizon_operations_evidence.model import LongHorizonEvidenceModel
from objc3c_long_horizon_operations_evidence.publication import PublishedLongHorizonEvidence


def render_console_lines(
    model: LongHorizonEvidenceModel,
    published: PublishedLongHorizonEvidence,
) -> list[str]:
    status_line = (
        "objc3c-long-horizon-evidence: PASS"
        if model.passed
        else "objc3c-long-horizon-evidence: FAIL"
    )
    return [
        f"summary_path: {published.summary_path}",
        f"artifact_path: {published.artifact_path}",
        status_line,
    ]
