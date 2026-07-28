# CA tools for experiments/toys

This folder contains helper files and example configuration for CA experiments.

Files:
- example_experiment.cfg — simple key=value file used by CA_4nPCA_012.c when started with --config

Usage
- Put configuration files here and pass `--config experiments/toys/ca_tools/example_experiment.cfg` to the CA binary.
- The CA binary also supports --load-rules, --save-rules, --log and --log-interval flags.

Append-only policy: new files are additive. The CA source was lightly patched to add binary rules import/export and CSV logging.
