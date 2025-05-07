#!/bin/zsh

if [ $# -lt 1 ]; then
  echo "Usage: $0 config.json" >&2
  exit 1
fi

json_file="$1"

run_name=$(jq -r '.run.run_name' "$json_file")
output_dir="$(pwd)/out/$run_name/data"
mkdir -p "$output_dir"

cd cpp

output_file="$output_dir/simulations_output.txt"
echo "Length (L) Absorbed std Reflected std Transmitted std" > "$output_file" 

g++ -std=c++11 -Iinclude main.cpp src/*.cpp -o simulation 

if [ $? -ne 0 ]; then
  echo "Compilation failed. Aborting."
  exit 1 
fi

for L in {1..10}; do
    output=$(./simulation ../$json_file $L)
    echo "$L $output" >> "$output_file"
done

rm simulation

cd .. 

python3 plot_particle_transport.py --configuration $json_file 