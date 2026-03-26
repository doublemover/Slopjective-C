# M318-C002 Expectations

Contract ID: `objc3c-governance-budget-enforcement-implementation/m318-c002-v1`

`M318-C002` makes the long-lived governance guard live.

The implementation must:
- provide one shared runner that emits stage summaries and a topology summary;
- provide one CI workflow that invokes the shared runner;
- fail closed on:
  - public command budget regression
  - validation budget regression
  - expired exception records
  - source-hygiene regression
  - artifact-authenticity regression

The implementation should reuse existing policy/proof surfaces where they already exist,
especially the `M315` anti-noise and authenticity checks, instead of recreating them.
