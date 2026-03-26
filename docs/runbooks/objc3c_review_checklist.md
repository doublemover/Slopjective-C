# Objective-C 3 Review Checklist

Use this checklist for roadmap, conformance, and cleanup work that can affect governed
budgets or policy surfaces.

## Required review questions

- Which governed budget family changed, if any?
- Is the change within budget, or does it require an approved exception record?
- Which validation posture and concrete evidence support the change?
- What rollback or regression path applies if a budget alarm or policy guard fails?

## Regression handling

- Expired exceptions block merge.
- Missing exception references block merge when the issue budget impact is
  `requires_exception_record`.
- Budget-affecting changes must name a rollback path.
- If a policy or budget guard fails after merge, triage against the relevant budget
  family first, then decide whether to roll back or open a governed exception.
