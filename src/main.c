/* dic - a cli dice calculator - main.
 * Copyright (C) 2023  hcjimmy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "libdie/libdie.h"
#include "libdie/string_ops.h"
#include "flagi/flagi.h"

#include "print_dierrors.h"
#include "out_colors.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define OTHER_ERRORS_EXIT	(4)
#define INVALID_ARGS		(3)
#define FLAG_ERROR_EXIT 	(2)
#define EXP_ERROR_EXIT		(1)
#define NO_ERROR_EXIT		(0)
#define OOM_EXIT		(-1)

#define OOM_MSG	"Error: out-of-memory...\n"\
		"\tUnless you're running on very-low ram, or somehow wasted all of it, then this is probably an internal bug\n"\
		"\t(the program errenously thinks we run of memory) since it's very uncommon for it to actually happen.\n"\
		"Please report this so I could quickly fix the problem.\n"

#define SEE_HELP "See -h or --help for help.\n"

#ifdef EXPAND_DEFAULT
	#define DEF_EXAPAND_STR " (default)"
	#define DEF_COLLPSE_STR ""
#else
	#define DEF_EXAPAND_STR ""
	#define DEF_COLLPSE_STR " (default)"
#endif

#define HELP_MSG "dic - a dice calculator for TTRPGs.\n"\
"\n"\
"Usage:\n"\
"  dic <dice-expression>\n"\
"  dic [options] <args> <dice-expression>\n"\
"\n"\
"Options:\n"\
"  -h    --help            Show this help message.\n"\
"  -r    --repeat <n>      Repeat the calculation n times.\n"\
"  -A    --advantage       Roll twice and take the higher (show the lower greyed-out).\n"\
"  -D    --disadvantage    Roll twice and take the lower (show the higher greyed-out).\n"\
"  -c    --collapse        Don't show all the dice-rolls if a die is operated by not +-" DEF_COLLPSE_STR ".\n"\
"                          E.g. dic 2*3d4 will give 2*7 = 14  instead of  2*(2+2+3) = 14.\n"\
"  -x    --expand          Always show all the dice rolls (opposite of --collpase)" DEF_EXAPAND_STR ".\n"\
"  -q    --quiet           Don't show the calculation, just the result.\n"\
"  -H    --hide-grey       Hide the greyed-out output of --disadvantage or --advantage.\n"\
"  -Q    --really-quiet    Don't print the calculation or the greyed-out output.\n"\
"                          Equivalent to giving both -H and -q.\n"\
"\n"\
"Examples:\n"\
"  dic 2d4                 Roll a 4-sided die 2 times.\n"\
"  dic d20+3               Roll a 20-sided die 1 time and add 3.\n"\
"  dic d20+3 -r 4          Calculate d20+3, 4 times, and print the results seperately.\n"\
"  dic -r d4 d20-d6        Calculate d20-d6, 1d4 times.\n"\
"\n"\
"    Allowed operators: +-/*%()[]{}\n"


#define EPS 0.0000001
#define DBL_EQUALS(dbl1, dbl2) (\
		((dbl1) <= (dbl2) + EPS) && ((dbl1) >= (dbl2) - EPS)\
		)

#ifdef _WIN32
#include <windows.h>
#include <sysinfoapi.h>

void seed_rand()
{
	srand(GetTickCount());
}

/* For some reason I get "undefined referece to stpcpy" when compiling
 * with mingw64 on Linux for windows... */
char *stpcpy(char *restrict dst, const char *restrict src)
{
	while((*dst = *src))
		++dst, ++src;
	return dst;
}

#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
void seed_rand()
{
	FILE* urand;
	unsigned seed;

	urand = fopen("/dev/urandom", "r");
	if(!urand) {
		perror("Failed to access /dev/urandom");
		exit(OTHER_ERRORS_EXIT);
	}

	if(fread(&seed, sizeof(seed), 1, urand) != 1) {
		perror("Failed to read /dev/urandom");
		fclose(urand);
		exit(OTHER_ERRORS_EXIT);
	}

	srand(seed);
	if(fclose(urand)) {
		perror("Error with file /dev/urandom");
		exit(OTHER_ERRORS_EXIT);
	}
}

#else

#include <assert.h>
static_assert(false, "Don't know this platform. Contact me so I'll fix the issue...\n");

#endif

/* Roll repetitions from repeat_string and return it as size_t.
 * If repeat_string is NULL, return 1. */
size_t get_repeates(char* repeat_string)
{
	if(!repeat_string)
		return 1;

	struct Operation *operation;
	struct Dierror *dierrors;

	size_t repeats;

	operation = exp_to_op(repeat_string, &dierrors);

	if(operation) {
		bool error;
		double double_repeats;

		double_repeats = operate(operation, NULL, NO_FLAG);
		clear_operation_pointer(operation);

		// Check for errors on the returned value.
		if(DBL_EQUALS(double_repeats, 0.0)) {
			fputs("As requested: we're repeating 0 times.\n", stderr);
			exit(INVALID_ARGS);	// (The other errors can't happen in this case...)
		}

		if((error = (double_repeats < 0.0)))
			fputs("Error: cannot repeat negative amount of times...\n", stderr);

		const double fmod_one = fmod(double_repeats, 1.0);
		if(!DBL_EQUALS(fmod_one, 0.0)) {
			fputs("Error: can't repeat non-integer amount of times...\n", stderr);
			error = true;
		}

		if(error)
			exit(INVALID_ARGS);

		repeats = (size_t) double_repeats;
		return repeats;
	}

	if(!dierrors) {
		fputs(OOM_MSG, stderr);
		exit(OOM_EXIT);
	}

	fprint_dierrors_colored(stderr, COLOR_RED, "Invalid repetitions",
			repeat_string, dierrors);
	free(dierrors);
	exit(INVALID_ARGS);
}

/* Cocatenate n elements from string array into dest buffer. */
void concatenate_n_str_array(char *dest, char **string_array, size_t n)
{
	for(; n--; ++string_array)
		dest = stpcpy(dest, *string_array);

}

// Assumes *argv != NULL.
char* make_exp(char **argv, bool *isflag)
{
	char *expression;
	size_t exp_buf_count;
	size_t exp_element_count;

	// Get the start of expression.
	while(*isflag) {
		++isflag, ++argv;
		if(*argv == NULL) {
			fputs("Error: missing dice-expression. " SEE_HELP, stderr);
			exit(INVALID_ARGS);
		}
	}
	char **argv_exp_start = argv;

	// Get it's length (both number of elements in argv and buffer size).
	exp_buf_count = strlen(*argv) +1;
	exp_element_count = 1;

	while(*++argv != NULL) {

		if(*++isflag) {
			// Found flag after exp, if there's a non-flag after it, we have multiple expressions...
			while(*++argv != NULL) {
				if(!*++isflag) {
					fputs("Error: multiple dice-expressions found. " SEE_HELP, stderr);
					exit(INVALID_ARGS);
				}
			}
			break;
		}

		exp_buf_count += strlen(*argv);
		exp_element_count++;
	}

	expression = malloc(exp_buf_count * sizeof(*expression));
	if(!expression) {
		fputs(OOM_MSG, stderr);
		exit(OOM_EXIT);
	}

	concatenate_n_str_array(expression, argv_exp_start, exp_element_count);
	return expression;
}


void fprint_result(FILE* fp, char *calc_string, double result, bool quiet, bool greyed_out)
{
#define RESULT_PRESICIION 9

	if(greyed_out)
		fputs(COLOR_STRIKE_THROUGH COLOR_GREY, fp);

	char *result_buffer = alloca((snprintf(NULL, 0, "%." STRINGER(RESULT_PRESICIION) "lf", result) + 1)
					* sizeof((*result_buffer)));
	if(!quiet) {
		fputs(calc_string, fp);
		fputs(" = ", fp);
	}

	stringify_double(result, RESULT_PRESICIION, result_buffer);
	fputs(result_buffer, fp);

	if(greyed_out)
		fputs(COLOR_RESET, fp);

	fputc('\n', fp);
}

void print_result(char *calc_string, double result, bool quiet, bool greyed_out)
{
	fprint_result(stdout, calc_string, result, quiet, greyed_out);
}

// Temporary macros for clarification.
#define STRIKE_THROUGH true
#define DONT_STRIKE false

void print_disadvantage(char *calc_string1, double result1,
		char* calc_string2, double result2,
		bool quiet, bool hide_greyed)
{
	if(result1 > result2) {
		print_result(calc_string2, result2, quiet, DONT_STRIKE);
		if(!hide_greyed)
			print_result(calc_string1, result1, quiet, STRIKE_THROUGH);
	} else {
		print_result(calc_string1, result1, quiet, DONT_STRIKE);
		if(!hide_greyed)
			print_result(calc_string2, result2, quiet, STRIKE_THROUGH);
	}

}

void print_advantage(char *calc_string1, double result1,
		char* calc_string2, double result2,
		bool quiet, bool hide_greyed)
{
	if(result1 < result2) {
		print_result(calc_string2, result2, quiet, DONT_STRIKE);
		if(!hide_greyed)
			print_result(calc_string1, result1, quiet, STRIKE_THROUGH);
	} else {
		print_result(calc_string1, result1, quiet, DONT_STRIKE);
		if(!hide_greyed)
			print_result(calc_string2, result2, quiet, STRIKE_THROUGH);
	}

}

#undef STRIKE_THROUGH
#undef DONT_STRIKE

int roll_exp_inner(struct Operation *operation, char *dice_exp, size_t repeats,
		const bool expand, const bool disadvantage, const bool advantage,
		const bool quiet, const bool hide_greyed)
{
	double result1;
	double result2;

	char* calc_string1;
	char* calc_string2;

	// Allocate calculation string buffers if needed.
	if(!quiet) {
		calc_string1 = malloc(get_calc_string_length(operation) * sizeof(*calc_string1));
		if(!calc_string1)
			return OOM_EXIT;
		if((disadvantage || advantage)) {
			calc_string2 = malloc(get_calc_string_length(operation) * sizeof(*calc_string2));
			if(!calc_string2) {
				free(calc_string1);
				return OOM_EXIT;
			}
		} else {
			calc_string2 = NULL;
		}
	} else {
		calc_string1 = calc_string2 = NULL;
	}

	do {
		result1 = operate(operation, calc_string1, (expand) ? NO_FLAG : COLLAPSE_DICE);
		if(disadvantage) {
			result2 = operate(operation, calc_string2, (expand) ? NO_FLAG : COLLAPSE_DICE);
			print_disadvantage(calc_string1, result1, calc_string2, result2, quiet, hide_greyed);
		} else if(advantage) {
			result2 = operate(operation, calc_string2, (expand) ? NO_FLAG : COLLAPSE_DICE);
			print_advantage(calc_string1, result1, calc_string2, result2, quiet, hide_greyed);

		} else {
			print_result(calc_string1, result1, quiet, false);
		}
	} while(--repeats);

	free(calc_string1);
	free(calc_string2);

	return EXIT_SUCCESS;
}

int roll_exp(char *dice_exp, size_t repeats,
		bool expand, bool disadvantage, bool advantage,
		bool quiet, bool hide_greyed)
{
	struct Operation *operation;
	struct Dierror *errors;

	int return_value;

	operation = exp_to_op(dice_exp, &errors);
	if(!operation) {
		if(!errors) {
			fputs(OOM_MSG, stderr);
			return OOM_EXIT;
		}

		fprint_dierrors_colored(stderr, COLOR_RED,
				(errors[1].type == end_of_list) ? "Error" : "Errors",
				dice_exp, errors);
		free(errors);
		return EXP_ERROR_EXIT;
	}

	if(!quiet && is_single_num_operation(operation))
		quiet = true;

	return_value = roll_exp_inner(operation, dice_exp,
			repeats, expand, disadvantage, advantage, quiet, hide_greyed);
	if(return_value == OOM_EXIT)
		fputs(OOM_MSG, stderr);

	clear_operation_pointer(operation);
	return return_value;

}

void get_flags(char **argv, bool **isflag,
		bool *help, bool *expand, bool *disadvantage, bool *advantage,
		bool *quiet, bool *really_quiet, bool *hide_greyed,
		char **repeat_string)
{

	// Set default values.
	*help = *disadvantage = *advantage = *quiet = *really_quiet = *hide_greyed = false;
	*repeat_string = NULL;

#ifdef EXPAND_DEFAULT
	*expand = true;
#else
	*expand = false;
#endif

	BoolOneFlag bool_ones[] = {
		{ 'h', help },
		{ 'x', expand },
		{ 'D', disadvantage },
		{ 'A', advantage },
		{ 'q', quiet },
		{ 'Q', really_quiet },
		{ 'H', hide_greyed },
		{ '\0' }
	};

	ArgOneFlag arg_ones[] = {
		{ 'r', repeat_string },
		{ '\0' }
	};

	BoolTwoFlag bool_twos[] = {
		{ "help", help },
		{ "expand", expand },
		{ "disadvantage", disadvantage },
		{ "advantage", advantage },
		{ "quiet", quiet },
		{ "really_quiet", really_quiet },
		{ "hide-grey", hide_greyed },
		{ NULL }
	};

	ArgTwoFlag arg_twos[] = {
		{ "repeat", repeat_string },
		{ NULL }
	};

	BoolOneFlag negative_ones[] = {
		{ 'c', expand },
		{ '\0' }
	};

	BoolTwoFlag negative_twos[] = {
		{ "collapse", expand },
		{ NULL }
	};

	if(flag_ignore(argv, *isflag, bool_ones, bool_twos, arg_ones, arg_twos, negative_ones, negative_twos)) {
		fputs(" " SEE_HELP, stderr);
		exit(FLAG_ERROR_EXIT);
	}
}

int main(int argc, char **argv)
{
	bool help, expand, disadvantage, advantage, quiet;
	bool really_quiet, hide_greyed;

	size_t repeats;
	char *repeat_string;

	char *dice_exp;

	int return_value;

	if(argc <= 1) {
		fputs("Missing arguments. " SEE_HELP, stderr);
		return INVALID_ARGS;
	}

	++argv;

	bool *isflag = alloca((argc-1) * sizeof(*isflag));

	get_flags(argv, &isflag,
			&help, &expand, &disadvantage,
			&advantage, &quiet, &really_quiet,
			&hide_greyed, &repeat_string);

	if(help) {
		fputs(HELP_MSG, stdout);
		exit(EXIT_SUCCESS);
	}

	if(disadvantage && advantage) {
		fputs("Error: can't roll with both disadvantage and advantage.\n", stderr);
		exit(FLAG_ERROR_EXIT);
	}

	if(really_quiet)
		quiet = hide_greyed = true;

	seed_rand();
	repeats = get_repeates(repeat_string);
	dice_exp = make_exp(argv, isflag);

	return_value = roll_exp(dice_exp, repeats, expand, disadvantage, advantage, quiet, hide_greyed);
	free(dice_exp);
	exit(return_value);

}

