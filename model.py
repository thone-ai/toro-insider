# Thone AI ToRo v0.1 31/01/24
import torch
import pandas as pd
from trl import SFTTrainer
from functions import create_prompt, generate_response, tokenize_prompts, plot_data_lengths, plot_data_lengths_2
from datasets import load_dataset, concatenate_datasets
from transformers import AutoModelForCausalLM, AutoTokenizer, BitsAndBytesConfig
from peft import LoraConfig, get_peft_model, prepare_model_for_kbit_training
from transformers import TrainingArguments

instruct_tune_dataset_1 = load_dataset("json", data_files="train.jsonl", split="train")
instruct_tune_dataset_2 = load_dataset("mosaicml/dolly_hhrlhf", split="train")
instruct_tune_dataset_3 = load_dataset("wikimedia/wikipedia", split="train")
instruct_tune_dataset_4 = load_dataset("somosnlp/recetas-cocina", split="train")
combined_dataset = concatenate_datasets([instruct_tune_dataset_1, instruct_tune_dataset_2])

print(create_prompt(combined_dataset[4]))

model_id = "mistralai/Mixtral-8x7B-v0.1"

nf4_config = BitsAndBytesConfig(
   load_in_8bit=True
)

model = AutoModelForCausalLM.from_pretrained(
    model_id,
    device_map='auto',
    quantization_config=nf4_config,
    use_cache=False,
    attn_implementation="flash_attention_2"
)

tokenizer = AutoTokenizer.from_pretrained(model_id)

tokenizer.pad_token = tokenizer.eos_token
tokenizer.padding_side = "right"

prompt="""[INST]Use the provided input to create an instruction that could have been used to generate the response with an LLM. \nThere are more than 12,000 species of grass. The most common is Kentucky Bluegrass, because it grows quickly, easily, and is soft to the touch. Rygrass is shiny and bright green colored. Fescues are dark green and shiny. Bermuda grass is harder but can grow in drier soil.[\INST]"""

generate_response(prompt, model)

tokenized_train_dataset = instruct_tune_dataset_1.map(tokenize_prompts)

plot_data_lengths(tokenized_train_dataset)

plot_data_lengths_2(tokenized_train_dataset)

peft_config = LoraConfig(
    lora_alpha=16,
    lora_dropout=0.1,
    r=64,
    bias="none",
        target_modules=[
        "q_proj",
        "k_proj",
        "v_proj",
        "o_proj",
        "gate_proj",
        "up_proj",
        "down_proj",
        "lm_head",
    ],
    task_type="CAUSAL_LM"
)

model = prepare_model_for_kbit_training(model)
model = get_peft_model(model, peft_config)

if torch.cuda.device_count() > 1: # If more than 1 GPU
    print(torch.cuda.device_count())
    model.is_parallelizable = True
    model.model_parallel = True

args = TrainingArguments(
  output_dir = "Mixtral_Toro_FineTuning",
  #num_train_epochs=5,
  max_steps = 1000, # comment out this line if you want to train in epochs
  per_device_train_batch_size = 32,
  warmup_steps = 0.03,
  logging_steps=10,
  save_strategy="epoch",
  #evaluation_strategy="epoch",
  evaluation_strategy="steps",
  eval_steps=10, # comment out this line if you want to evaluate at the end of each epoch
  learning_rate=2.5e-5,
  bf16=True,
  # lr_scheduler_type='constant',
)

max_seq_length = 1024

trainer = SFTTrainer(
  model=model,
  peft_config=peft_config,
  max_seq_length=max_seq_length,
  tokenizer=tokenizer,
  packing=True,
  formatting_func=create_prompt, # this will aplly the create_prompt mapping to all training and test dataset
  args=args,
  train_dataset=instruct_tune_dataset_1,
)

trainer.train()

trainer.save_model("Mixtral_Toro_FineTuning")
