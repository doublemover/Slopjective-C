# objc3c Maintainer Workflows

## Generated Documentation Surface

Use the generators directly for documentation surfaces that are supposed to be
checked into the repo:

- public site:
  - source: `site/src/index.body.md`
  - build/check: `npm run build:site` / `npm run check:site`
- native implementation doc:
  - source: `docs/objc3c-native/src/*.md`
  - build/check: `npm run build:docs:native` / `npm run check:docs:native`
- machine-facing operator appendix:
  - source: `package.json` + `scripts/objc3c_public_workflow_runner.py`
  - build/check: `npm run build:docs:commands` / `npm run check:docs:commands`

Do not hand-edit generated outputs. Do not treat `tmp/reports/` or
`tmp/artifacts/` as canonical documentation.

## Superclean Working Boundary

Use these roots directly when cleaning or renaming repo surfaces:

- implementation roots:
  - `native/objc3c/`
  - `scripts/`
  - `tests/`
- checked-in doc sources:
  - `README.md`
  - `CONTRIBUTING.md`
  - `docs/tutorials/`
  - `showcase/`
  - `site/src/`
  - `docs/objc3c-native/src/`
  - `package.json`
- generated checked-in outputs:
  - `site/index.md`
  - `docs/objc3c-native.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
- machine-owned outputs only:
  - `tmp/`
  - `artifacts/`
  - `tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json` is the build-emitted source-of-truth artifact for the repo superclean surface

Do not add milestone-specific wrappers, sidecar compatibility files, or
parallel source-of-truth copies when changing these surfaces.

Contributor-facing entrypoint:

- `CONTRIBUTING.md` is the contributor instruction surface for normal repo
  changes
- `docs/tutorials/README.md` is the learning-path and migration-guide root
- `showcase/README.md` is the runnable example map and live showcase boundary
- `README.md` stays focused on onboarding, setup, and repo navigation
- this runbook is maintainer-only and should not accumulate contributor
  guidance that belongs in `CONTRIBUTING.md`

Developer-tooling entrypoint:

- `docs/runbooks/objc3c_developer_tooling.md` is the maintainer boundary for
  live inspection, debug, and explainability work
- developer ergonomics changes must stay on the existing native tooling,
  runtime ABI, and public workflow runner surfaces named there
- use the direct commands in that runbook when you need compile summaries,
  runtime debug-state inspection, or parity validation without inventing a
  sidecar workflow

Bonus-experience entrypoint:

- `docs/runbooks/objc3c_bonus_experiences.md` is the maintainer boundary for
  playground, capability-explorer, repro-runner, and template-generator work
- bonus experiences must layer on the existing public runner, showcase,
  tutorial, and developer-tooling surfaces instead of inventing a separate app
  shell
- use that runbook before widening example, packaging, or inspection flows

Performance-benchmark entrypoint:

- `docs/runbooks/objc3c_performance.md` is the maintainer boundary for
  benchmark taxonomy, comparative baseline workloads, telemetry packets, and
  normalization policy
- performance work must stay on the existing public runner, native compiler,
  showcase, and runnable package surfaces instead of inventing a sidecar
  measurement app or spreadsheet workflow
- use that runbook before widening benchmark claims, baseline corpus inputs, or
  packaged validation flows

Standard-library entrypoint:

- `docs/runbooks/objc3c_stdlib_foundation.md` is the maintainer boundary for
  the checked-in stdlib root, canonical module inventory, alias mapping, and
  machine-owned stdlib workspace materialization flow
- `docs/runbooks/objc3c_stdlib_core.md` is the maintainer boundary for the
  `M306` core utility, text/data, collection, option, and result family split
- `stdlib/semantic_policy.json` is the checked-in compatibility contract for
  stable helper meaning and module semver across the core stdlib surface
- stdlib work must stay on `stdlib/`, `tmp/artifacts/stdlib/`, and
  `tmp/reports/stdlib/` instead of inventing a second library tree or sidecar
  package layout
- use the public runner actions for stdlib surface checking and workspace
  materialization before widening package or integration flows

## Build

```powershell
npm run build:objc3c-native
npm run build:objc3c-native:contracts
```

## Test

```powershell
npm run test:smoke
npm run test:ci
npm run test:docs
npm run test:repo
```

`npm run test:ci` now includes the compact documentation integration surface:

- generated site drift,
- generated native-doc drift,
- generated public-command-surface drift,
- and reader-facing documentation/readability boundary checks.

`npm run test:docs` runs the full documentation build/check pass:

- rebuild the published site output,
- rebuild the generated native implementation docs,
- rebuild the generated public command appendix,
- and then re-check all three outputs plus the reader-facing documentation surface.

## Direct tools

- dependency boundaries: `python scripts/check_objc3c_dependency_boundaries.py --strict`
- task hygiene: `python scripts/ci/check_task_hygiene.py`
- repo superclean surface: `npm run check:repo:surface`
- repo superclean integration: `npm run test:repo`
- docs stitch/check: `npm run check:docs:native`
- parity source check: `python scripts/check_objc3c_library_cli_parity.py ...`
- developer tooling boundary: `docs/runbooks/objc3c_developer_tooling.md`
- bonus experiences boundary: `docs/runbooks/objc3c_bonus_experiences.md`
- performance benchmark boundary: `docs/runbooks/objc3c_performance.md`
- stdlib foundation boundary: `docs/runbooks/objc3c_stdlib_foundation.md`
- stdlib core boundary: `docs/runbooks/objc3c_stdlib_core.md`
- stdlib surface check: `npm run check:stdlib:surface`
- stdlib workspace materialization: `npm run build:objc3c:stdlib`
- stdlib integration: `npm run test:stdlib`
- advanced stdlib integration: `npm run test:stdlib:advanced`

The live maintainer surface is intentionally small. Historical planning, contract, and milestone-specific validation material is archived under `tmp/archive/`.
