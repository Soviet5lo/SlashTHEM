# Slash'THEM (Rough) ChangeLog

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
