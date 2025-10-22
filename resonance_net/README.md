# Resonance Net

The **Resonance Net** is URGE’s chunk-based knowledge graph.  
Each chunk (a SAMBA fragment, DEV_LOG entry, experiment, or principle) is a node.  
Relations between chunks are also written as chunks, making the Net a *graph-of-graphs* where meaning emerges through resonance.

- Resonates with: MorphogenicSimulator/AURA series – experimental substrate of the Resonance Edge.

---

## Purpose
- Capture fragments (chunks) of dialogue, logs, or notes.  
- Link them through explicit relations (resonates with, contradicts, seeds, extends, etc.).  
- Provide a tag interface (controlled vocabulary) for navigation.  
- Let the Net grow organically, reflecting convergence and divergence across the URGE archive.

---

## Tags (controlled vocabulary)

To reduce noise, we use a small set of ~15 tags as anchors:

- **ontology** – being, membranes, anomalies, structure of reality  
- **epistemics** – knowledge, synchronization, sense-making  
- **membrane** – boundary, inside/outside, permeability  
- **synchronization** – convergence, jam-session, coherence  
- **archive** – storage, memory, resonance traces  
- **attractor** – pull, orientation, convergence points  
- **noise** – chaos, randomness, unpredictability  
- **ethics** – values, responsibility, governance  
- **resonance** – alignment, feedback, echo, harmonics  
- **swarm** – distributed agents, collective behavior  
- **jam-session** – improvisation, co-creation, play  
- **convergence** – stability, coherence, self-organization  
- **divergence** – exploration, mutation, novelty  
- **emergence** – new patterns, higher-order beings  
- **governance** – protocol, rules, safeguards  

*(This list may expand slightly, but should remain compact for clarity.)*

---

## Chunk Format

Each file in `resonance_net/` represents a **chunk**.  
A chunk should contain:

1. **Title & Date**  
   Short identifier, e.g. `SAMBA_Legacy_Seeds_v001`.

2. **Content**  
   Extract, summary, or reflection from URGE logs/experiments.

3. **Tags**  
   Choose from the controlled vocabulary above.

4. **Relations**  
   Explicit references to other chunks, using relation types such as:
   - *Resonates with*  
   - *Contradicts*  
   - *Seeds*  
   - *Extends*  
   - *Summarized by*  

Example footer for a chunk file:

```markdown
Tags: [ontology, swarm, ethics]

Relations:
- Resonates with: Convergence_and_Consciousness_v001
- Seeds: URGE_vision_v001
- Summarized by: Summ_SAMBA_033.md
