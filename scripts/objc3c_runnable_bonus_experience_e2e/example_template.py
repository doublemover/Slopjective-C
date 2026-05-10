from __future__ import annotations

from pathlib import Path


def write_template_readme(
    *,
    package_root: Path,
    template_readme: Path,
    example_id: str,
    source_path: Path,
    template_source: Path,
    guided_walkthrough_manifest: Path,
) -> None:
    template_readme.write_text(
        "\n".join(
            [
                f"# {example_id} Template",
                "",
                "This machine-owned template is derived from the packaged showcase portfolio.",
                "",
                f"- packaged source: `{source_path.relative_to(package_root).as_posix()}`",
                f"- generated source: `{template_source.relative_to(package_root).as_posix()}`",
                (
                    "- guided walkthrough manifest: "
                    f"`{guided_walkthrough_manifest.relative_to(package_root).as_posix()}`"
                ),
            ]
        )
        + "\n",
        encoding="utf-8",
    )


__all__ = ("write_template_readme",)
