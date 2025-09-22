import random

def harvest_protocol_proto(logs, num_picks=1, weights=None):
    """
    Harvest logs: Random picks for desc/cat/weight; filter for vision drafts.
    
    Args:
        logs (list): Timestamp signatures (e.g., '2025-09-22_12:39:11').
        num_picks (int): Picks.
        weights (dict, optional): E.g., {'log_id': 1e-10}.
    
    Returns:
        dict: {'picked': list, 'descs': list, 'cats': list, 'weights': list}.
    
    Example:
        >>> harvest_protocol_proto(['2025-09-22'], weights={'2025-09-22': 1e-10})
        {'picked': ['2025-09-22'], 'descs': ['Noisy chat'], 'cats': ['Vision'], 'weights': [1e-10]}
    """
    picked = random.sample(logs, min(num_picks, len(logs)))
    descs = ['Noisy chat' for _ in picked]  # AI-describe
    cats = ['Vision' for _ in picked]  # AI-categorize
    w = weights or {p: 1e-10 for p in picked}  # Weight
    weights_list = [w[p] for p in picked]
    return {'picked': picked, 'descs': descs, 'cats': cats, 'weights': weights_list}
