from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import display_path


def validate_extract_log(extract_log_path: Path) -> list[str]:
    try:
        text = extract_log_path.read_text(encoding="utf-8")
    except FileNotFoundError:
        return [f"extract log does not exist: {display_path(extract_log_path)}."]

    findings: list[str] = []
    if not text.startswith("# extract_open_blockers snapshot-json command output\n"):
        findings.append(
            "extract log header drift: expected "
            "'# extract_open_blockers snapshot-json command output'."
        )
    if "\n## stdout\n" not in text:
        findings.append("extract log missing '## stdout' section.")
    if "\n## stderr\n" not in text:
        findings.append("extract log missing '## stderr' section.")
    return findings
