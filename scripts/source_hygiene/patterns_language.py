from __future__ import annotations

from .pattern_model import ForbiddenPattern


LANGUAGE_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "public-compatibility-mode",
        "Compatibility-mode surfaces are removed in the canonical-only cutover.",
        r"\bcompatibility_mode\b|\bcompatibility[-\s]+mode\b|--objc3-compat-mode|\bObjc3\w*CompatibilityMode\b|OBJC3C_FRONTEND_COMPATIBILITY_MODE",
        residue_class="legacy-compatibility-text",
    ),
    ForbiddenPattern(
        "legacy-language-profile-enum",
        "Language profile enums are canonical-only and must not retain legacy values.",
        r"\bkLegacy\b|\bLegacy\s*=\s*1\b|\bObjc3\w*LanguageProfile::k?Legacy\b",
        residue_class="legacy-compatibility-text",
    ),
    ForbiddenPattern(
        "canonical-rejection-diagnostics-surface",
        "Migration-assist surfaces are removed in the hard cutover.",
        r"\bmigration[_-]?assist\b|--objc3-canonical-rejection-diagnostics|\blegacy_literal_diagnostics\b",
        residue_class="legacy-compatibility-text",
    ),
)
