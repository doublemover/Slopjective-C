# ObjC3C Modular Developer Guide (`M134-E001`)

## 1. Purpose

This guide is the onboarding path for module-level and subsystem-level work in the modularized ObjC3C codebase.

It defines:

- lane-oriented workflow for parallel development,
- ownership expectations for each module and subsystem,
- troubleshooting paths for common failures,
- test selection by subsystem.

## 2. module and subsystem map

The modular frontend is organized into stage modules plus integration subsystems:

| module/subsystem | Primary paths | Allowed downstream dependencies | Primary lane ownership |
| --- | --- | --- | --- |
| `lex` subsystem | `native/objc3c/src/lex/*` | none | Lane A |
| `parse` subsystem | `native/objc3c/src/parse/*` | `lex` | Lane A |
| `sema` subsystem | `native/objc3c/src/sema/*` | `parse` | Lane B |
| `lower` subsystem | `native/objc3c/src/lower/*` | `sema` | Lane A |
| `ir` subsystem | `native/objc3c/src/ir/*` | `lower` | Lane A |
| `pipeline` subsystem | `native/objc3c/src/pipeline/*` | `lex`, `parse`, `sema`, `lower`, `ir` | Lane B |
| `libobjc3c_frontend` module | `native/objc3c/src/libobjc3c_frontend/*` | `pipeline` | Lane B |
| `driver` subsystem | `native/objc3c/src/driver/*` | `libobjc3c_frontend`, `io` | Lane A |
| `io` subsystem | `native/objc3c/src/io/*` | none | Lane A |

Boundary rules are fail-closed through:

```sh
python scripts/check_objc3c_dependency_boundaries.py --strict
```

## 3. lane-oriented workflow

Use this workflow for every issue that touches a module or subsystem.

1. Confirm issue scope and owning lane before coding.
2. Pick one primary subsystem (`lex`, `parse`, `sema`, `lower`, `ir`, or integration subsystem).
3. Run subsystem-focused checks first (see section 5).
4. Run boundary gate: `npm run check:objc3c:boundaries`.
5. Implement minimal slice scoped to one issue and one subsystem objective.
6. Re-run subsystem checks and boundary gate after edits.
7. Include ownership handoff notes in PR when touching another lane-owned module.
8. Merge only when workflow evidence (commands + exit codes) is present.

## 4. ownership model

Ownership is module-centric with explicit escalation.

| ownership area | primary | backup | escalation trigger |
| --- | --- | --- | --- |
| Stage modules (`lex`, `parse`, `lower`, `ir`) | Lane A owner | Integrator | unresolved defect > 1 business day |
| Semantic and orchestration (`sema`, `pipeline`, `libobjc3c_frontend`) | Lane B owner | Integrator | cross-module regressions or blocked merges |
| Workflow/governance/docs and CI wiring | Lane E owner | Release owner | missing workflow evidence or stale onboarding docs |
| Test and determinism signal checks | Lane D owner | Integrator | flaky or non-deterministic subsystem signal |

Cross-lane rule:

- If a change crosses ownership boundaries, both the owning lane and integration owner must be referenced in PR notes.

## 5. test selection by subsystem

Start with the smallest deterministic command set that covers the changed subsystem.

| subsystem focus | default command | additional command(s) when needed |
| --- | --- | --- |
| `lex` | `npm run dev:objc3c:lex` | `npm run check:objc3c:boundaries` |
| `parse` | `npm run dev:objc3c:parse` | `npm run check:objc3c:boundaries` |
| `sema` | `npm run dev:objc3c:sema` | `npm run test:objc3c:execution-replay-proof` |
| `lower` | `npm run dev:objc3c:lower` | `npm run test:objc3c:typed-abi-replay-proof` |
| `ir` | `npm run dev:objc3c:ir` | `npm run test:objc3c:execution-smoke` |
| `pipeline`/`libobjc3c_frontend` | `npm run dev:objc3c:sema` | `npm run dev:objc3c:lower`, `npm run dev:objc3c:ir` |
| `driver`/`io` | `npm run check:objc3c:boundaries` | `npm run test:objc3c:execution-smoke` |

Selection policy:

- Do not start with `test:objc3c:full` unless multiple subsystems changed.
- If two or more subsystems are touched, run each relevant subsystem workflow command plus the boundary gate.

## 6. troubleshooting workflow

### 6.1 boundary checker failure

Symptom:

- `forbidden include '<path>' (<from> -> <to>)`

Actions:

1. Reproduce with `npm run check:objc3c:boundaries`.
2. Identify violating module direction.
3. Refactor include path or move shared contract to an allowed subsystem.
4. Re-run boundary gate and subsystem tests.
5. Follow `docs/refactor/objc3c_dependency_violation_playbook.md` if unresolved.

### 6.2 parser/lex workflow failures

Symptom:

- `test:objc3c:parser-replay-proof` fixture mismatch or replay drift.

Actions:

1. Isolate failing fixture from summary output in `tmp/artifacts/objc3c-native/parser-replay-proof/*`.
2. Confirm whether failure is lex tokenization or parse structure.
3. Fix only the targeted subsystem path first.
4. Re-run `dev:objc3c:lex` or `dev:objc3c:parse`.

### 6.3 sema workflow failures

Symptom:

- diagnostics replay mismatch or unexpected diagnostic code set.

Actions:

1. Re-run `npm run dev:objc3c:sema`.
2. Compare diagnostics replay outputs under `tmp/artifacts/objc3c-native/diagnostics-replay-proof/*`.
3. Verify deterministic code lists and expected fixture metadata.

### 6.4 lower/ir workflow failures

Symptom:

- IR drift, expectation token mismatch, or typed ABI replay mismatch.

Actions:

1. Re-run the relevant subsystem command (`dev:objc3c:lower` or `dev:objc3c:ir`).
2. Inspect generated summaries under `tmp/artifacts/objc3c-native/lowering-regression/*` or `tmp/artifacts/objc3c-native/typed-abi-replay-proof/*`.
3. Check that lowered output remains deterministic across replay runs.

## 7. PR checklist for modular subsystem work

- [ ] The changed module/subsystem is named explicitly in PR scope.
- [ ] The correct subsystem workflow command(s) were run.
- [ ] Ownership handoff notes are present for cross-lane edits.
- [ ] Boundary checker result and exit code are included.
- [ ] Troubleshooting notes are included when failures required remediation.

## 8. Validation command

```sh
rg -n "module|subsystem|workflow|ownership" docs/refactor/objc3c_modular_developer_guide.md
```
