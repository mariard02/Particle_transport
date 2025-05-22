# Particle Transport Simulation

## Usage

### Running a Simulation from a Configuration File

The simulation can be executed using a user-defined configuration file (`config.json`), which includes parameters for geometry, material properties, initial particle conditions, and the number of simulation runs.

#### Example Configuration File (`config.json`):
```json
{
  "run": {
    "run_name": "run_1",
    "simulations": 1000,
    "save_histories": "True"
  },
  "particle": {
    "type": "charged",
    "x": 0,
    "y": 0,
    "z": 0,
    "vx": 0.5,
    "vy": 0,
    "vz": 0,
    "charge": 1,
    "mass": 1
  },
  "material": {
    "mean_free_path": 0.5,
    "pabs": 0.1,
    "k": 0.01,
    "A": 50,
    "stopping_power": 0.1
  },
  "geometry": {
    "shape": "sphere",
    "scale": 20
  }
}
```

If "save_histories": "True" is set, the program stores full trajectories of one absorbed, one reflected, and one transmitted particle.

## Geometry Configuration

Each geometry requires specific parameters:

### Regular slab:
- scale: Slab length along the x-axis.
- x_init: Initial x-position.
- Material properties: mean_free_path, pabs, k, absorption_power, A (optional).
### Sphere:
- scale: Radius.
- Material properties as above.
### Double slab:
- total_length: Total length in x.
- scale: Ratio between slab lengths.
- Material properties for each slab: mean_free_path1/2, pabs1/2, k1/2, stopping_power1/2, A1/2.
### Finite slab:
- scale, xlength, ylength: Dimensions in x, y, z.
- Material properties as above.

## Particle Definition

- Types: neutron or charged.

- Required for charged particles:
  - mass: In units of neutron mass.
  - charge: In units of elementary charge e.
- Initial conditions: Position (x, y, z) and velocity (vx, vy, vz).

> **Notes:**
>
>A is optional (set to null if omitted).
>
>stopping_power is required for charged particles.
>

## Running the Simulation

1. Make the script executable:
```bash
chmod u+x Run_Geometry.sh
```

2. Execute:
```bash
./Run_Geometry.sh config.json
```
Outputs are saved in out/<run_name>/, including:
- configuration_output.txt: Proportions of absorbed/reflected/transmitted particles with uncertainties.
- Trajectory files (if save_histories is True).
- Plot of trajectories (if enabled).

## Varying Geometry Size
To study how particle fractions vary with geometry size:
1. Replace scale in config.json with min_scale and max_scale.
2. Make the script executable
```bash
chmod u+x Particle_Transport.sh
```
3. Run 
```bash
./Particle_Transport.sh config.json
```
- The simulation runs for 21 values between min_scale and max_scale.
- Output includes a plot of particle fractions vs. geometry size.

## Frontend Application (Graphical Interface)
A user-friendly Electron app is available to configure and run simulations without editing config.json manually.
### Launching the app:
1. Make the script executable:
```bash
chmod u+x RunParticleTransportApp.sh
```
2. Run:
```bash
./RunParticleTransportApp.sh
```
- The app checks for npm and installs it if missing.
- Configure parameters in the GUI, then click:
  - Save configuration: Generates config.json.
  - Run simulation: Executes the simulation (20 geometry sizes between min_scale and max_scale).
