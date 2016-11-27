#include <stdio.h>
#include <stdlib.h>
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

typedef struct {
	double activation, bias;
} neuron_t;

typedef struct {
	neuron_t inputs[2];
	neuron_t hidden[3];
	neuron_t output[1];
} network_t;

int main() {
	int i;
	network_t *net;
	char *raw_json;
	json_t *root, *neurons_json, *neuron_json;
	json_error_t error;

	net = malloc(sizeof(network_t));

	raw_json = read_file("net.json");
	root = json_loads(raw_json, 0, &error);
	free(raw_json);

	neurons_json = json_object_get(root, "neurons");

	for(i = 0; i < json_array_size(neurons_json); i++) {
		neuron_json = json_array_get(neurons_json, i);
		printf("%f\n", json_number_value(json_object_get(neuron_json, "bias")));

		
	}
}
