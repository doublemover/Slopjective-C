#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel, resolve_repo_path

from build_full_envelope_claimability_dashboard_payloads import (
    build_dashboard_payload,
    build_public_payload,
    build_report_text,
    load_claimability_inputs,
)

try:
    from build_full_envelope_claimability_contracts import (
        CLAIMABILITY_REPORT_MD,
        DASHBOARD_SUMMARY,
        PUBLIC_SUMMARY,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        CLAIMABILITY_REPORT_MD,
        DASHBOARD_SUMMARY,
        PUBLIC_SUMMARY,
    )


ROOT = Path(__file__).resolve().parents[1]
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_REPORT_DIR = ROOT / "tmp/reports/full-envelope-claimability"
OUT_ARTIFACT_DIR = ROOT / "tmp/artifacts/full-envelope-claimability/report"
DASHBOARD_JSON = resolve_repo_path(DASHBOARD_SUMMARY)
PUBLIC_JSON = resolve_repo_path(PUBLIC_SUMMARY)
REPORT_MD = resolve_repo_path(CLAIMABILITY_REPORT_MD)


def main() -> int:
    inputs = load_claimability_inputs(root=ROOT)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    dashboard_payload = build_dashboard_payload(inputs)
    public_payload = build_public_payload(
        dashboard_payload=dashboard_payload,
        contract=inputs.contract,
        report_markdown_path=REPORT_MD,
    )
    report_text = build_report_text(
        dashboard_payload=dashboard_payload,
        public_claim_class=public_payload["public_claim_class"],
    )

    OUT_REPORT_DIR.mkdir(parents=True, exist_ok=True)
    OUT_ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(DASHBOARD_JSON, dashboard_payload)
    write_json_file(PUBLIC_JSON, public_payload)
    REPORT_MD.write_text(report_text, encoding="utf-8")

    summary = {
        "contract_id": "objc3c.full_envelope.claimability.dashboard.publication.summary.v1",
        "status": "PASS",
        "runner_path": "scripts/build_full_envelope_claimability_dashboard.py",
        "runbook_mentions_dashboard_surface": "## Dashboard And Claim Publication Surface" in runbook_text,
        "dashboard_json": repo_rel(DASHBOARD_JSON),
        "public_summary_json": repo_rel(PUBLIC_JSON),
        "report_markdown_path": repo_rel(REPORT_MD),
        "current_rollout_class": dashboard_payload["current_rollout_class"],
        "public_claim_class": public_payload["public_claim_class"],
    }
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
