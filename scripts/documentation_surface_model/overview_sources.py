"""Root-level reader documentation sources for the documentation surface model."""

from __future__ import annotations

from documentation_surface import paths as docs_paths
from documentation_surface.model import DocumentationSurfaceSource

from .source_factory import _source


def overview_documentation_sources() -> tuple[DocumentationSurfaceSource, ...]:
    return (
        _source(
            docs_paths.README_PATH,
            required_tokens=(
                "## Start Here",
                "## Fresh Setup",
                "## First Working Session",
                "## Public Command Surface",
                "## Spec Structure",
                "## Superclean Boundary",
                "published site",
                "CONTRIBUTING.md",
                "docs/tutorials/",
                "showcase/README.md",
                "docs/runbooks/objc3c_public_command_surface.md",
                "Canonical roots:",
                "Explicit non-goals for cleanup work:",
            ),
        ),
        _source(
            docs_paths.CONTRIBUTING_PATH,
            required_tokens=(
                "## Contributor Surface",
                "## Repo Boundary",
                "README.md",
                "CONTRIBUTING.md",
                "docs/tutorials/README.md",
                "docs/tutorials/getting_started.md",
                "docs/tutorials/build_run_verify.md",
                "docs/tutorials/guided_walkthrough.md",
                "docs/tutorials/objc2_to_objc3_migration.md",
                "docs/tutorials/objc2_swift_cpp_comparison.md",
                "docs/runbooks/objc3c_public_command_surface.md",
                "docs/runbooks/objc3c_maintainer_workflows.md",
                "showcase/",
                "native/objc3c/",
                "scripts/",
                "tests/",
                "tmp/",
                "artifacts/",
                "npm run objc3c -- check-repo-superclean-surface",
            ),
        ),
        _source(
            docs_paths.SHOWCASE_README_PATH,
            required_tokens=(
                "# Showcase Examples",
                "## Portfolio Boundary",
                "## Capability-First Entry Points",
                "showcase/portfolio.json",
                "showcase/auroraBoard/main.objc3",
                "showcase/signalMesh/main.objc3",
                "showcase/patchKit/main.objc3",
                "actor-shaped messaging",
                "stdlib/README.md",
                "objc3.core",
                "objc3.concurrency",
                "objc3.keypath",
                "objc3.system",
                "scripts/check_showcase_surface.py",
                "tmp/artifacts/showcase/",
                "## Explicit Non-Goals",
            ),
            forbidden_tokens=(
                "target story: status bridging, actors, runtime messaging",
                "--capability actors",
            ),
        ),
    )


__all__ = ("overview_documentation_sources",)
