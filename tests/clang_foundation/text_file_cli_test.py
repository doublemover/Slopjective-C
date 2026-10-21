"""Exercise the native application from a directory outside its source tree."""
from pathlib import Path
import subprocess
import sys
import tempfile

application = Path(sys.argv[1]).resolve(strict=True)
with tempfile.TemporaryDirectory(prefix="objc3-text-application-") as work:
    root = Path(work)
    cases = (
        ("unicode.txt", "caf\u00e9:3\n\U0001f642:4\nzero:0\n".encode(), 0, "records=3 sum=7\n", ""),
        ("empty.txt", b"", 0, "records=0 sum=0\n", ""),
        ("signed.txt", b"a:9223372036854775807\r\nb:-9223372036854775808\r\n", 0, "records=2 sum=-1\n", ""),
        ("no-final-newline.txt", "\u96ea:-7".encode(), 0, "records=1 sum=-7\n", ""),
        ("nul-label.txt", b"a\0b:3\n", 0, "records=1 sum=3\n", ""),
        ("many-records.txt", b"a:1\n" * 4096, 0, "records=4096 sum=4096\n", ""),
        ("capacity.txt", b"a" * ((1 << 20) - 3) + b":0\n", 0, "records=1 sum=0\n", ""),
        ("too-large.txt", b"a" * (1 << 20) + b"a", 8, "", "kind=input_too_large service_status=8"),
        ("malformed.txt", b"\xff", 5, "", "kind=invalid_utf8 service_status=5"),
        ("truncated-utf8.txt", b"a:1\n\xf0\x9f\x99", 5, "", "kind=invalid_utf8 service_status=5"),
        ("invalid-record.txt", b"a:3:4\n", 65, "", "kind=invalid_record app_status=65"),
        ("empty-label.txt", b":3\n", 65, "", "kind=invalid_record app_status=65"),
        ("empty-number.txt", b"a:\n", 65, "", "kind=invalid_record app_status=65"),
        ("empty-record.txt", b"a:1\n\n", 65, "", "kind=empty_record app_status=65"),
        ("integer-range.txt", b"a:9223372036854775808\n", 65, "", "kind=invalid_integer app_status=65"),
        ("trailing-integer-junk.txt", b"a:3x\n", 65, "", "kind=invalid_integer app_status=65"),
        ("nul-number.txt", b"a:3\0\n", 65, "", "kind=invalid_integer app_status=65"),
        ("overflow.txt", b"a:9223372036854775807\nb:1\n", 7, "", "kind=aggregate_overflow service_status=7"),
        ("underflow.txt", b"a:-9223372036854775808\nb:-1\n", 7, "", "kind=aggregate_overflow service_status=7"),
        ("missing.txt", None, 66, "", "kind=io app_status=66"),
    )
    for name, contents, code, output, diagnostic in cases:
        path = root / name
        if contents is not None:
            path.write_bytes(contents)
        result = subprocess.run([str(application), str(path)], cwd=root,
                                text=True, capture_output=True, timeout=10)
        assert result.returncode == code, (name, result.returncode, result.stderr)
        assert result.stdout == output, (name, result.stdout)
        assert diagnostic in result.stderr, (name, result.stderr)
        if not diagnostic:
            assert not result.stderr, (name, result.stderr)
    usage = subprocess.run([str(application)], cwd=root, capture_output=True,
                           text=True, timeout=10)
    assert usage.returncode == 64 and "kind=usage app_status=64" in usage.stderr
    assert not usage.stdout
print(f"Text application passed {len(cases) + 1} input/error cases outside the source tree")
