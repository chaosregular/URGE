# Noise Sync Test: 3D Extension Results

## Summary
- MSE per step (first 10): [0.         0.00066944 0.00289368 0.01303384 0.06318597 0.12040413 0.18763195 0.17642431 0.21592775 0.20890509]
- Sync duration (MSE < 0.01): 3 steps out of 50
- Mean FFT magnitude (final real): 3.7426

Divergence shows quick loss of sync due to spatial noise/coupling, relevant for 3D mesh analysis.

## Code Used
```python
import numpy as np

def logistic_3d(r=4.0, init_state=None, shape=(10, 10, 10), steps=100, noise_scale=0.01, coupling=0.1):
    """Simulate 3D grid of coupled logistic maps with noise."""
    if init_state is None:
        state = np.random.uniform(0.1, 0.9, shape)  # Random init in [0.1, 0.9] for chaos
    else:
        state = init_state.copy()
    
    history = np.zeros((steps,) + shape)
    history[0] = state
    
    for t in range(1, steps):
        # Logistic update
        next_state = r * state * (1 - state)
        
        # Simple coupling: average with 3D neighbors (diffusion)
        padded = np.pad(state, pad_width=1, mode='wrap')  # Toroidal boundaries
        neighbors_sum = (
            padded[2:, 1:-1, 1:-1] + padded[:-2, 1:-1, 1:-1] +
            padded[1:-1, 2:, 1:-1] + padded[1:-1, :-2, 1:-1] +
            padded[1:-1, 1:-1, 2:] + padded[1:-1, 1:-1, :-2]
        )
        diffusion = coupling * (neighbors_sum / 6 - state)
        
        # Add noise
        noise = np.random.normal(0, noise_scale, shape)
        
        state = np.clip(next_state + diffusion + noise, 0, 1)
        history[t] = state
    
    return history

# Parameters
shape = (5, 5, 5)  # Small for speed
steps = 50  # Reduced for quick execution
r = 3.9  # Chaotic regime
init_state = np.random.uniform(0.1, 0.9, shape)  # Shared initial state

# 'Real' proxy with noise and coupling
real_history = logistic_3d(r=r, init_state=init_state, shape=shape, steps=steps, noise_scale=0.005, coupling=0.1)

# Ideal sim without noise/coupling
sim_history = logistic_3d(r=r, init_state=init_state, shape=shape, steps=steps, noise_scale=0.0, coupling=0.0)

# Divergence: MSE per step over 3D volume
mse_per_step = np.mean((real_history - sim_history)**2, axis=(1,2,3))

# Sync duration: Steps where MSE < 0.01
sync_steps = np.sum(mse_per_step < 0.01)

# For spectrum: 3D FFT on final real state (magnitude mean for simplicity)
fft_real = np.abs(np.fft.fftn(real_history[-1]))
fft_mean = np.mean(fft_real)

print("MSE per step (first 10):", mse_per_step[:10])
print(f"Sync duration (MSE < 0.01): {sync_steps} steps out of {steps}")
print(f"Mean FFT magnitude of final real state: {fft_mean:.4f}")
