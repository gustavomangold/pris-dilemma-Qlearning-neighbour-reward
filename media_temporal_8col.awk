# Comments:
# The evaluated errors are sample to sample errors.
# Data lines starting with '#' are comments.
#
# Command line:  awk -f media_temporal.awk input
#
# V0.01 - 10/05/2002 - MHV
# If not a comment, read record:
BEGIN {
      media1=0;
      media2=0;
      media3=0;
      media4=0;
      media5=0;
      media6=0;
      media7=0;
      media8=0;
      }
{
 if ($1 !~ /#/)
       {
	if ($1 > INITIAL_TIME)
	{
# The number of columns: should be constant, but since some lines can
# be empty...
     	 media1  += $1;
	 media2  += $2;
	 media3  += $3;
      	 media4  += $4;
      	 media5  += $5;
      	 media6  += $6;
      	 media7  += $7;
      	 media8  += $8;

	 ++i;
	}
      }
}
END {
# The integer division gives the number of lines (incomplete lines
# are neglected):
               printf("%15.8f\t %15.8f %15.8f\t %15.8f %15.8f\t %15.8f %15.8f\t \n",media2/i,media3/i,media4/i,media5/i,media6/i,media7/i,media8/i);
    }


