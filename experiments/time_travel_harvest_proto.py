import random

def time_travel_harvest_proto(logs, num_picks=1, weights=None):
    """
    Harvest old logs for time-travel: Random picks, desc/cat/weight for vision drafts.
    
    Args:
        logs (list): Timestamp IDs (e.g., '2025-04-05_17:54:40').
        num_picks (int): Picks.
        weights (dict, optional): E.g., {'log_id': 1e-10}.
    
    Returns:
        dict: {'picked': list, 'descs': list, 'cats': list, 'weights': list}.
    
    Example:
        >>> time_travel_harvest_proto(['2025-04-05'], weights={'2025-04-05': 1e-10})
        {'picked': ['2025-04-05'], 'descs': ['Noisy log'], 'cats': ['Vision'], 'weights': [1e-10]}
    """
    picked = random.sample(logs, min(num_picks, len(logs)))
    descs = ['Noisy log' for _ in picked]  # AI-describe
    cats = ['Vision' for _ in picked]  # AI-categorize
    w = weights or {p: 1e-10 for p in picked}
    weights_list = [w[p] for p in picked]
    return {'picked': picked, 'descs': descs, 'cats': cats, 'weights': weights_list}
