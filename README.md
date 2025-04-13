# AI CLI Chatbot

A modular CLI chatbot with Discord webhook integration optimized for edge devices using GGUF quantized models.

## Features

- Interactive CLI interface with rich text formatting
- Optimized for edge devices (Raspberry Pi) using GGUF models
- Fast CPU inference without GPU requirements
- Discord webhook integration for message logging
- Modular architecture for easy extension

## Setup

1. Clone the repository
2. Install the package:
   ```bash
   pip install -r requirements.txt
   ```
3. Copy `.env.example` to `.env` and configure:
   ```
   DISCORD_WEBHOOK_URL=your_discord_webhook_url_here
   MODEL_NAME=TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF
   MODEL_FILE=tinyllama-1.1b-chat-v1.0.Q3_K_S.gguf
   MAX_TOKENS=512
   TEMPERATURE=0.7
   TOP_P=0.95
   TOP_K=40
   NUM_THREADS=4
   DEBUG_MODE=false
   ```

## Usage

Run the chatbot:
```bash
ai-chat
```

Or directly with Python:
```bash
python src/main.py
```

- Type your message and press Enter to chat
- Type 'exit' or 'quit' to end the session
- All conversations will be logged to your Discord channel
- Debug mode shows model name, timing information, and token usage

## Project Structure

```
src/
└── chatbot/
    ├── __init__.py
    ├── core/
    │   ├── __init__.py
    │   ├── cli_interface.py
    │   └── model_handler.py
    └── utils/
        ├── __init__.py
        └── discord_handler.py
```

## Requirements

- Python 3.8+
- 2GB RAM minimum (4GB recommended)
- Discord webhook URL
- No GPU required

## Supported Models

The chatbot is optimized for GGUF quantized models, which provide fast CPU inference and low memory usage. The default configuration uses TinyLlama GGUF, which offers an excellent balance of performance and resource usage for edge devices.

Recommended GGUF models:
- TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF (default, ~1GB RAM)
  - Q3_K_S.gguf - Fastest, good for 2GB RAM devices
  - Q4_K_M.gguf - Better quality, needs 3GB+ RAM
- TheBloke/Phi-2-GGUF (good alternative)
  - phi-2.Q3_K_S.gguf - Balanced performance
- TheBloke/neural-chat-7B-v3-1-GGUF (more powerful, needs 4GB+ RAM)

## Performance

Typical performance metrics on Raspberry Pi 4:
- Memory Usage: ~1GB with Q3_K_S quantization
- Response Time: 5-10 seconds
- No GPU required for inference

## Debug Mode

When DEBUG_MODE=true in .env:
- Shows model name in chat interface
- Displays generation timing information
- Reports token usage statistics
- Includes debug info in Discord messages
