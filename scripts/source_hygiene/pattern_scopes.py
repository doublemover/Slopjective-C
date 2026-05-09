from __future__ import annotations


PUBLIC_SURFACE_PATHS: tuple[str, ...] = (
    "README.md",
    "CONTRIBUTING.md",
    ".github/**",
    "docs/**",
    "showcase/**",
    "spec/**",
    "schemas/**",
    "site/**",
    "stdlib/**",
    "package.json",
)

ACTIVE_SOURCE_PATHS: tuple[str, ...] = PUBLIC_SURFACE_PATHS + (
    "native/objc3c/**",
    "scripts/**",
    "tests/**",
)

IMPLEMENTATION_SOURCE_PATHS: tuple[str, ...] = (
    "native/objc3c/**",
    "scripts/**",
)

WORKFLOW_COMMAND_SOURCE_PATHS: tuple[str, ...] = (
    "scripts/**",
    "tests/**",
)

WORKFLOW_COMMAND_SOURCE_EXCLUDES: tuple[str, ...] = (
    "scripts/source_hygiene/**",
    "scripts/objc3c_workflow/runner.py",
    "tests/tooling/source_hygiene/**",
)

PACKAGE_MANIFEST_PATHS: tuple[str, ...] = ("package.json",)

CANONICAL_REJECTION_REGISTRY_PATHS: tuple[str, ...] = (
    "native/objc3c/src/config/**",
    "native/objc3c/src/diagnostics/modes/**",
)
