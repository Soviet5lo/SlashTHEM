NOTE: This is not a complete list, just something to note down into a file.

### Must be done before an actual "1.0" release.
* Extensive testing and bugfixing, there's still plenty of things here.
    * Bottom status bar (mostly) disappears upon death with dump enabled.
    * Segfault when using raise zombies on a human corpse (issue #29 on github)
    * Plenty of potion of blood weirdness in relation to the DYWYPISI patch.
    * #borrowing a wielded weapon from a monster sometimes results in the monster attacking with the stolen weapon.
* Put the added throne skill enhancement perk into a menu like do_acquirement instead of several y/n prompts.
* Update upgrade_obj in potion.c for any new objects added.
* Somehow during the addition of statuscolors botl_shortening was broken.  Fix it so it works again and functions with statuscolors enabled.

### Would be nice, but not high priority currently
* Curses support, preferably with all of the enhancements and fixes from Nethack 3.6/Unnethack.
* Android port, utilizing the code from [here.](https://github.com/gurrhack/SlashEM-Android/)
* Redo all of the remaining slash'em extended role quests (except Jedi, Bard, and Noble?) so they're not copies of existing quests.  Unique dialog in quest.txt as well.
* Backporting Dumplog and Overview enhancements from NetHack 3.6.
* Implement the #tip command from Nethack 3.6.
* #give command, to act as sort of a reverse #borrow for pets.
* Remove several old defines (FIREARMS, TOURIST, YEOMAN).
* Port over blasters from dNethack for Stormtroopers to use in the Jedi quest.
* Update the info on the Nethack Wiki as its completely out of date at this point.
* Update menucolors on windows to use regex instead of basic globbing.
* Better windows default config.
* If playing as a ghoul, give a message when a corpse in inventory/line of sight becomes edible.

### Probably won't be done unless someone assists with this
* GTK/Win32 window ports that utilize tiles.

### Don't count on it
* A complete rebase onto Nethack-Current.
