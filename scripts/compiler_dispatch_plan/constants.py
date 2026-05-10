from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_ISSUES_JSON = ROOT / "tmp" / "gh_open_issues_pages.json"
LANE_ORDER: tuple[str, ...] = ("A", "B", "C", "D", "E")
TASK_ID_RE = re.compile(r"\[M(?P<milestone>\d+)-(?P<lane>[A-E])(?P<seq>\d{3})\]")
COMPILER_DISPATCH_CONTRACT_ID = "objc3c.compiler.dispatch.plan.v1"
COMPILER_DISPATCH_FIXTURE_CONTRACT_ID = (
    "objc3c.compiler.dispatch.fixture.owner_contract.v1"
)
COMPILER_DISPATCH_OWNER = "compiler-dispatch-plan"
COMPILER_DISPATCH_SNAPSHOT_OWNER = "compiler-dispatch-snapshot"
COMPILER_DISPATCH_RESULT_OWNER = "compiler-dispatch-result"
COMPILER_DISPATCH_ARTIFACT_OWNER = "compiler-dispatch-artifact"
COMPILER_DISPATCH_STATUS_OWNER = "compiler-dispatch-status"
OWNER_CONTRACT_FIELDS: tuple[str, ...] = (
    "contract_id",
    "dispatch_owner",
    "snapshot_owner",
    "result_owner",
    "artifact_owner",
    "status_owner",
    "no_retired_route_or_evidence_log_claims",
)
