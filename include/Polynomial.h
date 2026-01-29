#pragma once
#include "SortedList.h"

class Polynomial {
public:
    struct Variable {
        char name_;
        int power_;
    };
    struct VariableCompare {
        bool operator()(const Variable& v1, const Variable& v2) const noexcept { return v1.name_ < v2.name_; }
    };
    struct VariableValue {
        char name_;
        double value_;
    };
    struct VariableValueCompare {
        bool operator()(const VariableValue& a, const VariableValue& b) const noexcept { return a.name_ < b.name_; }
    };

    class Monom {
    private:
        double coefficient_;
        SortedList<Variable, VariableCompare> variables_;

    public:
        Monom(double coeff = 1.0, const std::initializer_list<Variable>& vars = {});
        Monom(const std::string& str);

        Monom& operator*=(double scalar) noexcept;
        Monom& operator*=(const Monom& other);

        friend Monom operator*(double scalar, const Monom& other);
        friend Monom operator*(Monom other, double scalar);
        friend Monom operator*(Monom lhs, const Monom& rhs);

        bool operator==(const Monom& other) const noexcept;
        bool operator!=(const Monom& other) const noexcept;

        friend std::ostream& operator<<(std::ostream& ostr, const Monom& m);

        double coefficient() const noexcept { return coefficient_; }
        const SortedList<Variable, VariableCompare>& variables() const noexcept{ return variables_; }

        void set_coefficient(double coeff) noexcept { coefficient_ = coeff; }
        void add_variable(const Variable& var);

        int total_deg() const noexcept;
        bool is_similar(const Monom& other) const noexcept;
        bool is_zero() const noexcept { return std::abs(coefficient_) < std::numeric_limits<double>::epsilon(); };
    };

    struct MonomCompare {
        bool operator()(const Monom& m1, const Monom& m2) const noexcept;
    };

private:
    SortedList<Monom, MonomCompare> polynomial_;
    void combine_like_terms() noexcept;
    void normalize() noexcept;

public:
    Polynomial() = default;
    Polynomial(const Monom& monom);
    Polynomial(const std::initializer_list<Monom>& init);
    Polynomial(const std::string& str);

    Polynomial& operator+=(const Monom& rhs);
    Polynomial& operator-=(const Monom& rhs);
    Polynomial& operator+=(const Polynomial& rhs);
    Polynomial& operator-=(const Polynomial& rhs);
    Polynomial& operator*=(double scalar) noexcept;
    Polynomial& operator*=(const Monom& rhs) noexcept;
    Polynomial& operator*=(const Polynomial& rhs);

    friend Polynomial operator+(Polynomial lhs, const Polynomial& rhs);
    friend Polynomial operator-(Polynomial lhs, const Polynomial& rhs);
    friend Polynomial operator*(Polynomial lhs, double scalar) noexcept;
    friend Polynomial operator*(double scalar, const Polynomial& rhs) noexcept;
    friend Polynomial operator*(Polynomial lhs, const Monom& rhs);
    friend Polynomial operator*(const Monom& lhs, const Polynomial& rhs);
    friend Polynomial operator*(Polynomial lhs, const Polynomial& rhs);

    bool operator==(const Polynomial& other) const noexcept;
    bool operator!=(const Polynomial& other) const noexcept;

    friend std::ostream& operator<<(std::ostream& ostr, const Polynomial& p);

    auto begin()        noexcept { return polynomial_.begin(); }
    auto end()          noexcept { return polynomial_.end(); }
    auto begin()  const noexcept { return polynomial_.cbegin(); }
    auto end()    const noexcept { return polynomial_.cend(); }
    auto cbegin() const noexcept { return polynomial_.cbegin(); }
    auto cend()   const noexcept { return polynomial_.cend(); }

    bool is_zero() const noexcept { return polynomial_.is_empty(); };
    size_t term_count() const noexcept { return polynomial_.size(); };
    int deg() const noexcept;
    SortedList<char> get_variables() const;
    double calculate(const SortedList<VariableValue, VariableValueCompare>& values) const noexcept;
};
