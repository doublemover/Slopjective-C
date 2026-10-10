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


def qualify_lsp(clangd: Path, source: Path, database_dir: Path,
                resource_dir: Path, work: Path,
                env: dict[str, str]) -> dict[str, Any]:
    """Require clean diagnostics, hover and definition from the sealed server."""
    source_text = source.read_text(encoding="utf-8")
    lines = source_text.splitlines()
    line = next(i for i, value in enumerate(lines) if "return throws(value)" in value)
    column = lines[line].index("value", lines[line].index("return"))
    declaration_column = lines[line].index("value")
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
            position = {"textDocument": {"uri": uri},
                        "position": {"line": line, "character": column}}
            hover = request("textDocument/hover", position)
            if not hover or "int" not in json.dumps(hover):
                raise RuntimeError(f"clangd hover lost native int type: {hover}")
            definitions = request("textDocument/definition", position)
            if isinstance(definitions, dict):
                definitions = [definitions]
            if not definitions or not any(
                    item.get("uri") == uri and
                    item.get("range", {}).get("start") == {
                        "line": line, "character": declaration_column}
                    for item in definitions):
                raise RuntimeError(f"clangd definition did not resolve the actual parameter: {definitions}")
            completion = request("textDocument/completion", {
                "textDocument": {"uri": uri},
                "position": {"line": line, "character": column + 3},
                "context": {"triggerKind": 1}})
            items = completion.get("items", []) if isinstance(completion, dict) else completion
            if not items or not any(item.get("label", "").strip().split()[0] == "value"
                                    for item in items if item.get("label", "").strip()):
                raise RuntimeError(f"clangd completion did not include the native parameter: {completion}")
            request("shutdown", None)
            send({"jsonrpc": "2.0", "method": "exit", "params": None})
            process.wait(timeout=10)
            if process.returncode != 0:
                raise RuntimeError(f"clangd LSP shutdown exit {process.returncode}")
            return {"command": command, "exit_code": process.returncode,
                    "diagnostics": diagnostics, "hover": hover, "definition": definitions,
                    "completion": completion,
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
