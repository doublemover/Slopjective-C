"""Drive a real owned clangd stdio session for source-navigation qualification."""
from __future__ import annotations

import json
import os
from pathlib import Path
import queue
import subprocess
import threading
import time
from typing import Any


def probe_positions(source_text: str, probe: dict[str, str]) -> tuple[dict[str, int], dict[str, int]]:
    """Locate one exact use/declaration anchor in the formatted source."""
    lines = source_text.splitlines()

    def locate(anchor_key: str, name_key: str) -> dict[str, int]:
        anchor = probe[anchor_key]
        name = probe[name_key]
        candidates = [index for index, line in enumerate(lines) if anchor in line]
        if len(candidates) != 1 or sum(line.count(anchor) for line in lines) != 1:
            raise RuntimeError(f"LSP probe {probe['name']} needs one {anchor_key} anchor: {anchor!r}")
        line = candidates[0]
        try:
            column = lines[line].index(name, lines[line].index(anchor))
        except ValueError as error:
            raise RuntimeError(f"LSP probe {probe['name']} anchor has no {name!r}") from error
        return {"line": line, "character": column}

    return locate("use_line", "use_name"), locate("declaration_line", "declaration_name")


def qualify_lsp(clangd: Path, source: Path, database_dir: Path,
                resource_dir: Path, work: Path,
                env: dict[str, str],
                probes: list[dict[str, str]] | None = None) -> dict[str, Any]:
    """Require clean diagnostics and ordinary/private navigation from the sealed server."""
    source_text = source.read_text(encoding="utf-8")
    navigation_probes = [{"name": "ordinary_parameter",
        "use_line": "return throws(value)", "use_name": "value",
        "declaration_line": "int async(int value)", "declaration_name": "value",
        "hover_type": "int", "completion_name": "value"}, *(probes or [])]
    probe_names: set[str] = set()
    for probe in navigation_probes:
        for key in ("name", "use_line", "use_name", "declaration_line", "declaration_name",
                    "hover_type", "completion_name"):
            if not isinstance(probe.get(key), str) or not probe[key]:
                raise RuntimeError(f"LSP probe has no nonempty {key}: {probe}")
        if probe["name"] in probe_names:
            raise RuntimeError(f"duplicate LSP probe name: {probe['name']}")
        probe_names.add(probe["name"])
    locations = {probe["name"]: probe_positions(source_text, probe)
                 for probe in navigation_probes}
    uri = source.as_uri()
    incoming: queue.Queue[Any] = queue.Queue()
    transcript: list[dict[str, Any]] = []
    stderr_path = work / ("clangd-lsp" + source.suffix + ".stderr.log")
    transcript_path = work / ("clangd-lsp" + source.suffix + ".json")
    command = [str(clangd), f"--compile-commands-dir={database_dir}",
               f"--resource-dir={resource_dir}", "--enable-config=0",
               "--background-index=0", "--log=verbose"]
    flags = subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0
    request_id = 0
    with stderr_path.open("wb") as stderr:
        process = subprocess.Popen(command, cwd=database_dir, env=env,
                                   stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=stderr, creationflags=flags)

        def read_messages() -> None:
            try:
                assert process.stdout is not None
                while True:
                    headers: dict[bytes, bytes] = {}
                    while True:
                        header = process.stdout.readline()
                        if not header:
                            incoming.put(RuntimeError("clangd closed its LSP stream"))
                            return
                        if header in (b"\r\n", b"\n"):
                            break
                        name, value = header.split(b":", 1)
                        headers[name.lower()] = value.strip()
                    length = int(headers[b"content-length"])
                    body = process.stdout.read(length)
                    if len(body) != length:
                        raise RuntimeError("partial clangd LSP message")
                    incoming.put(json.loads(body))
            except Exception as error:
                incoming.put(error)

        reader = threading.Thread(target=read_messages, daemon=True)
        reader.start()

        def send(message: dict[str, Any]) -> None:
            assert process.stdin is not None
            data = json.dumps(message, ensure_ascii=False).encode("utf-8")
            process.stdin.write(f"Content-Length: {len(data)}\r\n\r\n".encode() + data)
            process.stdin.flush()
            transcript.append({"direction": "client", "message": message})

        def receive(predicate, seconds: float = 25.0) -> dict[str, Any]:
            deadline = time.monotonic() + seconds
            while True:
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    raise RuntimeError("timed out waiting for clangd LSP response")
                try:
                    message = incoming.get(timeout=remaining)
                except queue.Empty as error:
                    raise RuntimeError("timed out waiting for clangd LSP response") from error
                if isinstance(message, Exception):
                    raise message
                transcript.append({"direction": "server", "message": message})
                if "method" in message and "id" in message:
                    # Advertised client capabilities require no server request.
                    send({"jsonrpc": "2.0", "id": message["id"], "error": {
                        "code": -32601, "message": "Unsupported qualification client method"}})
                if predicate(message):
                    return message

        def request(method: str, params: Any) -> Any:
            nonlocal request_id
            request_id += 1
            send({"jsonrpc": "2.0", "id": request_id, "method": method, "params": params})
            message = receive(lambda item: item.get("id") == request_id and "method" not in item)
            if "error" in message:
                raise RuntimeError(f"clangd {method} failed: {message['error']}")
            return message.get("result")

        try:
            initialized = request("initialize", {
                "processId": None, "rootUri": database_dir.as_uri(),
                "capabilities": {"textDocument": {
                    "hover": {"contentFormat": ["plaintext"]},
                    "completion": {"completionItem": {"snippetSupport": False}},
                    "definition": {"linkSupport": False}}}})
            if not initialized or not initialized.get("capabilities", {}).get("definitionProvider"):
                raise RuntimeError("clangd did not advertise native definition support")
            send({"jsonrpc": "2.0", "method": "initialized", "params": {}})
            send({"jsonrpc": "2.0", "method": "textDocument/didOpen", "params": {
                "textDocument": {"uri": uri, "languageId":
                    "objective-cpp" if source.suffix == ".mm" else "objective-c",
                    "version": 1, "text": source_text}}})
            published = receive(lambda item: item.get("method") == "textDocument/publishDiagnostics"
                                and item.get("params", {}).get("uri") == uri)
            diagnostics = published["params"].get("diagnostics", [])
            if any(item.get("severity", 1) == 1 for item in diagnostics):
                raise RuntimeError(f"clangd live diagnostics contain errors: {diagnostics}")
            navigation_results: dict[str, Any] = {}
            for probe in navigation_probes:
                name = probe["name"]
                use, declaration = locations[name]
                position = {"textDocument": {"uri": uri}, "position": use}
                hover = request("textDocument/hover", position)
                if not hover or probe["hover_type"] not in json.dumps(hover):
                    raise RuntimeError(f"clangd {name} hover lost {probe['hover_type']!r}: {hover}")
                definitions = request("textDocument/definition", position)
                if isinstance(definitions, dict):
                    definitions = [definitions]
                if not definitions or not any(
                        item.get("uri") == uri and
                        item.get("range", {}).get("start") == declaration
                        for item in definitions):
                    raise RuntimeError(f"clangd {name} definition did not resolve its declaration: {definitions}")
                completion = request("textDocument/completion", {
                    "textDocument": {"uri": uri},
                    "position": {"line": use["line"],
                                 "character": use["character"] + min(3, len(probe["use_name"]))},
                    "context": {"triggerKind": 1}})
                items = completion.get("items", []) if isinstance(completion, dict) else completion
                if not items or not any(
                        item.get("label", "").strip().split()[0] == probe["completion_name"]
                        for item in items if item.get("label", "").strip()):
                    raise RuntimeError(f"clangd {name} completion lost {probe['completion_name']!r}: {completion}")
                navigation_results[name] = {"position": use, "declaration": declaration,
                    "hover": hover, "definition": definitions, "completion": completion}
            request("shutdown", None)
            send({"jsonrpc": "2.0", "method": "exit", "params": None})
            process.wait(timeout=10)
            if process.returncode != 0:
                raise RuntimeError(f"clangd LSP shutdown exit {process.returncode}")
            ordinary = navigation_results["ordinary_parameter"]
            return {"command": command, "exit_code": process.returncode,
                    "diagnostics": diagnostics, "hover": ordinary["hover"],
                    "definition": ordinary["definition"], "completion": ordinary["completion"],
                    "navigation_probes": navigation_results,
                    "transcript": str(transcript_path), "stderr": str(stderr_path)}
        finally:
            if process.poll() is None:
                process.kill()
                process.wait(timeout=10)
            if process.stdin is not None:
                process.stdin.close()
            if process.stdout is not None:
                process.stdout.close()
            reader.join(timeout=1)
            transcript_path.write_text(json.dumps(transcript, indent=2) + "\n", encoding="utf-8")
