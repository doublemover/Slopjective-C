from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import resolve_repo_path

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.support_links import _row_support_claims

OBJECT_MODEL_IMPLEMENTED_PREFIX = "runtime.object-model."
OBJECT_MODEL_SUPPORT_CLAIM_PREFIX = "objc3c.behavior.runtime.object-model-"
OBJECT_MODEL_RUNTIME_FIXTURE_PREFIX = "tests/native/runtime/object_model/"
OBJECT_MODEL_RUNTIME_SOURCE_PREFIXES = (
    "native/objc3c/src/runtime/classes/",
    "native/objc3c/src/runtime/images/",
    "native/objc3c/src/runtime/reflection/",
    "native/objc3c/src/runtime/state/",
    "native/objc3c/src/runtime/storage/",
)
OBJECT_MODEL_FULL_REALIZATION_ID = "runtime.object-model.full-realization"
OBJECT_MODEL_BROAD_SCOPE_PHRASES = (
    "full object-model",
    "full runtime object",
    "full live class",
    "object-model runtime realization",
    "broad object-model",
    "public reflection abi",
)

FOUNDATION_BOUNDARY_EXPECTATIONS = (
    {
        "id": "runtime.object-model.full-realization",
        "state": "reserved",
        "summary_tokens": (
            "production compiler-owned object-model source identity",
            "statement-level debugger stepping remain reserved",
        ),
        "evidence_paths": (
            "tests/tooling/fixtures/object_model_closure/debugger_value_inspection_replay_contract.json",
            "tests/tooling/fixtures/cross_lane_e2e/object_reflection_debugger.expectation.json",
        ),
        "no_support_claims": True,
    },
    {
        "id": "language.advanced-runtime-closure",
        "state": "implemented",
        "summary_tokens": (
            "native link/run",
            "broad scheduler fairness",
        ),
        "evidence_paths": (
            "tests/native/runtime/advanced_closure/negative_matrix.contract.json",
            "tests/tooling/fixtures/advanced_runtime_closure/combined_runtime_identity_contract.json",
            "tests/tooling/fixtures/cross_lane_e2e/advanced_runtime_closure.expectation.json",
        ),
    },
    {
        "id": "compiler.optimization.method-inlining",
        "state": "reserved",
        "summary_tokens": (
            "production ir still retains",
            "fails closed",
        ),
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/optimization_runtime_equivalence.expectation.json",
            "tests/tooling/fixtures/semantic_optimization_pipeline/reserved_method_inlining_skip.json",
        ),
        "no_support_claims": True,
    },
    {
        "id": "modules.direct-import-syntax",
        "state": "implemented",
        "summary_tokens": (
            "parser-admitted",
            "locked package provenance",
            "fail closed",
        ),
        "evidence_paths": (
            "tests/tooling/fixtures/package_ecosystem/direct_import_module_syntax_contract.json",
            "scripts/check_objc3c_direct_import_module_syntax.py",
        ),
    },
    {
        "id": "runtime.debug-trace.full-source-map-publication",
        "state": "reserved",
        "summary_tokens": (
            "full source-map publication remains reserved",
            "statement stepping",
        ),
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/text_collections_package.expectation.json",
            "tests/tooling/fixtures/developer_tooling/runtime_debug_trace/debug-map.json",
        ),
        "no_support_claims": True,
    },
    {
        "id": "runtime.debug-trace.statement-stepping",
        "state": "reserved",
        "summary_tokens": (
            "statement-level debugger stepping is fail-closed",
            "emitted on the canonical toolchain path",
        ),
        "evidence_paths": (
            "tests/tooling/fixtures/developer_tooling/runtime_debug_trace/debug-map.json",
        ),
        "no_support_claims": True,
    },
    {
        "id": "ecosystem.package-manager.public-hosted-registry",
        "state": "reserved",
        "summary_tokens": (
            "public hosted package registry service support remains reserved",
            "fallback registry success",
        ),
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/distribution_package_lifecycle.expectation.json",
        ),
        "no_support_claims": True,
    },
)

FOUNDATION_IMPLEMENTED_EVIDENCE_EXPECTATIONS = (
    {
        "id": "stdlib.text.runtime-builder-interpolation",
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/text_collections_package.expectation.json",
        ),
    },
    {
        "id": "language.collections.literal-syntax-runtime-backed",
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/text_collections_package.expectation.json",
        ),
    },
    {
        "id": "language.collections.for-in-syntax-runtime-backed",
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/text_collections_package.expectation.json",
        ),
    },
    {
        "id": "runtime.interop.package-loader-bridge",
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/text_collections_package.expectation.json",
        ),
    },
    {
        "id": "compiler.optimization.devirtualization",
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/optimization_runtime_equivalence.expectation.json",
        ),
    },
    {
        "id": "ecosystem.package-install.clean-distribution",
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/distribution_package_lifecycle.expectation.json",
        ),
    },
    {
        "id": "release.operations.channel-lifecycle",
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/distribution_package_lifecycle.expectation.json",
        ),
    },
    {
        "id": "conformance.public.stable-suite-manifest",
        "evidence_paths": (
            "tests/tooling/fixtures/cross_lane_e2e/text_collections_package.expectation.json",
            "tests/tooling/fixtures/cross_lane_e2e/optimization_runtime_equivalence.expectation.json",
            "tests/tooling/fixtures/cross_lane_e2e/advanced_runtime_closure.expectation.json",
            "tests/tooling/fixtures/cross_lane_e2e/distribution_package_lifecycle.expectation.json",
        ),
    },
)


def _require_matrix_shape(matrix: dict[str, Any]) -> list[dict[str, Any]]:
    capabilities = matrix.get("capabilities")
    if not isinstance(capabilities, list):
        raise CapabilityDocsError("capabilities must be a list")
    rows: list[dict[str, Any]] = []
    seen: set[str] = set()
    for index, row in enumerate(capabilities):
        if not isinstance(row, dict):
            raise CapabilityDocsError(f"capabilities[{index}] must be an object")
        capability_id = row.get("id")
        if not isinstance(capability_id, str):
            raise CapabilityDocsError(f"capabilities[{index}].id must be a string")
        if capability_id in seen:
            raise CapabilityDocsError(f"duplicate capability id: {capability_id}")
        seen.add(capability_id)
        rows.append(row)
    return rows


def _validate_evidence_rows(rows: list[dict[str, Any]]) -> None:
    for row in rows:
        capability_id = str(row["id"])
        state = str(row["state"])
        evidence = row["evidence"]
        if not isinstance(evidence, list) or not evidence:
            raise CapabilityDocsError(f"{capability_id} must have evidence")
        kinds = {str(item.get("kind")) for item in evidence if isinstance(item, dict)}
        if state == "implemented" and "test" not in kinds:
            raise CapabilityDocsError(f"{capability_id} implemented rows require test evidence")
        if state == "reserved" and kinds.isdisjoint({"diagnostic", "doc"}):
            raise CapabilityDocsError(f"{capability_id} reserved rows require diagnostic or doc evidence")
        for item in evidence:
            if not isinstance(item, dict):
                raise CapabilityDocsError(f"{capability_id} evidence entries must be objects")
            raw_path = item.get("path")
            if not isinstance(raw_path, str) or not raw_path:
                raise CapabilityDocsError(f"{capability_id} evidence path must be non-empty")
            path = resolve_repo_path(raw_path)
            if not path.exists():
                raise CapabilityDocsError(f"{capability_id} evidence path is missing: {raw_path}")


def _validate_object_model_scope(rows: list[dict[str, Any]]) -> None:
    for row in rows:
        capability_id = str(row["id"])
        state = str(row["state"])
        if (
            state != "implemented"
            or not capability_id.startswith(OBJECT_MODEL_IMPLEMENTED_PREFIX)
            or capability_id == OBJECT_MODEL_FULL_REALIZATION_ID
        ):
            continue

        claims = _row_support_claims(row)
        if not claims or any(
            not claim.startswith(OBJECT_MODEL_SUPPORT_CLAIM_PREFIX)
            for claim in claims
        ):
            raise CapabilityDocsError(
                f"{capability_id} object-model support claims must use "
                f"{OBJECT_MODEL_SUPPORT_CLAIM_PREFIX}*"
            )

        title = str(row.get("title", ""))
        summary = str(row.get("summary", ""))
        scope_text = f"{title} {summary}".lower()
        for phrase in OBJECT_MODEL_BROAD_SCOPE_PHRASES:
            if phrase in scope_text:
                raise CapabilityDocsError(
                    f"{capability_id} object-model implemented rows must stay narrow; "
                    f"broad scope phrase is reserved for {OBJECT_MODEL_FULL_REALIZATION_ID}: "
                    f"{phrase}"
                )

        evidence_paths = [
            str(item.get("path", ""))
            for item in row.get("evidence", [])
            if isinstance(item, dict)
        ]
        owner_paths = [
            str(path)
            for path in row.get("owner_modules", [])
            if isinstance(path, str)
        ]
        if not any(path.startswith(OBJECT_MODEL_RUNTIME_FIXTURE_PREFIX) for path in evidence_paths):
            raise CapabilityDocsError(
                f"{capability_id} object-model implemented rows require canonical "
                f"runtime object-model fixture evidence under "
                f"{OBJECT_MODEL_RUNTIME_FIXTURE_PREFIX}"
            )
        if not any(
            path.startswith(OBJECT_MODEL_RUNTIME_SOURCE_PREFIXES)
            for path in [*evidence_paths, *owner_paths]
        ):
            raise CapabilityDocsError(
                f"{capability_id} object-model implemented rows require "
                "runtime object-model source ownership under "
                + ", ".join(OBJECT_MODEL_RUNTIME_SOURCE_PREFIXES)
            )


def _evidence_paths(row: dict[str, Any]) -> set[str]:
    return {
        str(item.get("path"))
        for item in row.get("evidence", [])
        if isinstance(item, dict) and isinstance(item.get("path"), str)
    }


def _require_expected_evidence(
    *,
    row: dict[str, Any],
    expected_paths: tuple[str, ...],
) -> None:
    capability_id = str(row["id"])
    paths = _evidence_paths(row)
    missing = [path for path in expected_paths if path not in paths]
    if missing:
        raise CapabilityDocsError(
            f"{capability_id} is missing required boundary evidence: "
            + ", ".join(missing)
        )


def _validate_foundation_boundary_rows(rows: list[dict[str, Any]]) -> None:
    rows_by_id = {str(row["id"]): row for row in rows}

    for expected in FOUNDATION_BOUNDARY_EXPECTATIONS:
        capability_id = str(expected["id"])
        row = rows_by_id.get(capability_id)
        if row is None:
            raise CapabilityDocsError(f"missing capability boundary row: {capability_id}")
        expected_state = str(expected["state"])
        if row["state"] != expected_state:
            raise CapabilityDocsError(
                f"{capability_id} must remain {expected_state}, found {row['state']}"
            )
        if expected.get("no_support_claims") and _row_support_claims(row):
            raise CapabilityDocsError(f"{capability_id} must not publish support claims")
        summary = str(row.get("summary", "")).lower()
        missing_tokens = [
            token for token in expected["summary_tokens"] if token not in summary
        ]
        if missing_tokens:
            raise CapabilityDocsError(
                f"{capability_id} summary is missing boundary tokens: "
                + ", ".join(missing_tokens)
            )
        _require_expected_evidence(
            row=row,
            expected_paths=expected["evidence_paths"],
        )

    for expected in FOUNDATION_IMPLEMENTED_EVIDENCE_EXPECTATIONS:
        capability_id = str(expected["id"])
        row = rows_by_id.get(capability_id)
        if row is None:
            raise CapabilityDocsError(f"missing implemented capability row: {capability_id}")
        if row["state"] != "implemented":
            raise CapabilityDocsError(
                f"{capability_id} must remain implemented, found {row['state']}"
            )
        _require_expected_evidence(
            row=row,
            expected_paths=expected["evidence_paths"],
        )
