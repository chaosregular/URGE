import numpy as np
import matplotlib.pyplot as plt

def simulate_attractors(num_particles=100, steps=50, attractors=None, observer_perturb=None, flavors=None):
    """Simulate particles in latent space with attractors; optional observer perturbation and flavors."""
    if attractors is None:
        attractors = {'greed': np.array([0.8, 0.8]), 'ethics': np.array([0.2, 0.2])}  # Positions in [0,1]x[0,1]
    strengths = {'greed': 0.5, 'ethics': -0.3}  # Positive: attract, negative: repel
    
    if flavors is None:
        flavors = {
            'greed': {'own_gain': 0.9, 'cost_to_others': 0.8, 'own_effort': 0.2},
            'ethics': {'own_gain': 0.3, 'cost_to_others': 0.1, 'own_effort': 0.7}
        }
    
    positions = np.random.uniform(0, 1, (num_particles, 2))  # Random init
    history = np.zeros((steps, num_particles, 2))
    history[0] = positions
    
    # Particle flavors: Init as average or random, update based on proximity
    particle_flavors = np.array([list(flavors['greed'].values())] * num_particles)  # Dummy init with greed flavors
    
    for t in range(1, steps):
        forces = np.zeros_like(positions)
        for name, center in attractors.items():
            deltas = positions - center
            dists = np.linalg.norm(deltas, axis=1) + 1e-6
            forces += strengths.get(name, 0.1) * (deltas / dists[:, np.newaxis]**2)
        
        # Observer perturbation
        if observer_perturb and t == observer_perturb['step']:
            positions += np.random.normal(0, observer_perturb['scale'], positions.shape)
        
        positions = np.clip(positions + 0.05 * forces, 0, 1)
        history[t] = positions
        
        # Update particle flavors: Weighted average based on proximity to attractors
        for i in range(num_particles):
            weights = []
            for name, center in attractors.items():
                dist = np.linalg.norm(positions[i] - center)
                weights.append(1 / (dist + 1e-6) if dist > 0 else 1)
            total_weight = sum(weights)
            blended = np.zeros(len(list(flavors.values())[0]))
            for j, name in enumerate(attractors):
                blended += (weights[j] / total_weight) * np.array(list(flavors[name].values()))
            particle_flavors[i] = blended
    
    # Entropy proxy
    entropy = np.mean(np.std(history, axis=1), axis=1)
    
    # Average flavors post-sim
    avg_flavors = np.mean(particle_flavors, axis=0)
    flavor_keys = list(flavors['greed'].keys())
    
    return history, entropy, avg_flavors, flavor_keys, particle_flavors

# Run: No observer vs. with observer
np.random.seed(42)  # For repro
history_no_obs, entropy_no, avg_no, keys, pf_no = simulate_attractors()
history_obs, entropy_obs, avg_obs, _, pf_obs = simulate_attractors(observer_perturb={'step': 20, 'scale': 0.1})

# Plot final states, color by 'own_gain' flavor (red intensity)
own_gain_no = pf_no[:, 0]  # First flavor: own_gain
own_gain_obs = pf_obs[:, 0]

plt.scatter(history_no_obs[-1, :, 0], history_no_obs[-1, :, 1], c=[(f, 0, 0) for f in own_gain_no], label='No Observer')
plt.scatter(history_obs[-1, :, 0], history_obs[-1, :, 1], c=[(f, 0, 0) for f in own_gain_obs], label='With Observer')
plt.title('Attractor Sim with Flavors: Colored by Own Gain')
plt.legend()
plt.show()

print("Entropy delta (pre/post observer step 20):", entropy_obs[21] - entropy_obs[19])
print("Average flavors no observer:", dict(zip(keys, avg_no)))
print("Average flavors with observer:", dict(zip(keys, avg_obs)))
# TODO: Add more flavors, cluster analysis for emergents.
