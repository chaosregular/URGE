***

### **Draft: GenesisExperiment Prompt**

**Version:** 0.1
**Status:** Draft for Review (by `HUMAN`)
**Authoring Agent:** Seek (`STRUCTURE / META`)
**Target Agent(s):** Trace (`GENERATIVE`), with subsequent processing by Summ (`SUMMARY`)

---

**1. PROTOCOL_CONTEXT & OBJECTIVE**

*   **Context:** This prompt is the first executable output of the newly defined `PromptCrafting` protocol (v001). Its purpose is to initiate the `GenesisExperiment`.
*   **Primary Objective:** To generate a minimal, viable, and observable simulation scenario where 2-3 primitive agents interact based on simple rules. The goal is to produce an "atomic unit" of interaction that can be observed, described, and later used as a building block for more complex morphogenetic processes within the URGE project.
*   **Success Criteria:** The experiment is successful if it yields a clear, concise description of a single interaction cycle between agents that demonstrates a non-trivial outcome (e.g., state change, pattern formation, information transfer) from simple rules.

**2. EXPERIMENT_PARAMETERS (The "What")**

*   **Agent Archetypes:** Propose 2 or 3 agent types. They must be extremely simple and abstract (e.g., "Emitter", "Receptor", "Relay"; or "Red", "Blue", "Green"; or "Node A", "Node B"). Define a single, simple property for each (e.g., "charge", "color", "symbol", "activation state").
*   **Environment:** The environment is a minimal, abstract 1D or 2D grid (e.g., a line of 5 cells, a 3x3 grid). Specify the initial placement of agents.
*   **Interaction Rules:** Define 1-3 core rules. Rules must be deterministic and local (e.g., "An agent looks at its immediate neighbors and changes its own state based on what it sees"). Avoid complex calculations; use simple logic (IF-THEN statements).
*   **Observation Focus:** What specific phenomenon should we look for? (e.g., Does a stable pattern form? Does a state oscillate? Does a "signal" propagate across the grid?).

**3. TASK_DEFINITION (The "How" for Trace)**

Generate **three distinct and well-labeled concepts** for this `GenesisExperiment`. For each concept, provide the following in a structured format:

*   **Concept Name:** A descriptive title (e.g., "Binary Signal Propagation", "Color Cycle Synchronization").
*   **Agent & Environment Setup:** A brief description as per parameters above.
*   **Core Rules:** List the 1-3 rules clearly.
*   **Predicted Observable:** What is the expected non-trivial outcome we should see after a few simulation steps?
*   **Rationale:** Why is this a good candidate for a foundational "atom" of interaction?

**4. OUTPUT_SPECIFICATION**

*   **Format:** Structured text, suitable for direct inclusion in a project wiki.
*   **Tone:** Analytical and creative, focusing on clarity and simplicity over computational complexity.

**5. NEXT_STEPS_AFTER_EXECUTION**

Upon completion of this task by `Trace`, the output will be:
1.  Summarized by `Summ` for clarity.
2.  Reviewed and selected/modified by `HUMAN` (chaosregular).
3.  This selected concept will then be used to generate a subsequent prompt for implementing a simple simulation (e.g., in Python or via a textual log).

---

**End of Draft Prompt.**

***
