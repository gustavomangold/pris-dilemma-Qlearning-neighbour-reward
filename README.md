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
+  &#8594;
+  &#8594;
+  &#8594;
+  &#8594;
+  &#8594;
+  &#8594;
+  &#8594;
+  &#8594;
+  &#8594;
