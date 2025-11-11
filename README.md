# 🧠 Task Distribution Application Using Dijkstra's Algorithm

### C++/CLI Windows Forms Coursework Project

This software application implements a **task allocation model** using **Dijkstra’s algorithm** to minimize total execution time.
It provides a **graphical interface** that allows users to add tasks, define dependencies, and distribute them among available workers.

---

## 🏗️ 1. General Program Structure

The program consists of three main files:

| File            | Description                                                                             |
| --------------- | --------------------------------------------------------------------------------------- |
| **MyForm.h**    | Header file containing the declaration of the form class and the `Task` data structure. |
| **MyForm.cpp**  | Implementation file that contains the function definitions for the form class.          |
| **Program.cpp** | Entry point that launches the graphical user interface (GUI).                           |

The program is implemented using **C++/CLI** and **Windows Forms** to create a GUI for managing and visualizing task allocation.

---

## 💾 2. Main Data Structures

### **Task Structure**

Used to store information about a single task:

* `name` — name of the task
* `cost` — task cost (complexity or duration)
* `mandatory` — flag indicating whether the task is mandatory
* `taskId` — unique task identifier

### **Other Data Structures (MyForm class)**

| Variable      | Description                                                                         |
| ------------- | ----------------------------------------------------------------------------------- |
| `tasks`       | List of tasks (`List<Task^>`)                                                       |
| `taskWeights` | Matrix of edge weights (execution time between tasks), size `MAX_TASKS × MAX_TASKS` |
| `nextTaskId`  | Counter for assigning unique IDs to new tasks                                       |

---

## 🖥️ 3. Description of the User Interface

The application window is divided into several logical sections:

### ➕ **Adding Tasks**

* Fields for **task name** and **cost**
* Checkbox for marking the task as **mandatory**
* Buttons for **adding** and **removing** tasks
* List box displaying all added tasks

### 🔗 **Setting Task Connections**

* Drop-down menus for selecting **start** and **end** tasks
* Field for entering the **execution time (weight)**
* Buttons to **add** or **remove** connections
* List of existing connections

### ⚙️ **Task Distribution**

* Field to input the **number of workers**
* Button to start **task distribution**
* Output window displaying **results**

### 📁 **File Operations**

* Button to **save** data to a file
* Button to **load** data from a file

---

## 🧩 4. Structural Diagram and Module Descriptions

The application consists of **14 modules**, each performing a specific function.

### 🔹 `UpdateTaskCountLabel()`

Updates the label showing the number of tasks and disables the **Add Task** button when the maximum number of tasks (`MAX_TASKS`) is reached.

```cpp
void MyForm::UpdateTaskCountLabel() {
    lblTaskCount->Text = String::Format("Tasks added: {0}/{1}", tasks->Count, MAX_TASKS);
    btnAddTask->Enabled = (tasks->Count < MAX_TASKS);
}
```

---

### 🔹 `UpdateTaskList()`

Clears and refills the list of tasks. Also updates combo boxes and counters.

```cpp
void MyForm::UpdateTaskList() {
    listTasks->Items->Clear();
    for (int i = 0; i < tasks->Count; i++) {
        Task^ task = tasks[i];
        String^ displayText = String::Format(
            "[{0}] {1} (Cost: {2}, {3})",
            task->taskId, task->name, task->cost,
            task->mandatory ? "Mandatory" : "Optional"
        );
        listTasks->Items->Add(displayText);
    }
    UpdateTaskCombos();
    UpdateTaskCountLabel();
}
```

---

### 🔹 `btnAddTask_Click()`

Handles adding a new task after validating user input.

```cpp
void MyForm::btnAddTask_Click(System::Object^ sender, System::EventArgs^ e) {
    int cost;
    if (!Int32::TryParse(txtTaskCost->Text, cost) || cost <= 0) {
        MessageBox::Show("Cost should be a positive number", "Input error");
        return;
    }

    Task^ newTask = gcnew Task(txtTaskName->Text, cost, chkMandatory->Checked, nextTaskId++);
    tasks->Add(newTask);
    UpdateTaskList();

    txtTaskName->Clear();
    txtTaskCost->Clear();
    chkMandatory->Checked = false;
}
```

---

### 🔹 `btnRemoveTask_Click()`

Removes the selected task and all its associated edges.

```cpp
void MyForm::btnRemoveTask_Click(System::Object^ sender, System::EventArgs^ e) {
    int selectedIndex = listTasks->SelectedIndex;
    if (selectedIndex >= 0 && selectedIndex < tasks->Count) {
        int taskId = tasks[selectedIndex]->taskId;
        for (int i = 0; i < MAX_TASKS; i++) {
            taskWeights[taskId][i] = 0;
            taskWeights[i][taskId] = 0;
        }
        tasks->RemoveAt(selectedIndex);
        UpdateTaskList();
        UpdateWeightsList();
    } else {
        MessageBox::Show("Please choose a task to delete", "Error");
    }
}
```

---

### 🔹 `UpdateTaskCombos()`

Updates the combo boxes used for defining task connections.

### 🔹 `UpdateWeightsList()`

Displays all task relationships (edges) based on the `taskWeights` matrix.

### 🔹 `btnAddWeight_Click()` / `btnRemoveWeight_Click()`

Add or remove edges between tasks after validating input.

---

### 🔹 `FindEarliestAvailableWorker()`

Finds the earliest available worker based on previous task completion time and workload.

---

### 🔹 `DijkstraSingleSource()` & `MinDistance()`

Implement **Dijkstra’s algorithm** for finding the shortest paths between tasks in the dependency graph.

---

### 🔹 `AllocateTasksUsingDijkstra()`

Performs **task allocation among workers**, considering dependencies and earliest possible start times.

Steps include:

1. Running Dijkstra’s algorithm for all active tasks
2. Dividing tasks into **mandatory** and **optional**
3. Sorting tasks by start time
4. Assigning each to the **earliest available worker**

---

### 🔹 `OutputAllocationResultsWithDijkstra()`

Displays the results of the allocation process.

### 🔹 File Handling Modules

* **Save to file** — writes tasks and connections using `SaveFileDialog`
* **Load from file** — reads data and reconstructs the graph structure

---

## 🧾 5. Conclusion

This coursework project demonstrates a **software system that models task distribution among workers using Dijkstra’s algorithm**.

### ✅ Achievements:

* Implementation of **Dijkstra’s algorithm** for optimal task assignment
* Development of a **graphical interface** for user interaction
* Functionality for **data saving and loading**
* Visualization of tasks, dependencies, and results

### 💡 Educational Outcome:

The project provides a practical understanding of **graph theory in programming** and its application to real-world task scheduling problems.
Creating the GUI also enhances visualization and user interaction, making it easier to perceive the sequence of operations.

### 🚀 Future Enhancements:

* Add user account identification
* Implement saving/loading of full program state
* Extend analysis and visualization features

---

## 📚 Source List

1. [GeeksforGeeks — Dijkstra's Algorithm](https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/)
2. [Saylor Academy — Dijkstra's Algorithm](https://learn.saylor.org/mod/page/view.php?id=19015&forceview=1)
3. [TutorialsPoint — Dijkstra’s Shortest Path Algorithm](https://www.tutorialspoint.com/data_structures_algorithms/dijkstras_shortest_path_algorithm.htm)
4. [GeeksforGeeks — Introduction to .NET Framework](https://www.geeksforgeeks.org/introduction-to-net-framework/)
5. [Simplilearn — Creating C++ GUI Applications](https://www.simplilearn.com/tutorials/cpp-tutorial/cpp-gui)
