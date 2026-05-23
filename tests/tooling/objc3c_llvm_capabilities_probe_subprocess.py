from __future__ import annotations

from pathlib import Path
import subprocess


def fake_completed(
    command: list[str],
    *,
    returncode: int,
    stdout: str = "",
    stderr: str = "",
) -> subprocess.CompletedProcess[str]:
    return subprocess.CompletedProcess(command, returncode, stdout=stdout, stderr=stderr)


def fake_supported_secondary_tool_run(
    command: list[str],
) -> subprocess.CompletedProcess[str] | None:
    cmd = tuple(command)
    if cmd == ("clang++", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
    if cmd == ("llvm-ar", "--version"):
        return fake_completed(command, returncode=0, stdout="LLVM version 19.1.0\n")
    if cmd == ("llvm-config", "--version"):
        return fake_completed(command, returncode=0, stdout="19.1.0\n")
    if cmd == ("llvm-config", "--includedir"):
        return fake_completed(command, returncode=0, stdout="/opt/llvm/include\n")
    if cmd == ("llvm-config", "--libdir"):
        return fake_completed(command, returncode=0, stdout="/opt/llvm/lib\n")
    return None


def fake_capabilities_detected_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    secondary = fake_supported_secondary_tool_run(command)
    if secondary is not None:
        return secondary
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
    secondary = fake_supported_secondary_tool_run(command)
    if secondary is not None:
        return secondary
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
    secondary = fake_supported_secondary_tool_run(command)
    if secondary is not None:
        return secondary
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
    secondary = fake_supported_secondary_tool_run(command)
    if secondary is not None:
        return secondary
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
    secondary = fake_supported_secondary_tool_run(command)
    if secondary is not None:
        return secondary
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
    secondary = fake_supported_secondary_tool_run(command)
    if secondary is not None:
        return secondary
    cmd = tuple(command)
    if cmd == ("clang", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
    if cmd[0] == "llc":
        raise FileNotFoundError("llc not found")
    raise AssertionError(f"unexpected command: {command}")


def fake_llvm_ar_missing_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("llvm-ar", "--version"):
        return fake_completed(command, returncode=127, stderr="not found\n")
    secondary = fake_supported_secondary_tool_run(command)
    if secondary is not None:
        return secondary
    return fake_capabilities_detected_run(command)


def fake_llvm_config_headers_missing_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("llvm-config", "--includedir"):
        return fake_completed(command, returncode=0, stdout="")
    if cmd == ("llvm-config", "--libdir"):
        return fake_completed(command, returncode=1, stderr="libdir unavailable\n")
    secondary = fake_supported_secondary_tool_run(command)
    if secondary is not None:
        return secondary
    return fake_capabilities_detected_run(command)


def fake_mismatched_llvm_tool_versions_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    cmd = tuple(command)
    if cmd == ("clang", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 22.1.0\n")
    if cmd == ("clang++", "--version"):
        return fake_completed(command, returncode=0, stdout="clang version 22.1.0\n")
    if cmd == ("llc", "--version"):
        return fake_completed(command, returncode=0, stdout="LLVM version 21.1.0\n")
    if cmd == ("llvm-ar", "--version"):
        return fake_completed(command, returncode=0, stdout="LLVM version 22.1.0\n")
    if cmd == ("llvm-config", "--version"):
        return fake_completed(command, returncode=0, stdout="22.1.0\n")
    if cmd == ("llvm-config", "--includedir"):
        return fake_completed(command, returncode=0, stdout="/opt/llvm/include\n")
    if cmd == ("llvm-config", "--libdir"):
        return fake_completed(command, returncode=0, stdout="/opt/llvm/lib\n")
    if cmd == ("llc", "--help"):
        return fake_completed(command, returncode=0, stdout="--filetype=<type> ... obj ...\n")
    if cmd == ("llc", "--filetype=obj", "--version"):
        return fake_completed(command, returncode=0, stdout="LLVM version 21.1.0\n")
    raise AssertionError(f"unexpected command: {command}")


def fake_mixed_toolchain_root_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    tool_name = Path(command[0]).name.lower()
    option_tuple = tuple(command[1:])
    if tool_name in {"clang.exe", "clang++.exe"} and option_tuple == ("--version",):
        return fake_completed(command, returncode=0, stdout="clang version 22.1.0\n")
    if tool_name in {"llc.exe", "llvm-ar.exe"} and option_tuple == ("--version",):
        return fake_completed(command, returncode=0, stdout="LLVM version 22.1.0\n")
    if tool_name == "llvm-config.exe" and option_tuple == ("--version",):
        return fake_completed(command, returncode=0, stdout="22.1.0\n")
    if tool_name == "llvm-config.exe" and option_tuple == ("--includedir",):
        return fake_completed(command, returncode=0, stdout=str(Path(command[0]).parents[1] / "include") + "\n")
    if tool_name == "llvm-config.exe" and option_tuple == ("--libdir",):
        return fake_completed(command, returncode=0, stdout=str(Path(command[0]).parents[1] / "lib") + "\n")
    if tool_name == "llc.exe" and option_tuple == ("--help",):
        return fake_completed(command, returncode=0, stdout="--filetype=<type> ... obj ...\n")
    if tool_name == "llc.exe" and option_tuple == ("--filetype=obj", "--version"):
        return fake_completed(command, returncode=0, stdout="LLVM version 22.1.0\n")
    raise AssertionError(f"unexpected command: {command}")


def fake_windows_install_root_without_llvm_config_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    tool_name = Path(command[0]).name.lower()
    option_tuple = tuple(command[1:])
    if tool_name == "llvm-config.exe":
        return fake_completed(command, returncode=127, stderr="not found\n")
    if tool_name == "clang.exe" and option_tuple == ("--version",):
        return fake_completed(command, returncode=0, stdout="clang version 22.1.6\n")
    if tool_name == "clang++.exe" and option_tuple == ("--version",):
        return fake_completed(command, returncode=0, stdout="clang version 22.1.6\n")
    if tool_name == "llvm-ar.exe" and option_tuple == ("--version",):
        return fake_completed(command, returncode=0, stdout="LLVM version 22.1.6\n")
    if tool_name == "llc.exe" and option_tuple == ("--version",):
        return fake_completed(command, returncode=0, stdout="LLVM version 22.1.6\n")
    if tool_name == "llc.exe" and option_tuple == ("--help",):
        return fake_completed(command, returncode=0, stdout="--filetype=<type> ... obj ...\n")
    if tool_name == "llc.exe" and option_tuple == ("--filetype=obj", "--version"):
        return fake_completed(command, returncode=0, stdout="LLVM version 22.1.6\n")
    raise AssertionError(f"unexpected command: {command}")
