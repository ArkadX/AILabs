#include <iostream>
#include <math.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <sstream>

const int NUMBER_OF_ITEMS = 891;
const int TRAINING_SAMPLE_ITEMS = 846;
const int TESTING_SAMPLE_ITEMS = NUMBER_OF_ITEMS - TRAINING_SAMPLE_ITEMS;
const int NUMBER_OF_INPUTS = 6;
const double SATURATION_PARAMETER = 1;
const double SPEED_OF_TRAIN = 1;

using namespace std;

class NeuronInput {
public:
    void* p_value;
    double weight;
};

class Neuron {
public:
    int number_of_neuron_inputs;
    NeuronInput* inputs;
    double activated_state;
    double residual;

    void state_and_activate() {
        double state = 0;
        for (int i = 0; i < this->number_of_neuron_inputs; i++) {
            state += *(double*)this->inputs[i].p_value * this->inputs[i].weight;
        }
        this->activated_state = 1 / (1 + exp(-SATURATION_PARAMETER * state));
    }
};

class NeuronLayer {
public:
    double fictitious_unit = 1;
    int number_of_neurons;
    Neuron* neurons;
};

class Perceptron {
public:
    int number_of_inlets;
    double* inlets;
    int number_of_layers;
    NeuronLayer* layers;

    Perceptron(int nol, int* non) { //creates the whole perceptron: the inputs, the neuron layers, the neurons, the connections
        number_of_inlets = NUMBER_OF_INPUTS + 1; //creating inputs..
        inlets = new double[number_of_inlets];
        inlets[0] = 1;
        number_of_layers = nol; //creating neuron layers..
        layers = new NeuronLayer[number_of_layers];
        for (int i = 0; i < number_of_layers; i++) { //creating neurons in each layer..
            if (i == number_of_layers - 1) {
                layers[i].number_of_neurons = 1;
            }
            else {
                layers[i].number_of_neurons = non[i];
            }
            layers[i].neurons = new Neuron[layers[i].number_of_neurons];
            for (int j = 0; j < layers[i].number_of_neurons; j++) { //creating connections in each neuron..
                if (i == 0) { //the number of connections in each neuron of the first layer equals to the number of inputs
                    layers[i].neurons[j].number_of_neuron_inputs = number_of_inlets;
                    layers[i].neurons[j].inputs = new NeuronInput[layers[i].neurons[j].number_of_neuron_inputs];
                    for (int k = 0; k < layers[i].neurons[j].number_of_neuron_inputs; k++) {
                        layers[i].neurons[j].inputs[k].p_value = &inlets[k]; //connecting to the inputs, including the unit
                    }
                }
                else {
                    layers[i].neurons[j].number_of_neuron_inputs = layers[i - 1].number_of_neurons + 1;
                    layers[i].neurons[j].inputs = new NeuronInput[layers[i].neurons[j].number_of_neuron_inputs];
                    layers[i].neurons[j].inputs[0].p_value = &layers[i - 1].fictitious_unit; //connecting to the unit from the previous layer
                    for (int k = 1; k < layers[i].neurons[j].number_of_neuron_inputs; k++) {
                        layers[i].neurons[j].inputs[k].p_value = &layers[i - 1].neurons[k - 1].activated_state; //connecting to each neuron from the previous layer
                    }
                }
                for (int k = 0; k < layers[i].neurons[j].number_of_neuron_inputs; k++) { //selecting random starting weights from -0.5 to 0.5
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
        neurons[layers - 1] = 1;
        for (int i = 0; i < layers - 1; i++) {
            cout << "Enter the number of neurons in the layer " << i << ":";
            cin >> neurons[i];
        }
        Perceptron perceptron = Perceptron(layers, neurons);
        return perceptron;
    }

    void train(double** sample, int amount) {
        for (int i = 0; i < amount; i++) {
            bool invalid = 0;
            for (int j = 1; j < this->number_of_inlets; j++) {
                if (sample[i][j] < 0) {
                    bool invalid = 1;
                    break;
                }
                this->inlets[j] = sample[i][j];
            }
            if (invalid){
                continue;
            }
            for (int j = 0; j < this->number_of_layers; j++) {
                for (int k = 0; k < this->layers[j].number_of_neurons; k++) {
                    this->layers[j].neurons[k].state_and_activate();
                }
            }
            double delta = sample[i][0] - this->layers[this->number_of_layers - 1].neurons[0].activated_state;
            cout << delta << endl;
            this->layers[this->number_of_layers - 1].neurons[0].residual = delta * SATURATION_PARAMETER * this->layers[this->number_of_layers - 1].neurons[0].activated_state * (1 - this->layers[this->number_of_layers - 1].neurons[0].activated_state);
            for (int j = this->number_of_layers - 2; j >= 0; j--) {
                for (int k = 0; k < this->layers[j].number_of_neurons; k++) {
                    double sum = 0;
                    for (int l = 0; l < this->layers[j + 1].number_of_neurons; l++) {
                        sum += this->layers[j + 1].neurons[l].residual * this->layers[j + 1].neurons[l].inputs[k + 1].weight;
                    }
                    this->layers[j].neurons[k].residual = sum * SATURATION_PARAMETER * this->layers[j].neurons[k].activated_state * (1 - this->layers[j].neurons[k].activated_state);
                }
            }
            for (int j = 0; j < this->number_of_layers; j++) {
                for (int k = 0; k < this->layers[j].number_of_neurons; k++) {
                    for (int l = 0; l < this->layers[j].neurons[k].number_of_neuron_inputs; l++) {
                        this->layers[j].neurons[k].inputs[l].weight += SPEED_OF_TRAIN * this->layers[j].neurons[k].residual * *(double*)this->layers[j].neurons[k].inputs[l].p_value;
                    }
                }
            }
        }
    }

    void test(double** sample, int amount) {

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

double** process(string* items, int amount) { //this function code depends on the sample view
    double** sample = new double* [amount];
    for (int i = 0; i < amount; i++) {
        sample[i] = new double[NUMBER_OF_INPUTS + 1];
        for (int j = 0; j < NUMBER_OF_INPUTS + 1; j++) {
            sample[i][j] = -1;
        }
    }
    for (int i = 0; i < amount; i++) {
        string line = items[i].substr(items[i].find_first_of(',') + 1, items[i].length());

        if ((line[0] == '0') or (line[0] == '1')) {
            sample[i][0] = double(line[0]) - 48; // [0] - survived
        }
        else {
            cout << "Error 2" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        if (line[0] == '1') {
            sample[i][1] = 0; // [1] - Pclass
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
            sample[i][2] = 0; // [2] - Sex
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
            sample[i][3] = stod(line.substr(0, line.find_first_of(','))); // [3] - Age
        }
        catch(invalid_argument) {
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        try {
            sample[i][4] = stod(line.substr(0, line.find_first_of(','))); // [4] - SibSp
        }
        catch (invalid_argument) {
            cout << "Error 6" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        try {
            sample[i][5] = stod(line.substr(0, line.find_first_of(','))); // [5] - Parch
        }
        catch (invalid_argument) {
            cout << "Error 7" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        if (line[line.length() - 1] == 'S') {
            sample[i][6] = 0; // [6] - embarked
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
    
    for (int i = 0; i < NUMBER_OF_INPUTS + 1; i++) {
        double max = 0;
        for (int j = 0; j < amount; j++) {
            try {
                if (sample[j][i] >= max) {
                    max = sample[j][i];
                }
            }
            catch (invalid_argument) {
                continue;
            }
        }
        for (int j = 0; j < amount; j++) {
            sample[j][i] = sample[j][i] / max;
        }
    }

    for (int i = 0; i < amount - 1; i++) {
        for (int j = i + 1; j < amount; j++) {
            bool invalid = 1;
            for (int k = 1; k < NUMBER_OF_INPUTS + 1; k++) {
                if (sample[i][k] != sample[j][k]) {
                    invalid = 0;
                    break;
                }
            }
            if (invalid) {
                sample[j][1] = -1;
            }
        }
    }

    for (int i = 0; i < amount; i++) {
        for (int j = 0; j < NUMBER_OF_INPUTS + 1; j++) {
            cout << sample[i][j] << " ";
        }
        cout << endl;
    }
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
    cout << "Train:" << endl;
    *training_sample = process(training_items, TRAINING_SAMPLE_ITEMS);
    cout << "Test:" << endl;
    *testing_sample = process(testing_items, TESTING_SAMPLE_ITEMS);
}

int main()
{
    srand(time(NULL));
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
    perceptron.train(training_sample, TRAINING_SAMPLE_ITEMS);
    perceptron.test(testing_sample, TESTING_SAMPLE_ITEMS);
}