set -e

dir=/tmp/lake-zip
mkdir -p $dir


for f in neural.cpp Lake.hpp Neural.hpp NeuralNetworkData.hpp Error.hpp ../tools/NeuralNetwork/MultiNeuralNetwork.hpp ../tools/GeneticLearning/LearningParameters.hpp ../tools/NeuralNetwork/NeuralNetwork.hpp ../tools/NeuralNetwork/NeuralNetwork.cpp ../tools/NeuralNetwork/NeuronWeights.hpp; do
    cp $f $dir
done

curdir=$(pwd)
cd $dir
zip lake.zip *
mv lake.zip $curdir

#rm -rf $dir
