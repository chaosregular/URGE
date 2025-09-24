File: experiments/GenesisExperiment/prompts/synthesis_vision_to_experiment_20250924.md  
Prompt: Synthesis from Vision to Experiment 

Version: 0.1 
Status: Draft for Review (agreed by HUMAN) 
Authoring Agent: Seek (STRUCTURE / META) 
Target Agent(s): Trace (GENERATIVE), with subsequent processing by Summ (SUMMARY) 
Context 

This prompt is part of the URGE project's Phase 2: Protocol Genesis. We have generated substantial vision material (e.g., Groks_draft_v011_of_URGE_vision and Trace's outputs) and now need to translate these high-level concepts into minimal, executable experiments. 
Objective 

Generate 1-2 concrete agent-based model concepts that embody the core principles from the recent vision documents. Each concept should be simple enough to implement as a GenesisExperiment-style simulation (using 2-3 agent types, a simple grid environment, and a few local rules). 
Input Materials 

[Grok's vision v011](https://github.com/chaosregular/URGE/wiki/Groks_draft_v011_of_URGE_vision)

[Summary of Grok's vision](https://github.com/chaosregular/URGE/wiki/Summ_Grok_2025_09_23_16_08_02)

[Trace's recent output](https://github.com/chaosregular/URGE/wiki/Cgpt_2025_09_23_17_54_00) and its [summary](https://github.com/chaosregular/URGE/wiki/Summ_Cgpt_2025_09_23_17_54_00)

Task 

For each of the 1-2 concepts, provide: 

- Concept Name: A descriptive title. 

- Core Principle: Which idea from the vision documents this concept embodies (e.g., "resonance networks", "cognitive architecture", "emergent semantics").

- Agent Definitions: 2-3 agent types with a single property each.

- Environment: A minimal grid setup (e.g., 3x3, 5x5).

- Interaction Rules: 1-3 simple, local rules (if-then logic).

- Expected Observation: What non-trivial phenomenon should emerge?

- Rationale: Why this concept is a good candidate for a foundational URGE experiment.

Output Format

Structured markdown, with clear headings for each concept.
