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
const int AGES = 4000;
const int CHECK_ERRORS = 20;
const int CHECK_ERROR_EVERY = AGES / CHECK_ERRORS;
const double SATURATION_PARAMETER = 1;
const double SPEED_OF_TRAIN = 1;

using namespace std;

class NeuronInput {
public:
    double* p_value;
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
            cout << *this->inputs[i].p_value << endl;
            state += *this->inputs[i].p_value * this->inputs[i].weight; //smth wrong with the 1st layer
        }
        this->activated_state = 1 / (1 + exp(-SATURATION_PARAMETER * state));
    }
};

class NeuronLayer {
public:
    int number_of_neurons;
    Neuron* neurons;
};

class Sample {
public:
    double** sample;
    int inputs;
    int samples;

    Sample(int num) {
        inputs = NUMBER_OF_INPUTS + 1;
        samples = num;
        sample = new double* [samples];
        for (int i = 0; i < samples; i++) {
            sample[i] = new double[inputs];
        }
    }

    void normalize() {
        for (int i = 0; i < this->inputs; i++) {
            double max = 0;
            for (int j = 0; j < this->samples; j++) {
                if (this->sample[j][i] >= max) {
                    max = this->sample[j][i];
                }
            }
            for (int j = 0; j < this->samples; j++) {
                this->sample[j][i] = this->sample[j][i] / max;
            }
        }
    }

    void remove_unwanted() { // removing items with missing data, repetitions and contradictions
        for (int i = 0; i < this->samples - 1; i++) {
            for (int j = i + 1; j < this->samples; j++) {
                bool invalid = 1;
                for (int k = 1; k < this->inputs; k++) {
                    if (this->sample[i][k] != this->sample[j][k]) {
                        invalid = 0;
                        break;
                    }
                }
                if (invalid) {
                    this->sample[j][1] = -1; //REMOVE
                }
            }
        }
        for (int i = 0; i < this->samples; i++) {
            bool invalid = 0;
            for (int j = 0; j < this->inputs; j++) {
                if (this->sample[i][j] < 0) {
                    invalid = 1;
                    break;
                }
            }
            if (invalid) {
                this->samples--;
                for (int j = i; j < this->samples; j++) {
                    for (int k = 0; k < this->inputs; k++) {
                        this->sample[j][k] = this->sample[j + 1][k];
                    }
                }
                i--;
            }
        }
    }

    void length_set_to_1() { //useless
        for (int i = 0; i < this->samples; i++) {
            double length = 0;
            for (int j = 1; j < this->inputs; j++) {
                length += pow(this->sample[i][j], 2);
            }
            length = sqrt(length);
            for (int j = 1; j < this->inputs; j++) {
                this->sample[i][j] /= length;
            }
        }
    }

    void shuffle() {
        for (int i = 0; i < this->samples; i++) {
            int id = rand() % this->samples;
            double* temp = new double[this->inputs];
            for (int j = 0; j < this->inputs; j++) {
                temp[j] = this->sample[i][j];
                this->sample[i][j] = this->sample[id][j];
                this->sample[id][j] = temp[j];
            }
        }
    }

    void output() {
        for (int i = 0; i < this->samples; i++) {
            for (int j = 0; j < NUMBER_OF_INPUTS + 1; j++) {
                cout << this->sample[i][j] << " ";
            }
            cout << endl;
        }
        cout << this->samples << endl;
    }
};

class Perceptron {
public:
    double FICTITIOUS_UNIT = 1;
    int number_of_inlets;
    double* inlets;
    int number_of_layers;
    NeuronLayer* layers;

    Perceptron(int nol, int* non) { //creates the whole perceptron: the inputs, the neuron layers, the neurons, the connections
        //FICTITIOUS_UNIT = 1;
        number_of_inlets = NUMBER_OF_INPUTS; //creating inputs..
        inlets = new double[number_of_inlets];
        number_of_layers = nol; //creating neuron layers..
        layers = new NeuronLayer[number_of_layers];
        for (int i = 0; i < number_of_layers; i++) { //creating neurons in each layer..
            layers[i].number_of_neurons = non[i];
            layers[i].neurons = new Neuron[layers[i].number_of_neurons];
            for (int j = 0; j < layers[i].number_of_neurons; j++) { //creating connections in each neuron..
                if (i == 0) { //the number of connections in each neuron of the first layer equals to the number of inputs + 1
                    layers[i].neurons[j].number_of_neuron_inputs = number_of_inlets + 1;
                    layers[i].neurons[j].inputs = new NeuronInput[layers[i].neurons[j].number_of_neuron_inputs];
                    for (int k = 0; k < layers[i].neurons[j].number_of_neuron_inputs; k++) { //connecting to the inputs, including the unit
                        if (k == 0) {
                            layers[i].neurons[j].inputs[k].p_value = &FICTITIOUS_UNIT;
                            //cout << FICTITIOUS_UNIT << endl;
                            //cout << *(double*)layers[i].neurons[j].inputs[k].p_value << endl;
                        }
                        else {
                            layers[i].neurons[j].inputs[k].p_value = &inlets[k - 1];
                        }
                    }
                }
                else {
                    layers[i].neurons[j].number_of_neuron_inputs = layers[i - 1].number_of_neurons + 1;
                    layers[i].neurons[j].inputs = new NeuronInput[layers[i].neurons[j].number_of_neuron_inputs];
                    layers[i].neurons[j].inputs[0].p_value = &FICTITIOUS_UNIT; //connecting to the unit
                    for (int k = 1; k < layers[i].neurons[j].number_of_neuron_inputs; k++) {
                        layers[i].neurons[j].inputs[k].p_value = &layers[i - 1].neurons[k - 1].activated_state; //connecting to each neuron from the previous layer
                    }
                }
                for (int k = 0; k < layers[i].neurons[j].number_of_neuron_inputs; k++) { //selecting random starting weights from -0.5 to 0.5
                    double r = rand();
                    layers[i].neurons[j].inputs[k].weight = r / RAND_MAX - 0.5;
                    //layers[i].neurons[j].inputs[k].weight = 0.5;
                }
            }
        }
    }

    ~Perceptron() {
    }

    static Perceptron make_a_perceptron() {
        int layers;
        cout << "Enter the number of neuron layers in the perceptron:";
        cin >> layers;
        int* neurons = new int[layers];
        neurons[layers - 1] = 2;
        for (int i = 0; i < layers - 1; i++) {
            cout << "Enter the number of neurons in the layer " << i << ":";
            cin >> neurons[i];
        }
        Perceptron perceptron = Perceptron(layers, neurons);
        return perceptron;
    }

    void train(Sample sample_set) {
        cout << "Training.." << endl;
        for (int a = 0; a < AGES; a++) {
            double error = 0;
            //sample_set.shuffle();
            for (int i = 0; i < sample_set.samples; i++) {
                for (int j = 0; j < this->number_of_inlets; j++) {
                    this->inlets[j] = sample_set.sample[i][j + 1];
                    cout << this->inlets[j] << " ";
                }
                cout << endl;
                for (int j = 0; j < this->number_of_layers; j++) {
                    for (int k = 0; k < this->layers[j].number_of_neurons; k++) {
                        this->layers[j].neurons[k].state_and_activate();
                    }
                }
                double delta0, delta1;
                if (sample_set.sample[i][0] == 0) {
                    delta0 = 1 - this->layers[this->number_of_layers - 1].neurons[0].activated_state;
                    delta1 = -this->layers[this->number_of_layers - 1].neurons[1].activated_state;
                }
                else if (sample_set.sample[i][0] == 1) {
                    delta0 = -this->layers[this->number_of_layers - 1].neurons[0].activated_state;
                    delta1 = 1 - this->layers[this->number_of_layers - 1].neurons[1].activated_state;
                }
                else {
                    cout << "Error sample[0] not 0 or 1" << endl;
                }
                /*if (a % CHECK_ERROR_EVERY == 0) {
                    error += pow(delta, 2);
                }*/
                this->layers[this->number_of_layers - 1].neurons[0].residual = delta0 * SATURATION_PARAMETER * this->layers[this->number_of_layers - 1].neurons[0].activated_state * (1 - this->layers[this->number_of_layers - 1].neurons[0].activated_state);
                this->layers[this->number_of_layers - 1].neurons[1].residual = delta1 * SATURATION_PARAMETER * this->layers[this->number_of_layers - 1].neurons[1].activated_state * (1 - this->layers[this->number_of_layers - 1].neurons[1].activated_state);

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
                            this->layers[j].neurons[k].inputs[l].weight += SPEED_OF_TRAIN * this->layers[j].neurons[k].residual * *this->layers[j].neurons[k].inputs[l].p_value;
                        }
                    }
                }
            }
            /*if (a % CHECK_ERROR_EVERY == 0) {
                error = sqrt(error);
                cout << error << endl;
            }*/
        }
    }

    void test(Sample sample_set) {
        cout << "Test errors:" << endl;
        //double error = 0;
        for (int i = 0; i < sample_set.samples; i++) {
            for (int j = 0; j < this->number_of_inlets; j++) {
                this->inlets[j] = sample_set.sample[i][j + 1];
            }
            for (int j = 0; j < this->number_of_layers; j++) {
                for (int k = 0; k < this->layers[j].number_of_neurons; k++) {
                    this->layers[j].neurons[k].state_and_activate();
                }
            }
            //double delta = sample_set.sample[i][0] - this->layers[this->number_of_layers - 1].neurons[0].activated_state;
            //error += pow(delta, 2);
            //cout << delta << endl;
            cout << "Sample: " << sample_set.sample[i][0] << ", Net: 0: " << this->layers[this->number_of_layers - 1].neurons[0].activated_state << "1: " << this->layers[this->number_of_layers - 1].neurons[1].activated_state << endl;
            /*if (this->layers[this->number_of_layers - 1].neurons[0].activated_state > this->layers[this->number_of_layers - 1].neurons[1].activated_state) {
                cout << "0" << endl;
            }
            else {
                cout << "1" << endl;
            }*/
        }
    }
};

Sample process(string* items, int num) { //this function code depends on the sample view
    Sample sample_set(num);
    for (int i = 0; i < sample_set.samples; i++) {
        string line = items[i].substr(items[i].find_first_of(',') + 1, items[i].length());

        if ((line[0] == '0') or (line[0] == '1')) {
            sample_set.sample[i][0] = double(line[0]) - 48; // [0] - survived
        }
        else {
            cout << "Error 2" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        if (line[0] == '1') {
            sample_set.sample[i][1] = 0; // [1] - Pclass
        }
        else if (line[0] == '2') {
            sample_set.sample[i][1] = 0.5;
        } 
        else if (line[0] == '3'){
            sample_set.sample[i][1] = 1;
        }
        else {
            cout << "Error 3" << endl;
            continue;
        }
        line = line.substr(line.find("\",") + 2, line.length());

        if (line.substr(0, line.find_first_of(',')) == "female") {
            sample_set.sample[i][2] = 0; // [2] - Sex
        }
        else if (line.substr(0, line.find_first_of(',')) == "male") {
            sample_set.sample[i][2] = 1;
        }
        else {
            cout << "Error 4" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        try {
            sample_set.sample[i][3] = stod(line.substr(0, line.find_first_of(','))); // [3] - Age
        }
        catch(invalid_argument) {
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        try {
            sample_set.sample[i][4] = stod(line.substr(0, line.find_first_of(','))); // [4] - SibSp
        }
        catch (invalid_argument) {
            cout << "Error 6" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        try {
            sample_set.sample[i][5] = stod(line.substr(0, line.find_first_of(','))); // [5] - Parch
        }
        catch (invalid_argument) {
            cout << "Error 7" << endl;
            continue;
        }
        line = line.substr(line.find_first_of(',') + 1, line.length());

        if (line[line.length() - 1] == 'S') {
            sample_set.sample[i][6] = 0; // [6] - embarked
        }
        else if (line[line.length() - 1] == 'C') {
            sample_set.sample[i][6] = 0.5;
        }
        else if (line[line.length() - 1] == 'Q') {
            sample_set.sample[i][6] = 1;
        }
        else {
            continue;
        }
    }
    sample_set.normalize();
    sample_set.remove_unwanted();

    return sample_set;
}

ifstream openfile(string name) {
    ifstream file(name);
    if (!file.is_open()) {
        cout << "Error 0" << endl;
        exit(-1);
    }
    return file;
}

void get_the_samples(Sample* training_sample, Sample* testing_sample) {
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
    //cout << "Train:" << endl;
    *training_sample = process(training_items, TRAINING_SAMPLE_ITEMS);
    //cout << "Test:" << endl;
    *testing_sample = process(testing_items, TESTING_SAMPLE_ITEMS);
}

int main()
{
    srand(time(NULL));
    Perceptron perceptron = Perceptron::make_a_perceptron();

    cout << *perceptron.layers[0].neurons[0].inputs[0].p_value << endl; //FICTITIOUS_UNIT = 1 ok

    Sample training_sample(TRAINING_SAMPLE_ITEMS);
    Sample testing_sample(TESTING_SAMPLE_ITEMS);

    cout << *perceptron.layers[0].neurons[0].inputs[0].p_value << endl; //FICTITIOUS_UNIT = 6.95137e-310 ???
    cout << perceptron.FICTITIOUS_UNIT << endl; //1

    get_the_samples(&training_sample, &testing_sample);
    //training_sample.output();
    testing_sample.output();
    perceptron.train(training_sample);
    perceptron.test(testing_sample);
}