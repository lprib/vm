import subprocess
from pathlib import Path

exe = Path(__file__ + "/../../../out/integration_exe/integration_exe").resolve()
assert exe.exists()
assert exe.is_file()

expected_out = open(Path(__file__).parent / "expected.txt").read()
out = subprocess.run(f"{exe}", capture_output=True, text=True).stdout

assert expected_out == out

print("Integration test passed")