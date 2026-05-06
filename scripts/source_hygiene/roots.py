from __future__ import annotations


DEFAULT_SCAN_ROOTS: tuple[str, ...] = (
    "README.md",
    ".github",
    "docs",
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
    "native/objc3c/src/config/**",
    "native/objc3c/src/diagnostics/modes/**",
)

TEXT_SUFFIXES: frozenset[str] = frozenset(
    {
        ".c",
        ".cc",
        ".cmake",
        ".cpp",
        ".h",
        ".hpp",
        ".inc",
        ".json",
        ".md",
        ".ps1",
        ".py",
        ".txt",
        ".yaml",
        ".yml",
    }
)
