def anomaly_layer_proto(layers=3, dim=8):
    # Toy: Activate hidden dims per layer
    A_base = np.random.uniform(-1, 1, dim)
    layers_anoms = [A_base.copy() for _ in range(layers)]
    for i in range(1, layers):
        layers_anoms[i][dim//2:] += np.random.normal(0, 0.1, dim//2)  # Spooky shifts
    return layers_anoms  # Test neighbor similarities
