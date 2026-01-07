#pragma once
#include "SortedList.h"

class Polynomial {
public:
    struct Variable {
        char name_;
        int power_;
    };
    struct VariableCompare {
        bool operator()(const Variable& v1, const Variable& v2) const noexcept;
    };
    struct VariableValue {
        char name_;
        double value_;
    };
    struct VariableValueCompare {
        bool operator()(const VariableValue& a, const VariableValue& b) const noexcept;
    };

    class Monom {
    private:
        double coefficient_;
        SortedList<Variable, VariableCompare> variables_;

        void normalize();

    public:
        Monom(double coeff = 1.0, std::initializer_list<Variable> vars = {});
        Monom(const std::string& str);

        Monom& operator*=(double scalar);
        Monom& operator*=(const Monom& other);

        friend Monom operator*(double scalar, const Monom& other);
        friend Monom operator*(const Monom& other, double scalar);
        friend Monom operator*(const Monom& lhs, const Monom& rhs);

        bool operator==(const Monom& other) const;
        bool operator!=(const Monom& other) const;

        friend std::ostream& operator<<(std::ostream& ostr, const Monom& m);

        double coefficient() const { return coefficient_; }
        const SortedList<Variable, VariableCompare>& variables() const { return variables_; }

        void set_coefficient(double coeff) { coefficient_ = coeff; }
        void add_variable(const Variable& var);

        int total_deg() const;
        bool is_similar(const Monom& other) const;
        bool is_zero() const noexcept { return coefficient_ == 0.0; };
    };

    struct MonomCompare {
        bool operator()(const Monom& m1, const Monom& m2) const noexcept;
    };

private:
    SortedList<Monom, MonomCompare> polynomial_;
    void normalize();

public:
    Polynomial() = default;
    Polynomial(const Monom& monom);
    Polynomial(std::initializer_list<Monom> init);
    Polynomial(const std::string& str);

    Polynomial& operator+=(const Monom& rhs);
    Polynomial& operator-=(const Monom& rhs);
    Polynomial& operator+=(const Polynomial& rhs);
    Polynomial& operator-=(const Polynomial& rhs);
    Polynomial& operator*=(double scalar);
    Polynomial& operator*=(const Monom& rhs);
    Polynomial& operator*=(const Polynomial& rhs);

    friend Polynomial operator+(Polynomial lhs, const Polynomial& rhs);
    friend Polynomial operator-(Polynomial lhs, const Polynomial& rhs);
    friend Polynomial operator*(Polynomial lhs, double scalar);
    friend Polynomial operator*(double scalar, const Polynomial& rhs);
    friend Polynomial operator*(Polynomial lhs, const Monom& rhs);
    friend Polynomial operator*(const Monom& lhs, const Polynomial& rhs);
    friend Polynomial operator*(Polynomial lhs, const Polynomial& rhs);

    bool operator==(const Polynomial& other) const;
    bool operator!=(const Polynomial& other) const;

    friend std::ostream& operator<<(std::ostream& ostr, const Polynomial& p);

    auto begin() { return polynomial_.begin(); }
    auto end() { return polynomial_.end(); }
    auto begin() const { return polynomial_.cbegin(); }
    auto end() const { return polynomial_.cend(); }
    auto cbegin() const { return polynomial_.cbegin(); }
    auto cend() const { return polynomial_.cend(); }

    bool is_zero() const noexcept { return polynomial_.is_empty(); };
    size_t term_count() const { return polynomial_.size(); };
    int deg() const;
    SortedList<char> get_variables() const;
    double calculate(const SortedList<VariableValue, VariableValueCompare>& values) const;
};
