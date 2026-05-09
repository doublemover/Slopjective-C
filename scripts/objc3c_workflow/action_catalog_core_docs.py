"""Core documentation and public command-surface action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_DOCS_ACTION_SPECS: dict[str, ActionSpec] = {
    "build-site": ActionSpec("build-site", "build published site output and format it", "python:scripts/build_site_index.py + npx prettier"),
    "check-site": ActionSpec("check-site", "check generated site output for drift", "python:scripts/build_site_index.py --check", validation_tier="docs", guarantee_owner="published site index generation stays in sync with site/src inputs"),
    "build-native-docs": ActionSpec("build-native-docs", "build the generated native implementation docs", "python:scripts/build_objc3c_native_docs.py"),
    "check-native-docs": ActionSpec("check-native-docs", "check generated native implementation docs for drift", "python:scripts/build_objc3c_native_docs.py --check", validation_tier="docs", guarantee_owner="generated native implementation documentation stays in sync with docs/objc3c-native/src inputs"),
    "build-public-command-surface": ActionSpec("build-public-command-surface", "build the generated public command-surface appendix", "python:scripts/render_objc3c_public_command_surface.py"),
    "check-public-command-surface": ActionSpec("check-public-command-surface", "check the generated public command-surface appendix for drift", "python:scripts/render_objc3c_public_command_surface.py --check", validation_tier="docs", guarantee_owner="operator-facing machine appendix stays in sync with the live workflow runner and package scripts"),
    "build-public-command-contract": ActionSpec("build-public-command-contract", "build the canonical public command contract artifact", "python:scripts/build_objc3c_public_command_contract.py"),
    "check-public-command-contract": ActionSpec("check-public-command-contract", "check the canonical public command contract artifact for drift", "python:scripts/build_objc3c_public_command_contract.py --check"),
    "check-public-command-budget": ActionSpec("check-public-command-budget", "check the public command budget and appendix sync against the canonical command contract", "python:scripts/check_objc3c_public_command_budget.py"),
    "check-documentation-surface": ActionSpec("check-documentation-surface", "check the reader-facing documentation structure and machine-appendix boundary", "python:scripts/check_documentation_surface.py", validation_tier="docs", guarantee_owner="reader-facing onboarding, site structure, and machine-appendix boundary stay accessible and explicit"),
    "check-markdown": ActionSpec("check-markdown", "check markdown formatting drift across checked-in docs", "npx prettier --check <checked-in-md-globs>"),
    "format-markdown": ActionSpec("format-markdown", "rewrite markdown formatting across checked-in docs", "npx prettier --write <checked-in-md-globs>"),
    "lint-markdown": ActionSpec("lint-markdown", "run markdownlint across checked-in docs", "npx markdownlint-cli2 <checked-in-md-globs>"),
    "validate-documentation-surface": ActionSpec("validate-documentation-surface", "run the full documentation build and reader-surface validation flow", "runner-internal + generated documentation checks", validation_tier="docs", guarantee_owner="site output, native docs, command appendix, and reader-facing onboarding remain buildable, in sync, and explicit"),
}
