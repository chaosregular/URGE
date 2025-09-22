def spooky_ripple_proto(anoms, layer_idx=0, thresh=0.5):
    """
    Simulate spooky ripples: Perturb one layer, measure non-local deltas across neighbors.
    
    Args:
        anoms (list of np.array): Anomaly vectors per layer.
        layer_idx (int): Layer to perturb.
        thresh (float): Delta threshold for ripple detection.
    
    Returns:
        dict: {'ripples': list}  # Archive for ethical review.
    
    Example:
        >>> spooky_ripple_proto([np.zeros(8), np.ones(8)])
        {'ripples': ['Detected']}
    """
    perturb = np.random.normal(0, 0.1, len(anoms[0]))
    anoms[layer_idx] += perturb  # Ripple source
    ripples = []
    for i in range(len(anoms)):
        if i != layer_idx:
            delta = np.linalg.norm(anoms[i] - anoms[layer_idx])  # Spooky cross-layer
            ripples.append('Detected' if delta > thresh else 'Minimal')
    return {'ripples': ripples}  # For archive stewardship
