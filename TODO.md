NOTE: This is not a complete list, just something to note down into a file.

### Must be done before an actual "1.0" release.
* Extensive testing and bugfixing, there's still plenty of things here.
    * Bottom status bar (mostly) disappears upon death with dump enabled.
    * Plenty of potion of blood weirdness in relation to the DYWYPISI patch.
    * #borrowing a wielded weapon from a monster sometimes results in the monster attacking with the stolen weapon.
    * Figure out and solve why monster blinding via camera is completely broken with NEPHI_PHOTOGRAPHY defined.
* Put the added throne skill enhancement perk into a menu like do_acquirement instead of several y/n prompts.
* Update upgrade_obj in potion.c for any new objects added.
* Differentiate some added races a bit more.

### Would be nice, but not high priority currently
* Curses support, preferably with all of the enhancements and fixes from Nethack 3.6/Unnethack.  Perhaps port over from Slashem 9?
* Replace the old Unicode support with code from Unnethack.
* Android port, utilizing the code from [here.](https://github.com/gurrhack/SlashEM-Android/)
* Redo all of the remaining slash'em extended role quests so they're not copies of existing quests.  Change up role leader/nemesis.  Unique dialog in quest.txt as well.
    * Acid Mage, Chef, Corsair, Diver, Drunk, Electric Mage, Firefighter, Geek, Gladiator, Locksmith, Musician, Ninja, Officer, Zookeeper.
* Backporting Dumplog and Overview enhancements from NetHack 3.6.
* Remove several old defines (FIREARMS, TOURIST, YEOMAN).
* Port over blasters from dNethack for Stormtroopers to use in the Jedi quest.
* Update the [info](https://nethackwiki.com/wiki/SlashTHEM) on the Nethack Wiki as its completely out of date at this point.
* Update menucolors on windows to use regex instead of basic globbing.
* Better windows default config.
* If playing as a ghoul, give a message when a corpse in inventory/line of sight becomes edible.
* Allow pets to use ranged weapons, maybe ranged attacks as well.
* Full update of database, guidebook and rumor files.
* Somehow during the addition of statuscolors I broke shorten_bot2.  Fix it so it works again and functions with statuscolors enabled.
* Port over Sphinx riddle attacks from [the Biodiversity patch.](https://nethackwiki.com/wiki/Biodiversity_patch)
* Clean up build warnings; these almost certainly are causing issues.

### Probably won't be done unless someone assists with this
* New level compiler from Sporkhack/Unnethack.
* GTK/Win32 window ports that utilize tiles.

### Don't count on it
* A complete rebase onto Nethack-Current.
* Ports/Active Support for legacy platforms (Amiga, VAX, etc.)
* A rewrite of SlashTHEM in Rust.
