import os
import subprocess
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
from tkinter import filedialog
import matplotlib.animation as animation
import numpy as np
import datetime
import threading


def create_output_directory():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    output_dir = os.path.join(current_dir, "output")
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

def run_optimizer(filename, time_steps, initial_cells, n_simulations, simulator):
    create_output_directory()

    now = datetime.datetime.now()
    current_dir = os.path.dirname(os.path.realpath(__file__))

    datetime_str = now.strftime("%Y-%m-%d_%H-%M-%S")
    output_filename = f"output\\optimized_variables_{datetime_str}.csv"
    output_filename = os.path.join(current_dir,output_filename)
    

    exe_path = os.path.join(current_dir,simulator)

    input_path = os.path.abspath(filename)
    command = [exe_path, input_path, time_steps, initial_cells, n_simulations,output_filename]
    result = subprocess.run(command, capture_output=True, text=True)
    print(result)
    #subprocess.run(command)


    output_lines = result.stdout.split("\n")
    optimized_variables = []
    for line in output_lines:
        if line.startswith("p_div:"):
            parts = line.split(", ")
            p_div = float(parts[0].split(": ")[1])
            func_decline = float(parts[1].split(": ")[1])
            p_diff = float(parts[2].split(": ")[1])
            p_die = float(parts[3].split(": ")[1])
            fitness = float(parts[4].split(": ")[1])
            optimized_variables.append((p_div, func_decline, p_diff, p_die, fitness))

    with open(output_filename, "w") as f:
        f.write("p_div,func_decline,p_diff,p_die,fitness\n")
        for variables in optimized_variables:
            f.write(f"{variables[0]},{variables[1]},{variables[2]},{variables[3]},{variables[4]}\n")

    return optimized_variables

def animate(i):
    current_dir =  os.path.dirname(os.path.realpath(__file__))
    filename = os.path.join(current_dir,"output","fitness.csv")
    filename = "C:\\Users\\hikob\\fitness.csv"
    if not os.path.exists(filename):
        return
    
    data = np.loadtxt(filename, delimiter=',')
    #print(data[0])
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)
    ax.clear()
    ax.plot(data[:, 0], data[:, 1])
    plt.xlabel('Generation')
    plt.ylabel('Fitness')
    plt.title('Fitness over Generations')

def plot_line():
    fig = plt.figure()
    ani = animation.FuncAnimation(fig, animate, interval=10000)
    plt.show()

def delete_fitness_file():
    filename = "C:\\Users\\hikob\\OneDrive\\Desktop\\output\\fitness.csv"
    if os.path.exists(filename):
        os.remove(filename)

def get_filename(filename_var):
    filename = filedialog.askopenfilename()
    filename_var.set(filename)

def main():

    root = tk.Tk()

    # create a StringVar to store the selected filename
    filename_var = tk.StringVar()
# create a button to get the filename
    filename_button = tk.Button(root, text="Select File", command=lambda: get_filename(filename_var))
    filename_button.pack()

    # create text boxes for time_steps and initial_cells
    time_steps_label = tk.Label(root, text="Time Steps:")
    time_steps_label.pack()
    time_steps_entry = tk.Entry(root)
    time_steps_entry.pack()

    initial_cells_label = tk.Label(root, text="Initial Cells:")
    initial_cells_label.pack()
    initial_cells_entry = tk.Entry(root)
    initial_cells_entry.pack()

    n_simulations_label = tk.Label(root, text="Number of Simulations:")
    n_simulations_label.pack()
    n_simulations_entry = tk.Entry(root)
    n_simulations_entry.pack()

    # create a dropdown menu for selecting the optimizer program
    optimizer_label = tk.Label(root, text="Optimizer Program:")
    optimizer_label.pack()
    optimizer_var = tk.StringVar(root)
    optimizer_var.set("Simulated Annealing") # default value
    optimizer_menu = tk.OptionMenu(root, optimizer_var, "Simulated Annealing", "Genetic Algorithm", "Particle Swarm")
    optimizer_menu.pack()

    # Change the optimizer_var to the name of the actual exe file
    if optimizer_var.get() == "Simulated Annealing":
        simulator = "program\\optimizer_simulated_annealing.exe"
    elif optimizer_var.get() == "Genetic Algorithm":
        simulator = "program\\optimizer_GA.exe"
    elif optimizer_var.get() == "Particle Swarm":
        simulator = "program\\optimizer_particle_swarm.exe"


    execute_button = tk.Button(root, text="Run optimization", command=lambda: run_optimizer(filename_var, lambda: time_steps_entry.get(), lambda: initial_cells_entry.get(),lambda: n_simulations_entry.get(),simulator))
    execute_button.pack()

    root.mainloop()
    


if __name__ == "__main__":
    main()