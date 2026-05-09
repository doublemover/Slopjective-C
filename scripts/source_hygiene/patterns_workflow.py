from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import PACKAGE_MANIFEST_PATHS, PUBLIC_SURFACE_PATHS


WORKFLOW_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "retired-public-workflow-runner",
        "The public workflow runner file was replaced by the canonical scripts.objc3c_workflow module.",
        r"\bobjc3c_public_workflow_runner\.py\b",
        residue_class="old-public-command-surface",
    ),
    ForbiddenPattern(
        "retired-npm-workflow-command",
        "Workflow actions must route through npm run objc3c -- <action>, not retired colon-style npm command names.",
        r"\bnpm\s+run\s+(?!objc3c\b)[a-z0-9][a-z0-9:_-]*:",
        residue_class="alias-residue",
    ),
    ForbiddenPattern(
        "retired-package-script-alias-key",
        "package.json must expose only the objc3c bridge, not retired colon-style public script aliases.",
        r'^\s*"(?!objc3c")[A-Za-z0-9][A-Za-z0-9_-]*:[A-Za-z0-9:_-]*"\s*:',
        include_paths=PACKAGE_MANIFEST_PATHS,
        residue_class="alias-residue",
    ),
    ForbiddenPattern(
        "direct-workflow-module-doc-command",
        "Public workflow instructions must expose the npm bridge instead of direct scripts.objc3c_workflow module commands.",
        r"(?<![A-Za-z0-9_.-])python\s+-m\s+scripts\.objc3c_workflow\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        exclude_paths=PACKAGE_MANIFEST_PATHS,
        residue_class="old-public-command-surface",
    ),
    ForbiddenPattern(
        "direct-public-script-command",
        "Public workflow instructions must expose npm run objc3c -- <action>, not direct script commands.",
        r"(?<![A-Za-z0-9_.-])(?:python(?:\s+-m)?|py|pwsh|powershell(?:\.exe)?|bash|sh)\s+(?:\.?[\\/])?scripts[\\/][^\n`]+",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="old-public-command-surface",
    ),
    ForbiddenPattern(
        "direct-native-compile-wrapper-command",
        "Public native compile instructions must expose npm run objc3c -- compile-objc3c instead of direct PowerShell wrapper commands.",
        r"\b(?:pwsh|powershell(?:\.exe)?)\s+[^\n]{0,180}\bscripts[\\/]+objc3c_native_compile\.ps1\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="old-public-command-surface",
    ),
    ForbiddenPattern(
        "retired-lint-default-action",
        "The retired lint-default action must not reappear; use npm run objc3c -- lint.",
        r"\bnpm\s+run\s+objc3c\s+--\s+lint-default\b|(?<![A-Za-z0-9_-])lint-default(?![A-Za-z0-9_-])",
        residue_class="old-public-command-surface",
    ),
    ForbiddenPattern(
        "retired-public-script-alias-metadata",
        "Retired public script alias metadata must not reappear on active hard-cutover surfaces.",
        r"(?<![A-Za-z0-9_])public_scripts(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])publicScripts(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])public_script_aliases(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])publicScriptAliases(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])public[-_\s]+scripts?(?:[-_\s]+alias(?:es)?)?(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])public[-_\s]+script[-_\s]+alternates?(?![A-Za-z0-9_])",
        residue_class="alias-residue",
    ),
    ForbiddenPattern(
        "retired-command-lane-support-wording",
        "Public command surfaces must not publish retired command-lane or retired-source support wording.",
        r"\bfallback[-_\s]+command[-_\s]+lanes?\b"
        r"|\bfallback[-_\s]+lanes?\b"
        r"|\bmigration[-_\s]+lane[^\n]{0,80}\bsupport(?:\s+claims?|\s+semantics)?\b"
        r"|\bretired[-_\s]+package[-_\s]+script[-_\s]+aliases\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="old-public-command-surface",
    ),
    ForbiddenPattern(
        "retired-workflow-action-registry-facade",
        "Workflow actions must use the canonical action catalog, not the retired registry.py facade.",
        r"scripts[\\/]+objc3c_workflow[\\/]+registry\.py\b"
        r"|scripts\.objc3c_workflow\.registry\b"
        r"|from\s+\.{1,2}registry\s+import\s+ACTION_SPECS"
        r"|from\s+scripts\.objc3c_workflow\.registry\s+import\s+ACTION_SPECS",
        residue_class="old-public-command-surface",
    ),
)
