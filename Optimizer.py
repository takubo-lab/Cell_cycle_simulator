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


class MyApplication:
    def __init__(self):
        self.root = tk.Tk()
        self.filename = ""
        self.input_path = ""
        # create a StringVar to store the selected filename
        
        # create a button to get the filename
        self.filename_button = tk.Button(self.root, text="Select File", command=self.get_filename)
        self.filename_button.pack()

        # create text boxes for time_steps and initial_cells
        self.time_steps_label = tk.Label(self.root, text="Time Steps:")
        self.time_steps_label.pack()
        self.time_steps_entry = tk.Entry(self.root)
        self.time_steps_entry.pack()

        self.initial_cells_label = tk.Label(self.root, text="Initial Cells:")
        self.initial_cells_label.pack()
        self.initial_cells_entry = tk.Entry(self.root)
        self.initial_cells_entry.pack()

        self.n_simulations_label = tk.Label(self.root, text="Number of Simulations:")
        self.n_simulations_label.pack()
        self.n_simulations_entry = tk.Entry(self.root)
        self.n_simulations_entry.pack()

        # create a dropdown menu for selecting the optimizer program
        self.optimizer_label = tk.Label(self.root, text="Optimizer Program:")
        self.optimizer_label.pack()
        self.optimizer_var = tk.StringVar(self.root)
        self.optimizer_var.set("Simulated Annealing") # default value
        self.optimizer_menu = tk.OptionMenu(self.root, self.optimizer_var, "Simulated Annealing", "Genetic Algorithm", "Particle Swarm")
        self.optimizer_menu.pack()

        # Change the optimizer_var to the name of the actual exe file
        if self.optimizer_var.get() == "Simulated Annealing":
            self.simulator = "program\\optimizer_simulated_annealing.exe"
        elif self.optimizer_var.get() == "Genetic Algorithm":
            self.simulator = "program\\optimizer_GA.exe"
        elif self.optimizer_var.get() == "Particle Swarm":
            self.simulator = "program\\optimizer_particle_swarm.exe"

        self.execute_button = tk.Button(self.root, text="Run optimization", command=self.run_optimizer)
        self.execute_button.pack()

    # rest of your methods (e.g., get_filename, run_optimizer, ...) go here
    def get_filename(self):
        self.filename = filedialog.askopenfilename() # get the value from the StringVar
        self.input_path = os.path.abspath(self.filename)

    def create_output_directory(self):
        current_dir = os.path.dirname(os.path.realpath(__file__))
        self.output_dir = os.path.join(current_dir, "output")
        if not os.path.exists(self.output_dir):
            os.mkdir(self.output_dir)

    def run_optimizer(self):
        self.create_output_directory()

        now = datetime.datetime.now()
        current_dir = os.path.dirname(os.path.realpath(__file__))

        datetime_str = now.strftime("%Y-%m-%d_%H-%M-%S")
        output_filename = f"output\\optimized_variables_{datetime_str}.csv"
        output_filename = os.path.join(current_dir, output_filename)

        exe_path = os.path.join(current_dir, self.simulator)

        command = [exe_path, self.input_path, self.time_steps_entry.get(), self.initial_cells_entry.get(), self.n_simulations_entry.get(), output_filename]
        result = subprocess.run(command, capture_output=False, text=False)
        print(result)

        output_lines = result.stdout.split("\n")
        self.optimized_variables = []
        for line in output_lines:
            if line.startswith("p_div:"):
                parts = line.split(", ")
                p_div = float(parts[0].split(": ")[1])
                func_decline = float(parts[1].split(": ")[1])
                p_diff = float(parts[2].split(": ")[1])
                p_die = float(parts[3].split(": ")[1])
                self.optimized_variables.append((p_div, func_decline, p_diff, p_die))

        with open(output_filename, "w") as f:
            f.write("p_div,func_decline,p_diff,p_die\n")
            for variables in self.optimized_variables:
                f.write(f"{variables[0]},{variables[1]},{variables[2]},{variables[3]}\n")

        return self.optimized_variables
    def mainloop(self):
        self.root.mainloop()

def main():
    app = MyApplication()
    app.mainloop()

if __name__ == "__main__":
    main()