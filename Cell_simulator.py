import os
import subprocess
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import numpy as np



    # C++プログラムを実行する

def run_simulation():


    divide_prob = float(divide_prob_entry.get())
    death_prob = float(death_prob_entry.get())
    diff_prob = float(diff_prob_entry.get())
    func_decline = float(func_decline_entry.get())
    time_steps = int(time_steps_entry.get())
    initial_cells = int(initial_cells_entry.get())
    n_simulations = int(n_simulations_entry.get())

    current_dir = os.path.dirname(os.path.realpath(__file__))
    exe_path = os.path.join(current_dir, "Cell_simulator.exe")

    if divide_prob +death_prob + diff_prob <= 1:
        label.config(text="")
        result = subprocess.run([exe_path, str(divide_prob), str(death_prob),
                             str(diff_prob), str(func_decline), str(time_steps),
                             str(initial_cells), str(n_simulations)],
                             capture_output=True, text=True)
        plot_bar_graph(result)
    
    else:
        label.config(text="確率の総和は1以下にしてください")
        
def save_data(data):
    current_dir = os.path.dirname(os.path.realpath(__file__))
    filename = os.path.join(current_dir, "data.csv")
    np.savetxt(filename, [x[0] for x in data], delimiter="\n")
   
    

    # 出力を解析する
def plot_bar_graph(result):

    output = result.stdout.split('\n')
    data = [list(map(int, line.split())) for line in output if line]

    # データを細胞数でソートする
    data.sort(key=lambda x: x[0], reverse=True)
    print(data)
    save_data(data)
    # データを棒グラフとしてプロットする
    plt.bar(range(len(data)), [x[0] for x in data], tick_label=[x[1] for x in data])
    plt.xlabel('Simulation Result')
    plt.ylabel('Number of Cells')
    plt.show()

def main():
    global label
    global divide_prob_entry
    global diff_prob_entry  
    global death_prob_entry  
    global func_decline_entry
    global time_steps_entry
    global initial_cells_entry
    global n_simulations_entry

    root = tk.Tk()
    label = tk.Label(root)
    label.pack()

    divide_prob_label = ttk.Label(root, text="Divide Probability")
    divide_prob_label.pack()
    divide_prob_entry = ttk.Entry(root)
    divide_prob_entry.insert(0, '0.5')
    divide_prob_entry.pack()

    diff_prob_label = ttk.Label(root, text="Differentiation Probability")
    diff_prob_label.pack()
    diff_prob_entry = ttk.Entry(root)
    diff_prob_entry.insert(0, '0.2')
    diff_prob_entry.pack()

    death_prob_label = ttk.Label(root, text="Death Probability")
    death_prob_label.pack()
    death_prob_entry = ttk.Entry(root)
    death_prob_entry.insert(0, '0.1')
    death_prob_entry.pack()


    func_decline_label = ttk.Label(root, text="Function Decline d: 1/(1+ d * t)")
    func_decline_label.pack()
    func_decline_entry = ttk.Entry(root)
    func_decline_entry.insert(0, '1')
    func_decline_entry.pack()

    time_steps_label = ttk.Label(root, text="Time Steps")
    time_steps_label.pack()
    time_steps_entry = ttk.Entry(root)
    time_steps_entry.insert(0, '30')
    time_steps_entry.pack()

    initial_cells_label = ttk.Label(root, text="Initial Cells")
    initial_cells_label.pack()
    initial_cells_entry = ttk.Entry(root)
    initial_cells_entry.insert(0, '1')
    initial_cells_entry.pack()

    n_simulations_label = ttk.Label(root, text="Number of Simulations")
    n_simulations_label.pack()
    n_simulations_entry = ttk.Entry(root)
    n_simulations_entry.insert(0, '100')
    n_simulations_entry.pack()

    run_button = ttk.Button(root, text='Run Simulation', command=run_simulation)
    run_button.pack()

    root.mainloop()

if __name__ == "__main__":
    main()