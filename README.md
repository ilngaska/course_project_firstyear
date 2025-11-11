**1 General program structure**

The program consists of three main files:
MyForm.h – header file containing the form class declaration and data structure for tasks.
MyForm.cpp – implementation file containing the form class function code.
Program.cpp – entry point of the program that launches the graphical interface.
The program is implemented using C++/CLI (Common Language Infrastructure) and Windows Forms to create a graphical interface.

**2 Main data structures**

Task – structure for storing information about the task:
name – task name;
cost – cost (complexity) of the task;
mandatory – flag indicating whether the task is mandatory;
taskId – unique task identifier.
Other data structures of the MyForm class
tasks – list of tasks (List<Task^> is used);
taskWeights – matrix of weights (execution time) between tasks of size MAX_TASKS×MAX_TASKS;
nextTaskId – counter for assigning unique identifiers to new tasks.

**3 Description of the user interface**

Adding tasks:
fields for entering the task name, its cost;
checkbox for marking the task as mandatory;
buttons for adding and removing tasks;
list of added tasks.

Setting up connections between tasks:
drop-down lists for selecting the initial and final task;
field for entering the execution time (connection weight);
buttons for adding and removing connections;
list of added connections.

Starting distribution:
field for entering the number of executors;
button for starting the distribution algorithm;
window for displaying results.

Working with files:
button for saving to a file;
button for reading from a file.

4 Structural diagram of the program and description of the main modules of the program

The structural diagram of the software application consists of 14 modules, each of which is responsible for performing certain actions.
In the UpdateTaskCountLabel() module, the label with the number of tasks added is updated, and the add task button is also blocked when the MAX_TASKS limit is reached. Listing of the UpdateTaskCountLabel() function:
void MyForm::UpdateTaskCountLabel() {
lblTaskCount->Text = String::Format("Tasks added: {0}/{1}", tasks->Count, MAX_TASKS);

// if the limit is reached, deactivate the add task button
if (tasks->Count >= MAX_TASKS) {
btnAddTask->Enabled = false;
}
else {
btnAddTask->Enabled = true;
}
}
In the UpdateTaskList() module, the list of tasks listTasks is cleared and refilled. In addition, the combo boxes and the task counter are updated. Listing of the UpdateTaskList() function:
void MyForm::UpdateTaskList() {
listTasks->Items->Clear();
for (int i = 0; i < tasks->Count; i++) {
Task^ task = tasks[i];
String^ displayText = String::Format("[{0}] {1} (Cost: {2}, {3})",
task->taskId,
task->name,
task->cost,
task->mandatory ? "Mandatory" : "Optional");
listTasks->Items->Add(displayText);
}

// update combo boxes with the number of tasks
UpdateTaskCombos();

// update information about the number of tasks
UpdateTaskCountLabel();
}
The btnAddTask_Click() module processes the event of clicking the add task button. Data validation, creation of the Task object, its addition to the list, and clearing of the input fields are performed. Listing of the btnAddTask_Click() function:
void MyForm::btnAddTask_Click(System::Object^ sender, System::EventArgs^ e) {
int cost;
if (!Int32::TryParse(txtTaskCost->Text, cost) || cost <= 0) {
MessageBox::Show("Cost should be a positive number", "Input error");
return;
}

// Create a new task and add it to the list
Task^ newTask = gcnew Task(txtTaskName->Text, cost, chkMandatory->Checked, nextTaskId);
nextTaskId++; // increment the task ID counter
tasks->Add(newTask);

// update the task list
UpdateTaskList();

// clear the input fields
txtTaskName->Clear();
txtTaskCost->Clear();
chkMandatory->Checked = false;
}
}
The btnRemoveTask_Click() module implements the removal of the selected task and all its connections in the graph. After that, the task and weight lists are updated. Listing of the btnRemoveTask_Click() function:
void MyForm::btnRemoveTask_Click(System::Object^ sender, System::EventArgs^ e) {
int selectedIndex = listTasks->SelectedIndex;
if (selectedIndex >= 0 && selectedIndex < tasks->Count) {
// get the ID of the deleted task
int taskId = tasks[selectedIndex]->taskId;

// remove all connections with this task
for (int i = 0; i < MAX_TASKS; i++) {
taskWeights[taskId][i] = 0;
taskWeights[i][taskId] = 0;
}

// remove the task from the list
tasks->RemoveAt(selectedIndex);

// update the lists
UpdateTaskList();
UpdateWeightsList();
}
else {
MessageBox::Show("Please choose a task to delete", "Error");
}}
The UpdateTaskCombos() module updates the lists in the cmbFromTask and cmbToTask combo boxes, which are used to create relationships between tasks.
with the same tasks The user's previous selection is saved. Listing of the UpdateTaskCombos() function:
void MyForm::UpdateTaskCombos() {
// Save the selection
Object^ fromSelected = cmbFromTask->SelectedItem;
Object^ toSelected = cmbToTask->SelectedItem;

// Clear the combo boxes
cmbFromTask->Items->Clear();
cmbToTask->Items->Clear();

// Fill the combo boxes with tasks
for (int i = 0; i < tasks->Count; i++) {
Task^ task = tasks[i];
String^ displayText = String::Format("[{0}] {1}", task->taskId, task->name);
cmbFromTask->Items->Add(displayText);
cmbToTask->Items->Add(displayText);
}

// Restore selection if possible
if (fromSelected != nullptr && cmbFromTask->Items->Contains(fromSelected)) {
cmbFromTask->SelectedItem = fromSelected;
}

if (toSelected != nullptr && cmbToTask->Items->Contains(toSelected)) {
cmbToTask->SelectedItem = toSelected;
}
}
The UpdateWeightsList() module generates a list of existing relationships between tasks, which are stored as a taskWeights matrix. Listing of the UpdateWeightsList() function:
void MyForm::UpdateWeightsList() {
listWeights->Items->Clear();

// Check each pair of indices in the weight matrix
for (int i = 0; i < MAX_TASKS; i++) {
for (int j = 0; j < MAX_TASKS; j++) {
if (taskWeights[i][j] > 0) {
Task^ fromTask = nullptr;
Task^ toTask = nullptr;

// Search for tasks by ID
for each (Task ^ task in tasks) {
if (task->taskId == i)
fromTask = task;
if (task->taskId == j)
toTask = task;
}

if (fromTask != nullptr && toTask != nullptr) {
String^ displayText = String::Format("[{0}] {1} -> [{2}] {3}: {4}",
fromTask->taskId, fromTask->name,
toTask->taskId, toTask->name,
taskWeights[i][j]);
listWeights->Items->Add(displayText);
}
}
}
}
}
The btnAddWeight_Click() module implements adding a link (edge) between two tasks. The correctness of the input, the difference of tasks, the admissibility of the ID are checked, and then the weight matrix and the list of links are updated.
The btnRemoveWeight_Click() module implements removing a link between tasks based on the selected row in listWeights. After analyzing the task IDs, the corresponding link is removed from the matrix.
The FindEarliestAvailableWorker() module searches for the nearest available worker, taking into account both the time of its previous work completion and the current workload. Listing of the FindEarliestAvailableWorker() function:
int MyForm::FindEarliestAvailableWorker(cli::array<int>^ workerEndTime, cli::array<int>^ workerLoad, int taskEarliestStart, int workers)
{
int minIndex = 0;
int minTime = Math::Max(workerEndTime[0], taskEarliestStart);

for (int i = 1; i < workers; i++) {
int workerAvailableTime = Math::Max(workerEndTime[i], taskEarliestStart);

if (workerAvailableTime < minTime) {
minTime = workerAvailableTime;
minIndex = i;
}
else if (workerAvailableTime == minTime && workerLoad[i] < workerLoad[minIndex]) {
// If the time is the same, we prefer the worker with a lower load
minIndex = i;
}
}

return minIndex;
}
The DijkstraSingleSource() and MinDistance() modules implement Dijkstra's algorithm for calculating the shortest paths in the dependency graph between tasks. MinDistance() is an auxiliary function for finding the next vertex with the minimum distance.
The AllocateTasksUsingDijkstra() module performs a step-by-step distribution of tasks between workers, taking into account the dependencies between tasks.
Listing of the AllocateTasksUsingDijkstra() function:
// We apply Dijkstra's algorithm for each task
for each (int taskId in activeTaskIds) {
cli::array<int>^ distances = gcnew cli::array<int>(MAX_TASKS);
cli::array<bool>^ visited = gcnew cli::array<bool>(MAX_TASKS);
for (int i = 0; i < MAX_TASKS; i++) {
distances[i] = Int32::MaxValue / 2;
visited[i] = false;
}
distances[taskId] = 0;
// Dijkstra's algorithm for finding the shortest path to the vertices
for (int count = 0; count < MAX_TASKS - 1; count++) {
int minDistance = Int32::MaxValue / 2;
int minIndex = -1;
// Find the distance among the raw ones with the minimum distance
for (int i = 0; i < MAX_TASKS; i++) {
if (!visited[i] && distances[i] < minDistance) {
minDistance = distances[i];
minIndex = i;
}
}
// If there are no available vertices, exit the loop
if (minIndex == -1) break;
visited[minIndex] = true;
// Update the distance to neighboring vertices
for (int v = 0; v < MAX_TASKS; v++) {
// Update the distance if:
// 1. There is an edge from minIndex to v
// 2. v has not been processed yet
// 3. The current distance to minIndex is not infinity
// 4. The path through minIndex is shorter than the current distance to v
if (!visited[v] &&
transposedGraph[minIndex][v] > 0 &&
distances[minIndex] != Int32::MaxValue / 2 &&
distances[minIndex] + transposedGraph[minIndex][v] < distances[v]) {
distances[v] = distances[minIndex] + transposedGraph[minIndex][v];
}
}
}
// Update the start time
for (int i = 0; i < MAX_TASKS; i++) {
if (distances[i] < Int32::MaxValue / 2 && distances[i] > earliestStartTime[i]) {
earliestStartTime[i] = distances[i];
}
}
}
}
// STEP 3: Divide tasks into mandatory and optional
// STEP 4: Sort tasks by execution time
// Mandatory tasks
for (int i = 0; i < mandatoryTasks->Count - 1; i++) {
for (int j = i + 1; j < mandatoryTasks->Count; j++) {
if (earliestStartTime[mandatoryTasks[i]->taskId] > earliestStartTime[mandatoryTasks[j]->taskId]) {
Task^ temp = mandatoryTasks[i];
mandatoryTasks[i] = mandatoryTasks[j];
mandatoryTasks[j] = temp;
}
}
}
// Optional tasks follow the same principle
// STEP 5: Determine workers {}
// STEP 6: Determine the actual start and end time of tasks{}
// STEP 7: Distribute mandatory tasks
for each (Task ^ task in mandatoryTasks) {
int taskId = task->taskId;
int earliestTime = earliestStartTime[taskId];
// Find the worker who can complete the task the fastest
int selectedWorker = -1;
int earliestPossibleStart = Int32::MaxValue;
for (int w = 0; w < workers; w++) {
// Calculate when the worker can start work
int workerAvailTime = workerEndTime[w];
int possibleStartTime = Math::Max(workerAvailTime, earliestTime);
// Choose a worker
if (possibleStartTime < earliestPossibleStart) {
earliestPossibleStart = possibleStartTime;
selectedWorker = w;
}
// If the time is the same - give preference to the less loaded worker {}
if (selectedWorker >= 0) {
// Assign a task to a worker
workerTasks[selectedWorker]->Add(task);
actualStartTimes[taskId] = earliestPossibleStart;
actualEndTimes[taskId] = + actualStartTimes[taskId];
workerEndTime[selectedWorker] = actualEndTimes[taskId];
workerLoad[selectedWorker] += task->cost;
}
}
// STEP 8: Distribution of optional tasks according to the same principle
}
}
}
The OutputAllocationResultsWithDijkstra() module outputs the results of task distribution in a convenient form.
The thirteenth module implements data writing to a file. Uses SaveFileDialog. Writes the number of tasks, each task in the format: id|name|cost|mandatory, the number of links between tasks, a list of links: from|to|weight.
The fourteenth module implements data reading from a file. The program reads the number of tasks, each task (checks the format and value), the number of links, a list of links. Clears previous data, updates tasks, taskWeights, nextTaskId. Calls UpdateTaskList() and UpdateWeightsList().

**Conclusion:** The result of this coursework is a software application that models the structure of task distribution between employees using Dijkstra's algorithm. The application has a graphical interface that allows you to interact with the program:
Implementation of Dijkstra's algorithm for distributing tasks between workers. This algorithm allows you to effectively solve the problem of finding the minimum paths of the tower by recursively dividing it into subtasks.
Development of a graphical interface that allows the user to interact with the program, namely to enter data, load their own and save data to a file.
Providing the ability to choose the number of tasks and workers. The application allows the user to configure the parameters by selecting the desired number of tasks and workers, and configure the connections between tasks.
Completing this coursework allows you to better understand the principles of programming based on graph theory and apply them to solve a specific problem. In addition, the development of a graphical interface adds an element of visualization, which improves the user experience and helps to better perceive the sequence of operations that occur.
Future directions: Software product development options may include improving its functionality, for example, the ability to set a user ID, the ability to save and load the state of the program so that the user can continue working from a previous point.
Thus, this coursework solves most of the tasks set by modeling a graphical representation of the distribution of tasks between employees, as well as allowing the user to directly interact with it and configure parameters, namely the number of tasks and employees, task names, and establishing relationships between them.

**Source list:**
Geeks for Geeks. Dijkstra's Algorithm to find Shortest Paths from a Source to all. URL: 
https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/ 
Saylor Academy. Dijkstra's Algorithm. URL: https://learn.saylor.org/mod/page/view.php?id=19015&forceview=1
Tutorialspoint. Dijkstras Shortest Path Algorithm. URL: https://www.tutorialspoint.com/data_structures_algorithms/dijkstras_shortest_path_algorithm.htm
Geeksforgeeks. 	Introduction to .NET Framework. URL:  https://www.geeksforgeeks.org/introduction-to-net-framework/
Simplilearn. The Best Guide To Create The C++ GUI Application Using Libraries URL: https://www.simplilearn.com/tutorials/cpp-tutorial/cpp-gui
