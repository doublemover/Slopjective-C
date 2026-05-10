from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_GRAPH_PATH = ROOT / "tmp" / "reports" / "v013_seed_dependency_graph.json"
DEFAULT_OUTPUT_PATH = ROOT / "tmp" / "reports" / "v013_seed_issue_payloads.json"

SEED_ID_RE = re.compile(r"^V013-[A-Z]+-[0-9]{2}$")
WAVE_ID_RE = re.compile(r"^W[0-9]+$")
ACCEPTANCE_GATE_ID_RE = re.compile(r"^AC-V013-[A-Z]+-[0-9]{2}$")
SHARD_CLASSES = {"small", "medium", "large"}
SEED_TOKEN_RE = re.compile(r"(?<![A-Z0-9-])(V013-[A-Z]+-[0-9]{2})(?![A-Z0-9-])")
ISSUE_STATES = {"open", "closed"}
SOURCE_CONTRACT_ID = "V013-TOOL-03-SEED-DAG-v1"
SOURCE_SEED_ID = "V013-TOOL-03"


class ParseError(ValueError):
    """Raised when the seed graph input cannot be parsed deterministically."""


@dataclass(frozen=True)
class SeedMetadata:
    seed_id: str
    title: str
    wave_id: str
    depends_on: tuple[str, ...]
    shard_class: str
    acceptance_gate_id: str
    priority_score: int
    duv: int
    dc: int
    tier: str


@dataclass(frozen=True)
class TemplateMetadata:
    labels: tuple[str, ...]
    validation_commands: tuple[str, ...]


@dataclass(frozen=True)
class IssueOverlayMetadata:
    number: int
    title: str
    labels: tuple[str, ...]
    issue_url: str | None
    issue_state: str
    closed_at: str | None
    matched_seed_ids: tuple[str, ...]
