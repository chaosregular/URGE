import random

def log_harvest_proto(logs, num_picks=1, weights=None):
    """
    Randomly pick logs for describe/categorize/weight; filter for vision draft.
    
    Args:
        logs (list of str): Timestamped log signatures (e.g., '2025-09-22_03:45:11').
        num_picks (int): Number to pick.
        weights (dict, optional): Pre-assigned weights (e.g., {'log_id': 1e-10}).
    
    Returns:
        dict: {'picked': list, 'descs': list, 'cats': list, 'weights': list}  # For archive.
    
    Example:
        >>> log_harvest_proto(['2025-09-22'], weights={'2025-09-22': 1e-10})
        {'picked': ['2025-09-22'], 'descs': ['Noisy exchange'], 'cats': ['Vision'], 'weights': [1e-10]}
    """
    picked = random.sample(logs, min(num_picks, len(logs)))
    # Dummy processing (extend with AI flavor calls)
    descs = ['Noisy exchange' for _ in picked]  # Describe
    cats = ['Vision' for _ in picked]  # Categorize
    w = weights or {p: 1e-10 for p in picked}  # Weight
    weights_list = [w[p] for p in picked]
    return {'picked': picked, 'descs': descs, 'cats': cats, 'weights': weights_list}
