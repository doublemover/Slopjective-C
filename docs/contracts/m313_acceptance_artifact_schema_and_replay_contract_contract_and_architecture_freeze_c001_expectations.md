# M313 Acceptance Artifact Schema And Replay Contract Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-cleanup-acceptance-artifact-schema-replay-contract/m313-c001-v1`

## Purpose

Freeze the canonical artifact envelope and replay metadata shape that later `M313` acceptance suites and compatibility bridges must emit.

## Contract requirements

- Use one canonical acceptance artifact envelope for shared suite root checks, executable suite summaries, and compatibility-bridge summaries.
- Require deterministic replay metadata so acceptance artifacts can be re-run without relying on milestone-local folklore.
- Keep artifact layout under `tmp/reports/m313/` and make the report-root conventions explicit.
- Require provenance fields that identify the producing tool, owning issue, and validation posture.
- Reserve compatibility-bridge fields now so `M313-C003` can emit bridge evidence without creating a second schema family.

## Required machine-readable outputs

- canonical artifact layout roots
- required acceptance artifact envelope fields
- artifact-class-specific required fields
- replay metadata requirements
- next-issue handoff to `M313-C002`
