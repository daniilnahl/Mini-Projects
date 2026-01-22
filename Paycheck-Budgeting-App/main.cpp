#include "Budget.h"
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <bits/stdc++.h>
#include <OpenXLSX.hpp>
#include <filesystem>

using namespace OpenXLSX;

// Helper function to print vectors
template <typename T>
void print_vector(std::vector<T>& vec) {
    for (auto& item : vec) {
        std::cout << item << ", ";
    }
}

template <typename T>
void print_vector(std::vector<T>& vec, std::ofstream& outfile) {
    for (auto& item : vec) {
        outfile << item << " ";
    }
}

void budget_settings_menu(std::vector<Budget>& budgets, Budget*& current_budget, std::string& username) {
    int choice;
    std::string budget_name;

    while (true) {
        std::cout << "\n=== BUDGET SETTINGS ===\n";
        std::cout << "1. Select current budget\n";
        std::cout << "2. List budgets\n";
        std::cout << "3. Modify current budget\n";
        std::cout << "4. Create budget\n";
        std::cout << "5. Delete budget\n";
        std::cout << "6. Go back to menu\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                if (budgets.empty()) {
                    std::cout << "No budgets available.\n";
                    break;
                }
                std::cout << "Available budgets:\n";
                for (size_t i = 0; i < budgets.size(); i++) {
                    std::cout << i + 1 << ". " << budgets[i].get_name() << "\n";
                }
                std::cout << "Select budget (number): ";
                int sel;
                std::cin >> sel;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (sel > 0 && sel <= (int)budgets.size()) {
                    current_budget = &budgets[sel - 1];
                    std::cout << "Budget '" << current_budget->get_name() << "' selected.\n";
                } else {
                    std::cout << "Invalid selection.\n";
                }
                break;
            }
            case 2: {
                if (budgets.empty()) {
                    std::cout << "No budgets available.\n";
                } else {
                    std::cout << "Budgets:\n";
                    for (size_t i = 0; i < budgets.size(); i++) {
                        std::cout << "- " << budgets[i].get_name() << "\n";
                    }
                }
                break;
            }
            case 3: {
                if (!current_budget) {
                    std::cout << "No budget selected.\n";
                    break;
                }
                std::cout << "Enter new budget name: ";
                std::getline(std::cin, budget_name);
                current_budget->set_name(budget_name);
                std::cout << "Budget renamed to '" << budget_name << "'.\n";
                break;
            }
            case 4: {
                std::cout << "Enter new budget name: ";
                std::getline(std::cin, budget_name);
                Budget temp_budget(budget_name);
                temp_budget.set_fixed_exps();
                temp_budget.set_perc_exps();
                budgets.push_back(temp_budget);
                current_budget = &budgets.back();
                std::cout << "Budget '" << budget_name << "' created.\n";
                break;
            }
            case 5: {
                if (budgets.empty()) {
                    std::cout << "No budgets to delete.\n";
                    break;
                }
                std::cout << "Available budgets:\n";
                for (size_t i = 0; i < budgets.size(); i++) {
                    std::cout << i + 1 << ". " << budgets[i].get_name() << "\n";
                }
                std::cout << "Select budget to delete (number): ";
                int del;
                std::cin >> del;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (del > 0 && del <= (int)budgets.size()) {
                    if (&budgets[del - 1] == current_budget) {
                        current_budget = nullptr;
                    }
                    budgets.erase(budgets.begin() + del - 1); //deletes the object at specific index
                    std::cout << "Budget deleted.\n";
                } else {
                    std::cout << "Invalid selection.\n";
                }
                break;
            }
            case 6:
                return;
            default:
                std::cout << "Invalid choice.\n";
        }
    }
}

void main_menu(std::string username) {
    int num_of_saved_budgets;
    std::ofstream outfile;
    std::ifstream infile;
    std::vector<Budget> budgets;

    //read in saved budgets
    infile.open("misc.txt");

    if (!infile.is_open()) {
        std::cerr << "Error: Could not open misc.txt file.\n";
    }else{
        int temp_count;
        std::string file_name;
        infile >> temp_count;
        infile.close();
        std::cout << "Found " << temp_count << " budgets from previous session. Loading...\n";

        for (int i = 0; i < temp_count; i++){
            file_name = "budget" + std::to_string(i + 1);
            file_name += ".txt";       
            infile.open(file_name);

            if (!infile.is_open()) {
                std::cerr << "Error: Could not open file.\n";
                return;
            }

            //all necessary temp variables for reading in budget objects
            std::regex del(" ");
            std::sregex_token_iterator it;
            std::sregex_token_iterator end;
            std::string temp_str1;
            float temp_flt1;
            std::vector<std::string> string_vec;
            std::vector<float> float_vec;
            Budget temp_budget;

            //get its name
            getline(infile, temp_str1);
            temp_budget.set_name(temp_str1);

            //get fixed expenses names
            std::getline(infile, temp_str1);
            
            it = std::sregex_token_iterator(temp_str1.begin(), temp_str1.end(), del, -1);
            end = std::sregex_token_iterator();

            while (it != end){
                string_vec.push_back(*it);
                ++it;
            }

            temp_budget.set_fixed_exps_names(string_vec);
            string_vec.clear();
            temp_str1.clear();

            //get fixed expenses nums
            std::getline(infile, temp_str1);
            
            it = std::sregex_token_iterator(temp_str1.begin(), temp_str1.end(), del, -1);
            end = std::sregex_token_iterator();

            while (it != end){
                temp_flt1 = std::stof(*it);
                float_vec.push_back(temp_flt1);
                ++it;
            }   

            temp_budget.set_fixed_exps_nums(float_vec);        
            float_vec.clear();

            //get perc expenses names
            std::getline(infile, temp_str1);
            
            it = std::sregex_token_iterator(temp_str1.begin(), temp_str1.end(), del, -1);
            end = std::sregex_token_iterator();

            while (it != end){
                string_vec.push_back(*it);
                ++it;
            }

            temp_budget.set_perc_exps_names(string_vec);
            string_vec.clear();
            temp_str1.clear();

            //get perc expenses nums
            std::getline(infile, temp_str1);
            
            it = std::sregex_token_iterator(temp_str1.begin(), temp_str1.end(), del, -1);
            end = std::sregex_token_iterator();

            while (it != end){
                temp_flt1 = std::stof(*it);
                float_vec.push_back(temp_flt1);
                ++it;
            }   

            temp_budget.set_perc_exps_nums(float_vec);        
            float_vec.clear();
            infile.close();
            budgets.push_back(temp_budget);
        }
    }

    Budget* current_budget = nullptr;
    int choice;

    while (true) {
        std::cout << "\n========== PAYCHECK BUDGETING APP ==========\n";
        std::cout << "User: " << username << "\n";
        std::cout << "Current Budget: " << (current_budget ? current_budget->get_name() : "None selected") << "\n";
        std::cout << "==========================================\n";
        std::cout << "1. Starting utility\n";
        std::cout << "2. Budget settings\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                if (!current_budget) {
                    std::cout << "Please select a budget first.\n";
                } else {
                    std::cout << "Starting utility for '" << current_budget->get_name() << "'...\n";
                    // TODO: Implement utility logic
                    float usr_amount;
                    int row_num = 1;
                    std::string cell_id, cell_val;
                    std::vector<std::string> column_ltrs = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ", "AK", "AL", "AM", "AN"};
                    XLDocument doc;

                    std::cout << "Enter paycheck amount: ";
                    std ::cin >> usr_amount;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if(std::filesystem::exists("./budget-app1.xlsx")){
                        doc.open("./budget-app1.xlsx");
                    }else {
                        doc.create("./budget-app1.xlsx", XLForceOverwrite);
                    }

                    auto wks = doc.workbook().worksheet("Sheet1");
                    cell_id = "A" + std::to_string(row_num);
                    XLCell cell_obj = wks.cell(cell_id);
                    cell_val = cell_obj.getString();
                    

                    while (!cell_val.empty()){
                        row_num++;
                        cell_id = "A" + std::to_string(row_num);
                        cell_obj = wks.cell(cell_id);
                        cell_val = cell_obj.getString();
                    }
                    //if row 1 is free write the names of expenses
                    if (row_num == 1 || row_num == 2){
                        int header_ptr1 = 0, header_ptr2 = 0;
                        std::vector<std::string> headers = current_budget->get_fixed_exps_names();
                        while (header_ptr1 < headers.size()){
                            cell_id = column_ltrs[header_ptr1] + std::to_string(row_num);
                            wks.cell(cell_id).value() = headers[header_ptr1];
                            header_ptr1++;
                        }

                        headers = current_budget->get_perc_exps_names();
                        while (header_ptr2 < headers.size()){
                            cell_id = column_ltrs[header_ptr1] + std::to_string(row_num);
                            wks.cell(cell_id).value() = headers[header_ptr2];
                            header_ptr2++;
                            header_ptr1++;
                        }
                        row_num++;  // Move to next row for data
                    }

                    //first fixed expenses
                    std::vector<float> nums = current_budget->get_fixed_exps_nums();
                    int move_ptr = 0, temp_ptr = 0;
                    while(move_ptr < nums.size()){
                        cell_id = column_ltrs[move_ptr] + std::to_string(row_num);
                        wks.cell(cell_id).value() = nums[move_ptr];
                        usr_amount -= nums[move_ptr];
                        move_ptr++;
                    }

                    //second percentage expenses
                    nums = current_budget->get_perc_exps_nums();
                    while(temp_ptr < nums.size()){
                        cell_id = column_ltrs[move_ptr] + std::to_string(row_num);
                        wks.cell(cell_id).value() = (nums[temp_ptr]*usr_amount);
                        temp_ptr++;
                        move_ptr++;
                    }

                    doc.save();
                    doc.close();
                }
                break;
            case 2:
                budget_settings_menu(budgets, current_budget, username);
                break;
            case 3: {
                std::cout << "Goodbye!\n";

                //save budgets
                outfile.open("misc.txt");

                if (!outfile.is_open()) {
                    std::cerr << "Error: Could not open file.\n";
                    return;
                }

                outfile << budgets.size() << std::endl;
                outfile.close();

                std::string file_name;

                for (int i = 0; i < budgets.size(); i++){
                    file_name = "budget" + std::to_string(i + 1);
                    file_name += ".txt";
                    outfile.open(file_name);

                    if (!outfile.is_open()) {
                        std::cerr << "Error: Could not open file.\n";
                        return;
                    }

                    outfile << budgets[i].get_name() << std::endl;
                    print_vector(budgets[i].get_fixed_exps_names(), outfile);
                    outfile << std::endl;
                    print_vector(budgets[i].get_fixed_exps_nums(), outfile);
                    outfile << std::endl;
                    print_vector(budgets[i].get_perc_exps_names(), outfile);
                    outfile << std::endl;
                    print_vector(budgets[i].get_perc_exps_nums(), outfile);
                    outfile << std::endl;  
                    outfile.close();                  
                }
                return;
            }
            default:
                std::cout << "Invalid choice.\n";
        }
    }
}

int main(){
    std::string username;
    std::cout << "Enter your name: ";
    std::getline(std::cin, username);
    
    main_menu(username);
    
    return 0;
}