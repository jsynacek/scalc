/* scalc -- stream calculator
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Written by Jan Synacek <jsynacek@redhat.com>.
*/

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROGRAM_NAME "scalc"

#define AUTHORS \
  proper_name_utf8 ("Jan Synacek", "TODO")

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

/* Maximum size of accumulator arrays. 65536 with a 16-byte long double. */
const size_t MAX_ACC = 1024 * 1024 / sizeof (long double);

static void
usage (const char *program_name)
{
  printf ("Usage: %s [OPTIONS] [FILE]\n"
	  "Simple stream calculator.\n\n"
	  "  -a        compute arithmetic mean\n"
	  "  -p        compute product\n"
	  "  -s        compute sum (default)\n"
	  "  -d        number of digits after decimal point (default: 0)\n"
	  "  -h        print help\n",
	  program_name);
  exit (EXIT_FAILURE);
}

static void
die (const char *err)
{
  fputs (err, stderr);
  exit (EXIT_FAILURE);
}

static void
sum (long double *result, const long double x, long double *unused)
{
  *result += x;
}

static void
product (long double *result, const long double x, long double *unused)
{
  *result *= x;
}

static void
minimum (long double *result, const long double x, long double *unused)
{
  *result = min (*result, x);
}

static void
maximum (long double *result, const long double x, long double *unused)
{
  *result = max (*result, x);
}

static long double
_mean (long double *arr, size_t n)
{
  long double sum = 0;
  size_t i = 0;

  while (i < n)
    sum += arr[i++];

  return sum / n;
}

/* FIXME: rewrite *without* requiring to keep all numbers */
static void
average (long double *result, const long double x, long double *acc)
{
  static unsigned int i = 0;

  acc[i++] = x;
  *result = _mean (acc, i);
}

/* FIXME: rewrite *without* requiring to keep all numbers */
static void
standard_deviation (long double *result, const long double x, long double *acc)
{
  static unsigned int i = 0;
  size_t j = 0;
  long double mean, sum = 0;

  acc[i++] = x;
  mean = _mean (acc, i);

  while (j < i)
    {
      long double val = acc[j++] - mean;

      sum += val * val;
    }

  *result = sqrt (sum / i);
}

int
main (int argc, char **argv)
{
  long double result = 0;
  long double acc[MAX_ACC];
  void (*operation) (long double *, const long double, long double *) = sum;
  int opt;
  int precision = 0;

  int exit_status = EXIT_SUCCESS;

  while ((opt = getopt (argc, argv, "spixatd:h")) != -1)
    {
      switch (opt) {
      case 's':
	operation = sum;
	break;
      case 'p':
	operation = product;
	result = 1;
	break;
      case 'i':
        operation = minimum;
        break;
      case 'x':
        operation = maximum;
        break;
      case 'a':
	operation = average;
	break;
      case 't':
        operation = standard_deviation;
        break;
      case 'd':
	precision = atoi (optarg);
	break;
      case 'h':
	usage (argv[0]);
	break;
      default:
	fprintf (stderr, "Try '%s -h' for more information.\n", argv[0]);
	exit (EXIT_FAILURE);
      }
    }

  while (true)
    {
      int rc;
      long double x;

      rc = scanf("%Lf", &x);
      if (rc == 0)
	{
	  exit_status = EXIT_FAILURE;
	  break;
	}
      else if (rc == EOF)
	{
	  break;
	}
      else
	{
	  operation (&result, x, acc);
	}
    }

  if (exit_status == EXIT_SUCCESS)
    {
      char fmt[64];

      snprintf (fmt, 63, "%%.%dLf\n", precision);
      printf (fmt, result);
    }

  return exit_status;
}

/* vim: set cinoptions=>4,n-2,{2,^-2,\:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1 sw=2 ts=8 et: */
