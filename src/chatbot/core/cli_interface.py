from rich.console import Console
from rich.prompt import Prompt
from rich.panel import Panel
from rich.markdown import Markdown
from rich.live import Live
from rich.spinner import Spinner
from rich.table import Table
from chatbot.core.model_handler import ModelHandler
from chatbot.utils.discord_handler import DiscordHandler
import os
from dotenv import load_dotenv

load_dotenv()

class CLIInterface:
    def __init__(self):
        self.console = Console()
        self.model_handler = ModelHandler()
        self.discord_handler = DiscordHandler()
        self.debug_mode = os.getenv("DEBUG_MODE", "false").lower() == "true"
        
    def _format_timing_table(self, timing: dict) -> Table:
        """Format timing information into a rich table"""
        table = Table(title="Generation Timing", show_header=True, header_style="bold magenta")
        table.add_column("Stage", style="cyan")
        table.add_column("Time (ms)", justify="right", style="green")
        
        for stage, time in timing.items():
            table.add_row(
                stage.replace("_", " ").title(),
                f"{time:.2f}"
            )
            
        return table
        
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
                # Show generation status if in debug mode
                with Live(Spinner("dots", text="Generating response..."), refresh_per_second=10) as live:
                    result = self.model_handler.generate_response(user_input)
                
                # Display response
                self.console.print("\n[bold blue]AI[/bold blue]")
                self.console.print(Markdown(result["response"]))
                
                # Show timing information in debug mode
                if self.debug_mode and result["timing"]:
                    self.console.print(self._format_timing_table(result["timing"]))
                
                # Send to Discord with debug info if debug mode is enabled
                self.discord_handler.send_message(
                    user_input, 
                    result["response"],
                    debug_info=result["timing"] if self.debug_mode else None
                )
                
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