#include <gtest.h>
#include "Polynomial.h"

//======================MONOM=====================

TEST(Polynomial, monom_with_coeff_has_no_variables) {
	Polynomial::Monom m(2.5);
	EXPECT_TRUE(m.variables().is_empty());
}

TEST(Polynomial, monom_with_coeff_and_variables_has_coeff_and_variables) {
	Polynomial::Monom m(2.5, { {'x',1},{'y',1} });
	EXPECT_DOUBLE_EQ(m.coefficient(), 2.5);
	auto it = m.variables().cbegin();
	EXPECT_EQ(it->name_, 'x');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'y');
	EXPECT_EQ(it->power_, 1);
}

TEST(Polynomial, monom_from_string_with_coeff_and_variables_has_coeff_and_variables) {
	Polynomial::Monom m("2.5xy");
	EXPECT_DOUBLE_EQ(m.coefficient(), 2.5);
	auto it = m.variables().cbegin();
	EXPECT_EQ(it->name_, 'x');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'y');
	EXPECT_EQ(it->power_, 1);
}

TEST(Polynomial, monom_sorts_variables) {
	Polynomial::Monom m("2.5dcab");
	auto it = m.variables().cbegin();
	EXPECT_EQ(it->name_, 'a');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'b');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'c');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'd');
	EXPECT_EQ(it->power_, 1);
}

TEST(Polynomial, equal_monomials_is_equal) {
	Polynomial::Monom m1("2.5xzy");
	Polynomial::Monom m2("2.5xyz");
	EXPECT_EQ(m1, m2);
}

TEST(Polynomial, non_equal_monomials_is_not_equal) {
	Polynomial::Monom m1("5xzy^2");
	Polynomial::Monom m2("2.5xyz");
	EXPECT_NE(m1, m2);
}

TEST(Polynomial, setting_the_coeff_sets_the_coeff) {
	Polynomial::Monom m;
	m.set_coefficient(2.5);
	EXPECT_DOUBLE_EQ(m.coefficient(), 2.5);
}

TEST(Polynomial, adding_the_var_adds_the_var) {
	Polynomial::Monom m;
	m.add_variable({ 'x',2 });
	EXPECT_EQ(m.variables().size(), 1);
	EXPECT_EQ(m.total_deg(), 2);
	auto it = m.variables().begin();
	EXPECT_EQ(it->name_, 'x');
	EXPECT_EQ(it->power_, 2);
}

TEST(Polynomial, similar_monomials_is_similar) {
	Polynomial::Monom m1("2xz^3q^7");
	Polynomial::Monom m2("6z^3q^7x");
	EXPECT_TRUE(m1.is_similar(m2));
}


TEST(Polynomial, can_mul_monom_by_scalar_with_assignment) {
	Polynomial::Monom m("2xy");
	m *= 2.5;
	EXPECT_DOUBLE_EQ(m.coefficient(), 5.0);
}

TEST(Polynomial, can_mul_monom_by_monom_with_assignment) {
	Polynomial::Monom m("2xy");
	m *= {2.5, { {'y',1},{'x',1} }};
	Polynomial::Monom m1("5x^2y^2");
	EXPECT_EQ(m, m1);
}

TEST(Polynomial, can_mul_monom_by_scalar) {
	Polynomial::Monom m("2xy");
	Polynomial::Monom m1 = m * 2.5;
	EXPECT_DOUBLE_EQ(m1.coefficient(), 5.0);
}

TEST(Polynomial, can_mul_monomials) {
	Polynomial::Monom m1("yx");
	Polynomial::Monom m2("4w^2x");
	Polynomial::Monom m3("4yx^2w^2");
	
	EXPECT_EQ(m1 * m2, m3);
}

TEST(Polynomial, mul_monom_by_scalar_is_commutative) {
	Polynomial::Monom m("2yzx");
	EXPECT_EQ(m * 2.5, 2.5 * m);
}

//===================POLYNOMIAL=======================

TEST(Polynomial, empty_poly_has_zero_deg) {
	Polynomial p;
	EXPECT_EQ(p.deg(), 0);
}

TEST(Polynomial, poly_with_monom_has_non_zero_deg) {
	Polynomial p("2xyq");
	EXPECT_EQ(p.deg(), 3);
}

TEST(Polynomial, poly_sorts_monomials) {
	Polynomial p("3abc - ab^2c + xy");
	auto it = p.begin();
	Polynomial::Monom m1(-1.0, { {'a',1},{'b',2},{'c',1} });
	Polynomial::Monom m2(3.0, { {'a',1},{'b',1},{'c',1} });
	Polynomial::Monom m3(1.0, { {'x',1},{'y',1} });
	EXPECT_EQ(*it, m1); 
	++it;
	EXPECT_EQ(*it, m2); 
	++it;
	EXPECT_EQ(*it, m3);
}

TEST(Polynomial, similar_polynomials_is_equal) {
	Polynomial p1("3abc - 2bca + xyz");
	Polynomial p2("abc + xyz");
	EXPECT_EQ(p1, p2);
}

TEST(Polynomial, non_similar_polynomials_is_not_equal) {
	Polynomial p1("3abc - 15bca + xyz");
	Polynomial p2("abc + xyz");
	EXPECT_NE(p1, p2);
}

TEST(Polynomial, polynomial_is_normalize_automatically) {
	Polynomial p1("3a^2bc^2 - 2bc^2a^2 + xyz - 4yzx");
	EXPECT_EQ(p1.term_count(), 2);
}

TEST(Polynomial, can_add_monom_with_poly_with_assignment) {
	Polynomial p("2xyz");
	Polynomial::Monom m("xyz^2");
	p += m;
	EXPECT_EQ(p, Polynomial{ "xyz^2 + 2xyz" });
}

TEST(Polynomial, can_sub_monom_with_poly_with_assignment) {
	Polynomial p("2xyz");
	Polynomial::Monom m("xyz^2");
	p -= m;
	EXPECT_EQ(p, Polynomial{ "-xyz^2 + 2xyz" });
}

TEST(Polynomial, can_add_polynomials_with_assignment) {
	Polynomial p1("2xyzw - 4t^2yz");
	Polynomial p2("5t^2yz - xyzw");
	p1 += p2;
	EXPECT_EQ(p1, Polynomial{ "t^2yz + xyzw" });
}

TEST(Polynomial, can_sub_polynomials_with_assignment) {
	Polynomial p1("2xyzw - 4t^2yz");
	Polynomial p2("5t^2yz - xyzw");
	p1 -= p2;
	EXPECT_EQ(p1, Polynomial{ "-9t^2yz + 3xyzw" });
}

TEST(Polynomial, can_mul_poly_by_scalar_with_assignment) {
	Polynomial p("2xyzq^2 - 5z^2y^3");
	p *= 5.0;
	EXPECT_EQ(p, Polynomial{ "10xyzq^2 - 25z^2y^3" });
}

TEST(Polynomial, can_mul_poly_by_monom_with_assignment) {
	Polynomial p("xyz - xy^2z^2");
	Polynomial::Monom m("x^2");
	p *= m;
	EXPECT_EQ(p, Polynomial{ "x^3yz - x^3y^2z^2" });
}

TEST(Polynomial, can_mul_poly_by_poly_with_assignment) {
	Polynomial p1("a^2 - b^2");
	Polynomial p2("a^2 + b^2");
	p1 *= p2;
	EXPECT_EQ(p1, Polynomial{ "a^4 - b^4" });
}

TEST(Polynomial, can_add_monom_with_poly) {
	Polynomial p("2xyz");
	Polynomial::Monom m("xyz^2");
	EXPECT_EQ(p + m, Polynomial{ "xyz^2 + 2xyz" });
}

TEST(Polynomial, can_sub_monom_with_poly) {
	Polynomial p("2xyz");
	Polynomial::Monom m("xyz^2");
	EXPECT_EQ(p - m, Polynomial{ "-xyz^2 + 2xyz" });
}

TEST(Polynomial, can_add_polynomials) {
	Polynomial p1("2xyzw - 4t^2yz");
	Polynomial p2("5t^2yz - xyzw");
	EXPECT_EQ(p1 + p2, Polynomial{ "t^2yz + xyzw" });
}

TEST(Polynomial, can_sub_polynomials) {
	Polynomial p1("2xyzw - 4t^2yz");
	Polynomial p2("5t^2yz - xyzw");
	EXPECT_EQ(p1 - p2, Polynomial{ "-9t^2yz + 3xyzw" });
}

TEST(Polynomial, can_mul_poly_by_scalar) {
	Polynomial p("2xyzq^2 - 5z^2y^3");
	EXPECT_EQ(p * 5, Polynomial{ "10xyzq^2 - 25z^2y^3" });
}

TEST(Polynomial, can_mul_poly_by_monom) {
	Polynomial p("abc^3 - ab^2c");
	Polynomial::Monom m("ab");
	EXPECT_EQ(p * m, Polynomial{ "a^2b^2c^3 - a^2b^3c" });
}

TEST(Polynomial, can_mul_poly_by_poly) {
	Polynomial p1("a - b");
	Polynomial p2("a^2 + ab + b^2");
	EXPECT_EQ(p1 * p2, Polynomial{ "a^3 - b^3" });
}

TEST(Polynomial, mul_poly_by_scalar_is_commutative) {
	Polynomial p("a + y");
	EXPECT_EQ(p * 2.0, 2.0 * p);
}

TEST(Polynomial, mul_poly_by_monom_is_commutative) {
	Polynomial p("a + x + y");
	Polynomial::Monom m("a");
	EXPECT_EQ(p * m, m * p);
}

TEST(Polynomial, mul_poly_by_poly_is_commutative) {
	Polynomial p1("a + b - 5");
	Polynomial p2("a^2 - ab + b^2");
	EXPECT_EQ(p1 * p2, p2 * p1);
}

TEST(Polynomial, mul_by_zero_clears_the_poly) {
	Polynomial p("a + b + c + 1");
	p *= 0.0;
	EXPECT_TRUE(p.is_zero());
}

TEST(Polynomial, can_calc_the_value_at_the_point) {
	Polynomial p("ax^2 + bx + c");
	SortedList<Polynomial::VariableValue, Polynomial::VariableValueCompare> values;
	values.insert({ 'x',5.0 });
	values.insert({ 'a',1.0 });
	values.insert({ 'b',12.0 });
	values.insert({ 'c',3.0 });
	EXPECT_EQ(p.calculate(values), 1.0 * 5.0 * 5.0 + 12.0 * 5.0 + 3.0);
}