# Initial Analysis and Observations of Original Code

## Primary Loop
- The primary loop is a triple loop which uses real variables *f, fp, fptilde*. These typically stand in for function and derivative values, but no context is given.
- The first loop runs for a little over 1000 iterations, (hoping to) go through the set [-5, -4.99, ..., 0, ...., 4.99, 5.00]. So *f* takes on positive values, negative values, and 0.
- The second loop for *fp* runs for a little over 1000 iterations desring to take on the values [0, 0.01, ..., 9.99, 10.0]. 
- The third loops for *fptidle* runs for a little over 1000 iterations taking on the values [0, 0.01, ..., 9.99, 10.0].

### Improvements and Considerations
- We have to consider domain issues with the main function ```function_j```, particularly because all values can be 0 and some can be negative.  
- The loop structure works well enough in theory, however using = with floating point values is a dangerous game. **One improvement will be to change to to and (i,j,k) loop structure and compute the values for *f, fp, fptible* through standard mean**
- It is also worth noting that in the current construction, the only value for *S* accessible upon exiting the loop is that where *f, fp, fptilde* take on their final value. **I'm going to assume this is a mistake and proceed as though we need all possible values for *S*.**
- While refactoring, I would also like to use more descriptive variable names. However, no context is given, so for now **leave variable names in current implementation**.

## Function_j Analysis and Improvements
The initial analysis of function_j shows the following:
- The code uses a large number of values that are constant. However, these are stored on each iteration. In particular *pi* is computed instead of using a built in constant and *fptildemin* is computed instead each iteration instead of being computed once and reused. **Make _gC, aC, a, b, g, pi, fptildemin, aX, gX, saC, saX, sbC, sbX_ constants or at the very least global variables taht are stored and reused**.
- In line 34 we divide by *fp*, but *fp* can be zero. Not great. **Make the first value fo *fp* to be 0.01 instead of 0**.
- On each call to this function we compute both ```sigma_a``` and ```sigma_b```; However, only one of these is used. Also, at times ```sigma_a``` and ```sigma_b``` are using the constant *fptildemin* for computation, making them constant. **Compute only the needed value of sigma**  
- The computation for *sigma* only using *fptidle*. Thus, if we can rearrange the loop, this value can be computed before entering the inner loops. **Rearrange the loop so that *fptilde* is the outer loop and compute sigma here**
- **Do the same for alpha and gamma** as the above suggests
- There are several calls to the ```pow``` function that use an integer as the exponent. The ```pow``` is O(log(n)), I believe. However, for integers we can reduce this to O(1) by using simple arithmetic.  **Change calls to pow to use multiplication and/or division when the exponent is an integer**.

