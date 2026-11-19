from pathlib import Path
import subprocess
import sys

import pytest


@pytest.mark.parametrize('options', [[], ['-O'], ['-OO']])
def test_archive_debugger_check_requires_the_complete_transcript(tmp_path, options):
    root = Path(__file__).resolve().parents[2]
    script = (root / 'scripts/qualify_linux_archive.sh').read_text()
    check = script.split('python3 - "$scratch/debugger.log" <<\'PY\'\n', 1)[1].split('\nPY\n', 1)[0]
    transcript = tmp_path / 'debugger.log'
    expected = ['(int) cleanup_order = 0', '(int) $0 = 42', 'Process exited with status = 0']
    command = [sys.executable, *options, '-c', check, str(transcript)]
    for line in expected:
        transcript.write_text('\n'.join(item for item in expected if item != line))
        result = subprocess.run(command, capture_output=True, text=True, check=False)
        assert result.returncode != 0
        assert 'LLDB transcript is missing expected output' in result.stderr
    transcript.write_text('\n'.join(expected))
    subprocess.run(command, check=True)
