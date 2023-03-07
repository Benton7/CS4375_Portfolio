#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <chrono>
#include <string>
using namespace std;
using namespace std::chrono;
//sigmoid function
double sigmoid(double z) {
	return 1.0 / (1 + exp(-1 * z));
}
//multiply a vector by a matrix and return the sum of each of the rows multiplied by the vector
vector<double> matrix_vector_multiply(vector<vector<double>> matrix, vector<double> vector) {
	int x = matrix.size();
	int y = matrix[0].size();
	std::vector<double> result(y, 0.0);
	for (int z = 0; z < y; z++) {
		for (int a = 0; a < x; a++) {
			result[z] += matrix[a][z] * vector[a];
		}
	}
	return result;
}
//transpose the matrix
vector<vector<double>> transMatrix(vector<vector<double>> matrix) {
	vector<vector<double>> holdMatrix(matrix[0].size(), vector<double>(matrix.size()));
	for (int x = 0; x < matrix.size(); x++) {
		for (int y = 0; y < matrix[x].size(); y++) {
			holdMatrix[y][x] = matrix[x][y];
		}
	}
	return holdMatrix;
}
//find the coefficients
vector<double> findCoef(vector<vector<double>> train_data) {
	vector<vector<double>> data_matrix(2, vector<double>(train_data[0].size()));
	for (int i = 0; i < train_data[0].size(); i++) {
		data_matrix[0][i] = 1;
		data_matrix[1][i] = train_data[1][i];
	}
	//initialize important components for finding the coefficients
	vector<double> weights = { 1, 1 };
	vector<double> labels = train_data[0];
	double lr = 0.001;
	vector<double> probabilities(labels.size()); 
	vector<double> errors(labels.size());
	//perform the gradient descent
	for (int x = 1; x < 1000; x++) {
		//find probabilities
		//multiply data by the weights
		vector<double> result = matrix_vector_multiply(data_matrix, weights);
		for (int y = 0; y < data_matrix[0].size(); y++)
		{
			probabilities[y] = sigmoid(result[y]);
		}
		for (int y = 0; y < labels.size(); y++)
		{
			errors[y] = labels[y] - probabilities[y];
		}
		//transpose data 
		train_data = transMatrix(train_data);
		//multiply the data by the errors
		vector<vector<double>> tranMatrix = transMatrix(data_matrix);
		vector<double> temp = matrix_vector_multiply(tranMatrix, errors);
		//scalar multiply the data by the learning rate
		for (int y = 0; y < temp.size(); y++)
		{
			temp[y] = temp[y] * lr;
		}
		//add weights and the matrix
		for (int y = 0; y < temp.size(); y++)
		{
			weights[y] += temp[y];
		}
	}
	return weights;
}
vector<double> predict(vector<vector<double>> testMatrix, vector<double> weights)
{
	vector<double> pred = matrix_vector_multiply(testMatrix, weights);

	vector<double> prob(pred.size());

	//Calculate the probabilties
	for (int i = 0; i < prob.size(); i++)
	{
		prob[i] = exp(pred[i]) / (1 + exp(pred[i]));
	}

	//Based on probabilties predict if survived or not
	for (int i = 0; i < pred.size(); i++)
	{
		if (prob[i] > .5)
			pred[i] = 1;
		else
			pred[i] = 0;
	}
	return pred;
}

int main(int argc, char** argv)
{
	vector<vector<double>> train_data;
	vector<vector<double>> test_data;
	ifstream inFS;
	string line;
	string inNum, inPclass, inSurvived, inSex, inAge;
	vector<double> pclass, survived, sex, age;

	cout << "Opening file" << endl;
	inFS.open("titanic_project.csv");
	if (!inFS.is_open())
	{
		cout << "Could not open file" << endl;
		return 1;
	}
	//remove the top line
	getline(inFS, line); 
	int temp = 0;
	pclass.resize(1100);
	survived.resize(1100);
	sex.resize(1100);
	age.resize(1100);

	while (getline(inFS, inNum, ',') && getline(inFS, inPclass, ',') &&
		getline(inFS, inSurvived, ',') && getline(inFS, inSex, ',') &&
		getline(inFS, inAge))
	{
		pclass.at(temp) = stod(inPclass);
		survived.at(temp) = stod(inSurvived);
		sex.at(temp) = stod(inSex);
		age.at(temp) = stod(inAge);

		temp++;
	}

	inFS.close();

	pclass.resize(temp);
	survived.resize(temp);
	sex.resize(temp);
	age.resize(temp);

	int testRows = temp - 800;
	train_data.resize(2, vector<double>(800));
	test_data.resize(2, vector<double>(testRows));

	for (int x = 0; x < 800; x++)
	{
		train_data[0][x] = survived[x];
		train_data[1][x] = sex[x];
	}

	for (int x = 800; x < temp; x++)
	{
		test_data[0][x - 800] = survived[x];
		test_data[1][x - 800] = sex[x];
	}
	time_point<system_clock> begin;
	begin = system_clock().now();
	vector<double> weights = findCoef(train_data);
	duration<double> time = system_clock().now() - begin;
	cout << "Coefficients: " << weights[0] << " " << weights[1] << endl;
	cout << "Time : " << time.count() << endl;
	vector<vector<double>> temp_test = test_data;
	for (int x = 0; x < test_data[0].size(); x++)
	{
		temp_test[0][x] = 1;
	}
	vector<double> preds = predict(temp_test, weights);
	//Count the true/false positives/negatives
	int tP = 0, fN = 0, fP = 0, tN = 0;
	for (int i = 0; i < preds.size(); i++)
	{
		if (preds[i] == 1 && test_data[0][i] == 1)
			tP++;
		if (preds[i] == 1 && test_data[0][i] == 0)
			fP++;
		if (preds[i] == 0 && test_data[0][i] == 1)
			fN++;
		if (preds[i] == 0 && test_data[0][i] == 0)
			tN++;
	}
	//calculate and output the accuracy, sensitivity, and specificity
	cout << "Accuracy: " << (tP + tN) / (double)(tP + tN+fP+fN) << endl;
	cout << "Sensitivity: " << (tP) / (double)(tP+fN) << endl;
	cout << "Specificity: " << (tN) / (double)(tN + fP) << endl;
	return 0;
}
