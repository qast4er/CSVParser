#pragma once
#include <fstream>
#include <memory>
#include <regex>
#include <string>
#include <tuple>
#include <vector>
#include <exception>
#include "print_tuple.h"
#include "CSVParser.h"


template<class ...Args>
class CSVParser {
public:
    class CSVIterator;

    explicit CSVParser(std::ifstream& fin, size_t skip) : input(fin), offset(skip) {
        if (!fin.is_open()) {
            throw std::exception("It's unable to open the input file.");
        }
        input.clear();
        input.seekg(0, std::ifstream::beg);
        std::string tmp_str;
        for (file_length = 0; getline(input, tmp_str); file_length++);
        input.clear();
        input.seekg(0, std::ifstream::beg);

        if (skip >= file_length) {
            throw std::exception("It's unable to skip more lines then file length.");
        }
        if (skip < 0) {
            throw std::exception("It's unable to skip less then zero lines.");
        }
    }

    CSVParser(const CSVParser<Args...>& src) = delete;

    CSVIterator begin() {
        input.clear();
        input.seekg(0, std::ifstream::beg);

        for (size_t i = 0; i < offset; i++) {
            get_line();
        }

        return CSVIterator(*this, CSVIterator::Mode::begin);
    }

    CSVIterator end() {
        return CSVIterator(*this, CSVIterator::Mode::end);
    }

private:
    std::ifstream& input;
    size_t offset;
    size_t file_length = 0;
    char column_delim = ';';
    char line_delim = '\n';
    char escape_sym = '\"';
    size_t cur_line = 0;

    std::string get_line() {
        std::string line;
        std::getline(input, line, line_delim);

        cur_line++;
        return line;
    };

    std::vector<std::string> parse_line(const std::string& line) {
        // split line in columns with column_delim
        std::regex column_delim_regex({ column_delim });
        std::vector<std::string> values(std::sregex_token_iterator(line.begin(), line.end(), column_delim_regex, -1), std::sregex_token_iterator());

        for (size_t i = 0; i < values.size(); ++i) {
            escape_symbols(values[i], i);
        }

        if (values.size() < sizeof...(Args)) {
            throw std::exception("Less columns in file then in input parameters.");
        }
        if (values.size() > sizeof...(Args)) {
            throw std::exception("More columns in file then in input parameters.");
        }

        return values;
    };

    void escape_symbols(std::string& value, size_t ind) {
        std::regex escape_sym_regex({ escape_sym });

        // split string in substrings with escape_sym
        std::vector<std::string> values(std::sregex_token_iterator(value.begin(), value.end(), escape_sym_regex, -1), std::sregex_token_iterator());

        // merge processed substrings
        value.clear();
        for (size_t i = 0; i < values.size(); i++) {
            if (i % 2) {
                values[i] = std::regex_replace(values[i], std::regex("\a"), "\\a");
                values[i] = std::regex_replace(values[i], std::regex("\n"), "\\n");
                values[i] = std::regex_replace(values[i], std::regex("\r"), "\\r");
                values[i] = std::regex_replace(values[i], std::regex("\t"), "\\t");
                values[i] = std::regex_replace(values[i], std::regex("\v"), "\\v");
            }

            value += values[i];
        }
    };
};

template<class ...Args>
class CSVParser<Args...>::CSVIterator {
public:
    using value_T = std::tuple<Args...>;
    using reference = std::tuple<Args...>&;
    using pointer = std::shared_ptr<std::tuple<Args...>>;
    enum class Mode {
        begin,
        end
    };

    CSVIterator(CSVParser<Args...>& parent, Mode mode) : CSV_parent(parent) {
        switch (mode) {
        case Mode::begin:
            UpdatePointer();
            break;
        case Mode::end:
            CSV_ptr = nullptr;
            break;
        }
    }

    reference operator*() const {
        return *CSV_ptr;
    }

    const CSVIterator operator++() {// prefix
        UpdatePointer();
        return *this;
    }

    const CSVIterator operator++(int) { // postfix
        const CSVIterator rv = *this;
        UpdatePointer();
        return rv;
    }

    bool operator==(const CSVIterator& a) const {
        return a.CSV_ptr == CSV_ptr;
    }

    bool operator!=(const CSVIterator& a) const {
        return a.CSV_ptr != CSV_ptr;
    }

private:
    pointer CSV_ptr;
    CSVParser<Args...>& CSV_parent;

    void UpdatePointer() {
        std::string new_line = CSV_parent.get_line();

        if (new_line.empty()) {
            CSV_ptr = nullptr;
        }
        else {
            try {
                CSV_ptr = std::make_shared<value_T>(make_tuple<Args...>(CSV_parent.parse_line(new_line)));
            }
            catch (size_t ind) {
                throw std::exception("Unable process");
            }
        }
    }
};