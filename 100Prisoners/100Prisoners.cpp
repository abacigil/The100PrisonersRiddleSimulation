// 100Prisoners.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <random>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <filesystem>


int totalNumberOfSimulations = 100;
int totalNumberOfPrisoners = 100;
int limitToCheck = 50;
std::map<int, int> boxes;
std::vector<int> checks;
bool enableLog = false;
std::string base_directory = "simulation";
std::string checkLogs = "";


struct  SimulationResult {
	float lessThanLimitPercentage;
	float moreThanLimitPercentage;
	bool success;
};
std::map<int, SimulationResult> results;

void log(std::string output) {
	if (enableLog) {
		std::cout << output;
	}
}

void writeBoxToFile(int simulation, std::string results);
void writeResultsToFile(std::string results);

/// <summary>
/// First create a simple array that contains only numbers.
/// Shuffle the array
/// Assign shuffled numbers to the "boxes" map, thus, boxes will have random numbers.
/// </summary>
void createBoxes(int simulation) {
	std::vector<size_t> arrayToShuffle;

	for (size_t i = 0; i < totalNumberOfPrisoners; i++)
	{
		arrayToShuffle.push_back(i);
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	std::shuffle(arrayToShuffle.begin(), arrayToShuffle.end(), std::default_random_engine(seed));

	std::string boxResult = "";

	// Assign randomly placed numbers to boxes.
	for (size_t i = 0; i < totalNumberOfPrisoners; i++)
	{
		boxes[i] = arrayToShuffle[i];
		boxResult += std::to_string(i) + std::to_string(arrayToShuffle[i]) + "\n";
	}

	writeBoxToFile(simulation, boxResult);
}

void openBox(int simulation, int prisonerNumber, int boxNumber, int checkCount) {
	// If checks in a loop reach to the limit
	if (checkCount > limitToCheck) {
		log("LIMIT REACHED.\n");
	}

	// Go to box and open it
	int boxValue = boxes[boxNumber];
	log(std::to_string(checkCount) + ". Check: Prisoner " + std::to_string(prisonerNumber) + " checking box " + std::to_string(boxNumber) + " box value:" + std::to_string(boxValue) + "\n");
	checkLogs += std::to_string(simulation) + "\t" + std::to_string(checkCount) + "\t" + std::to_string(prisonerNumber) + "\t" + std::to_string(boxNumber) + "\t" + std::to_string(boxValue) + "\n";
	// If the number inside the box equals to the prisoner number, close the loop.
	if (boxValue == prisonerNumber) {
		log("Prisoner " + std::to_string(prisonerNumber) + " found his number at Box " + std::to_string(boxNumber) + ". Total Checks: " + std::to_string(checkCount) + ". Box value:" + std::to_string(boxValue) + "\n");
		checks.push_back(checkCount);
	}
	else {
		// Run the loop until the prioner finds their own number.
		// Increase the number of checks.
		return openBox(simulation, prisonerNumber, boxValue, checkCount + 1);
	}
}

void writeBoxToFile(int simulation, std::string results) {
	std::ofstream ResultFile(base_directory + "/box" + std::to_string(simulation) + ".csv");
	ResultFile << "Simulation\tBox\tValue\n";
	ResultFile << results;
	ResultFile.close();
}

void writeResultsToFile(std::string results) {
	std::ofstream ResultFile("results.csv");
	ResultFile << "Simulation\tSuccess\tMore Than Limit\tLess Than Limit\n";
	ResultFile << results;
	ResultFile.close();
}

void writeChecksToFile() {
	std::ofstream ResultFile("checks.csv");
	ResultFile << "Simulation\tCheck Number\tPrisoner\tBox Number\tBox Value\n";
	ResultFile << checkLogs;
	ResultFile.close();
}

int main()
{

	std::filesystem::create_directory(base_directory);

	/// <summary>
	/// Run simulations
	/// </summary>
	/// <returns></returns>
	for (int simulationNumber = 0; simulationNumber < totalNumberOfSimulations; simulationNumber++)
	{


		// Create boxes and randomize
		createBoxes(simulationNumber);
		

		// Tell each prisoner to open a box until they found their own number in the box.
		for (size_t prisoner = 0; prisoner < totalNumberOfPrisoners; prisoner++)
		{
			// Prisoner will open the first box numbered with their own number.
			// Check count will be 1 for the beginning
			openBox(simulationNumber, prisoner, prisoner, 1);
		}


		int checksMoreThanLimit = 0;
		int checksLessThanLimit = 0;
		size_t sizeOfChecks = checks.size();
		bool prisonerFreedSuccess = true;

		// If a single prisoner passes the limit,
		// Simulation result will be FALSE, means no prisoners to be freed.
		for (size_t i = 0; i < sizeOfChecks; i++)
		{
			int c = checks[i];
			if (c > limitToCheck) {
				checksMoreThanLimit++;
				prisonerFreedSuccess = false;
			}
			else {
				checksLessThanLimit++;
			}
		}

		// Find the percentage of chekcs that reaches to limit and stay under the limit.
		float pMoreThanLimit = 100 * checksMoreThanLimit / static_cast<float>(sizeOfChecks);
		float pLessThanLimit = 100 * checksLessThanLimit / static_cast<float>(sizeOfChecks);

		// Create a struct at the end of each simulation
		SimulationResult s;
		s.lessThanLimitPercentage = pLessThanLimit;
		s.moreThanLimitPercentage = pMoreThanLimit;
		s.success = prisonerFreedSuccess;
		results[simulationNumber] = s;

		// Clear the checks for the new/next simulation.
		checks.clear();

		log(simulationNumber + ". simulation ended. Failed prisoners:%" + std::to_string(pMoreThanLimit) + ", Succeed Prisoners:%" + std::to_string(pLessThanLimit) + "\n");
	}

	writeChecksToFile();

	// Find the success rate of simulations
	int simulationFailed = 0;
	int simulationSucceed = 0;
	std::string simulationResultLines = "";
	for (size_t i = 0; i < results.size(); i++)
	{
		SimulationResult r = results[i];
		if (r.success) {
			simulationSucceed++;
		}
		else {
			simulationFailed++;
		}

		simulationResultLines += std::to_string(i) + "\t" + (r.success ? "true" : "false") + "\t" + std::to_string(r.moreThanLimitPercentage) + "\t" + std::to_string(r.lessThanLimitPercentage) + "\n";
	}


	// Write the results to a file.
	writeResultsToFile(simulationResultLines);


	float simulationFailedPercentage = 100 * static_cast<unsigned long long>(simulationFailed) / results.size();
	float simulationSucceedPercentage = 100 * static_cast<unsigned long long>(simulationSucceed) / results.size();

	std::cout << totalNumberOfSimulations << " has run. simulation ended. Failed simulations:%" << simulationFailedPercentage << ", Succeed simulations:%" << simulationSucceedPercentage;
}
