#!/usr/bin/env python3
"""Validate the Objective-C 3.0 optimization proof model."""

from __future__ import annotations

from objc3c_semantic_optimization_pipeline import (
    PROOF_MODEL_REPORT_PATH,
    validate_optimization_proof_model,
)
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel


def main() -> int:
    result = validate_optimization_proof_model()
    write_json_file(PROOF_MODEL_REPORT_PATH, result.payload)
    print(f"summary_path: {repo_rel(PROOF_MODEL_REPORT_PATH)}")
    print(f"objc3c-optimization-proof-model: {result.payload['status']}")
    if result.failures:
        for failure in result.failures:
            print(f"failure: {failure}")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
