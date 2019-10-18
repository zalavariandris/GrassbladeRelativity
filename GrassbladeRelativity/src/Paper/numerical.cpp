﻿#include "numerical.h"
#include <iostream>
#include <algorithm> //min, max, max_element
/*
ported from paperjs, numerical.js
*/

#include <utility> //std::pair
/*
 * Paper.js - The Swiss Army Knife of Vector Graphics Scripting.
 * http://paperjs.org/
 *
 * Copyright (c) 2011 - 2019, Juerg Lehni & Jonathan Puckey
 * http://scratchdisk.com/ & https://puckey.studio/
 *
 * Distributed under the MIT license. See LICENSE file for details.
 *
 * All rights reserved.
 */

 /**
  * @name Numerical
  * @namespace
  * @private
  */

  // Lookup tables for abscissas and weights with values for n = 2 .. 16.
	  // As values are symmetric, only store half of them and adapt algorithm
	  // to factor in symmetry.
std::vector<std::vector<double>> abscissas = {
	{0.5773502691896257645091488},
	{0,0.7745966692414833770358531},
	{0.3399810435848562648026658,0.8611363115940525752239465},
	{0,0.5384693101056830910363144,0.9061798459386639927976269},
	{0.2386191860831969086305017,0.6612093864662645136613996,0.9324695142031520278123016},
	{0,0.4058451513773971669066064,0.7415311855993944398638648,0.9491079123427585245261897},
	{0.1834346424956498049394761,0.5255324099163289858177390,0.7966664774136267395915539,0.9602898564975362316835609},
	{0,0.3242534234038089290385380,0.6133714327005903973087020,0.8360311073266357942994298,0.9681602395076260898355762},
	{0.1488743389816312108848260,0.4333953941292471907992659,0.6794095682990244062343274,0.8650633666889845107320967,0.9739065285171717200779640},
	{0,0.2695431559523449723315320,0.5190961292068118159257257,0.7301520055740493240934163,0.8870625997680952990751578,0.9782286581460569928039380},
	{0.1252334085114689154724414,0.3678314989981801937526915,0.5873179542866174472967024,0.7699026741943046870368938,0.9041172563704748566784659,0.9815606342467192506905491},
	{0,0.2304583159551347940655281,0.4484927510364468528779129,0.6423493394403402206439846,0.8015780907333099127942065,0.9175983992229779652065478,0.9841830547185881494728294},
	{0.1080549487073436620662447,0.3191123689278897604356718,0.5152486363581540919652907,0.6872929048116854701480198,0.8272013150697649931897947,0.9284348836635735173363911,0.9862838086968123388415973},
	{0,0.2011940939974345223006283,0.3941513470775633698972074,0.5709721726085388475372267,0.7244177313601700474161861,0.8482065834104272162006483,0.9372733924007059043077589,0.9879925180204854284895657},
	{0.0950125098376374401853193,0.2816035507792589132304605,0.4580167776572273863424194,0.6178762444026437484466718,0.7554044083550030338951012,0.8656312023878317438804679,0.9445750230732325760779884,0.9894009349916499325961542}
};

std::vector<std::vector<double>> weights = {
	{1},
	{0.8888888888888888888888889,0.5555555555555555555555556},
	{0.6521451548625461426269361,0.3478548451374538573730639},
	{0.5688888888888888888888889,0.4786286704993664680412915,0.2369268850561890875142640},
	{0.4679139345726910473898703,0.3607615730481386075698335,0.1713244923791703450402961},
	{0.4179591836734693877551020,0.3818300505051189449503698,0.2797053914892766679014678,0.1294849661688696932706114},
	{0.3626837833783619829651504,0.3137066458778872873379622,0.2223810344533744705443560,0.1012285362903762591525314},
	{0.3302393550012597631645251,0.3123470770400028400686304,0.2606106964029354623187429,0.1806481606948574040584720,0.0812743883615744119718922},
	{0.2955242247147528701738930,0.2692667193099963550912269,0.2190863625159820439955349,0.1494513491505805931457763,0.0666713443086881375935688},
	{0.2729250867779006307144835,0.2628045445102466621806889,0.2331937645919904799185237,0.1862902109277342514260976,0.1255803694649046246346943,0.0556685671161736664827537},
	{0.2491470458134027850005624,0.2334925365383548087608499,0.2031674267230659217490645,0.1600783285433462263346525,0.1069393259953184309602547,0.0471753363865118271946160},
	{0.2325515532308739101945895,0.2262831802628972384120902,0.2078160475368885023125232,0.1781459807619457382800467,0.1388735102197872384636018,0.0921214998377284479144218,0.0404840047653158795200216},
	{0.2152638534631577901958764,0.2051984637212956039659241,0.1855383974779378137417166,0.1572031671581935345696019,0.1215185706879031846894148,0.0801580871597602098056333,0.0351194603317518630318329},
	{0.2025782419255612728806202,0.1984314853271115764561183,0.1861610000155622110268006,0.1662692058169939335532009,0.1395706779261543144478048,0.1071592204671719350118695,0.0703660474881081247092674,0.0307532419961172683546284},
	{0.1894506104550684962853967,0.1826034150449235888667637,0.1691565193950025381893121,0.1495959888165767320815017,0.1246289712555338720524763,0.0951585116824927848099251,0.0622535239386478928628438,0.0271524594117540948517806}
};

double Numerical::EPSILON = 1e-12;

double Numerical::MACHINE_EPSILON = 1.12e-16;

double Numerical::CURVETIME_EPSILON = 1e-8;

double Numerical::GEOMETRIC_EPSILON = 1e-7;

double Numerical::TRIGONOMETRIC_EPSILON = 1e-8;

double Numerical::KAPPA = 4 * (sqrt(2) - 1) / 3;

// Math short-cuts for often used methods and values
double Numerical::clamp(double value, double min, double max) {
	return value < min ? min : value > max ? max : value;
}

double Numerical::getDiscriminant(double a, double b, double c) {
	// d = b^2 - a * c  computed accurately enough by a tricky scheme.
	// Ported from @hkrish's polysolve.c
	auto split = [](double v)->std::pair<double, double> {
		double x = v * 134217729;
		double y = v - x;
		double hi = y + x; // Don't optimize y away!
		double lo = v - hi;
		return { hi, lo };
	};

	double D = b * b - a * c;
	double E = b * b + a * c;

	if (abs(D) * 3 < E) {
		std::pair<double, double> ad = split(a);
		std::pair<double, double> bd = split(b);
		std::pair<double, double> cd = split(c);
		double p = b * b;
		double dp = (bd.first * bd.first - p + 2 * bd.first * bd.second) + bd.second * bd.second;
		double q = a * c;
		double dq = (ad.first * cd.first - q + ad.first * cd.second + ad.second * cd.first)
			+ ad.second * cd.second;
		D = (p - q) + (dp - dq); // Don’t omit parentheses!
	}
	return D;
}

double Numerical::getNormalizationFactor(std::vector<double> values) {
	// Normalize coefficients à la Jenkins & Traub's RPOLY.
	// Normalization is done by scaling coefficients with a power of 2, so
	// that all the bits in the mantissa remain unchanged.
	// Use the infinity norm (max(sum(abs(a)…)) to determine the appropriate
	// scale factor. See @hkrish in #1087#issuecomment-231526156
	double norm = *std::max_element(values.begin(), values.end());
	return norm && (norm < 1e-8 || norm > 1e8)
		? pow(2, -round(log2(norm)))
		: 0;
}

/**
* Checks if the value is 0, within a tolerance defined by
* Numerical.EPSILON.
*/
bool Numerical::isZero(double val) {
	return val >= -EPSILON && val <= EPSILON;
};

bool Numerical::isMachineZero(double val) {
	return val >= -MACHINE_EPSILON && val <= MACHINE_EPSILON;
};

/**
* Returns a number whose value is clamped by the given range.
*
* @param {Number} value the value to be clamped
* @param {Number} min the lower boundary of the range
* @param {Number} max the upper boundary of the range
* @return {Number} a number in the range of [min, max]
*/


/**
* Gauss-Legendre Numerical Integration.
*/
double Numerical::integrate(std::function<double(double)> f, double a, double b, int n) {
	auto x = abscissas[n - 2];
	auto w = weights[n - 2];
	double A = (b - a) * 0.5;
	double B = A + a;
	auto i = 0;
	auto m = (n + 1) >> 1;
	auto sum = n & 1 ? w[i++] * f(B) : 0; // Handle odd n
	while (i < m) {
		double Ax = A * x[i];
		sum += w[i++] * (f(B + Ax) + f(B - Ax));
	}
	return A * sum;
};

/**
* Root finding using Newton-Raphson Method combined with Bisection.
*/
double Numerical::findRoot(std::function<double(double)> f, std::function<double(double)> df, double x, double a, double b, int n, double tolerance) {
	for (auto i = 0; i < n; i++) {
		auto fx = f(x);
		// Calculate a new candidate with the Newton-Raphson method.
		auto dx = fx / df(x);
		auto nx = x - dx;
		// See if we can trust the Newton-Raphson result. If not we use
		// bisection to find another candidate for Newton's method.
		if (abs(dx) < tolerance) {
			x = nx;
			break;
		}
		// Update the root-bounding interval and test for containment of
		// the candidate. If candidate is outside the root-bounding
		// interval, use bisection instead.
		// There is no need to compare to lower / upper because the
		// tangent line has positive slope, guaranteeing that the x-axis
		// intercept is larger than lower / smaller than upper.
		if (fx > 0) {
			b = x;
			x = nx <= a ? (a + b) * 0.5 : nx;
		}
		else {
			a = x;
			x = nx >= b ? (a + b) * 0.5 : nx;
		}
	}
	// Return the best result even though we haven't gotten close
	// enough to the root... (In paper.js this never seems to happen).
	// But make sure, that it actually is within the given range [a, b]
	return clamp(x, a, b);
};

/**
* Solve a quadratic equation in a numerically robust manner;
* given a quadratic equation ax² + bx + c = 0, find the values of x.
*
* References:
*  Kahan W. - "To Solve a Real Cubic Equation"
*  http://www.cs.berkeley.edu/~wkahan/Math128/Cubic.pdf
*  Blinn J. - "How to solve a Quadratic Equation"
*  Harikrishnan G.
*  https://gist.github.com/hkrish/9e0de1f121971ee0fbab281f5c986de9
*
* @param {Number} a the quadratic term
* @param {Number} b the linear term
* @param {Number} c the constant term
* @param {Number[]} roots the array to store the roots in
* @param {Number} [min] the lower bound of the allowed roots
* @param {Number} [max] the upper bound of the allowed roots
* @return {Number} the number of real roots found, or -1 if there are
* infinite solutions
*
* @author Harikrishnan Gopalakrishnan <hari.exeption@gmail.com>
*/
int Numerical::solveQuadratic(double a, double b, double c, std::vector<double> & roots, double min, double max) {
	auto x1 = std::numeric_limits<double>::infinity();
	auto x2 = x1;
	if (abs(a) < EPSILON) {
		// This could just be a linear equation
		if (abs(b) < EPSILON)
			return abs(c) < EPSILON ? -1 : 0;
		x1 = -c / b;
	}
	else {
		// a, b, c are expected to be the coefficients of the equation:
		// Ax² - 2Bx + C == 0, so we take b = -b/2:
		b *= -0.5;
		auto D = getDiscriminant(a, b, c);
		// If the discriminant is very small, we can try to normalize
		// the coefficients, so that we may get better accuracy.
		if (D && abs(D) < MACHINE_EPSILON) {

			auto f = getNormalizationFactor({ abs(a), abs(b), abs(c) });
			if (f) {
				a *= f;
				b *= f;
				c *= f;
				D = getDiscriminant(a, b, c);
			}
		}
		if (D >= -MACHINE_EPSILON) { // No real roots if D < 0
			auto Q = D < 0 ? 0 : sqrt(D);
			auto R = b + (b < 0 ? -Q : Q);
			// Try to minimize floating point noise.
			if (R == 0) {
				x1 = c / a;
				x2 = -x1;
			}
			else {
				x1 = R / a;
				x2 = c / R;
			}
		}
	}

	int count = 0;
	bool boundless = min == NAN;
	double minB = min - EPSILON;
	double maxB = max + EPSILON;
	// We need to include EPSILON in the comparisons with min / max,
	// as some solutions are ever so lightly out of bounds.
	if (isfinite(x1) && (boundless || x1 > minB && x1 < maxB)) {
		roots.push_back(boundless ? x1 : clamp(x1, min, max));
		count++;
	}
	if (x2 != x1 && isfinite(x2) && (boundless || x2 > minB && x2 < maxB)) {
		roots.push_back( boundless ? x2 : clamp(x2, min, max) );
		count++;
	}
	return count;
};

/**
* Solve a cubic equation, using numerically stable methods,
* given an equation of the form ax³ + bx² + cx + d = 0.
*
* This algorithm avoids the trigonometric/inverse trigonometric
* calculations required by the "Italins"' formula. Cardano's method
* works well enough for exact computations, this method takes a
* numerical approach where the double precision error bound is kept
* very low.
*
* References:
*  Kahan W. - "To Solve a Real Cubic Equation"
*   http://www.cs.berkeley.edu/~wkahan/Math128/Cubic.pdf
*  Harikrishnan G.
*  https://gist.github.com/hkrish/9e0de1f121971ee0fbab281f5c986de9
*
* W. Kahan's paper contains inferences on accuracy of cubic
* zero-finding methods. Also testing methods for robustness.
*
* @param {Number} a the cubic term (x³ term)
* @param {Number} b the quadratic term (x² term)
* @param {Number} c the linear term (x term)
* @param {Number} d the constant term
* @param {Number[]} roots the array to store the roots in
* @param {Number} [min] the lower bound of the allowed roots
* @param {Number} [max] the upper bound of the allowed roots
* @return {Number} the number of real roots found, or -1 if there are
* infinite solutions
*
* @author Harikrishnan Gopalakrishnan <hari.exeption@gmail.com>
*/
int Numerical::solveCubic(double a, double b, double c, double d, std::vector<double> & roots, double min, double max) {
	double f = getNormalizationFactor({ abs(a), abs(b), abs(c), abs(d) });
	double x, b1, c2, qd, q;
	if (f) {
		a *= f;
		b *= f;
		c *= f;
		d *= f;
	}

	auto evaluate = [a, b, c, d, &b1,  &c2,  &qd,  &q,  &x](double x0) {
		x = x0;
		// Evaluate q, q', b1 and c2 at x
		double tmp = a * x;
		b1 = tmp + b;
		c2 = b1 * x + c;
		qd = (tmp + b1) * x + c2;
		q = c2 * x + d;
	};

	// If a or d is zero, we only need to solve a quadratic, so we set
	// the coefficients appropriately.
	if (abs(a) < EPSILON) {
		a = b;
		b1 = c;
		c2 = d;
		x = std::numeric_limits<double>::infinity();
	}
	else if (abs(d) < EPSILON) {
		b1 = b;
		c2 = c;
		x = 0;
	} else {
		// Here onwards we iterate for the leftmost root. Proceed to
		// deflate the cubic into a quadratic (as a side effect to the
		// iteration) and solve the quadratic.
		evaluate(-(b / a) / 3);
		// Get a good initial approximation.
		auto t = q / a;
		auto r = pow(abs(t), 1 / 3);
		auto s = t < 0 ? -1 : 1;
		auto td = -qd / a;
		// See Kahan's notes on why 1.324718*... works.
		auto rd = td > 0 ? 1.324717957244746 * std::max(r, sqrt(td)) : r;
		auto x0 = x - s * rd;
		if (x0 != x) {
			do {
				evaluate(x0);
				// Newton's. Divide by 1 + MACHINE_EPSILON (1.000...002)
				// to avoid x0 crossing over a root.
				x0 = qd == 0 ? x : x - q / qd / (1 + MACHINE_EPSILON);
			} while (s * x0 > s * x);
			// Adjust the coefficients for the quadratic.
			if (abs(a) * x * x > abs(d / x)) {
				c2 = -d / x;
				b1 = (c2 - c) / x;
			}
		}
	}
	// The cubic has been deflated to a quadratic.
	int count = Numerical::solveQuadratic(a, b1, c2, roots, min, max);
	bool boundless = min == NAN;
	if (isfinite(x) && (count == 0
		|| count > 0 && x != roots[0] && (roots.size()>1 ? x != roots[1] : true)  )
		&& (boundless || x > min - EPSILON && x < max + EPSILON))
	{
		roots.push_back(boundless ? x : clamp(x, min, max));
		count++;
	}
	return count;
};
