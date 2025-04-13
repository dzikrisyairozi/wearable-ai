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
   
   # For 2-4GB RAM devices (TinyLlama):
   MODEL_NAME=TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF
   MODEL_FILE=tinyllama-1.1b-chat-v1.0.Q3_K_S.gguf
   
   # For 8GB+ RAM devices (Mistral, recommended):
   # MODEL_NAME=TheBloke/Mistral-7B-Instruct-v0.1-GGUF
   # MODEL_FILE=mistral-7b-instruct-v0.1.Q4_K_M.gguf
   
   MAX_TOKENS=512
   TEMPERATURE=0.3
   TOP_P=0.95
   TOP_K=20
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
- RAM requirements depend on model choice:
  - 2GB RAM minimum for TinyLlama
  - 4GB RAM for Phi-2
  - 8GB RAM recommended for Mistral-7B
- Discord webhook URL
- No GPU required

## Supported Models

The chatbot is optimized for GGUF quantized models, which provide fast CPU inference and low memory usage. Choose your model based on available system resources:

### Entry Level (2-4GB RAM)
- **TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF**
  - Q3_K_S.gguf (~1GB RAM) - Fastest, good for 2GB devices
  - Q4_K_M.gguf (~1.5GB RAM) - Better quality, needs 3GB+ RAM
  - Best for: Basic chat, simple tasks, resource-constrained devices
  
### Mid Range (4-6GB RAM)
- **TheBloke/Phi-2-GGUF**
  - phi-2.Q3_K_S.gguf (~2GB RAM) - Balanced performance
  - phi-2.Q4_K_M.gguf (~2.5GB RAM) - Better quality
  - Best for: Improved reasoning, better response quality

### High Performance (8GB+ RAM)
- **TheBloke/Mistral-7B-Instruct-v0.1-GGUF** (Recommended)
  - Q3_K_S.gguf (~4GB RAM) - Good balance of size/quality
  - Q4_K_M.gguf (~5GB RAM) - Excellent quality, recommended
  - Best for: Complex reasoning, multilingual support, high-quality responses

## Performance

Performance varies by model and quantization:

### TinyLlama (2-4GB RAM):
- Memory: ~1GB with Q3_K_S
- Response Time: 5-10 seconds
- Quality: Basic, suitable for simple tasks

### Mistral-7B (8GB+ RAM):
- Memory: ~5GB with Q4_K_M
- Response Time: 10-20 seconds
- Quality: Excellent, with strong multilingual support

## Debug Mode

When DEBUG_MODE=true in .env:
- Shows model name in chat interface
- Displays generation timing information
- Reports token usage statistics
- Includes debug info in Discord messages

## Model Selection Guide

1. For Raspberry Pi 4 (2GB): Use TinyLlama with Q3_K_S
2. For Raspberry Pi 4 (4GB): Use Phi-2 with Q3_K_S
3. For Raspberry Pi 4 (8GB): Use Mistral-7B with Q4_K_M (recommended)
4. For Desktop/Server: Use Mistral-7B with Q5_K_M for best quality
