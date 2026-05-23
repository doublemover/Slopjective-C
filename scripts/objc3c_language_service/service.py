from __future__ import annotations

import hashlib
import json
from dataclasses import dataclass
from typing import Any


CONTRACT_ID = "objc3c.language_service.core.v1"
PUBLIC_ACTION_VERSION = "objc3c-language-service-actions-v1"


def stable_digest(payload: object) -> str:
    encoded = json.dumps(payload, sort_keys=True, separators=(",", ":")).encode(
        "utf-8"
    )
    return hashlib.sha256(encoded).hexdigest()


def _text_digest(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def _zero_range() -> dict[str, Any]:
    return {
        "start": {"line": 0, "character": 0},
        "end": {"line": 0, "character": 1},
    }


def _position_in_range(position: dict[str, Any], range_payload: dict[str, Any]) -> bool:
    line = int(position.get("line", 0) or 0)
    character = int(position.get("character", 0) or 0)
    start = range_payload.get("start", {})
    end = range_payload.get("end", {})
    start_key = (
        int(start.get("line", 0) or 0),
        int(start.get("character", 0) or 0),
    )
    end_key = (
        int(end.get("line", 0) or 0),
        int(end.get("character", 0) or 0),
    )
    return start_key <= (line, character) < end_key


def _request_id(request: dict[str, Any]) -> Any:
    return request.get("id") if "id" in request else None


@dataclass
class DocumentRecord:
    uri: str
    text: str
    version: int
    open: bool = True

    @property
    def digest(self) -> str:
        return _text_digest(self.text)


class ObjectiveC3LanguageService:
    """Deterministic request service over the compiler/editor source graph."""

    def __init__(
        self,
        *,
        source_graph: dict[str, Any],
        source_texts: dict[str, str] | None = None,
        workspace_files: list[str] | None = None,
        package_lock_manifest_digest: str = "",
        stdlib_digest: str = "",
        compiler_manifest_digest: str = "",
        capability_matrix_digest: str = "",
        public_action_version: str = PUBLIC_ACTION_VERSION,
    ) -> None:
        self.source_graph = source_graph
        self.source_graph_digest = str(source_graph.get("source_graph_digest", "") or "")
        self.compiler_manifest_digest = compiler_manifest_digest or stable_digest(
            source_graph.get("evidence", {}).get("manifest_path", "")
        )
        self.package_lock_manifest_digest = package_lock_manifest_digest or stable_digest(
            source_graph.get("package_provenance", {})
        )
        self.stdlib_digest = stdlib_digest or stable_digest(
            source_graph.get("package_provenance", {}).get("package_dependencies", [])
        )
        self.capability_matrix_digest = capability_matrix_digest or stable_digest(
            self._capability_matrix()
        )
        self.public_action_version = public_action_version
        self.documents: dict[str, DocumentRecord] = {}
        self.workspace_files: dict[str, dict[str, Any]] = {}
        self.invalidations: list[dict[str, Any]] = []

        primary_uri = str(source_graph.get("source_path", "") or "")
        for uri, text in sorted((source_texts or {}).items()):
            self.documents[uri] = DocumentRecord(uri=uri, text=text, version=1, open=False)
        if primary_uri and primary_uri not in self.documents:
            self.documents[primary_uri] = DocumentRecord(
                uri=primary_uri,
                text="",
                version=0,
                open=False,
            )
        file_uris = sorted(set([primary_uri, *(workspace_files or [])]))
        for uri in file_uris:
            if uri:
                self.workspace_files[uri] = {
                    "uri": uri,
                    "indexed": True,
                    "source": "source-graph-primary" if uri == primary_uri else "workspace-file",
                }

    def dispatch(self, request: dict[str, Any]) -> dict[str, Any]:
        method = str(request.get("method", "") or "")
        params = request.get("params", {})
        if not isinstance(params, dict):
            params = {}
        handlers = {
            "initialize": self.initialize,
            "shutdown": self.shutdown,
            "textDocument/didOpen": self.did_open,
            "textDocument/didChange": self.did_change,
            "textDocument/didClose": self.did_close,
            "textDocument/publishDiagnostics": self.publish_diagnostics,
            "textDocument/documentSymbol": self.document_symbols,
            "textDocument/definition": self.definition,
            "textDocument/hover": self.hover,
            "textDocument/semanticTokens/full": self.semantic_tokens,
            "textDocument/semanticTokens/range": self.semantic_tokens,
            "workspace/symbol": self.workspace_symbol,
            "workspace/didChangeWatchedFiles": self.did_change_watched_files,
            "textDocument/references": self.references,
            "textDocument/rename": self.rename,
            "textDocument/codeAction": self.code_action,
        }
        handler = handlers.get(method)
        result = handler(params) if handler is not None else self.unsupported(method, params)
        return {
            "jsonrpc": "2.0",
            "id": _request_id(request),
            "method": method,
            "result": result,
        }

    def initialize(self, _: dict[str, Any]) -> dict[str, Any]:
        capabilities = self._capability_matrix()
        return {
            "contract_id": CONTRACT_ID,
            "server_info": {
                "name": "objc3c-language-service",
                "version": "1",
            },
            "capabilities": capabilities,
            "source_graph_digest": self.source_graph_digest,
            "cache_key": self.cache_key(),
        }

    def shutdown(self, _: dict[str, Any]) -> dict[str, Any]:
        return {"ok": True, "cache_key": self.cache_key()}

    def did_open(self, params: dict[str, Any]) -> dict[str, Any]:
        text_document = self._text_document(params)
        uri = str(text_document.get("uri", "") or "")
        text = str(text_document.get("text", "") or "")
        version = int(text_document.get("version", 1) or 1)
        if uri:
            self.documents[uri] = DocumentRecord(uri=uri, text=text, version=version, open=True)
            self.workspace_files[uri] = {"uri": uri, "indexed": True, "source": "didOpen"}
            self._invalidate("document-open", uri)
        return self._document_result(uri)

    def did_change(self, params: dict[str, Any]) -> dict[str, Any]:
        text_document = self._text_document(params)
        uri = str(text_document.get("uri", "") or "")
        version = int(text_document.get("version", 0) or 0)
        changes = params.get("contentChanges", [])
        text = ""
        if isinstance(changes, list) and changes:
            last_change = changes[-1]
            if isinstance(last_change, dict):
                text = str(last_change.get("text", "") or "")
        if uri:
            previous = self.documents.get(uri)
            self.documents[uri] = DocumentRecord(
                uri=uri,
                text=text,
                version=version or ((previous.version + 1) if previous else 1),
                open=True,
            )
            self.workspace_files[uri] = {"uri": uri, "indexed": True, "source": "didChange"}
            self._invalidate("document-change", uri)
        return self._document_result(uri)

    def did_close(self, params: dict[str, Any]) -> dict[str, Any]:
        text_document = self._text_document(params)
        uri = str(text_document.get("uri", "") or "")
        if uri and uri in self.documents:
            self.documents[uri].open = False
            self._invalidate("document-close", uri)
        return self._document_result(uri)

    def publish_diagnostics(self, params: dict[str, Any]) -> dict[str, Any]:
        uri = self._uri(params)
        diagnostics = []
        for diagnostic in self.source_graph.get("diagnostics", []):
            if not isinstance(diagnostic, dict):
                continue
            diagnostics.append(
                {
                    "range": _zero_range(),
                    "severity": str(diagnostic.get("severity", "info") or "info"),
                    "code": str(diagnostic.get("code", "") or ""),
                    "source": str(diagnostic.get("source", "objc3c-source-graph") or ""),
                    "message": str(diagnostic.get("message", "") or ""),
                    "fail_closed": str(diagnostic.get("code", "") or "").startswith("O3SG_"),
                }
            )
        return {
            "uri": uri,
            "diagnostics": diagnostics,
            "diagnostic_count": len(diagnostics),
            "source_graph_digest": self.source_graph_digest,
            "cache_key": self.cache_key(uri),
        }

    def document_symbols(self, params: dict[str, Any]) -> dict[str, Any]:
        uri = self._uri(params)
        symbols = []
        for declaration in self._declarations_for_uri(uri):
            target = declaration.get("definition_target", {})
            symbols.append(
                {
                    "name": declaration.get("symbol", ""),
                    "kind": declaration.get("kind", ""),
                    "location": {
                        "uri": target.get("target_uri", uri),
                        "range": target.get("target_range", _zero_range()),
                    },
                    "selectionRange": target.get("target_range", _zero_range()),
                    "source": "source-graph-declaration-coordinate",
                }
            )
        return {
            "uri": uri,
            "symbols": symbols,
            "symbol_count": len(symbols),
            "cache_key": self.cache_key(uri),
        }

    def definition(self, params: dict[str, Any]) -> dict[str, Any]:
        uri = self._uri(params)
        declaration = self._declaration_at_position(uri, params.get("position", {}))
        if declaration is None:
            return self._fail_closed(
                "definition",
                "definition is only published from source-graph declaration coordinates; semantic reference closure is not available",
                uri=uri,
            )
        target = declaration.get("definition_target", {})
        return {
            "supported": True,
            "source": "source-graph-declaration-coordinate",
            "target": {
                "uri": target.get("target_uri", uri),
                "range": target.get("target_range", _zero_range()),
                "compiler_range": target.get("target_compiler_range", {}),
            },
            "cache_key": self.cache_key(uri),
        }

    def hover(self, params: dict[str, Any]) -> dict[str, Any]:
        uri = self._uri(params)
        declaration = self._declaration_at_position(uri, params.get("position", {}))
        if declaration is None:
            return self._fail_closed(
                "hover",
                "hover is only published from source-graph declaration coordinates until semantic references are emitted",
                uri=uri,
            )
        kind = str(declaration.get("kind", "") or "")
        symbol = str(declaration.get("symbol", "") or "")
        target = declaration.get("definition_target", {})
        return {
            "supported": True,
            "contents": {"kind": "plaintext", "value": f"{kind} {symbol}".strip()},
            "range": target.get("target_range", _zero_range()),
            "source": "source-graph-declaration-coordinate",
            "cache_key": self.cache_key(uri),
        }

    def workspace_symbol(self, params: dict[str, Any]) -> dict[str, Any]:
        query = str(params.get("query", "") or "").lower()
        symbols = []
        for node in self.source_graph.get("nodes", []):
            if not isinstance(node, dict):
                continue
            name = str(node.get("display_name", "") or "")
            if query and query not in name.lower():
                continue
            target = node.get("definition_target", {})
            symbols.append(
                {
                    "name": name,
                    "kind": str(node.get("symbol_kind", "") or ""),
                    "location": {
                        "uri": target.get("target_uri", node.get("location", {}).get("uri", "")),
                        "range": target.get("target_range", node.get("location", {}).get("range", _zero_range())),
                    },
                    "source": str(node.get("source_truth_kind", "") or "source-graph"),
                }
            )
        symbols = sorted(symbols, key=lambda symbol: (symbol["name"], symbol["kind"]))
        return {
            "query": query,
            "symbols": symbols,
            "symbol_count": len(symbols),
            "workspace_file_count": len(self.workspace_files),
            "cache_key": self.cache_key(),
        }

    def did_change_watched_files(self, params: dict[str, Any]) -> dict[str, Any]:
        changes = params.get("changes", [])
        indexed_changes = []
        if isinstance(changes, list):
            for change in changes:
                if not isinstance(change, dict):
                    continue
                uri = str(change.get("uri", "") or "")
                change_type = str(change.get("type", "") or "")
                if not uri:
                    continue
                if change_type in {"deleted", "3", "Delete"}:
                    self.workspace_files.pop(uri, None)
                else:
                    self.workspace_files[uri] = {
                        "uri": uri,
                        "indexed": True,
                        "source": "watched-file-change",
                        "change_type": change_type,
                    }
                indexed_changes.append({"uri": uri, "type": change_type})
                self._invalidate("workspace-file-change", uri)
        return {
            "changes": indexed_changes,
            "workspace_file_count": len(self.workspace_files),
            "workspace_root_digest": self.workspace_root_digest(),
            "cache_key": self.cache_key(),
        }

    def semantic_tokens(self, params: dict[str, Any]) -> dict[str, Any]:
        uri = self._uri(params)
        status = self.source_graph.get("semantic_tokens", {})
        if not isinstance(status, dict) or status.get("supported") is not True:
            return self._fail_closed(
                "semanticTokens",
                "full semantic token publication requires compiler-owned reference classification; declaration anchors remain non-authoritative for semantic coloring",
                uri=uri,
                extra={
                    "source_graph_state": status,
                    "token_anchor_count": int(self.source_graph.get("semantic_token_count", 0) or 0),
                },
            )
        return {
            "supported": True,
            "tokens": status.get("tokens", []),
            "legend": status.get("legend", {}),
            "cache_key": self.cache_key(uri),
        }

    def references(self, params: dict[str, Any]) -> dict[str, Any]:
        return self._fail_closed(
            "references",
            "references require compiler-owned semantic reference closure; lexical candidates are not authoritative",
            uri=self._uri(params),
            extra={"source_graph_state": self.source_graph.get("references", {})},
        )

    def rename(self, params: dict[str, Any]) -> dict[str, Any]:
        return self._fail_closed(
            "rename",
            "rename requires semantic reference closure, collision scopes, selector disambiguation, generated-accessor provenance, and package write-boundary proofs",
            uri=self._uri(params),
            extra={"source_graph_state": self.source_graph.get("rename", {})},
        )

    def code_action(self, params: dict[str, Any]) -> dict[str, Any]:
        return self._fail_closed(
            "codeAction",
            "code actions that edit source require diagnostic fix-it provenance plus package write-boundary proofs and collision-safe edit planning",
            uri=self._uri(params),
            extra={
                "required_proofs": [
                    "compiler-owned edit provenance",
                    "package write-boundary proof",
                    "collision-safe workspace edit plan",
                ]
            },
        )

    def unsupported(self, method: str, params: dict[str, Any]) -> dict[str, Any]:
        return self._fail_closed(
            method or "unknown",
            "request is not published by the Objective-C 3 language-service core",
            uri=self._uri(params),
        )

    def cache_key(self, uri: str = "") -> dict[str, Any]:
        document = self.documents.get(uri) if uri else None
        source_digest = document.digest if document else stable_digest(
            sorted((record.uri, record.digest) for record in self.documents.values())
        )
        document_version = document.version if document else max(
            [record.version for record in self.documents.values()] or [0]
        )
        return {
            "source_digest": source_digest,
            "document_version": document_version,
            "workspace_root_digest": self.workspace_root_digest(),
            "package_lock_manifest_digest": self.package_lock_manifest_digest,
            "stdlib_digest": self.stdlib_digest,
            "compiler_manifest_digest": self.compiler_manifest_digest,
            "capability_matrix_digest": self.capability_matrix_digest,
            "source_graph_digest": self.source_graph_digest,
            "public_action_version": self.public_action_version,
        }

    def workspace_root_digest(self) -> str:
        return stable_digest(self.workspace_files)

    def _capability_matrix(self) -> dict[str, Any]:
        semantic_tokens = self.source_graph.get("semantic_tokens", {})
        semantic_tokens_supported = (
            isinstance(semantic_tokens, dict) and semantic_tokens.get("supported") is True
        )
        return {
            "textDocumentSync": {"openClose": True, "change": "full"},
            "hoverProvider": True,
            "definitionProvider": True,
            "documentSymbolProvider": True,
            "workspaceSymbolProvider": True,
            "publishDiagnosticsProvider": True,
            "semanticTokensProvider": {
                "supported": semantic_tokens_supported,
                "fail_closed": not semantic_tokens_supported,
                "source": "source-graph",
            },
            "referencesProvider": False,
            "renameProvider": False,
            "codeActionProvider": False,
            "unsupported_request_policy": "stable fail-closed response payloads",
        }

    def _text_document(self, params: dict[str, Any]) -> dict[str, Any]:
        value = params.get("textDocument", {})
        return value if isinstance(value, dict) else {}

    def _uri(self, params: dict[str, Any]) -> str:
        text_document = self._text_document(params)
        return str(text_document.get("uri", "") or self.source_graph.get("source_path", "") or "")

    def _declarations_for_uri(self, uri: str) -> list[dict[str, Any]]:
        declarations = []
        for declaration in self.source_graph.get("declarations", []):
            if not isinstance(declaration, dict):
                continue
            target = declaration.get("definition_target", {})
            if not isinstance(target, dict):
                continue
            if str(target.get("target_uri", "") or "") == uri:
                declarations.append(declaration)
        return declarations

    def _declaration_at_position(
        self,
        uri: str,
        position: object,
    ) -> dict[str, Any] | None:
        if not isinstance(position, dict):
            return None
        for declaration in self._declarations_for_uri(uri):
            target = declaration.get("definition_target", {})
            if _position_in_range(position, target.get("target_range", {})):
                return declaration
        return None

    def _document_result(self, uri: str) -> dict[str, Any]:
        document = self.documents.get(uri)
        return {
            "uri": uri,
            "open": bool(document.open) if document else False,
            "version": int(document.version) if document else 0,
            "source_digest": document.digest if document else "",
            "workspace_file_count": len(self.workspace_files),
            "cache_key": self.cache_key(uri),
        }

    def _invalidate(self, reason: str, uri: str) -> None:
        self.invalidations.append(
            {
                "reason": reason,
                "uri": uri,
                "workspace_root_digest": self.workspace_root_digest(),
                "source_graph_digest": self.source_graph_digest,
            }
        )

    def _fail_closed(
        self,
        capability: str,
        reason: str,
        *,
        uri: str,
        extra: dict[str, Any] | None = None,
    ) -> dict[str, Any]:
        return {
            "supported": False,
            "fail_closed": True,
            "capability": capability,
            "reason": reason,
            "uri": uri,
            "cache_key": self.cache_key(uri),
            **(extra or {}),
        }
