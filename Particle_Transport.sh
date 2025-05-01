#!/bin/zsh

# Leer el archivo JSON
json_file="config.json"
json_data=$(cat "$json_file")

variables_needed=(
  "run.run_name"
  "run.simulations"
  "material.lambda"
  "material.pabs"
  "material.k"
  "geometry.shape"
)

variables_opcionales=(
  "run.simulations"
  "material.pabs"
  "material.k"
)

assign_variables() {
  local key=$1
  local needed=$2
  local var_name="${key##*.}"

  if echo "$json_data" | jq -e ".${key}" > /dev/null 2>&1; then
    eval "${var_name}=\$(echo \"\$json_data\" | jq -r \".${key}\")"
    return 0
  else
    if [[ "$needed" == "true" ]]; then
      echo "✗ Error: '${key}' is not defined." >&2
      return 1
    else
      return 0
    fi
  fi
}

for key in "${variables_needed[@]}"; do
  if ! assign_variables "$key" "true"; then
    exit 1
  fi
done

for key in "${variables_opcionales[@]}"; do
  assign_variables "$key" "false"
done

cd cpp

output_file="../data/simulations_output.txt"
echo "Length (L) Absorbed Reflected Transmitted" > "$output_file" 

g++ -std=c++11 -Iinclude main.cpp src/*.cpp -o simulation

if [ $? -ne 0 ]; then
  echo "Compilation failed. Aborting."
  exit 1 
fi

for L in {1..10}; do
    output=$(./simulation "$lambda" "$pabs" "$k" "$L")
    echo "$L $output" >> "$output_file"
done

rm simulation

cd .. 

python3 plot_particle_transport.py --lambda "$lambda" --pabs "$pabs" --k "$k"