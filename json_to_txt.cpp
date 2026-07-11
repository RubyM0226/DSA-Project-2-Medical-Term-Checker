#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct DrugRecord {
    std::string genericName;
    std::string brandName;
    std::string manufacturer;
    std::string dosageForm;
    std::string strength;
};

std::string getString(const json& object, const std::string& key) {
    if (object.contains(key) && object[key].is_string()) {
        return object[key].get<std::string>();
    }

    return "";
}

std::string trim(const std::string& text) {
    std::size_t first = text.find_first_not_of(" \t\n\r");

    if (first == std::string::npos) {
        return "";
    }

    std::size_t last = text.find_last_not_of(" \t\n\r");

    return text.substr(first, last - first + 1);
}

std::string cleanField(std::string value) {
    for (char& character : value) {
        if (character == '\n' ||
            character == '\r' ||
            character == '\t') {
            character = ' ';
        } else if (character == '|') {
            character = '/';
        }
    }

    value = trim(value);

    if (value.empty()) {
        return "N/A";
    }

    return value;
}

std::string toLowercase(std::string text) {
    for (char& character : text) {
        character = static_cast<char>(
            std::tolower(
                static_cast<unsigned char>(character)
            )
        );
    }

    return text;
}

std::string getStrength(const json& drugJson) {
    if (!drugJson.contains("active_ingredients") ||
        !drugJson["active_ingredients"].is_array() ||
        drugJson["active_ingredients"].empty()) {
        return "N/A";
    }

    std::vector<std::string> strengths;

    for (const auto& ingredient :
         drugJson["active_ingredients"]) {

        if (!ingredient.is_object()) {
            continue;
        }

        std::string strength =
            cleanField(getString(ingredient, "strength"));

        if (strength != "N/A") {
            strengths.push_back(strength);
        }
    }

    if (strengths.empty()) {
        return "N/A";
    }

    std::ostringstream combinedStrength;

    for (std::size_t i = 0; i < strengths.size(); ++i) {
        if (i > 0) {
            combinedStrength << "; ";
        }

        combinedStrength << strengths[i];
    }

    return combinedStrength.str();
}

DrugRecord createDrugRecord(const json& drugJson) {
    DrugRecord drug;

    drug.genericName = cleanField(
        getString(drugJson, "generic_name")
    );

    drug.brandName = cleanField(
        getString(drugJson, "brand_name")
    );

    drug.manufacturer = cleanField(
        getString(drugJson, "labeler_name")
    );

    drug.dosageForm = cleanField(
        getString(drugJson, "dosage_form")
    );

    drug.strength = getStrength(drugJson);

    return drug;
}

std::string createDuplicateKey(const DrugRecord& drug) {
    const char separator = '\x1F';

    return toLowercase(drug.genericName) + separator +
           toLowercase(drug.brandName) + separator +
           toLowercase(drug.manufacturer) + separator +
           toLowercase(drug.dosageForm) + separator +
           toLowercase(drug.strength);
}

bool sortByGenericName(const DrugRecord& first,
                       const DrugRecord& second) {
    std::string firstGeneric =
        toLowercase(first.genericName);

    std::string secondGeneric =
        toLowercase(second.genericName);

    if (firstGeneric != secondGeneric) {
        return firstGeneric < secondGeneric;
    }

    std::string firstBrand =
        toLowercase(first.brandName);

    std::string secondBrand =
        toLowercase(second.brandName);

    if (firstBrand != secondBrand) {
        return firstBrand < secondBrand;
    }

    std::string firstManufacturer =
        toLowercase(first.manufacturer);

    std::string secondManufacturer =
        toLowercase(second.manufacturer);

    if (firstManufacturer != secondManufacturer) {
        return firstManufacturer < secondManufacturer;
    }

    std::string firstDosageForm =
        toLowercase(first.dosageForm);

    std::string secondDosageForm =
        toLowercase(second.dosageForm);

    if (firstDosageForm != secondDosageForm) {
        return firstDosageForm < secondDosageForm;
    }

    return toLowercase(first.strength) <
           toLowercase(second.strength);
}

void writeDrug(std::ofstream& output,
               const DrugRecord& drug) {
    output << drug.genericName << " | "
           << drug.brandName << " | "
           << drug.manufacturer << " | "
           << drug.dosageForm << " | "
           << drug.strength << '\n';
}

int main() {
    const std::string inputFile = "../drug-ndc-0001-of-0001.json";
    const std::string outputFile = "term.txt";

    std::ifstream input(inputFile);

    if (!input.is_open()) {
        std::cerr << "Could not open "
                  << inputFile << '\n';
        return 1;
    }

    json root;

    try {
        input >> root;
    } catch (const json::exception& error) {
        std::cerr << "JSON error: "
                  << error.what() << '\n';
        return 1;
    }

    input.close();

    if (!root.contains("results") ||
        !root["results"].is_array()) {
        std::cerr << "The JSON does not contain "
                     "a results array.\n";
        return 1;
    }

    std::vector<DrugRecord> drugs;
    std::unordered_set<std::string> duplicateKeys;

    std::size_t totalRecords = 0;
    std::size_t duplicatesRemoved = 0;
    std::size_t missingStrengthCount = 0;

    for (const auto& drugJson : root["results"]) {
        if (!drugJson.is_object()) {
            continue;
        }

        ++totalRecords;

        DrugRecord drug = createDrugRecord(drugJson);

        if (drug.strength == "N/A") {
            ++missingStrengthCount;
        }

        std::string duplicateKey =
            createDuplicateKey(drug);

        if (duplicateKeys.find(duplicateKey) !=
            duplicateKeys.end()) {
            ++duplicatesRemoved;
            continue;
        }

        duplicateKeys.insert(duplicateKey);
        drugs.push_back(drug);
    }

    std::sort(
        drugs.begin(),
        drugs.end(),
        sortByGenericName
    );

    std::ofstream output(outputFile);

    if (!output.is_open()) {
        std::cerr << "Could not create "
                  << outputFile << '\n';
        return 1;
    }

    output << "generic name | brand name | "
              "manufacturer | dosage form | strength\n";

    for (const DrugRecord& drug : drugs) {
        writeDrug(output, drug);
    }

    output.close();

    std::cout << "Created " << outputFile << '\n';
    std::cout << "Original records: "
              << totalRecords << '\n';
    std::cout << "Unique records: "
              << drugs.size() << '\n';
    std::cout << "Duplicates removed: "
              << duplicatesRemoved << '\n';
    std::cout << "Records with missing strength: "
              << missingStrengthCount << '\n';

    return 0;
}
