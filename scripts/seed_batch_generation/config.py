"""Configuration constants for v0.13 seed batch generation."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_MATRIX_PATH = ROOT / "tmp" / "reports" / "v013_future_work_seed_matrix.md"
DEFAULT_OUTPUT_PATH = ROOT / "tmp" / "reports" / "v013_seed_dependency_graph.json"

SEED_ID_RE = re.compile(r"^V013-[A-Z]+-[0-9]{2}$")
EDGE_ID_RE = re.compile(r"^EDGE-V013-[0-9]{3}$")
WAVE_ID_RE = re.compile(r"^W[0-9]+$")
DATE_RE = re.compile(r"([0-9]{4}-[0-9]{2}-[0-9]{2})")
TOKEN_RE = re.compile(r"(V013-[A-Z]+-[0-9]{2}|BATCH-V013-[SML]-[0-9]{2})")
OWNER_DATE_RE = re.compile(r"^[0-9]{4}-[0-9]{2}-[0-9]{2}$")
OWNER_TOKEN_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9._:+/-]*$")

SEED_TABLE_COLUMNS = (
    "Seed ID",
    "Family",
    "Worklane",
    "Proposed GH issue title",
    "Primary artifact targets",
    "Depends on",
    "Shard class",
    "Acceptance gate ID",
)
EDGE_TABLE_COLUMNS = ("Edge ID", "Predecessor", "Successor", "Type", "Rationale")
WAVE_TABLE_COLUMNS = ("Wave", "Seeds eligible for execution (all hard predecessors satisfied)")
BATCH_TABLE_COLUMNS = (
    "Batch ID",
    "Class",
    "Included seed IDs",
    "Entry prerequisites",
    "Exit signal",
)
PRIORITY_TABLE_COLUMNS = (
    "Seed ID",
    "CPI",
    "DUV",
    "RBV",
    "ERC",
    "ECP",
    "DC",
    "Priority score",
    "Tier",
)

REQUIRED_EDGE_IDS = [f"EDGE-V013-{index:03d}" for index in range(1, 22)]
REQUIRED_WAVE_IDS = [f"W{index}" for index in range(0, 8)]
OWNER_MAP_CONTRACT_ID = "V013-SEED-OWNER-REGISTRY-v1"
OWNER_MAP_SEED_ID = "V013-TOOL-03"
INVALID_OWNER_VALUES = {
    "na",
    "n/a",
    "none",
    "tbd",
    "todo",
    "unknown",
    "unassigned",
}
DEFAULT_UNASSIGNED_OWNER = "unassigned"

CLASS_RANK = {
    "small": 0,
    "medium": 1,
    "large": 2,
}

FAMILY_LABEL = {
    "FAM-SPEC": "spec",
    "FAM-TOOL": "tooling",
    "FAM-GOV": "governance",
    "FAM-REL": "release",
    "FAM-CONF": "conformance",
}

WORKLANE_LABEL = {
    "WL-SPEC": "spec",
    "WL-TOOL": "tooling",
    "WL-GOV": "governance",
    "WL-REL": "release",
    "WL-CONF": "conformance",
}
