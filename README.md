Here we present a short documentation with the description of the main functionalities.

This code is responsible for the results in the respective paper, which concerns, in summary,
different situations in which agents learn independently to play a spatial prisoner's dilemma in
an environment with defects, each with their own Q-table, then composing a multi-agent reinforcement learning framework,
but with single agent characteristics.

# Variables and constants

+ **NUM_CONF**, **LSIZE**  &#8594; number of independent samples and lattice size.
+ **INITIAL_STATE** &#8594; defines the initialization pattern, which is then passed to the
switch statement:
```
switch (INITIALSTATE)
   {
		case 1 : fprintf(freq,"Random\n");
			break;
		case 2 : fprintf(freq,"One D\n");
			break;
		case 3 : fprintf(freq,"D-Block\n");
			break;
		case 4 : fprintf(freq,"Exact\n");
			break;
		case 5 : fprintf(freq,"2 C's\n");
			break;
   }
```
+ **PROB_C**, **PROB_D** &#8594; probability of initialization as C or D.
+ **TOTALSTEPS**, **MEASURES** &#8594; total Monte Carlo steps and total measures to save in file.
+ **NUM_NEIGH** &#8594; number of neighbours.
+ **FRANDOM1** &#8594; variable to generate a random number uniformly between 0 and 1 using gsl.
+ **C**, **D**, **MOVE**, **Cindex**, **Dindex**, **MOVEindex** &#8594; integers associated with the actions and their respective indexes
+ **STATES[NUM_STATES]**, **ACTIONS[NUM_ACTIONS]** &#8594; lists with available states and actions.
+ **TEMPTATION_PAYOFF**, **NUM_DEFECTS**, **P_DIFFUSOION**, **ALPHA_SHARE** &#8594; value of the temptation b,
number of the defects (holes) in the lattice, probability of succesfully moving and sharing parameter s_r. All of these must
be passed as arguments to the executable.
+ **Q[LL][NUM_STATES][NUM_ACTIONS]** &#8594; Q table list, which will define a NUM_ACTIONS x NUM_STATES
matrix for all the LL agents, i.e., LL entries in the list.
+ **number_coop_average[MEASURES]**, **number_def_average[MEASURES]**, **number_coop_to_def_average[MEASURES]**, **average_Q_table[MEASURES][NUM_STATES][NUM_ACTIONS]** &#8594; lists to be used as
averages through each measure.
+ **s[LL]**, **payoff[LL]** &#8594; contain all the states and the payoffs of the players and will be
severely accessed through the simulation.
+ **right[LL]**, **left[LL]**, **top[LL]**, **down[LL]**, **neigh[LL][NUM_NEIGH]** &#8594; defines
all the neighbours in each direction for every player, with the __neigh__ list containing all of them.
+ **empty_matrix[LL]**, **which_empty[LL]** &#8594; will account for the empty spaces in the lattice.

# Functions and code funcionality

The **main** function is (clearly) first called. After doing a check that all arguments are rightly
passed (with no check for not a number or such, as this is expected from you to get right), we
generate a random seed and creating a time table, determining with it which steps will be measured
and saved, we call the __neighbours_2d__ function from __mc.h__, which gets the nearest neighbours
in the left, right, ..., lists, then calling __determine_neighbours__, which fills the **neigh[LL]**
list with neighbours, as such:

```
for(i=0; i<LL; ++i)
	{
		neigh[i][0] = left[i];
		neigh[i][1] = right[i];
		neigh[i][2] = top[i];
		neigh[i][3] = down[i];
	}
```

With the neighbours determined for each player, we then initialize the file, which will function as a
sort of database, writing in a .dat with the name given by:

```
sprintf(output_file_freq,"data/T%.2f_S_%.2f_LSIZE%d_rho%.5f_CONF_%d_%ld_prof.dat",
							TEMPTATION_PAYOFF,SUCKER_PAYOFF,LSIZE,1.0-NUM_DEFECTS/((float)LL),NUM_CONF,seed);
```

The condition __#ifdef USEGFX__ will then check if the flag is active to display the snapshots; if not,
we finally initialize the simulation, calling the main function of the code, __simulation()__.
