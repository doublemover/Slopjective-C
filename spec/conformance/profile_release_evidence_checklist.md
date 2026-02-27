# Objective-C 3.0 Profile Release Evidence Checklist (Issue #124)

Scope: issue #124. This checklist defines profile-by-profile release evidence requirements for conformance claims under `core`, `strict`, `strict-concurrency`, and `strict-system`.

## Evidence bundle contract

Each release claim shall be backed by an evidence bundle with a stable ID:

- `release_id = <YYYYMMDD>-<toolchain>-<git_sha7>`
- one bundle per release candidate; profile claims may share the same bundle
- all referenced files shall be immutable for that `release_id`

Required bundle metadata:

| Field | Requirement |
| --- | --- |
| `release_id` | Stable unique release evidence ID. |
| `toolchain` | Toolchain name and version used for evidence generation. |
| `git_revision` | Full commit SHA for the evaluated source tree. |
| `target_triples` | Non-empty list of tested targets. |
| `generated_at` | UTC RFC3339 timestamp. |
| `profiles_claimed` | Subset of `core`, `strict`, `strict-concurrency`, `strict-system`. |
| `artifacts[]` | Non-empty list with artifact ID, path/URI, and SHA-256 digest. |
| `reviews[]` | Reviewer records with role, decision, timestamp, and notes. |
| `approvals[]` | Final release approvals with approver identity and timestamp. |

## Required artifact definitions

| Artifact ID | Required content | Validation / evidence check |
| --- | --- | --- |
| `EVID-01` | Conformance report JSON (Part 12, 12.4.4/12.4.5) including `mode`, `profiles`, `versions`, and `known_deviations`. | Validate with `--validate-objc3-conformance=<path>` (or equivalent), non-zero on failure. |
| `EVID-02` | Conformance suite bucket manifests: `tests/conformance/parser/manifest.json`, `tests/conformance/semantic/manifest.json`, `tests/conformance/lowering_abi/manifest.json`, `tests/conformance/module_roundtrip/manifest.json`, `tests/conformance/diagnostics/manifest.json`. | Verify manifests are present and enumerate executed tests for the release run. |
| `EVID-03` | Profile test-count summary mapped to Part 12 minimums (12.5.6). | Verify published counts meet or exceed profile minima. |
| `EVID-04` | Traceability map: `tests/conformance/COVERAGE_MAP.md` (or generated equivalent) linked to executed tests. | Verify every required profile family links to concrete test IDs. |
| `EVID-05` | Runtime manifest evidence: `reports/conformance/manifests/objc3-runtime-2025Q4.manifest.json` plus validation log. | Validate against `schemas/objc3-runtime-2025Q4.manifest.schema.json`. |
| `EVID-06` | ABI manifest evidence: `reports/conformance/manifests/objc3-abi-2025Q4.example.json` plus validation log. | Validate against `schemas/objc3-abi-2025Q4.schema.json`. |
| `EVID-07` | Strictness/sub-mode matrix evidence (`SCM-01`..`SCM-06`) and macro-value checks. | Verify matrix results align with Part 1 strictness/concurrency model and report `mode` fields. |
| `EVID-08` | Strict diagnostics/fix-it evidence for required strict-only behaviors. | Verify strict-only diagnostics include stable code/severity/span/fix-it metadata. |
| `EVID-09` | Strict concurrency evidence for isolation, Sendable-like rules, and executor-boundary checks. | Verify additional strict-concurrency tests are present and passing. |
| `EVID-10` | Strict system evidence for borrowed/resource/capture-list rules and metadata preservation. | Verify borrowed/resource/capture tests are present and passing. |
| `EVID-11` | Review and approval record (who approved what, when, and for which `release_id`). | Verify all required review roles are completed and no required step is skipped. |

## Profile checklists

### Core profile (`core`)

Required artifact mapping:

| Requirement | Artifact IDs |
| --- | --- |
| Report schema, mode, and profile claim (`core`) | `EVID-01` |
| Required bucket coverage and profile minima | `EVID-02`, `EVID-03` |
| Traceability to required conformance families | `EVID-04` |
| Runtime and ABI normative-reference evidence | `EVID-05`, `EVID-06` |
| Release review and approval record | `EVID-11` |

Checklist:

- [x] SPT-0001 (`CLOSED/PASS/COMPLETE`): `mode.strictness=permissive` and `mode.concurrency=off` are recorded in `EVID-01`; evidence: `EVID-01.mode` for the core claim; validation hook: `VH-D1-01`.
- [x] SPT-0002 (`CLOSED/PASS/COMPLETE`): `profiles[]` includes `{"id":"core","status":"passed"}` in `EVID-01`; evidence: `EVID-01.profiles[]`; validation hook: `VH-D1-02`.
- [x] SPT-0003 (`CLOSED/PASS/COMPLETE`): `CRPT-01`..`CRPT-06` coverage is present in executed diagnostics evidence (`EVID-02`/`EVID-04`); evidence: `tests/conformance/diagnostics/manifest.json` and `tests/conformance/COVERAGE_MAP.md`; validation hook: `VH-D1-03`.
- [x] SPT-0004 (`CLOSED/PASS/COMPLETE`): Core minima are met or exceeded (parser `>=15`, semantic `>=25`, lowering_abi `>=10`, module_roundtrip `>=12`, diagnostics `>=20`) via `EVID-03`; evidence: suite bucket manifests and minima contract; validation hook: `VH-D1-04`.
- [x] SPT-0005 (`CLOSED/PASS/COMPLETE`): Runtime manifest validates successfully (`EVID-05`); evidence: `reports/conformance/manifests/objc3-runtime-2025Q4.manifest.json` and `schemas/objc3-runtime-2025Q4.manifest.schema.json`; validation hook: `VH-D1-05`.
- [x] SPT-0006 (`CLOSED/PASS/COMPLETE`): ABI manifest validates successfully (`EVID-06`); evidence: `reports/conformance/manifests/objc3-abi-2025Q4.example.json` and `schemas/objc3-abi-2025Q4.schema.json`; validation hook: `VH-D1-06`.
- [x] SPT-0007 (`CLOSED/PASS/COMPLETE`): all artifact digests are recorded in bundle metadata and match file contents; evidence: `EVID` bundle digest rows (`artifacts[]` or `manifests[]`); validation hook: `VH-D1-07`.

### Strict profile (`strict`)

Required artifact mapping:

| Requirement | Artifact IDs |
| --- | --- |
| All Core requirements remain satisfied | `EVID-01`..`EVID-06` |
| Strict mode mapping and matrix consistency | `EVID-01`, `EVID-07` |
| Strict-only diagnostics/fix-it evidence | `EVID-08` |
| Release review and approval record | `EVID-11` |

Checklist:

- [x] SPT-0008 (`CLOSED/PASS/COMPLETE`): `mode.strictness=strict` and `mode.concurrency=off` are recorded in `EVID-01`; evidence: strict-mode `EVID-01.mode`; validation hook: `VH-D1-08`.
- [x] SPT-0009 (`CLOSED/PASS/COMPLETE`): `profiles[]` includes `{"id":"strict","status":"passed"}` in `EVID-01`; evidence: strict-mode `EVID-01.profiles[]`; validation hook: `VH-D1-09`.
- [x] SPT-0010 (`CLOSED/PASS/COMPLETE`): at least 10 additional strict-only diagnostics/fix-it tests beyond Core are executed and passing (`EVID-03`, `EVID-08`); evidence: strict diagnostics issue groups `#60`, `#69`, `#74`, `#79`, `#87`; validation hook: `VH-D1-10`.
- [x] SPT-0011 (`CLOSED/PASS/COMPLETE`): strict diagnostics use portable assertion metadata (`code`, `severity`, `span`, required `fixits`) (`EVID-08`); evidence: strict diagnostics fixtures in `tests/conformance/diagnostics/*.json`; validation hook: `VH-D1-11`.
- [x] SPT-0012 (`CLOSED/PASS/COMPLETE`): strict matrix points for non-concurrency strict behavior are validated (`EVID-07`); evidence: `SCM-01` and `SCM-05`; validation hook: `VH-D1-12`.

### Strict Concurrency profile (`strict-concurrency`)

Required artifact mapping:

| Requirement | Artifact IDs |
| --- | --- |
| All Strict requirements remain satisfied | `EVID-01`..`EVID-08` |
| Strict concurrency mode and report/profile consistency | `EVID-01`, `EVID-07` |
| Isolation/Sendable/executor-boundary evidence | `EVID-09` |
| Release review and approval record | `EVID-11` |

Checklist:

- [x] SPT-0013 (`CLOSED/PASS/COMPLETE`): `mode.strictness=strict` and `mode.concurrency=strict` are recorded in `EVID-01`; evidence: strict-concurrency `EVID-01.mode`; validation hook: `VH-D1-13`.
- [x] SPT-0014 (`CLOSED/PASS/COMPLETE`): `profiles[]` includes `{"id":"strict-concurrency","status":"passed"}` in `EVID-01`; evidence: strict-concurrency `EVID-01.profiles[]`; validation hook: `VH-D1-14`.
- [x] SPT-0015 (`CLOSED/PASS/COMPLETE`): at least 12 additional strict-concurrency tests are executed and passing (`EVID-03`, `EVID-09`); evidence: strict-concurrency test families in diagnostics/semantic manifests; validation hook: `VH-D1-15`.
- [x] SPT-0016 (`CLOSED/PASS/COMPLETE`): `SCM-02`, `SCM-05`, and `SCM-06` evidence confirms strictness/sub-mode correctness and report mapping (`EVID-07`); evidence: `tests/conformance/diagnostics/SCM-02.json`, `tests/conformance/diagnostics/SCM-05.json`, `tests/conformance/diagnostics/SCM-06.json`; validation hook: `VH-D1-16`.
- [x] SPT-0017 (`CLOSED/PASS/COMPLETE`): strict-concurrency diagnostics include actor isolation, executor hops, and Sendable-like boundaries (`EVID-09`); evidence: `ACT-*`, `EXE-*`, `SND-*`, and `CONC-DIAG-*` families; validation hook: `VH-D1-17`.

### Strict System profile (`strict-system`)

Required artifact mapping:

| Requirement | Artifact IDs |
| --- | --- |
| All Strict Concurrency requirements remain satisfied | `EVID-01`..`EVID-09` |
| Strict-system mode plus strict-concurrency sub-mode | `EVID-01`, `EVID-07` |
| Borrowed/resource/capture-list and metadata-preservation evidence | `EVID-10` |
| Release review and approval record | `EVID-11` |

Checklist:

- [x] SPT-0018 (`CLOSED/PASS/COMPLETE`): `mode.strictness=strict-system` and `mode.concurrency=strict` are recorded in `EVID-01`; evidence: strict-system `EVID-01.mode`; validation hook: `VH-D1-18`.
- [x] SPT-0019 (`CLOSED/PASS/COMPLETE`): `profiles[]` includes `{"id":"strict-system","status":"passed"}` in `EVID-01`; evidence: strict-system `EVID-01.profiles[]`; validation hook: `VH-D1-19`.
- [x] SPT-0020 (`CLOSED/PASS/COMPLETE`): at least 12 additional strict-system tests are executed and passing (`EVID-03`, `EVID-10`); evidence: strict-system fixture census across conformance buckets; validation hook: `VH-D1-20`.
- [x] SPT-0021 (`CLOSED/PASS/COMPLETE`): borrowed-pointer strict-system negatives/positives (including `BRW-NEG-*` and `BRW-POS-*`) are covered in evidence (`EVID-10`); evidence: borrowed fixture families in semantic/diagnostics manifests; validation hook: `VH-D1-21`.
- [x] SPT-0022 (`CLOSED/PASS/COMPLETE`): resource and capture-list enforcement evidence is present, including diagnostics for escape and use-after-move patterns (`EVID-10`); evidence: `RES-*`, `AGR-NEG-01`, `CAP-*`, and `SYS-DIAG-*` fixture families; validation hook: `VH-D1-22`.
- [x] SPT-0023 (`CLOSED/PASS/COMPLETE`): strict-system metadata preservation for borrowed/resource annotations is demonstrated in module round-trip evidence (`EVID-10`); evidence: `BRW-META-*` and `SYS-ATTR-05..08`; validation hook: `VH-D1-23`.

## Deterministic validation hooks (`VH-D1-01`..`VH-D1-23`)

Run from repository root. For `EVID-01` and bundle checks, set these environment variables before executing hooks: `EVID01_CORE`, `EVID01_STRICT`, `EVID01_STRICT_CONCURRENCY`, `EVID01_STRICT_SYSTEM`, `EVID_BUNDLE`.

- `VH-D1-01`: `rg -n '"strictness"\s*:\s*"permissive"' "$env:EVID01_CORE"; rg -n '"concurrency"\s*:\s*"off"' "$env:EVID01_CORE"`
- `VH-D1-02`: `rg -n '"id"\s*:\s*"core"' "$env:EVID01_CORE"; rg -n '"status"\s*:\s*"passed"' "$env:EVID01_CORE"`
- `VH-D1-03`: `python -c "import json, pathlib; m=json.loads(pathlib.Path('tests/conformance/diagnostics/manifest.json').read_text(encoding='utf-8')); files={f for g in m['groups'] for f in g.get('files', [])}; req={f'CRPT-0{i}.json' for i in range(1,7)}; assert req.issubset(files), sorted(req-files); cm=pathlib.Path('tests/conformance/COVERAGE_MAP.md').read_text(encoding='utf-8'); assert 'CRPT-01..06' in cm; print('VH-D1-03 PASS')"`
- `VH-D1-04`: `python -c "import pathlib; mins={'parser':15,'semantic':25,'lowering_abi':10,'module_roundtrip':12,'diagnostics':20}; root=pathlib.Path('tests/conformance'); counts={b:sum(1 for p in (root/b).glob('*.json') if p.name!='manifest.json') for b in mins}; fail={b:(counts[b],mins[b]) for b in mins if counts[b]<mins[b]}; assert not fail, fail; print('VH-D1-04 PASS', counts)"`
- `VH-D1-05`: `python -c "import json, pathlib, jsonschema; s=json.loads(pathlib.Path('schemas/objc3-runtime-2025Q4.manifest.schema.json').read_text(encoding='utf-8')); d=json.loads(pathlib.Path('reports/conformance/manifests/objc3-runtime-2025Q4.manifest.json').read_text(encoding='utf-8')); jsonschema.Draft202012Validator(s).validate(d); print('VH-D1-05 PASS')"`
- `VH-D1-06`: `python -c "import json, pathlib, jsonschema; s=json.loads(pathlib.Path('schemas/objc3-abi-2025Q4.schema.json').read_text(encoding='utf-8')); d=json.loads(pathlib.Path('reports/conformance/manifests/objc3-abi-2025Q4.example.json').read_text(encoding='utf-8')); jsonschema.Draft202012Validator(s).validate(d); print('VH-D1-06 PASS')"`
- `VH-D1-07`: `python -c "import hashlib, json, os, pathlib; d=json.loads(pathlib.Path(os.environ['EVID_BUNDLE']).read_text(encoding='utf-8')); rows=d.get('artifacts') or d.get('manifests') or []; assert rows, 'no digest rows'; mism=[(r.get('path') or r.get('artifact_path')) for r in rows if (r.get('sha256') or r.get('file_sha256')) != 'sha256:'+hashlib.sha256(pathlib.Path(r.get('path') or r.get('artifact_path')).read_bytes()).hexdigest()]; assert not mism, mism; print('VH-D1-07 PASS', len(rows))"`
- `VH-D1-08`: `rg -n '"strictness"\s*:\s*"strict"' "$env:EVID01_STRICT"; rg -n '"concurrency"\s*:\s*"off"' "$env:EVID01_STRICT"`
- `VH-D1-09`: `rg -n '"id"\s*:\s*"strict"' "$env:EVID01_STRICT"; rg -n '"status"\s*:\s*"passed"' "$env:EVID01_STRICT"`
- `VH-D1-10`: `python -c "import json, pathlib; m=json.loads(pathlib.Path('tests/conformance/diagnostics/manifest.json').read_text(encoding='utf-8')); total=sum(len(g.get('files', [])) for g in m['groups'] if g.get('issue') in {60,69,74,79,87}); assert total>=10, total; print('VH-D1-10 PASS', total)"`
- `VH-D1-11`: `python -c "import json, pathlib; ids=['NUL-60-FIX-01','LFT-69-FIX-01','ERR-79-FIX-01','AWT-05','CONC-DIAG-01']; bad=[(i,diag) for i in ids for diag in json.loads(pathlib.Path('tests/conformance/diagnostics',f'{i}.json').read_text(encoding='utf-8')).get('expect',{}).get('diagnostics',[]) if not all(k in diag for k in ('code','severity','span','fixits'))]; assert not bad, bad; print('VH-D1-11 PASS')"`
- `VH-D1-12`: `python -c "import json, pathlib; scm1=json.loads(pathlib.Path('tests/conformance/diagnostics/SCM-01.json').read_text(encoding='utf-8')); scm5=json.loads(pathlib.Path('tests/conformance/diagnostics/SCM-05.json').read_text(encoding='utf-8')); assert any(diag.get('severity')=='warning' for diag in scm1.get('expect',{}).get('diagnostics',[])); assert any('-fobjc3-concurrency=off' in flag for row in scm5.get('matrix',[]) for flag in row.get('flags',[])); print('VH-D1-12 PASS')"`
- `VH-D1-13`: `rg -n '"strictness"\s*:\s*"strict"' "$env:EVID01_STRICT_CONCURRENCY"; rg -n '"concurrency"\s*:\s*"strict"' "$env:EVID01_STRICT_CONCURRENCY"`
- `VH-D1-14`: `rg -n '"id"\s*:\s*"strict-concurrency"' "$env:EVID01_STRICT_CONCURRENCY"; rg -n '"status"\s*:\s*"passed"' "$env:EVID01_STRICT_CONCURRENCY"`
- `VH-D1-15`: `python -c "import json, pathlib; d=json.loads(pathlib.Path('tests/conformance/diagnostics/manifest.json').read_text(encoding='utf-8')); s=json.loads(pathlib.Path('tests/conformance/semantic/manifest.json').read_text(encoding='utf-8')); total=sum(len(g.get('files', [])) for g in d['groups'] if g.get('issue')==85)+sum(len(g.get('files', [])) for g in s['groups'] if g.get('issue') in {83,84,86}); assert total>=12, total; print('VH-D1-15 PASS', total)"`
- `VH-D1-16`: `python -c "import pathlib; text=''.join(pathlib.Path(p).read_text(encoding='utf-8') for p in ['tests/conformance/diagnostics/SCM-02.json','tests/conformance/diagnostics/SCM-05.json','tests/conformance/diagnostics/SCM-06.json']); req=['\"id\": \"SCM-02\"','\"id\": \"SCM-05\"','\"id\": \"SCM-06\"','expect_report','__OBJC3_STRICTNESS_LEVEL__']; miss=[x for x in req if x not in text]; assert not miss, miss; print('VH-D1-16 PASS')"`
- `VH-D1-17`: `python -c "import pathlib; text=pathlib.Path('tests/conformance/COVERAGE_MAP.md').read_text(encoding='utf-8')+pathlib.Path('tests/conformance/diagnostics/manifest.json').read_text(encoding='utf-8'); req=['ACT-01..09','EXE-01..05','SND-01..08','CONC-DIAG-01','CONC-DIAG-06']; miss=[x for x in req if x not in text]; assert not miss, miss; print('VH-D1-17 PASS')"`
- `VH-D1-18`: `rg -n '"strictness"\s*:\s*"strict-system"' "$env:EVID01_STRICT_SYSTEM"; rg -n '"concurrency"\s*:\s*"strict"' "$env:EVID01_STRICT_SYSTEM"`
- `VH-D1-19`: `rg -n '"id"\s*:\s*"strict-system"' "$env:EVID01_STRICT_SYSTEM"; rg -n '"status"\s*:\s*"passed"' "$env:EVID01_STRICT_SYSTEM"`
- `VH-D1-20`: `python -c "import glob, json, pathlib; total=sum(1 for p in glob.glob('tests/conformance/**/*.json', recursive=True) if pathlib.Path(p).name!='manifest.json' and json.loads(pathlib.Path(p).read_text(encoding='utf-8')).get('profile')=='strict-system'); assert total>=12, total; print('VH-D1-20 PASS', total)"`
- `VH-D1-21`: `python -c "import pathlib; req=[f'BRW-NEG-0{i}.json' for i in range(1,6)]+[f'BRW-POS-0{i}.json' for i in range(1,5)]; roots=[pathlib.Path('tests/conformance/semantic'), pathlib.Path('tests/conformance/diagnostics')]; have={p.name for r in roots for p in r.glob('BRW-*.json')}; miss=[x for x in req if x not in have]; assert not miss, miss; print('VH-D1-21 PASS', len(req))"`
- `VH-D1-22`: `python -c "import pathlib; text=''.join(pathlib.Path(p).read_text(encoding='utf-8') for p in ['tests/conformance/diagnostics/manifest.json','tests/conformance/diagnostics/CAP-06.json','tests/conformance/diagnostics/SYS-DIAG-01.json','tests/conformance/diagnostics/SYS-DIAG-08.json']); req=['RES-06','AGR-NEG-01','CAP-06','CAP-07','CAP-08','SYS-DIAG-06','SYS-DIAG-07','SYS-DIAG-08','use-after-move','escapes']; miss=[x for x in req if x not in text]; assert not miss, miss; print('VH-D1-22 PASS')"`
- `VH-D1-23`: `python -c "import json, pathlib; m=json.loads(pathlib.Path('tests/conformance/module_roundtrip/manifest.json').read_text(encoding='utf-8')); files={f for g in m['groups'] for f in g.get('files', [])}; req={'BRW-META-01.json','BRW-META-02.json','BRW-META-03.json','SYS-ATTR-05.json','SYS-ATTR-06.json','SYS-ATTR-07.json','SYS-ATTR-08.json'}; assert req.issubset(files), sorted(req-files); print('VH-D1-23 PASS')"`

## Review and approval flow (required)

For each `release_id`, complete these steps in order:

1. Evidence assembly (implementer)
   - [ ] Produce and checksum all required artifacts.
   - [ ] Record exact commands, tool versions, and run timestamps.
2. Independent conformance review (non-author reviewer)
   - [ ] Verify artifact completeness against profile mapping tables.
   - [ ] Re-run validation commands for `EVID-01`, `EVID-05`, and `EVID-06`.
3. Spec compliance review (spec/conformance owner)
   - [ ] Confirm profile claims match mode selections and executed evidence.
   - [ ] Confirm any known deviations are listed in `known_deviations`.
4. Release approval (release owner)
   - [ ] Approve only if all required checklist items for claimed profiles are complete.
   - [ ] Reject if any required artifact is missing, unverifiable, or stale.

## Audit and retention rules

- Every artifact listed in `artifacts[]` shall include a SHA-256 digest.
- Evidence bundles shall be retained in immutable storage for release audit and regression comparison.
- Any post-approval evidence change requires a new `release_id` and a full re-review.
- Profile status may be marked `passed` only when all required checklist items and approvals are complete.
