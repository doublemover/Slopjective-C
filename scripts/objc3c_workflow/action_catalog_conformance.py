"""Conformance corpus action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CONFORMANCE_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-conformance-corpus": ActionSpec(
        "validate-conformance-corpus",
        "run the integrated conformance corpus taxonomy indexing and current gate-surface validation flow",
        "python:scripts/check_objc3c_conformance_corpus_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "conformance corpus taxonomy, retained longitudinal suites, coverage indexing, "
            "and current gate surfaces stay executable on the live public workflow"
        ),
    ),
    "validate-public-conformance-suite": ActionSpec(
        "validate-public-conformance-suite",
        "validate and stage the stable public conformance suite package, fixture boundary, external-validation intake policy, and release-candidate profile",
        "python:scripts/check_objc3c_public_conformance_suite.py",
        validation_tier="repo",
        guarantee_owner=(
            "public conformance suite taxonomy, package replay evidence, fixture-publication "
            "boundary, external-validation admission policy, and release-candidate profile stay "
            "checked-in and executable through the public workflow"
        ),
    ),
    "check-conformance-minima": ActionSpec(
        "check-conformance-minima",
        "verify conformance suite minima and required families",
        "pwsh:scripts/check_conformance_suite.ps1",
        validation_tier="ci",
        guarantee_owner="conformance minima stay routed through the public workflow bridge",
    ),
    "validate-runnable-conformance-corpus": ActionSpec(
        "validate-runnable-conformance-corpus",
        "validate the staged runnable conformance corpus package surface end to end",
        "python:scripts/check_objc3c_runnable_conformance_corpus_end_to_end.py",
        validation_tier="full",
        guarantee_owner=(
            "packaged conformance corpus contracts, retained longitudinal suites, and current "
            "gate surfaces stay reproducible from the staged runnable toolchain bundle"
        ),
    ),
}
