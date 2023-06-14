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

        # create a frame for column 1
        self.column1 = tk.Frame(self.root)
        self.column1.grid(column=0, row=0)

        # create a frame for column 2
        self.column2 = tk.Frame(self.root)
        self.column2.grid(column=1, row=0)

        # create a frame for column 3
        self.column3 = tk.Frame(self.root)
        self.column3.grid(column=2, row=0)

        # create a StringVar to store the selected filename
        self.filename_var = tk.StringVar(self.column1)

        # create a label to display the selected filename
        self.filename_label = tk.Label(self.column1, textvariable=self.filename_var)
        self.filename_label.grid(row=0)

        # create a button to get the filename, packed in column 1
        self.filename_button = tk.Button(self.column1, text="Select File", command=self.get_filename)
        self.filename_button.grid(row=1)

        # create text boxes for time_steps and initial_cells
        self.time_steps_label = tk.Label(self.column1, text="Time Steps:")
        self.time_steps_label.grid(row=2)
        self.time_steps_entry = tk.Entry(self.column1)
        self.time_steps_entry.insert(0,"30")
        self.time_steps_entry.grid(row=3)

        self.initial_cells_label = tk.Label(self.column1, text="Initial Cells:")
        self.initial_cells_label.grid(row=4)
        self.initial_cells_entry = tk.Entry(self.column1)
        self.initial_cells_entry.insert(0, "1")
        self.initial_cells_entry.grid(row=5)

        # create a dropdown menu for selecting the optimizer program
        self.optimizer_label = tk.Label(self.column1, text="Optimizer Program:")
        self.optimizer_label.grid(row=6)
        self.optimizer_var = tk.StringVar(self.column1)
        self.optimizer_var.set("Simulated Annealing") # default value
        self.optimizer_menu = tk.OptionMenu(self.column1, self.optimizer_var, "Simulated Annealing", "Genetic Algorithm", "Particle Swarm")
        self.optimizer_menu.grid(row=10)
        
        self.empty_label_1 = tk.Label(self.column1, text="")
        self.empty_label_1.grid(row=11)


        # Add entry fields for optimizer_GA parameters
        self.ga_population_size_label = tk.Label(self.column2, text="GA Hyper parameters")
        self.ga_population_size_label.grid(row=0)
        self.ga_population_size_label = tk.Label(self.column2, text="GA Population Size:")
        self.ga_population_size_label.grid(row=1)
        self.ga_population_size_entry = tk.Entry(self.column2)
        self.ga_population_size_entry.insert(0,"100")
        self.ga_population_size_entry.grid(row=2)
        
        self.ga_tournament_size_label = tk.Label(self.column2, text="GA Tournament Size:")
        self.ga_tournament_size_label.grid(row=3)
        self.ga_tournament_size_entry = tk.Entry(self.column2)
        self.ga_tournament_size_entry.insert(0,"5")
        self.ga_tournament_size_entry.grid(row=4)

        self.crossover_rate_label = tk.Label(self.column2, text="Crossover Rate:")
        self.crossover_rate_label.grid(row=5)
        self.crossover_rate_entry = tk.Entry(self.column2)
        self.crossover_rate_entry.insert(0, "0.8")
        self.crossover_rate_entry.grid(row=6)

        self.mutation_rate_label = tk.Label(self.column2, text="Mutation Rate:")
        self.mutation_rate_label.grid(row=7)
        self.mutation_rate_entry = tk.Entry(self.column2)
        self.mutation_rate_entry.insert(0, "0.1")
        self.mutation_rate_entry.grid(row=8)

        self.elitism_count_label = tk.Label(self.column2, text="Elitism Count:")
        self.elitism_count_label.grid(row=9)
        self.elitism_count_entry = tk.Entry(self.column2)
        self.elitism_count_entry.insert(0, "10")
        self.elitism_count_entry.grid(row=10)

        self.max_generations_label = tk.Label(self.column2, text="Max Generations:")
        self.max_generations_label.grid(row=11)
        self.max_generations_entry = tk.Entry(self.column2)
        self.max_generations_entry.insert(0, "100")
        self.max_generations_entry.grid(row=12)


        self.ga_population_size_label = tk.Label(self.column3, text="Simulated annealing Hyper parameters")
        self.ga_population_size_label.grid(row=0)
        self.initial_temperature_label = tk.Label(self.column3, text="Initial Temperature:")
        self.initial_temperature_label.grid(row=1)
        self.initial_temperature_entry = tk.Entry(self.column3)
        self.initial_temperature_entry.insert(0, "100.0")
        self.initial_temperature_entry.grid(row=2)

        self.final_temperature_label = tk.Label(self.column3, text="Final Temperature:")
        self.final_temperature_label.grid(row=3)
        self.final_temperature_entry = tk.Entry(self.column3)
        self.final_temperature_entry.insert(0, "0.1")
        self.final_temperature_entry.grid(row=4)

        self.cooling_rate_label = tk.Label(self.column3, text="Cooling Rate:")
        self.cooling_rate_label.grid(row=5)
        self.cooling_rate_entry = tk.Entry(self.column3)
        self.cooling_rate_entry.insert(0, "0.99")
        self.cooling_rate_entry.grid(row=6)

        self.max_iterations_label = tk.Label(self.column3, text="Max Iterations:")
        self.max_iterations_label.grid(row=7)
        self.max_iterations_entry = tk.Entry(self.column3)
        self.max_iterations_entry.insert(0, "1000")
        self.max_iterations_entry.grid(row=8)

        self.execute_button = tk.Button(self.column3, text="Run optimization", command=self.run_optimizer)
        self.execute_button.grid(row=9)

    # rest of your methods (e.g., get_filename, run_optimizer, ...) go here
    def get_filename(self):
        self.filename = filedialog.askopenfilename() # get the value from the StringVar
        self.input_path = os.path.abspath(self.filename)
        self.filename_var.set(self.filename)

    def create_output_directory(self):
        current_dir = os.path.dirname(os.path.realpath(__file__))
        self.output_dir = os.path.join(current_dir, "output")
        if not os.path.exists(self.output_dir):
            os.mkdir(self.output_dir)

    def create_output_filename(self,simulator):
        now = datetime.datetime.now()
        current_dir = os.path.dirname(os.path.realpath(__file__))

        datetime_str = now.strftime("%Y-%m-%d_%H-%M-%S")
        output_filename = f"output\\optimized_variables_{simulator}_{datetime_str}.csv"
        output_filename = os.path.join(current_dir, output_filename)

        return output_filename

    def parse_output(self, output_lines):
        optimized_variables = []
        for line in output_lines:
            if line.startswith("p_div:"):
                parts = line.split(", ")
                p_div = float(parts[0].split(": ")[1])
                func_decline = float(parts[1].split(": ")[1])
                p_diff = float(parts[2].split(": ")[1])
                p_die = float(parts[3].split(": ")[1])
                optimized_variables.append((p_div, func_decline, p_diff, p_die))

        return optimized_variables


    def run_optimizer(self):
        self.create_output_directory()
        current_dir = os.path.dirname(os.path.realpath(__file__))
        command = []
        exe_path =""
        params = []

        # Change the optimizer_var to the name of the actual exe file
        if self.optimizer_var.get() == "Simulated Annealing":
            self.simulator = "program\\optimizer_simulated_annealing.exe"
            exe_path = os.path.join(current_dir, self.simulator)
            output_filename = self.create_output_filename("SA")
            params = [self.initial_temperature_entry.get(), self.final_temperature_entry.get(), self.cooling_rate_entry.get(),self.max_iterations_entry.get(),output_filename]
            
        elif self.optimizer_var.get() == "Genetic Algorithm":
            self.simulator = "program\\optimizer_GA.exe"
            exe_path = os.path.join(current_dir, self.simulator)
            output_filename = self.create_output_filename("GA")
            params = [self.ga_population_size_entry.get(),self.ga_tournament_size_entry.get(),self.crossover_rate_entry.get(),self.mutation_rate_entry.get(),self.elitism_count_entry.get(),self.max_generations_entry.get(),output_filename]
            

        elif self.optimizer_var.get() == "Particle Swarm":
            self.simulator = "program\\optimizer_particle_swarm.exe"

        command = [exe_path, self.input_path, self.time_steps_entry.get(), self.initial_cells_entry.get()]
        command = command + params

        subprocess.run(command, capture_output=False, text=False)
    

    def mainloop(self):
        self.root.mainloop()

def main():
    app = MyApplication()
    app.mainloop()

if __name__ == "__main__":
    main()