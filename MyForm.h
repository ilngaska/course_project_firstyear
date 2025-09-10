// MyForm.h
#pragma once
namespace TaskAllocationApp {
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    // Константа для максимальної кількості тасок
    const int MAX_TASKS = 1000;

    // Структура для зберігання інформації про задачу
    public ref struct Task {
        String^ name;
        int cost;          // вартість задачі
        bool mandatory;
        int taskId;        // унікальний айді задачі

        Task(String^ name, int cost, bool mandatory, int id) {
            this->name = name;
            this->cost = cost;
            this->mandatory = mandatory;
            this->taskId = id;
        }
    };

    public ref class MyForm : public System::Windows::Forms::Form
    {
    public:
        MyForm(void)
        {
            InitializeComponent();

            // ініціалізація колекції задач
            tasks = gcnew System::Collections::Generic::List<Task^>();
            nextTaskId = 0;

            // ініціалізація матриці ваг
            taskWeights = gcnew cli::array<cli::array<int>^>(MAX_TASKS);
            for (int i = 0; i < MAX_TASKS; i++) {
                taskWeights[i] = gcnew cli::array<int>(MAX_TASKS);
                for (int j = 0; j < MAX_TASKS; j++) {
                    taskWeights[i][j] = 0;
                }
            }

            // додавання пояснювальних текстів до елементів форми
            this->label3 = (gcnew System::Windows::Forms::Label());
            this->label3->Text = L"Task name";
            this->label3->Location = System::Drawing::Point(10, 20);
            this->label3->Size = System::Drawing::Size(150, 15);
            this->groupBoxTask->Controls->Add(this->label3);

            this->label4 = (gcnew System::Windows::Forms::Label());
            this->label4->Text = L"Cost";
            this->label4->Location = System::Drawing::Point(170, 20);
            this->label4->Size = System::Drawing::Size(80, 15);
            this->groupBoxTask->Controls->Add(this->label4);

            // ініціалізація мітки для обмеження кількості тасок
            this->lblTaskCount = (gcnew System::Windows::Forms::Label());
            this->lblTaskCount->Location = System::Drawing::Point(15, 130);
            this->lblTaskCount->Size = System::Drawing::Size(280, 15);
            this->Controls->Add(this->lblTaskCount);

            // статус обмеження на таски
            UpdateTaskCountLabel();
        }
    protected:
        ~MyForm()
        {
            if (components)
            {
                delete components;
            }
        }
    private:
        System::Collections::Generic::List<Task^>^ tasks;
        // матриця ваг (часу) між задачами
        cli::array<cli::array<int>^>^ taskWeights;
        int nextTaskId;    // лічильник для присвоєння айді задачам
        System::Windows::Forms::Label^ label1;
        System::Windows::Forms::Label^ label2;
        System::Windows::Forms::Label^ label3;
        System::Windows::Forms::Label^ label4;
        System::Windows::Forms::Label^ lblTaskCount;
        System::Windows::Forms::TextBox^ txtWorkers;
        System::Windows::Forms::Button^ btnStart;
        System::Windows::Forms::RichTextBox^ outputBox;
        System::Windows::Forms::GroupBox^ groupBoxTask;
        System::Windows::Forms::TextBox^ txtTaskName;
        System::Windows::Forms::TextBox^ txtTaskCost;
        System::Windows::Forms::CheckBox^ chkMandatory;
        System::Windows::Forms::Button^ btnAddTask;
        System::Windows::Forms::ListBox^ listTasks;
        System::Windows::Forms::Button^ btnRemoveTask;
        // елементи для вводу ваг між елементами
        System::Windows::Forms::GroupBox^ groupBoxWeight;
        System::Windows::Forms::ComboBox^ cmbFromTask;
        System::Windows::Forms::ComboBox^ cmbToTask;
        System::Windows::Forms::TextBox^ txtWeight;
        System::Windows::Forms::Button^ btnAddWeight;
        System::Windows::Forms::ListBox^ listWeights;
        System::Windows::Forms::Button^ btnRemoveWeight;
        System::ComponentModel::Container^ components;
        int FindEarliestAvailableWorker(cli::array<int>^ workerEndTime, cli::array<int>^ workerLoad, int taskEarliestStart, int workers);
        // Алгоритм розподілу тасок на основі Дейкстри
        void AllocateTasksUsingDijkstra(int workers);

        // Допоміжний метод для пошуку найкращого працівника для таски
        int FindBestWorkerForTask(cli::array<int>^ workerEndTime, cli::array<int>^ workerLoad, int taskEarliestStart, int workers);

        // Допоміжний метод для виведення результатів розподілу
        void OutputAllocationResultsWithDijkstra(cli::array<System::Collections::Generic::List<Task^>^>^ workerTasks,
            cli::array<int>^ workerLoad,
            cli::array<int>^ workerEndTime,
            int workers,
            cli::array<int>^ earliestStartTime,
            cli::array<int>^ actualStartTimes);
        System::Windows::Forms::Button^ btnSaveToFile;
        System::Windows::Forms::Button^ btnLoadFromFile;
        void btnSaveToFile_Click(System::Object^ sender, System::EventArgs^ e);
        void btnLoadFromFile_Click(System::Object^ sender, System::EventArgs^ e);

#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->txtWorkers = (gcnew System::Windows::Forms::TextBox());
            this->btnStart = (gcnew System::Windows::Forms::Button());
            this->outputBox = (gcnew System::Windows::Forms::RichTextBox());
            this->groupBoxTask = (gcnew System::Windows::Forms::GroupBox());
            this->txtTaskName = (gcnew System::Windows::Forms::TextBox());
            this->txtTaskCost = (gcnew System::Windows::Forms::TextBox());
            this->chkMandatory = (gcnew System::Windows::Forms::CheckBox());
            this->btnAddTask = (gcnew System::Windows::Forms::Button());
            this->listTasks = (gcnew System::Windows::Forms::ListBox());
            this->btnRemoveTask = (gcnew System::Windows::Forms::Button());

            // ініціалізація елементів для вводу ваг
            this->groupBoxWeight = (gcnew System::Windows::Forms::GroupBox());
            this->cmbFromTask = (gcnew System::Windows::Forms::ComboBox());
            this->cmbToTask = (gcnew System::Windows::Forms::ComboBox());
            this->txtWeight = (gcnew System::Windows::Forms::TextBox());
            this->btnAddWeight = (gcnew System::Windows::Forms::Button());
            this->listWeights = (gcnew System::Windows::Forms::ListBox());
            this->btnRemoveWeight = (gcnew System::Windows::Forms::Button());
            this->groupBoxTask->SuspendLayout();
            this->SuspendLayout();

            // label1
            this->label1->Text = L"Tasks and workers";
            this->label1->Location = System::Drawing::Point(12, 15);
            this->label1->Size = System::Drawing::Size(250, 23);

            // label2
            this->label2->Text = L"Workers:";
            this->label2->Location = System::Drawing::Point(415, 260);
            this->label2->Size = System::Drawing::Size(100, 23);

            // txtWorkers
            this->txtWorkers->Location = System::Drawing::Point(520, 260);
            this->txtWorkers->Size = System::Drawing::Size(100, 50);
            this->txtWorkers->Text = L"2";

            // groupBoxTask
            this->groupBoxTask->Text = L"Add task";
            this->groupBoxTask->Location = System::Drawing::Point(15, 50);
            this->groupBoxTask->Size = System::Drawing::Size(370, 110);

            // txtTaskName
            this->txtTaskName->Location = System::Drawing::Point(10, 35);
            this->txtTaskName->Size = System::Drawing::Size(150, 20);
            this->txtTaskName->Text = L"";
            this->groupBoxTask->Controls->Add(this->txtTaskName);

            // txtTaskCost
            this->txtTaskCost->Location = System::Drawing::Point(170, 35);
            this->txtTaskCost->Size = System::Drawing::Size(80, 20);
            this->txtTaskCost->Text = L"";
            this->groupBoxTask->Controls->Add(this->txtTaskCost);

            // chkMandatory
            this->chkMandatory->Text = L"Mandatory";
            this->chkMandatory->Location = System::Drawing::Point(260, 35);
            this->chkMandatory->Size = System::Drawing::Size(100, 20);
            this->groupBoxTask->Controls->Add(this->chkMandatory);

            // btnAddTask
            this->btnAddTask->Text = L"Add task";
            this->btnAddTask->Location = System::Drawing::Point(90, 65);
            this->btnAddTask->Size = System::Drawing::Size(120, 30);
            this->btnAddTask->Click += gcnew System::EventHandler(this, &MyForm::btnAddTask_Click);
            this->groupBoxTask->Controls->Add(this->btnAddTask);

            // listTasks
            this->listTasks->Location = System::Drawing::Point(15, 170);
            this->listTasks->Size = System::Drawing::Size(280, 100);

            // btnRemoveTask
            this->btnRemoveTask->Text = L"Delete";
            this->btnRemoveTask->Location = System::Drawing::Point(305, 170);
            this->btnRemoveTask->Size = System::Drawing::Size(80, 30);
            this->btnRemoveTask->Click += gcnew System::EventHandler(this, &MyForm::btnRemoveTask_Click);

            // groupBoxWeight - група для зв'язків між задачами
            this->groupBoxWeight->Text = L"Connection between two tasks (completion time)";
            this->groupBoxWeight->Location = System::Drawing::Point(410, 50);
            this->groupBoxWeight->Size = System::Drawing::Size(420, 200);

            // Label для "From task"
            System::Windows::Forms::Label^ lblFromTask = (gcnew System::Windows::Forms::Label());
            lblFromTask->Text = L"From task:";
            lblFromTask->Location = System::Drawing::Point(10, 30);
            lblFromTask->Size = System::Drawing::Size(70, 20);
            this->groupBoxWeight->Controls->Add(lblFromTask);

            // cmbFromTask - список початкової задачі
            this->cmbFromTask->Location = System::Drawing::Point(85, 30);
            this->cmbFromTask->Size = System::Drawing::Size(120, 20);
            this->cmbFromTask->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->groupBoxWeight->Controls->Add(this->cmbFromTask);

            // Label для "To task"
            System::Windows::Forms::Label^ lblToTask = (gcnew System::Windows::Forms::Label());
            lblToTask->Text = L"To task:";
            lblToTask->Location = System::Drawing::Point(215, 30);
            lblToTask->Size = System::Drawing::Size(70, 20);
            this->groupBoxWeight->Controls->Add(lblToTask);

            // cmbToTask - випадаючий список кінцевої задачі
            this->cmbToTask->Location = System::Drawing::Point(290, 30);
            this->cmbToTask->Size = System::Drawing::Size(120, 20);
            this->cmbToTask->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->groupBoxWeight->Controls->Add(this->cmbToTask);

            // Label для "Time"
            System::Windows::Forms::Label^ lblWeight = (gcnew System::Windows::Forms::Label());
            lblWeight->Text = L"Time:";
            lblWeight->Location = System::Drawing::Point(10, 60);
            lblWeight->Size = System::Drawing::Size(70, 20);
            this->groupBoxWeight->Controls->Add(lblWeight);

            // txtWeight - ввід часу (ваг)
            this->txtWeight->Location = System::Drawing::Point(85, 60);
            this->txtWeight->Size = System::Drawing::Size(60, 20);
            this->txtWeight->Text = L"";
            this->groupBoxWeight->Controls->Add(this->txtWeight);

            // btnAddWeight - додавання зв'язку
            this->btnAddWeight->Text = L"Add connection";
            this->btnAddWeight->Location = System::Drawing::Point(155, 60);
            this->btnAddWeight->Size = System::Drawing::Size(120, 25);
            this->btnAddWeight->Click += gcnew System::EventHandler(this, &MyForm::btnAddWeight_Click);
            this->groupBoxWeight->Controls->Add(this->btnAddWeight);

            // listWeights - список зв'язків між тасками
            this->listWeights->Location = System::Drawing::Point(10, 95);
            this->listWeights->Size = System::Drawing::Size(320, 100);
            this->groupBoxWeight->Controls->Add(this->listWeights);

            // btnRemoveWeight - видалення зв'язку
            this->btnRemoveWeight->Text = L"Delete";
            this->btnRemoveWeight->Location = System::Drawing::Point(340, 95);
            this->btnRemoveWeight->Size = System::Drawing::Size(70, 25);
            this->btnRemoveWeight->Click += gcnew System::EventHandler(this, &MyForm::btnRemoveWeight_Click);
            this->groupBoxWeight->Controls->Add(this->btnRemoveWeight);

            // btnStart
            this->btnStart->Text = L"Allocate tasks";
            this->btnStart->Location = System::Drawing::Point(680, 255);
            this->btnStart->Size = System::Drawing::Size(150, 30);
            this->btnStart->Click += gcnew System::EventHandler(this, &MyForm::btnStart_Click);

            // outputBox
            this->outputBox->Location = System::Drawing::Point(15, 300);
            this->outputBox->Size = System::Drawing::Size(370, 300);
            this->outputBox->ReadOnly = true;

            // btnSaveToFile
            this->btnSaveToFile = (gcnew System::Windows::Forms::Button());
            this->btnSaveToFile->Text = L"Save to File";
            this->btnSaveToFile->Location = System::Drawing::Point(560, 600);
            this->btnSaveToFile->Size = System::Drawing::Size(100, 30);
            this->btnSaveToFile->Click += gcnew System::EventHandler(this, &MyForm::btnSaveToFile_Click);

            // btnLoadFromFile
            this->btnLoadFromFile = (gcnew System::Windows::Forms::Button());
            this->btnLoadFromFile->Text = L"Load from File";
            this->btnLoadFromFile->Location = System::Drawing::Point(690, 600);
            this->btnLoadFromFile->Size = System::Drawing::Size(100, 30);
            this->btnLoadFromFile->Click += gcnew System::EventHandler(this, &MyForm::btnLoadFromFile_Click);

            // MyForm
            this->ClientSize = System::Drawing::Size(850, 650);
            this->Controls->Add(this->label1);
            this->Controls->Add(this->label2);
            this->Controls->Add(this->txtWorkers);
            this->Controls->Add(this->btnStart);
            this->Controls->Add(this->outputBox);
            this->Controls->Add(this->groupBoxTask);
            this->Controls->Add(this->listTasks);
            this->Controls->Add(this->btnRemoveTask);
            this->Controls->Add(this->groupBoxWeight);
            this->Text = L"Task Allocator";
            this->groupBoxTask->ResumeLayout(false);
            this->groupBoxTask->PerformLayout();
            this->ResumeLayout(false);
            this->PerformLayout();
            this->Controls->Add(this->btnSaveToFile);
            this->Controls->Add(this->btnLoadFromFile);
        }
#pragma endregion
    private:
        void btnStart_Click(System::Object^ sender, System::EventArgs^ e);
        void btnAddTask_Click(System::Object^ sender, System::EventArgs^ e);
        void btnRemoveTask_Click(System::Object^ sender, System::EventArgs^ e);
        void btnAddWeight_Click(System::Object^ sender, System::EventArgs^ e);
        void btnRemoveWeight_Click(System::Object^ sender, System::EventArgs^ e);
        void UpdateTaskList();
        void UpdateTaskCountLabel();
        void UpdateWeightsList();
        void UpdateTaskCombos();
    };
}