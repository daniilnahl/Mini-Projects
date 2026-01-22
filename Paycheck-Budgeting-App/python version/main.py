# from openpyxl import Workbook
# import openpyxl
# import pickle
# import os

# EXPENSES_FILE = "expenses.pkl"
# SURPLUS_FILE = "surplus.pkl"
# MISC_FILE = "misc.pkl"
   
# def save_file(dict, filename):
#     with open(filename, "wb") as f: #wb - binary write mode
#         pickle.dump(dict, f)
  
# def load_file(filename):
#     if os.path.exists(filename):
#         with open(filename, "rb") as f: #rb - binary read mode
#             return pickle.load(f)
#     return {}

# def modify_exp_or_surp(type):
#     if type == "expenses":
#         file = EXPENSES_FILE
#         key_word = "expense"
        
#     elif type == "surplus":
#         key_word = "surplus"
#         file = SURPLUS_FILE
    
#     dict = load_file(file)
    
#     print(f"Current {type}: ")
#     for key, value in dict.items():
#         print(f"{key}: ${value}")
    
#     while True:
#         exp_name = input(f"Enter the name of the {key_word} you want to modify: ")
#         if exp_name in dict:
#             exp_amount = float(input(f"Enter the new amount of the {key_word}(without $): "))
#             dict.update({exp_name: exp_amount})
#             print(f"{exp_name} {key_word} updated to ${exp_amount}")
#         else:
#             print(f"{exp_name} {type} not found.")
        
#         cont = input(f"Do you want to modify more {type}? (y/n): ").lower()
#         if cont == 'n' or cont == 'no':
#             break
#         else:
#             print()
    
#     save_file(dict, file)
    
# def delete_exp_or_surp(type):
#     if type == "expenses":
#         file = EXPENSES_FILE
#         key_word = "expense"
        
#     elif type == "surplus":
#         key_word = "surplus"
#         file = SURPLUS_FILE
    
#     dict = save_file(file)
    
#     print(f"Current {type}: ")
#     for key, value in dict.items():
#         print(f"{key}: ${value}")
        
#     while True:   
#         exp_name = input(f"Enter the name of the {key_word} you want to delete: ")
#         if exp_name in dict:
#             del dict[exp_name]
#             print(f"{exp_name} {key_word} deleted.")
#         else:
#             print(f"{exp_name} {type} not found.")
        
#         cont = input(f"Do you want to delete more {type}? (y/n): ").lower()
#         if cont == 'n' or cont == 'no':
#             break
#         else:
#             print()
            
#     save_file(dict, file)
       
# def add_exp_or_surp(type):  
#     if type == "expenses":
#         file = EXPENSES_FILE
#         key_word = "expense"
        
#     elif type == "surplus":
#         key_word = "surplus"
#         file = SURPLUS_FILE
    
#     dict = load_file(file)
    
#     while True:
#         exp_name = input(f"Enter the name of the {key_word}: ")
#         exp_amount = float(input(f"Enter the amount of the {key_word}(without $): "))
        
#         dict.update({exp_name: exp_amount})
#         print(f"{exp_name} {key_word} added with ${exp_amount}")
        
#         cont = input(f"Do you want to add more {type}? (y/n): ").lower()
#         if cont == 'n' or cont == 'no':
#             break
#         else:
#             print()
    
#     save_file(dict, file) 
 
# def expenses_config():    
#     #loads in the expenses  dictionaries
#     expenses = load_file(EXPENSES_FILE)
    
    
#     if expenses == {} or expenses == None:
#         print("No expenses found. Configuration for expenses intiated.\n")
#         #config for expenses
#         expenses = {}
#         while True:
#             exp_name = input("Enter the name of the expense: ")
#             exp_amount = float(input("Enter the amount of the expense(without $): "))
            
#             #stores to expenses dictionary
#             expenses.update({exp_name: exp_amount})
            
#             cont = input("Do you want to add more expenses? (y/n): ").lower()
#             if cont == 'n' or cont == 'no':
#                 break
#             else:
#                 print()
        
#     else:
#         print("Expenses found. Loading expenses.")
    
#     #saves the expenses dictionary
#     save_file(expenses, EXPENSES_FILE)
    
# def surplus_config():
#     #config for surplus 
#     surplus = load_file(SURPLUS_FILE)
    
#     if surplus == {} or surplus == None:
#         print("\nNo surplus found. Configuration for surplus intiated.\nNext you will be prompted to configure your paycheck surplus allocation in PERCENT.")
#         sectors = int(input("Enter the amount of sectors you want to split your surplus in(Eg. Retirement, Crypto and Vacation would be 3): ")) 
        
#         valid_allocation = False
#         while (valid_allocation == False):
#             total_percent = 0
#             surplus.clear() #clears the dict from failed loops 
            
#             for i in range(sectors):
#                 #creates a sector object
#                 name = input(f"Enter name for sector {i + 1}: ")
#                 percent = float(input(f"Enter allocation percent for {name} in decimal form (Eg. 0.5): "))
                
#                 total_percent += percent
#                 #check for over allocation
#                 if total_percent > 1: 
#                     print(f"Total percent is over limit, {round(total_percent * 100.0, 2)}%. It should be 100%. Please try again.")
#                     break
                
#                 if name in surplus:
#                     print(f"Name {name} already exists. Please try again.")
#                     break
#                 else:
#                     #add sector to surplus dictionary
#                     surplus.update({name: percent})
                
#             else: #if the loop completes without a break    
#                 if total_percent < 1: #check for under allocation
#                     print(f"Total percent is under limit, {round(total_percent * 100.0, 2)}%. It should be 100%. Please try again.")
#                 else:
#                     valid_allocation = True
    
#     #saves surplus dictionary
#     save_file(surplus, SURPLUS_FILE)
           
# def excel_output(surplus_dict, expenses_dict, paycheck):
#     #loads save data
#     file = "paycheck_budget.xlsx"
#     data = load_file(MISC_FILE)
#     header_filled, row = data[0], data[1]
    
#     if header_filled == False:
#         #opens the file with openpyxl
#         workbook = openpyxl.Workbook()
#         worksheet = workbook.active
        
#         column = 1
#         row = 1
#         total_expenses = 0
        
#         #writes down expenses
#         for expense, value in expenses_dict.items():
#             worksheet.cell(row, column, expense)
#             worksheet.cell(row+1, column, f"${value}")
#             column+=1
#             total_expenses += value
        
#         column += 2 #spacing in between 
#         leftover = paycheck - total_expenses
#         worksheet.cell(row, column, "Leftover After Expenses")
#         worksheet.cell(row+1, column, leftover) 
#         column += 4#spacing in between 
        
#         #writes down surplus
#         for surplus, allocation in surplus_dict.items():
#             worksheet.cell(row, column, surplus)
#             worksheet.cell(row+1, column, f"${round(leftover * allocation, 2)}")
#             column += 1

#         row += 2
#         header_filled = True
#         workbook.save(file)
        
#     else:
#         workbook = openpyxl.load_workbook(file)
#         worksheet = workbook.active
        
#         column = 1
#         total_expenses = 0
        
#         #writes down expenses
#         for expense, value in expenses_dict.items():
#             worksheet.cell(row, column, f"${value}")
#             total_expenses += value
#             column += 1
        
#         column += 2 #spacing in between 
#         leftover = paycheck - total_expenses
#         worksheet.cell(row, column, leftover) 
#         column += 4#spacing in between 
        
#         #writes down surplus
#         for surplus, allocation in surplus_dict.items():
#             worksheet.cell(row, column, f"${round(leftover * allocation, 2)}")
#             column += 1

#         row += 1
#         workbook.save(file)
           
#     save_file([header_filled, row], MISC_FILE)
              