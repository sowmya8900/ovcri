# OVCRI Submission

This C++ application processes training completion data stored in a JSON file (`trainings.txt`) and performs the following tasks:
1. **Load Data:** Load data for multiple individuals from a JSON file.
2. **Count Completed Trainings:** Count the most recent completion for each training for all individuals.
3. **Trainings by Fiscal Year:** Identify individuals who have completed specific trainings within a given fiscal year.
4. **Expired or Soon-to-Expire Trainings:** Determine which individuals have trainings that are either expired or about to expire within one month from a given reference date.

## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Known Issues](#known-issues)

## Features

### Task 0: Load Data
The `load_data` function loads the JSON training data from a file. Each person's name and their list of completed trainings (with completion and expiration dates) are stored in corresponding structures.

### Task 1: Count Completed Trainings
The `count_completed_trainings` function counts how many times each training has been completed by any individual, only considering the most recent completion for each training.

### Task 2: Trainings by Fiscal Year
The `trainings_by_fiscal_year` function filters individuals who completed specific trainings within a given fiscal year (defined by a start and end date). The function uses the fiscal year dates `10/01/2023` to `09/30/2024` for filtering.

### Task 3: Expired or Soon-to-Expire Trainings
The `find_expired_trainings` function identifies individuals whose trainings have expired or are soon to expire (within one month of a given reference date). It uses a helper function `add_month` to calculate expiry windows.

## Installation

### Prerequisites
- C++11 or later
- `nlohmann/json` library (can be installed via a package manager like `vcpkg`, or downloaded from [here](https://github.com/nlohmann/json))
  
### Steps
1. Clone the repository:
    ```bash
    git clone https://github.com/sowmya8900/ovcri.git
    ```
2. Make sure `nlohmann/json.hpp` is included in your project directory.
3. Compile the project using a C++ compiler:
    ```bash
    g++ training_analyzer.cpp -o training_analyzer -std=c++17
    ```

## Usage

1. Ensure that your `trainings.txt` file is structured in JSON format.
2. Run the compiled executable:
    ```bash
    ./training_analyzer
    ```
3. After execution, the program generates the following output files:
   - `completed_trainings.json`: Contains the count of each completed training.
   - `trainings_by_fiscal_year.json`: Lists individuals who completed specified trainings in the fiscal year.
   - `expired_trainings.json`: Contains people with expired or soon-to-expire trainings.

## Known Issues

1. **Date Parsing:** The program assumes all dates are in `MM/DD/YYYY` format. If dates with inconsistent formats are provided, it may cause parsing errors or incorrect results.
2. **Missing Expiration Dates:** Trainings without an expiration date are not considered for expiration checks (dates with null).
3. **Case-Sensitivity:** Training names are case-sensitive. Ensure consistency in the input data.
