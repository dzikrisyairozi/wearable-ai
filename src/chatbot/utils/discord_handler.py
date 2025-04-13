from discord_webhook import DiscordWebhook
import os
from dotenv import load_dotenv
from typing import Optional

load_dotenv()

class DiscordHandler:
    def __init__(self):
        self.webhook_url = os.getenv("DISCORD_WEBHOOK_URL")
        if not self.webhook_url:
            print("Warning: Discord webhook URL not found in environment variables")
            
    def send_message(self, user_message: str, ai_response: str) -> bool:
        """Send a message to Discord webhook"""
        if not self.webhook_url:
            return False
            
        try:
            webhook = DiscordWebhook(url=self.webhook_url)
            
            # Format the message
            message = f"**User:** {user_message}\n**AI:** {ai_response}"
            
            webhook.content = message
            webhook.execute()
            return True
        except Exception as e:
            print(f"Error sending message to Discord: {e}")
            return False 