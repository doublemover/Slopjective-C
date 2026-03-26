# M313 Historical Checker Compatibility Bridge And Deprecation Surface Core Feature Expansion Expectations (C003)

Contract ID: `objc3c-cleanup-historical-checker-compatibility-bridge/m313-c003-v1`

## Purpose

Implement the compatibility bridge that maps historical checker/readiness/pytest-wrapper families onto the new shared suite artifacts and emits schema-compliant deprecation summaries.

## Bridge requirements

- Emit `M313-C001` compatibility-bridge summaries under `tmp/reports/m313/compatibility/<bridge_id>/summary.json`.
- Keep bridge families aligned to the consolidation routes frozen in `M313-B003`.
- Measure legacy wrappers observed and remaining for each bridge family.
- Freeze a concrete deprecation owner issue for every bridge family.
- Hand the resulting bridge surface directly to `M313-D001` for CI-topology freezing.

## Required machine-readable outputs

- compatibility bridge plan keyed to route families
- schema-compliant compatibility bridge summaries
- measured legacy wrapper counts
- next-issue handoff to `M313-D001`
