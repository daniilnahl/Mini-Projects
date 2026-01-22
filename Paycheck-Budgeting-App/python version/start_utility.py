import typer
import os
import main

EXPENSES_FILE = "expenses.pkl"
SURPLUS_FILE = "surplus.pkl"
MISC_FILE = "misc.pkl"

app = typer.Typer()

@app.command()
def start():
    """starts the utility."""
    #checks if the user has already configured their expenses and surplus
    if os.path.exists("expenses.pkl") and os.path.exists("surplus.pkl"):
        main_loop = True
        while main_loop:
            try:
                paycheck = float(typer.prompt("Enter your paycheck amount"))
                
                expenses = main.load_file(EXPENSES_FILE)
                surplus = main.load_file(SURPLUS_FILE)
                
                while True:
                    usr_answr = typer.prompt("Skip to file output? (y/n)").lower()
                        
                    if usr_answr == "y":
                        main.excel_output(surplus, expenses, paycheck)
                        
                        typer.echo("File output complete.")
                        
                        main_loop = False
                        break
                        
                    if usr_answr == "n": #visuals for user
                        typer.echo("Expenses:")
                        total_expenses = 0
                        for key, value in expenses.items():
                            typer.echo(f"{key}: ${value}")
                            total_expenses += value
                            
                        paycheck_aft_exp = paycheck - total_expenses    
                        typer.echo(f"Total Expenses: ${total_expenses}")
                        typer.echo(f"Paycheck after expenses: ${paycheck_aft_exp}")    
                        typer.echo()    
                            
                        typer.echo("Surplus:")
                        for key, value in surplus.items():
                            typer.echo(f"{key}: ${round(value*paycheck_aft_exp, 2)}")
                                
                        typer.echo("\nbzzzzzzzzzzzzzzzzzzzz brrrrrrrrrrrrrrr bzzzzzzzzzzzzzz gears are moving")
                        main.excel_output(surplus, expenses, paycheck)
                        
                        typer.echo("File output complete.")
                        
                        main_loop = False
                        break
                    
                    else:
                        typer.echo("Invalid input. Please enter 'y' or 'n'.")
                            
            except ValueError:
                typer.echo("Invalid input. Please enter numeric values, such as 10.1 or 123.")

    else:
        typer.echo("Please configure your expenses and surplus first.")
        typer.echo("Run the command 'config' to configure your expenses and surplus.")  
        
@app.command()
def config():
    """configures expenses and surplus allocation."""
    main.expenses_config()
    main.surplus_config()
    typer.echo("Expenses and surplus successfully configured.")
      
if __name__ == "__main__":
    app()