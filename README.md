
# Cell cycle simulator

Welcome to our comprehensive suite of cell biology tools, encompassing both an Optimizer and a Cell Simulator.

The Optimizer is designed to find the optimal parameters for the simulator, namely the probabilities of cell division, differentiation, and death, as well as the decline rate of division probability after differentiation. These parameters are adjusted iteratively to best replicate the actual cell counts observed in real-life experiments, thus providing a robust model for cell growth and behavior.

On the other hand, the Cell Simulator provides a Graphical User Interface (GUI) to simulate cell proliferation. Users can specify probabilities associated with cell division, differentiation, and death, along with the function decline parameter, number of simulation time steps, initial cell count, and number of simulations. Multiple simulations can be run with results displayed as a bar graph, enabling visualization of cell behavior under different conditions.

Together, these tools offer a unique opportunity for researchers and enthusiasts to explore, understand, and manipulate cellular behaviors, providing valuable insights into cell biology.



# Cell_simulator.py
This is a GUI for simulating cell proliferation. In this program, the user can specify the probabilities of cell division, differentiation, and death, as well as the function decline parameter, the number of time steps for the simulation, the initial number of cells, and the number of simulations. The simulation can be run multiple times, and the results are displayed as a bar graph.


## Installation

No installation is needed. Clone the repository and run the Python file:

```
git clone https://github.com/takubo-lab/Cell_cycle_simulator.git
cd your-repository
python3 Cell_simulator.py
```

## Dependencies

To run this application, you will need:

* Python 3
* Tkinter
* matplotlib
* numpy


## Usage

1. Run the script.
2. Set parameters in the text boxes on the GUI.
    - **Divide Probability**: Specify the probability of a cell dividing.
    - **Differentiation Probability**: Specify the probability of a cell differentiating.
    - **Death Probability**: Specify the probability of a cell dying.
    - **Function Decline**: Specify the function decline parameter. This determines the degree to which the probability of cell division decreases over time.
    - **Time Steps**: Specify the number of time steps for the simulation.
    - **Initial Cells**: Specify the number of initial cells for the simulation.
    - **Number of Simulations**: Specify the number of times the simulation will be run.
3. Click the 'Run Simulation' button. The simulation will run and the results will be displayed as a bar graph.





# Optimizer.py

This Python-based graphical user interface (GUI) optimizes parameters for a cell division simulation. The program uses different optimization algorithms such as Simulated Annealing, Genetic Algorithm, and Particle Swarm.

## Features

* Load data from CSV files.
* Input fields for time steps and initial cells.
* Choice of optimizer program from Simulated Annealing, Genetic Algorithm, and Particle Swarm.
* Hyperparameters settings for each optimization algorithm.


## Usage

1. Open the GUI by running the Python file.
2. Click the "Select File" button to load your CSV data.
3. Input the desired time steps and initial cells.
4. Select the desired optimizer program.
5. Input the required hyperparameters for the selected optimization algorithm.
6. Click the "Run optimization" button to start the optimization process.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

[MIT](https://choosealicense.com/licenses/mit/)
