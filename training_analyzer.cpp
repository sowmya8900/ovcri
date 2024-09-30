#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "json.hpp"
#include <algorithm>
#include <tuple>
#include <ctime>

// Using nlohmann::json for handling JSON in C++
using json = nlohmann::json;
using namespace std;

// Struct for a Training
struct Training {
    string name;
    string completion_date;
    string expiration_date;
};

// Struct for a Person
struct Person {
    string name;
    vector<Training> completions;
};

// Task 0: Reads all data from .Json file - Function to load JSON data from the file
vector<Person> load_data(const string &filename) {
    ifstream file(filename);
    json j;

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return {};
    }

    try {
        file >> j;
    } catch (const json::exception& e) {
        cerr << "Error: Invalid JSON format in file " << filename << endl;
        return {};
    }

    vector<Person> people;
    for (const auto &entry : j) {
        Person person;
        person.name = entry["name"];
        for (const auto &t : entry["completions"]) {
            string expiration_date = t.contains("expires") && !t["expires"].is_null() ? t["expires"] : "";
            Training training = { t["name"], t["timestamp"], expiration_date };
            person.completions.push_back(training);
        }
        people.push_back(person);
    }
    return people;
}

// Task 1: Count Completed Trainings
map<string, int> count_completed_trainings(const vector<Person>& people) {
    map<string, int> training_count;
    for (const auto& person : people) {
        map<string, Training> recent_trainings;
        // Only count most recent completion for each training
        for (const auto& completion : person.completions) {
            if (recent_trainings.find(completion.name) == recent_trainings.end() ||
                completion.completion_date > recent_trainings[completion.name].completion_date) {
                recent_trainings[completion.name] = completion;
            }
        }
        for (const auto& rt : recent_trainings) {
            training_count[rt.first]++;
        }
    }
    return training_count;
}

// Helper to parse dates in MM/DD/YYYY format and compare them
bool is_in_fiscal_year(const string& date, const string& fiscal_year_start, const string& fiscal_year_end) {
    // Split date strings into month, day, year components
    auto parse_date = [](const string& date) {
        int first_slash = date.find('/');
        int second_slash = date.find('/', first_slash + 1);
        int month = stoi(date.substr(0, first_slash));
        int day = stoi(date.substr(first_slash + 1, second_slash - first_slash - 1));
        int year = stoi(date.substr(second_slash + 1));
        return make_tuple(month, day, year);
    };

    auto [month, day, year] = parse_date(date);
    auto [fy_start_month, fy_start_day, fy_start_year] = parse_date(fiscal_year_start);
    auto [fy_end_month, fy_end_day, fy_end_year] = parse_date(fiscal_year_end);

    // Check if the date is within the fiscal year range
    if (year < fy_start_year || year > fy_end_year) {
        return false;
    }
    if (year == fy_start_year && month < fy_start_month) {
        return false;
    }
    if (year == fy_end_year && month > fy_end_month) {
        return false;
    }
    return true;
}

// Task 2: List of people who completed specific trainings in a fiscal year
map<string, vector<string>> trainings_by_fiscal_year(const vector<Person>& people, const vector<string>& specified_trainings, const string& fiscal_year_start, const string& fiscal_year_end) {
    map<string, vector<string>> result;

    for (const auto& person : people) {
        for (const auto& completion : person.completions) {
            if (find(specified_trainings.begin(), specified_trainings.end(), completion.name) != specified_trainings.end() &&
                is_in_fiscal_year(completion.completion_date, fiscal_year_start, fiscal_year_end)) {
                result[completion.name].push_back(person.name);
            }
        }
    }
    return result;
}

// Helper to add months to a date
string add_month(const string& date, int months) {
    struct tm tm{};
    sscanf(date.c_str(), "%d/%d/%d", &tm.tm_mon, &tm.tm_mday, &tm.tm_year);
    tm.tm_year -= 1900; // tm_year is years since 1900
    tm.tm_mon -= 1;     // tm_mon is zero-based (0 for Jan, 11 for Dec)

    // Add months
    tm.tm_mon += months;

    // Normalize time structure
    mktime(&tm);

    // Return as MM/DD/YYYY string
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y", &tm); // MM/DD/YYYY format
    return string(buffer);
}


// Task 3: List of people with expired or soon-to-expire trainings
map<string, map<string, vector<string>>> find_expired_trainings(const vector<Person>& people, const string& reference_date) {
    map<string, map<string, vector<string>>> result;

    for (const auto& person : people) {
        map<string, Training> recent_trainings;
        for (const auto& completion : person.completions) {
            if (recent_trainings.find(completion.name) == recent_trainings.end() ||
                completion.expiration_date > recent_trainings[completion.name].expiration_date) {
                recent_trainings[completion.name] = completion;
            }
        }

        for (const auto& rt : recent_trainings) {
            string exp_date = rt.second.expiration_date;
            if (!exp_date.empty() && exp_date < reference_date) {
                result[person.name]["expired"].push_back(rt.second.name);
            } else if (!exp_date.empty() && exp_date <= add_month(reference_date, 1)) {
                result[person.name]["expires soon"].push_back(rt.second.name);
            }
        }
    }
    return result;
}

// Helper function to write JSON output to file
void write_json(const string& filename, const json& j) {
    ofstream file(filename);
    file << setw(4) << j << endl;
}

int main() {
    // Task 0: Load data from trainings.txt (JSON format)
    vector<Person> people = load_data("trainings.txt");

    // Task 1: Count Completed Trainings
    auto completed_trainings = count_completed_trainings(people);
    write_json("completed_trainings.json", completed_trainings);

    // Task 2: People who completed specific trainings in Fiscal Year 2024
    vector<string> specified_trainings = { "Electrical Safety for Labs", "X-Ray Safety", "Laboratory Safety Training" };
    string fiscal_year_start = "10/01/2023";
    string fiscal_year_end = "09/30/2024";
    auto fiscal_year_results = trainings_by_fiscal_year(people, specified_trainings, fiscal_year_start, fiscal_year_end);
    write_json("trainings_by_fiscal_year.json", fiscal_year_results);

    // Task 3: Expired or Soon-to-Expire Trainings as of October 1st, 2023
    string reference_date = "10/01/2023";
    auto expired_trainings = find_expired_trainings(people, reference_date);
    write_json("expired_trainings.json", expired_trainings);

    return 0;
}
