from transformers import AutoModelForCausalLM, AutoTokenizer
import torch
from typing import Optional, List, Dict
import os
import time
from dotenv import load_dotenv

load_dotenv()

class ModelHandler:
    def __init__(self):
        # Model configuration
        self.model_name = os.getenv("MODEL_NAME", "TinyLlama/TinyLlama-1.1B-Chat-v1.0")
        
        # Generation parameters
        self.max_length = int(os.getenv("MAX_LENGTH", "512"))
        self.temperature = float(os.getenv("TEMPERATURE", "0.7"))
        self.top_p = float(os.getenv("TOP_P", "0.9"))
        self.repetition_penalty = float(os.getenv("REPETITION_PENALTY", "1.1"))
        
        # Hardware settings
        self.use_gpu = os.getenv("USE_GPU", "true").lower() == "true"
        self.num_threads = int(os.getenv("NUM_THREADS", "4"))
        
        # Debug settings
        self.debug_mode = os.getenv("DEBUG_MODE", "false").lower() == "true"
        
        # Initialize model and tokenizer
        self.model = None
        self.tokenizer = None
        
        # Conversation history
        self.conversation_history = []
        
    def load_model(self):
        """Load the model and tokenizer"""
        start_time = time.time()
        print(f"Loading model: {self.model_name}")
        
        # Set number of threads for CPU inference
        if not self.use_gpu:
            torch.set_num_threads(self.num_threads)
            print(f"Using {self.num_threads} CPU threads")
        
        # Load tokenizer
        self.tokenizer = AutoTokenizer.from_pretrained(self.model_name)
        if self.tokenizer.pad_token is None:
            self.tokenizer.pad_token = self.tokenizer.eos_token
        
        # Load model with appropriate settings
        if self.use_gpu and torch.cuda.is_available():
            print("Using GPU for inference")
            self.model = AutoModelForCausalLM.from_pretrained(
                self.model_name,
                torch_dtype=torch.float16,
                device_map="auto"
            )
        else:
            print("Using CPU for inference")
            self.model = AutoModelForCausalLM.from_pretrained(
                self.model_name,
                torch_dtype=torch.float32,
                device_map="cpu"
            )
        
        load_time = (time.time() - start_time) * 1000
        print(f"Model loaded successfully! (took {load_time:.2f}ms)")
        
    def _build_prompt(self, user_input: str) -> str:
        """Build the prompt with conversation history"""
        # Add the new user input to history
        self.conversation_history.append({"role": "user", "content": user_input})
        
        # Build the full prompt with a system message
        prompt = "A chat between a user and an AI assistant. The assistant is helpful, friendly, and provides accurate responses.\n\n"
        
        for message in self.conversation_history[-4:]:  # Keep only last 4 messages for context
            if message["role"] == "user":
                prompt += f"User: {message['content']}\n"
            else:
                prompt += f"Assistant: {message['content']}\n"
        
        prompt += "Assistant:"
        return prompt
        
    def generate_response(self, prompt: str) -> Dict[str, any]:
        """Generate a response for the given prompt"""
        if self.model is None or self.tokenizer is None:
            raise RuntimeError("Model not loaded. Call load_model() first.")
            
        try:
            # Start timing
            start_time = time.time()
            
            # Build the full prompt with conversation history
            full_prompt = self._build_prompt(prompt)
            
            # Time for prompt building
            prompt_time = (time.time() - start_time) * 1000
            
            # Tokenize and generate
            tokenize_start = time.time()
            inputs = self.tokenizer(full_prompt, return_tensors="pt", padding=True).to(self.model.device)
            tokenize_time = (time.time() - tokenize_start) * 1000
            
            # Generate response
            generate_start = time.time()
            with torch.no_grad():
                outputs = self.model.generate(
                    **inputs,
                    max_length=self.max_length,
                    temperature=self.temperature,
                    top_p=self.top_p,
                    repetition_penalty=self.repetition_penalty,
                    do_sample=True,
                    pad_token_id=self.tokenizer.pad_token_id,
                    eos_token_id=self.tokenizer.eos_token_id
                )
            generate_time = (time.time() - generate_start) * 1000
            
            # Decode the response
            decode_start = time.time()
            response = self.tokenizer.decode(outputs[0], skip_special_tokens=True)
            response = response.split("Assistant:")[-1].strip()
            decode_time = (time.time() - decode_start) * 1000
            
            # Add the response to conversation history
            self.conversation_history.append({"role": "assistant", "content": response})
            
            # Calculate total time
            total_time = (time.time() - start_time) * 1000
            
            # Return response with timing information
            return {
                "response": response,
                "timing": {
                    "prompt_building": prompt_time,
                    "tokenization": tokenize_time,
                    "generation": generate_time,
                    "decoding": decode_time,
                    "total": total_time
                }
            }
            
        except Exception as e:
            print(f"Error generating response: {str(e)}")
            return {
                "response": "I apologize, but I encountered an error. Could you please try again?",
                "timing": None,
                "error": str(e)
            } 