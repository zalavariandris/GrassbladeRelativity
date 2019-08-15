#pragma once
/*
ported from paperjs, numerical.js
*/
#include <vector>
#include <functional>
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
namespace Numerical {
// Constants
/**
	* A very small absolute value used to check if a value is very close to
	* zero. The value should be large enough to offset any floating point
	* noise, but small enough to be meaningful in computation in a nominal
	* range (see MACHINE_EPSILON).
	*
	* http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
	* http://www.cs.berkeley.edu/~wkahan/Math128/Cubic.pdf
	*/

	extern double EPSILON;

	/**
	* The machine epsilon for a double precision (Javascript Number) is
	* 2.220446049250313e-16. (try this in the js console:
	*     (function(){ for (var e = 1; 1 < 1+e/2;) e/=2; return e }())
	*
	* The constant MACHINE_EPSILON here refers to the constants δ and ε
	* such that, the error introduced by addition, multiplication on a
	* 64bit float (js Number) will be less than δ and ε. That is to say,
	* for all X and Y representable by a js Number object, S and P be their
	* 'exact' sum and product respectively, then
	* |S - (x+y)| <= δ|S|, and |P - (x*y)| <= ε|P|.
	* This amounts to about half of the actual machine epsilon.
	*/

	extern double MACHINE_EPSILON;

	/**
	* The epsilon to be used when handling curve-time parameters. This
	* cannot be smaller, because errors add up to around 2e-7 in the bezier
	* fat-line clipping code as a result of recursive sub-division.
	*/
	extern double CURVETIME_EPSILON;
	/**
	* The epsilon to be used when performing "geometric" checks, such as
	* distances between points and lines.
	*/
	extern double GEOMETRIC_EPSILON;
	/**
	* The epsilon to be used when performing "trigonometric" checks, such
	* as examining cross products to check for collinearity.
	*/
	extern double TRIGONOMETRIC_EPSILON;
	/**
	* Kappa is the value which which to scale the curve handles when
	* drawing a circle with bezier curves.
	*
	* http://whizkidtech.redprince.net/bezier/circle/kappa/
	*/
	extern double KAPPA;

	double clamp(double value, double min, double max);

	double getDiscriminant(double a, double b, double c);

	double getMax(std::vector<double> values);

	double getMin(std::vector<double> values);

	double getNormalizationFactor(std::vector<double> values);

	/**
	* Checks if the value is 0, within a tolerance defined by
	* Numerical.EPSILON.
	*/
	bool isZero(double val);

	bool isMachineZero(double val);

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
	double integrate(std::function<double(double)> f, double a, double b, int n);

	/**
	* Root finding using Newton-Raphson Method combined with Bisection.
	*/
	double findRoot(std::function<double(double)> f, std::function<double(double)> df, double x, double a, double b, int n, double tolerance);

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
	double solveQuadratic(double a, double b, double c, std::vector<double> roots, double min = NAN, double max = NAN);

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
	double solveCubic(double a, double b, double c, double d, std::vector<double> & roots, double min = NAN, double max = NAN);
};