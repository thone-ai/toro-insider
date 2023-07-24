import csv

def add_conversation_to_dataset(question, answer):
    with open('dataset.csv', 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([question, answer])

# Example usage
new_question = "What is the capital of France?"
new_answer = "The capital of France is Paris."
add_conversation_to_dataset(new_question, new_answer)