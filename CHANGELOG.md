# Slash'THEM (Rough) ChangeLog

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
