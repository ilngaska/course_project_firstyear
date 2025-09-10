#include "MyForm.h"
#include <fstream>

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace TaskAllocationApp;

void MyForm::UpdateTaskCountLabel() {
    lblTaskCount->Text = String::Format("Tasks added: {0}/{1}", tasks->Count, MAX_TASKS);

    // якщо ліміт досягнуто, деактивувати кнопку додавання задач
    if (tasks->Count >= MAX_TASKS) {
        btnAddTask->Enabled = false;
    }
    else {
        btnAddTask->Enabled = true;
    }
}

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

    // оновлюємо комбобокси з кількістю задач
    UpdateTaskCombos();

    // оновлюємо інфо про кількість задач
    UpdateTaskCountLabel();
}

void MyForm::btnAddTask_Click(System::Object^ sender, System::EventArgs^ e) {
    try {
        // перевірка на перевищення ліміту задач 
        if (tasks->Count >= MAX_TASKS) {
            MessageBox::Show(String::Format("Tasks limit reached ({0})", MAX_TASKS), "Warning");
            return;
        }

        // перевірка вхідних даних
        if (String::IsNullOrWhiteSpace(txtTaskName->Text)) {
            MessageBox::Show("Please enter task name", "Input error");
            return;
        }

        int cost;
        if (!Int32::TryParse(txtTaskCost->Text, cost) || cost <= 0) {
            MessageBox::Show("Cost should be a positive number", "Input error");
            return;
        }

        // Створення нової задачі і додавання її в список
        Task^ newTask = gcnew Task(txtTaskName->Text, cost, chkMandatory->Checked, nextTaskId);
        nextTaskId++; // збільшуємо лічильник ID задач
        tasks->Add(newTask);

        // оновлюємо список задач
        UpdateTaskList();

        // очищуємо поля вводу
        txtTaskName->Clear();
        txtTaskCost->Clear();
        chkMandatory->Checked = false;
    }
    catch (Exception^ ex) {
        MessageBox::Show("Task addind error: " + ex->Message, "Error");
    }
}

void MyForm::btnRemoveTask_Click(System::Object^ sender, System::EventArgs^ e) {
    int selectedIndex = listTasks->SelectedIndex;
    if (selectedIndex >= 0 && selectedIndex < tasks->Count) {
        // отримуємо ID видаленої таски
        int taskId = tasks[selectedIndex]->taskId;

        // видаляємо всі зв'язки з цією таскою
        for (int i = 0; i < MAX_TASKS; i++) {
            taskWeights[taskId][i] = 0;
            taskWeights[i][taskId] = 0;
        }

        // видаляємо таску зі списку
        tasks->RemoveAt(selectedIndex);

        // оновлюємо списки
        UpdateTaskList();
        UpdateWeightsList();
    }
    else {
        MessageBox::Show("Please choose a task to delete", "Error");
    }
}

void MyForm::UpdateTaskCombos() {
    // Зберігаємо вибір
    Object^ fromSelected = cmbFromTask->SelectedItem;
    Object^ toSelected = cmbToTask->SelectedItem;

    // Очищуємо комбобокси
    cmbFromTask->Items->Clear();
    cmbToTask->Items->Clear();

    // Заповнюємо комбобокси тасками
    for (int i = 0; i < tasks->Count; i++) {
        Task^ task = tasks[i];
        String^ displayText = String::Format("[{0}] {1}", task->taskId, task->name);
        cmbFromTask->Items->Add(displayText);
        cmbToTask->Items->Add(displayText);
    }

    // Відновлюємо вибір якщо це можливо
    if (fromSelected != nullptr && cmbFromTask->Items->Contains(fromSelected)) {
        cmbFromTask->SelectedItem = fromSelected;
    }

    if (toSelected != nullptr && cmbToTask->Items->Contains(toSelected)) {
        cmbToTask->SelectedItem = toSelected;
    }
}

void MyForm::UpdateWeightsList() {
    listWeights->Items->Clear();

    // Перевіряємо кожну пару індексів в матриці ваг
    for (int i = 0; i < MAX_TASKS; i++) {
        for (int j = 0; j < MAX_TASKS; j++) {
            if (taskWeights[i][j] > 0) {
                Task^ fromTask = nullptr;
                Task^ toTask = nullptr;

                // Шукаємо таски по ID
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

void MyForm::btnAddWeight_Click(System::Object^ sender, System::EventArgs^ e) {
    try {
        // Перевіряємо що обрані обидві завдання
        if (cmbFromTask->SelectedIndex < 0 || cmbToTask->SelectedIndex < 0) {
            MessageBox::Show("Choose two task to create connection", "Input error");
            return;
        }

        // Перевіряємо чи задачі різні
        if (cmbFromTask->SelectedIndex == cmbToTask->SelectedIndex) {
            MessageBox::Show("Unable to create connection", "Input error");
            return;
        }

        // Отримуємо ID задач з різних строк
        String^ fromText = cmbFromTask->SelectedItem->ToString();
        String^ toText = cmbToTask->SelectedItem->ToString();

        int fromId = -1;
        int toId = -1;

        // Беремо ID зі строки виду "ID Name"
        if (fromText->Length > 1 && fromText[0] == '[') {
            int closePos = fromText->IndexOf(']');
            if (closePos > 1) {
                Int32::TryParse(fromText->Substring(1, closePos - 1), fromId);
            }
        }

        if (toText->Length > 1 && toText[0] == '[') {
            int closePos = toText->IndexOf(']');
            if (closePos > 1) {
                Int32::TryParse(toText->Substring(1, closePos - 1), toId);
            }
        }

        if (fromId < 0 || toId < 0) {
            MessageBox::Show("Unable to define task ID", "Error");
            return;
        }

        //  перевірка що ID в межах матриці
        if (fromId >= MAX_TASKS || toId >= MAX_TASKS) {
            MessageBox::Show(String::Format("Task ID can't be bigger or equal to {0}", MAX_TASKS), "Error");
            return;
        }

        // Перевіряємо вагу зв'язку
        int weight;
        if (!Int32::TryParse(txtWeight->Text, weight) || weight <= 0) {
            MessageBox::Show("Time should be a positive number", "Input error");
            return;
        }

        // Додаємо вагу в матрицю
        taskWeights[fromId][toId] = weight;

        // Оновлюємо список зв'язків
        UpdateWeightsList();

        // Очищуємо поле вводу ваги
        txtWeight->Clear();
    }
    catch (Exception^ ex) {
        MessageBox::Show("Path adding error: " + ex->Message, "Error");
    }
}

void MyForm::btnRemoveWeight_Click(System::Object^ sender, System::EventArgs^ e) {
    if (listWeights->SelectedIndex < 0) {
        MessageBox::Show("Choose a connection to delete", "Error");
        return;
    }

    try {
        String^ selectedText = listWeights->SelectedItem->ToString();

        // Парсимо обрану строку для отримання ID тасків
        int fromId = -1;
        int toId = -1;

        // Строка вигляду: "[fromId] Name -> [toId] Name: weight"
        int firstBracket = selectedText->IndexOf('[');
        int firstClose = selectedText->IndexOf(']', firstBracket);
        int secondBracket = selectedText->IndexOf('[', firstClose);
        int secondClose = selectedText->IndexOf(']', secondBracket);

        if (firstBracket >= 0 && firstClose > firstBracket &&
            secondBracket > firstClose && secondClose > secondBracket) {
            Int32::TryParse(selectedText->Substring(firstBracket + 1, firstClose - firstBracket - 1), fromId);
            Int32::TryParse(selectedText->Substring(secondBracket + 1, secondClose - secondBracket - 1), toId);
        }

        if (fromId >= 0 && toId >= 0) {
            // Видаляємо зв'язок з матриці
            taskWeights[fromId][toId] = 0;

            // Оновлюємо список зв'язків
            UpdateWeightsList();
        }
        else {
            MessageBox::Show("Can't define a task in chosen connection", "Error");
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Connection delete error: " + ex->Message, "Error");
    }
}

// Helper function to find the worker with earliest available time
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
            // Якщо час однаковий, віддаємо перевагу працівнику з меншим навантаженням
            minIndex = i;
        }
    }

    return minIndex;
}

// Пошук вершини з мінімальною відстанню, яка ще не оброблена
int MinDistance(cli::array<int>^ dist, cli::array<bool>^ sptSet, int V) {
    int min = Int32::MaxValue / 2, min_index = -1;

    for (int v = 0; v < V; v++) {
        if (!sptSet[v] && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }

    return min_index;
}

// Звичайний Дейкстра для одного джерела
void DijkstraSingleSource(int src, cli::array<int>^ dist, cli::array<cli::array<int>^>^ weights, int V) {
    cli::array<bool>^ sptSet = gcnew cli::array<bool>(V);

    for (int i = 0; i < V; i++) {
        dist[i] = Int32::MaxValue / 2;
        sptSet[i] = false;
    }

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = MinDistance(dist, sptSet, V);
        if (u == -1) break; // Нема досяжних вершин
        sptSet[u] = true;

        for (int v = 0; v < V; v++) {
            if (!sptSet[v] && weights[u][v] > 0 && dist[u] + weights[u][v] < dist[v]) {
                dist[v] = dist[u] + weights[u][v];
            }
        }
    }
}

void MyForm::AllocateTasksUsingDijkstra(int workers)
{
    outputBox->Clear();
    outputBox->AppendText("Task allocation results (Improved Dijkstra-based algorithm):\n\n");
    // STEP 1: Створити колекцію завдань
    System::Collections::Generic::List<int>^ activeTaskIds =
        gcnew System::Collections::Generic::List<int>();
    for each (Task ^ task in tasks) {
        activeTaskIds->Add(task->taskId);
    }
    // STEP 2: Визначити час найранішого початку за допомогою алгоритму Дейкстри
    cli::array<int>^ earliestStartTime = gcnew cli::array<int>(MAX_TASKS);
    for (int i = 0; i < MAX_TASKS; i++) {
        earliestStartTime[i] = 0; // ініціалізуємо час початку як 0
    }
    // Перевіряємо наявність залежностей в графі
    bool dependencyExists = false;
    for (int i = 0; i < MAX_TASKS; i++) {
        for (int j = 0; j < MAX_TASKS; j++) {
            if (taskWeights[i][j] > 0) {
                dependencyExists = true;
                break;
            }
        }
        if (dependencyExists) break;
    }
    if (dependencyExists) {
        // Матриця суміжності для графа залежностей
        cli::array<cli::array<int>^>^ graph = gcnew cli::array<cli::array<int>^>(MAX_TASKS);
        for (int i = 0; i < MAX_TASKS; i++) {
            graph[i] = gcnew cli::array<int>(MAX_TASKS);
            for (int j = 0; j < MAX_TASKS; j++) {
                // якщо зв'язок є, додаємо в граф
                if (taskWeights[i][j] > 0) {
                    graph[i][j] = taskWeights[i][j];
                }
                else {
                    graph[i][j] = 0;
                }
            }
        }
        // Транспонуємо матрицю для знаходження залежностей "до задачі" замість "від задачі"
        cli::array<cli::array<int>^>^ transposedGraph = gcnew cli::array<cli::array<int>^>(MAX_TASKS);
        for (int i = 0; i < MAX_TASKS; i++) {
            transposedGraph[i] = gcnew cli::array<int>(MAX_TASKS);
            for (int j = 0; j < MAX_TASKS; j++) {
                transposedGraph[i][j] = graph[j][i];
            }
        }
        // Алгоритм Дейкстри застосовуємо для кожної задачі
        for each (int taskId in activeTaskIds) {
            cli::array<int>^ distances = gcnew cli::array<int>(MAX_TASKS);
            cli::array<bool>^ visited = gcnew cli::array<bool>(MAX_TASKS);
            for (int i = 0; i < MAX_TASKS; i++) {
                distances[i] = Int32::MaxValue / 2;
                visited[i] = false;
            }
            distances[taskId] = 0;
            // Алгоритм Дейкстри для пошуку найкоротшого шляху до вершин
            for (int count = 0; count < MAX_TASKS - 1; count++) {
                int minDistance = Int32::MaxValue / 2;
                int minIndex = -1;
                // Знаходимо відстань серед необроблених з мінімальною відстанню
                for (int i = 0; i < MAX_TASKS; i++) {
                    if (!visited[i] && distances[i] < minDistance) {
                        minDistance = distances[i];
                        minIndex = i;
                    }
                }
                // Якщо немає доступних вершин виходимо з циклу
                if (minIndex == -1) break;
                visited[minIndex] = true;
                // Оновлюємо відстань до сусідніх вершин
                for (int v = 0; v < MAX_TASKS; v++) {
                    // Оновлюємо відстань якщо:
                    // 1. Є ребро від minIndex до v
                    // 2. v ще не оброблено
                    // 3. Поточна відстань до minIndex не нескінченість
                    // 4. Шлях через minIndex коротше, ніж поточна відстань до v
                    if (!visited[v] &&
                        transposedGraph[minIndex][v] > 0 &&
                        distances[minIndex] != Int32::MaxValue / 2 &&
                        distances[minIndex] + transposedGraph[minIndex][v] < distances[v]) {
                        distances[v] = distances[minIndex] + transposedGraph[minIndex][v];
                    }
                }
            }
            // Оновлюємо час початку
            for (int i = 0; i < MAX_TASKS; i++) {
                if (distances[i] < Int32::MaxValue / 2 && distances[i] > earliestStartTime[i]) {
                    earliestStartTime[i] = distances[i];
                }
            }
        }
    }
    // Виводимо найраніший час початку виконання завдання
    outputBox->AppendText("Earliest start times calculated using Dijkstra's algorithm:\n\n");
    for each (Task ^ task in tasks) {
        outputBox->AppendText(String::Format("[{0}] {1}: {2}\n",
            task->taskId, task->name, earliestStartTime[task->taskId]));
    }
    outputBox->AppendText("\n");
    // STEP 3: Розділяємо задачі на обов'язкові і необов'язкові
    System::Collections::Generic::List<Task^>^ mandatoryTasks =
        gcnew System::Collections::Generic::List<Task^>();
    System::Collections::Generic::List<Task^>^ optionalTasks =
        gcnew System::Collections::Generic::List<Task^>();
    for each (Task ^ task in tasks) {
        if (task->mandatory) {
            mandatoryTasks->Add(task);
        }
        else {
            optionalTasks->Add(task);
        }
    }
    // STEP 4: Сортуємо задачі за часом виконання
    // Обов'язкові задачі
    for (int i = 0; i < mandatoryTasks->Count - 1; i++) {
        for (int j = i + 1; j < mandatoryTasks->Count; j++) {
            if (earliestStartTime[mandatoryTasks[i]->taskId] > earliestStartTime[mandatoryTasks[j]->taskId]) {
                Task^ temp = mandatoryTasks[i];
                mandatoryTasks[i] = mandatoryTasks[j];
                mandatoryTasks[j] = temp;
            } }}
    // Необов'язкові задачі за тим же принципом
    for (int i = 0; i < optionalTasks->Count - 1; i++) {
        for (int j = i + 1; j < optionalTasks->Count; j++) {
            if (earliestStartTime[optionalTasks[i]->taskId] > earliestStartTime[optionalTasks[j]->taskId]) {
                Task^ temp = optionalTasks[i];
                optionalTasks[i] = optionalTasks[j];
                optionalTasks[j] = temp;
            }
        }
    }
    // STEP 5: Визначаємо працівників
    cli::array<System::Collections::Generic::List<Task^>^>^ workerTasks =
        gcnew cli::array<System::Collections::Generic::List<Task^>^>(workers);
    cli::array<int>^ workerLoad = gcnew cli::array<int>(workers);
    cli::array<int>^ workerEndTime = gcnew cli::array<int>(workers);
    for (int i = 0; i < workers; i++) {
        workerTasks[i] = gcnew System::Collections::Generic::List<Task^>();
        workerLoad[i] = 0;
        workerEndTime[i] = 0;
    }

    // STEP 6: Визначаємо реальний час початку і кінця виконання задачі
    cli::array<int>^ actualStartTimes = gcnew cli::array<int>(MAX_TASKS);
    cli::array<int>^ actualEndTimes = gcnew cli::array<int>(MAX_TASKS);
    for (int i = 0; i < MAX_TASKS; i++) {
        actualStartTimes[i] = -1; // Ще не визначено
        actualEndTimes[i] = -1;   // Ще не визначено
    }

    // STEP 7: Розподіл обов'язкових задач
    for each (Task ^ task in mandatoryTasks) {
        int taskId = task->taskId;
        int earliestTime = earliestStartTime[taskId];
        // Знаходимо працівника який може виконати задачу швидше за всіх
        int selectedWorker = -1;
        int earliestPossibleStart = Int32::MaxValue;
        for (int w = 0; w < workers; w++) {
            // Вираховуємо, коли працівник може взятись за роботу
            int workerAvailTime = workerEndTime[w];
            // Працівник виконує завдання лише після завершення попереднього
            int possibleStartTime = Math::Max(workerAvailTime, earliestTime);
            // Обираємо працівника
            if (possibleStartTime < earliestPossibleStart) {
                earliestPossibleStart = possibleStartTime;
                selectedWorker = w;
            }
            // Якщо час однаковий - надаємо перевагу менш навантаженому працівнику
            else if (possibleStartTime == earliestPossibleStart &&
                workerLoad[w] < workerLoad[selectedWorker]) {
                selectedWorker = w;
            }
        }
        if (selectedWorker >= 0) {
            // Призначаємо задачу працівнику
            workerTasks[selectedWorker]->Add(task);
            // Записуємо час початку і кінця
            actualStartTimes[taskId] = earliestPossibleStart;
            actualEndTimes[taskId] = + actualStartTimes[taskId];
            // Оновлюємо час і навантаження працівника
            workerEndTime[selectedWorker] = actualEndTimes[taskId];
            workerLoad[selectedWorker] += task->cost;
        }
    }

    // STEP 8: Розподіл необов'язкових по тому ж принципу
    for each (Task ^ task in optionalTasks) {
        int taskId = task->taskId;
        int earliestTime = earliestStartTime[taskId];
        int selectedWorker = -1;
        int earliestPossibleStart = Int32::MaxValue;
        for (int w = 0; w < workers; w++) {
            int workerAvailTime = workerEndTime[w];
            int possibleStartTime = Math::Max(workerAvailTime, earliestTime);
            if (possibleStartTime < earliestPossibleStart) {
                earliestPossibleStart = possibleStartTime;
                selectedWorker = w;
            }
            else if (possibleStartTime == earliestPossibleStart &&
                workerLoad[w] < workerLoad[selectedWorker]) {
                selectedWorker = w;
            }
        }

        if (selectedWorker >= 0) {
            workerTasks[selectedWorker]->Add(task);

            actualStartTimes[taskId] = earliestPossibleStart;
            actualEndTimes[taskId] = workerEndTime[selectedWorker] +actualStartTimes[taskId];

            workerEndTime[selectedWorker] = actualEndTimes[taskId];
            workerLoad[selectedWorker] += task->cost;
        }
    }

    // STEP 9: Результати
    int totalLoad = 0;
    int maxEndTime = 0;

    outputBox->AppendText("Final Task Allocation:\n\n");

    // Виводимо задачи в порядке виконання
    for (int i = 0; i < workers; i++) {
        outputBox->AppendText(String::Format("Worker {0}:\n", i + 1));

        if (workerTasks[i]->Count > 0) {
            outputBox->AppendText("  Tasks (in execution order):\n");

            // Сортуємо в порядку виконання у працівника
            for (int j = 0; j < workerTasks[i]->Count - 1; j++) {
                for (int k = j + 1; k < workerTasks[i]->Count; k++) {
                    if (actualStartTimes[workerTasks[i][j]->taskId] > actualStartTimes[workerTasks[i][k]->taskId]) {

                        Task^ temp = workerTasks[i][j];
                        workerTasks[i][j] = workerTasks[i][k];
                        workerTasks[i][k] = temp;
                    }
                }
            }

            // Виводимо задачи в порядке виконання
            for each (Task ^ task in workerTasks[i]) {
                int taskId = task->taskId;
                outputBox->AppendText(String::Format("    [{0}] {1} (Cost: {2}, {3})\n",
                    taskId,
                    task->name,
                    task->cost,
                    task->mandatory ? "Mandatory" : "Optional"));
                outputBox->AppendText(String::Format("      Earliest possible start: {0}\n",
                    earliestStartTime[taskId]));
                outputBox->AppendText(String::Format("      Actual start: {0}, End: {1}\n",
                    actualStartTimes[taskId], actualEndTimes[taskId]));
            }
        }
        else {
            outputBox->AppendText("  Tasks: no tasks\n");
        }

        outputBox->AppendText(String::Format("  Total cost: {0}\n", workerLoad[i]));
        outputBox->AppendText(String::Format("  Completion time: {0}\n\n", workerEndTime[i]));

        totalLoad += workerLoad[i];
        maxEndTime = Math::Max(maxEndTime, workerEndTime[i]);
    }

    // Виводимо статистику
    double avgLoad = static_cast<double>(totalLoad) / workers;
    outputBox->AppendText(String::Format("Average worker load: {0:F2}\n", avgLoad));
    outputBox->AppendText(String::Format("Total project completion time: {0}\n", maxEndTime));

}

int MyForm::FindBestWorkerForTask(cli::array<int>^ workerEndTime, cli::array<int>^ workerLoad, int taskEarliestStart, int workers)
{
    int bestWorkerIndex = 0;
    int bestStartTime = Math::Max(workerEndTime[0], taskEarliestStart);

    for (int i = 1; i < workers; i++) {
        int workerAvailableTime = Math::Max(workerEndTime[i], taskEarliestStart);

        // Обрати працівника який може взяти завдання чимшвидше
        if (workerAvailableTime < bestStartTime) {
            bestStartTime = workerAvailableTime;
            bestWorkerIndex = i;
        }
        // Якщо час однаковий, беремо того хто менш завантажений
        else if (workerAvailableTime == bestStartTime && workerLoad[i] < workerLoad[bestWorkerIndex]) {
            bestWorkerIndex = i;
        }
    }

    return bestWorkerIndex;
}

// Функція для виводу результатів
void MyForm::OutputAllocationResultsWithDijkstra(cli::array<System::Collections::Generic::List<Task^>^>^ workerTasks,
    cli::array<int>^ workerLoad,
    cli::array<int>^ workerEndTime,
    int workers,
    cli::array<int>^ earliestStartTime,
    cli::array<int>^ actualStartTimes)
{
    int totalLoad = 0;
    int maxEndTime = 0;

    // Вивести задачі призначені для кожного робітника
    for (int i = 0; i < workers; i++) {
        outputBox->AppendText(String::Format("Worker {0}:\n", i + 1));

        if (workerTasks[i]->Count > 0) {
            outputBox->AppendText("  Tasks (in execution order):\n");

            // Показати робітників з їх задачами і часом
            for each (Task ^ task in workerTasks[i]) {
                int actualStart = actualStartTimes[task->taskId];
                int taskEnd = actualStart + task->cost;

                outputBox->AppendText(String::Format("    {0} (ID: {1}, Cost: {2})\n",
                    task->name,
                    task->taskId,
                    task->cost));
                outputBox->AppendText(String::Format("      Earliest possible start: {0}\n",
                    earliestStartTime[task->taskId]));
                outputBox->AppendText(String::Format("      Actual start: {0}, End: {1}\n",
                    actualStart, taskEnd));
            }
        }
        else {
            outputBox->AppendText("  Tasks: no tasks\n");
        }

        outputBox->AppendText(String::Format("  Total cost: {0}\n", workerLoad[i]));
        outputBox->AppendText(String::Format("  Completion time: {0}\n\n", workerEndTime[i]));

        totalLoad += workerLoad[i];
        maxEndTime = Math::Max(maxEndTime, workerEndTime[i]);
    }

    // Вивести статистику
    double avgLoad = static_cast<double>(totalLoad) / workers;
    outputBox->AppendText(String::Format("Average worker load: {0:F2}\n", avgLoad));
    outputBox->AppendText(String::Format("Total project completion time: {0}\n", maxEndTime));

    // Визначити навантаження
    double variance = 0;
    for (int i = 0; i < workers; i++) {
        variance += Math::Pow(workerLoad[i] - avgLoad, 2);
    }

    // Вивести інфо про залежності
    bool hasDependencies = false;
    for (int i = 0; i < MAX_TASKS; i++) {
        for (int j = 0; j < MAX_TASKS; j++) {
            if (taskWeights[i][j] > 0) {
                hasDependencies = true;
                break;
            }
        }
        if (hasDependencies) break;
    }

    if (hasDependencies) {
        outputBox->AppendText("- Task dependencies determine when tasks can begin\n");
        outputBox->AppendText("- Task costs determine execution duration\n");
    }
    else {
        outputBox->AppendText("- No dependencies between tasks were specified\n");
        outputBox->AppendText("- Tasks were scheduled based on their cost only\n");
    }
}

void MyForm::btnStart_Click(System::Object^ sender, System::EventArgs^ e)
{
    try {
        // Підтвердити дані
        if (tasks->Count == 0) {
            MessageBox::Show("Please add at least one task", "Input error");
            return;
        }

        int workers;
        if (!Int32::TryParse(txtWorkers->Text, workers) || workers <= 0) {
            MessageBox::Show("Workers quantity should be a positive number", "Input error");
            return;
        }

        // Викликати алгоритм Дейкстри
        AllocateTasksUsingDijkstra(workers);
    }
    catch (Exception^ ex) {
        MessageBox::Show("Task allocation error: " + ex->Message, "Error");
    }
}
void MyForm::btnSaveToFile_Click(System::Object^ sender, System::EventArgs^ e) {
    try {
        // Створити діалог збереження файлу
        SaveFileDialog^ saveFileDialog = gcnew SaveFileDialog();
        saveFileDialog->Filter = "Text files (*.txt)|*.txt|All Files (*.*)|*.*";
        saveFileDialog->Title = "Save Task Data";
        saveFileDialog->DefaultExt = "txt";

        // Показати діалог і виконати якщо користувач натиснув OK
        if (saveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            String^ fileName = saveFileDialog->FileName;

            // Відкрити файл для запису
            System::IO::StreamWriter^ writer = gcnew System::IO::StreamWriter(fileName);

            // Записати кількість завдань
            writer->WriteLine(tasks->Count);

            // Записати дані про задачі
            for each (Task ^ task in tasks) {
                writer->WriteLine(String::Format("{0}|{1}|{2}|{3}",
                    task->taskId,
                    task->name,
                    task->cost,
                    task->mandatory ? "1" : "0"));
            }

            // Записати ваги з'єднань
            int connectionsCount = 0;
            // Спершу їх порахувавши
            for (int i = 0; i < MAX_TASKS; i++) {
                for (int j = 0; j < MAX_TASKS; j++) {
                    if (taskWeights[i][j] > 0) {
                        connectionsCount++;
                    }
                }
            }

            // Записати кількість
            writer->WriteLine(connectionsCount);

            // Записати всі з'єднання
            for (int i = 0; i < MAX_TASKS; i++) {
                for (int j = 0; j < MAX_TASKS; j++) {
                    if (taskWeights[i][j] > 0) {
                        writer->WriteLine(String::Format("{0}|{1}|{2}", i, j, taskWeights[i][j]));
                    }
                }
            }

            // Close file
            writer->Close();

            MessageBox::Show("Task data successfully saved to file.", "Success");
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Error saving file: " + ex->Message, "Error");
    }
}

void MyForm::btnLoadFromFile_Click(System::Object^ sender, System::EventArgs^ e) {
    try {
        // Створити діалог відкриття файлу
        OpenFileDialog^ openFileDialog = gcnew OpenFileDialog();
        openFileDialog->Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*";
        openFileDialog->Title = "Load Task Data";

        // Показати вікно і виконати якщо користувач натиснув OK
        if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            String^ fileName = openFileDialog->FileName;

            // Open file for reading
            System::IO::StreamReader^ reader = gcnew System::IO::StreamReader(fileName);

            // Очистити існуючі дані
            tasks->Clear();
            // Скинути матрицю ваг
            for (int i = 0; i < MAX_TASKS; i++) {
                for (int j = 0; j < MAX_TASKS; j++) {
                    taskWeights[i][j] = 0;
                }
            }

            // Прочитати кількість завдань
            int taskCount;
            if (!Int32::TryParse(reader->ReadLine(), taskCount)) {
                throw gcnew Exception("Invalid file format: could not read task count");
            }

            // Скинути ID лічильник для наступного завдання
            nextTaskId = 0;

            // Прочитати завдання
            for (int i = 0; i < taskCount; i++) {
                String^ line = reader->ReadLine();
                if (String::IsNullOrEmpty(line)) {
                    throw gcnew Exception("Invalid file format: missing task data");
                }

                array<String^>^ parts = line->Split('|');
                if (parts->Length != 4) {
                    throw gcnew Exception("Invalid file format: task data format incorrect");
                }

                int taskId, cost;
                bool mandatory;

                if (!Int32::TryParse(parts[0], taskId) ||
                    !Int32::TryParse(parts[2], cost) ||
                    !Boolean::TryParse(parts[3] == "1" ? "true" : "false", mandatory)) {
                    throw gcnew Exception("Invalid file format: cannot parse task values");
                }

                // Створити завдання
                Task^ task = gcnew Task(parts[1], cost, mandatory, taskId);
                tasks->Add(task);

                // Оновити ID наступного завдання якщо необхідно
                if (taskId >= nextTaskId) {
                    nextTaskId = taskId + 1;
                }
            }
            // Прочитати кількість з'єднань
            int connectionsCount;
            if (!Int32::TryParse(reader->ReadLine(), connectionsCount)) {
                throw gcnew Exception("Invalid file format: could not read connections count");
            }
            // Прочитати з'єднання
            for (int i = 0; i < connectionsCount; i++) {
                String^ line = reader->ReadLine();
                if (String::IsNullOrEmpty(line)) {
                    throw gcnew Exception("Invalid file format: missing connection data");
                }

                array<String^>^ parts = line->Split('|');
                if (parts->Length != 3) {
                    throw gcnew Exception("Invalid file format: connection data format incorrect");
                }

                int fromId, toId, weight;

                if (!Int32::TryParse(parts[0], fromId) ||
                    !Int32::TryParse(parts[1], toId) ||
                    !Int32::TryParse(parts[2], weight)) {
                    throw gcnew Exception("Invalid file format: cannot parse connection values");
                }

                // Додати вагу 
                if (fromId < MAX_TASKS && toId < MAX_TASKS) {
                    taskWeights[fromId][toId] = weight;
                }
            }

            // Закрити файл
            reader->Close();

            // Оновити UI
            UpdateTaskList();
            UpdateWeightsList();

            MessageBox::Show("Task data successfully loaded from file.", "Success");
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Error loading file: " + ex->Message, "Error");
    }
}