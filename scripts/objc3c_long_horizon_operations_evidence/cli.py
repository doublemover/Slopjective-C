from __future__ import annotations

from objc3c_long_horizon_operations_evidence.evidence_loading import load_long_horizon_inputs
from objc3c_long_horizon_operations_evidence.model import build_long_horizon_model
from objc3c_long_horizon_operations_evidence.paths import LongHorizonEvidencePaths
from objc3c_long_horizon_operations_evidence.publication import publish_long_horizon_evidence
from objc3c_long_horizon_operations_evidence.rendering import render_console_lines


def main() -> int:
    paths = LongHorizonEvidencePaths.for_root()
    inputs = load_long_horizon_inputs(paths)
    model = build_long_horizon_model(paths, inputs)
    published = publish_long_horizon_evidence(paths, model)
    for line in render_console_lines(model, published):
        print(line)
    return 0 if model.passed else 1
