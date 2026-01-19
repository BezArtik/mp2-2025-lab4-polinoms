#include "Polynomial.h"
#include <limits>
#include <string>
#include <iostream>
#include <cctype>
#include <cmath>
#include <initializer_list>

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

void Polynomial::Monom::add_variable(const Variable& var) {
    if (var.power_ == 0) return;

    Variable new_var{ var.name_,var.power_ };

    Variable key{ var.name_,0 };
    auto it = variables_.find(key);

    if (it != variables_.end()) {
        Variable updated = *it;
        updated.power_ += var.power_;

        variables_.erase(it);
        if (updated.power_ != 0) {
            variables_.insert(updated);
        }
    }
    else {
        variables_.insert(new_var);
    }
}

Polynomial::Monom::Monom(double coeff, const std::initializer_list<Variable>& vars)
    : coefficient_(coeff) {
    for (const auto& var : vars) {
		if (var.power_ < 0) {
            throw std::invalid_argument("Negative power in variable");
        }
        add_variable(var);
    }
}

Polynomial::Monom::Monom(const std::string& str) 
    : coefficient_(1.0) {
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
        if (s[i] == '*') {
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

        add_variable({ var_name, power });
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
            res.insert_back(*iter1);
            ++iter1;
        }
        else if (iter1->name_ > iter2->name_) {
            res.insert_back(*iter2);
            ++iter2;
        }
        else {
            int new_pow = iter1->power_ + iter2->power_;
            if (new_pow != 0) {
                res.insert_back({ iter1->name_, new_pow });
            }
            ++iter1;
            ++iter2;
        }
    }

    while (iter1 != variables_.cend()) {
        res.insert_back(*iter1);
        ++iter1;
    }

    while (iter2 != other.variables_.cend()) {
        res.insert_back(*iter2);
        ++iter2;
    }
    variables_ = std::move(res);
    return *this;
}

Polynomial::Monom operator*(Polynomial::Monom other, double scalar) {
    return other *= scalar;
}

Polynomial::Monom operator*(double scalar, const Polynomial::Monom& other) {
    return other * scalar;
}

Polynomial::Monom operator*(Polynomial::Monom lhs, const Polynomial::Monom& rhs) {
    return lhs *= rhs;
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
    if (m.is_zero()) {
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

void Polynomial::combine_like_terms() {
    if (polynomial_.is_empty()) return;

    auto it = polynomial_.begin();
    auto next = it;
    ++next;

    while (next != polynomial_.end()) {
        if (it->is_similar(*next)) {
            it->set_coefficient(it->coefficient() + next->coefficient());
			next = polynomial_.erase(next);

            if(it->is_zero()) {
                it = polynomial_.erase(it);
				if (it == polynomial_.end()) {
                    break;
                }
				next = it;
				++next;
            }
        }
        else {
            ++it;
            ++next;
		}

    }
}

void Polynomial::normalize() {
    if (polynomial_.is_empty()) return;
	if (!polynomial_.is_sorted()) {
        polynomial_.sort();
    }
	combine_like_terms();
}

Polynomial::Polynomial(const Polynomial::Monom& monom) {
    if (!monom.is_zero()) {
        polynomial_.insert_back(monom);
    }
}

Polynomial::Polynomial(const std::initializer_list<Polynomial::Monom>& init) {
    for (const auto& monom : init) {
        if (!monom.is_zero()) {
            polynomial_.insert_back(monom);
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
            polynomial_.insert_back(Monom(monom_str));
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
    return *this += (-1.0) * rhs;
}

Polynomial& Polynomial::operator+=(const Polynomial& rhs) {
    if (rhs.is_zero()) return *this;
    if (is_zero()) {
        *this = rhs;
        return *this;
    }

	SortedList<Monom, MonomCompare> temp = rhs.polynomial_;
	polynomial_.merge_sorted(std::move(temp));
	normalize();

    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial& rhs) {
    if (rhs.is_zero()) return *this;
    if (is_zero()) {
        *this = rhs;
        return *this;
    }

	SortedList<Monom, MonomCompare> temp;
	for (const auto& monom : rhs.polynomial_) {
        Monom neg_monom = monom;
        neg_monom *= -1.0;
        temp.insert_back(neg_monom);
    }
	polynomial_.merge_sorted(std::move(temp));
	normalize();

    return *this;
}

Polynomial& Polynomial::operator*=(double scalar) {
    if (std::abs(scalar) < std::numeric_limits<double>::epsilon()) {
        polynomial_.clear();
        return *this;
    }

    if (std::abs(scalar - 1.0) < std::numeric_limits<double>::epsilon()) {
        return *this;
    }

    for (auto& monom : polynomial_) {
        monom *= scalar;
    }

    normalize();
    return *this;
}

Polynomial& Polynomial::operator*=(const Monom& rhs) {
    if (rhs.is_zero()) {
        polynomial_.clear();
        return *this;
    }

    for (auto& monom : polynomial_) {
        monom *= rhs;
    }

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
            res_poly.insert_back(res_monom);
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
    if (term_count() != other.term_count()) {
        return false;
    }

    auto it1 = polynomial_.begin();
    auto it2 = other.polynomial_.begin();

    while (it1 != polynomial_.end() && it2 != other.polynomial_.end()) {
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

int Polynomial::deg() const {
    if (polynomial_.is_empty()) return 0;
    return polynomial_.front().total_deg();
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

            if (it != values.end()) {
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