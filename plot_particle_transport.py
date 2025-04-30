import matplotlib.pyplot as plt
import numpy as np
import argparse

parser = argparse.ArgumentParser(description='Plot simulation results.')

parser.add_argument('--lambda', dest='mean_free_path', type=float, help='Mean free path of neutrons in the material.')
parser.add_argument('--pabs', type=float, help='Probability of absoroption of neutrons in the material.')
parser.add_argument('--k', type=float, help='Drag force.')

args = parser.parse_args()

data = np.loadtxt('data/simulations_output.txt', skiprows = 1)

L = data[:, 0]
absorbed = data[:, 1]
reflected = data[:, 2]
transmitted = data[:, 3]

plt.figure()
plt.plot(L, absorbed, marker = "o", label = "Absorbed")
plt.plot(L, reflected,  marker = "o", label = "Reflected")
plt.plot(L, transmitted,  marker = "o", label = "Transmitted")
plt.legend()

plt.title(f"λ = {args.mean_free_path}, p_abs = {args.pabs}, k = {args.k}")
plt.xlabel("Length of the slab")
plt.ylabel("Fraction of neutrons")
plt.show()