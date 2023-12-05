# Results

Making changes to the code base based purely on visual observations produced a significant improvement in performance. Timing the new code and comparing to the original code showed the following execution times in seconds:

 original | improved |
----------|----------|
 546      | 119      |

 Thus we get about a 4.6x performance improvement. (Note: this is based on one data point. We should perform the experiment several times to get a more accurate result). 
 
 ## Correctness
 The optimized function output was also compared to the original function output to ensure that the results were consistent. 

## Possible Improvements

### Looping over *f/fp*   
Another observation is that the function has some added symmetry that we can exploit if further speed up is required.  
While one part of the function does use *f^-5*; the computation steps in the inner most loop that are most expensive are the exponential calls. As the arguments of the exponential calls we have 
- a terms that involves *f/fp*
- another term of the form *(f-fp)/fp* that can be rewritten as *f/fp -1*. 
Therefore it is apparent that these computations do not hinge on the individual values of *f* and *fp*, but on the ratio. 
The value of *f* is from the set {-5, -4.99, ..., 0, ...., 4.99, 5.00}. 
The values of  *fp* are from {0, 0.01, ..., 9.99, 10.0}. 
Running the two loops we have roughly 1000 x 1000 = 1,000,000 combinations. 
However, taking all possible ratios of values form the two sets leaves us with only about 1000 possibilities as many ratios are repeated (e.g. 2/1 is the same as 4/2). 

Thus it is possible to get further **significant** performance improvements by making the inner loop iterate over the 325966 unique values of *f/fp* and have the most inner loop iterate over *f*. This would reduce the number of calls to ```exp``` drastically. The inner most loop would simply do the last multiplication factor for *f^-5*, with the middle loop computing the two ```exp```.

# Lessons Learned

A few observations
- It is difficult to fully optimize without knowing the underlying functions and corresponding documentation. Further improvements could certainly be possible with
increased understanding of how the values need to be used and if storing values is required.
- From first programming courses we are told descriptive variables names and comments are important. I think this type of code drives home that fact.
- WSL is a good approximation to Linux, but it seems as though the profilers need the real deal.