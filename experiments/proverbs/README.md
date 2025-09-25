### **Your Next "Monkey" Tasks**

1.  **Save the scripts:** Copy each script into a new text file, saving them as `feature_extractor.py` and `pattern_finder.py` in the same folder as your `proverbs.csv`.
2.  **Run the first script:** Open a terminal in that folder and run:
    ```bash
    python3 feature_extractor.py
    ```
    *   The first run will download the model. This is the biggest step.
    *   It should output `proverbs_with_vectors.csv`.
3.  **Run the second script:** Then run:
    ```bash
    python3 pattern_finder.py
    ```
    *   This will generate the `proverb_clusters.png` image and print a summary to the console.

**What to Expect:** With only 7 proverbs, the clustering might not be dramatic, but we should see the two `Perseverance` proverbs in Chinese and Eastern European languages potentially grouping near each other in the 2D space, distinct from `Haste` and `Resourcefulness`. The primary goal is to see the pipeline work without errors.
