from __future__ import annotations

import subprocess


def fake_completed(
    command: list[str],
    *,
    returncode: int,
    stdout: str = "",
    stderr: str = "",
) -> subprocess.CompletedProcess[str]:
    return subprocess.CompletedProcess(command, returncode, stdout=stdout, stderr=stderr)


def fake_capabilities_detected_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("clang", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
    if cmd == ("llc", "--version"):
        return fake_completed(
            command,
            returncode=0,
            stdout="Debian LLVM version 19.1.0\n",
        )
    if cmd == ("llc", "--help"):
        return fake_completed(
            command,
            returncode=0,
            stdout="--filetype=<type> ... obj ...\n",
        )
    if cmd == ("llc", "--filetype=obj", "--version"):
        return fake_completed(
            command,
            returncode=0,
            stdout="Debian LLVM version 19.1.0\n",
        )
    raise AssertionError(f"unexpected command: {command}")


def fake_llc_missing_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("clang", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
    if cmd == ("llc", "--version"):
        return fake_completed(command, returncode=127, stderr="not found\n")
    raise AssertionError(f"unexpected command: {command}")


def fake_filetype_command_probe_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("clang", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
    if cmd == ("llc", "--version"):
        return fake_completed(
            command,
            returncode=0,
            stdout="Debian LLVM version 19.1.0\n",
        )
    if cmd == ("llc", "--help"):
        return fake_completed(
            command,
            returncode=0,
            stdout="llc help without obj token\n",
        )
    if cmd == ("llc", "--filetype=obj", "--version"):
        return fake_completed(
            command,
            returncode=0,
            stdout="Debian LLVM version 19.1.0\n",
        )
    raise AssertionError(f"unexpected command: {command}")


def fake_llc_filetype_unsupported_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("clang", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
    if cmd == ("llc", "--version"):
        return fake_completed(
            command,
            returncode=0,
            stdout="Debian LLVM version 19.1.0\n",
        )
    if cmd == ("llc", "--help"):
        return fake_completed(
            command,
            returncode=0,
            stdout="llc help without object filetype support\n",
        )
    if cmd == ("llc", "--filetype=obj", "--version"):
        return fake_completed(
            command,
            returncode=1,
            stderr="unknown option --filetype=obj\n",
        )
    raise AssertionError(f"unexpected command: {command}")


def fake_clang_missing_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("clang", "--version"):
        return fake_completed(command, returncode=127, stderr="not found\n")
    if cmd == ("llc", "--version"):
        return fake_completed(
            command,
            returncode=0,
            stdout="Debian LLVM version 19.1.0\n",
        )
    if cmd == ("llc", "--help"):
        return fake_completed(
            command,
            returncode=0,
            stdout="--filetype=<type> ... obj ...\n",
        )
    if cmd == ("llc", "--filetype=obj", "--version"):
        return fake_completed(
            command,
            returncode=0,
            stdout="Debian LLVM version 19.1.0\n",
        )
    raise AssertionError(f"unexpected command: {command}")


def fake_llc_launch_file_not_found_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("clang", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
    if cmd[0] == "llc":
        raise FileNotFoundError("llc not found")
    raise AssertionError(f"unexpected command: {command}")
