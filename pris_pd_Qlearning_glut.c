/***************************************************************************
 *                 Prisoner's dilemma with Q-learning                      *
 *                      reward sharing version                             *
 *                                                                         *
 *    For the documentation and description of functions and variables,    *
 *    please read the README.md                                            *
 *                                                                         *
 *    Code written by: Mendeli Vanstein and Gustavo Mangold                *
 ***************************************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>

 #include <mc.h>

 #include "glut_view_pd_best.h"

 #include <time.h>
 #include <gsl/gsl_rng.h>
 #include <gsl/gsl_randist.h>


 /***************************************************************************
  *                          Constant Declarations                           *
  ***************************************************************************/
 const int NUM_CONF       = 1;
 #define   LSIZE           100 //200
 #define   LL              (LSIZE*LSIZE)

 const int INITIALSTATE   = 4;

 const double PROB_C	  = 0.5;
 const double PROB_D      = 1.0 - PROB_C;

 const int    TOTALSTEPS  = 5000;

 #define    MEASURES   1000
 #define    NUM_NEIGH  4

 #define FRANDOM1   (gsl_rng_uniform(r))

 //#define USEGFX
 //#define SELF_INTERACTION
 //#define DIFFUSE

 //Here we define the actions, then the states
 const int  C              =  1;
 const int  D              = -1;
 const int  MOVE           =  0;

 const int  Dindex  	   = 0;
 const int  Cindex  	   = 1;
 const int  MOVEindex      = 2;

 #define NUM_STATES  	     2

 const int STATES[NUM_STATES] = {D, C};

 #ifdef DIFFUSE
 #define NUM_ACTIONS 	   3
	const int ACTIONS[NUM_ACTIONS] = {D, C, MOVE};

 #else
 #define NUM_ACTIONS 	   2
	const int ACTIONS[NUM_ACTIONS] = {D, C};
 #endif

 const int STATE_INDEX[NUM_STATES] = {Dindex, Cindex};

 const float    SUCKER_PAYOFF = 0.0;
 const float    PUNISH_PAYOFF = 0.0;
 const float	REWARD_PAYOFF = 1.0;

 /****** Q-Learning parameteres **********/
 const double  EPSILON_MIN = 0.02;//0.1;
 const double  LAMBDA      = 0.02;
 const double  ALPHA       = 0.75;//0.75;
 const double  GAMMA       = 0.8;//0.75;

 /***************************************************************************
 *                      Variable Declarations                               *
 ***************************************************************************/

 float          TEMPTATION_PAYOFF;

 double         P_DIFFUSION;

 unsigned long  num_empty_sites;
 unsigned long  seed, numsteps, num_c, num_cd, num_dc, num_d;
 unsigned long  NUM_DEFECTS;

 /****** Q-Learning parameteres **********/
 double         EPSILON	    = .02;//1.0;
 double         ALPHA_SHARE = 0;

 /***************************************************************************
 *                      List Variable Declarations                          *
 ***************************************************************************/

 double         Q[LL][NUM_STATES][NUM_ACTIONS];
 double         number_coop_average[MEASURES], number_def_average[MEASURES], number_coop_to_def_average[MEASURES];
 double         average_Q_table[MEASURES][NUM_STATES][NUM_ACTIONS];

 int            s[LL];

 float          payoff[LL];

 long           t[MEASURES];
 unsigned long  right[LL], left[LL], top[LL], down[LL], neigh[LL][NUM_NEIGH];
 unsigned long  empty_matrix[LL], which_empty[LL];

 FILE           *freq, *fconf;

 unsigned long int rseed;
 const gsl_rng_type * T;
 gsl_rng * r;

/***************************************************************************
*                           Function Declarations                          *
***************************************************************************/
void file_initialization (void);
void initialization      (void);
void local_dynamics      (int *s, unsigned long *empty_matrix, unsigned long *which_emp);
void count_neighbours    (int *s, int ii, int *nc, int *nd);
void determine_neighbours(unsigned long neigh[][NUM_NEIGH]);
void initial_state       (int *s, int lsize, int initialstate, double probc, double probd);

double get_mean_neighbours_payoff(float *payoff, int *s, int index);
double pd_payoff                 (int *s, int state, int ii);

unsigned long empty_site(unsigned long ll, int *nn,
                         unsigned long *empty_matrix, unsigned long *which_empty);

void update_empty_sites(unsigned long s1, unsigned long s2,
                        unsigned long *which_empty, unsigned long *empty_matrix);

int odd(int x);


/***************************************************************************
*                    PSEUDO-RANDOM NUMBER GENERATOR                        *
***************************************************************************/
unsigned long int set_gsl_rng(void)
{
#ifdef DEBUG
  rseed=42;
#else
  rseed=time(NULL);
#endif

  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  gsl_rng_set (r, rseed);

  return rseed;
}

/***************************************************************************
*                            Simulation                                    *
***************************************************************************/
extern void simulation(void)
{
    //indexes
	int config_index;
	int i, j, k, l, m;

	static int CONFIG_INDEX = 0;

	double epsilon_to_decay;

   while(CONFIG_INDEX < NUM_CONF) //FOR GLUT
   {
		for (config_index = 0; config_index < NUM_CONF; ++config_index)
		{
			fprintf(stdout, "Initializing configuration number %d\n", config_index);
			fflush(stdout);
			++CONFIG_INDEX;

			initialization();

			for (i = 0; i < MEASURES; ++i)
			{
				EPSILON = 1.;
				while (numsteps <= t[i])
				{
					epsilon_to_decay = EPSILON * exp(-LAMBDA * numsteps);
					EPSILON = EPSILON_MIN;
					if (epsilon_to_decay > EPSILON_MIN){
					   EPSILON = epsilon_to_decay;
					}

					local_dynamics(s, empty_matrix, which_empty);

					++numsteps;
				}
				number_coop_average[i]        += num_c;
				number_def_average[i]         += num_d;
				number_coop_to_def_average[i] += num_cd+num_dc;

				for (k = num_empty_sites; k < LL; ++k)
				{
					for(l = 0; l < NUM_STATES; ++l)
						for(m = 0; m < NUM_ACTIONS; ++m)
							average_Q_table[i][l][m] += Q[empty_matrix[k]][l][m];
				}

				if ((num_d == (LL - num_empty_sites)) || (num_c == (LL - num_empty_sites)))
				{
					for (j = i+1; j < MEASURES; ++j)
					{
						number_coop_average[j]        += num_c;
						number_def_average[j]         += num_d;
						number_coop_to_def_average[j] += 0.0;

						for(l=0; l<NUM_STATES; ++l)
							for(m=0; m<NUM_ACTIONS; ++m)
								average_Q_table[j][l][m] = average_Q_table[i][l][m];
					}
					break;
				}
			}
		}

		for(i = 0; i < MEASURES; ++i)
		{
			number_coop_average[i]        /= NUM_CONF;
			number_def_average[i]         /= NUM_CONF;
			number_coop_to_def_average[i] /= NUM_CONF;

			for(l = 0; l < NUM_STATES; ++l)
				for(m = 0; m < NUM_ACTIONS; ++m)
					average_Q_table[i][l][m] /= NUM_CONF;

			fprintf(freq, "%ld %.6f %.6f %.6f ", t[i], number_coop_average[i], number_def_average[i],
			        number_coop_to_def_average[i]);

			for(l = 0; l < NUM_STATES; ++l)
				for(m = 0; m < NUM_ACTIONS; ++m)
					fprintf(freq, "%.6f ", average_Q_table[i][l][m]);
			fprintf(freq, "\n");
		}
		fclose(freq);

}//GLUT END!!!
  return;
}
/***************************************************************************
 *                           Determine neighbours                          *
 ***************************************************************************/
void determine_neighbours(unsigned long neigh[][(int) NUM_NEIGH])
{
	int i;

	for(i = 0; i < LL; ++i)
	{
		neigh[i][0] = left[i];
		neigh[i][1] = right[i];
		neigh[i][2] = top[i];
		neigh[i][3] = down[i];
	}
	return;
}

/***********************************************************************
*                       Get neighbours patoff                          *
***********************************************************************/
double get_mean_neighbours_payoff(float *payoff, int *s, int index){
    int k;

    double total_payoff  = 0;
    double total_players = 0;

    for (k=0; k < NUM_NEIGH; ++k)
	{
        if (s[neigh[index][k]] != 0)
		{
			total_payoff += pd_payoff(s, s[neigh[index][k]], neigh[index][k]);
			total_players++;
		}
     }

    if (total_players != 0) {
        return total_payoff / total_players;
    }

    return 0;

}

/***************************************************************************
 *                          Random Diffusion                               *
 ***************************************************************************/
int rand_diffusion(int *s1, int *s, float p_diffusion,unsigned long *empty_matrix,unsigned long *which_empty)
{
	int    i, j, k, s2;

	if (FRANDOM1 < p_diffusion)
    {
		i    = (int)((double)(NUM_NEIGH) * FRANDOM1); // choose random direction
		s2   = neigh[*s1][i];

		if (! s[s2]) // test if chosen direction is empty, !0 = 1
		{
			s[s2]  = s[*s1]; // Change strategy
			s[*s1] = 0;

			payoff[s2]  = payoff[*s1]; // Change payoffs
			payoff[*s1] = 0.0;

			for(j = 0; j < NUM_STATES; ++j) // Change Q values
			{
				for(k = 0; k < NUM_ACTIONS; ++k)
				{
					Q[s2][j][k] = Q[*s1][j][k];
					Q[*s1][j][k] = 0.0;
				}
			}
			/*s1=empty_matrix[j]; neighborhood --> no-empty*/
			/*s2=j;  site --> empty*/
			update_empty_sites(*s1, s2, which_empty, empty_matrix); // emp ---> empty_matrix

			*s1 = s2; // Change position

			return 1;
		}
    }
  return 0;
}
/***************************************************************************
 *                           Local Combat                                  *
 ***************************************************************************/
void count_neighbours(int *s, int ii, int *nc, int *nd)
{
	int k;

	*nc = 0;
	*nd = 0;

#ifdef SELF_INTERACTION
	switch(s[ii])
	{
		case D: *nd = 1;
			break;
		case C: *nc = 1;
			break;
	}
#endif

   for (k = 0; k < NUM_NEIGH; ++k)
	{
       switch (s[neigh[ii][k]])
		{
			case  C: ++(*nc);
				break;
			case  D: ++(*nd);
				break;
		}
    }
   return;
 }

/***************************************************************************
 *                    Prisoner's Dilemma Payoffs                           *
 ***************************************************************************/
double pd_payoff(int *s, int state, int ii)
{
	int nc, nd;
	double pay = 0.0;

	count_neighbours(s, ii, &nc, &nd);

	switch (state)
	{
		case C: pay = REWARD_PAYOFF * nc + SUCKER_PAYOFF * nd;
				 break;
		case D: pay = TEMPTATION_PAYOFF * nc + PUNISH_PAYOFF * nd;
				 break;
		default: printf("ERROR: ss = %d numsteps = %ld %d %d\n", state, numsteps, nc, nd);
				 exit(1);
				 break;
	}

	return pay;
}

/***************************************************************************
 *                           Random choice                                 *
 ***************************************************************************/
void random_choice_cant_move(int site, int *new_action, int *new_action_index)
{
	// Chooses an integer in [0,NUM_ACTIONS)

	*new_action_index = (int) (FRANDOM1 * (NUM_ACTIONS - 1));
	*new_action = ACTIONS[*new_action_index];

	return;
}

/***************************************************************************
 *                       Maximum Q_value per row  when can't move          *
 ***************************************************************************/
void find_maximum_Q_value_cant_move(int chosen_site, int state_index, int *maxQ_action, int *maxQ_ind, double *maxQ)
{
	int i;

	//randomly choose initial maximum value in case of ties
	random_choice_cant_move(chosen_site, maxQ_action, maxQ_ind);
	*maxQ = Q[chosen_site][state_index][*maxQ_ind];

	// now find maximum
	for (i = 0; i < (NUM_ACTIONS - 1); ++i)
		if (Q[chosen_site][state_index][i] > *maxQ)
		{
			*maxQ     = Q[chosen_site][state_index][i];
			*maxQ_ind = i;
		}

	*maxQ_action = ACTIONS[*maxQ_ind];

	return;
}

/***************************************************************************
 *                           Random choice                                 *
 ***************************************************************************/
void random_choice(int site, int *new_action, int *new_action_index)
{
	// Chooses an integer in [0,NUM_ACTIONS)

	*new_action_index = (int) (FRANDOM1 * NUM_ACTIONS);
	*new_action = ACTIONS[*new_action_index];

	return;
}
/***************************************************************************
 *                       Maximum Q_value per row                           *
 ***************************************************************************/
void find_maximum_Q_value(int chosen_site, int state_index, int *maxQ_action, int *maxQ_ind, double *maxQ)
{
	int i;

	//randomly choose initial maximum value in case of ties
	random_choice(chosen_site, maxQ_action, maxQ_ind);
	*maxQ = Q[chosen_site][state_index][*maxQ_ind];

	// now find maximum
	for (i = 0; i < NUM_ACTIONS; ++i)
		if (Q[chosen_site][state_index][i] > *maxQ)
		{
			*maxQ     = Q[chosen_site][state_index][i];
			*maxQ_ind = i;
		}

	// *maxQ_state = (*maxQ_ind == Cindex ? C : D);
	*maxQ_action = ACTIONS[*maxQ_ind];

	return;
}
/***************************************************************************
 *                           Local Dynamics                                *
 ***************************************************************************/
void local_dynamics (int *s, unsigned long *empty_matrix, unsigned long *which_emp)
{
	int stemp[LL];
	int i,j,chosen_index, chosen_site;
	int initial_s_index, new_action_index;
	int initial_s;

	double reward, final_payoff, total_reward;
	double maxQ, new_maxQ;
	double total_payoff;
	int new_action, future_action, future_action_index;

	num_c  = 0;
	num_cd = 0;
	num_dc = 0;
	num_d  = 0;

	reward       = 0;
	final_payoff = 0;
	total_payoff = 0;
	total_reward = 0;

	for (i=num_empty_sites; i < LL; ++i)
		stemp[empty_matrix[i]] = s[empty_matrix[i]];

	for (j=0; j < LL; ++j)
    {
		chosen_index = (int)(num_empty_sites + FRANDOM1*(LL-num_empty_sites));
		chosen_site  = empty_matrix[chosen_index];

		initial_s = s[chosen_site];

		if  (initial_s != 0)
		{
			initial_s_index = (initial_s == C ? Cindex : Dindex);

			if (FRANDOM1 < EPSILON) //random
				random_choice(chosen_site, &new_action, &new_action_index);
			else // greedy
				find_maximum_Q_value(chosen_site, initial_s_index, &new_action, &new_action_index, &maxQ);

			if (new_action_index != MOVEindex)
			{
				find_maximum_Q_value(chosen_site, new_action_index, &future_action, &future_action_index, &new_maxQ);

				s[chosen_site] = new_action;

				double neighbours_payoff = get_mean_neighbours_payoff(payoff, s, chosen_site);

				final_payoff = pd_payoff(s, new_action, chosen_site);

				reward = ALPHA_SHARE * neighbours_payoff + (1 - ALPHA_SHARE) * final_payoff;

				// Q[chosen_site][initial_s_index][new_action_index] = (1.- ALPHA)*Q[chosen_site][initial_s_index][new_action_index]  + ALPHA*(final_payoff + GAMMA*new_maxQ);
				// This is equivalent to expression above:
				Q[chosen_site][initial_s_index][new_action_index] +=  ALPHA * (reward + GAMMA * new_maxQ
										- Q[chosen_site][initial_s_index][new_action_index]);

			}
			else // try to move
			{
			    #ifndef DIFFUSE
					printf("\nSomething's wrong, program shouldn't call MOVEindex when directive DIFFUSE is not present. \n");
					exit(1);
				#endif

				int moved = rand_diffusion(&chosen_site, s, P_DIFFUSION, empty_matrix, which_empty);
				// Chosen site possivelmente atualizado

				final_payoff = pd_payoff(s, initial_s, chosen_site);

				// this is done in order to differentiate when
				// the player moves or not
				// if it moves, update the q_value referent to moving
				// if not, update the value referent to mantaining its
				// previous state
				if (moved) {
					find_maximum_Q_value(chosen_site, initial_s_index, &future_action, &future_action_index, &new_maxQ);

					double neighbours_payoff = get_mean_neighbours_payoff(payoff, s, chosen_site);

					reward = ALPHA_SHARE * neighbours_payoff + (1 - ALPHA_SHARE) * final_payoff;

					Q[chosen_site][initial_s_index][new_action_index] +=  ALPHA * (reward + GAMMA * new_maxQ
											- Q[chosen_site][initial_s_index][new_action_index] );
				}

				// with this, cooperation increases
				// don't know why, but it makes it differ
				// with static case (?)

				/*else {
				    reward = initial_payoff;

					find_maximum_Q_value_cant_move(chosen_site, initial_s_index, &future_action, &future_action_index, &new_maxQ);

					Q[chosen_site][initial_s_index][initial_s_index] +=  ALPHA * (reward + GAMMA*new_maxQ
											- Q[chosen_site][initial_s_index][initial_s_index] );

				}*/

				//reward = final_payoff;

				//find_maximum_Q_value(chosen_site, initial_s_index, &future_action, &future_action_index, &new_maxQ);
			}
			payoff[chosen_index]  = final_payoff;
			total_payoff         += final_payoff;
			total_reward         += reward;
		} // if(s1!=0)
	}

	//printf("%f, %f\n", total_payoff / LL, total_reward / LL);

	for (i=num_empty_sites; i< LL; ++i)
	{
		int s1 = empty_matrix[i];

		switch (s[s1])
		{
			case C: {
						++num_c;
						if  (stemp[s1] == D) ++num_dc;
					 }	break;
			case D: {
						++num_d;
						if  (stemp[s1] == C) ++num_cd;

					} break;
		}
	}

#ifdef USEGFX
    if (numsteps % 1000 == 0)
	    view2d(LSIZE, s, numsteps, TOTALSTEPS, t, num_c, num_d, NUM_DEFECTS);
	//syst_return = system("sleep 1");
#endif

  return;
}


/***************************************************************************
*                               Main                                       *
***************************************************************************/
int main(int argc, char **argv)
{
    #ifdef DIFFUSE
    	if (argc != 5)
    	{
    		printf("\nThe program must be called with 4 parameters, T, NUM_DEFECTS, P_DIFFUSION and ALPHA_SHARE\n");
    		exit(1);
    	}
    	else
        {
    		TEMPTATION_PAYOFF = atof(argv[1]);
    		NUM_DEFECTS       = atof(argv[2]);
    		P_DIFFUSION       = atof(argv[3]);
            ALPHA_SHARE       = atof(argv[4]);
    	}
	#else
    	if (argc != 4)
    	{
    		printf("\nThe program must be called with 3 parameters, T, NUM_DEFECTS and ALPHA_SHARE \n");
    		exit(1);
    	}
    	else
        {
    		TEMPTATION_PAYOFF = atof(argv[1]);
    		NUM_DEFECTS       = atof(argv[2]);
    		ALPHA_SHARE       = atof(argv[2]);
    	}
	#endif

	seed = set_gsl_rng();	 //Start GSL Random number generator

	create_time_table_2(t, TOTALSTEPS, MEASURES);
	neighbours_2d(right, left, top, down, LSIZE);
	determine_neighbours(neigh);

	file_initialization();

#ifdef USEGFX
	char window_title[50];
	sprintf(window_title, "Prisoners Dilemma - Q learning");
	initialize_glut_display(simulation, &argc, argv, window_title);
#else
	simulation();
#endif

	//printf("Done initialize_glut_display\n"); fflush(stdout);
  return 0;
}

/***************************************************************************
 *                        File Initialization                              *
 ***************************************************************************/
void file_initialization(void)
{
	char output_file_freq[200];
	int i,j,k;

	sprintf(output_file_freq,"data/T%.2f_S_%.2f_LSIZE%d_rho%.5f_ALPHA_SHARE%.2f_P_DIFFUSION%.2f_CONF_%d_%ld_prof.dat",
                              TEMPTATION_PAYOFF, SUCKER_PAYOFF, LSIZE, 1.0 - NUM_DEFECTS / ((float) LL), ALPHA_SHARE,
                              P_DIFFUSION, NUM_CONF, seed);

	freq = fopen(output_file_freq,"w");

	fprintf(freq,"# Diffusive and Diluted Spatial Games - 2D ");//- V%s\n",VERSION);
	fprintf(freq,"# Lattice: %d x %d = %d\n",LSIZE,LSIZE,LL);
	fprintf(freq,"# Random seed: %ld\n",seed);
	fprintf(freq,"# N_CONF = %d \n",NUM_CONF);
	fprintf(freq,"# TEMPTATION_PAYOFF = %5.3f\n", TEMPTATION_PAYOFF);
	fprintf(freq,"# REWARD_PAYOFF = %5.3f\n", REWARD_PAYOFF);
	fprintf(freq,"# SUCKER_PAYOFF = %5.3f\n", SUCKER_PAYOFF);

	fprintf(freq,"# rho = %.4f\n",1.0-NUM_DEFECTS/((float)LL));
	fprintf(freq,"# Num defects = %ld \n",NUM_DEFECTS);
	fprintf(freq,"# Initial prob(c) = %5.4f\n",PROB_C);
	fprintf(freq,"# Initial prob(d) = %5.4f\n",PROB_D);
#ifdef DIFFUSE
	fprintf(freq,"# Prob diffusion = %5.4f\n",P_DIFFUSION);
#endif

	fprintf(freq,"# GAMMA   = %5.3f\n", GAMMA);
	fprintf(freq,"# LAMBDA  = %5.3f\n", LAMBDA);
	fprintf(freq,"# EPSILON = %5.3f\n", EPSILON);
	fprintf(freq,"# EPSILON_MIN = %5.3f\n", EPSILON_MIN);
	fprintf(freq,"# NUM_STATES  = %d\n", NUM_STATES);
	fprintf(freq,"# NUM_ACTIONS = %d\n", NUM_ACTIONS);
	fprintf(freq,"# ALPHA_SHARE = %5.3f\n", ALPHA_SHARE);

	fprintf(freq,"# Initial configuration: ");
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
	fprintf(freq,"# Steps: Total = %5d\n",TOTALSTEPS);

	//fprintf(freq,"#t  f_c  f_d  f_ac  Qcc  Qcd Qdc Qdd P\n");

	#ifdef DIFFUSE
	   fprintf(freq,"#t  f_c  f_d  f_ac Qdd  Qdc Qdm Qcd Qcc Qcm\n");
	#else
	   fprintf(freq,"#t  f_c  f_d  f_ac Qdd  Qdc Qcd Qcc\n");
	#endif

	for (i=0;i<MEASURES;++i)
	{
		number_coop_average[i]  = 0.0;
		number_def_average[i]   = 0.0;

		number_coop_to_def_average[i] = 0.0;

		for(j = 0; j < NUM_STATES; ++j)
			for(k = 0; k < NUM_ACTIONS; ++k)
				average_Q_table[i][j][k] = 0.0;
	}

	return;
}
/***************************************************************************
 *                      Environment Initialization                         *
 ***************************************************************************/
void initialization(void)
{
	int i,j,k;

	numsteps  = 0;
	num_c     = 0;
	num_d     = 0;
	num_cd    = 0;

	initial_state(s, LSIZE, INITIALSTATE, PROB_C, PROB_D);

	num_empty_sites = empty_site(LL, s, empty_matrix, which_empty); //ja existia uma matriz which_empty

	for (i=0; i < LL; ++i)
    {
		payoff[i] = 0.0;

		switch(s[i])
		{
			case C: ++num_c;
				break;
			case D: ++num_d;
				break;
		}

		for(j = 0; j < NUM_STATES; ++j)
		{
			for(k = 0; k < NUM_ACTIONS; ++k)
				Q[i][j][k] = 0.0;//FRANDOM1;
		}
	}
	fflush(freq);

#ifdef USEGFX
	int syst_return;

	view2d(LSIZE, s, numsteps, TOTALSTEPS, t, num_c, num_d, NUM_DEFECTS);
	syst_return = system("sleep 0.5");

	if (syst_return == 1000)
		printf("Uot!\n");
#endif

  return;
}
/***********************************************************************
 *                            Initial State                             *
 ***********************************************************************/
void initial_state(int *s,  int lsize, int initialstate, double probc, double probd)

{
  int i, l2, vazios;
  double sample_random;

  l2 = lsize * lsize;
  switch (initialstate)
    {
    case 1 : for (i = 0; i < l2; ++i)
	{
	  *(s+i) = 0;
	  sample_random = FRANDOM1;
	  if (sample_random<probc) *(s+i) = 1;
	  else if (sample_random < (probc+probd)) *(s+i) = D;
	}
      break;
    case 2 : for (i = 0; i < l2; ++i)
	{
	  *(s+i) = 0;
	  sample_random = FRANDOM1;
	  if (((!odd(lsize) && i==(l2+lsize)/2))
	      || ((odd(lsize)) && (i == l2/2) ))
	    *(s+i) = D;
	  else if (sample_random<probc) *(s+i) = C;
	}
      break;
    case 3 : for (i = 0; i < l2; ++i)
	{
	  *(s+i) = 0;
	  /*if ((i==(l2+lsize)/2) || (i==(l2+lsize)/2 +1) ||
	      (i==(l2+lsize)/2 -lsize) || (i==(l2+lsize)/2 +lsize) )
	    *(s+i) = 1;
	    else *(s+i)=-1;*/
	  /*  if ((i==(l2+lsize)/2) || (i==(l2+lsize)/2 +1) ||
		(i==(l2+lsize)/2 -lsize) ||
		(i==(l2+lsize)/2-lsize+1))
	      *(s+i) = 1;
	      else *(s+i) = -1;*/
	  sample_random = FRANDOM1;
	  if ((i==(l2+lsize)/2) || (i==(l2+lsize)/2 +1) ||
	      (i==(l2+lsize)/2 -lsize) ||
	      (i==(l2+lsize)/2-lsize+1))
	    *(s+i) = -1;
	  else if (sample_random<probc) *(s+i) = 1;
	}
      break;
    case 4 : for (i = 0; i < l2; ++i)
			{
				*(s+i) = 1;
			}
      vazios=0;
      while (vazios < (int) NUM_DEFECTS)
		{
			i= (int) (FRANDOM1*l2);
			if (*(s+i)!=0)
				{
					*(s+i)=0;
					++vazios;
				}
		}
	  for (i = 0; i < l2; ++i)
		{
			if (*(s+i)!=0)
			{
				sample_random = FRANDOM1;
				if (sample_random<(probd/(probc+probd)))
				{
					*(s+i) = -1;
				}
	    }
	}
      break;
    case 5 :for (i = 0; i < l2; ++i)
			{
				*(s+i) = -1;
			}
      *(s+l2/2)=1;
      *(s+l2/2+lsize)=1;
      /**/ i=(int) (FRANDOM1*l2);
       *(s+i)=1;
       *(s+i+1)=1;
       vazios=0;
       while (vazios < (int) (l2*(1.-PROB_C-PROB_D)))
         {
           i= (int) (FRANDOM1*l2);
           if ( ( *(s+i)!=0 ) && ( *(s+i)!=1 ) )
             {
               *(s+i)=0;
               ++vazios;
             }
	    }/**/
      break;
    }
return;
}

/*********************************************************************
***                  Number and Position of Empty Sites            ***
***                      Last modified: 30/05/1999                 ***
*********************************************************************/
unsigned long empty_site(unsigned long ll, int *nn,
                         unsigned long *empty_matrix, unsigned long *which_emp)
{
	unsigned long i,k;

	k = 0;
	for (i=0; i<ll; ++i)     /* Empty sites position, starting from k==0 */
	{
		if (nn[i]==0)
		{                      /* nn represents the state */
			empty_matrix[k] = i; /* lattice number corresponding to empty site */
			which_emp[i] = k;    /* position of the empty site in the emp matrix*/
			++k;
		}
		else
		{ /* sites which are not empty are placed at the end of matrix */
			empty_matrix[ll-1-i+k] = i;
			which_emp[i]           = ll-1-i+k;
		}
    }
  return k;
}
/*********************************************************************
 ***                       Update Empty_Sites Matrix                ***
 ***                       Last modified: 24/04/1999                ***
 *********************************************************************/
void update_empty_sites(unsigned long s1, unsigned long s2,
                        unsigned long *which_empty, unsigned long *empty_matrix)
{
	unsigned long sitetmp;

	empty_matrix[which_empty[s1]] = s2;
	empty_matrix[which_empty[s2]] = s1;
	sitetmp = which_empty[s1];
	which_empty[s1] = which_empty[s2];
	which_empty[s2] = sitetmp;

	return;
}

/***************************************************************************
*                              Odd                                         *
***************************************************************************/
int odd(int x)
{
	return (x%2);
}
