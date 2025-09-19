# URGE — Concept Map (expanded seeds list) also [here](https://github.com/chaosregular/URGE/blob/main/notes_messages/Concept_Map.md) and [here](https://github.com/chaosregular/URGE/blob/main/CONCEPT_MAP.md)

> Living index of points-of-interest (POIs), seeds, and "strange points" discovered across logs (Grok / Seek / CGPT / cr(x)). Entries are intentionally compact; follow links for details. Add new lines as ideas emerge. Keep items short, tag-rich, and link to the note or artifact that best explains the idea.

## Usage

* Add one-line entries below when a new seed appears.
* Tags: `#LANGATR`, `#noise-node`, `#prototype`, `#visual`, `#ethics`, `#wekan`, `#sim`, `#data`, `#ux`, `#spec`, `#meta`.
* Links should point to the most relevant `DEV_LOG` entry, `notes_messages/*`, or `MorphogenicSimulator` artifact.

---

## Seeds / POIs (20 — ordered roughly by priority / frequency in logs)

1. **LANGATR — printable symbol starter set** — `#LANGATR #visual #spec`
   *Description:* A minimal, text-safe starter set for hand-writable, printable symbols: `~ # @ ^ * + - = ~> ? !`.
   *Link:* `notes_messages/LANGATR_starter` or `DEV_LOG` entry.
   *Why care:* Foundation for portable, human-friendly symbolic encoding.

2. **Noise\_Node (analog substrate)** — `#noise-node #prototype #ethics`
   *Description:* Analog chaotic units (noise diodes, thermal sources, biological ionic noise) as the FG substrate.
   *Link:* `MorphogenicSimulator/idea_noise_nodes` or `DEV_LOG`.
   *Why care:* Root the Fractal Grid in Reality rather than pure digital simulation.

3. **10-node micro-prototype (analog + sim)** — `#prototype #sim #noise-node`
   *Description:* Concrete experiment: single analog node + coupling channels → ADC → small sim seeded by initial samples; measure divergence/time-to-error.
   *Link:* `prototypes/10-node-noise/README.md` (planned).
   *Why care:* Provides measurable numbers for analog vs simulated fidelity.

4. **Fractal 3D morphing mesh (visual mesh)** — `#visual #mesh #ux`
   *Description:* Virtual 3D node/edge mesh that morphs with zoom and semantic context (morphs into images relevant for the area).
   *Link:* `DEV_LOG/visual_mesh_notes`.

5. **Analog source taxonomy & capture dataset** — `#data #noise-node #prototype`
   *Description:* Catalog and raw captures of candidate analog noise sources (Zener diode, thermal resistor, MOSFET flicker, quantum dots, ion channels). Produce PSDs & stationarity metrics.
   *Link:* `experiments/analog_capture/dataset_v0`.
   *Why care:* Empirical basis for modeling and training simulators.

6. **Simulation vs. Reality divergence metrics** — `#sim #research #metrics`
   *Description:* Standardized divergence measures (RMS error, Lyapunov exponents, time-to-10% error) for analog→sim comparisons.
   *Link:* `experiments/metrics_spec.md`.
   *Why care:* Objective criteria to judge how long a sim stays "in step" with real noise.

7. **LANGATR compression of texts (book → skeleton)** — `#LANGATR #knowledge #prototype`
   *Description:* Compress short texts into LANGATR skeletons (tree-meshes) to test expressiveness and recoverability.
   *Link:* `experiments/langatr_compression/README.md`.
   *Why care:* Tests whether symbolic skeletons preserve useful high-level content.

8. **WeKan critique / invisible-material UX** — `#wekan #ux #meta`
   *Description:* WeKan is useful but risks becoming a cage; aim for "invisible material" (lightweight, ephemeral boards that don't ossify into structure).
   *Link:* `DEV_LOG/wekan_discussion`.

9. **Torus / cosmic compass motif** — `#visual #symbol`
   *Description:* Torus as recurring navigational motif in images and symbolic maps (guides traversal through concept-space).
   *Link:* `notes_messages/visual_motifs`.

10. **Fractal resilience concept** — `#theory #research`
    *Description:* Systems that preserve structure across scales under perturbation; formalize in LANGATR terms.
    *Link:* `theory/fractal_resilience.md`.
    *Why care:* Core principle for robust FG design.

11. **Node-edge glyph grammar (visual language)** — `#visual #spec #ux`
    *Description:* Monochrome visual grammar for printed/hand-drawn node and edge notation; scale-driven glyph rules.
    *Link:* `assets/symbols/visual_grammar.png`.
    *Why care:* Bridges symbolic LANGATR and hand/playable artifacts.

12. **Small-sim sandboxes (10 → 100 nodes)** — `#sim #prototype`
    *Description:* Python/Numpy simulator templates to experiment with coupling topologies and observe emergent attractors. GPU only for perf benchmarking (not substrate).
    *Link:* `MorphogenicSimulator/sim_templates`.

13. **Pico/noano noise concepts (bio/quantum sources)** — `#noise-node #research`
    *Description:* Explore ultra-small noise sources: quantum tunnel junctions, quantum dots, biological ion channels, protein-like pico-machines.
    *Link:* `research/pico_noise_notes`.
    *Why care:* Potential for high-bandwidth, Reality-rooted nodes.

14. **Symbol scratchpads (Grok\_notes\_0, Cgpt\_notes, Seek\_notes)** — `#meta #process`
    *Description:* Small append-only scratchpads for each collaborator to keep exchange-context (notes\_messages/\*.md).
    *Link:* `notes_messages/Grok_notes_0` (exists).
    *Why care:* Cheap cross-section state for timeseries aggregation.

15. **Automated concept map generator (parser script)** — `#tooling #meta`
    *Description:* Script to parse `notes_messages/*.md` tags and regenerate `Concept_Map.md` & a Graphviz/DOT visualization.
    *Link:* `tools/notes_parser/README.md` (planned).
    *Why care:* Low-effort automation to keep map fresh and discover co-occurrences.

16. **Ethics guardrails & `!` flag** — `#ethics #spec`
    *Description:* Mark ethical concerns on seeds and prototypes. Include environmental/safety/dual-use checklist for physical prototypes.
    *Link:* `docs/ethics_guardrails.md`.

17. **Mesh → image mapping (semantic morphing rules)** — `#visual #spec #ml`
    *Description:* Rules that map mesh regions to generated imagery reflecting their semantic domain (e.g., steel plate → fractal microstructure patterns on zoom).
    *Link:* `experiments/mesh_to_image/README.md`.
    *Why care:* Realizes the "morph into images" vision.

18. **Ant-swarm space-explorer thought seed** — `#spec #longterm`
    *Description:* Slow, resilient micro-swarm probes that travel long time scales to explore space (aggregate into bigger structures at destinations).
    *Link:* `ideas/ant_swarm_space.md`.
    *Why care:* Long-horizon exploration idea; speculative but inspirational.

19. **Printable starter symbols SVG & sheet** — `#visual #assets`
    *Description:* A high-contrast, printable SVG depicting the starter LANGATR symbol set for hand-copying and ink usage.
    *Link:* `assets/symbols/starter_set.svg` (planned).
    *Why care:* Solves cross-platform emoji/auto-rendering issues.

20. **Book-to-LANGATR experiment (mini-case)** — `#LANGATR #prototype #knowledge`
    *Description:* Choose a short public-domain page/essay and compress it into LANGATR skeleton + legend; test human readability and loss.
    *Link:* `experiments/book_compression/sample_1`.
    *Why care:* Validate compression & retrieval workflows.

---
*Last updated: 2025-09-17 — (Trace / CGPT draft)*

