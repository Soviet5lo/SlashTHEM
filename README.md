SlashTHEM
=====================

SlashTHEM is a another Nethack variant.  In particular, it takes after Slash'EM and an earlier version of Slash'EM Extended.  I consider it more of a Slash'EM variant at this point since it's far more in line with that rather than Extended.

A changelog of SlashTHEM can be found [here](CHANGELOG.md).  A changelog of the current develop branch can be found [here](CHANGELOG-NG.md)

If you want the last stable release for whatever reason, you can download the source code from the master branch.  More than likely if you're reading this, you're looking at the develop branch which contains every change since the last stable.

Current Status
--------------

I very rarely come back to this project. Only times I work on it now is when I get the urge to play it, honestly.  There's plenty of bugs that exist and things that haven't been properly worked on, but there shouldn't be too many signifigant segfaults hopefully.  Consider it dormant, essentially. I don't necessarily consider this stable, but its playable.   Plus, there's plenty of other (better) variants out there to play.  If you must play Slash'EM you could go vanilla, or:

- [Slash'EM 9](https://github.com/moon-chilled/slashem9), if you want Slash'EM + Curses Interface + Unicode + a much more coherient codebase, as well as balance tweaks and changes/ports from Nethack 3.6.
- [Slash'EM Extended](https://github.com/SLASHEM-Extended/SLASHEM-Extended), if you want a variant that includes absolutely everything and every possible idea, good and bad.
- [Slash'EM-Upgrades and Patches](https://github.com/BarclayII/slashem-up/tree/develop), if you want Slash'EM + Balance changes, a few backports from Nethack 3.6, various upgrades and integrated patches.

The source code builds fine on Windows using MinGW, as well as Linux.  Other Operating Systems are untested.  I highly doubt it runs on legacy systems however (VMS, Amiga, etc.).  Pull requests are indeed looked at.

Windows (x64) builds are available on the [releases](https://github.com/Soviet5lo/SlashTHEM/releases) page.  I attempt to make a new build based on the current code at the beginning of the month, assuming there's enough changes worth building.

Credits
-------
Credits go to Chris for dNetHack (Used a few artifacts), Grunt for GruntHack, BarclayII for SlashEM-up (some balance changes come from this), Patric Mueller (bhaak) for UnNethack, and of course AmyBSOD for Slash'EM Extended.

Credits also go to Percival Tiglao for the minor sling upgrade patch, Mikael Lind for Rubber Gloves and Gauntlets of Reflection patches, Paxed for the "highlight object piles/hidden stairs" patch, André Bertelli Araújo for the music code for bards, Chris-plus-alphanumericgibberish (Chris) for additions and fixes to the music code, and L for various patches implemented in this fork.

Credit also goes to Edoardo Spadolini for adding server specific code.
