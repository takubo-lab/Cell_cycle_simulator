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


def run_optimizer(filename):
    now = datetime.datetime.now()
    current_dir = os.path.dirname(os.path.realpath(__file__))

    datetime_str = now.strftime("%Y-%m-%d_%H-%M-%S")
    output_filename = f"optimized_variables_{datetime_str}.csv"
    output_filename = os.path.join(current_dir,output_filename)
    exe_path = os.path.join(current_dir, "optimizer_GA.exe")

    input_path = os.path.abspath(filename)
    command = [exe_path, input_path]
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
    #current_dir =  os.path.dirname(os.path.realpath(__file__))
    #filename = os.path.join(current_dir,"fitness.csv")
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
    filename = "C:\\Users\\hikob\\fitness.csv"
    if os.path.exists(filename):
        os.remove(filename)


def main():

    root = tk.Tk()
    root.withdraw() # we don't want a full GUI, so keep the root window from appearing
    filename = filedialog.askopenfilename() # show an "Open" dialog box and return the path to the selected file
    
    optimizer_thread = threading.Thread(target=run_optimizer, args=(filename,))
    optimizer_thread.start()
    # call the C++ simulator with the filename as a command line argument
    #plot_line()
    optimizer_thread.join()
    delete_fitness_file()
    run_optimizer(filename)
    
    


if __name__ == "__main__":
    main()