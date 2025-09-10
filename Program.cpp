// Program.cpp - файл для входу в програму
#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    TaskAllocationApp::MyForm form;
    Application::Run(% form);
    return 0;
}