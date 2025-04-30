#!/bin/zsh
cd cpp

if [ $# -ne 3 ]; then
  echo "Usage: $0 <lambda> <pabs> <k>\n";
  exit 1
fi

output_file="../data/simulations_output.txt"
echo "Length (L) Absorbed Reflected Transmitted" > "$output_file" 

g++ -std=c++11 -Iinclude main.cpp src/*.cpp -o simulation

if [ $? -ne 0 ]; then
  echo "Compilation failed. Aborting."
  exit 1 
fi

for L in {1..10}; do
    output=$(./simulation "$1" "$2" "$3" "$L")
    echo "$L $output" >> "$output_file"
done

rm simulation

cd .. 

python3 plot_particle_transport.py --lambda "$1" --pabs "$2" --k "$3"