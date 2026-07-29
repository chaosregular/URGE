#!/usr/bin/env bash
# run_ca_batch.sh
# Usage: run_ca_batch.sh <path-to-executable> [args...]
# Example: run_ca_batch.sh ./universe_004_batch_04 --batch 137 137 200

set -euo pipefail

if [ $# -lt 1 ]; then
  echo "Usage: $0 <path-to-executable> [args...]"
  exit 2
fi

EXE="$1"
shift || true
ARGS=("$@")

TS=$(date -u +"%Y%m%dT%H%M%SZ")
GIT_SHA="unknown"
if git rev-parse --short HEAD >/dev/null 2>&1; then
  GIT_SHA=$(git rev-parse --short HEAD)
fi
HOST_INFO=$(uname -a)

OUTDIR="results/ca_run_${TS}_${GIT_SHA}"
mkdir -p "$OUTDIR"

MANIFEST="$OUTDIR/manifest.json"
LOGFILE="$OUTDIR/run.log"

# Save environment manifest
cat > "$MANIFEST" <<EOF
{
  "timestamp": "${TS}",
  "git_sha": "${GIT_SHA}",
  "host": "${HOST_INFO}",
  "command": "${EXE} ${ARGS[*]}",
  "cwd": "$(pwd)"
}
EOF

# Run the executable, saving stdout/stderr
echo "Running: ${EXE} ${ARGS[*]}"
"${EXE}" "${ARGS[@]}" 2>&1 | tee "$LOGFILE"

# After run, save a copy of the manifest into the run folder (already there) and print summary
echo "Run finished. Results in: ${OUTDIR}"

echo "NOTE: If the program produces snapshots, move or copy them to ${OUTDIR}/snapshots for analysis."
