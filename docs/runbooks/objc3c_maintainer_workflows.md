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
  - source: `package.json` + `scripts/objc3c_public_workflow_runner.py` + `scripts/build_objc3c_public_command_contract.py`
  - build/check: `npm run build:docs:commands` / `npm run check:docs:commands`
  - maintainer contract checks:
    - `python scripts/objc3c_public_workflow_runner.py build-public-command-contract`
    - `python scripts/objc3c_public_workflow_runner.py check-public-command-contract`
    - `python scripts/objc3c_public_workflow_runner.py check-public-command-budget`

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

Performance-governance entrypoint:

- `docs/runbooks/objc3c_performance_governance.md` is the maintainer boundary
  for budget contracts, regression classification, dashboard derivation,
  waivers, and publishable performance evidence
- performance governance must stay on the existing performance benchmark,
  compiler-throughput, runtime-performance, public runner, and checked-in
  schema surfaces instead of inventing a second dashboard spreadsheet or
  sidecar release report
- use the public runner actions for source-surface checking, schema checking,
  dashboard derivation, and publication before widening release-performance
  claims or waiver semantics

Governance-sustainability entrypoint:

- `docs/runbooks/objc3c_governance_sustainability.md` is the maintainer
  boundary for repo-shape budgets, anti-noise ratchets, waiver ownership,
  maintainer review expectations, and long-horizon regression evidence
- governance sustainability must stay on the existing task-hygiene,
  repo-superclean, documentation-surface, dependency-boundary, and public
  workflow-runner surfaces instead of inventing sidecar milestone wrappers,
  duplicate planning roots, or spreadsheet-only waiver tracking
- use the replayable governance summaries before widening package scripts,
  runbooks, schemas, checker surfaces, or publication workflows:
  - `python scripts/build_governance_budget_inventory_summary.py`
  - `python scripts/build_governance_policy_summary.py`
  - `python scripts/build_governance_maintainer_review_summary.py`

Release-foundation entrypoint:

- `docs/runbooks/objc3c_release_foundation.md` is the maintainer boundary for
  release artifact taxonomy, runnable payload selection, reproducible package
  assembly, SBOM publication, and attestation binding
- release-foundation work must stay on the existing runnable package,
  release-evidence, repo-superclean, and public workflow runner surfaces
  instead of inventing a second package layout, hand-maintained checksum
  spreadsheet, or installer-shaped sidecar bundle
- use the public runner actions for source-surface checking, schema checking,
  release-manifest derivation, and provenance publication before widening
  release claims

Packaging-channels entrypoint:

- `docs/runbooks/objc3c_packaging_channels.md` is the maintainer boundary for
  portable archives, local installer images, offline bootstrap bundles, and
  install or rollback smoke
- packaging-channel work must stay on the existing runnable package,
  release-foundation, and public workflow runner surfaces instead of inventing
  a second installer payload or manual archive assembly flow
- use the public runner actions for packaging-channel source checks, schema
  checks, package generation, and install smoke before widening distribution
  claims

Release-operations entrypoint:

- `docs/runbooks/objc3c_release_operations.md` is the maintainer boundary for
  semantic versioning claims, support windows, update-manifest publication,
  compatibility warnings, rollback guidance, and release-operations metadata
- release-operations work must stay on the existing release-foundation,
  packaging-channel, and public workflow runner surfaces instead of inventing a
  hosted updater, second payload lineage, or package-manager-only upgrade flow
- use the public runner actions for release-operations source checking, schema
  checking, update-manifest derivation, publication, and end-to-end validation
  before widening compatibility or deprecation claims

Runtime-performance entrypoint:

- `docs/runbooks/objc3c_runtime_performance.md` is the maintainer boundary for
  startup/dispatch/reflection/ownership hot-path measurement, runtime counter
  snapshots, and runnable runtime-performance validation
- runtime-performance work must stay on the existing runtime library, runtime
  acceptance probes, public workflow runner, and runnable package surfaces
  instead of inventing a benchmark-only runtime shim or sidecar report flow
- use that runbook before widening runtime hot-path claims, counter fields, or
  packaged runtime-performance validation

Compiler-throughput entrypoint:

- `docs/runbooks/objc3c_compiler_throughput.md` is the maintainer boundary for
  direct native compile throughput, wrapper cache proof, incremental
  invalidation, macro-host cache publication, docs-generation cost, and
  heavyweight validation-tier ownership
- compiler-throughput work must stay on the existing native compiler
  executable, compile wrapper, public workflow runner, native docs generators,
  and runnable package surfaces instead of inventing a second benchmark harness
  or spreadsheet-only workflow
- use the public runner actions for compiler-throughput benchmarking and
  integrated validation before widening timing claims, cache semantics, or
  packaged throughput validation

Stress-validation entrypoint:

- `docs/runbooks/objc3c_stress_validation.md` is the maintainer boundary for
  malformed-input fuzzing, lowering/runtime stress sweeps, mixed-module
  differentials, and reducer/triage artifact flows
- stress work must stay on the existing compile wrapper, execution-smoke,
  conformance corpus, runtime acceptance, and runnable package surfaces instead
  of inventing a separate fuzz service or sidecar corpus root
- use that runbook before widening fixture families, reducer outputs, workflow
  commands, or packaged stress validation claims

External-validation entrypoint:

- `docs/runbooks/objc3c_external_validation.md` is the maintainer boundary for
  external fixture intake, normalized replay contracts, quarantine, and
  ecosystem-facing credibility work
- external validation must stay on the existing conformance corpus, replay
  proof, release evidence, and runnable package surfaces instead of inventing a
  second corpus, sidecar harness, or manual-only replay workflow
- use that runbook before widening intake roots, replay publication, or
  ecosystem evidence claims

Public-conformance-reporting entrypoint:

- `docs/runbooks/objc3c_public_conformance_reporting.md` is the maintainer
  boundary for public conformance scorecards, badges, publication summaries,
  and third-party-legible reporting
- public conformance reporting must stay on the existing conformance corpus,
  external-validation, release-evidence, and checked-in schema surfaces
  instead of inventing a second claim spreadsheet or dashboard-only workflow
- use the public runner actions for source-surface checking, schema checking,
  scorecard derivation, and publication before widening public claim language

Standard-library entrypoint:

- `docs/runbooks/objc3c_stdlib_foundation.md` is the maintainer boundary for
  the checked-in stdlib root, canonical module inventory, alias mapping, and
  machine-owned stdlib workspace materialization flow
- `docs/runbooks/objc3c_stdlib_core.md` is the maintainer boundary for the
  core stdlib utility, text/data, collection, option, and result family split
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

- command-surface contract build: `python scripts/objc3c_public_workflow_runner.py build-public-command-contract`
- command-surface contract check: `python scripts/objc3c_public_workflow_runner.py check-public-command-contract`
- command-surface budget check: `python scripts/objc3c_public_workflow_runner.py check-public-command-budget`
- dependency boundaries: `npm run check:objc3c:boundaries`
- task hygiene: `npm run check:task-hygiene`
- repo superclean surface: `npm run check:repo:surface`
- repo superclean integration: `npm run test:repo`
- docs stitch/check: `npm run check:docs:native`
- parity source check: `python scripts/check_objc3c_library_cli_parity.py ...`
- developer tooling boundary: `docs/runbooks/objc3c_developer_tooling.md`
- bonus experiences boundary: `docs/runbooks/objc3c_bonus_experiences.md`
- performance benchmark boundary: `docs/runbooks/objc3c_performance.md`
- performance governance boundary: `docs/runbooks/objc3c_performance_governance.md`
- performance governance source-surface check: `npm run check:objc3c:performance-governance:surface`
- performance governance schema check: `npm run check:objc3c:performance-governance:schemas`
- performance governance dashboard build: `npm run inspect:objc3c:performance-dashboard`
- performance governance publication: `npm run publish:objc3c:performance-report`
- integrated performance governance workflow: `npm run test:objc3c:performance-governance`
- performance governance integration proof: `npm run test:objc3c:performance-governance:integration`
- performance governance end-to-end proof: `npm run test:objc3c:performance-governance:e2e`
- release foundation boundary: `docs/runbooks/objc3c_release_foundation.md`
- release foundation source-surface check: `npm run check:objc3c:release-foundation:surface`
- release foundation schema check: `npm run check:objc3c:release-foundation:schemas`
- release manifest build: `npm run inspect:objc3c:release-manifest`
- release provenance publication: `npm run publish:objc3c:release-provenance`
- integrated release foundation workflow: `npm run test:objc3c:release-foundation`
- packaging channels boundary: `docs/runbooks/objc3c_packaging_channels.md`
- packaging channels source-surface check: `npm run check:objc3c:packaging-channels:surface`
- packaging channels schema check: `npm run check:objc3c:packaging-channels:schemas`
- packaging channels build: `npm run package:objc3c:channels`
- integrated packaging channels workflow: `npm run test:objc3c:packaging-channels`
- packaging channels end-to-end proof: `npm run test:objc3c:packaging-channels:e2e`
- release operations boundary: `docs/runbooks/objc3c_release_operations.md`
- release operations source-surface check: `npm run check:objc3c:release-operations:surface`
- release operations schema check: `npm run check:objc3c:release-operations:schemas`
- update manifest build: `npm run inspect:objc3c:update-manifest`
- release operations publication: `npm run publish:objc3c:release-operations`
- integrated release operations workflow: `npm run test:objc3c:release-operations`
- release operations end-to-end proof: `npm run test:objc3c:release-operations:e2e`
- distribution credibility boundary: `docs/runbooks/objc3c_distribution_credibility.md`
- distribution credibility source-surface check: `npm run check:objc3c:distribution-credibility:surface`
- distribution credibility schema check: `npm run check:objc3c:distribution-credibility:schemas`
- distribution credibility dashboard build: `npm run inspect:objc3c:distribution-credibility`
- distribution credibility publication: `npm run publish:objc3c:distribution-credibility`
- integrated distribution credibility workflow: `npm run test:objc3c:distribution-credibility`
- distribution credibility end-to-end proof: `npm run test:objc3c:distribution-credibility:e2e`
- runtime performance boundary: `docs/runbooks/objc3c_runtime_performance.md`
- runtime performance benchmark: `npm run inspect:objc3c:runtime-performance`
- integrated runtime performance validation: `npm run test:objc3c:runtime-performance`
- runnable runtime performance validation: `npm run test:objc3c:runnable-runtime-performance`
- compiler throughput boundary: `docs/runbooks/objc3c_compiler_throughput.md`
- compiler throughput benchmark: `npm run inspect:objc3c:compiler-throughput`
- integrated compiler throughput validation: `npm run test:objc3c:compiler-throughput`
- runnable compiler throughput validation: `npm run test:objc3c:runnable-compiler-throughput`
- stress validation boundary: `docs/runbooks/objc3c_stress_validation.md`
- external validation boundary: `docs/runbooks/objc3c_external_validation.md`
- external validation source-surface check: `npm run check:external-validation:surface`
- integrated external validation drill: `npm run test:objc3c:external-validation`
- external validation replay drill: `npm run test:objc3c:external-validation:replay`
- external repro publication: `npm run publish:objc3c:external-repro-corpus`
- external validation integration proof: `npm run test:objc3c:external-validation:integration`
- public conformance reporting boundary: `docs/runbooks/objc3c_public_conformance_reporting.md`
- public conformance source-surface check: `npm run check:objc3c:public-conformance:surface`
- public conformance schema check: `npm run check:objc3c:public-conformance:schemas`
- public conformance scorecard build: `npm run inspect:objc3c:public-conformance:scorecard`
- public conformance publication: `npm run publish:objc3c:public-conformance`
- integrated public conformance workflow: `npm run test:objc3c:public-conformance`
- public conformance integration proof: `npm run test:objc3c:public-conformance:integration`
- public conformance end-to-end proof: `npm run test:objc3c:public-conformance:e2e`
- stress source-surface check: `npm run check:stress:surface`
- parser/sema fuzz safety: `npm run test:objc3c:fuzz-safety`
- lowering/runtime stress: `npm run test:objc3c:lowering-runtime-stress`
- mixed-module differential stress: `npm run test:objc3c:mixed-module-differential`
- stress minimization: `npm run test:objc3c:stress-minimization`
- stress crash triage: `npm run test:objc3c:stress-crash-triage`
- integrated stress workflow: `npm run test:objc3c:stress`
- stress integration proof: `npm run test:objc3c:stress:integration`
- stress end-to-end proof: `npm run test:objc3c:stress:e2e`
- stdlib foundation boundary: `docs/runbooks/objc3c_stdlib_foundation.md`
- stdlib core boundary: `docs/runbooks/objc3c_stdlib_core.md`
- stdlib surface check: `npm run check:stdlib:surface`
- stdlib workspace materialization: `npm run build:objc3c:stdlib`
- stdlib integration: `npm run test:stdlib`
- advanced stdlib integration: `npm run test:stdlib:advanced`
- advanced stdlib runnable integration: `npm run test:stdlib:advanced:e2e`

The live maintainer surface is intentionally small. Historical planning, contract, and milestone-specific validation material is archived under `tmp/archive/`.
