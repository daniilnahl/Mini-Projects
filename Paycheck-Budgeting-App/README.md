# Paycheck Budgeting App

A C++ command-line application that helps you allocate and track your paycheck across fixed and percentage-based expenses. The app exports your budget breakdowns to Excel spreadsheets for easy visualization and record-keeping.

## Features

- **Budget Management**: Create, select, modify, and delete multiple budgets
- **Flexible Expense Types**:
  - Fixed expenses (e.g., rent, utilities)
  - Percentage-based expenses (e.g., savings, investments)
- **Excel Export**: Automatically exports paycheck allocations to `budget-app1.xlsx`
- **Session Persistence**: Saves all budgets between sessions in text files
- **User-Friendly Menu**: Intuitive command-line interface with budget settings management

## Requirements

- C++17 or later
- CMake 3.10+
- OpenXLSX library (included as submodule)

## Building

### 1. Clone/Navigate to the project directory
```bash
cd Paycheck-Budgeting-App
```

### 2. Create and enter build directory
```bash
mkdir -p build
cd build
```

### 3. Build with CMake
```bash
cmake ..
make
```

The executable `budget_app` will be created in the `build` directory.

## Usage

### Running the Application
```bash
./budget_app
```

### Main Menu Options

**1. Starting Utility**
- Select a budget first
- Enter your paycheck amount
- The app calculates and writes your expense allocations to Excel:
  - Fixed expenses are subtracted from your paycheck
  - Percentage-based expenses are calculated from remaining funds
  - Each entry is written to a new row in the spreadsheet

**2. Budget Settings**
- **Select current budget**: Choose from existing budgets
- **List budgets**: View all saved budgets
- **Modify current budget**: Change the budget name
- **Create budget**: Create a new budget with fixed and percentage expenses
- **Delete budget**: Remove a budget from your list

**3. Exit**
- Saves all budgets to disk and closes the application

### Budget Creation Workflow

When creating a new budget:

1. **Enter budget name** - Give your budget a descriptive name
2. **Define fixed expenses** - Add expenses with fixed amounts (e.g., "Rent: $1500")
3. **Define percentage expenses** - Add expense categories with percentages that sum to 100% (e.g., "Savings: 0.20", "Entertainment: 0.15", etc.)

## Output

The app generates:
- **budget-app1.xlsx**: Excel file containing all paycheck allocations
  - Headers (when first row is empty): expense category names
  - Data rows: amount allocated for each expense per paycheck entry
- **budget*.txt**: Text files storing budget configurations (created automatically)
- **misc.txt**: Index file tracking number of saved budgets

## Example Workflow

```
1. Launch app: ./budget_app
2. Enter name: "John"
3. Go to Budget Settings → Create budget
4. Name budget: "Monthly Budget"
5. Add fixed expenses:
   - Rent: 1500
   - Groceries: 300
6. Add percentage expenses (must total 100%):
   - Savings: 0.25 (25%)
   - Utilities: 0.10 (10%)
   - Entertainment: 0.15 (15%)
   - Other: 0.50 (50%)
7. Select the budget
8. Return to main menu → Starting utility
9. Enter paycheck: 3000
10. Check budget-app1.xlsx for your expense breakdown
```

## File Structure

```
Paycheck-Budgeting-App/
├── main.cpp              # Main application logic
├── Budget.h              # Budget class definition
├── Budget.cpp            # Budget class implementation
├── CMakeLists.txt        # Build configuration
├── budget-app1.xlsx      # Generated Excel output
├── budget*.txt           # Saved budget configurations
├── misc.txt              # Budget index file
└── OpenXLSX-master/      # Excel library (submodule)
```

## Key Classes

### Budget
Manages budget data and operations:
- Fixed expenses (names and amounts)
- Percentage-based expenses (names and percentages)
- Persistence methods for saving/loading

### Main Application
- User input handling
- Budget selection and management
- Excel file creation and data export

## Technical Details

- Uses **OpenXLSX** for Excel file manipulation
- Data persisted using simple text file format
- 1-based cell indexing (standard Excel format)
- Automatically creates new rows in Excel without overwriting existing data

## Notes

- Percentage expenses must sum to exactly 100% (1.0) during budget creation
- Fixed and percentage expense names must be single words
- Each paycheck entry appends to the Excel file as a new row
- The app handles file creation/opening automatically

## Troubleshooting

**Excel file not creating?**
- Ensure you have write permissions in the build directory

**Budget not saving?**
- Check that text files (misc.txt, budget*.txt) can be written to build directory

**Invalid cell address error?**
- This indicates the Excel file is full or corrupted. Delete budget-app1.xlsx and restart

## Future Enhancements

- CSV export support
- Budget visualization and charts
- Month/year tracking
- Category spending limits and warnings
- Data import from existing Excel files
