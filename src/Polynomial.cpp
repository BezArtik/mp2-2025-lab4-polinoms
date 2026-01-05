#include "Polynomial.h"
#include <limits>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

bool Polynomial::VariableCompare::operator()(const Variable& v1, const Variable& v2) const noexcept {
    return v1.name_ < v2.name_;
}

bool Polynomial::MonomCompare::operator()(const Monom& m1, const Monom& m2) const noexcept {
    int m1_deg = m1.total_deg();
    int m2_deg = m2.total_deg();
    if (m1_deg != m2_deg) {
        return m1_deg > m2_deg;
    }

    auto iter_m1 = m1.variables().begin();
    auto iter_m2 = m2.variables().begin();

    while (iter_m1 != m1.variables().end() && iter_m2 != m2.variables().end()) {
        if (iter_m1->name_ != iter_m2->name_) {
            return iter_m1->name_ < iter_m2->name_;
        }
        if (iter_m1->power_ != iter_m2->power_) {
            return iter_m1->power_ < iter_m2->power_;
        }
        ++iter_m1;
        ++iter_m2;
    }
    return m1.coefficient() < m2.coefficient();
}

bool Polynomial::VariableValueCompare::operator()(const VariableValue& a, const VariableValue& b) const noexcept {
    return a.name_ < b.name_;
}

void Polynomial::Monom::add_variable(const Variable& var) {
    if (var.power_ != 0) {
        variables_.insert(var);
    }
}

Polynomial::Monom::Monom(double coeff, std::initializer_list<Variable> vars)
    : coefficient_(coeff) {
    for (const auto& var : vars) {
        add_variable(var);
    }
}

Polynomial::Monom::Monom(const std::string& str) : coefficient_(0.0) {
    if (str.empty()) {
        throw std::invalid_argument("Empty string");
    }

    std::string s = str;
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());

    size_t i = 0;
    bool has_explicit_coefficient = false;
    bool negative = false;

    if (s[i] == '+' || s[i] == '-') {
        negative = (s[i] == '-');
        ++i;
    }

    if (i < s.size() && (std::isdigit(s[i]) || s[i] == '.')) {
        size_t start = i;
        while (i < s.size() && std::isdigit(s[i])) {
            ++i;
        }

        if (i < s.size() && s[i] == '.') {
            ++i;
            while (i < s.size() && std::isdigit(s[i])) {
                ++i;
            }
        }

        coefficient_ = std::stod(s.substr(start, i - start));
        has_explicit_coefficient = true;
    }
    else {
        coefficient_ = 1.0;
    }

    if (negative) {
        coefficient_ = -coefficient_;
    }

    while (i < s.size()) {
        if (s[i] == '*' || std::isspace(s[i])) {
            ++i;
            continue;
        }

        if (!std::isalpha(s[i])) {
            throw std::invalid_argument("Invalid variable name");
        }

        char var_name = s[i];
        ++i;
        int power = 1;

        if (i < s.size() && s[i] == '^') {
            ++i;
            if (i >= s.size() || !std::isdigit(s[i])) {
                throw std::invalid_argument("Invalid power after '^'");
            }

            size_t power_start = i;
            while (i < s.size() && std::isdigit(s[i])) {
                ++i;
            }
            power = std::stoi(s.substr(power_start, i - power_start));
        }

        variables_.insert({ var_name, power });
    }

    if (is_zero()) {
        variables_.clear();
    }

    if (!has_explicit_coefficient && variables_.is_empty()) {
        throw std::invalid_argument("Invalid monom format");
    }
}

int Polynomial::Monom::total_deg() const {
    int deg = 0;
    for (const auto& var : variables_) {
        deg += var.power_;
    }
    return deg;
}

bool Polynomial::Monom::is_similar(const Monom& other) const {
    if (variables_.size() != other.variables_.size()) {
        return false;
    }

    auto iter1 = variables_.begin();
    auto iter2 = other.variables_.begin();

    while (iter1 != variables_.end()) {
        if (iter1->name_ != iter2->name_ || iter1->power_ != iter2->power_) {
            return false;
        }
        ++iter1;
        ++iter2;
    }
    return true;
}

bool Polynomial::Monom::is_zero() const noexcept {
    return coefficient_ == 0.0;
}

Polynomial::Monom& Polynomial::Monom::operator*=(double scalar) {
    coefficient_ *= scalar;
    return *this;
}

Polynomial::Monom& Polynomial::Monom::operator*=(const Monom& other) {
    coefficient_ *= other.coefficient();

    if (variables_.is_empty() && other.variables_.is_empty()) {
        return *this;
    }

    SortedList<Variable, VariableCompare> res;
    auto iter1 = variables_.cbegin();
    auto iter2 = other.variables_.cbegin();

    while (iter1 != variables_.cend() && iter2 != other.variables_.cend()) {
        if (iter1->name_ < iter2->name_) {
            res.insert(*iter1);
            ++iter1;
        }
        else if (iter1->name_ > iter2->name_) {
            res.insert(*iter2);
            ++iter2;
        }
        else {
            int new_pow = iter1->power_ + iter2->power_;
            if (new_pow != 0) {
                res.insert({ iter1->name_, new_pow });
            }
            ++iter1;
            ++iter2;
        }
    }

    while (iter1 != variables_.cend()) {
        res.insert(*iter1);
        ++iter1;
    }

    while (iter2 != other.variables_.cend()) {
        res.insert(*iter2);
        ++iter2;
    }

    variables_ = std::move(res);
    return *this;
}

Polynomial::Monom operator*(const Polynomial::Monom& other, double scalar) {
    Polynomial::Monom res = other;
    res.set_coefficient(res.coefficient() * scalar);
    return res;
}

Polynomial::Monom operator*(double scalar, const Polynomial::Monom& other) {
    return other * scalar;
}

Polynomial::Monom operator*(const Polynomial::Monom& lhs, const Polynomial::Monom& rhs) {
    Polynomial::Monom res = lhs;
    res *= rhs;
    return res;
}

bool Polynomial::Monom::operator==(const Monom& rhs) const {
    if (std::abs(coefficient_ - rhs.coefficient_) > std::numeric_limits<double>::epsilon()) {
        return false;
    }
    return is_similar(rhs);
}

bool Polynomial::Monom::operator!=(const Monom& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& ostr, const Polynomial::Monom& m) {
    if (m.coefficient() == 0.0) {
        ostr << "0";
        return ostr;
    }

    if (m.coefficient() == -1.0 && !m.variables().is_empty()) {
        ostr << "-";
    }
    else if (m.variables().is_empty() || m.coefficient() != 1.0) {
        ostr << m.coefficient();
    }

    for (const auto& var : m.variables()) {
        ostr << var.name_;
        if (var.power_ != 1) {
            ostr << "^" << var.power_;
        }
    }
    return ostr;
}

void Polynomial::normalize() {
    if (polynomial_.is_empty()) return;

    SortedList<Monom, MonomCompare> norm_polynomial;
    auto it = polynomial_.cbegin();

    Monom curr = *it;
    ++it;

    while (it != polynomial_.cend()) {
        if (curr.is_similar(*it)) {
            curr.set_coefficient(curr.coefficient() + it->coefficient());
        }
        else {
            if (!curr.is_zero()) {
                norm_polynomial.insert(curr);
            }
            curr = *it;
        }
        ++it;
    }

    if (!curr.is_zero()) {
        norm_polynomial.insert(curr);
    }

    polynomial_ = std::move(norm_polynomial);
}

Polynomial::Polynomial(const Polynomial::Monom& monom) {
    if (!monom.is_zero()) {
        polynomial_.insert(monom);
    }
}

Polynomial::Polynomial(std::initializer_list<Polynomial::Monom> init) {
    for (const auto& monom : init) {
        if (!monom.is_zero()) {
            polynomial_.insert(monom);
        }
    }
    normalize();
}

Polynomial::Polynomial(const std::string& str) {
    std::string s = str;
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());

    size_t curr_pos = 0;
    while (curr_pos < s.length()) {
        size_t next_pos = curr_pos + 1;
        while (next_pos < s.length() && s[next_pos] != '+' && s[next_pos] != '-') {
            ++next_pos;
        }

        std::string monom_str = s.substr(curr_pos, next_pos - curr_pos);
        if (!monom_str.empty()) {
            polynomial_.insert(Monom(monom_str));
        }

        curr_pos = next_pos;
    }

    normalize();
}

Polynomial& Polynomial::operator+=(const Monom& rhs) {
    if (rhs.is_zero()) return *this;
    polynomial_.insert(rhs);
    normalize();
    return *this;
}

Polynomial& Polynomial::operator-=(const Monom& rhs) {
    if (rhs.is_zero()) return *this;

    Monom negative_monom = rhs;
    negative_monom.set_coefficient(-negative_monom.coefficient());
    polynomial_.insert(negative_monom);

    normalize();
    return *this;
}

Polynomial& Polynomial::operator+=(const Polynomial& rhs) {
    if (rhs.is_zero()) return *this;
    if (is_zero()) {
        *this = rhs;
        return *this;
    }

    SortedList<Monom, MonomCompare> result;
    auto it1 = polynomial_.cbegin();
    auto it2 = rhs.polynomial_.cbegin();

    while (it1 != polynomial_.cend() && it2 != rhs.polynomial_.cend()) {
        if (MonomCompare{}(*it1, *it2)) {
            result.insert(*it1);
            ++it1;
        }
        else if (MonomCompare{}(*it2, *it1)) {
            result.insert(*it2);
            ++it2;
        }
        else {
            Monom curr = *it1;
            curr.set_coefficient(curr.coefficient() + it2->coefficient());
            if (!curr.is_zero()) {
                result.insert(curr);
            }
            ++it1;
            ++it2;
        }
    }

    while (it1 != polynomial_.cend()) {
        result.insert(*it1);
        ++it1;
    }

    while (it2 != rhs.polynomial_.cend()) {
        result.insert(*it2);
        ++it2;
    }

    polynomial_ = std::move(result);
    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial& rhs) {
    if (rhs.is_zero()) return *this;

    for (const auto& monom : rhs.polynomial_) {
        Monom tmp = monom;
        tmp.set_coefficient(-tmp.coefficient());
        polynomial_.insert(tmp);
    }

    normalize();
    return *this;
}

Polynomial& Polynomial::operator*=(double scalar) {
    if (std::abs(scalar) < std::numeric_limits<double>::epsilon()) {
        polynomial_.clear();
        return *this;
    }

    if (scalar == 1.0) {
        return *this;
    }

    SortedList<Monom, MonomCompare> res;
    for (const auto& monom : polynomial_) {
        Monom curr = monom;
        curr.set_coefficient(curr.coefficient() * scalar);
        res.insert(curr);
    }

    polynomial_ = std::move(res);
    return *this;
}

Polynomial& Polynomial::operator*=(const Monom& rhs) {
    if (rhs.is_zero()) {
        polynomial_.clear();
        return *this;
    }

    SortedList<Monom, MonomCompare> res;
    for (const auto& monom : polynomial_) {
        Monom curr = monom;
        curr *= rhs;
        res.insert(curr);
    }

    polynomial_ = std::move(res);
    normalize();
    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial& rhs) {
    if (rhs.is_zero()) {
        polynomial_.clear();
        return *this;
    }

    SortedList<Monom, MonomCompare> res_poly;
    for (const auto& monom1 : polynomial_) {
        for (const auto& monom2 : rhs.polynomial_) {
            Monom res_monom = monom1;
            res_monom *= monom2;
            res_poly.insert(res_monom);
        }
    }

    polynomial_ = std::move(res_poly);
    normalize();
    return *this;
}

Polynomial operator+(Polynomial lhs, const Polynomial& rhs) {
    return lhs += rhs;
}

Polynomial operator-(Polynomial lhs, const Polynomial& rhs) {
    return lhs -= rhs;
}

Polynomial operator*(Polynomial lhs, double scalar) {
    return lhs *= scalar;
}

Polynomial operator*(double scalar, const Polynomial& rhs) {
    return rhs * scalar;
}

Polynomial operator*(Polynomial lhs, const Polynomial::Monom& rhs) {
    return lhs *= rhs;
}

Polynomial operator*(const Polynomial::Monom& lhs, const Polynomial& rhs) {
    return rhs * lhs;
}

Polynomial operator*(Polynomial lhs, const Polynomial& rhs) {
    return lhs *= rhs;
}

bool Polynomial::operator==(const Polynomial& other) const {
    Polynomial this_norm(*this);
    Polynomial other_norm(other);
    this_norm.normalize();
    other_norm.normalize();

    if (this_norm.term_count() != other_norm.term_count()) {
        return false;
    }

    auto it1 = this_norm.polynomial_.cbegin();
    auto it2 = other_norm.polynomial_.cbegin();

    while (it1 != this_norm.polynomial_.cend() && it2 != other_norm.polynomial_.cend()) {
        if (*it1 != *it2) {
            return false;
        }
        ++it1;
        ++it2;
    }

    return true;
}

bool Polynomial::operator!=(const Polynomial& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& ostr, const Polynomial& p) {
    if (p.is_zero()) {
        ostr << "0";
        return ostr;
    }

    bool first = true;
    for (const auto& monom : p.polynomial_) {
        if (first) {
            ostr << monom;
            first = false;
            continue;
        }

        if (monom.coefficient() >= 0.0) {
            ostr << " + " << monom;
        }
        else {
            Polynomial::Monom tmp = monom;
            tmp.set_coefficient(-tmp.coefficient());
            ostr << " - " << tmp;
        }
    }

    return ostr;
}

bool Polynomial::is_zero() const noexcept {
    return polynomial_.is_empty();
}

int Polynomial::deg() const {
    int max_degree = 0;
    for (const auto& monom : polynomial_) {
        max_degree = std::max(max_degree, monom.total_deg());
    }
    return max_degree;
}

size_t Polynomial::term_count() const {
    return polynomial_.size();
}

std::string Polynomial::to_string() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}

SortedList<char> Polynomial::get_variables() const {
    SortedList<char> res;
    for (const auto& monom : polynomial_) {
        for (const auto& var : monom.variables()) {
            res.insert_unique(var.name_);
        }
    }
    return res;
}

double Polynomial::calculate(const SortedList<VariableValue, VariableValueCompare>& values) const {
    if (is_zero()) return 0.0;

    double result = 0.0;
    for (const auto& monom : polynomial_) {
        double term_value = monom.coefficient();
        bool term_valid = true;

        for (const auto& var : monom.variables()) {
            VariableValue key{ var.name_, 0.0 };
            auto it = values.find(key);

            if (it != values.cend()) {
                term_value *= std::pow(it->value_, var.power_);
            }
            else {
                term_valid = false;
                break;
            }
        }

        if (term_valid) {
            result += term_value;
        }
    }

    return result;
}