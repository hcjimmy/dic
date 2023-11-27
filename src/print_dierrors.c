/* dic's error printing.
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
#include "print_dierrors.h"
#include "out_colors.h"
#include <stdio.h>

/* -- Error printing -- */

void fprint_c_xtimes(FILE* fp, char c, size_t x)
{
	while(x-- > 0)
		fputc(c, fp);
}

#define GET_DIERROR_TYPE_AS_STR_LITERAL(derror_type) (\
	  ((derror_type) == invalid_operator) ? "invalid operator"\
	: ((derror_type) == invalid_num) ? "invalid number"\
	: ((derror_type) == missing_num) ? "missing number"\
	: ((derror_type) == invalid_reps) ? "invalid number of rolls"\
	: ((derror_type) == zero_reps) ? "can't roll 0 times..."\
	: ((derror_type) == invalid_sides) ? "invalid number of sides"\
	: ((derror_type) == non_existant_sides) ? "missing number of sides"\
	: ((derror_type) == zero_sides) ? "can't roll 0-sided die"\
	: ((derror_type) == unclosed_parenthesis) ? "unclosed parenthesis"\
	: ((derror_type) == invalid_parenthesis) ? "invalid parenthesis"\
	: "I can't roll nuthin'")	// == empty_expression

#define GET_DIERROR_TYPE_AS_STR_LITERAL_CAPITAL_START(derror_type) (\
	  ((derror_type) == invalid_operator) ? "Invalid operator"\
	: ((derror_type) == invalid_num) ? "Invalid number"\
	: ((derror_type) == missing_num) ? "Missing number"\
	: ((derror_type) == invalid_reps) ? "Invalid number of rolls"\
	: ((derror_type) == zero_reps) ? "Can't roll 0 times..."\
	: ((derror_type) == invalid_sides) ? "Invalid number of sides"\
	: ((derror_type) == non_existant_sides) ? "Missing number of sides"\
	: ((derror_type) == zero_sides) ? "Can't roll 0-sided die"\
	: ((derror_type) == unclosed_parenthesis) ? "Unclosed parenthesis"\
	: ((derror_type) == invalid_parenthesis) ? "Invalid parenthesis"\
	: "I can't roll nuthin'")	// == empty_expression

/* Print dice_exp with the errenous section colored as error_color.
 * Prerequisites:
 * 	dierror->invalid_section_start != NULL.
 * 	*dierror->invalid_section_start != '\0'. */
void dierror_print_exp_colored(FILE* fp, char *error_color, char *dice_exp, struct Dierror *dierror)
{

	while(dice_exp < dierror->invalid_section_start)
		fputc(*dice_exp++, fp);

	fputs(error_color, fp);
	while(dice_exp < dierror->invalid_section_end)
		fputc(*dice_exp++, fp);
	fputs(COLOR_RESET, fp);

	while(*dice_exp != '\0')
		fputc(*dice_exp++, fp);

}

/* Print dierror in the form of: "<explanation>\t<error-colored dice-exp>\n" */
void dierror_print_colored(FILE *fp, char* error_color, char *dice_exp, Dierror *dierror, bool capital_start)
{
	if(capital_start)
		fputs(GET_DIERROR_TYPE_AS_STR_LITERAL_CAPITAL_START(dierror->type), fp);
	else
		fputs(GET_DIERROR_TYPE_AS_STR_LITERAL(dierror->type), fp);

	if(dierror->invalid_section_start == NULL || *dierror->invalid_section_start == '\0') {
		fputc('.', fp);
	} else {
		fputc('\t', fp);
		dierror_print_exp_colored(fp, error_color, dice_exp, dierror);
	}

	fputc('\n', fp);
}

void fprint_dierrors_colored(FILE* fp, char* error_color, char* prefix, char *dice_exp, Dierror *dierror_array)
{
	fputs(prefix, fp);

	if(dierror_array->type == end_of_list) {
		fputc('.', fp);
		fputc('\n', fp);
		return;
	}

	fputc(':', fp);
	if(dierror_array[1].type == end_of_list) {
		fputc(' ', fp);
		dierror_print_colored(fp, error_color, dice_exp, dierror_array, false);
		return;
	}

	fputc('\n', fp);
	fputc('\t', fp);
	dierror_print_colored(fp, error_color, dice_exp, dierror_array++, true);

	do {
		fputc('\t', fp);
		dierror_print_colored(fp, error_color, dice_exp, dierror_array++, true);
	} while(dierror_array->type != end_of_list);


}
