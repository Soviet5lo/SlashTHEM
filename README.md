SlashTHEM
=====================

SlashTHEM is a fork of Slash'EM Extended version 75.  It's primary goals are to add more flavor to the game, flesh out and create unique quests for most of the new roles, and apply some much needed balance changes.

A current changelog of SlashTHEM can be found [here] (https://github.com/Soviet5lo/SlashTHEM/blob/master/CHANGELOG.md)

SlashTHEM is also playable online at https://nethack.dank.ninja/.

Current Status
--------------

Currently considered a beta release.  Most of the new role quests haven't been touched so may be a bit more difficult than Vanilla/Slash'EM/Patch roles.

It's difficulty at the current moment should be much, much easier than Slash'EM Extended.  However there are still a few mechanics that need to be worked on, as well as the eventual monster cleanup (to reduce the amount of stealing monsters, monsters with very minor differences, etc)

This should compile fine on Windows (with MinGW) and unix systems.  I haven't done any testing on other platforms.

Windows (x64) builds are available on the [releases] (https://github.com/Soviet5lo/SlashTHEM/releases) page.

Todo
----

- Fix new and old bugs.
- Clean up compiler warnings.
- Finish giving every role a sacrifice gift.
- Overhaul a few of the lazily done quests.
- More balance changes. (Weapon skills, weapon damage balancing, buff/nerf techniques)
- A massive clean-up of monsters, objects and roles.
- Update the Guidebook.
- Plenty of stuff not yet listed here.

Bugs
----
- Some roles do not get the quest dialogue when picking up the quest artifact (Lunatics, Locksmiths, a few others).
- Unicode support is terribly broken, causing the game to not function at all under windows and breaking Pokedex under unix.
- Vault teleporters to aquariums teleport the player to a random location in the level, not the aquarium.
- Artifact protection isn't correctly calculated upon save and load, adding -2 to AC upon every load.
- Probably a ton of others.

Credits
-------
Credits go to Chris for dNetHack (Used a few artifacts), Grunt for GruntHack, BarclayII for SlashEM-up (some balance changes come from this), and of course AmyBSOD for Slash'EM Extended.

Credits also go to Percival Tiglao for the minor sling upgrade patch, Mikael Lind for Rubber Gloves and Gauntlets of Reflection patches, Paxed for the "highlight object piles/hidden stairs" patch, André Bertelli Araújo for the music code for bards, Chris-plus-alphanumericgibberish (Chris) for additions and fixes to the music code, and L for various patches implemented in this fork.

Credit also goes to Edoardo Spadolini for adding server specific code.
