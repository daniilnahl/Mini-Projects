#include "Budget.h"
#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>

//need another constructor where it can fully be filled up
Budget::Budget() : fixed_exp_names({}), fixed_expenses({}), percent_exp_names({}), percent_expenses({}), budget_name("unnamed")
{

}

Budget::Budget(std::string name) : fixed_exp_names({}), fixed_expenses({}), percent_exp_names({}), percent_expenses({}), budget_name(name)
{

}

Budget::~Budget(){
}

std::vector<std::string>& Budget::get_fixed_exps_names(){
    return fixed_exp_names;
}

std::vector<float>& Budget::get_fixed_exps_nums(){
    return fixed_expenses;
}

std::vector<std::string>& Budget::get_perc_exps_names(){
    return percent_exp_names;
}

std::vector<float>& Budget::get_perc_exps_nums(){
    return percent_expenses;
}

std::string Budget::get_name(){
    return budget_name;
}

void Budget::set_fixed_exps(){//this function should only be callable if user did not config their expense
    std::string expense_name;
    float expense_amount;
    int user_input;

    while (true){
        std::cout << "Enter the name of the fixed expense(only 1 word): ";
        std::getline(std::cin, expense_name);
        std::cout << "Enter amount for " << expense_name << ": ";
        std:: cin >> expense_amount;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        fixed_exp_names.push_back(expense_name);
        fixed_expenses.push_back(expense_amount);

        std::cout << "Added " << expense_name << ": $" << expense_amount << std::endl;
            
        //continue?
        std::cout << "Add more? (1=yes, 2=no): ";
        std::cin >> user_input;

        while(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Invalid input. Enter 1 or 2: ";
            std::cin >> user_input;
            }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (user_input == 2) break; 
    }
}

void Budget::set_perc_exps(){ //this function should only be callable if user did not config their expense     
    std::string expense_name;
    float expense_amount;
    int user_input, sectors;
    float tot_percent = 0;
    bool valid_alloc = false;

    std::cout << "Enter number of sectors: ";
    std::cin >> sectors;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (!valid_alloc){
        tot_percent = 0;
        bool error_flg = false;
        percent_exp_names.clear();
        percent_expenses.clear();
        int j = 0;

        while(j < sectors){
            std::cout << "Enter sector name(only 1 word): ";
            getline(std::cin, expense_name);

            std::cout << "Enter allocation % for " << expense_name << " (0.0-1.0): ";
            std:: cin >> expense_amount;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            tot_percent += expense_amount;

            if (tot_percent > 1){
                break;
            }

            auto temp = std::find(percent_exp_names.begin(), percent_exp_names.end(), expense_name);

            if (temp != percent_exp_names.end()) {
                std::cout << "Sector with name of '"<< expense_name <<"' already exists at index " << std::distance(percent_exp_names.begin(), temp) << ". Try again.\n";
                error_flg = true;
                break;
            } 

            error_flg == false;
            percent_exp_names.push_back(expense_name);
            percent_expenses.push_back(expense_amount);
            std::cout << "Added " << expense_name << ": " << expense_amount * 100 << "%\n";
            j++;     
        }

        if (tot_percent == 1 && error_flg == false){    
            valid_alloc = true;
        }else{
            std::cout << "Total: " << tot_percent * 100 << "%. Must be 100%. Resetting...\n";
        }    
    }  
}

void Budget::set_fixed_exps_names(std::vector<std::string>& temp){
    fixed_exp_names.clear();
    fixed_exp_names = temp;
}

void Budget::set_fixed_exps_nums(std::vector<float>& temp){
    fixed_expenses.clear();
    fixed_expenses = temp;
}

void Budget::set_perc_exps_names(std::vector<std::string>& temp){
    percent_exp_names.clear();
    percent_exp_names = temp;
}

void Budget::set_perc_exps_nums(std::vector<float>& temp){
    percent_expenses.clear();
    percent_expenses = temp;
}

void Budget::set_name(std::string name){
        budget_name = name;
}
