# AI CLI Chatbot

A modular CLI chatbot with Discord webhook integration that can work with various language models.

## Features

- Interactive CLI interface with rich text formatting
- Support for various language models (TinyLlama, Llama, Mistral, etc.)
- Discord webhook integration for message logging
- Modular architecture for easy extension

## Setup

1. Clone the repository
2. Install the package:
   ```bash
   pip install -e .
   ```
3. Copy `.env.example` to `.env` and configure:
   ```
   DISCORD_WEBHOOK_URL=your_discord_webhook_url_here
   MODEL_NAME=TinyLlama/TinyLlama-1.1B-Chat-v1.0
   MAX_LENGTH=512
   TEMPERATURE=0.7
   TOP_P=0.9
   REPETITION_PENALTY=1.1
   USE_GPU=true
   NUM_THREADS=4
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
- CUDA-capable GPU (recommended)
- Discord webhook URL

## Supported Models

The chatbot is designed to work with any Hugging Face model that supports text generation. By default, it uses TinyLlama, but you can easily switch to other models by changing the `MODEL_NAME` in your `.env` file.

Some recommended models:
- TinyLlama/TinyLlama-1.1B-Chat-v1.0 (lightweight, good for Raspberry Pi)
- mistralai/Mistral-7B-Instruct-v0.2 (more powerful, requires more resources)
- meta-llama/Llama-2-7b-chat-hf (requires approval from Meta)
- TheBloke/Llama-2-7B-Chat-GGUF (quantized version, more efficient)
