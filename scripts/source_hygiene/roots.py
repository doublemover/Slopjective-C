from __future__ import annotations

SOURCE_HYGIENE_ROOTS_CONTRACT_ID = "source-hygiene-roots-v1"
SOURCE_HYGIENE_ROOTS_OWNER_SURFACE = "scripts/source_hygiene/roots.py"

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


def roots_contract_summary() -> dict[str, object]:
    return {
        "contract_id": SOURCE_HYGIENE_ROOTS_CONTRACT_ID,
        "owner_surface": SOURCE_HYGIENE_ROOTS_OWNER_SURFACE,
        "default_scan_roots": list(DEFAULT_SCAN_ROOTS),
        "default_excludes": list(DEFAULT_EXCLUDES),
        "text_suffixes": sorted(TEXT_SUFFIXES),
        "public_command_truth_roots": [
            root
            for root in DEFAULT_SCAN_ROOTS
            if root in {"README.md", "CONTRIBUTING.md", "docs", "spec", "site", "stdlib"}
        ],
        "tmp_and_report_outputs_excluded": True,
        "canonical_rejection_registry_excludes_are_scoped": True,
    }


__all__ = [
    "DEFAULT_EXCLUDES",
    "DEFAULT_SCAN_ROOTS",
    "SOURCE_HYGIENE_ROOTS_CONTRACT_ID",
    "SOURCE_HYGIENE_ROOTS_OWNER_SURFACE",
    "TEXT_SUFFIXES",
    "roots_contract_summary",
]
