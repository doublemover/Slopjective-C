"""Long-horizon operations, adoption, governance, and planning specs."""

from __future__ import annotations

from .action_spec import ActionSpec

LONG_HORIZON_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-long-horizon-operations": ActionSpec("validate-long-horizon-operations", "validate upgrade, revert, soak, aging, and support-window evidence over the live package and application workflows", "python:scripts/check_objc3c_long_horizon_operations_integration.py", validation_tier="full", guarantee_owner="long-horizon support claims stay backed by generated upgrade, revert, soak, aging, package, application, performance, and conformance evidence"),
    "publish-long-horizon-operations": ActionSpec("publish-long-horizon-operations", "publish support-window and long-horizon operator metadata from generated evidence", "python:scripts/publish_objc3c_long_horizon_operations_metadata.py", validation_tier="release", guarantee_owner="operator-facing support-window metadata stays generated from long-horizon evidence and blocks when claim audit reports release blockers"),
}

ADOPTION_LEGIBILITY_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-adoption-legibility": ActionSpec("validate-adoption-legibility", "validate adoption boundary inventory, artifact contract, capability comparison, adoption replay, onboarding, and public claim policy evidence", "python:scripts/check_objc3c_adoption_legibility_integration.py", validation_tier="repo", guarantee_owner="external evaluator, adoption replay, comparison, onboarding, and public claim surfaces stay generated from adoption-legibility contracts"),
    "publish-adoption-legibility": ActionSpec("publish-adoption-legibility", "publish evaluator-facing adoption metadata from generated boundary, comparison, adoption replay, onboarding, and claim-policy evidence", "python:scripts/publish_objc3c_adoption_legibility_metadata.py", validation_tier="release", guarantee_owner="evaluator-facing adoption metadata stays generated from adoption evidence and blocks when claim audit reports release blockers"),
}

GOVERNANCE_SUSTAINABILITY_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-governance-sustainability": ActionSpec("validate-governance-sustainability", "validate governance budget, waiver, stewardship, extension review, anti-regression, artifact, and metadata-publication evidence", "python:scripts/check_objc3c_governance_sustainability_integration.py", validation_tier="repo", guarantee_owner="governance and extension-review claims stay executable across checked-in contracts, public workflow entrypoints, and generated evidence"),
    "publish-governance-sustainability": ActionSpec("publish-governance-sustainability", "publish governance stewardship and extension-review metadata from generated budget, waiver, anti-regression, and artifact evidence", "python:scripts/publish_objc3c_governance_sustainability_metadata.py", validation_tier="release", guarantee_owner="governance publication stays generated from integration evidence and blocks when stewardship or extension-review evidence reports release blockers"),
}

PLANNING_PUBLICATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-post-cutover-issue-evidence": ActionSpec("validate-post-cutover-issue-evidence", "validate Objective-C 3 post-cutover issue evidence coverage and public replay surfaces", "python:scripts/check_objc3c_post_cutover_issue_evidence.py", validation_tier="repo", guarantee_owner="post-cutover issue evidence stays tied to checked-in source truth, registered public commands, and capability/evidence-map rows"),
    "publish-planning-issues": ActionSpec("publish-planning-issues", "publish checked-in Objective-C 3 planning issues to GitHub while preserving GitHub-assigned numbers", "python:scripts/publish_objc3c_planning_issues.py", validation_tier="repo", guarantee_owner="issue publication stays rooted in checked-in planning payloads, durable GitHub number mappings, labels, milestones, and blocker references", pass_through_args=True),
    "check-planning-publication-drift": ActionSpec("check-planning-publication-drift", "audit checked-in Objective-C 3 planning publication references for GitHub mapping drift", "python:scripts/audit_objc3c_planning_publication.py --check", validation_tier="repo", guarantee_owner="planning payloads, markdown reports, durable GitHub mappings, and dependency references stay synchronized with assigned GitHub numbers", pass_through_args=True),
}

ADOPTION_GOVERNANCE_ACTION_SPECS: dict[str, ActionSpec] = {
    **LONG_HORIZON_ACTION_SPECS,
    **ADOPTION_LEGIBILITY_ACTION_SPECS,
    **GOVERNANCE_SUSTAINABILITY_ACTION_SPECS,
    **PLANNING_PUBLICATION_ACTION_SPECS,
}
