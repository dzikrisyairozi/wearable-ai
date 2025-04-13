from llama_cpp import Llama
import os
import time
from typing import Optional, List, Dict
from dotenv import load_dotenv
import requests
import os.path

load_dotenv()

class ModelHandler:
    def __init__(self):
        # Model configuration
        self.model_name = os.getenv("MODEL_NAME", "TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF")
        self.model_file = os.getenv("MODEL_FILE", "tinyllama-1.1b-chat-v1.0.Q3_K_S.gguf")
        
        # Generation parameters
        self.max_tokens = int(os.getenv("MAX_TOKENS", "512"))
        self.temperature = float(os.getenv("TEMPERATURE", "0.7"))
        self.top_p = float(os.getenv("TOP_P", "0.95"))
        self.top_k = int(os.getenv("TOP_K", "40"))
        
        # Hardware settings
        self.num_threads = int(os.getenv("NUM_THREADS", "4"))
        
        # Debug settings
        self.debug_mode = os.getenv("DEBUG_MODE", "false").lower() == "true"
        
        # Initialize model
        self.model = None
        
        # Conversation history
        self.conversation_history = []
        
    def download_model(self):
        """Download the GGUF model if not present"""
        if not os.path.exists(self.model_file):
            print(f"Downloading model {self.model_file}...")

            url = f"https://huggingface.co/TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF/resolve/main/{self.model_file}"
            
            response = requests.get(url, stream=True)
            response.raise_for_status()
            
            total_size = int(response.headers.get('content-length', 0))
            block_size = 1024  # 1 KB
            
            with open(self.model_file, 'wb') as f:
                for data in response.iter_content(block_size):
                    f.write(data)
            
            print("Model downloaded successfully!")
        
    def load_model(self):
        """Load the GGUF model"""
        start_time = time.time()
        print(f"Loading model: {self.model_name}")
        
        # Download model if needed
        self.download_model()
        
        # Load the model
        self.model = Llama(
            model_path=self.model_file,
            n_threads=self.num_threads,    # Use all CPU threads
            n_ctx=2048,                    # Context window
            n_batch=512,                   # Batch size for prompt processing
        )
        
        load_time = (time.time() - start_time) * 1000
        print(f"Model loaded successfully! (took {load_time:.2f}ms)")
        
    def generate_response(self, prompt: str) -> Dict[str, any]:
        """Generate a response using the GGUF model"""
        if self.model is None:
            raise RuntimeError("Model not loaded. Call load_model() first.")
            
        try:
            # Start timing
            start_time = time.time()
            
            # Build messages list with conversation history
            messages = [{"role": "system", "content": "You are a helpful, respectful and honest assistant. Always answer as helpfully as possible while being safe."}]
            
            # Add conversation history (last 4 messages)
            for message in self.conversation_history[-4:]:
                messages.append({"role": message["role"], "content": message["content"]})
            
            # Add current user message
            messages.append({"role": "user", "content": prompt})
            
            # Time for prompt building
            prompt_time = (time.time() - start_time) * 1000
            
            # Generate response
            generate_start = time.time()
            output = self.model.create_chat_completion(
                messages=messages,
                max_tokens=self.max_tokens,
                temperature=self.temperature,
                top_p=self.top_p,
                top_k=self.top_k,
                stream=False
            )
            generate_time = (time.time() - generate_start) * 1000
            
            # Extract response
            response = output["choices"][0]["message"]["content"]
            
            # Add to conversation history
            self.conversation_history.append({"role": "user", "content": prompt})
            self.conversation_history.append({"role": "assistant", "content": response})
            
            # Calculate total time
            total_time = (time.time() - start_time) * 1000
            
            # Return response with timing information
            return {
                "response": response,
                "timing": {
                    "prompt_building": prompt_time,
                    "generation": generate_time,
                    "total": total_time,
                    "tokens_used": output["usage"]["total_tokens"]
                }
            }
            
        except Exception as e:
            print(f"Error generating response: {str(e)}")
            return {
                "response": "I apologize, but I encountered an error. Could you please try again?",
                "timing": None,
                "error": str(e)
            } 