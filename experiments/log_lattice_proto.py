import random
import networkx as nx

def log_lattice_proto(logs, num_picks=3, tags=None):
    """
    Generate interlinked log graph: Random picks, tag axes for exploration.
    
    Args:
        logs (list): Timestamp IDs.
        num_picks (int): Picks.
        tags (dict, optional): E.g., {'log_id': ['#Anomaly']}.
    
    Returns:
        str: Mermaid graph code for wiki embed.
    
    Example:
        >>> log_lattice_proto(['2025-04-14'], tags={'2025-04-14': ['#NoiseSync']})
        'graph TD\nA[2025-04-14] -->|#NoiseSync| B[Vision_v010]'
    """
    picked = random.sample(logs, min(num_picks, len(logs)))
    G = nx.Graph()
    for p in picked:
        G.add_node(p, desc='Noisy log', cat='Theme')
        t = tags.get(p, ['#Default']) if tags else ['#Default']
        for tag in t:
            G.add_edge(p, tag)  # Axis links
    mermaid = 'graph TD\n' + '\n'.join([f'{u} -->|{e}| {v}' for u,v,e in G.edges(data='label', default='link')])
    return mermaid  # Embed in wiki for live structure
