# CA tools for experiments/toys/CA_4nPCA_012

This small toolbox contains helper scripts and example configs to run batch experiments and perform lightweight analysis of snapshots produced by the CA / nParticles simulation in experiments/toys/.

Purpose
- Provide a reproducible wrapper for running batch experiments (captures git SHA, host info and command-line)
- Provide a minimal Python-based snapshot analyzer to extract simple visual features (histograms, entropy, edge density)
- Provide a small chunk generator (markdown) to help publishing experiment artefacts into URGE

Included files
- run_ca_batch.sh — wrapper that runs the CA binary in batch mode, captures provenance and saves stdout to run.log
- example_experiment.cfg — sample key=value config file
- analyze_snapshots.py — minimal snapshot feature extractor (depends on Python + Pillow + numpy)
- generate_chunk.py — small script to create a markdown summary linking results
- Makefile — convenience for building and running

Usage
1) Build the simulation (from repo root):
   make -C experiments/toys/ca_tools build

2) Run a batch experiment (example):
   experiments/toys/ca_tools/run_ca_batch.sh ./universe_004_batch_04 --batch 137 137 200

3) After run finishes, use the analyzer on the snapshots directory (if you enabled snapshoting):
   python3 experiments/toys/ca_tools/analyze_snapshots.py results/<your-run-dir>/snapshots

Notes
- The wrapper is intentionally non-invasive: it doesn't change the simulation source code. It captures environment metadata and stores it next to results for provenance.
- For heavier image analysis (CNN embeddings, clustering) see the suggestions in the repo README or ask me to add them.
