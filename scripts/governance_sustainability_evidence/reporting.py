"""Public payload builder for governance sustainability evidence."""

from __future__ import annotations

from .loading import report_payload
from .model import (
    GovernanceSustainabilityEvidenceInputs,
    GovernanceSustainabilityEvidencePayloads,
)
from .rendering import render_evidence_payload, render_summary_payload
from .schema_checks import claim_audit, release_blockers


def build_governance_sustainability_evidence_payloads(
    inputs: GovernanceSustainabilityEvidenceInputs,
) -> GovernanceSustainabilityEvidencePayloads:
    budget_inventory = report_payload(
        inputs.report_payloads,
        "budget-inventory",
        "governance_budget_inventory_summary.json",
    )
    policy = report_payload(
        inputs.report_payloads,
        "sustainable-progress-policy",
        "governance_policy_summary.json",
    )
    maintainer_review = report_payload(
        inputs.report_payloads,
        "maintainer-review-regression",
        "governance_maintainer_review_summary.json",
    )
    extension_policy = report_payload(
        inputs.report_payloads,
        "extension-review-policy",
        "governance_extension_review_policy_summary.json",
    )
    extension_workflow = report_payload(
        inputs.report_payloads,
        "extension-review-workflow",
        "governance_extension_review_workflow_summary.json",
    )
    stewardship = report_payload(
        inputs.report_payloads,
        "stewardship-semantics",
        "governance_stewardship_semantics_summary.json",
    )
    budget_enforcement = report_payload(
        inputs.report_payloads,
        "budget-enforcement",
        "governance_budget_enforcement_summary.json",
    )
    anti_regression = report_payload(
        inputs.report_payloads,
        "anti-regression",
        "governance_anti_regression_summary.json",
    )
    integration = report_payload(
        inputs.report_payloads,
        "integration",
        "governance_sustainability_integration_summary.json",
    )

    failures = release_blockers(
        report_payloads=inputs.report_payloads,
        budget_enforcement=budget_enforcement,
        integration=integration,
    )
    audit = claim_audit(
        failures=failures,
        budget_inventory=budget_inventory,
    )

    evidence = render_evidence_payload(
        contract=inputs.contract,
        waiver_registry=inputs.waiver_registry,
        generated_reports=inputs.generated_reports,
        artifact_contract_path=inputs.artifact_contract_path,
        budget_inventory=budget_inventory,
        budget_enforcement=budget_enforcement,
        anti_regression=anti_regression,
        policy=policy,
        maintainer_review=maintainer_review,
        extension_policy=extension_policy,
        extension_workflow=extension_workflow,
        stewardship=stewardship,
        integration=integration,
        claim_audit=audit,
        failures=failures,
    )
    summary = render_summary_payload(
        evidence=evidence,
        generated_reports=inputs.generated_reports,
        artifact_contract_path=inputs.artifact_contract_path,
        evidence_artifact_path=inputs.evidence_artifact_path,
        failures=failures,
    )
    return GovernanceSustainabilityEvidencePayloads(
        evidence=evidence,
        summary=summary,
        failures=failures,
    )


__all__ = [
    "build_governance_sustainability_evidence_payloads",
]
