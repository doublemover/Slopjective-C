"""Core documentation runbook source catalog rows."""

from __future__ import annotations

from documentation_surface import paths as docs_paths

from .runbook_source_types import RunbookSourceSpec


def core_runbook_source_specs() -> tuple[RunbookSourceSpec, ...]:
    return (
        RunbookSourceSpec(
            docs_paths.SITE_BODY_PATH,
            required_tokens=(
                "## At a Glance {#toc-status-scope-note}",
                "## Quick Routes {#toc-quick-routes}",
                "## Reader Promises {#toc-reader-promises}",
                "## Specification Map {#toc-front-matter}",
                "## Language Parts {#toc-parts}",
                "[README.md](../README.md)",
                "[docs/tutorials/README.md](../docs/tutorials/README.md)",
                "[docs/tutorials/getting_started.md](../docs/tutorials/getting_started.md)",
                "[docs/tutorials/build_run_verify.md](../docs/tutorials/build_run_verify.md)",
                "[docs/tutorials/guided_walkthrough.md](../docs/tutorials/guided_walkthrough.md)",
                "[docs/tutorials/objc2_to_objc3_migration.md](../docs/tutorials/objc2_to_objc3_migration.md)",
                "[docs/tutorials/objc2_swift_cpp_comparison.md](../docs/tutorials/objc2_swift_cpp_comparison.md)",
                "[docs/objc3c-native.md](../docs/objc3c-native.md)",
                "[capability matrix](../docs/support/capability_matrix.md)",
                "[evidence map](../docs/support/evidence_map.md)",
            ),
            forbidden_tokens=(
                "[archived spec index](../docs/reference/legacy_spec_anchor_index.md#legacy-files)",
            ),
        ),
        RunbookSourceSpec(
            docs_paths.SITE_POLICY_PATH,
            required_tokens=(
                "## Tone and Accessibility Rules",
            ),
        ),
        RunbookSourceSpec(
            docs_paths.NATIVE_OWNERSHIP_PATH,
            required_tokens=(
                "## Public Doc Style And Accessibility Rules",
            ),
        ),
        RunbookSourceSpec(
            docs_paths.NATIVE_FRAGMENT_README_PATH,
            required_tokens=(
                "## Canonical Naming And Path Rules",
                "user-facing package entrypoints come from `package.json`",
                "transient outputs stay under `tmp/`",
                "published binaries and libraries stay under `artifacts/`",
            ),
        ),
        RunbookSourceSpec(
            docs_paths.MAINTAINER_WORKFLOW_PATH,
            required_tokens=(
                "## Superclean Working Boundary",
                "implementation roots:",
                "generated checked-in outputs:",
                "docs/tutorials/",
                "CONTRIBUTING.md",
                "showcase/",
                "docs/runbooks/objc3c_developer_tooling.md",
                "docs/runbooks/objc3c_bonus_experiences.md",
                "docs/runbooks/objc3c_performance.md",
                "docs/runbooks/objc3c_runtime_performance.md",
                "docs/runbooks/objc3c_compiler_throughput.md",
                "Do not add milestone-specific wrappers, sidecar compatibility files, or",
            ),
        ),
    )


__all__ = ("core_runbook_source_specs",)
