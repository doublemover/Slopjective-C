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
install_llvm() { :; }; install_ccache() { :; }
parallel_installs
''')
    assert result.returncode == 0, result.stderr
    assert result.stdout.count("Completed install_") == 4


def test_parallel_failure_is_reported_after_other_jobs_complete(tmp_path: Path) -> None:
    result = run_shell(tmp_path, '''
install_python() { echo broken >&2; return 7; }
install_node() { sleep 0.1; touch "$setup_root/node-complete"; }
install_llvm() { :; }; install_ccache() { :; }
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
