#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

char *read_file(char *filename) {
	long int length;
	char *buffer;
	FILE *f = fopen(filename, "r");

	if(f == NULL) {
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	length = ftell(f);

	buffer = malloc(length + 1);

	if(buffer == NULL) {
		fclose(f);
		return NULL;
	}

	fseek(f, 0, SEEK_SET);

	if(fread(buffer, 1, length, f) != length) {
		fclose(f);
		free(buffer);
		return NULL;
	}

	fclose(f);

	buffer[length] = '\0';
	return buffer;
}

typedef double number_t;

typedef struct {
	number_t activation;
	number_t bias;
	number_t state;
	int id;
} neuron_t;

#define INPUT_NEURONS 2
#define HIDDEN_NEURONS 3
#define OUTPUT_NEURONS 1
#define TOTAL_NEURONS INPUT_NEURONS + HIDDEN_NEURONS + OUTPUT_NEURONS

typedef struct {
	neuron_t input[INPUT_NEURONS];
	neuron_t hidden[HIDDEN_NEURONS];
	neuron_t output[OUTPUT_NEURONS];
	number_t weights_ih[INPUT_NEURONS][HIDDEN_NEURONS];
	number_t weights_ho[HIDDEN_NEURONS][HIDDEN_NEURONS];
} network_t;

#define object_get_string(object, key) json_string_value(json_object_get(object, key))
#define object_get_number(object, key) json_number_value(json_object_get(object, key))
#define NEURON_STR "output %d\tbias: %f\tactivation: %f\n"

size_t strtosize_t(const char *input) {
	size_t i;
	sscanf(input, "%zu", &i);

	return i;
}

void network_activate(network_t *net) {
	int i, j;
	double iln, hln, oln; /* ln(input), ln(hidden), ln(output) */

	for(i = 0; i < INPUT_NEURONS; i++) {
		for(j = 0; j < HIDDEN_NEURONS; j++) {
			printf("INPUT(%d) HIDDEN(%d) WEIGHT\t%f\n", i, j, net->weights_ih[i][j]);
		}
	}
}

int main() {
	int i, j;
	network_t *net;
	neuron_t neuron;
	number_t weight;
	char *raw_json;
	char *layer;
	size_t from, to;
	json_t *root, *neurons_json, *neuron_json, *connections_json, *connection_json;
	json_error_t error;
	size_t input_counter = 0;
	size_t hidden_counter = 0;
	size_t output_counter = 0;

	printf("sizeof(network_t) = %lu\n", sizeof(network_t));

	net = calloc(1, sizeof(network_t));

	raw_json = read_file("net.json");
	root = json_loads(raw_json, 0, &error);
	free(raw_json);

	/* Parse neurons json */

	neurons_json = json_object_get(root, "neurons");

	for(i = 0; i < json_array_size(neurons_json); i++) {
		neuron_json = json_array_get(neurons_json, i);

		neuron.activation = object_get_number(neuron_json, "activation");
		neuron.bias = object_get_number(neuron_json, "bias");
		neuron.id = i;

		layer = (char *) object_get_string(neuron_json, "layer");

		if(strcmp(layer, "input") == 0) {
			net->input[input_counter++] = neuron;
		} else if(strcmp(layer, "0") == 0) {
			net->hidden[hidden_counter++] = neuron;
		} else if(strcmp(layer, "output") == 0) {
			net->output[output_counter++] = neuron;
		} else {
			printf("Error: layer '%s' not recognised.\n", layer);
		}
	}

	for(i = 0; i < sizeof(net->input) / sizeof(neuron_t); i++) {
		printf(NEURON_STR, i, net->input[i].bias, net->input[i].activation);
	}

	for(i = 0; i < sizeof(net->hidden) / sizeof(neuron_t); i++) {
		printf(NEURON_STR, i, net->hidden[i].bias, net->hidden[i].activation);
	}

	for(i = 0; i < sizeof(net->output) / sizeof(neuron_t); i++) {
		printf(NEURON_STR, i, net->output[i].bias, net->output[i].activation);
	}

	/* Parse connections json */

	connections_json = json_object_get(root, "connections");

	for(i = 0; i < json_array_size(connections_json); i++) {
		connection_json = json_array_get(connections_json, i);

		from = strtosize_t(object_get_string(connection_json, "from"));
		to = strtosize_t(object_get_string(connection_json, "to"));

		weight = object_get_number(connection_json, "weight");

		if(from < INPUT_NEURONS) {
			net->weights_ih[from][to - INPUT_NEURONS] = weight;
		} else {
			net->weights_ho[from - INPUT_NEURONS][to - INPUT_NEURONS - HIDDEN_NEURONS] = weight;
		}
	}

	/* Run the net! */

	net->input[0].state = 0;
	net->input[1].state = 1;

	network_activate(net);

	printf("%f\n", net->output[0].state);
}
