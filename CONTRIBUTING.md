# Contributing

Keep changes coherent, source-owned and reviewable. Implement Objective-C 3.0
against the pinned native Clang toolchain; use hard cutovers when contracts change.

## Current contributor routes

Start with the [README](README.md), [tutorials](docs/tutorials/README.md) and
[Linux roadmap](docs/roadmap/linux_forward_development.md). The
[command appendix](docs/runbooks/objc3c_public_command_surface.md) describes the
current repository CLI. The [maintainer guide](docs/runbooks/objc3c_maintainer_workflows.md)
covers compiler patch export and coordinated SDK delivery.

## Source and output ownership

- Compiler, runtime and frontend integration: `native/objc3c/`, maintained LLVM
  patches in `toolchains/llvm/`, scripts in `scripts/`, and fixtures in `tests/`.
- Language requirements: `spec/`; current library contracts: `stdlib/`.
- Contributor documentation: this file, `README.md`, `docs/tutorials/` and the two
  linked command/maintainer guides. Feature implementation boundaries live in
  `docs/roadmap/`.
- Site source: `site/src/`; regenerate `site/index.md` with
  `python -B scripts/build_pages.py` when its source changes.
- `out/`, `tmp/`, `artifacts/` and build caches are machine-owned outputs. Keep
  generated binaries, SDK archives, temporary reports and Python bytecode out of Git.

The canonical LLVM development checkout is prepared under `out/toolchains/`.
Export its actual source edits into the maintained patch stack before committing;
that checkout is the compiler authoring tree, not a second tracked source copy.

## Lightweight checks

Install the documented pinned development tools and Node dependencies first.
Run CLI discovery and the reader-route guard for contributor-facing changes:

```sh
python -B -m scripts.objc3c_toolchain --help
python -B -m scripts.objc3c_toolchain.check_onboarding
```

Format and lint the Markdown/JSON files you changed. For example, for this guide:

```sh
npx --no-install prettier --check CONTRIBUTING.md
npx --no-install markdownlint-cli2 --no-globs CONTRIBUTING.md
git diff --check
```

For onboarding or CLI guard changes, run the focused tooling tests:

```sh
python -B -m pytest -q tests/tooling/test_current_onboarding.py
```

For compiler/runtime changes, start with the affected syntax, type, compile and
focused runtime checks. Record postponed validation and run it before delivery.
A prose-only change does not require rebuilding or qualifying an SDK.

## Compiler and delivery work

Use the [maintainer guide](docs/runbooks/objc3c_maintainer_workflows.md) to export
and seal native edits, select the relevant frontend/native checks, and perform
one coordinated SDK/archive qualification for a coherent implementation batch.
Honor the configured CPU, memory, compile and link limits. Reuse verified build
outputs and the configured temporary workspace; avoid duplicate SDK stages and
unbounded caches.

## Branches, commits and pull requests

- Use descriptive branch names and keep each implementation span reviewable.
- Commit and push completed work; preserve unrelated work in the shared tree.
- Target the current integration branch when a PR depends on unmerged work.
- Explain the resulting behavior, validation actually run, material limitations
  and remaining implementation. Do not claim whole-profile conformance from a
  bounded fixture or a successful compiler build.
