#!/bin/bash

# Exit immediately if any command fails
set -e

cd "$(dirname "$0")"

if [ $# -lt 1 ]; then
  echo "Usage: $0 config.json" >&2
  exit 1
fi

json_file="$1"

# Check if jq is installed
if ! command -v jq &> /dev/null; then
    echo "Error: jq is not installed. Please install it first." >&2
    exit 1
fi

# Check if the JSON file exists
if [ ! -f "$json_file" ]; then
    echo "Error: configuration file '$json_file' not found." >&2
    exit 1
fi

# Extract values
run_name=$(jq -r '.run.run_name' "$json_file" || { echo "Error reading run_name from $json_file" >&2; exit 1; })
scale=$(jq -r '.geometry.scale' "$json_file" || { echo "Error reading scale from $json_file" >&2; exit 1; })

# Check if values are empty
if [ -z "$run_name" ]; then
    echo "Error: run_name is empty or not found in $json_file" >&2
    exit 1
fi

if [ -z "$scale" ]; then
    echo "Error: scale is empty or not found in $json_file" >&2
    exit 1
fi

if ! echo "$scale > 0" | bc -l | grep -q 1; then
    echo "Error: scale must be bigger than 0" >&2
    exit 1
fi

output_dir="$(pwd)/out/$run_name/data"
mkdir -p "$output_dir" || { echo "Error creating output directory $output_dir" >&2; exit 1; }

cd cpp || { echo "Error entering cpp directory" >&2; exit 1; }

output_file="$output_dir/simulations_output.txt"
echo "Scale Absorbed std Reflected std Scaped std" > "$output_file" || { echo "Error creating output file" >&2; exit 1; }

# Compile
g++ -std=c++14 -Iinclude main.cpp src/*.cpp -o simulation || {
    echo "Compilation failed. Aborting." >&2
    exit 1 
}

output=$(./simulation "../$json_file" $scale)
echo "$scale $output" >> "$output_file" || { echo "Error writing to output file" >&2; exit 1; }

rm simulation || { echo "Warning: could not remove simulation binary" >&2; }

cd .. || { echo "Error returning to parent directory" >&2; exit 1; }

# Run Python script
python3 plot_particle_transport.py --configuration "$json_file" --no_multiple_geometries|| {
    echo "Error running Python plotting script" >&2
    exit 1
}

echo "Simulation completed successfully."