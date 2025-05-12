import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import argparse
import shutil
import json
import os

def parse_arguments():
    parser = argparse.ArgumentParser(description='Plot simulation results.')
    parser.add_argument('--configuration', type=str, required=True, help="Configuration file")
    return parser.parse_args()

def load_simulation_data(filename):
    # Cargar los datos del archivo, que ahora incluyen las desviaciones estándar
    data = np.loadtxt(filename, skiprows=1)
    L = data[:, 0]  # Longitudes de los slabs
    absorbed = data[:, 1]  # Probabilidad de absorción
    absorbed_std = data[:, 2]  # Desviación estándar de absorción
    reflected = data[:, 3]  # Probabilidad de reflexión
    reflected_std = data[:, 4]  # Desviación estándar de reflexión
    transmitted = data[:, 5]  # Probabilidad de transmisión
    transmitted_std = data[:, 6]  # Desviación estándar de transmisión
    return L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std

def clean_filename(value):
    return str(value).replace('.', 'p')

def plot_probabilities(L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std, data, output_dir):
    plt.figure(figsize=(8, 5))
    
    # Graficar las probabilidades con barras de error
    plt.errorbar(L, absorbed, yerr=absorbed_std, marker=".", label="Absorbed", color="red", fmt=':')
    plt.errorbar(L, reflected, yerr=reflected_std, marker=".", label="Reflected", color="green", fmt=':')
    plt.errorbar(L, transmitted, yerr=transmitted_std, marker=".", label="Transmitted", color="blue", fmt=':')
    
    plt.legend()
    plt.grid(True)
    plt.xlabel("Slab length (L)")
    plt.ylabel("Fraction of neutrons")
    
    # Título con los parámetros del material
    title = f"λ = {data['material']['mean_free_path']}, p_abs = {data['material']['pabs']}, k = {data['material']['k']}"
    plt.title(title)
    plt.tight_layout()

    # Guardar la gráfica
    fname_suffix = f"lambda_{clean_filename(data['material']['mean_free_path'])}_pabs_{clean_filename(data['material']['pabs'])}_k_{clean_filename(data['material']['k'])}"
    filename = os.path.join(output_dir, f"probabilities_{fname_suffix}.png")
    plt.savefig(filename, dpi=300)
    plt.close()
    print(f"✅ Saved plot: {filename}")

def load_coords(filename):
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

def plot_trajectories(data, output_dir):
    trayectories = {
    "Absorbed": (f"out/{data['run']['run_name']}/data/hist_absorbed.txt", "red"),
    "Reflected": (f"out/{data['run']['run_name']}/data/hist_reflected.txt", "green"),
    "Transmitted": (f"out/{data['run']['run_name']}/data/hist_transmitted.txt", "blue")
    }

    fig = plt.figure(figsize=(9, 6))
    ax = fig.add_subplot(111, projection='3d')

    for label, (filename, color) in trayectories.items():
        coords = load_coords(filename)
        if not coords:
            continue
        x_vals = [c[0] for c in coords]
        y_vals = [c[1] for c in coords]
        z_vals = [c[2] for c in coords]
        ax.plot(x_vals, y_vals, z_vals, linewidth=2, color=color, label=label)

    y = np.linspace(-10, 10, 2)
    z = np.linspace(-10, 10, 2)
    Y, Z = np.meshgrid(y, z)

    for x_val in [0, 10]:
        X = np.full_like(Y, x_val)
        ax.plot_surface(X, Y, Z, alpha=0.2, color='gray', edgecolor='none')

    ax.set_title("Neutron trajectories")
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    ax.set_xlim(-2, 12)
    ax.legend()
    plt.tight_layout()

    fname_suffix = f"lambda_{clean_filename(data['material']['mean_free_path'])}_pabs_{clean_filename(data['material']['pabs'])}_k_{clean_filename(data['material']['k'])}"
    filename = os.path.join(output_dir, f"trajectories_{fname_suffix}.png")
    plt.savefig(filename, dpi=300)
    plt.close()
    print(f"✅ Saved 3D trajectory plot: {filename}")


def main():
    args = parse_arguments()
    f = open(args.configuration)
    data = json.load(f)
    output_dir = f'out/{data["run"]["run_name"]}'
    os.makedirs(output_dir, exist_ok=True)
    shutil.copy(args.configuration, output_dir)
    L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std = load_simulation_data(f'out/{data["run"]["run_name"]}/data/simulations_output.txt')

    plot_probabilities(L, absorbed, absorbed_std, reflected, reflected_std, transmitted, transmitted_std, data, output_dir)

    if data.get("save_hist", True):
        plot_trajectories(data, output_dir)

if __name__ == "__main__":
    main()
