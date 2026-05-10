from __future__ import annotations

from objc3c_adoption_legibility_evidence.evidence_loading import load_adoption_legibility_inputs
from objc3c_adoption_legibility_evidence.model import build_adoption_legibility_model
from objc3c_adoption_legibility_evidence.paths import AdoptionLegibilityEvidencePaths
from objc3c_adoption_legibility_evidence.publication import publish_adoption_legibility_evidence
from objc3c_adoption_legibility_evidence.rendering import render_console_lines


def main() -> int:
    paths = AdoptionLegibilityEvidencePaths.for_root()
    inputs = load_adoption_legibility_inputs(paths)
    model = build_adoption_legibility_model(paths, inputs)
    published = publish_adoption_legibility_evidence(paths, model)
    for line in render_console_lines(model, published):
        print(line)
    return 0 if model.passed else 1
