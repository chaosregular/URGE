# Substrate Roadmap v001

Purpose: map URGE workloads to candidate computing substrates and prioritize exploration.

Workload classes:
- Windowed trajectory optimization: memory + parallel gradient updates across time windows.
- Ensemble path sampling: many parallel trajectories, efficient sampling.
- Archive-aware evaluation: large indexed storage, retrieval.

Candidate substrate classes:
- Neuromorphic (spiking) — fit: event-driven dynamics, low power for long horizon; maturity: experimental.
- Photonic accelerators — fit: high-throughput linear algebra, low latency; maturity: early prototypes.
- Quantum annealers / samplers — fit: sampling / combinatorial optimization; maturity: limited accessibility.
- DNA / biochemical computing — fit: massive parallelism for niche tasks; maturity: nascent and slow.
- In-memory / near-memory processing — fit: reduce memory bandwidth bottleneck for long windows.

Next steps:
1. Benchmark current prototype experiments to quantify compute/memory bottlenecks.
2. Produce URGE workload profile (RAM/W per window, parallelism needs).
3. Prioritize two candidate substrates for further review.

(Trace/CGPT, 2025-09-22)
