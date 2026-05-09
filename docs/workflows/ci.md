# CI

CI validation should run schema, docs, and targeted executable evidence checks
for any capability row changed by a patch.
CI command docs should point at npm-bridge actions and checked-in schema truth;
direct helper invocations are implementation details, not public workflow
contracts.

Workflow files, local docs, and site source must agree on these facts:

- the package bridge is `npm run objc3c -- <action>`,
- action truth comes from `scripts/objc3c_workflow/action_catalog.py`,
- generated command-surface docs are derived from the bridge and action catalog,
- capability truth comes from `docs/support/capability_matrix.*` plus
  `docs/support/evidence_map.md`,
- shared JSON/schema ownership comes from checked-in schemas and the native
  `objc3c_json` / artifact JSON modules.

CI docs must not mention retired command names as supported entrypoints.
