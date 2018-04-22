#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

//check, if parts are overlapping
inline bool isOverlapping(std::vector<int>::iterator first, std::vector<int>::iterator second, int partSize) {
	return *(first + partSize - 1) > *second;
}

//Print vector through iterators
void printVector(std::vector<int>::iterator start, std::vector<int>::iterator end) {
	std::copy(start, end, std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n";
}

//Sorting two parts together (returns two divided parts)
inline std::pair<std::vector<int>, std::vector<int>> sortPair(std::vector<int>* first, std::vector<int>* second) {
	const size_t firstSize = (*first).size();
	(*first).insert((*first).end(), (*second).begin(), (*second).end());
	std::stable_sort((*first).begin(), (*first).end());
	return std::make_pair(std::vector<int>((*first).begin(), (*first).begin() + firstSize),
		std::vector<int>((*first).begin() + firstSize, (*first).end()));
}

//Print first and last element of each part
void printTable(std::vector<std::vector<int>::iterator>& delimeters) {
	std::cout << "TABLE:\n";
	for (std::vector<std::vector<int>::iterator>::iterator it = delimeters.begin(); it != --delimeters.end(); ++it) {
		std::cout << **(it) << " " << *(*(it + 1) - 1) << "\n";
	}
	std::cout << "\n";
}

inline void initializeAllowedNumbers(std::vector<int>& init, int size) {
	for (int i = 0; i < size; ++i) {
		init.push_back(i);
	}
}

int main() {

	srand(time(NULL));

	//Delimeters divide vector into parts
	std::vector<std::vector<int>::iterator> delimeters;

	//Data set to sort
	std::vector<int> numbers;

	std::vector<int> allowedNumbers;

	//Size of data set
	int N = 0;

	//Size of one part
	int size = 0;

	//Number of iterations
	int numberOfIterations = 0;

	//Input of data set size and part size
	std::cout << "Vector size: ";
	std::cin >> N;
	std::cout << "Part size: ";
	std::cin >> size;

	//Initialize data set
	for (size_t j = 0; j < N; ++j) {
		numbers.push_back(rand() % 100);
	}

	if (numbers.size() <= 100) {
		std::cout << "\nVECTOR:\n";
		printVector(numbers.begin(), numbers.end());
		std::cout << "\n";
	}

	//Initialize parts delimeters
	for (size_t j = 0; j < N; ++j) {
		if (j % size == 0) {
			delimeters.push_back(numbers.begin() + j);
		}
	}
	initializeAllowedNumbers(allowedNumbers, delimeters.size());
	delimeters.push_back(numbers.end());

	std::vector<int> copyOfAllowedNumbers = allowedNumbers;

	std::vector<int> one;
	std::vector<int> two;

	std::pair<std::vector<int>, std::vector<int>> result;

	std::vector<std::pair<int, int>> pairsToSort;

	double start = omp_get_wtime();

	//Sort of each part
#pragma omp parallel for
		for (size_t i = 0; i < delimeters.size() - 1; ++i) {
			std::stable_sort(delimeters[i], delimeters[i + 1]);
		}

	for (;;) {
		for (size_t i = 0; i < delimeters.size() - 2; ++i) {
			if (allowedNumbers.size() == 1) {
				break;
			}
			if (!std::binary_search(allowedNumbers.begin(), allowedNumbers.end(), i)) {
				continue;
			}
			for (size_t j = i + 1; j < delimeters.size() - 1; ++j) {
				if (!std::binary_search(allowedNumbers.begin(), allowedNumbers.end(), j)) {
					continue;
				}
					if (isOverlapping(delimeters[i], delimeters[j], size)) {
						pairsToSort.push_back(std::make_pair(i, j));
						allowedNumbers.erase(std::find(allowedNumbers.begin(), allowedNumbers.end(), j));
						allowedNumbers.erase(std::find(allowedNumbers.begin(), allowedNumbers.end(), i));
					}
					break;
			}
		}

#pragma omp parallel for private(result, one, two) shared (pairsToSort,delimeters,start)
		for (size_t i = 0; i < pairsToSort.size(); ++i) {
			one.insert(one.end(),delimeters[pairsToSort[i].first], delimeters[pairsToSort[i].first + 1]);
			two.insert(two.end(), delimeters[pairsToSort[i].second], delimeters[pairsToSort[i].second + 1]);

			result = sortPair(&one,&two);

			std::copy(result.first.begin(), result.first.end(), delimeters[pairsToSort[i].first]);
			std::copy(result.second.begin(), result.second.end(), delimeters[pairsToSort[i].second]);

			one.clear();
			two.clear();
		}

		if (allowedNumbers.size() == copyOfAllowedNumbers.size()) {
			break;
		}
		allowedNumbers = copyOfAllowedNumbers;
		pairsToSort.clear();
		++numberOfIterations;
	}

	double end = omp_get_wtime();

	//Print result
	if (numbers.size() <= 100) {
		printVector(numbers.begin(), numbers.end());
	}

	std::cout << "\n";

	//Print time of sort and number of iterations
	std::cout << "\nWork took " << end - start << " seconds\n";
	std::cout << "Number of iterations: " << numberOfIterations << "\n";

	return 0;
}
