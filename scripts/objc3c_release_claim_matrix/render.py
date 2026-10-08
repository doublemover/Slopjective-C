"""Artifact rendering for the release/runtime claim matrix."""

from __future__ import annotations

from objc3c_tooling.paths import display_path

from .probes import MatrixProbeArtifacts


def render_markdown(probes: MatrixProbeArtifacts) -> str:
    return f"""# Release/Runtime Claim Matrix

| Surface | Current state |
| --- | --- |
| Claimed profiles | `core`, `strict`, `strict-concurrency` |
| Language profiles | `canonical`, `strict`, `strict-concurrency` |
| Removed mode options | rejected with canonical diagnostics |
| Strict-system | fail-closed and not claimed |
| Feature-macro publication | truthful and fail-closed; no source-only macro promotion |
| Emit/validate format | `json` only |
| Native CLI sidecars | report + publication + validation |
| Frontend C API sidecars | report + publication |
| Optional features | `throws`, `async-await`, `actors`, `blocks`, and `arc` remain not claimed |
| Follow-on surface | `objc3c.releaseclaims.compatibilityupgrade.boundary.v1` |

## Live proofs

- Native CLI report: `{display_path(probes.native_report_path)}`
- Native CLI publication: `{display_path(probes.native_publication_path)}`
- Native CLI validation: `{display_path(probes.validation_path)}`
- Frontend C API report: `{display_path(probes.runner_report_path)}`
- Frontend C API publication: `{display_path(probes.runner_publication_path)}`
- Strict-system reject rc: `{probes.strict_reject.returncode}`
- YAML emit reject rc: `{probes.yaml_reject.returncode}`
"""
