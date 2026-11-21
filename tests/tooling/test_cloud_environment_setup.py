from __future__ import annotations

from pathlib import Path
import subprocess

import pytest

SCRIPT = Path(__file__).resolve().parents[2] / "scripts/setup_cloud_environment.sh"


def run_shell(tmp_path: Path, body: str) -> subprocess.CompletedProcess[str]:
    (tmp_path / "logs").mkdir(exist_ok=True)
    return subprocess.run(
        ["bash", "-c", 'source "$1"; setup_root="$2"; repo_root="$2"; ' + body,
         "test", str(SCRIPT), str(tmp_path)],
        text=True, capture_output=True, timeout=10,
    )


def test_installers_overlap_and_wait_for_each_other(tmp_path: Path) -> None:
    result = run_shell(tmp_path, '''
install_python() {
  touch "$setup_root/python-started"
  for ((i=0;i<100;i++)); do
    [[ -f "$setup_root/node-started" ]] && return 0
    sleep 0.01
  done
  return 1
}
install_node() { test -f "$setup_root/python-started" || sleep 0.1; touch "$setup_root/node-started"; }
install_llvm() { :; }; install_ccache() { :; }; install_cmake() { :; }
parallel_installs
''')
    assert result.returncode == 0, result.stderr
    assert result.stdout.count("Completed install_") == 5


def test_parallel_failure_is_reported_after_other_jobs_complete(tmp_path: Path) -> None:
    result = run_shell(tmp_path, '''
install_python() { echo broken >&2; return 7; }
install_node() { sleep 0.1; touch "$setup_root/node-complete"; }
install_llvm() { :; }; install_ccache() { :; }; install_cmake() { :; }
parallel_installs
''')
    assert result.returncode == 1
    assert "Failed install_python" in result.stderr
    assert "broken" in result.stderr
    assert (tmp_path / "node-complete").exists()


@pytest.mark.parametrize("free,tmp_size,expected", [(4096,8192,0), (4095,0,1), (8192,8193,1)])
def test_disk_reserve_and_temp_limits(tmp_path: Path, free: int, tmp_size: int, expected: int) -> None:
    (tmp_path / "tmp").mkdir()
    result = run_shell(tmp_path, f'''
df() {{ printf 'Filesystem 1M-blocks Used Available Use%% Mounted\nvolume 32768 0 {free} 0%% /workspace\n'; }}
du() {{ case "${{@: -1}}" in */tmp) printf '{tmp_size}\\t%s\\n' "${{@: -1}}";; *) printf '0\\t%s\\n' "${{@: -1}}";; esac; }}
check_disk_budget
''')
    assert result.returncode == expected


def test_download_rejects_checksum_mismatch(tmp_path: Path) -> None:
    result = run_shell(tmp_path, '''
curl() { printf corrupt > "${@: -1}"; }
download_verified https://example.invalid/archive "$setup_root/archive" "$(printf '%064d' 0)"
''')
    assert result.returncode != 0
    assert "FAILED" in result.stdout


@pytest.mark.parametrize("fallback_fails,corrupt", [(False, False), (True, False), (False, True)])
def test_release_download_fallback_still_requires_exact_bytes(
    tmp_path: Path, fallback_fails: bool, corrupt: bool,
) -> None:
    import hashlib

    digest = hashlib.sha256(b"verified-release").hexdigest()
    result = run_shell(tmp_path, f'''
curl() {{
  printf '%s\\n' "$*" >> "$setup_root/downloads"
  if [[ " $* " == *' https://example.invalid/release '* ]]; then
    printf partial > "${{@: -1}}"
    return 22
  fi
  {'return 22' if fallback_fails else 'printf ' + ('corrupt' if corrupt else 'verified-release') + ' > "${@: -1}"'}
}}
download_verified https://example.invalid/release "$setup_root/archive" {digest} https://api.github.com/repos/example/release/releases/assets/123
''')
    assert result.returncode == (1 if fallback_fails or corrupt else 0), result.stderr
    attempts = (tmp_path / "downloads").read_text().splitlines()
    assert len(attempts) == 2
    assert "Accept: application/octet-stream" in attempts[1]
    assert "--max-filesize 2300000000" in attempts[1]


def test_checksum_failure_does_not_try_another_source(tmp_path: Path) -> None:
    result = run_shell(tmp_path, '''
curl() { printf '%s\\n' "$*" >> "$setup_root/downloads"; printf corrupt > "${@: -1}"; }
download_verified https://example.invalid/release "$setup_root/archive" "$(printf '%064d' 0)" https://api.github.com/repos/example/release/releases/assets/123
''')
    assert result.returncode != 0
    assert len((tmp_path / "downloads").read_text().splitlines()) == 1


def test_bootstrap_pruning_removes_unselected_independent_llvm_products(tmp_path: Path) -> None:
    llvm = tmp_path / 'llvm'
    removed = ('bin/flang-23', 'bin/mlir-opt', 'bin/llvm-bolt', 'lib/libFortranSemantics.a',
               'lib/libMLIRIR.a', 'lib/libLLVMBOLTPasses.a', 'include/flang/header.h',
               'include/mlir/header.h', 'lib/cmake/flang/FlangConfig.cmake',
               'lib/cmake/mlir/MLIRConfig.cmake')
    retained = ('bin/clang-23', 'bin/llc', 'bin/opt', 'bin/llvm-reduce', 'bin/lldb',
                'lib/libclang-cpp.so.23.1', 'lib/libLLVM.so.23.1',
                'lib/cmake/llvm/LLVMConfig.cmake', 'lib/cmake/clang/ClangConfig.cmake')
    for relative in (*removed, *retained):
        path = llvm / relative;path.parent.mkdir(parents=True, exist_ok=True);path.touch()
    result = run_shell(tmp_path, f'''prune_llvm "{llvm}"''')
    assert result.returncode == 0, result.stderr
    assert not any((llvm / relative).exists() for relative in removed)
    assert all((llvm / relative).exists() for relative in retained)
    assert 'Flang, MLIR/FIR and BOLT removed' in (llvm / '.objc3c-pruned-components').read_text()
