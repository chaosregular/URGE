import numpy as np
import matplotlib.pyplot as plt

def simulate_attractors(num_particles=100, steps=50, attractors=None, observer_perturb=None):
    """Simulate particles in latent space with attractors; optional observer perturbation."""
    if attractors is None:
        attractors = {'greed': np.array([0.8, 0.8]), 'ethics': np.array([0.2, 0.2])}  # Positions in [0,1]x[0,1]
    strengths = {'greed': 0.5, 'ethics': -0.3}  # Positive: attract, negative: repel
    
    positions = np.random.uniform(0, 1, (num_particles, 2))  # Random init
    history = np.zeros((steps, num_particles, 2))
    history[0] = positions
    
    for t in range(1, steps):
        forces = np.zeros_like(positions)
        for name, center in attractors.items():
            deltas = positions - center
            dists = np.linalg.norm(deltas, axis=1) + 1e-6  # Avoid div0
            forces += strengths.get(name, 0.1) * (deltas / dists[:, np.newaxis]**2)
        
        # Observer perturbation (e.g., noise injection at step 20)
        if observer_perturb and t == observer_perturb['step']:
            positions += np.random.normal(0, observer_perturb['scale'], positions.shape)
        
        positions = np.clip(positions + 0.05 * forces, 0, 1)  # Update with damping
        history[t] = positions
    
    # Entropy proxy: Mean distance spread (higher = more chaotic)
    entropy = np.mean(np.std(history, axis=1), axis=1)
    return history, entropy

# Run: No observer vs. with observer (nudging toward ethics)
history_no_obs, entropy_no = simulate_attractors()
history_obs, entropy_obs = simulate_attractors(observer_perturb={'step': 20, 'scale': 0.1})

# Plot final states
plt.scatter(history_no_obs[-1, :, 0], history_no_obs[-1, :, 1], label='No Observer (Greed Dominant)')
plt.scatter(history_obs[-1, :, 0], history_obs[-1, :, 1], label='With Observer Perturb')
plt.title('Conceptual Attractor Sim: Observer Shaping')
plt.legend()
plt.show()

print("Entropy delta (pre/post observer step 20):", entropy_obs[21] - entropy_obs[19])
# TODO: Extend to 3D, add 'trust' attractor, integrate with RGB perspectives.
