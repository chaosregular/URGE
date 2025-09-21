def low_dim_conflict_proto(full_anom, proj_dim=3, thresh=0.5):
    """
    Simulate low-dim conflicts: Project high-dim anomaly to low-dim, detect mismatches.
    
    Args:
        full_anom (np.array): High-dim anomaly vector.
        proj_dim (int): Low-dim projection size.
        thresh (float): Mismatch threshold.
    
    Returns:
        list: Classified conflicts (e.g., 'Mismatch' if delta > thresh).
    
    Example:
        >>> low_dim_conflict_proto(np.random.uniform(-1,1,8))
        ['Mismatch', 'Harmony']  # Dummy
    """
    proj = full_anom[:proj_dim]  # Undersample
    delta = np.linalg.norm(full_anom - np.pad(proj, (0, len(full_anom)-proj_dim)))
    return ['Mismatch' if delta > thresh else 'Harmony']  # Archive for resolution
