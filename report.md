<!-- AUTHOR -> AAYUSH GOEL -->
# Question 3
* To compile -> gcc -pthread q3.c
* To Run -> ./a.out

# BONUS
* To compile -> gcc -pthread q3_bonus.c
* To Run -> ./a.out

## q3.c compares efficiency between implementing mergesort through simple recursion or using child processes at each step.
<!-- Mergesort with forking work upto no. of elements < 1040000 -->

## q3_bonus.c compares efficiency between implementing mergesort through simple recursion and using thread processes at each step.
<!-- Mergesort with threads work upto no. of elements < 40000 -->

# REPORT
No. of elements---Recursion Time---Time with Forking---Time with Threads  
   1                0.000002           0.000058           0.000327
   10               0.000006           0.000441           0.000594
   100              0.000033           0.000284           0.007469
   1000             0.000145           0.000541           0.043780
   10000            0.001777           0.001157           0.277778
   20000            0.003543           0.001858           0.528239
   40000            0.007031           0.003283           1.002899

## Time for threads is the max because of the overhead  of thread creation. Thread should promote higher efficiency but higher overhead reduces efficiency manyhold. 