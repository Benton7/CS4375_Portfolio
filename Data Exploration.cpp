// Data Exploration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string>
using namespace std;
void print_stats(vector<double> col);
double getCovariance(vector<double> rm, vector<double> medv);
double getCorrelation(vector<double> rm, vector<double> medv);
int main(int argc, char** argv)
{
    //1a. Read in csv file
    ifstream inFS;
    string line;
    string rm_in, medv_in;
    const int MAX_LEN = 1000;
    vector<double> rm(MAX_LEN);
    vector<double> medv(MAX_LEN);
    //try open file
    cout << "Opening file Boston.csv" << endl;

    inFS.open("Boston.csv");
    if (!inFS.is_open())
    {
        cout << "Could not open file Boston.csv." << endl;
        return 1; //error
    }

    cout << "Reading line 1" << endl;
    getline(inFS, line);

    cout << "heading: " << line << endl;
    int numObservations = 0;
    while (inFS.good())
    {
        getline(inFS, rm_in, ',');
        getline(inFS, medv_in, '\n');

        rm.at(numObservations) = stof(rm_in);
        medv.at(numObservations) = stof(medv_in);

        numObservations++;
    }
    rm.resize(numObservations);
    medv.resize(numObservations);

    cout << "new length " << rm.size() << endl;

    cout << "Closing file Boston.csv." << endl;
    inFS.close();

    cout << "Number of records: " << numObservations << endl;

    cout << "\nStats for rm" << endl;
    print_stats(rm);

    cout << "\nStats for medv" << endl;
    print_stats(medv);

    cout << "\nCovariance = " << getCovariance(rm, medv) << endl;
    cout << "\nCorrelation = " << getCorrelation(rm, medv) << endl;
    cout << "\nProgram terminated.";
    return 0;
}
double getSum(vector<double> col)
{
    double sum = 0;
    for (int x = 0; x < col.size(); x++)
    {
        sum += col[x];
    }
    return sum;
}
 double getMean(vector<double> col)
{
    double mean = 0;
    for (int x = 0; x < col.size(); x++)
    {
        mean += col[x];
    }
    return (mean / col.size());
}

double getMedian(vector<double> col)
{
    sort(col.begin(), col.end());
    if (col.size() % 2 == 0)
        return (col[(int)(col.size() / 2)] + col[(int)(col.size() / 2) + 1]) / 2;
    else
        return col[col.size() / 2];
}
double getRange(vector<double> col)
{
    double min = col[0];
    double max = col[0];
    for (int x = 0; x < col.size(); x++)
    {
        if (col[x] > max)
            max = col[x];
        if (col[x] < min)
            min = col[x];
    }
    return max - min;
}
void print_stats(vector<double> col)
{
    cout << "Sum: " << getSum(col) << endl;
    cout << "Mean: " << getMean(col) << endl;
    cout << "Median: " << getMedian(col) << endl;
    cout << "Range: " << getRange(col) << endl;
}
double getCovariance(vector<double> rm, vector<double> medv)
{
    if (!(rm.size() == medv.size()))
    {
        cout << "Cannot compute the covariance between these vectors." << endl;
        return 1;
    }
    double temp = 0;
    double meanRM = getMean(rm);
    double meanMedv = getMean(medv);
    for (int x = 0; x < rm.size(); x++)
    {
        temp += (rm[x] - meanRM) * (medv[x] - meanMedv);
    }
    double covariance = temp / (rm.size()-1);
    return temp / (rm.size());
}
double getCorrelation(vector<double> rm, vector<double> medv)
{
    if (!(rm.size() == medv.size()))
    {
        cout << "Cannot compute the correlation between these vectors." << endl;
        return 1;
    }
    //standard deviation
    double rmStdDev = 0;
    double medvStdDev = 0;
    double rmMean=getMean(rm);
    double medvMean = getMean(medv);
    double temp = 0;
    for (int x=0; x < rm.size(); x++)
    {
        temp += (rm[x] - rmMean) * (rm[x] - rmMean);
    }
    temp = temp / rm.size();
    rmStdDev = sqrt(temp);
    temp = 0;
    for (int x = 0; x < medv.size(); x++)
    {
        temp += (medv[x] - medvMean) * (medv[x] - medvMean);
    }
    temp = temp / medv.size();
    medvStdDev = sqrt(temp);
    return getCovariance(rm, medv)/(rmStdDev*medvStdDev);
}
