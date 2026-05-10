"""Support package for the claimability dashboard release-blocker contract."""

from __future__ import annotations

from .blocker_contract import (
    DashboardReleaseBlockerProjection,
    evaluate_contract_checks,
    parse_projection,
)
from .cli import build_arg_parser
from .config import (
    DASHBOARD_SCRIPT,
    DEFAULT_JSON_OUT,
    DEFAULT_MD_OUT,
    DEFAULT_POLICY,
    RELEASE_BLOCKER_SCRIPT,
    ROOT,
    RUNBOOK,
    SUMMARY_CONTRACT_ID,
    resolve_repo_path,
)
from .dashboard_payload import build_summary
from .errors import ContractError
from .input_loading import (
    SourceTruthInputs,
    load_source_truth_inputs,
    read_json,
    require_string,
    require_string_list,
)
from .orchestration import main
from .output import write_outputs
from .rendering import render_markdown
from .source_contracts import (
    DASHBOARD_DECISION_FIELDS,
    DASHBOARD_SUMMARY,
    NON_PRODUCTION_PUBLIC_CLAIM_CLASSES,
    PUBLIC_SUMMARY,
    PUBLIC_SUMMARY_DECISION_FIELDS,
    ROLLOUT_CLASS_CANDIDATE,
    ROLLOUT_CLASS_PREVIEW,
)
from .tooling import expected_json_report, repo_rel, write_report_outputs

__all__ = [
    "DASHBOARD_DECISION_FIELDS",
    "DASHBOARD_SCRIPT",
    "DASHBOARD_SUMMARY",
    "DEFAULT_JSON_OUT",
    "DEFAULT_MD_OUT",
    "DEFAULT_POLICY",
    "NON_PRODUCTION_PUBLIC_CLAIM_CLASSES",
    "PUBLIC_SUMMARY",
    "PUBLIC_SUMMARY_DECISION_FIELDS",
    "RELEASE_BLOCKER_SCRIPT",
    "ROLLOUT_CLASS_CANDIDATE",
    "ROLLOUT_CLASS_PREVIEW",
    "ROOT",
    "RUNBOOK",
    "SUMMARY_CONTRACT_ID",
    "ContractError",
    "DashboardReleaseBlockerProjection",
    "SourceTruthInputs",
    "build_arg_parser",
    "build_summary",
    "evaluate_contract_checks",
    "expected_json_report",
    "load_source_truth_inputs",
    "main",
    "parse_projection",
    "read_json",
    "repo_rel",
    "render_markdown",
    "require_string",
    "require_string_list",
    "resolve_repo_path",
    "write_outputs",
    "write_report_outputs",
]
