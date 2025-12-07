# sudoku

a native sudoku engine written in c99 + raylib

a few months of work on and off and i now use this as my primary way of playing
classic sudoku.

## features:

- random, valid puzzle generation at 5 levels of difficulty
- puzzle importing
- notes/candidates/pencil markings
    - currently only corner markings, no centre markings (TODO)
- column/row/digit highlighting
- arrow key and mouse movement
- cell coloring with 9-color palette
    - red, orange, yellow, green, blue, indigo, violet, light gray, white
    - click color buttons in sidebar to apply to selected cell
    - clicking same color removes it from the cell
- game pause/play and board hiding overlay

## todo:

- centre markings
- number completion tracking
- difficulty rating
    - maybe implement a se calculator + some bespoke rating
- modding
    - themes
    - custom rulesets
