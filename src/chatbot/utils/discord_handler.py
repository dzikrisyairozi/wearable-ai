from discord_webhook import DiscordWebhook
import os
from dotenv import load_dotenv
from typing import Optional, Dict
import json

load_dotenv()

class DiscordHandler:
    def __init__(self):
        self.webhook_url = os.getenv("DISCORD_WEBHOOK_URL")
        if not self.webhook_url:
            print("Warning: Discord webhook URL not found in environment variables")
            
    def send_message(self, user_message: str, ai_response: str, debug_info: Optional[Dict] = None, model_name: Optional[str] = None) -> bool:
        """Send a message to Discord webhook"""
        if not self.webhook_url:
            return False
            
        try:
            webhook = DiscordWebhook(url=self.webhook_url)
            
            # Format the message with model name if in debug mode
            display_name = model_name.split('/')[-1] if debug_info and model_name else "AI"
            message = f"**User:** {user_message}\n**{display_name}:** {ai_response}"
            
            # Add debug information if available
            if debug_info:
                debug_table = "**Debug Information:**\n```\n"
                for stage, time in debug_info.items():
                    debug_table += f"{stage.replace('_', ' ').title()}: {time:.2f}ms\n"
                debug_table += "```"
                message += f"\n\n{debug_table}"
            
            webhook.content = message
            webhook.execute()
            return True
        except Exception as e:
            print(f"Error sending message to Discord: {e}")
            return False 