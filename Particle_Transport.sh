#!/bin/zsh

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
    echo "Error: JSON file '$json_file' not found." >&2
    exit 1
fi

# Extract values with error checking
run_name=$(jq -r '.run.run_name' "$json_file" || { echo "Error reading run_name from $json_file" >&2; exit 1; })
min_scale=$(jq -r '.geometry.min_scale' "$json_file" || { echo "Error reading min_scale from $json_file" >&2; exit 1; })
max_scale=$(jq -r '.geometry.max_scale' "$json_file" || { echo "Error reading max_scale from $json_file" >&2; exit 1; })

# Check if values are empty
if [ -z "$run_name" ]; then
    echo "Error: run_name is empty or not found in $json_file" >&2
    exit 1
fi

if [ -z "$min_scale" ]; then
    echo "Error: min_scale is empty or not found in $json_file" >&2
    exit 1
fi

if [ -z "$max_scale" ]; then
    echo "Error: max_scale is empty or not found in $json_file" >&2
    exit 1
fi

output_dir="$(pwd)/out/$run_name/data"
mkdir -p "$output_dir" || { echo "Error creating output directory $output_dir" >&2; exit 1; }

cd cpp || { echo "Error entering cpp directory" >&2; exit 1; }

output_file="$output_dir/simulations_output.txt"
echo "Length (L) Absorbed std Reflected std Transmitted std" > "$output_file" || { echo "Error creating output file" >&2; exit 1; }

# Compile with error checking
g++ -std=c++14 -Iinclude main.cpp src/*.cpp -o simulation || {
    echo "Compilation failed. Aborting." >&2
    exit 1 
}

# Run simulations
for L in $(seq $min_scale $max_scale); do
    output=$(./simulation "../$json_file" $L)
    echo "$L $output" >> "$output_file" || { echo "Error writing to output file" >&2; exit 1; }
done

rm simulation || { echo "Warning: could not remove simulation binary" >&2; }

cd .. || { echo "Error returning to parent directory" >&2; exit 1; }

# Run Python script with error checking
python3 plot_particle_transport.py --configuration "$json_file" || {
    echo "Error running Python plotting script" >&2
    exit 1
}

echo "Simulation and plotting completed successfully."