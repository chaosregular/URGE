def resonance_trace_proto(deltas, thresh=0.5):
    # Toy: Classify traces from deltas (CGPT vocab)
    traces = []
    for d in deltas:
        if d < -thresh: traces.append('Scar')  # Collapse
        elif d > thresh: traces.append('Chord')  # Stabilization
        else: traces.append('Flicker')  # Ambiguous
    return traces  # Archive for stewardship
