#ifndef BUDGET_H
#define BUDGET_H

#include <string>
#include <vector>

class Budget{
private:
    std::vector<std::string> fixed_exp_names;
    std::vector<float> fixed_expenses;
    std::vector<std::string> percent_exp_names;
    std::vector<float> percent_expenses;
    std::string budget_name;

public:
    Budget();
    Budget(std::string name);
    ~Budget();

    std::vector<std::string>& get_fixed_exps_names();
    std::vector<float>& get_fixed_exps_nums();
    std::vector<std::string>& get_perc_exps_names();
    std::vector<float>& get_perc_exps_nums();
    std::string get_name();
    
    void set_fixed_exps();
    void set_perc_exps();
    void set_fixed_exps_names(std::vector<std::string>& temp);
    void set_fixed_exps_nums(std::vector<float>& temp);
    void set_perc_exps_names(std::vector<std::string>& temp);
    void set_perc_exps_nums(std::vector<float>& temp);
    void set_name(std::string name);
};

#endif