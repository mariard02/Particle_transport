import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import argparse
import os

def parse_arguments():
    parser = argparse.ArgumentParser(description='Plot simulation results.')
    parser.add_argument('--lambda', dest='mean_free_path', type=float, required=True,
                        help='Mean free path of neutrons in the material.')
    parser.add_argument('--pabs', type=float, required=True,
                        help='Probability of absorption of neutrons in the material.')
    parser.add_argument('--k', type=float, required=True,
                        help='Drag force.')
    return parser.parse_args()

def load_simulation_data(filename):
    data = np.loadtxt(filename, skiprows=1)
    L = data[:, 0]
    absorbed = data[:, 1]
    reflected = data[:, 2]
    transmitted = data[:, 3]
    return L, absorbed, reflected, transmitted

def clean_filename(value):
    return str(value).replace('.', 'p')

def plot_probabilities(L, absorbed, reflected, transmitted, args, output_dir):
    plt.figure(figsize=(8, 5))
    plt.plot(L, absorbed, marker="o", label="Absorbed", color="red")
    plt.plot(L, reflected, marker="o", label="Reflected", color="green")
    plt.plot(L, transmitted, marker="o", label="Transmitted", color="blue")
    plt.legend()
    plt.grid(True)
    plt.xlabel("Slab length (L)")
    plt.ylabel("Fraction of neutrons")
    title = f"λ = {args.mean_free_path}, p_abs = {args.pabs}, k = {args.k}"
    plt.title(title)
    plt.tight_layout()

    fname_suffix = f"lambda_{clean_filename(args.mean_free_path)}_pabs_{clean_filename(args.pabs)}_k_{clean_filename(args.k)}"
    filename = os.path.join(output_dir, f"probabilities_{fname_suffix}.png")
    plt.savefig(filename, dpi=300)
    plt.close()
    print(f"✅ Saved plot: {filename}")

def load_coords(filename):
    coords = []
    if not os.path.exists(filename):
        print(f"⚠️ Warning: File not found: {filename}")
        return coords

    with open(filename, "r") as file:
        for line in file:
            try:
                x, y, z = map(float, line.strip().split())
                coords.append((x, y, z))
            except ValueError:
                continue
    return coords

def plot_trajectories(args, output_dir):
    trayectories = {
        "Absorbed": ("data/hist_absorbed.txt", "red"),
        "Reflected": ("data/hist_reflected.txt", "green"),
        "Transmitted": ("data/hist_transmitted.txt", "blue")
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

    fname_suffix = f"lambda_{clean_filename(args.mean_free_path)}_pabs_{clean_filename(args.pabs)}_k_{clean_filename(args.k)}"
    filename = os.path.join(output_dir, f"trajectories_{fname_suffix}.png")
    plt.savefig(filename, dpi=300)
    plt.close()
    print(f"✅ Saved 3D trajectory plot: {filename}")

def main():
    args = parse_arguments()
    output_dir = "figures"
    os.makedirs(output_dir, exist_ok=True)

    L, absorbed, reflected, transmitted = load_simulation_data('data/simulations_output.txt')
    plot_probabilities(L, absorbed, reflected, transmitted, args, output_dir)
    plot_trajectories(args, output_dir)

if __name__ == "__main__":
    main()
