import numpy as np

def simulate_anomaly_membrane(num_beings=10, dim=8, steps=100, perturb_scale=0.1, perturb_step=50, awareness_thresh=0.5):
    """Toy sim with awareness: Beings repel high influences."""
    A = np.zeros(dim)
    A[:3] = np.random.uniform(-1, 1, 3)
    
    P = [np.random.uniform(0, 1, dim) for _ in range(num_beings)]
    M = [np.zeros(dim) for _ in range(num_beings)]
    
    history = [[] for _ in range(num_beings)]
    for t in range(steps):
        for i in range(num_beings):
            projected = P[i] * A + np.random.normal(0, 0.01, dim)
            M[i] = 0.9 * M[i] + 0.1 * projected
            history[i].append(M[i].copy())
        
        if t == perturb_step:
            influencer = 0
            for j in range(1, num_beings):
                delta = perturb_scale * (M[influencer] - M[j])
                # Awareness: If delta norm > thresh, repel (dampen/reverse)
                delta_norm = np.linalg.norm(delta)
                if delta_norm > awareness_thresh:
                    delta *= -0.5  # Repel half-strength
                M[j] += delta
    
    influence = np.zeros((num_beings, num_beings))
    for i in range(num_beings):
        for j in range(num_beings):
            if i != j:
                influence[i,j] = np.linalg.norm(history[j][-1] - history[j][0])
    
    return influence, [np.mean(h, axis=0) for h in history]

# Run example
np.random.seed(42)
influence, avg_models = simulate_anomaly_membrane()
print("Influence Matrix:\n", influence)
print("Avg Models per Being:\n", avg_models)
