/* Author: Jeremy Becnel
Date: 12/3/2023
This exercise is to take some original code and attempt to make it run faster by
both examining the code for possible inefficiencies and also
use profiling tools such as GPROF and PERF to identify and quantify 
the steps that are most time-consuming.
*/
#include <stdio.h>
#include <math.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// gcc -lm test_j.c -o test_j

// Constants used in function computation.
// TODO: find out names of constants to make more descriptive
const double A = 0.0081;
const double B = 0.6;
const double G = 9.807;
const double G2 = 9.807 * 9.807;
const double G_C = 5.87;
const double A_C = 0.0317;
const double SA_C = 0.0547;
const double SA_X = 0.32;
const double SB_C = 0.0783;
const double SB_X = 0.16;
const double TWO_PI_NEG4 = 0.00064162389;

//=====================ORIGINAL CODE====================

/* For reference, this is the original (non-optimized)
function that computes function j given values for f, fp, fptilde.

We optimize this below.*/
double function_j(double f, double fp, double fptilde)
{
  double a = 0.0081;
  double b = 0.6;
  double g = 9.807;
  double pi = 4. * atan(1.);

  double fptildemin = (1.0 / 2.0 / pi) * pow((4.0 * b / 5.0), (1.0 / 4.0));

  double gC = 5.87;
  double aC = 0.0317;

  double aX = (log(a) - log(aC)) / log(fptildemin);
  double gX = -log(gC) / log(fptildemin);

  double saC = 0.0547;
  double saX = 0.32;

  double sbC = 0.0783;
  double sbX = 0.16;

  double fpt = MAX(fptilde, fptildemin);

  double alpha = aC * pow(fpt, aX);
  double gamma = gC * pow(fpt, gX);
  double sigma_a = saC * pow(fpt, saX);
  double sigma_b = sbC * pow(fpt, sbX);
  double exp1arg = -1.25 * pow((f / fp), -4);
  double sigma = (f <= fp) * sigma_a + (f > fp) * sigma_b;

  double exp2arg = -0.5 * pow((f - fp) / (sigma * fp), 2);

  double S = alpha * pow(g, 2) * pow((2 * pi), -4) * pow(f, -5) * exp(exp1arg) * pow(gamma, exp(exp2arg));

  return S;
}

/* For reference this is the original loop provided in the main
method for this file.
*/
static void originalLoop()
{
  double S, f, fp, fptilde;

  for (f = -5.; f <= 5.; f += 0.01)
  {
    for (fp = 0.; fp <= 10.; fp += 0.01)
    {
      for (fptilde = 0.; fptilde <= 10.; fptilde += 0.01)
      {
        S = function_j(f, fp, fptilde);
      }
    }
  }
}

//=======================HELPER FUNCTIONS =================
// The following three fuctions are designed to replace calls
// to pow in our computation.

/* This is a helper method that compute the argument
to the expoential used in function j*/
static double findArg1(double f, double fp)
{
  // compute -1.25 * (f/fp)^-4 which is the same as -1.25 * (fp/f)^4
  double value = fp / f;
  value = value * value; // squared
  value = value * value; // fourth power
  return -1.25 * value;
}

/* This is a second helper method to compute the second
argument used in the expoential involved in function_j.*/
static double findArg2(double f, double fp, double sigma)
{
  // compute -0.5 [(f-fp)/(sigma*fp)]^2
  double value = (f - fp) / (sigma * fp);
  return -0.5 * value * value;
}

/* This is the third helper method that is meant */
static double fNegFive(double f)
{
  // compute f^-5
  double value = f * f;      // squard
  value = value * value * f; // fifth
  return 1 / value;          // negative fifth;
}

//=======================FUNCTION COMPUTATION =============================
// This following computes the value of function_j is a more optimized way.
static double function_j_opt(double f, double fp, double fptilde, double alpha, double gamma, double sigma)
{
  double arg1 = findArg1(f, fp);
  double arg2 = findArg2(f, fp, sigma);

  return alpha * G2 * TWO_PI_NEG4 * fNegFive(f) * exp(arg1) * pow(gamma, exp(arg2));
}

// The following reorders the original loop to make the computation
// quicker.
static void improvedLoop()
{
  // The following values is approximately 0.132474
  double fptildemin = (1.0 / 2.0 / M_PI) * pow((4.0 * B / 5.0), (1.0 / 4.0));

  // Note: log is  ln, i.e. base e
  double aX = (log(A) - log(A_C)) / log(fptildemin); // could combine logs into 1 here
  double gX = -log(G_C) / log(fptildemin);

  int n = 1000; // number of iterations

  // values computed in oor
  double f, fp, fptilde, fpt;
  double alpha, gamma, sigma_a, sigma_b, sigma;

  // structure the loop to compute fptilde first and accompanying variables
  // fptilde and it's calculations are the most computationally expensive
  // so we opt to do this first
  // Also, the final computation needs the value for sigma which is computed
  // from fptilde
  for (int i = 0; i <= n; i++)
  {
    fptilde = 0 + i * 0.01;
    fpt = MAX(fptilde, fptildemin);

    alpha = A_C * pow(fpt, aX);
    gamma = G_C * pow(fpt, gX);
    sigma_a = SA_C * pow(fpt, SA_X);
    sigma_b = SB_C * pow(fpt, SB_X);
    // now we find values for f and fp and compute the function value given the
    // information found thuse for
    for (int j = 1; j <= n; j++)
    { // start at 1 to avoid fp = 0; in findArg2 we divide by fp
      fp = 0 + j * 0.01;
      for (int k = 0; k <= n; k++)
      {
        if (k != 500) // avoid division by zero in findArg1
        {
          f = -5 + k * 0.01;
          sigma = (f <= fp) * sigma_a + (f > fp) * sigma_b;
          function_j_opt(f, fp, fptilde, alpha, gamma, sigma);
        }
      }
    }
  }
}

static void timeComparison()
{
  clock_t t;
  t = clock();
  originalLoop();
  t = clock() - t;
  double time_taken = ((double)t) / CLOCKS_PER_SEC;
  printf("Original takes: %f seconds.\n", time_taken);

  t = clock();
  improvedLoop();
  t = clock() - t;
  time_taken = ((double)t) / CLOCKS_PER_SEC;
  printf("Improved takes: %f seconds.\n", time_taken);
}


static void compareValues()
{
  // The following values is approximately 0.132474
  double fptildemin = (1.0 / 2.0 / M_PI) * pow((4.0 * B / 5.0), (1.0 / 4.0));

  // Note: log is  ln, i.e. base e
  double aX = (log(A) - log(A_C)) / log(fptildemin); // could combine logs into 1 here
  double gX = -log(G_C) / log(fptildemin);

  int n = 5; // number of iterations

  // values computed in oor
  double f, fp, fptilde, fpt;
  double alpha, gamma, sigma_a, sigma_b, sigma;

  // structure the loop to compute fptilde first and accompanying variables
  // fptilde and it's calculations are the most computationally expensive
  // so we opt to do this first
  // Also, the final computation needs the value for sigma which is computed
  // from fptilde
  for (int i = 0; i <= n; i++)
  {
    fptilde = 0 + i * 2.0;
    fpt = MAX(fptilde, fptildemin);

    alpha = A_C * pow(fpt, aX);
    gamma = G_C * pow(fpt, gX);
    sigma_a = SA_C * pow(fpt, SA_X);
    sigma_b = SB_C * pow(fpt, SB_X);
    // now we find values for f and fp and compute the function value given the
    // information found thuse for
    for (int j = 1; j <= n; j++)
    { // start at 1 to avoid fp = 0; in findArg2 we divide by fp
      fp = 0 + j * 2.0;
      for (int k = 0; k <= n; k++)
      {
          f = -5 + k * 2.0;
          sigma = (f <= fp) * sigma_a + (f > fp) * sigma_b;
          double output = function_j_opt(f, fp, fptilde, alpha, gamma, sigma);
          printf("Optimized function output: %.8f\n", output);
          output = function_j(f,fp,fptilde);
          printf("Original function output: %.8f\n", output);
        
      }
    }
  }
}


int main()
{
  // call the following for timing comparison
  timeComparison();

  // call the following for comparing values to enure
  // new code results in same computational values
  //compareValues();
  return 0;
}
