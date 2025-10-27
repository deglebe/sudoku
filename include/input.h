/* include/input.h
 * input handling functions
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include "game.h"

bool Input_EscapePressed(void);
void Input_Update(Game *g);

#endif // INPUT_H
