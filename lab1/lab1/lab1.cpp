
#include <iostream>
#include <math.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <sstream>

const int NUMBER_OF_ITEMS = 891;
const int TRAINING_SAMPLE_ITEMS = 624;
const int TESTING_SAMPLE_ITEMS = 267;
const int NUMBER_OF_INPUTS = 6;

using namespace std;

class Input {
public:
    double value;

};

class NeuronInput {
public:
    double value;
    double weight;

};

class Neuron {
public:
    int number_of_neuron_inputs;
    NeuronInput* inputs;

    static double state_and_activate() {
        
    }
};

class NeuronLayer {
public:
    int number_of_neurons;
    Neuron* neurons;

};

class Perceptron {
public:
    int number_of_inputs;
    Input* inputs;
    int number_of_layers;
    NeuronLayer* layers;

    Perceptron(int nol, int* non) { //creates the whole perceptron: the inputs, the neuron layers, the neurons, the connections
        number_of_inputs = NUMBER_OF_INPUTS; //creating inputs..
        inputs = new Input[number_of_inputs];
        number_of_layers = nol; //creating neuron layers..
        layers = new NeuronLayer[number_of_layers];
        for (int i = 0; i < number_of_layers; i++) { //creating neurons in each layer..
            layers[i].number_of_neurons = non[i];
            layers[i].neurons = new Neuron[layers[i].number_of_neurons];
            for (int j = 0; j < layers[i].number_of_neurons; j++) { //creating connections in each neuron..
                if (i == 0) { //the number of connections in each neuron of the first layer equals to the number of inputs
                    layers[i].neurons[j].number_of_neuron_inputs = number_of_inputs + 1;
                }
                else {
                    layers[i].neurons[j].number_of_neuron_inputs = layers[i - 1].number_of_neurons + 1;
                }
                layers[i].neurons[j].inputs = new NeuronInput[layers[i].neurons[j].number_of_neuron_inputs];
                layers[i].neurons[j].inputs[0].value = 1; //initializing the unit input for each neuron
                for (int k = 0; k < layers[i].neurons[j].number_of_neuron_inputs; k++) { //selecting random starting values for weights from -0.5 to 0.5
                    double r = rand();
                    layers[i].neurons[j].inputs[k].weight = r / RAND_MAX - 0.5;
                }
            }
        }
    }

    ~Perceptron() {
    }

    static Perceptron makeAPerceptron() {
        int layers;
        cout << "Enter the number of neuron layers in the perceptron:";
        cin >> layers;
        int* neurons = new int[layers];
        for (int i = 0; i < layers; i++) {
            cout << "Enter the number of neurons in the layer " << i << ":";
            cin >> neurons[i];
        }
        Perceptron perceptron = Perceptron(layers, neurons);
        return perceptron;
    }

    static void train(double** sample) {

    }

    static void test(double** sample) {

    }
};

ifstream openfile(string name) {
    ifstream file(name);
    if (!file.is_open()) {
        cout << "Error 0" << endl;
        exit(-1);
    }
    return file;
}

void normalize(double*** sample, int amount) {
    for (int i = 0; i < NUMBER_OF_INPUTS + 1; i++) {
        double max = 0;
        for (int j = 0; j < amount; j++) {
            try {
                if (*sample[j][i] > max) {
                    max = *sample[j][i];
                }
            }
            catch(invalid_argument) {
                continue;
            }
        }
        for (int j = 0; j < amount; j++) {
            *sample[j][i] = *sample[j][i] / max;
        }
    }
}

double** process(string* items, int amount) { //this function code depends on the sample view
    double** sample = new double* [amount];
    for (int i = 0; i < amount; i++) {
        sample[i] = new double[NUMBER_OF_INPUTS + 1];
        string line = items[i].substr(items[i].find_first_of(',') + 1, items[i].length());

        if ((line[0] == '0') or (line[0] == '1')) {
            sample[i][0] = double(line[0]) - 48;
        }
        else {
            cout << "Error 2" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        if (line[0] == '1') {
            sample[i][1] = 0;
        }
        else if (line[0] == '2') {
            sample[i][1] = 0.5;
        } 
        else if (line[0] == '3'){
            sample[i][1] = 1;
        }
        else {
            cout << "Error 3" << endl;
            continue;
        }
        line = line.substr(line.find("\",") + 2, line.length());

        if (line.substr(0, line.find_first_of(',')) == "female") {
            sample[i][2] = 0;
        }
        else if (line.substr(0, line.find_first_of(',')) == "male") {
            sample[i][2] = 1;
        }
        else {
            cout << "Error 4" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        try {
            sample[i][3] = stod(line.substr(0, line.find_first_of(',')));
        }
        catch(invalid_argument) {
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        try {
            sample[i][4] = stod(line.substr(0, line.find_first_of(',')));
        }
        catch (invalid_argument) {
            cout << "Error 6" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        try {
            sample[i][5] = stod(line.substr(0, line.find_first_of(',')));
        }
        catch (invalid_argument) {
            cout << "Error 7" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        if (line[line.length() - 1] == 'S') {
            sample[i][6] = 0;
        }
        else if (line[line.length() - 1] == 'C') {
            sample[i][6] = 0.5;
        }
        else if (line[line.length() - 1] == 'Q') {
            sample[i][6] = 1;
        }
        else {
            continue;
        }
    }
    for (int i = 0; i < TRAINING_SAMPLE_ITEMS; i++) {
        for (int j = 0; j < NUMBER_OF_INPUTS + 1; j++) {
            cout << sample[i][j] << " ";
        }
        cout << endl;
    }
    normalize(&sample, amount);
    return sample;
}

void get_the_samples(double*** training_sample, double*** testing_sample) {
    ifstream file = openfile("train.txt");
    string input_names;
    getline(file, input_names);
    string line;
    string* training_items = new string[TRAINING_SAMPLE_ITEMS];
    string* testing_items = new string[TESTING_SAMPLE_ITEMS];
    int count = 0;
    while (getline(file, line)) {
        if (count < TRAINING_SAMPLE_ITEMS) {
            training_items[count] = line;
        }
        else if (count >= TRAINING_SAMPLE_ITEMS) {
            testing_items[count - TRAINING_SAMPLE_ITEMS] = line;
        }
        else {
            cout << "Error 1" << endl;
            exit(-1);
        }
        count++;
    }
    file.close();
    *training_sample = process(training_items, TRAINING_SAMPLE_ITEMS);
    *testing_sample = process(testing_items, TESTING_SAMPLE_ITEMS);
}

int main()
{
    srand(time(NULL));
    //int neurons[1] = { 1 };
    //Perceptron perceptron = Perceptron(1, neurons);
    Perceptron perceptron = Perceptron::makeAPerceptron();

    double** training_sample = new double*[TRAINING_SAMPLE_ITEMS];
    for (int i = 0; i < TRAINING_SAMPLE_ITEMS; i++) {
        training_sample[i] = new double[NUMBER_OF_INPUTS + 1];
    }
    double** testing_sample = new double*[TESTING_SAMPLE_ITEMS];
    for (int i = 0; i < TESTING_SAMPLE_ITEMS; i++) {
        testing_sample[i] = new double[NUMBER_OF_INPUTS + 1];
    }
    get_the_samples(&training_sample, &testing_sample);
    perceptron.train(training_sample);
    perceptron.test(testing_sample);
    
}