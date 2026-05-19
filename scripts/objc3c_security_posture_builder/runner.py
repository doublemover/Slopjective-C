"""Security posture builder entrypoint."""

from __future__ import annotations

import sys

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .boundaries import build_trust_boundaries
from .constants import DISTRIBUTION_TRUST_REPORT, POSTURE_PATH, SUMMARY_PATH
from .payloads import build_evidence_paths, build_posture_payload, build_summary_payload, write_posture_outputs
from .reports import load_required_reports
from .state import headline_for_state, security_state_from_boundaries


def main() -> int:
    try:
        reports = load_required_reports()
    except RuntimeError as exc:
        print(f"objc3c-security-posture: FAIL\n- {exc}", file=sys.stderr)
        return 1

    if not DISTRIBUTION_TRUST_REPORT.is_file():
        print(
            f"objc3c-security-posture: FAIL\n- missing distribution trust report {repo_rel(DISTRIBUTION_TRUST_REPORT)}",
            file=sys.stderr,
        )
        return 1

    trust_report = load_json(DISTRIBUTION_TRUST_REPORT)
    trust_state = str(trust_report.get("trust_state", "blocked"))
    trust_boundaries = build_trust_boundaries(reports, trust_report, trust_state)
    security_state = security_state_from_boundaries(trust_boundaries)
    headline = headline_for_state(security_state)
    evidence_paths = build_evidence_paths(trust_report)
    posture_payload = build_posture_payload(security_state, headline, trust_boundaries, evidence_paths)
    summary_payload = build_summary_payload(security_state, headline, trust_boundaries, evidence_paths)
    write_posture_outputs(posture_payload, summary_payload)

    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"published_posture: {repo_rel(POSTURE_PATH)}")
    print("objc3c-security-posture: OK")
    return 0
