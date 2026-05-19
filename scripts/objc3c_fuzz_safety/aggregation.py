"""Result aggregation for the objc3c fuzz-safety runner."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import display_path

from .config import MODE, SCHEMA_VERSION
from .corpus import BASE_CORPUS, MUTATION_RULES, build_corpus, load_manifest_cases
from .execution import run_case


def evaluate(
    *,
    compiler_command: list[str],
    out_root: Path,
    manifest_path: Path,
    timeout_sec: float,
    max_cases: int | None,
    generated_at_utc: str,
) -> dict[str, object]:
    out_root.mkdir(parents=True, exist_ok=True)
    manifest_cases = load_manifest_cases(manifest_path)
    corpus = build_corpus(max_cases=max_cases, manifest_path=manifest_path)
    results = [
        run_case(
            case=case,
            compiler_command=compiler_command,
            out_root=out_root,
            timeout_sec=timeout_sec,
        )
        for case in corpus
    ]

    violations: list[dict[str, str]] = []
    for result in results:
        case_id = str(result["case_id"])
        errors = result["errors"]
        assert isinstance(errors, list)
        for error in errors:
            assert isinstance(error, dict)
            violations.append(
                {
                    "check_id": str(error["check_id"]),
                    "case_id": case_id,
                    "detail": str(error["detail"]),
                }
            )

    violations = sorted(
        violations,
        key=lambda item: (item["check_id"], item["case_id"], item["detail"]),
    )
    status = "PASS" if not violations else "FAIL"

    return {
        "mode": MODE,
        "schema_version": SCHEMA_VERSION,
        "generated_at_utc": generated_at_utc,
        "config": {
            "timeout_sec": timeout_sec,
            "iteration_count": 2,
            "case_count": len(corpus),
            "out_root": display_path(out_root),
        },
        "corpus_strategy": {
            "strategy_id": "objc3c-malformed-corpus-with-manifest-v2",
            "base_case_count": len(BASE_CORPUS),
            "manifest_path": display_path(manifest_path),
            "manifest_case_count": len(manifest_cases),
            "mutation_rules": [rule.name for rule in MUTATION_RULES],
            "deterministic_ordering": "case_id-lexicographic",
        },
        "status": status,
        "violation_count": len(violations),
        "violations": violations,
        "results": results,
    }
