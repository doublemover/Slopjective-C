#!/usr/bin/env python3
from __future__ import annotations

try:
    from objc3c_claimability_dashboard_release_blocker_contract import (
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SCRIPT,
        DASHBOARD_SUMMARY,
        DEFAULT_JSON_OUT,
        DEFAULT_MD_OUT,
        DEFAULT_POLICY,
        NON_PRODUCTION_PUBLIC_CLAIM_CLASSES,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        RELEASE_BLOCKER_SCRIPT,
        ROLLOUT_CLASS_CANDIDATE,
        ROLLOUT_CLASS_PREVIEW,
        ROOT,
        RUNBOOK,
        SUMMARY_CONTRACT_ID,
        ContractError,
        build_arg_parser,
        build_summary,
        expected_json_report,
        main,
        read_json,
        repo_rel,
        render_markdown,
        require_string,
        require_string_list,
        write_outputs,
        write_report_outputs,
    )
except ModuleNotFoundError:
    from scripts.objc3c_claimability_dashboard_release_blocker_contract import (
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SCRIPT,
        DASHBOARD_SUMMARY,
        DEFAULT_JSON_OUT,
        DEFAULT_MD_OUT,
        DEFAULT_POLICY,
        NON_PRODUCTION_PUBLIC_CLAIM_CLASSES,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        RELEASE_BLOCKER_SCRIPT,
        ROLLOUT_CLASS_CANDIDATE,
        ROLLOUT_CLASS_PREVIEW,
        ROOT,
        RUNBOOK,
        SUMMARY_CONTRACT_ID,
        ContractError,
        build_arg_parser,
        build_summary,
        expected_json_report,
        main,
        read_json,
        repo_rel,
        render_markdown,
        require_string,
        require_string_list,
        write_outputs,
        write_report_outputs,
    )


if __name__ == "__main__":
    raise SystemExit(main())
