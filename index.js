const fs = require("fs");
const { Architect } = require("synaptic");

const network = new Architect.Perceptron(2,3,1);
network.trainer.XOR();

fs.writeFileSync(__dirname + "/net.json", JSON.stringify(network.toJSON(), null, "\t"));
