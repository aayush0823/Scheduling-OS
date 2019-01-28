<!-- AUTHOR -> AAYUSH GOEL -->
# Question 1
* To compile -> gcc -pthread q1.c
* To Run -> ./a.out

## Logic
** The program asks an input n from user which implies 2*n players and n refrees will enter the academy.
** Randomly a player or an refree is called based upon the remaining number of refrees and players.
** For each person , a player or refree a thread is generated.
** Once the thread initiates the person enters the academy.
** A mutex lock named lock is used to ensure mutual exclusion.
** The players or refree wait till they don't form a required set of 3.
** Then they look for mutex lock and when it's available they meet the organiser and the player and refree evoke corresponding tasks till the game starts and manager is free , therefore , lock available.

# Question 2
* To compile -> gcc -pthread q2.c
* To Run -> ./a.out

## Logic
** Firstly we enter the number of booths and the corresponding number of evms and no of voters for each booth.
** A thread is generated for each booth.
** Each booth then initiates threads corresponding to all its evms and voters.
** Each voter in it's thread wait until a corresponding evm is available to vote.
** The evm gets randomly assigned voters and once its all slots are full or no more voters are available , it sends a signal to voter that it's ready for voting phase.
** After recieving the signal the voter casts its vote and then sends the corresponding signal.
** Once the evm recieves signal that all it's voters are done it terminates its voting phase.
** This continues till all voters at all booths have casted there votes successfully.