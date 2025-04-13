from rich.console import Console
from rich.prompt import Prompt
from rich.panel import Panel
from rich.markdown import Markdown
from chatbot.core.model_handler import ModelHandler
from chatbot.utils.discord_handler import DiscordHandler

class CLIInterface:
    def __init__(self):
        self.console = Console()
        self.model_handler = ModelHandler()
        self.discord_handler = DiscordHandler()
        
    def start(self):
        """Start the CLI chat interface"""
        self.console.print(Panel.fit(
            "Welcome to AI Chat!\nType 'exit' to quit.",
            title="AI Chat",
            border_style="blue"
        ))
        
        # Load the model
        self.model_handler.load_model()
        
        while True:
            # Get user input
            user_input = Prompt.ask("\n[bold green]You[/bold green]")
            
            if user_input.lower() in ['exit', 'quit']:
                self.console.print("\n[bold red]Goodbye![/bold red]")
                break
                
            # Generate response
            try:
                ai_response = self.model_handler.generate_response(user_input)
                
                # Display response in CLI
                self.console.print("\n[bold blue]AI[/bold blue]")
                self.console.print(Markdown(ai_response))
                
                # Send to Discord
                self.discord_handler.send_message(user_input, ai_response)
                
            except Exception as e:
                self.console.print(f"[bold red]Error:[/bold red] {str(e)}")
                
    def run(self):
        """Run the chat interface"""
        try:
            self.start()
        except KeyboardInterrupt:
            self.console.print("\n[bold red]Goodbye![/bold red]")
        except Exception as e:
            self.console.print(f"[bold red]Fatal error:[/bold red] {str(e)}")

def main():
    """Entry point for the CLI application"""
    cli = CLIInterface()
    cli.run() 