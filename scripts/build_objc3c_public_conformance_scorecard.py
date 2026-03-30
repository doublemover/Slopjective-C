#!/usr/bin/env python3
"""Build the live public conformance scorecard from upstream reports."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "public_conformance_reporting"
    / "stability_policy.json"
)
CORPUS_INTEGRATION_PATH = ROOT / "tmp" / "reports" / "conformance" / "corpus-integration-summary.json"
EXTERNAL_INTEGRATION_PATH = ROOT / "tmp" / "reports" / "external-validation" / "integration-summary.json"
EXTERNAL_PUBLICATION_PATH = ROOT / "tmp" / "reports" / "external-validation" / "publication-summary.json"
OUTPUT_PATH = ROOT / "tmp" / "reports" / "public-conformance" / "scorecard-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.public_conformance_reporting.scorecard.summary.v1"
SCHEMA_ANCHORS = [
    "schemas/objc3-conformance-dashboard-status-v1.schema.json",
    "schemas/objc3-conformance-evidence-bundle-v1.schema.json",
    "scripts/check_release_evidence.py",
]


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def fail(message: str) -> int:
    print(f"objc3c-public-conformance-scorecard: FAIL\n- {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def require_json(path: Path, *, kind: str) -> dict[str, Any]:
    if not path.is_file():
        raise RuntimeError(f"missing {kind}: {repo_rel(path)}")
    return load_json(path)


def find_score_band(policy: dict[str, Any], score: int) -> dict[str, Any]:
    for band in policy["score_bands"]:
        if band["min_score"] <= score <= band["max_score"]:
            return band
    raise RuntimeError(f"no score band matched score {score}")


def main() -> int:
    try:
        policy = require_json(POLICY_PATH, kind="stability policy")
        corpus = require_json(CORPUS_INTEGRATION_PATH, kind="corpus integration summary")
        external_integration = require_json(
            EXTERNAL_INTEGRATION_PATH,
            kind="external validation integration summary",
        )
        external_publication = require_json(
            EXTERNAL_PUBLICATION_PATH,
            kind="external validation publication summary",
        )
    except RuntimeError as exc:
        return fail(str(exc))

    minimum_claimability = policy["minimum_claimability"]
    deductions_policy = policy["deductions"]
    weights = policy["scoring_weights"]

    schema_anchor_paths = [ROOT / relative_path for relative_path in SCHEMA_ANCHORS]
    missing_schema_anchors = [
        repo_rel(path) for path in schema_anchor_paths if not path.exists()
    ]

    failures: list[str] = []
    hard_blocks: list[str] = []
    deductions: list[dict[str, Any]] = []

    score = 0

    corpus_pass = corpus.get("status") == "PASS"
    external_integration_pass = external_integration.get("status") == "PASS"
    retained_suite_count = int(corpus.get("retained_suite_count", 0))
    manifest_summary_count = int(corpus.get("manifest_summary_count", 0))
    accepted_fixture_count = int(external_publication.get("accepted_fixture_count", 0))
    redacted_fixture_count = int(external_publication.get("redacted_fixture_count", 0))
    blocked_fixture_count = int(external_publication.get("blocked_fixture_count", 0))

    if corpus_pass:
        score += int(weights["conformance_corpus"])
    else:
        hard_blocks.append("corpus integration summary did not pass")

    if external_integration_pass and accepted_fixture_count >= int(
        minimum_claimability["accepted_fixture_count"]
    ):
        score += int(weights["external_validation"])
    else:
        hard_blocks.append("external validation integration did not establish the minimum accepted evidence set")

    if not missing_schema_anchors:
        score += int(weights["schema_and_release_anchors"])
    else:
        hard_blocks.append("checked-in dashboard and release-evidence schema anchors are missing")

    retained_gap = max(
        0,
        int(minimum_claimability["retained_suite_count"]) - retained_suite_count,
    )
    if retained_gap:
        penalty = retained_gap * int(deductions_policy["missing_retained_suite_penalty"])
        score -= penalty
        deductions.append(
            {
                "deduction_id": "retained-suite-gap",
                "amount": penalty,
                "reason": f"retained suite count {retained_suite_count} is below the minimum {minimum_claimability['retained_suite_count']}",
            }
        )

    manifest_gap = max(
        0,
        int(minimum_claimability["manifest_summary_count"]) - manifest_summary_count,
    )
    if manifest_gap:
        penalty = manifest_gap * int(deductions_policy["missing_manifest_summary_penalty"])
        score -= penalty
        deductions.append(
            {
                "deduction_id": "manifest-summary-gap",
                "amount": penalty,
                "reason": f"manifest summary count {manifest_summary_count} is below the minimum {minimum_claimability['manifest_summary_count']}",
            }
        )

    if redacted_fixture_count:
        penalty = redacted_fixture_count * int(deductions_policy["redacted_fixture_penalty"])
        score -= penalty
        deductions.append(
            {
                "deduction_id": "redacted-evidence",
                "amount": penalty,
                "reason": f"{redacted_fixture_count} external-validation fixtures remain redacted-summary",
            }
        )

    if blocked_fixture_count:
        penalty = blocked_fixture_count * int(deductions_policy["blocked_fixture_penalty"])
        score -= penalty
        deductions.append(
            {
                "deduction_id": "blocked-evidence",
                "amount": penalty,
                "reason": f"{blocked_fixture_count} external-validation fixtures remain blocked from public promotion",
            }
        )

    score = max(0, min(100, score))
    score_band = find_score_band(policy, score)

    if retained_gap:
        hard_blocks.append("conformance corpus retained suite inventory is below the minimum claimability floor")
    if manifest_gap:
        hard_blocks.append("conformance corpus manifest summary inventory is below the minimum claimability floor")
    if accepted_fixture_count < int(minimum_claimability["accepted_fixture_count"]):
        hard_blocks.append("external validation accepted fixture count is below the minimum claimability floor")

    public_status = score_band["public_status"]
    badge = score_band["badge"]
    claim_ready = badge == "claim-ready"

    if hard_blocks:
        public_status = "blocked"
        badge = "blocked"
        claim_ready = False
    elif blocked_fixture_count or redacted_fixture_count:
        public_status = "caution"
        badge = "provisional"
        claim_ready = False

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "policy_path": repo_rel(POLICY_PATH),
        "upstream_reports": {
            "corpus_integration": repo_rel(CORPUS_INTEGRATION_PATH),
            "external_validation_integration": repo_rel(EXTERNAL_INTEGRATION_PATH),
            "external_validation_publication": repo_rel(EXTERNAL_PUBLICATION_PATH),
        },
        "schema_anchor_paths": [repo_rel(path) for path in schema_anchor_paths],
        "missing_schema_anchors": missing_schema_anchors,
        "score": score,
        "score_band_id": score_band["band_id"],
        "badge": badge,
        "public_status": public_status,
        "claim_ready": claim_ready,
        "upstream_status": {
            "corpus_integration": corpus.get("status"),
            "external_validation_integration": external_integration.get("status"),
            "external_validation_publication": external_publication.get("status"),
        },
        "claimability_counts": {
            "retained_suite_count": retained_suite_count,
            "manifest_summary_count": manifest_summary_count,
            "accepted_fixture_count": accepted_fixture_count,
            "redacted_fixture_count": redacted_fixture_count,
            "blocked_fixture_count": blocked_fixture_count,
        },
        "deductions": deductions,
        "hard_blocks": hard_blocks,
        "failures": failures,
    }
    OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(OUTPUT_PATH)}")
    print("objc3c-public-conformance-scorecard: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

