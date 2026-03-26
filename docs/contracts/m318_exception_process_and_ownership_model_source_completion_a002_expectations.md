# M318-A002 Expectations

Contract ID: `objc3c-governance-anti-noise-exception-process/m318-a002-v1`

`M318-A002` defines the durable anti-noise exception process that replaces the
cleanup-only `M313-B005` registry as the long-lived source-completion surface for
future budget waivers.

The process model must:
- keep exceptions empty by default;
- require ownership, rationale, approval, and expiry for every active exception;
- distinguish the budget family being exceeded from the concrete surface being added;
- point every exception at a rollback or replacement target so temporary noise has a
  removal path;
- preserve explicit status values so CI and reporting can fail closed on expired or
  abandoned exceptions.

The source-complete surface must include:
- a governance-owned process JSON file under `spec/governance/`;
- a governance-owned registry JSON file under `spec/governance/`;
- package metadata that points future automation to the budget-map and registry paths.

This issue does not yet implement CI enforcement, alarms, or maintainer review
checklists. It only establishes the durable source-of-truth model that later `M318`
issues will enforce.
