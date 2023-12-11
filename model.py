import os
os.environ['GRADIENT_ACCESS_TOKEN'] = "accees token"
os.environ['GRADIENT_WORKSPACE_ID'] = "workspace id"

import csv
from gradientai import Gradient

def read_csv(file_path):
    samples = []
    with open(file_path, newline='', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if len(row) == 2:
                input_text = f"### Instruction: {row[0]} \n\n### Response: {row[1]}"
                samples.append({"inputs": input_text})
    return samples

def main():
    # Ruta al archivo CSV con las preguntas y respuestas
    csv_file_path = 'tucsv.csv'

    with Gradient() as gradient:
        base_model = gradient.get_base_model(base_model_slug="nous-hermes2")

        new_model_adapter = base_model.create_model_adapter(
            name="test model 3"
        )
        print(f"Created model adapter with id {new_model_adapter.id}")

        sample_query = "### Instruction: Who is Samuel Corsan? \n\n### Response:"
        print(f"Asking: {sample_query}")

        # before fine-tuning
        completion = new_model_adapter.complete(query=sample_query, max_generated_token_count=100).generated_output
        print(f"Generated (before fine-tune): {completion}")

        # Leer preguntas y respuestas desde el archivo CSV
        samples = read_csv(csv_file_path)

        # this is where fine-tuning happens
        num_epochs = 3
        count = 0
        while count < num_epochs:
            print(f"Fine-tuning the model, iteration {count + 1}")
            new_model_adapter.fine_tune(samples=samples)
            count = count + 1

        # after fine-tuning
        completion = new_model_adapter.complete(query=sample_query, max_generated_token_count=100).generated_output
        print(f"Generated (after fine-tune): {completion}")

        new_model_adapter.delete()

if __name__ == "__main__":
    main()
