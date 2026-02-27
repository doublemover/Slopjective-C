# Remaining Task Review Catalog (510-Task Sweep)

_Generated on 2026-02-23 by scripts/seed_remaining_spec_tasks.py._

## Coverage Summary

- Total reviewed tasks: **5**
- Bucket counts: conformance-checklist **2**, release-evidence **1**, planning-checklist **2**
- Lane counts: A **0**, B **4**, C **0**, D **1**
- Definition quality counts: strong **0**, medium **1**, weak **4**

## Parallel Worklane Guidance

- Lane A: normative/spec closure tasks with cross-part semantic contracts.
- Lane B: implementation/tooling and conformance automation tasks.
- Lane C: governance, extension process, and ecosystem policy tasks.
- Lane D: release readiness, checkpoints, handoff, and operational control tasks.

## Task Reviews

### SPT-0001 - [SPT-0001][Lane D] Capture strict-system evidence bundle artifact and validation command output

- Source: `spec/conformance/profile_release_evidence_checklist.md:3`
- Bucket: `release-evidence`
- Lane: `Lane D - Program Control & Release`
- Milestone: `Conformance: Strict System (E)`
- Shard: `release-evidence-checklist`
- Original checkbox: `Capture strict-system evidence bundle artifact and validation command output.`
- Review quality: `medium`
- Gaps identified:
  - Missing explicit dependency/order constraints.
  - Outcome is activity-oriented; add measurable completion criteria.
- Improved objective: Capture strict-system evidence bundle artifact and validation command output
- Improved deliverables:
  - Produce the required evidence bundle fields/artifacts for the referenced profile gate.
  - Attach validation outputs proving schema/command-level correctness.
  - Record artifact digests and evidence pointers needed for release sign-off.
- Improved acceptance criteria:
  - All required release evidence keys/metrics for the row are present and conform to schema/contract.
  - Validation commands run successfully and outputs are attached in issue comments.
  - Source checklist row is updated to checked state with commit SHA + evidence reference.
- Dependencies:
  - No explicit hard dependency in the source row; treat as lane-parallel unless blocked by shared files.
  - Lane-level dependency: execute under Lane D governance (Program Control & Release).
- Validation commands:
  - `python scripts/spec_lint.py`
  - `npm run check:task-hygiene`
  - `python scripts/check_release_evidence.py`

### SPT-0002 - [SPT-0002][Lane B] [CORE] Validate parser accepts let bindings in strict mode

- Source: `spec/CONFORMANCE_PROFILE_CHECKLIST.md:3`
- Bucket: `conformance-checklist`
- Lane: `Lane B - Implementation & Tooling`
- Milestone: `Conformance: Core (E)`
- Shard: `conformance-profile-checklist`
- Original checkbox: `[CORE] Validate parser accepts `let` bindings in strict mode.`
- Review quality: `weak`
- Gaps identified:
  - Missing explicit artifact/file target in the task line.
  - Missing deterministic validation command or test expectation.
  - Missing explicit dependency/order constraints.
  - Outcome is activity-oriented; add measurable completion criteria.
- Improved objective: [CORE] Validate parser accepts let bindings in strict mode
- Improved deliverables:
  - Implement or codify the specified language/toolchain behavior.
  - Add or update positive/negative conformance tests that demonstrate the behavior.
  - Update conformance status evidence so the checklist row can be marked complete with traceable links.
- Improved acceptance criteria:
  - Required compiler/spec behavior is implemented (or explicitly documented as unsupported) with no ambiguity.
  - Validation commands run successfully and outputs are attached in issue comments.
  - Source checklist row is updated to checked state with commit SHA + evidence reference.
- Dependencies:
  - No explicit hard dependency in the source row; treat as lane-parallel unless blocked by shared files.
  - Lane-level dependency: execute under Lane B governance (Implementation & Tooling).
- Validation commands:
  - `python scripts/spec_lint.py`
  - `npm run check:task-hygiene`

### SPT-0003 - [SPT-0003][Lane B] [OPT-CXX] Confirm interop marker appears in emitted metadata

- Source: `spec/CONFORMANCE_PROFILE_CHECKLIST.md:4`
- Bucket: `conformance-checklist`
- Lane: `Lane B - Implementation & Tooling`
- Milestone: `Conformance: Optional Interop (E)`
- Shard: `conformance-profile-checklist`
- Original checkbox: `[OPT-CXX] Confirm interop marker appears in emitted metadata.`
- Review quality: `weak`
- Gaps identified:
  - Missing explicit artifact/file target in the task line.
  - Missing deterministic validation command or test expectation.
  - Missing explicit dependency/order constraints.
  - Outcome is activity-oriented; add measurable completion criteria.
- Improved objective: [OPT-CXX] Confirm interop marker appears in emitted metadata
- Improved deliverables:
  - Implement or codify the specified language/toolchain behavior.
  - Add or update positive/negative conformance tests that demonstrate the behavior.
  - Update conformance status evidence so the checklist row can be marked complete with traceable links.
- Improved acceptance criteria:
  - Required compiler/spec behavior is implemented (or explicitly documented as unsupported) with no ambiguity.
  - Validation commands run successfully and outputs are attached in issue comments.
  - Source checklist row is updated to checked state with commit SHA + evidence reference.
- Dependencies:
  - No explicit hard dependency in the source row; treat as lane-parallel unless blocked by shared files.
  - Lane-level dependency: execute under Lane B governance (Implementation & Tooling).
- Validation commands:
  - `python scripts/spec_lint.py`
  - `npm run check:task-hygiene`

### SPT-0004 - [SPT-0004][Lane B] Document kickoff decision record for lane batching in artifact tracker

- Source: `spec/planning/issue_140_seed_fixture.md:3`
- Bucket: `planning-checklist`
- Lane: `Lane B - Implementation & Tooling`
- Milestone: `v0.12 Lane B - Implementation & Tooling`
- Shard: `planning-issue-140`
- Original checkbox: `Document kickoff decision record for lane batching in artifact tracker.`
- Review quality: `weak`
- Gaps identified:
  - Missing deterministic validation command or test expectation.
  - Missing explicit dependency/order constraints.
  - Outcome is activity-oriented; add measurable completion criteria.
- Improved objective: Document kickoff decision record for lane batching in artifact tracker
- Improved deliverables:
  - Update the referenced planning artifact with concrete, non-placeholder content for this checklist row.
  - Capture review/approval evidence (owner, date, and decision record) linked in the issue.
  - Attach validation command results and closeout traceability so the row can be checked reliably.
- Improved acceptance criteria:
  - The original checklist intent is fully satisfied with concrete artifacts and links.
  - Validation commands run successfully and outputs are attached in issue comments.
  - Source checklist row is updated to checked state with commit SHA + evidence reference.
- Dependencies:
  - No hard dependency encoded in the row; schedule as parallel-ready within the same lane shard.
  - Lane-level dependency: execute under Lane B governance (Implementation & Tooling).
- Validation commands:
  - `python scripts/spec_lint.py`

### SPT-0005 - [SPT-0005][Lane B] Link dependency to Issue #111 and A-03 for traceability

- Source: `spec/planning/issue_140_seed_fixture.md:4`
- Bucket: `planning-checklist`
- Lane: `Lane B - Implementation & Tooling`
- Milestone: `v0.12 Lane B - Implementation & Tooling`
- Shard: `planning-issue-140`
- Original checkbox: `Link dependency to Issue #111 and A-03 for traceability.`
- Review quality: `weak`
- Gaps identified:
  - Missing explicit artifact/file target in the task line.
  - Missing deterministic validation command or test expectation.
  - Outcome is activity-oriented; add measurable completion criteria.
- Improved objective: Link dependency to Issue #111 and A-03 for traceability
- Improved deliverables:
  - Update the referenced planning artifact with concrete, non-placeholder content for this checklist row.
  - Capture review/approval evidence (owner, date, and decision record) linked in the issue.
  - Attach validation command results and closeout traceability so the row can be checked reliably.
- Improved acceptance criteria:
  - The original checklist intent is fully satisfied with concrete artifacts and links.
  - Validation commands run successfully and outputs are attached in issue comments.
  - Source checklist row is updated to checked state with commit SHA + evidence reference.
- Dependencies:
  - Traceability references: closed seed issue(s) #111; use for context while implementing this new task.
  - Explicit cross-task references detected: A-03; honor sequencing when these artifacts are touched.
  - Lane-level dependency: execute under Lane B governance (Implementation & Tooling).
- Validation commands:
  - `python scripts/spec_lint.py`

