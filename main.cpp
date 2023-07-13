#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <NeuralNetwork.h>

struct Data {
    std::vector<double> input;
    std::vector<double> output;
};

class AI {
private:
    std::vector<Data> dataset;
    NeuralNetwork network;

public:
    AI(int inputSize, int outputSize, int hiddenLayers, int hiddenLayerSize) {
        
        network.addLayer(inputSize);
        for (int i = 0; i < hiddenLayers; ++i) {
            network.addLayer(hiddenLayerSize);
        }
        network.addLayer(outputSize);
        network.setActivationFunction(NeuralNetwork::ActivationFunction::Sigmoid);
        network.setLearningRate(0.1);
        network.setMomentum(0.9);
    }

    void addData(const std::vector<double>& input, const std::vector<double>& output) {
        dataset.push_back({input, output});
    }

    void train() {
        std::vector<double> inputVector, outputVector;
        for (const auto& data : dataset) {
            inputVector = data.input;
            outputVector = data.output;
            network.train(inputVector, outputVector);
        }
    }

    std::vector<double> predict(const std::vector<double>& input) {
        return network.feedForward(input);
    }

    void saveDataToFile(const std::string& filename) {
        std::ofstream file(filename);

        if (file.is_open()) {
            for (const auto& data : dataset) {
                std::ostringstream inputString, outputString;

                for (const auto& inputValue : data.input) {
                    inputString << inputValue << ",";
                }

                for (const auto& outputValue : data.output) {
                    outputString << outputValue << ",";
                }

                std::string line = inputString.str() + outputString.str();
                line.pop_back(); 

                file << line << std::endl;
            }

            file.close();
            std::cout << "Datos guardados en el archivo " << filename << std::endl;
        } else {
            std::cout << "No se pudo abrir el archivo " << filename << " para guardar los datos." << std::endl;
        }
    }
};

int main() {
    #include <NeuralNetwork.h>

    const int inputSize = 2;
    const int outputSize = 1;
    const int hiddenLayers = 1;
    const int hiddenLayerSize = 3;

    AI ai(inputSize, outputSize, hiddenLayers, hiddenLayerSize);
    
    ai.addData("Quién es Disam?", "Disam es un desarrollador web full Stack y experto en ciberseguridad con +4 años de experiencia");
    ai.addData("Quién es Álvaro842?", "Álvaro842 es un desarrollador full Stack, experto en ciberseguridad y fundador de Thone AI");
    ai.addData("Quién gano el mundial de futbol 2022?", "El mundial de futbol 2022 lo gano la Selección Argentina venciendo a la Selección Francesa");


    ai.saveDataToFile("dataset.csv");

    std::vector<double> input = {0, 1};
    std::vector<double> output = ai.predict(input);

    std::cout << "Predicción: " << output[0] << std::endl;

    return 0;
}

