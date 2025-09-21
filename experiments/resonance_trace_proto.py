def classify_resonance_traces(deltas, threshold=0.5):
    """
    Classify resonance traces from a list of deltas (e.g., from sim perturbations).
    - Scar: Collapse (delta < -threshold)
    - Chord: Stabilization (delta > threshold)
    - Flicker: Ambiguous (otherwise)
    
    Args:
        deltas (list of float): List of delta values from simulations.
        threshold (float, optional): Boundary for classification. Defaults to 0.5.
    
    Returns:
        list of str: Classified traces for each delta.
    
    Example:
        >>> classify_resonance_traces([-0.6, 0.4, 0.7])
        ['Scar', 'Flicker', 'Chord']
    """
    traces = []
    for delta in deltas:
        if delta < -threshold:
            traces.append('Scar')  # Indicates collapse or destructive imprint
        elif delta > threshold:
            traces.append('Chord')  # Indicates stabilization or creative harmony
        else:
            traces.append('Flicker')  # Indicates ambiguous or transitional state
    return traces  # Use for archiving and stewardship in URGE ecosystems
