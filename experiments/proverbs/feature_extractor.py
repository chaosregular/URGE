
# feature_extractor.py
# Purpose: Convert proverb text into numerical vectors that represent their meaning.
# Usage: python3 feature_extractor.py

# Import necessary libraries
from sentence_transformers import SentenceTransformer
import pandas as pd
import numpy as np
import os

# Check if the input file exists
if not os.path.exists('proverbs.csv'):
    print("ERROR: Could not find 'proverbs.csv'. Please make sure it's in the same folder as this script.")
    exit()

# Load the pre-trained multilingual model.
# This will download the model (~1.5 GB) the first time you run it.
print("Loading the AI model... (This may take a moment and will download ~1.5 GB on first run)")
model = SentenceTransformer('paraphrase-multilingual-MiniLM-L12-v2')

# Load the proverbs from the CSV file
print("Reading proverbs.csv...")
df = pd.read_csv('proverbs.csv')

# Extract the list of proverb texts
proverb_texts = df['proverb_text'].tolist()

# Convert the list of texts into a list of numerical vectors (embeddings)
print("Converting proverbs to vectors...")
proverb_vectors = model.encode(proverb_texts)

# Convert the list of vectors into a DataFrame column (saving as a string for CSV compatibility)
df['vector'] = [str(list(vector)) for vector in proverb_vectors]

# Save the enriched DataFrame to a new CSV file
output_filename = 'proverbs_with_vectors.csv'
df.to_csv(output_filename, index=False)
print(f"Done! Vectors saved to {output_filename}")
print(f"Number of proverbs processed: {len(df)}")
