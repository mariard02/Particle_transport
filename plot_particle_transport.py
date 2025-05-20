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
    data = np.loadtxt(filename, skiprows=1)
    L = data[:, 0] 
    absorbed = data[:, 1]  
    absorbed_std = data[:, 2]  
    reflected = data[:, 3]  
    reflected_std = data[:, 4]  
    transmitted = data[:, 5]  
    transmitted_std = data[:, 6]  
    return L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std

class PlotConfig():
    def __init__(self, cli, data):
        self.__shape = data["geometry"]["shape"]
        self.__data = data
        self.__output_dir = f'out/{data["run"]["run_name"]}'
        os.makedirs(self.__output_dir, exist_ok=True)

    def getOutputDir(self):
        return self.__output_dir

    def get_xaxis(self):
        if self.__shape == "regular_slab":
            return "Slab length"
        elif self.__shape == "regular slab":
            return "Length in the z axis"
        elif self.__shape == "sphere":
            return "Radius"
        elif self.__shape == "double_slab":
            return "Ratio"
        else:
            return "Scale"

    def clean_filename(self, value):
        return str(value).replace('.', 'p')

    def plot_probabilities(self, L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std):
        plt.figure(figsize=(8, 5))
        
        plt.errorbar(L, absorbed, yerr=absorbed_std, marker=".", label="Absorbed", color="red", fmt=':')        
        plt.errorbar(L, transmitted, yerr=transmitted_std, marker=".", label="Escaped", color="blue", fmt=':')
        if self.__shape in ["regular_slab", "double_slab"]:
            plt.errorbar(L, reflected, yerr=reflected_std, marker=".", label="Reflected", color="green", fmt=':')
        
        plt.legend()
        plt.grid(True)
        plt.xlabel(self.get_xaxis())
        plt.ylabel("Fraction of neutrons")
        
        title = "Probabilities"
        plt.title(title)
        plt.tight_layout()
        
        filename = os.path.join(self.__output_dir, "probabilities.png")
        plt.savefig(filename, dpi=300)
        plt.close()
        print(f"✅ Saved plot: {filename}")

    def load_coords(self, filename):
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
                    continue
        return coords

    def plot_trajectories(self):
        # Definir trayectorias base (siempre absorbed y transmitted)
        trayectories = {
            "Absorbed": (f"out/{self.__data['run']['run_name']}/data/hist_absorbed.txt", "red"),
            "Escaped": (f"out/{self.__data['run']['run_name']}/data/hist_scaped.txt", "blue")
        }
        
        # Añadir reflejados solo para slab y double_slab
        if self.__shape in ["regular_slab", "double_slab"]:
            trayectories["Reflected"] = (f"out/{self.__data['run']['run_name']}/data/hist_reflected.txt", "green")

        fig = plt.figure(figsize=(9, 6))
        ax = fig.add_subplot(111, projection='3d')

        # Resto del código permanece igual...
        for label, (filename, color) in trayectories.items():
            coords = self.load_coords(filename)
            if not coords:
                continue
            x_vals = [c[0] for c in coords]
            y_vals = [c[1] for c in coords]
            z_vals = [c[2] for c in coords]
            ax.plot(x_vals, y_vals, z_vals, linewidth=2, color=color, label=label)

        # Add slabs visualization for regular_slab geometry
        if self.__shape == "regular_slab":
            # Get slab parameters from the configuration
            x_init = self.__data['geometry'].get('x_init')
            L = self.__data['geometry'].get('max_scale')
            x_end = x_init + L
            
            # Create planes at x_init and x_end
            y = np.linspace(-10, 10, 2)
            z = np.linspace(-10, 10, 2)
            Y, Z = np.meshgrid(y, z)

            # Plot initial plane
            X_init = np.full_like(Y, x_init)
            ax.plot_surface(X_init, Y, Z, alpha=0.2, color='gray', edgecolor='none')
            
            # Plot end plane
            X_end = np.full_like(Y, x_end)
            ax.plot_surface(X_end, Y, Z, alpha=0.2, color='gray', edgecolor='none')

            # Adjust x limits based on slab position
            padding = 2  # Extra space around the slab
            ax.set_xlim(min(x_init, x_end) - padding, max(x_init, x_end) + padding)
        elif self.__shape == "finite_slab":
            # Get dimensions from geometry configuration
            x_length = self.__data['geometry'].get('x_length', 1)
            y_length = self.__data['geometry'].get('y_length', 1)
            z_length = self.__data['geometry'].get('max_scale', 10)
            
            # Define the bounds of the parallelepiped
            x_min, x_max = -x_length/2, x_length/2
            y_min, y_max = -y_length/2, y_length/2
            z_min, z_max = 0, z_length
            
            # Create proper planar surfaces for each face
            def plot_rectangle(ax, point, u_vec, v_vec, **kwargs):
                u = np.linspace(0, 1, 2)
                v = np.linspace(0, 1, 2)
                u_grid, v_grid = np.meshgrid(u, v)
                x = point[0] + u_vec[0]*u_grid + v_vec[0]*v_grid
                y = point[1] + u_vec[1]*u_grid + v_vec[1]*v_grid
                z = point[2] + u_vec[2]*u_grid + v_vec[2]*v_grid
                ax.plot_surface(x, y, z, **kwargs)
            
            # Bottom face (z = z_min)
            plot_rectangle(ax, [x_min, y_min, z_min], 
                        [x_length, 0, 0], [0, y_length, 0],
                        alpha=0.2, color='gray', edgecolor='black')
            
            # Top face (z = z_max)
            plot_rectangle(ax, [x_min, y_min, z_max], 
                        [x_length, 0, 0], [0, y_length, 0],
                        alpha=0.2, color='gray', edgecolor='black')
            
            # Front face (y = y_min)
            plot_rectangle(ax, [x_min, y_min, z_min], 
                        [x_length, 0, 0], [0, 0, z_length],
                        alpha=0.2, color='gray', edgecolor='black')
            
            # Back face (y = y_max)
            plot_rectangle(ax, [x_min, y_max, z_min], 
                        [x_length, 0, 0], [0, 0, z_length],
                        alpha=0.2, color='gray', edgecolor='black')
            
            # Left face (x = x_min)
            plot_rectangle(ax, [x_min, y_min, z_min], 
                        [0, y_length, 0], [0, 0, z_length],
                        alpha=0.2, color='gray', edgecolor='black')
            
            # Right face (x = x_max)
            plot_rectangle(ax, [x_max, y_min, z_min], 
                        [0, y_length, 0], [0, 0, z_length],
                        alpha=0.2, color='gray', edgecolor='black')
            
            # Set appropriate axis limits
            padding = max(x_length, y_length, z_length) * 0.2
            ax.set_xlim(x_min - padding, x_max + padding)
            ax.set_ylim(y_min - padding, y_max + padding)
            ax.set_zlim(z_min - padding, z_max + padding)

        elif self.__shape == "sphere":
            radius = self.__data['geometry'].get('max_scale', 5)  # Usamos L como radio
            
            # Create a sphere mesh
            u = np.linspace(0, 2 * np.pi, 20)
            v = np.linspace(0, np.pi, 20)
            x = radius * np.outer(np.cos(u), np.sin(v))
            y = radius * np.outer(np.sin(u), np.sin(v))
            z = radius * np.outer(np.ones(np.size(u)), np.cos(v))
            
            # Plot the sphere surface with transparency
            ax.plot_surface(x, y, z, color='gray', alpha=0.2, edgecolor='black')
            
            # Set axis limits with some padding
            padding = radius * 1.2  # 20% padding
            ax.set_xlim(-padding, padding)
            ax.set_ylim(-padding, padding)
            ax.set_zlim(-padding, padding)
        elif self.__shape == "double_slab":
            x_init = self.__data['geometry'].get('x_init', 0)
            total_length = self.__data['geometry'].get('total_length', 10)
            L = self.__data['geometry'].get('max_scale', 0.5)  # Ratio entre los slabs
            
            # Calcular posiciones de los tres planos
            x_middle = x_init + total_length * L
            x_end = x_init + total_length
            
            # Crear planos
            y = np.linspace(-10, 10, 2)
            z = np.linspace(-10, 10, 2)
            Y, Z = np.meshgrid(y, z)
            
            # Plano inicial
            X_init = np.full_like(Y, x_init)
            ax.plot_surface(X_init, Y, Z, alpha=0.2, color='gray', edgecolor='none')
            
            # Plano medio (separación entre slabs)
            X_middle = np.full_like(Y, x_middle)
            ax.plot_surface(X_middle, Y, Z, alpha=0.2, color='blue', edgecolor='none')
            
            # Plano final
            X_end = np.full_like(Y, x_end)
            ax.plot_surface(X_end, Y, Z, alpha=0.2, color='gray', edgecolor='none')
            
            # Ajustar límites
            ax.set_xlim(x_init - 2, x_end + 2)
        

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
    parser = simple_parsing.ArgumentParser(add_help=True, description="Analyse the simulation results.")
    parser.add_arguments(TrainingCli, dest="cli")
    args = parser.parse_args()
    cli: TrainingCli = args.cli

    f = open(cli.configuration)
    data = json.load(f)
    
    plot_config = PlotConfig(cli, data)
    shutil.copy(cli.configuration, plot_config.getOutputDir())

    if (cli.no_multiple_geometries is False):
        L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std = load_simulation_data(f'out/{data["run"]["run_name"]}/data/simulations_output.txt')
        plot_config.plot_probabilities(L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std)

    if data['run'].get("save_hist", False):
        plot_config.plot_trajectories()

if __name__ == "__main__":
    main()