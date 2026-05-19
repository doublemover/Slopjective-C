from __future__ import annotations

from typing import Any, Sequence

from objc3c_library_cli_parity.artifacts import (
    build_dimension_results,
    resolve_artifact_digest,
)
from objc3c_library_cli_parity.contracts import ParityEvaluation
from objc3c_library_cli_parity.contracts import ParityInputs


def evaluate_parity(
    *,
    inputs: ParityInputs,
    artifacts: Sequence[str],
    dimension_map: dict[str, str],
) -> ParityEvaluation:
    failures: list[str] = []
    comparisons: list[dict[str, Any]] = []
    artifact_to_dimension = {
        artifact: dimension
        for dimension, artifact in dimension_map.items()
    }
    for artifact_name in artifacts:
        dimension = artifact_to_dimension.get(artifact_name)
        canonical_json = (
            dimension in {"diagnostics", "manifest"}
            and artifact_name.endswith(".json")
        )
        try:
            library_digest = resolve_artifact_digest(
                base_dir=inputs.library_dir,
                artifact_name=artifact_name,
                canonical_json=canonical_json,
            )
        except ValueError as exc:
            failures.append(f"library {artifact_name}: {exc}")
            continue
        try:
            cli_digest = resolve_artifact_digest(
                base_dir=inputs.cli_dir,
                artifact_name=artifact_name,
                canonical_json=canonical_json,
            )
        except ValueError as exc:
            failures.append(f"cli {artifact_name}: {exc}")
            continue

        if library_digest.source_kind != cli_digest.source_kind:
            failures.append(
                f"source-kind mismatch for {artifact_name}: "
                f"library={library_digest.source_kind} cli={cli_digest.source_kind}"
            )
            continue

        library_sha = library_digest.sha256
        cli_sha = cli_digest.sha256
        matches = library_sha == cli_sha
        if not matches:
            failures.append(
                f"digest mismatch for {artifact_name}: "
                f"library={library_sha[:16]} cli={cli_sha[:16]}"
            )

        comparisons.append(
            {
                "artifact": artifact_name,
                "dimension": dimension if dimension is not None else "extra",
                "source_kind": library_digest.source_kind,
                "library_source": library_digest.source_path,
                "cli_source": cli_digest.source_path,
                "library_sha256": library_sha,
                "cli_sha256": cli_sha,
                "matches": matches,
            }
        )

    return ParityEvaluation(
        comparisons=comparisons,
        dimensions=build_dimension_results(
            artifacts=artifacts,
            dimension_map=dimension_map,
        ),
        failures=failures,
    )


__all__ = ["evaluate_parity"]
