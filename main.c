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
	number_t activation, bias;
} neuron_t;

typedef struct {
	neuron_t input[2];
	neuron_t hidden[3];
	neuron_t output[1];
} network_t;

int main() {
	int i;
	network_t *net;
	neuron_t neuron;
	char *raw_json;
	char *layer;
	json_t *root, *neurons_json, *neuron_json;
	json_error_t error;
	size_t input_counter = 0;
	size_t hidden_counter = 0;
	size_t output_counter = 0;

	net = malloc(sizeof(network_t));

	raw_json = read_file("net.json");
	root = json_loads(raw_json, 0, &error);
	free(raw_json);

	neurons_json = json_object_get(root, "neurons");

	#define object_get_string(object, key) json_string_value(json_object_get(object, key))
	#define object_get_number(object, key) json_number_value(json_object_get(object, key))

	for(i = 0; i < json_array_size(neurons_json); i++) {
		neuron_json = json_array_get(neurons_json, i);

		neuron.activation = object_get_number(neuron_json, "activation");
		neuron.bias = object_get_number(neuron_json, "bias");

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

	#define NEURON_STR "output %d\tbias: %f\tactivation: %f\n"

	for(i = 0; i < sizeof(net->input) / sizeof(neuron_t); i++) {
		printf(NEURON_STR, i, net->input[i].bias, net->input[i].activation);
	}

	for(i = 0; i < sizeof(net->hidden) / sizeof(neuron_t); i++) {
		printf(NEURON_STR, i, net->hidden[i].bias, net->hidden[i].activation);
	}

	for(i = 0; i < sizeof(net->output) / sizeof(neuron_t); i++) {
		printf(NEURON_STR, i, net->output[i].bias, net->output[i].activation);
	}
}
