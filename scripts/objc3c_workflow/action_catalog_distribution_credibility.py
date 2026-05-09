"""Distribution credibility action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

DISTRIBUTION_CREDIBILITY_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-distribution-credibility-surface": ActionSpec("check-distribution-credibility-surface", "validate the checked-in distribution-credibility source surface", "python:scripts/check_distribution_credibility_source_surface.py", validation_tier="repo", guarantee_owner="distribution credibility only publishes from the checked-in trust-signal, install-doc, operator, and drill contracts"),
    "check-distribution-credibility-schema-surface": ActionSpec("check-distribution-credibility-schema-surface", "validate the checked-in distribution-credibility schema surface", "python:scripts/check_distribution_credibility_schema_surface.py", validation_tier="repo", guarantee_owner="distribution dashboard and trust-report artifacts stay on checked-in schema contracts"),
    "build-distribution-credibility-dashboard": ActionSpec("build-distribution-credibility-dashboard", "build the machine-owned distribution credibility dashboard from the live release artifacts", "python:scripts/build_objc3c_distribution_credibility_dashboard.py", validation_tier="repo", guarantee_owner="distribution credibility summaries stay tied to the live release-foundation, packaging-channel, release-operations, and release-evidence surfaces"),
    "publish-distribution-credibility": ActionSpec("publish-distribution-credibility", "publish the machine-owned distribution trust report artifacts", "python:scripts/publish_objc3c_distribution_trust_report.py", validation_tier="repo", guarantee_owner="distribution trust reporting stays derived from the live release drill artifacts and checked-in credibility policies"),
    "validate-distribution-credibility": ActionSpec("validate-distribution-credibility", "run the integrated distribution-credibility workflow", "runner-internal distribution-credibility child actions", validation_tier="nightly", guarantee_owner="distribution trust signals and release-drill reporting stay executable on the live shipped release surfaces"),
    "validate-distribution-credibility-end-to-end": ActionSpec("validate-distribution-credibility-end-to-end", "validate distribution trust-report publication and evidence linkage end to end", "python:scripts/check_objc3c_distribution_credibility_end_to_end.py", validation_tier="full", guarantee_owner="distribution credibility artifacts stay coherent with the live package-channel and release-operations evidence paths"),
}


__all__ = ["DISTRIBUTION_CREDIBILITY_ACTION_SPECS"]
