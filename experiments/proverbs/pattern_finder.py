# pattern_finder.py
# Purpose: Find clusters in the proverb vectors and create a visualization.
# Usage: python3 pattern_finder.py

# Import necessary libraries
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import HDBSCAN
import umap.umap_ as umap
import ast  # For converting the stringified vector back to a list of numbers
import os   # <--- Add this line

# Check if the input file exists
if not os.path.exists('proverbs_with_vectors.csv'):
    print("ERROR: Could not find 'proverbs_with_vectors.csv'. Please run 'feature_extractor.py' first.")
    exit()

# Load the data
print("Loading data...")
df = pd.read_csv('proverbs_with_vectors.csv')

# Convert the stringified vector column back into a list of numbers
print("Processing vectors...")
X = np.array([ast.literal_eval(vector_str) for vector_str in df['vector']])

# Step 1: Use UMAP to reduce the 384-dimensional vectors to 2D for plotting
print("Reducing dimensions with UMAP...")
reducer = umap.UMAP(random_state=42, n_components=2)
X_2d = reducer.fit_transform(X)

# Step 2: Use HDBSCAN to find clusters in the original high-dimensional space
print("Finding clusters with HDBSCAN...")
clusterer = HDBSCAN(min_cluster_size=2, gen_min_span_tree=True)
cluster_labels = clusterer.fit_predict(X)

# Add the 2D coordinates and cluster labels to the DataFrame
df['umap_x'] = X_2d[:, 0]
df['umap_y'] = X_2d[:, 1]
df['cluster'] = cluster_labels

# Step 3: Create a scatter plot
print("Generating plot...")
plt.figure(figsize=(10, 8))

# Create a color map for the themes
unique_themes = df['theme_tag'].unique()
color_map = {theme: plt.cm.Set1(i) for i, theme in enumerate(unique_themes)}

# Create a shape map for the languages
unique_langs = df['language'].unique()
shape_map = {lang: marker for lang, marker in zip(unique_langs, ['o', 's', '^', 'D', 'v', '<', '>'])}  # Circle, Square, Triangle, Diamond, etc.

# Plot each proverb
for theme in unique_themes:
    for lang in unique_langs:
        subset = df[(df['theme_tag'] == theme) & (df['language'] == lang)]
        if not subset.empty:
            plt.scatter(subset['umap_x'], subset['umap_y'],
                       c=[color_map[theme]],
                       marker=shape_map[lang],
                       label=f'{theme} ({lang})', s=100)  # s is the point size

plt.title('Proverb Clusters by Theme and Language')
plt.xlabel('UMAP Dimension 1')
plt.ylabel('UMAP Dimension 2')
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')  # Place legend outside the plot
plt.tight_layout()  # Adjust layout to make room for the legend

# Save the plot as a PNG image
plot_filename = 'proverb_clusters.png'
plt.savefig(plot_filename, dpi=150)
print(f"Plot saved as {plot_filename}")

# Print a simple summary to the console
print("\n--- Cluster Summary ---")
for cluster_id in np.unique(cluster_labels):
    cluster_proverbs = df[df['cluster'] == cluster_id]
    print(f"Cluster {cluster_id}: {len(cluster_proverbs)} proverbs")
    for _, row in cluster_proverbs.iterrows():
        print(f"  - '{row['proverb_text']}' ({row['language']}, {row['theme_tag']})")
