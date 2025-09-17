import numpy as np
import matplotlib.pyplot as plt

def logistic_noise(k=4.0, x0=0.5, steps=100, noise_scale=0.01):
    """Simulate chaotic logistic map with optional noise."""
    x = np.zeros(steps)
    x[0] = x0
    for i in range(1, steps):
        x[i] = k * x[i-1] * (1 - x[i-1]) + np.random.normal(0, noise_scale)
    return x

# Proxy for 'real' vs. sim: Two instances with same start, measure divergence
steps = 200
real_proxy = logistic_noise(k=3.9, x0=0.123, steps=steps, noise_scale=0.005)  # 'Real' with micro-noise
sim = logistic_noise(k=3.9, x0=0.123, steps=steps, noise_scale=0.0)  # Ideal sim

# Divergence metric: MSE over time
mse = np.mean((real_proxy - sim)**2, axis=0) if real_proxy.ndim > 1 else np.mean((real_proxy - sim)**2)

# For spectrum: FFT on 1D scalar (extend to 3D mesh later)
fft_real = np.fft.fft(real_proxy)
freq = np.fft.fftfreq(steps)

# Plot for observation
plt.plot(real_proxy, label='Real Proxy')
plt.plot(sim, label='Sim')
plt.title('Noise Sync Test: Divergence Over Steps')
plt.legend()
plt.show()

# TODO: Add 3D mesh (e.g., 2D grid of voltages), temp-rate triggers, key gen from spectrum.
print(f"Sync duration (MSE < 0.01): {np.sum(mse < 0.01)} steps")
