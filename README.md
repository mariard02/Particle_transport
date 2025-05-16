# Particle transport

This project focuses on simulating the passage of neutrons through different materials and geometries.
## Usage

### Running a Simulation from a Configuration File

The simulation code can be run using a user-defined configuration file. This file includes all relevant parameters: geometry type, material properties, initial particle conditions, and number of simulation runs.

To begin, edit the configuration file `config.json`. Here's an example:

```json
{
  "run": {
    "run_name": "run_1",
    "simulations": 1000,
    "save_histories": "True"
  },
  "particle": {
    "x": 0,
    "y": 0,
    "z": 0,
    "vx": 0.5,
    "vy": 0,
    "vz": 0
  },
  "material": {
    "mean_free_path": 0.5,
    "pabs": 0.1,
    "k": 0.01,
    "A": 50
  },
  "geometry": {
    "shape": "sphere",
    "scale": 20
  }
}
```

If `"save_histories": "True"` is included under the `run` section, the program will store the full trajectories of one absorbed, one reflected, and one transmitted particle.

#### Geometry Configuration

Each geometry requires specific parameters:

- **Regular slab**: define `scale` as the slab length along the x axis, and `x_init` as the initial x position. Set material properties with `mean_free_path`, `pabs`, `k`, and `A`.
- **Sphere**: define the radius using `scale`. Set material properties as above.
- **Double slab**: define `total_length` as the total length in x, and `scale` as the ratio between the lengths of the two slabs. Specify each slab’s properties with `mean_free_path1`, `pabs1`, `k1`, `A1` and `mean_free_path2`, `pabs2`, `k2`, `A2`.
- **Finite slab**: define the lengths in x, y, and z axes using `scale`, `xlength`, and `ylength` respectively. Provide material properties as before.

#### Running the Simulation

Make the run script executable:

```bash
chmod u+x Run_Geometry.sh
```

Then run the simulation:

```bash
./Run_Geometry.sh config.json
```

This will create a folder with the run name in the `out` directory. A copy of the `config.json` configuration file is saved there for reproducibility. Inside this folder, a `data` subdirectory will be created containing a `configuration_output.txt` file with the proportions of absorbed, reflected and transmitted neutrons, along with their uncertainties.

If the parameter `"save_histories": "True"` is specified, three additional files with the trajectories of one absorbed, one reflected, and one transmitted neutron will be saved **directly in the run folder** (not in `data`). A plot visualizing these trajectories will also be generated and saved in the same location, using the run name as filename.

> **Note:** the parameter `A` in all geometry types is **optional**. If it is not to be specified, set it explicitly as `null` in the configuration file.

### Varying Geometry from a Configuration File

Another way to use the code is to study how the fractions of transmitted, absorbed and reflected neutrons vary as a function of the geometry’s size. To perform this type of scan, the configuration file remains largely the same as in the previous case. The only required change is to replace the `scale` parameter with two new parameters: `min_scale` and `max_scale`.

Make the run script executable:

```bash
chmod u+x Particle_Transport.sh
```

Run the simulation with:

```bash
./Particle_Transport.sh config.json
```

The code will automatically perform simulations for 20 equally spaced values between `min_scale` and `max_scale`. For each value, it computes the proportions of transmitted, absorbed and reflected neutrons.

The output folder will follow the same structure as in the single-geometry case. In addition, a plot summarizing the variation of neutron fractions as a function of geometry size will be generated and saved in the same folder.

### Frontend Application

To improve user experience and eliminate the need to manually edit the configuration file, a graphical interface has been developed using Electron with JavaScript, HTML, and CSS.

To launch the application, first make the startup script executable:

```bash
chmod u+x RunParticleTransportApp.sh
```

Then, run it with:

```bash
./RunParticleTransportApp.sh
```

The script will first check whether the `npm` package manager is installed and will install it if necessary. Once the environment is ready, an application window will open.

Inside the app, the user can specify the properties of the neutron, the material, and the geometry. After filling in the desired parameters, click the `Save configuration` button to generate the `config.json` file. Then, click `Run simulation` to execute the simulation using the saved configuration. It will perform simulations for 20 equally spaced values between the minimum and maximum specified dimensions of the material, and the results will be generated following the same structure and logic described above.
