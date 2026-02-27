# Abstract Machine Consistency Audit 2026Q1

## Metadata

- Audit timestamp (UTC): `2026-02-23T11:41:50Z`
- Baseline commit SHA: `aed96f7e968bbd9ea9d6cccad8f9a3a4e5f29e91`
- Protocol version: `v0.11-A03`
- Auditor: `Codex (Lane A Wave 8 shard, issue_143 package)`

## Owner snapshot

Captured using the deterministic owner-resolution commands from
`spec/planning/issue_143_abstract_machine_audit_plan.md` Section 6.1:

```text
AM owner: doublemover <153689082+doublemover@users.noreply.github.com>
Part 0 owner: doublemover <153689082+doublemover@users.noreply.github.com>
Part 3 owner: doublemover <153689082+doublemover@users.noreply.github.com>
Part 6 owner: doublemover <153689082+doublemover@users.noreply.github.com>
Part 7 owner: doublemover <153689082+doublemover@users.noreply.github.com>
Part 8 owner: doublemover <153689082+doublemover@users.noreply.github.com>
Conformance owner: doublemover <153689082+doublemover@users.noreply.github.com>
Governance backup owner: doublemover <153689082+doublemover@users.noreply.github.com>
```

## Anchor verification

| File | Command | Result |
| --- | --- | --- |
| `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md` | `rg -N "\{#am-2-2\}|\{#am-4-4\}|\{#am-5\}|\{#am-6-1\}|\{#am-6-2\}|\{#am-6-3\}|\{#am-6-4\}|\{#am-6-5\}|\{#am-6-6\}|\{#am-6-7\}|\{#am-7\}"` | `PASS AM count=11 expected=11` |
| `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md` | `rg -N "\{#part-0-1\}|\{#part-0-2-1\}|\{#part-0-2-2\}|\{#part-0-2-4\}|\{#part-0-3-1\}|\{#part-0-3-2\}|\{#part-0-4-1\}|\{#part-0-4-6\}|\{#part-0-4-14\}|\{#part-0-4-16\}|\{#part-0-4-17\}|\{#part-0-4-18\}|\{#part-0-4-19\}|\{#part-0-4-20\}|\{#part-0-6\}"` | `PASS Part0 count=15 expected=15` |
| `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md` | `rg -N "\{#part-3-4-1\}|\{#part-3-4-2\}|\{#part-3-4-5\}"` | `PASS Part3 count=3 expected=3` |
| `spec/PART_6_ERRORS_RESULTS_THROWS.md` | `rg -N "\{#part-6-3\}|\{#part-6-4\}|\{#part-6-5\}|\{#part-6-6\}"` | `PASS Part6 count=4 expected=4` |
| `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md` | `rg -N "\{#part-7-3-3\}|\{#part-7-6\}|\{#part-7-9-1\}|\{#part-7-9-2\}|\{#part-7-9-3\}|\{#part-7-9-4\}"` | `PASS Part7 count=6 expected=6` |
| `spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md` | `rg -N "\{#part-8-1\}|\{#part-8-2-3\}|\{#part-8-3\}|\{#part-8-6\}"` | `PASS Part8 count=4 expected=4` |
| AM matrix/cross-part rows | `rg -n "AM-T0[1-9]|AM-T1[0-9]|Part 3|Part 6|Part 7|Part 8" spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md` | `PASS (rows AM-T01..AM-T19 and Part links present)` |
| Part 0 AM/cross-part terminology links | `rg -n "Abstract Machine and Semantic Core|#am|Part 3|Part 6|Part 7|Part 8|shall|must|ill-formed; diagnostic required" spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md` | `PASS (AM link + requirement wording present)` |

## Delta ledger

| delta_id | class | canonical_home | impacted_sections | am_matrix_rows | summary | accountable_owner | responsible_owner | backup_owner | target_date | status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| AM-AUDIT-2026Q1-01 | normative conflict | Part 6 (`#part-6-6`) | `#am-6-5`, `#part-6-6` | `AM-T07`, `AM-T08`, `AM-T13` | Part 6.6 introductory text still states optional propagation may early-return nil or throw by context, while AM.6.5 and later Part 6.6 clauses forbid optional postfix propagation in `throws` and `Result` contexts in v1. | doublemover <153689082+doublemover@users.noreply.github.com> | doublemover <153689082+doublemover@users.noreply.github.com> | doublemover <153689082+doublemover@users.noreply.github.com> | 2026-03-05 | resolved |
| AM-AUDIT-2026Q1-02 | editorial mismatch | Part 0 (`#part-0-4-1`) | `#part-0-4-1`, `#part-3-4-5` | `AM-T09` | Part 0 requires the exact phrase `ill-formed; diagnostic required` for static prohibitions; Part 3 restriction bullets use abbreviated `ill-formed` wording in some prohibitions. | doublemover <153689082+doublemover@users.noreply.github.com> | doublemover <153689082+doublemover@users.noreply.github.com> | doublemover <153689082+doublemover@users.noreply.github.com> | 2026-03-08 | tracked |
| AM-AUDIT-2026Q1-03 | missing example | Part 7 (`#part-7-6-5`) | `#part-7-6-5`, `#part-8-3`, `#part-8-6` | `AM-T12`, `AM-T14` | Part 7 cancellation cleanup guarantees are normative, but no worked cross-part example demonstrates cancellation-driven unwind ordering with `@cleanup`/`@resource` and explicit lifetime controls. | doublemover <153689082+doublemover@users.noreply.github.com> | doublemover <153689082+doublemover@users.noreply.github.com> | doublemover <153689082+doublemover@users.noreply.github.com> | 2026-03-12 | tracked |

## Detailed delta records

### AM-AUDIT-2026Q1-01

- Class: `normative conflict`
- Canonical home: `spec/PART_6_ERRORS_RESULTS_THROWS.md#part-6-6`
- Evidence excerpts:
  - `spec/PART_6_ERRORS_RESULTS_THROWS.md:294`: `T?`: unwrap or early-return `nil` (or throw) depending on surrounding function's declared return/effects.
  - `spec/PART_6_ERRORS_RESULTS_THROWS.md:337`: If the enclosing function is `throws` or returns `Result<…>`, use of `e?` is ill-formed in v1.
  - `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md:242`: using optional propagation to implicitly map into `throws` or `Result` contexts is ill-formed in v1.
- Reconciliation direction:
  - Treat Part 6 line-level rule at `#part-6-6` (ill-formed in `throws`/`Result`) plus AM.6.5 as canonical v1 behavior.
  - Remove/clarify the ambiguous parenthetical in the introductory bullet so no nil-to-throw implicit mapping interpretation remains.

### AM-AUDIT-2026Q1-02

- Class: `editorial mismatch`
- Canonical home: `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md#part-0-4-1`
- Evidence excerpts:
  - `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md:96`: Preferred wording in this draft is: **"ill-formed; diagnostic required"**.
  - `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md:97`: Parts 1-12 shall use this exact phrase when introducing static prohibitions.
  - `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md:391`: `?.` is ill-formed in strict mode and diagnosed in permissive mode.
- Reconciliation direction:
  - Normalize prohibition wording in Part 3 to include the exact phrase while keeping existing strict/permissive profile semantics intact.

### AM-AUDIT-2026Q1-03

- Class: `missing example`
- Canonical home: `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-6-5`
- Evidence excerpts:
  - `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md:402`: `defer` handlers shall execute exactly once, in LIFO order, when scope exit occurs due to cancellation-driven unwind.
  - `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md:404`: Async-frame captured objects retained across suspension shall be released when the frame is destroyed, including cancellation unwind.
  - `spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md:165`: each successfully initialized annotated field is registered for cleanup.
- Reconciliation direction:
  - Add one worked example under Part 7.9 or Part 8.3 showing `try await` cancellation path with `defer` + `@cleanup` + `keepAlive`, mapped to `AM-T12` and `AM-T14`.

## Validation transcript

| Command | Output | Result |
| --- | --- | --- |
| `Test-Path reports/spec_sync/abstract_machine_audit_2026Q1.md` | `True` | PASS |
| `rg -n "^## " reports/spec_sync/abstract_machine_audit_2026Q1.md` | `3:## Metadata`, `10:## Owner snapshot`, `26:## Anchor verification`, `39:## Delta ledger`, `47:## Detailed delta records`, `83:## Validation transcript`, `94:## Open blockers` | PASS |
| `if (-not (Test-Path 'reports/spec_sync/abstract_machine_audit_2026Q1.md')) { 'BLOCKED: missing reports/spec_sync/abstract_machine_audit_2026Q1.md'; exit 0 }; $total=(rg -N "^\| AM-AUDIT-2026Q1-[0-9]{2} \|" reports/spec_sync/abstract_machine_audit_2026Q1.md | Measure-Object).Count; $typed=(rg -N "^\| AM-AUDIT-2026Q1-[0-9]{2} \| (normative conflict|editorial mismatch|missing example) \|" reports/spec_sync/abstract_machine_audit_2026Q1.md | Measure-Object).Count; if ($total -gt 0 -and $total -eq $typed) { "PASS total=$total typed=$typed" } else { "FAIL total=$total typed=$typed" }` | `PASS total=3 typed=3` | PASS |
| `if (-not (Test-Path 'reports/spec_sync/abstract_machine_audit_2026Q1.md')) { 'BLOCKED: missing reports/spec_sync/abstract_machine_audit_2026Q1.md'; exit 0 }; $total=(rg -N "^\| AM-AUDIT-2026Q1-[0-9]{2} \|" reports/spec_sync/abstract_machine_audit_2026Q1.md | Measure-Object).Count; $filled=(rg -N "^\| AM-AUDIT-2026Q1-[0-9]{2} \| [^|]+ \| [^|]+ \| [^|]+ \| [^|]+ \| [^|]+ \| [^|]+ \| [^|]+ \| [^|]+ \| [0-9]{4}-[0-9]{2}-[0-9]{2} \| [^|]+ \|" reports/spec_sync/abstract_machine_audit_2026Q1.md | Measure-Object).Count; if ($total -gt 0 -and $total -eq $filled) { "PASS total=$total filled=$filled" } else { "FAIL total=$total filled=$filled" }` | `PASS total=3 filled=3` | PASS |
| `if (-not (Test-Path 'reports/spec_sync/abstract_machine_audit_2026Q1.md')) { 'BLOCKED: missing reports/spec_sync/abstract_machine_audit_2026Q1.md'; exit 0 }; $conflict=(rg -N "^\| AM-AUDIT-2026Q1-[0-9]{2} \| normative conflict \|" reports/spec_sync/abstract_machine_audit_2026Q1.md | Measure-Object).Count; $covered=(rg -N "^\| AM-AUDIT-2026Q1-[0-9]{2} \| normative conflict \| [^|]+ \| [^|]+ \| [^|]*AM-T[0-9]{2}[^|]* \|" reports/spec_sync/abstract_machine_audit_2026Q1.md | Measure-Object).Count; if ($conflict -eq $covered) { "PASS conflict=$conflict covered=$covered" } else { "FAIL conflict=$conflict covered=$covered" }` | `PASS conflict=1 covered=1` | PASS |
| `if (-not (Test-Path 'reports/spec_sync/abstract_machine_audit_2026Q1.md')) { 'BLOCKED: missing reports/spec_sync/abstract_machine_audit_2026Q1.md'; exit 0 }; $unresolved = @(rg -N "^\| AM-AUDIT-2026Q1-[0-9]{2} \| normative conflict \| .* \| (open|pending|blocked|escalated|in-progress) \|" reports/spec_sync/abstract_machine_audit_2026Q1.md | ForEach-Object { ($_ -split '\|')[1].Trim() }); $mapped=0; foreach ($id in $unresolved) { if (rg -q "$id.*(T\\+24h|T\\+48h|T\\+72h)" reports/spec_sync/abstract_machine_audit_2026Q1.md) { $mapped++ } }; if ($unresolved.Count -eq $mapped) { "PASS unresolved=$($unresolved.Count) mapped=$mapped" } else { "FAIL unresolved=$($unresolved.Count) mapped=$mapped" }` | `PASS unresolved=0 mapped=0` | PASS |
| `$reportPresent = Test-Path 'reports/spec_sync/abstract_machine_audit_2026Q1.md'; $lintOutput = python scripts/spec_lint.py; $lintExit=$LASTEXITCODE; $lintOk = ($lintOutput -match 'spec-lint: OK'); "lint_output=$lintOutput"; if ($reportPresent -and $lintExit -eq 0 -and $lintOk) { "PASS report_present=True lint_exit=0 lint_ok=True" } else { "FAIL report_present=$reportPresent lint_exit=$lintExit lint_ok=$lintOk" }` | `lint_output=spec-lint: OK` then `PASS report_present=True lint_exit=0 lint_ok=True` | PASS |

## Open blockers

Unresolved `normative conflict` entries requiring escalation mapping:

- None. `unresolved normative conflict count = 0` in this audit run (`V143-05: PASS unresolved=0 mapped=0`).
