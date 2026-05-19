"""Artifact loading for release-claims claimable feature cases."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT
from ..runtime_contract_release import RELEASE_CLAIMABLE_SURFACE_FIXTURE


@dataclass(frozen=True)
class ClaimableSurfaceArtifacts:
    fixture: Path
    compile_dir: Path
    report: dict[str, Any]
    publication: dict[str, Any]
    advanced_feature_gate: dict[str, Any]
    release_candidate_matrix: dict[str, Any]

    @property
    def runtime_capability_report(self) -> dict[str, Any]:
        return self.report.get("runtime_capability_report", {})

    @property
    def feature_claim_truth_surface(self) -> dict[str, Any]:
        return self.report.get("feature_claim_truth_surface", {})

    @property
    def canonical_selection_claim_semantics(self) -> dict[str, Any]:
        return self.report.get("canonical_selection_claim_semantics", {})


def compile_claimable_surface(run_dir: Path, case_id: str) -> ClaimableSurfaceArtifacts:
    case_dir = run_dir / case_id
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)
    return ClaimableSurfaceArtifacts(
        fixture=fixture,
        compile_dir=compile_dir,
        report=json.loads(
            (compile_dir / "module.objc3-conformance-report.json").read_text(
                encoding="utf-8"
            )
        ),
        publication=json.loads(
            (compile_dir / "module.objc3-conformance-publication.json").read_text(
                encoding="utf-8"
            )
        ),
        advanced_feature_gate=json.loads(
            (compile_dir / "module.objc3-advanced-feature-gate.json").read_text(
                encoding="utf-8"
            )
        ),
        release_candidate_matrix=json.loads(
            (compile_dir / "module.objc3-release-candidate-matrix.json").read_text(
                encoding="utf-8"
            )
        ),
    )
