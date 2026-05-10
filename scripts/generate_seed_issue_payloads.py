#!/usr/bin/env python3
"""Generate deterministic v0.13 seed issue payload records."""

from __future__ import annotations

import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from seed_issue_payloads.cli import build_parser
from seed_issue_payloads.cli import main
from seed_issue_payloads.graph import parse_seeds
from seed_issue_payloads.graph import parse_templates
from seed_issue_payloads.graph import seed_sort_key
from seed_issue_payloads.graph import wave_index
from seed_issue_payloads.issues import completion_status_for
from seed_issue_payloads.issues import extract_seed_ids_from_labels
from seed_issue_payloads.issues import extract_seed_ids_from_title
from seed_issue_payloads.issues import parse_issue_labels
from seed_issue_payloads.issues import parse_issue_number
from seed_issue_payloads.issues import parse_issue_state
from seed_issue_payloads.issues import parse_issues_overlay
from seed_issue_payloads.issues import resolve_seed_issue_overlay
from seed_issue_payloads.model import ACCEPTANCE_GATE_ID_RE
from seed_issue_payloads.model import DEFAULT_GRAPH_PATH
from seed_issue_payloads.model import DEFAULT_OUTPUT_PATH
from seed_issue_payloads.model import ISSUE_STATES
from seed_issue_payloads.model import SEED_ID_RE
from seed_issue_payloads.model import SEED_TOKEN_RE
from seed_issue_payloads.model import SHARD_CLASSES
from seed_issue_payloads.model import SOURCE_CONTRACT_ID
from seed_issue_payloads.model import SOURCE_SEED_ID
from seed_issue_payloads.model import WAVE_ID_RE
from seed_issue_payloads.model import IssueOverlayMetadata
from seed_issue_payloads.model import ParseError
from seed_issue_payloads.model import SeedMetadata
from seed_issue_payloads.model import TemplateMetadata
from seed_issue_payloads.parse_utils import expect_dict
from seed_issue_payloads.parse_utils import expect_int
from seed_issue_payloads.parse_utils import expect_list
from seed_issue_payloads.parse_utils import expect_nonempty_str
from seed_issue_payloads.parse_utils import parse_optional_nonempty_str
from seed_issue_payloads.parse_utils import parse_string_list
from seed_issue_payloads.payload import build_payload
from seed_issue_payloads.payload import generate

__all__ = [
    "ACCEPTANCE_GATE_ID_RE",
    "DEFAULT_GRAPH_PATH",
    "DEFAULT_OUTPUT_PATH",
    "ISSUE_STATES",
    "IssueOverlayMetadata",
    "ParseError",
    "SEED_ID_RE",
    "SEED_TOKEN_RE",
    "SHARD_CLASSES",
    "SOURCE_CONTRACT_ID",
    "SOURCE_SEED_ID",
    "SeedMetadata",
    "TemplateMetadata",
    "WAVE_ID_RE",
    "build_parser",
    "build_payload",
    "completion_status_for",
    "expect_dict",
    "expect_int",
    "expect_list",
    "expect_nonempty_str",
    "extract_seed_ids_from_labels",
    "extract_seed_ids_from_title",
    "generate",
    "main",
    "parse_issue_labels",
    "parse_issue_number",
    "parse_issue_state",
    "parse_issues_overlay",
    "parse_optional_nonempty_str",
    "parse_seeds",
    "parse_string_list",
    "parse_templates",
    "resolve_seed_issue_overlay",
    "seed_sort_key",
    "wave_index",
]


if __name__ == "__main__":
    raise SystemExit(main())
