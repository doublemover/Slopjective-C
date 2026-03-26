# M318-A001 Expectations

Contract ID: `objc3c-governance-anti-noise-budget-map/m318-a001-v1`

`M318-A001` freezes the long-lived anti-noise governance inventory so later `M318`
policy and automation work enforce one stable budget map instead of relying on
maintainer memory.

The inventory must cover at least these budget families:
- public command surface budget consumed from `M314`;
- validation-growth budget and no-new-growth rule consumed from `M313`;
- source-hygiene and milestone-residue zero-target boundaries consumed from `M315`;
- artifact-authenticity and synthetic-fixture guardrails consumed from `M315`;
- the currently active exception-registry surface that `M318-A002` will replace with
  a durable governance-owned process.

The frozen map must explicitly record:
- the source-of-truth file for each budget;
- the current measured or enforced maximum values;
- the owner issue already responsible for the underlying policy;
- the downstream `M318` issue that will automate or enforce the budget.

This issue is an inventory freeze only. It must not claim that the long-term
governance registry, CI alarms, or maintainer checklist already exist.
