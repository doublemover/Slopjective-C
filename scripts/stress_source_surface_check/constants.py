"""Static paths and contract identifiers for stress source-surface validation."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SURFACE_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "source-surface-summary.json"
SURFACE_CONTRACT_ID = "objc3c.stress.source.surface.v1"
SUMMARY_CONTRACT_ID = "objc3c.stress.source.surface.summary.v1"
SOURCE_CHECK_SCRIPT = "scripts/check_stress_source_surface.py"
WORKFLOW_SURFACE = "tests/tooling/fixtures/stress/workflow_surface.json"
CLAIM_GATE = "tests/tooling/fixtures/stress/claim_gate.json"
EXPECTED_FAMILIES = [
    "parser-sema-fuzz",
    "lowering-runtime-stress",
    "mixed-module-differential",
    "replay-backed-contracts",
]
