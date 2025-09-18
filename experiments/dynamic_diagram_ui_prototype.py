import networkx as nx
import matplotlib.pyplot as plt
import numpy as np

def visualize_concept_plane(concepts, weights, flavors):
    """Prototype UI: Weighted graph for concept plane, with mass flavors as node attrs."""
    G = nx.Graph()
    for concept, weight in zip(concepts, weights):
        G.add_node(concept, weight=weight, **flavors.get(concept, {}))
    
    # Add edges based on similarity (dummy: random for proto)
    for i in range(len(concepts)):
        for j in range(i+1, len(concepts)):
            sim = np.random.uniform(0, 1) * (weights[i] + weights[j]) / 2
            G.add_edge(concepts[i], concepts[j], weight=sim)
    
    pos = nx.spring_layout(G)  # Force-directed
    nx.draw(G, pos, with_labels=True, node_size=[G.nodes[n]['weight']*1000 for n in G])
    plt.title('Dynamic Concept Plane (Adjust Weights for Slices)')
    plt.show()

# Example: Your flavors
concepts = ['greed', 'trust', 'ethics']
weights = [0.7, 0.5, 0.3]  # Adjustable
flavors = {
    'greed': {'own_gain': 0.9, 'cost_to_others': 0.8},
    'trust': {'gain_of_others': 0.7, 'own_effort': 0.4},
    'ethics': {'limitations_forced': -0.5, 'own_imposed': 0.6}
}

visualize_concept_plane(concepts, weights, flavors)
# TODO: Add sliders for weights, user bring_up/push_down via inputs.
