import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import argparse
from dataclasses import asdict, dataclass
import simple_parsing
import shutil
import json
import os

@dataclass
class TrainingCli:
    """
    Command-line arguments for a training run.

    Attributes:
        run_name (str): Unique identifier for the training run (used in filenames/paths).
        no_config_edit (bool): If True, skip prompting user to edit the config file before running.
    """
    configuration: str
    no_multiple_geometries: bool = False

def load_simulation_data(filename):
    """
    Loads simulation data from a text file.

    Parameters:
        filename (str): Path to the file containing the data.

    Returns:
        tuple: Arrays for L (length/scale), absorbed fraction and std,
               reflected fraction and std, transmitted fraction and std.
    """
    data = np.loadtxt(filename, skiprows=1)
    L = data[:, 0] 
    absorbed = data[:, 1]  
    absorbed_std = data[:, 2]  
    reflected = data[:, 3]  
    reflected_std = data[:, 4]  
    transmitted = data[:, 5]  
    transmitted_std = data[:, 6]  
    return L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std

class PlotConfig:
    """
    Handles plotting configuration and generation based on simulation geometry and data.

    Attributes:
        cli: Command line interface arguments (unused here).
        data (dict): Configuration data loaded from a run.
    """
    
    def __init__(self, cli, data):
        self.__shape = data["geometry"]["shape"]
        self.__data = data
        self.__output_dir = f'out/{data["run"]["run_name"]}'
        os.makedirs(self.__output_dir, exist_ok=True)

    def getOutputDir(self):
        """Returns the directory where plots will be saved."""
        return self.__output_dir

    def get_xaxis(self):
        """Returns a label for the x-axis depending on the geometry shape."""
        if self.__shape == "regular_slab":
            return "Slab length"
        elif self.__shape == "finite_slab":
            return "Length in the z axis"
        elif self.__shape == "sphere":
            return "Radius"
        elif self.__shape == "double_slab":
            return "Ratio"
        else:
            return "Scale"

    def clean_filename(self, value):
        """Sanitizes float values to be filename-safe (e.g. '0.5' → '0p5')."""
        return str(value).replace('.', 'p')

    def plot_probabilities(self, L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std):
        """
        Plots the probabilities of absorption, transmission, and (if applicable) reflection.

        Parameters:
            L (array): Length or scale values.
            absorbed (array): Absorbed fractions.
            absorbed_std (array): Standard deviations of absorbed fractions.
            reflected (array): Reflected fractions.
            reflected_std (array): Standard deviations of reflected fractions.
            transmitted (array): Transmitted fractions.
            transmitted_std (array): Standard deviations of transmitted fractions.
        """
        plt.figure(figsize=(8, 5))
        
        plt.errorbar(L, absorbed, yerr=absorbed_std, marker=".", label="Absorbed", color="red", fmt=':')        
        plt.errorbar(L, transmitted, yerr=transmitted_std, marker=".", label="Escaped", color="blue", fmt=':')
        if self.__shape in ["regular_slab", "double_slab"]:
            plt.errorbar(L, reflected, yerr=reflected_std, marker=".", label="Reflected", color="green", fmt=':')
        
        plt.legend()
        plt.grid(True)
        plt.xlabel(self.get_xaxis())
        plt.ylabel("Fraction of neutrons")
        plt.title("Probabilities")
        plt.tight_layout()

        filename = os.path.join(self.__output_dir, "probabilities.png")
        plt.savefig(filename, dpi=300)
        plt.close()
        print(f"✅ Saved plot: {filename}")

    def load_coords(self, filename):
        """
        Loads 3D coordinates from a file.

        Parameters:
            filename (str): Path to the coordinate file.

        Returns:
            list of tuples: Each tuple contains (x, y, z) coordinates.
        """
        coords = []
        if not os.path.exists(filename):
            print(f"Warning: File not found: {filename}")
            return coords

        with open(filename, "r") as file:
            for line in file:
                try:
                    x, y, z = map(float, line.strip().split())
                    coords.append((x, y, z))
                except ValueError:
                    continue  # Skip malformed lines
        return coords

    def plot_trajectories(self):
        """
        Plots 3D trajectories of neutrons depending on their fate (absorbed, escaped, reflected),
        including the geometry of the simulation domain.
        """


        # Helper function to get scale parameter
        def get_scale(geometry_data):
            return geometry_data.get('max_scale', geometry_data.get('scale'))
        
        # Base trajectories: absorbed and escaped
        trayectories = {
            "Absorbed": (f"out/{self.__data['run']['run_name']}/data/hist_absorbed.txt", "red"),
            "Escaped": (f"out/{self.__data['run']['run_name']}/data/hist_scaped.txt", "blue")
        }

        # Add reflected if geometry supports it
        if self.__shape in ["regular_slab", "double_slab"]:
            trayectories["Reflected"] = (f"out/{self.__data['run']['run_name']}/data/hist_reflected.txt", "green")

        fig = plt.figure(figsize=(9, 6))
        ax = fig.add_subplot(111, projection='3d')

        # Plot each trajectory
        for label, (filename, color) in trayectories.items():
            coords = self.load_coords(filename)
            if not coords:
                continue
            x_vals = [c[0] for c in coords]
            y_vals = [c[1] for c in coords]
            z_vals = [c[2] for c in coords]
            ax.plot(x_vals, y_vals, z_vals, linewidth=2, color=color, label=label)

        # Geometry visualization
        if self.__shape == "regular_slab":
            # Plot entrance and exit planes
            x_init = self.__data['geometry'].get('x_init')
            L = get_scale(self.__data['geometry'])
            x_end = x_init + L
            y = np.linspace(-10, 10, 2)
            z = np.linspace(-10, 10, 2)
            Y, Z = np.meshgrid(y, z)
            ax.plot_surface(np.full_like(Y, x_init), Y, Z, alpha=0.2, color='gray')
            ax.plot_surface(np.full_like(Y, x_end), Y, Z, alpha=0.2, color='gray')
            ax.set_xlim(x_init - 2, x_end + 2)

        elif self.__shape == "finite_slab":
            # Plot a parallelepiped volume
            x_length = self.__data['geometry'].get('x_length', 1)
            y_length = self.__data['geometry'].get('y_length', 1)
            z_length = get_scale(self.__data['geometry'])
            x_min, x_max = -x_length/2, x_length/2
            y_min, y_max = -y_length/2, y_length/2
            z_min, z_max = 0, z_length

            def plot_rectangle(ax, point, u_vec, v_vec, **kwargs):
                """Utility to plot a rectangular surface given corner and two direction vectors."""
                u, v = np.linspace(0, 1, 2), np.linspace(0, 1, 2)
                u_grid, v_grid = np.meshgrid(u, v)
                x = point[0] + u_vec[0]*u_grid + v_vec[0]*v_grid
                y = point[1] + u_vec[1]*u_grid + v_vec[1]*v_grid
                z = point[2] + u_vec[2]*u_grid + v_vec[2]*v_grid
                ax.plot_surface(x, y, z, **kwargs)

            # Draw each face of the box
            plot_rectangle(ax, [x_min, y_min, z_min], [x_length, 0, 0], [0, y_length, 0], alpha=0.2, color='gray')
            plot_rectangle(ax, [x_min, y_min, z_max], [x_length, 0, 0], [0, y_length, 0], alpha=0.2, color='gray')
            plot_rectangle(ax, [x_min, y_min, z_min], [x_length, 0, 0], [0, 0, z_length], alpha=0.2, color='gray')
            plot_rectangle(ax, [x_min, y_max, z_min], [x_length, 0, 0], [0, 0, z_length], alpha=0.2, color='gray')
            plot_rectangle(ax, [x_min, y_min, z_min], [0, y_length, 0], [0, 0, z_length], alpha=0.2, color='gray')
            plot_rectangle(ax, [x_max, y_min, z_min], [0, y_length, 0], [0, 0, z_length], alpha=0.2, color='gray')

            padding = max(x_length, y_length, z_length) * 0.2
            ax.set_xlim(x_min - padding, x_max + padding)
            ax.set_ylim(y_min - padding, y_max + padding)
            ax.set_zlim(z_min - padding, z_max + padding)

        elif self.__shape == "sphere":
            # Plot a transparent sphere
            radius = get_scale(self.__data['geometry'])
            u = np.linspace(0, 2 * np.pi, 20)
            v = np.linspace(0, np.pi, 20)
            x = radius * np.outer(np.cos(u), np.sin(v))
            y = radius * np.outer(np.sin(u), np.sin(v))
            z = radius * np.outer(np.ones(np.size(u)), np.cos(v))
            ax.plot_surface(x, y, z, color='gray', alpha=0.2, edgecolor='black')

            padding = radius * 1.2
            ax.set_xlim(-padding, padding)
            ax.set_ylim(-padding, padding)
            ax.set_zlim(-padding, padding)

        elif self.__shape == "double_slab":
            # Plot 3 planes for two slabs separated by a gap
            x_init = self.__data['geometry'].get('x_init', 0)
            total_length = self.__data['geometry'].get('total_length', 10)
            L = get_scale(self.__data['geometry'])  # Ratio for first slab
            x_middle = x_init + total_length * L
            x_end = x_init + total_length

            y = np.linspace(-10, 10, 2)
            z = np.linspace(-10, 10, 2)
            Y, Z = np.meshgrid(y, z)
            ax.plot_surface(np.full_like(Y, x_init), Y, Z, alpha=0.2, color='gray')
            ax.plot_surface(np.full_like(Y, x_middle), Y, Z, alpha=0.2, color='blue')
            ax.plot_surface(np.full_like(Y, x_end), Y, Z, alpha=0.2, color='gray')

            ax.set_xlim(x_init - 2, x_end + 2)

        # Final plot configuration
        ax.set_title("Neutron trajectories")
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")
        ax.legend()
        plt.tight_layout()

        filename = os.path.join(self.__output_dir, "trajectories.png")
        plt.savefig(filename, dpi=300)
        plt.close()
        print(f"✅ Saved 3D trajectory plot: {filename}")

def main():
    # Create the command-line argument parser with a helpful description
    parser = simple_parsing.ArgumentParser(
        add_help=True, 
        description="Analyse the simulation results."
    )
    
    # Add arguments defined in the TrainingCli class
    parser.add_arguments(TrainingCli, dest="cli")
    
    # Parse the command-line arguments
    args = parser.parse_args()
    cli: TrainingCli = args.cli  # Extract the CLI configuration

    # Load the configuration JSON file specified in the CLI
    with open(cli.configuration) as f:
        data = json.load(f)

    # Create a PlotConfig object to handle plotting settings and output directory
    plot_config = PlotConfig(cli, data)

    # Copy the original configuration file into the output directory for reference
    shutil.copy(cli.configuration, plot_config.getOutputDir())

    # If multiple geometries are allowed, load the simulation results and plot the probabilities
    if not cli.no_multiple_geometries:
        # Load simulation data: lengths, probabilities, and standard deviations
        L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std = load_simulation_data(
            f'out/{data["run"]["run_name"]}/data/simulations_output.txt'
        )

        # Plot the absorption, reflection, and transmission probabilities
        plot_config.plot_probabilities(
            L, absorbed, absorbed_std, 
            reflected, reflected_std, 
            transmitted, transmitted_std
        )

    # If the configuration enables saving trajectories, plot the particle paths
    if data['run'].get("save_hist", False):
        plot_config.plot_trajectories()


# Run the main function if the script is executed directly
if __name__ == "__main__":
    main()