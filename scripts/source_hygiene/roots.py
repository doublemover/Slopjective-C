from __future__ import annotations


DEFAULT_SCAN_ROOTS: tuple[str, ...] = (
    "README.md",
    "CONTRIBUTING.md",
    "package.json",
    ".github",
    "docs",
    "showcase",
    "spec",
    "schemas",
    "native/objc3c",
    "scripts",
    "tests",
    "stdlib",
    "site",
)

DEFAULT_EXCLUDES: tuple[str, ...] = (
    ".git/**",
    "node_modules/**",
    "tmp/**",
    "artifacts/**",
    "reports/**",
    "scripts/source_hygiene/**",
    "scripts/check_source_hygiene_hard_cutover.py",
    "schemas/source-hygiene-hard-cutover-report-v1.schema.json",
    "tests/tooling/source_hygiene/**",
    "tests/tooling/test_objc3c_canonical_language_config.py",
    # Canonical rejection registries intentionally contain retired spellings as
    # rejected inputs, not support claims. Keep this scoped to the registries.
    "native/objc3c/src/config/**",
    "native/objc3c/src/diagnostics/modes/**",
)

TEXT_SUFFIXES: frozenset[str] = frozenset(
    {
        ".c",
        ".cc",
        ".cjs",
        ".cmake",
        ".cpp",
        ".css",
        ".h",
        ".hh",
        ".html",
        ".hpp",
        ".inc",
        ".js",
        ".json",
        ".jsx",
        ".md",
        ".mjs",
        ".ps1",
        ".py",
        ".toml",
        ".ts",
        ".tsx",
        ".txt",
        ".xml",
        ".yaml",
        ".yml",
    }
)
