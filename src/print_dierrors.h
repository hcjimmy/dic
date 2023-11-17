/* dic's error printing - header.
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
#include <stdio.h>

#include "libdie/libdie.h"


/* fprint_dierrors_colored - Print Dierrors from array to fp, with the errenous section
 * colored error_color.
 *
 * prefix		Message written once at start.
 * dice_exp		The dice_exp that was proccessed.
 * dierror_array	Array of dierrors, terminated by error type end_of_list.
 *
 * pre:
 * 	prefix != NULL.
 * 	dierror_array != NULL.
 * 	dierror_array is returned from exp_to_op.
 * 	dice_exp is the value given to exp_to_op.
 * 	error_color is anything specified at out_colors.h
 *
 * post:
 * 	prefix is printed.
 * 	If there were no errors (array was empty): ".\n" is printed after it.
 * 	Else:
 * 		If there was one error:	explanation is written on the same line along with '\n'.
 * 		Otherwise: each error is printed at it's own line after the first, prefixed by '\t'.
 *
 * 	After each error's explanation, dice_exp with the error's invalid section highlighted in error_color,
 * 	is printed. If there's no invalid section we can print, the explanation is suffixed by ".\n".
 *
 */
void fprint_dierrors_colored(FILE* fp, char* error_color, char* prefix, char *dice_exp, Dierror *dierror_array);
