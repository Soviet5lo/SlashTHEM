# Slash'THEM ChangeLog

Version 0.8.0 (2015-07-02)
==========================

## NOTICE: 0.8 will be the last release that contains certain roles, races, monsters, and objects.  This is mostly to remove some cruft and filler.

#### Feature Additions.
- Entirely replaced the Elph role with a brand new Chef role.
    - A mostly melee oriented role with a focus on one-handed weapons.
    - Skill set is:
        - Basic: Long Sword, Flail, Saber, Sling, Unicorn Horn, Protection Spells
        - Skilled: Dagger, Club, Paddle, Short Sword, Quarterstaff, Divination Spells, Bare Handed Combat, Two Weapon Combat.
        - Expert: Knife, Dart, Axe, Hammer.
    - Starting inventory consists of a blessed +1 Knife, blessed +1 Leather Armor, blessed +2 dented pot, 3 food rations, 2 sprigs of wolfsbane and 2 cloves of garlic  (acting as herbs and spices), 2 potions of water, 2 potions of booze, a can of grease, a towel, a blessed tinning kit, and a tin opener.
    - Chefs gain Poison Resistance at level 7, and Fire Resistance at level 15.  Chefs have permanent food appraisal.
    - Sacrifice gift is Tenderizer, a neutral aligned mallet.  +3 to-hit, and +7 damage.
    - The Chef Quest sees you meeting the Surpreme Chef at the Iron Kitchen of Gourmet, who tells you to travel into Hell's Kitchen and battle The Immortal Rot for The Deluxe Yendorian Knife.  The Deluxe Yendorian Knife is a neutral aligned knife that has +6 to-hit and +20 damage, as well as granting Fire Resistance and ESP while wielded, and Acid Resistance while carried.  #Invoke to Sharpen it's blade (+1 if blessed, +0 if uncursed, -1 if cursed).  the des file used is the Doppelganger quest from SLASH 6 as a temporary.
    - Chefs are now the only role that can create non-homemade tins.  All other roles use vanilla behavior.

- Implemented Music from the Bard Patch. More information can be found [here] (https://andreba.wordpress.com/projects/nethack-bard-patch/)
    - Bards, Rockers, and Musicians are the only 3 roles that can use Music currently.  All three gain Expert in Music.
    - Songs are played by 'a'pplying a non-magical instrument.  Each song can be played by two specific instruments.
    - A minor change was made.  Allowed any role unrestricted in music to play songs instead of relying on knowing the spell tied to the song.
    - Also applied fixes and enhancements for music from dNethack.
    - Current list of songs are:
        - Lullaby: Makes monsters fall asleep.  Also affects pets at basic skill.
        - Cacophony: Confuses monsters.
        - Drowsiness: Slows a monster based on music skill.  Reduces speed by 6 at basic, 9 at skilled, and 11 at expert.
        - Despair: Scares monsters.
        - Friendship: Temporarly make monsters "friendly".  Are shown as pets will drop armor and weapons, but will not attack others.
        - Inspire Courage: Makes pet more likely to attack higher level monsters and will ignore low hp.
        - Charge: Hastes pets.
        - Meditative Healing:  Heals pets.
        - Disruption: Temporarly cancels monsters.
        - Rally: Removes status effects from pets based on level.  Sleep and fear are removed at unskilled, stun and confusion removed at basic, blindness is removed at skilled, and cancellation is removed at expert.

- Implemented the Monster Colors patch from NAO.
- Implemented the Hallucination Gods patch.
- Merged in the Statuscolors branch, but it's still disabled by default as the segfault still exists.  Can be enabled at compile time by uncommenting STATUS_COLORS in config.h

#### Changes
- The foo Horrors and other U's no longer have randomly generated attributes or attacks.  These were found to cause massive game instability and frequent crashes when they attacked.
- Disabled viewing artifacts in the discovery screen for the moment, as quest artifacts will crash the game.
- Disabled the extra special effects for certain types of attacks introduced in Slash'EM Extended.  This includes level drain from all bite attacks, kicking attacks doing extra damage to male players, and the new boots added in Slash'EM Extended having specialized effects when used by monsters.
- Disabled the extended farlook information as using it repeatedly ends up corrupting mons[] over time, causing game instability.  Re-enable it by uncommenting EXTENDED_INFO in config.h.
- Reduced the quantity of Shurikens generated (rnd(100) -> rnd(20)).
- Tools, Wands and Spellbooks should no longer have abnormally low charges (Reverted to Slash'EM behavior).
- Reverted object charging to function the way it did in Slash'EM.
- Reverted the mall level back to the way it was in Slash'EM.  No longer will you get "mall-3" that contains no shops but a ton of nymphs.
- Sokoban's staircase always appears on the level directly after the Oracle again (Reverted to Slash'EM behavior).
- The Quest is 5 levels instead of 7 levels (Reverted to Slash'EM behavior).
- Grues no longer have displacement as this made them way too annoying to fight.
- Removed a few added random elements to sacrificing that were added by Slash'EM Extended (Reverted to vanilla behavior).
- Convicts get reduced prices in the Black Market (idea from Unnethack).
- Shimmerstaff and the Pen of the Void are now Neutral aligned, as roles are never offered an unaligned artifact for their first gift.
- Cleaver now hits adjacent monsters occasionally (Code from Nethack Fourk).
- Unicorn Horns no longer turn into dust (Reverted to Slash'EM behavior).
- Bards no longer recieve spellbooks when gaining a level (see Music above).
- Removed ranged grapple/hug attacks.
- Keys should no longer have an abnormally high chance of breaking (Reverted to Slash'EM behavior).
- Monsters will no longer randomly bypass locked doors.
- Removed a "feature" where players would be diseased and lose max alignment for causing a pudding split.
- Changed artifact protection to be extrinsic instead of intrinsic.
- Reverted a Slash'EM Extended change that made all wands/tools/scrolls/potions cost the same.
- Iron balls and chains are no longer randomly generated in the dungeon.

#### Bugfixes
- Fixed a segfault when chained iron balls were destroyed via rust.
- Fixed a bug where corpses had the incorrect color.
- Fixed an incorrect message in weapon.c.
- Fixed corrupted message when killed by a monster with a passive disease attack (for real this time).
- Fixed a crash with scrolls of food detection.
- Fixed a "feature" where Vlad's level would occasionally be replaced with a room-and-corridor level.
- Fixed livelog reporting an averted death twice during trollor revival.
- Fixed a minor issue where shop floors would have a few empty tiles.
- Fixed artifacts being destroyed via rusting.
- Fixed plasteel helm and plasteel boots granting an abnormally high MC of 9.

Version 0.7.0 (2015-05-23)
==========================

#### Feature Additions
- Implemented the ShowSym patch
- Implemented the Dungeon Overview patch.
- Implemented the Dungeoncolors patch.
- Implemented the Dumplog patch.
- Added two boolean options for Dungeon colorization.
    - dungeon_colors controls the colorization of special levels, branches, and special rooms.  Can be set in game or in the config file, set to false by default.
    - random_room_colors controls the colorization of all other rooms in room-and-corridor levels.  Can be set in game or in the config file, set to false by default.
- Added Instrument Shop, a shop that specializes in selling instruments both magical and mundane, as well as a small chance of selling a t-shirt and a lock pick.
- Implemented the Keen Memory property from the Ring of Memory patch.
    - Keen Memory slows spell memory decay to 2/3rd of the normal rate and protects against amnesia effects.
    - Keen memory can be gained by wearing an Amulet of Data Storage.
    - Monsters no longer wear the Amulet of Data Storage, nor do monsters gain reflection by wearing it.

#### Changes
- Normalized skill spreads for all roles.  No longer will a role be able to reach all expert with every skill they have.  Suggestions are welcome for buffs/nerfs/changes/etc to some role skill sets (Reverted to Vanilla/Slash'EM Behavior).
- Skills no longer take as long to level up (Reverted to Slash'EM levels).
- Officers now start wielding their clubs, not pistols.
- Gangsters now start with a tame little dog.
- Musicians now start with either a tame little dog or a tame kitten.
- Convicts no longer start with racial equipment.
- Illithids gain extrinsic telepathy at Level 5.
- Illithids count as Human for racial purposes.
- Nymphs gain teleport control at Level 15.
- Nymphs no longer can start with rings of teleport (control).
- Kobolts now always start with 15 darts, but now have a chance of either starting with 3 orcish daggers or 2 orcish spears, not both.
- Ghastly characters no longer start with 5 extra corpses in their starting inventory.
- Alien:
    - No longer start with a Loadstone.
    - Starting alignment penalty has been reduced (-20 -> -10).
    - Starting luck penalty has been increased (-2 -> -3).
    - Racial attribute maximums has been decreased to 15 except Intelligence, which is now capped at 18.
- Insectoid characters now have hands.
- Trollors no longer lose 2 levels upon revival.  Instead they lose 1 level but are set to the beginning of that experience level.
- Orc Acid/Electric Mages no longer start with extra food, to keep consistency with Orc Flame/Ice Mages and Wizards.
- Unicorn Horns no longer permanently remove attributes.
- The Scroll of Consecration no longer increases prayer timeout.
    - This also solves an issue where reading a scroll of consecration would result in an rn2(n) error.
- Iron bars and chains are no longer enchantable.
- Altars no longer randomly disappear.
- Corpses no longer mold into blobs, jellies, or puddings (reverted to Slash'EM Behavior).
- Elbereth works the same as it did in Slash'EM (no longer has a random chance of failing).
- Monsters no longer have a chance of being startled instead of fleeing normally. (reverted to Slash'EM Behavior)
- Thrones are no longer randomly generated in rooms (Reverted to Slash'EM Behavior).
- Walls in special levels are no longer randomly replaced with dungeon features (This can be re-enabled by defining RND_SPEC_WALLS in config.h).
- Mimics no longer appear outside of shops so early (Reverted to Slash'EM Behavior).
- Reduced the amount of angry deity spawns back to Slash'EM levels (Re-enable by defining MORE_SPAWNS in config.h).
- Co-aligned minions and monsters are peaceful to the player more often (Reverted to Slash'EM behavior).
- Sessile Monsters/Objects are no longer generated on top of statue traps, hiding them from view.
- Stopped seducing monsters from getting pregnant/getting the player pregnant.  This was just very, very out of place.
- Removed the Fresh Food, Rock Solid, Mining, Lightning, and Weirdo Shops.  Also removed the never used Crappy Weapons shop.
- Hallucination no longer randomly colorizes the entire map, unless you're playing using the "hippie" mode flag.
- Swapped the placement of food conducts in topten.c for livelog/xlogfile.
- Added a separate "mode" field for xlogfile for use in the Junethack tournament.

#### Bugfixes
- Fixed corpses in the orc minetown variant being the wrong species.
- Fixed a minor error in the zoo variant of minetown.
- Fixed an error causing The Cudgel of Cuthbert to not show up in the Chevalier quest.
- Fixed a warning about trailing space in mondata.h.
- Fixed a clang warning about an unclosed comment in uhitm.c.
- Fixed an empty return in display.c
- Fixed the Scroll of Consecration giving a "You feel (un)comfortable" message when it should give the opposite message.
- Fixed a very minor typo in the Scroll of Consecration's text ("consectrate" -> "consecrate").
- Fixed an issue where being killed by a gray fungus illness attack would corrupt the "Killed by" text at the epitaph screen.
- Fixed Ogros getting a +4 damage bonus when using clubs.

Version 0.6.0 (2015-05-02)
==========================

- Removed Scrolls of Barrhing, Lockout, Growth, Lava, Warping, and Tele Level.
- Special Character generation is now set in the configuration file (Backported from SLASH'EM Extended)
- Locksmiths now gain much higher unlocking bonuses (values backported from SLASH'EM Extended)
- Firefighters and Graduates have new level names.
- Made convict starting alignment less sadistic (-50 -> -20)
- Nymphs can reach 25 in Charisma.
- Orcs, Ogros, Trollors, and Gigants can Twoweapon.
- Dwarves are no longer peaceful to Illithid players.
- No longer possible to get slimed by eating rotten food, violating vegetarian conduct, or being gluttonous.
- Restored the Gnomish Mines monster generation to Vanilla status.  You should now see much, much, MUCH less gnome rogues/warriors/etc in the mines now.
- Reduced the probability of scrolls of wishing being randomly generated.
    - Increased the probability of Scrolls of Confuse Monster to compensate.
- Flying characters can now pick up objects in pits, and can now use whips to pull things from pools and pits.
    - Levitating characters can no longer pull characters out of pits.
- Phasing characters no longer have their vision obscured from being inside of a pit.
- Ugly backpacks are correctly automatically identified.
- Removed Pleasant Valley from castle.des as it's been turned into Pleasantville for the Town Branch.
- Disabled extra monsters spawning on dungeon features (re-enabled by enabling MORE_SPAWNS in config.h).
- Gladiator artifacts
    - Sacrifice gift: Gladius, a Neutral aligned Short Sword.  +8 to-hit and +7 damage.
    - Quest Artifact: The Imperial Token, a Neutral Ring of Aggravate Monster.  Grants regeneration and fire resistance while worn, drain resistance while carried.  #Invoke for Leadership effect.
- Korsair artifacts
    - Sacrifice gift: Queen's Guard, a Lawful aligned Rapier.  +6 to-hit, +6 damage.
    - Quest Artifact: The Pearl of Wisdom, a Neutral aligned Ring of Gain Wisdom.  Grants shock resistance and half physical damage while worn, magic resistance while carried.  #Invoke to reveal the map like a scroll of magic mapping.
- Warrior artifacts
    - Sacrifice gift: Hrunting, a Neutral aligned long sword.  +4 to-hit, +5 damage.  The name comes from Beowulf.
    - Quest Artifact: The Sword of Svyatogor, a Lawful aligned Two-Handed Sword.  +7 to-hit, +9 damage.  Grants Half Physical Damage and Magic Resistance while carried, Cold resistance while wielded.  #Invoke for Levitation.  Name comes from the tale of Ilya Muromets.
- Musician Artifacts
    - Sacrifice Gift: Dirk, a Neutral aligned Dagger.  +5 to-hit and +5 damage.  This is more than likely a temporary until I can figure out something better to give them.
    - Quest Artifact: The Harp of Harmony, a Lawful aligned Magic Harp.  Grants Warning, Stealth and Drain Resistance while carried.  Speaks when used.  #Invoke for Taming.
- Chevalier changes
    - Completely changed the starting inventory.  Now start with (all blessed) +1 Silver Short Sword, +1 Ring Mail, +0 Helmet, +0 Small Shield, +0 Leather Gloves, 2 Potions of Holy Water, 2 Cloves of Garlic, 2 Sprigs of Wolfsbane, a Spellbook of Healing, a Random scroll, a saddle, and a small chance of either an oil lamp/brass lantern or a magic marker.
    - Quest Artifact: The Cudgel of Cuthbert, a Neutral aligned Quarterstaff.  +5 to-hit and double damage to all cross-aligned monsters.  Grants Hallucination resistance and regeneration while wielded, grants drain resistance, warning, and half spell damage while carried.  #Invoke for Blessing.
- Implemented the Wand of Might from Nethack: The Next Generation.  Unaligned Wand of Wishing, only generated on the Castle level.  Cannot be wished for.
- Implemented a few standalone artifacts.
    - The Gambler's Suit: Unaligned Expensive Suit.  Grants Protection while worn, acts as a luckstone, grants an extra +3 bonus to Charisma while worn.
    - Shock Brand: Unaligned Long Sword.  +5 to-hit, double damage to all non-shock resistance monsters.  Grants shock resistance while wielded.
    - Acid Brand: Unaligned Long Sword.  +5 to-hit, double damage to all non-acid resistant monsters.  Grants acid resistance while wielded.
    - Mirror Brand: Unaligned Silver Long Sword.  +5 to-hit, double damage to all cross aligned monsters with Magicbane's special effects.  Grants reflection while wielded.  Artifact comes from dNethack.
- Gave Cat's Claw it's intended warning against rodents.
- Modified hallucination monsters names.
    - Removed a bunch of redundant names, currently vanilla + NAO + Unnethack.
    - Unique and Genocided monsters no longer appear as hallucination names.
- Fixed a few build errors on FreeBSD.
- Fixed Liontamer not getting its damage/to-hit bonus against felines.
- Fixed a link in README.md.
- Disabled reading Orb of Fate/Coins as most of the time it didn't work and the few times it did work it caused a dungeon collapse.

Version 0.5.2 (2015-04-23)
==========================
- Finally, definitely, completely kill the trap bug (for real this time.)
- Spineseeker does an extra d5 damage when it stuns a monster (or player).
- Disabled enchantable rocks, as the strength + racial bonus does more than eough.
- Reverted Gauntlets of Power to function as they do in Slash'EM.
- Stopped drunks from starting with potions of (vampire) blood, cyanide, radium, and Pan Galactic Gargle Blasters.
- Applied the MSGTYPE patch
    - Also removed a ton of --More-- Prompts that were added in Slash'EM Extended, as the MSGTYPE patch makes most of these redundant.
- Applied the While Helpless patch.
- Fix a leftover in monmove.c that resulted in some monsters searching for objects that they can't use.
- Prevented roles from randomly starting with Potions of Invulnerability.
- Removed a confusing prompt when squeezing into a space with a boulder.

Version 0.5.1 (2015-04-21)
==========================
- Wrapped AmyBSOD's Extra Spawning system into a define, disabled by default.  Uncomment MORE_SPAWNS in config.h to re-enable.
    - If MORE_SPAWNS is defined, regular monster generation is reduced.  Otherwise, monster spawn gen is the same as vanilla (increased from extended).
- Spell Memory no longer shows up in the spellcasting memu
- Spell Chances max out at 100% again
- Reverted Rock and Gem probability to Vanilla values. 
    - TL;DR: Rocks less common, glass more common, gems slightly more common, touchstones slightly less common, whetstones and luckstones slightly more common.
- Traps are no longer generated out of level (finally!)
    - This also fixes traps being generated in corridors.

Version 0.5.0 (2015-04-15)
==========================
- Officers now start with a tame little dog
- All roles start with less bullets
    - Officers: 100 -> 40
    - Gangsters: 100 -> 30
    - Pirates: 50 -> 25
    - Goff: 100 -> 20
    - Topmodel: 50 -> 20
- Elder Scrolls races are disabled by default to reduce race menu clutter.  Re-enable by uncommenting #ELDER_SCROLLS in config.h.
- Kobolt/Trollor/Ogro players gain a +2 damage bonus using their extra starting weapons.
- Village.des
    - Reduced chance of so many objects appearing in the village cache
    - Added 2 new potential goodies to the cache chest, and increased the chances of cursed goodies to 20%.
- Reverted priest/temple prices back to vanilla values
- Artifact locking/unlocking tools now work on all locks again.
- Move livelog defines to PUBLIC_SERVER (should stop fcntl errors on local installs.)
- Locksmiths artifact changes
    - Sacrifice gift is Oathkeeper.  Neutral aligned Stiletto, +7 to-hit and +9 damage.
    - Quest artifact is The Lockpick of Arsene Lupin.  Neutral aligned Lock pick, grants searching, telepathy, warning, stealth, and magic resistance while carried.  Also acts as a luckstone.  #Invoke to Untrap chests and doors.
- Undertaker artifacts
    - Sacrifice gift is Blackshroud.  Neutral aligned Cloak of Protection.  Acts as a Luckstone and grants Warning while worn.
    - Quest artifact is The Pick of The Grave.  Neutral aligned Pick-axe.  +8 to-hit and +10 damage with draining, grants cold resistance, regeneration, half physical damage and teleport control while wielded.  #Invoke for Blessing.
    - Black Death has been turned into a regular artifact.
- Gangster artifacts
    - Sacrifice gift is Shawshank.  Chaotic aligned Knife, +9 to-hit and +9 damage.
    - Quest Artifact is now The Tommy Gun of Capone.  Chaotic aligned Submachine Gun, +5 to-hit and +7 damage, Grants stealth, warning, magic resistance while carried.  Grants fire resistance while wielded.  Acts as a luckstone, #Invoke for Leadership.
- Knights get Clarent as their sacrifice gift.  Lawful aligned short sword, +8 to-hit and +2 damage to all thick skinned monsters.  Also acts as a luckstone.  #Invoke for Leadership effect.
- Moved the Hitchhiker, Computer, and Key levels from The Next Generation to Gehennom and increased the chances of them appearing to 50%.
- Reduced the amount of Heck levels in Gehennom from 5 to 3 and decreased the chance of them appearing from 90 to 50%.
- The Burned Moth Relay is set to NOGEN, and is generated on the UNIX Kernel in the Computer level.
- Reverted a change by AmyBSOD allowing Room-and-corridor levels in Gehennom.
- Fixed Bless invoke effect falling through to Leadership on use.
- Fixed misplaced SPFX_LUCK tags in artilist.h causing some artifacts to not act as luckstones.
- Fixed issue with shirts not removing their Charisma bonus when taken off.

The changelog that lists all changes before version numbers were used can be found [here] (https://github.com/Soviet5lo/SlashTHEM/blob/master/CHANGELOG-Old.md)
