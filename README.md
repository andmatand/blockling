![screenshot 1](http://www.billamonster.com/blockling/screenshots/1.png "A wild blockling carries a block on its head")


## About
In 1993, Doug and Larry Murk of Soleau Software <www.soleau.com> released
BLOCK-MAN 1.0.  I first played Block-Man at my grandmother's house as a young
lad circa 1996.  I loved going to her house because she had a few of those old
"demo CDs" which would have dozens of Shareware/Demo games on them (this was of
course back before broadband Internet access was widely available).  Since
Block-Man was "shareware", it happened to be included on one of those discs.

As I played the game, I was intrigued by its deceitfully simple gameplay:
Little blocks, innocently stacked in random piles amidst a room, radiated with
childlike playfulness; yet, as the levels progressed, the task of arranging
them in such a way as to reach the exit became a puzzle far too complex for
most grown men!  (Well, at least too complex for me at the time)

Being the avid nine-year-old BASIC programmer I was, I figured I'd attempt to
implement my own version of Block-Man.  I did so rather succesfully, adding my
own touches such as "telepads" and new abilities to push blocks and turn around
without walking.  These additions gave the game a somewhat different dynamic,
and I was pleased overall with the result, except for one big problem:  I had
written the whole thing in Microsoft QBasic!  As my code grew larger and
larger, eventually reaching the size-limit of the feeble QBasic interpreter, I
came to the harsh realization that I would need to learn a real programming
language if I ever wanted to finish my version of Block-Man.

Many years elapsed, but I eventually discovered Linux and then finally got
around to learning enough C++ that I was able to rewrite the game from scratch
and produce what I now call Blockling.

I hope you enjoy it.

Andrew Anderson  
March 2010


## Controls
    Esc           Menu
    F1            Help
    F2            Toggle Music
    F3            Toggle Sound
    F5            Restart the current level
    Alt + Enter   Toggle Fullscreen
    PAGE UP/DOWN  Change tile-set

### Game Controls (customizable)
    UP      Pick up a block
    DOWN    Set down a block
    LEFT    Move/turn left
    RIGHT   Move/turn right
    ENTER   Push a block
    U       Undo

### Camera Controls (customizable)
    w  Move camera up
    a  Move camera left
    s  Move camera down
    d  Move camera right


## Creating Your Own Levels
Levels can be easily created using a simple text editor.  Perhaps the easiest
way to begin is by examining some existing levels.  The game's default levels
may be found in the "levels/default/" directory within the "data/" directory.

When examining those levels on non-Unix systems, you will need to be sure to
use a text editor capable of displaying Unix LF linebreaks.

From examining those existing levels, you will likely come to an understanding
as to the format of a level file, but I shall here offer a complete listing of
each character's meaning:

    .     a blank tile.  Note: Regular spaces (i.e. " ") are ignored.
    @     the player
    X     a block
    0     a piece of land.  Note: Based on the tile's position relative to other
          tiles, it may automatically assume a different visual appearance (e.g.
          bricks, grass, etc.)
    1     manually specifies a land tile with the appearance of bricks
    2     manually specifies a land tile with the appearance of grass
    T     a torch.  Note: a "wall" tile will automatically be placed behind it.
    a-z   Lowercase letter pairs indicate telepad pairs.  These need not start
          with "a" or proceed in alphabetical order, but each letter much have
          exactly one mate (e.g. if a "q" is present, there must be one other
          "q" somewhere within the level)
    *     the level's exit.
    ^     a spike
    #     indicates that the rest of this line is a comment and will not be
          parsed.  You can type whatever you want here (e.g. your name)
    (LF)  A Unix linebreak indicates a new row of tiles in the level.  On 
          Windows systems, a Windows linebreak (CR + LF) may be used, since it
          also contains the LF character.  Don't worry; you don't need to
          understand what this means.  Just push Enter =)

Additionally, note the following caveats:

- All characters are case sensitive, thus e.g. "X" will be interpreted as a
  block, while "x" will be interpreted as a telepad.
- A line containing no characters (except the linebreak itself) will be
  ignored.  Thus, to indicate an empty row of tiles use a "." followed by a
  linebreak.

After you have constructed a custom level, place it in the the "levels/custom"
directory within the data directory.  File names must begin with 000 and
proceed in numerical order (e.g. 000, 001, 002, etc.).

When designing a level, it can be helpful to try out your changes by playing
through the level, or simply observing how the level is being parsed by the
game.  Since the game loads levels afresh from disk each time they are
reloaded, this can be accomplished by running the game and displaying the level
upon which you are working, reloading it whenever you save a change.  The F5
keyboard shortcut is particularly handy for quick reloading.

If the game finds any syntax errors in your file, it will let you know with a
(hopefully) helpful on-screen message (on the level-selection screen).


## License Stuff
Please distribute this file with Blockling

Blockling is free software.  It is distributed under the terms of the GNU
General Public License:  http://www.gnu.org/copyleft/gpl.html

The source code of Blockling is available from:
https://github.com/andmatand/blockling
