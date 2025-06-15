from transformers import pipeline
from langchain_huggingface import HuggingFacePipeline
from langchain.prompts import ChatPromptTemplate, MessagesPlaceholder
from langchain.memory import ConversationBufferMemory
from langchain.chains import ConversationChain
from transformers.utils.logging import set_verbosity_error
import os

# Suppress verbose warnings
set_verbosity_error()

def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear')

def main():
    print("🤖 AI Chat Assistant - TinyLlama Edition")
    print("----------------------------------------")
    print("Type 'exit', 'quit', or 'bye' to end the conversation.\n")
    
    # Initialize the model
    model = pipeline(
        "text-generation",
        model="TinyLlama/TinyLlama-1.1B-Chat-v1.0",
        max_length=256*2,  # Increased for longer responses
        truncation=True,
        temperature=0.7,  # Add some creativity
        top_p=0.9,
    )
    
    llm = HuggingFacePipeline(pipeline=model)
    
    # Set up conversation memory
    memory = ConversationBufferMemory(return_messages=True)
    
    # Create a chat prompt template that includes history
    prompt = ChatPromptTemplate.from_messages([
        ("system", f"You are a helpful assistant explaining concept."
                  f"Use simple words, short sentences, and fun examples anyone would understand."),
        MessagesPlaceholder(variable_name="history"),
        ("human", "{input}")
    ])
    
    # Create the conversation chain
    conversation = ConversationChain(
        llm=llm,
        prompt=prompt,
        memory=memory,
        verbose=False
    )
    
    # Chat loop
    while True:
        user_input = input("\nYou: ")
        
        # Check for exit commands
        if user_input.lower() in ['exit', 'quit', 'bye']:
            print("\nGoodbye!")
            break
        
        # Get AI response
        try:
            response = conversation.invoke({"input": user_input})
            print(f"\nAI: {response['response']}")
        except Exception as e:
            print(f"\nError: {str(e)}")