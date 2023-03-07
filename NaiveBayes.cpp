
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <string>
using namespace std;
using namespace std::chrono;
double calc_age_lh(double age, double age_mean, double age_var)
{
	

	double temp = 1 / sqrt(2 * 3.14159265359 * age_var);
	double squared = (age - age_mean) * (age - age_mean) * -1;
	return temp * exp(squared / (2 * age_var));
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
	train_data.resize(4, vector<double>(800));
	test_data.resize(4, vector<double>(testRows));

	for (int x = 0; x < 800; x++)
	{
		train_data[0][x] = pclass[x];
		train_data[1][x] = sex[x];
		train_data[2][x] = age[x];
		train_data[3][x] = survived[x];
	}
	for (int x = 800; x < temp; x++)
	{
		test_data[0][x - 800] = pclass[x];
		test_data[1][x - 800] = sex[x];
		test_data[2][x - 800] = age[x];
		test_data[3][x - 800] = survived[x];
	}
	time_point<system_clock> begin;
	begin = system_clock().now();
	//create the naive bayes model
	//get the survived counts for no and yes
	int yes_survived = 0;
	int no_survived = 0;
	for (int x = 0; x < train_data[3].size(); x++)
	{
		if (train_data[3][x] == 1)
			yes_survived++;
		else
			no_survived++;
	}
	//get the likelihood of pclass
	vector<vector<double>> lh_pclass(3, vector<double>(2));
	vector<int> sv = { 0, 1 };
	vector<int> pc = { 1, 2, 3 };
	for (int x = 0; x < sv.size(); x++)
	{
		for (int y = 1; y <= pc.size(); y++)
		{
			int sv_val = sv[x];
			int pc_val = pc[y-1];
			int temp = 0;
			for (int z = 0; z < train_data[0].size(); z++)
			{
				if (y == train_data[0][z] && x == train_data[3][z])
					temp++;
			}
			
			if(x==1)
				lh_pclass[pc_val - 1][x] = (double)temp / yes_survived;
			else
				lh_pclass[pc_val - 1][x] = (double)temp / no_survived;
		}
	}
	//get the likelihood of sex
	vector<vector<double>> lh_sex(2, vector<double>(2));
	vector<int> sx = { 0, 1 };
	for (int x = 0; x < sv.size(); x++)
	{
		for (int y = 0; y < sx.size(); y++)
		{
			int sv_val = sv[x];
			int sx_val = sx[y];
			temp = 0;
			for (int z = 0; z < train_data[3].size(); z++)
			{
				if (y == train_data[1][z] && x == train_data[3][z])
					temp++;
			}
			if (sv_val == 1)
				lh_sex[y][x] = (double)temp / yes_survived;
			else
				lh_sex[y][x] = (double)temp / no_survived;
		}
	}
	//get the means and variances of age
	vector<double> age_mean{ 0, 0 };
	vector<double> age_var{ 0, 0 };
	int index = 0;
	int index2 = 0;
	vector<double> survivedOnly(train_data[2]);
	vector<double> diedOnly(train_data[2]);
	for (int y = 0; y < train_data[3].size(); y++)
	{
		if (train_data[3][y] == 1)
		{
			survivedOnly[index] = train_data[2][y];
			index++;
		}
		else
		{
			diedOnly[index2] = train_data[2][y];
			index2++;
		}
	}
	survivedOnly.resize(index);
	diedOnly.resize(index2);
	//find the means
	double temp1=0;
	for (int y = 0; y < survivedOnly.size(); y++)
		temp1 += survivedOnly[y];
	age_mean[0] =  temp1 / survivedOnly.size();
	double temp2 = 0;
	for (int y = 0; y < diedOnly.size(); y++)
		temp2 += diedOnly[y];
	age_mean[1] = temp2 / diedOnly.size();
	//find the variances
	double temp3=0;
	for (int y = 0; y < survivedOnly.size(); y++)
		temp3 += pow(survivedOnly[y] - (age_mean[0]), 2);
	age_var[0] = temp3 / (survivedOnly.size() - 1);
	double temp4 = 0;
	for (int y = 0; y < diedOnly.size(); y++)
		temp4 += pow(diedOnly[y] - age_mean[1], 2);
		
	
	age_var[1] = temp4 / (diedOnly.size() - 1);
	//calculate std from the variances 
	duration<double> time = system_clock().now() - begin;
	//output all the information
	cout << "Survived Counts: \nYes= " << (double)yes_survived/train_data[3].size() << " No= " << (double)no_survived / train_data[3].size() << endl;
	cout << "Time : " << time.count() << endl;
	//output the probabilities
	cout << "pclass probabilites: \n" << lh_pclass[0][0] << " " << lh_pclass[1][0] << " " << lh_pclass[2][0] << endl;
	cout << lh_pclass[0][1] << " " << lh_pclass[1][1] << " " << lh_pclass[2][1] << endl;
	cout << "test" << endl;
	cout << "sex probabilites: \n" << lh_sex[0][0] << " " << lh_sex[1][0]  << endl;
	cout << lh_sex[0][1] << " " << lh_sex[1][1] << endl;
	cout << "age probabilites: \n" << age_mean[0] << " " << age_mean[1]  << endl;
	cout << sqrt(age_var[0]) << " " << sqrt(age_var[1]) <<  endl;
	//calculate predicted values 
	
	vector<vector<double>> preds(test_data[0].size());
	vector<double> prob_survived(preds.size());
	vector<double> prob_died(preds.size());
	
	for (int x = 0; x < preds.size(); x++)
	{
		double num_s = lh_pclass[test_data[0][x] - 1][1] * lh_sex[test_data[1][x]][1] * (double)yes_survived / train_data[3].size() *
			calc_age_lh(test_data[2][x], age_mean[0], age_var[1]);
		double num_p = lh_pclass[test_data[0][x] - 1][0] * lh_sex[test_data[1][x]][0] * (double)no_survived / train_data[3].size() *
			calc_age_lh(test_data[2][x], age_mean[1], age_var[0]);
		double denominator = num_s + num_p;
		 prob_survived[x]=num_s / denominator;
		 prob_died[x]= num_p / denominator;
	}
	vector<double> probs(preds.size());
	
	for (int x = 0; x < prob_survived.size(); x++) {
		if (prob_survived[x] > 0.5) {
			probs[x] = 1;
		}
		else {
			probs[x] = 0;
		}
	}
	
	//Count the true/false positives/negatives
	int tP = 0, fN = 0, fP = 0, tN = 0;
	for (int x = 0; x < probs.size(); x++)
	{
		if (probs[x] == 1 && test_data[3][x] == 1)
			tP++;
		if (probs[x] == 1 && test_data[3][x] == 0)
			fP++;
		if (probs[x] == 0 && test_data[3][x] == 1)
			fN++;
		if (probs[x] == 0 && test_data[3][x] == 0)
			tN++;
	}
	
	//calculate and output the accuracy, sensitivity, and specificity
	cout << "Accuracy: " << (tP + tN) / (double)(tP + tN + fP + fN) << endl;
	cout << "Sensitivity: " << (tP) / (double)(tP + fN) << endl;
	cout << "Specificity: " << (tN) / (double)(tN + fP) << endl;

	

}