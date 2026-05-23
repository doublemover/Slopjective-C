from __future__ import annotations

from typing import Any

from .service import CONTRACT_ID, ObjectiveC3LanguageService


def replay_requests(
    service: ObjectiveC3LanguageService,
    requests: list[dict[str, Any]],
) -> dict[str, Any]:
    responses = [service.dispatch(request) for request in requests]
    unsupported_methods = []
    for response in responses:
        result = response.get("result", {})
        if isinstance(result, dict) and result.get("fail_closed") is True:
            unsupported_methods.append(str(response.get("method", "")))
    return {
        "contract_id": "objc3c.language_service.replay.v1",
        "service_contract_id": CONTRACT_ID,
        "request_count": len(requests),
        "response_count": len(responses),
        "responses": responses,
        "unsupported_methods": sorted(set(unsupported_methods)),
        "cache_invalidations": service.invalidations,
        "cache_invalidation_count": len(service.invalidations),
        "workspace_file_count": len(service.workspace_files),
        "workspace_files": sorted(service.workspace_files.values(), key=lambda row: row["uri"]),
        "final_cache_key": service.cache_key(),
    }
