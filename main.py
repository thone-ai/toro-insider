import tensorflow as tf
import numpy as np
from tensorflow.keras.preprocessing.text import Tokenizer
from tensorflow.keras.preprocessing.sequence import pad_sequences
import pandas as pd

data = pd.read_csv('dataset.csv')
training_sentences = data['sentence']
training_labels = data['label']

print(training_sentences)
print(training_labels)

tokenizer = Tokenizer()
tokenizer.fit_on_texts(training_sentences)
total_words = len(tokenizer.word_index) + 1
input_sequences = []
for sentence in training_sentences:
    token_list = tokenizer.texts_to_sequences([sentence])[0]
    for i in range(1, len(token_list)):
        n_gram_sequence = token_list[:i+1]
        input_sequences.append(n_gram_sequence)
max_sequence_len = max([len(x) for x in input_sequences])
input_sequences = np.array(pad_sequences(input_sequences, maxlen=max_sequence_len, padding='pre'))

xs = input_sequences[:, :-1]
labels = input_sequences[:, -1]
ys = tf.keras.utils.to_categorical(labels, num_classes=total_words)

model = tf.keras.Sequential([
    tf.keras.layers.Embedding(total_words, 100, input_length=max_sequence_len-1),
    tf.keras.layers.Bidirectional(tf.keras.layers.GRU(150, return_sequences=True)),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.GRU(100),
    tf.keras.layers.Dense(total_words, activation='softmax')
])
model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
history = model.fit(xs, ys, epochs=100, verbose=1)

def generate_response(input_text):
    input_sequence = tokenizer.texts_to_sequences([input_text])[0]
    input_sequence = pad_sequences([input_sequence], maxlen=max_sequence_len-1, padding='pre')
    predictions = model.predict(input_sequence, verbose=0)
    predicted_class = tf.argmax(predictions, axis=-1)
    predicted_index = int(predicted_class) + 1
    output_word = tokenizer.index_word.get(predicted_index)
    return input_text if output_word is None else output_word

print(" Hola! Soy Thone AI, pregunta algo e intentare responder, escribe 'exit' para salir")
while True:
    user_input = input("Pregunta: ")
    if user_input.lower() == "exit":
        break
    else:
        response = generate_response(user_input)
        print("Respuesta:", response)