# First Pass Results

Make changes to the code base based purely on visual observations produced a significant improvement in performance. Timing the new code and comparing to the original code showed the following execution times in seconds:

 original | improved |
----------|----------|
 546      | 119      |
 Thus we get about a 4.6x performance improvement. (Note: this is based on one data point. We should perform the experiment several times to get a more accurate result). The code was compared to the original to ensure that the results were consistent. 

## Possible Improvements

### Looping over *f/fp*   
Another observation is that the function may have some added symmetry that we can exploit if further speed up is required.  While one part of the function does use *f^-5*; the computation steps in the inner most loop that are most expensive are the expoentials. As the arguments of the expoentials we have 
- a terms that involves *f/fp*
- another term of the form *(f-fp)/fp* that can be rewritten as *f/fp -1*. 
Therefore it is apparent that these computations do not hinge on the individual values of *f* and *fp*, but on there ratio. 
The value of *f* is from the set {-5, -4.99, ..., 0, ...., 4.99, 5.00}. 
The values of  *fp* are from {0, 0.01, ..., 9.99, 10.0}. 
Running the two loops we have roughly 1000 x 1000 = 1,000,000 combinations. 
However, taking all possible ratios of values form the two sets leaves us with only about 1000 possibilities as many ratios are repeated (e.g. 2/1 is the same as 4/2). 

Thus it is possible to get further significant performance improvements by making the inner loop iterate over the 1000 unique values of *f/fp* and have the most inner loop iterate over *f*. This would reduce the number of calls to ```exp``` drastically. The inner most loop would simply do the last multiplication factor for *f^-5* with the middle loop computing the two ```exp```.

# Evaluation with GPROF

I decide to use (and re-learn) gprof. Here are the general instructions. 
```
gcc -pg -o my_program my_program.c
./my_program
gprof my_program > my_program.gprof

```

To view teh top functions that are taking the most time, a command such as the following will work:
```
gprof -p my_program > my_program.gprof | sort -n -k10,10 | head -10

```