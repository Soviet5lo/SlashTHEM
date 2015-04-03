# Slash'THEM (Rough) ChangeLog

## Differences from Slash'EM Extended Version 75

### General
- Added Unix Build support
- Re-enabled certain roles/races not starting with specific objects.
- Updates the files to use Slash'EM 0.0.7E7F3 sources.
- Adjusted the general object class generation properties to be more like sensible.
- Re-enabled limited inventory (A maximum of 52 objects)
- Engraving works similar to how it did in vanilla (Engravings/Burned text doesn't erase easily)
- Decreased the chance of rumor engravings (now back to vanilla levels)
- Reduces the amount of text erased from randomly placed engravings.
- Restored experience requirements back to Slash'EM levels.
- Reverted the artifact nerf (half damage) applied in Slash'EM Extended version 75.
- Alphabetized roles and races in the selection menu.
- Use sequential letters for roles and races.
- Restored role and race attribute values to vanilla values.
    - Values from Patches/Variants are used for roles that do not exist in Vanilla/Slash'EM.
    - Most of the new roles have custom attributes befitting that monster type.
    - Removed roles and races getting random attribute bonuses, as this broke the vanilla system.
- Races now get Racial Equipment Bonuses. (+1 to Damage or AC, for weapons and armor accordingly)
    - Elves gain bonuses from using elven equipment
    - Dwarves gain bonuses from using dwarven equipment
    - Orcs, Kobolts and Lunatics gain bonuses from using orcish equipment
    - Drow gain bonuses from using droven equipment.
    - Gnomes gain bonuses from wearing gnomish armor (+2 AC to make up for cruddy armor), as well as using Crossbows and Alkys
    - Hobbits gain an attack bonus from using a sling.
- Removed random checks for warning and telepathy.
- Implemented the Food Appraisal patch by L.  All animals and lycanthropes have permanent food appraisal, and appraisal now warns of were/halluc/Rider corpses.
- Implemented the Wounds Patch by L.  Works with Healers, Scientists, Necromancers and Undertakers.
- Implemented the Hilite Item Piles and Hilite Stairs patches by Pasi Kallinen (paxed)
    - Boolean options can be set in the options menu or in the slashthemrc file.

### Roles

- Ninja
    - Initial inventory expanded:  A +1 pair of low boots, a +1 leather cloak, and a +1 grappling hook.
    - Ninja gain Searching at Level 3, Speed at Level 5, Swimming at Level 10, and Warning at Level 15
        - No longer gain Invisibility at Level 3.
    - Can now use the Vanish technique, and gain it at Level 1.
    - Can also now use Critical Strike technique at level 1.
    - Gains the Cutthroat technique at level 10.
    - Now gain the same backstab bonus Rogues have.
    - Ninja use Japanese object names.
    - Ninja get the same greetings as Samurai.
    - Level names and deities changed.
    - Specialized level up message while hallucinating.

- Samurai
    - Level names changed to be more accurate.

- Scientist
    - Decreased enchantment value of starting lab coat from +5 to +1.
    - Replaced the initial Hacker's Food with Sandwiches.
    - No longer knows about all weapons or armors.

- Graduate
    - No longer knows about all weapons or armors.
    - Init Inventory Changes:  Now start with a +1 Sharpened Pencil instead of a spoon, and an Ugly Backpack instead of a sack.

- Korsair
    - No longer starts with an Amulet of Magical Breathing
        - Instead starts with the Swimming intrinsic.

- Diver
    - No longer starts with an Amulet of Magical Breathing
        - Instead starts with the Swimming intrinsic.

- Necromancer
    - Gains an alignment bonus for digging into a grave if chaotic.

- Acid Mage
   - Changed 2 level names and changed deity names.

- Electric Mage
   - Changed deity names.

- Officer
    - Level names restored from Nethack--.

- Musician
    - Level names restored from Nethack--.

- Chevalier
    - Deity names are now chosen at random from all other roles (similar to Priests).

- Locksmith
    - Init inventory addition: A stack of 5 blessed +0 daggers.
    - Gain the same unlocking bonuses that Rogues get.
    - Level names restored from Nethack--.

- Drunk
    - Level names restored from Nethack--.
    - Minor change to Deities.

- Zookeeper
    - Level names restored from Nethack--.

- Lunatic
    - Initial inventory entirely changed: Now only consists of an orcish short sword, a potion of sickness, and 2 random scrolls.
    - Now have 2 squares of vision, similar to Cavemen and Cavewomen.
    - Quest overhauled
        - The quest now involves the player meeting the High Lycanthrope in the Calerin Forest.  There you are told to go into the Nightmare Forest and battle Sir Lorimar to recover the Staff of Withering.
        - Level enemies are centaurs and humans (elves).
        - Quest Dialogue and Levels all come from SLASH 6
    - Level names restored from SLASH 6 Lycanthrope role.

- Pirate
    - Fixed an item duplication bug that occurs with their initial inventory.
    - Now only start with 50 bullets, not 100.
    - Changed deities to those specified in original Pirate patch.

- Monk
    - Now have specific greetings like knights, monks, pirates, etc.

- Bleeder
    - No longer start with a Ring of Timely Backup, Amulet of Data Storage, Scroll of Root Password Detection, Ice Box or Faerie Floss Rhing.
    - Now start with a tame little dog or kitten as the starting pet.

- Bard
    - Init inventory addition: A +1 silver dagger.
    - Bards use Charisma for Spellcasting.
    - Minor change to deities.

- Undertaker
    - Init inventory additions: A brass lantern, and a blessed +0 Pick-axe.
    - Init inventory change: Replaced the Victorian Underwear with an Expensive Suit.
    - Can gain skill in Pick-axe.

- Gangster
    - Init inventory addition: A blessed +0 Knife.

- Courier
    - No longer start with the Conflict intrinsic.
    - Can now wear Helmets.

### Races

- Gastly
    - Gastly characters no longer suffer from cannibalism penalty.
    - Gastly characters no longer suffer from rotten food effects
        - They now heal 2d2 + (level) hitpoints from eating such corpses.
    - Gastly characters can drink potions of (vampire) blood without getting sick.
    - No longer get an alignment penalty when eating meat as a vegetarian role.


- Illithid
    - Illithids no longer suffer from Cannibalism penalty.
    - Can wear Helmets, with the exception of the Plasteel Helm.
    - Only other mind flayers are peaceful to illithids.
    - Now spawn as correct undead (Undead Mind Flayer)

- Hobbit
    - Hobbits start with a +1 Luck bonus.
    - Hobbits gain multishot with slings

- Vampire
    - Vampires lose nutrition at half speed while hungry or worse.
    - No longer start with alignment/luck penalties or sins if playing as a Chaotic Necromancer.
    - No longer get an alignment penalty when eating meat as a vegetarian role.

- Doppelganger.
    - No longer can start with a ring of polymorph control.
    - Can polymorph into monsters above level 30 again.
        - Instead, they now gain polymorph control at Level 25.

- Kobolt
    - No longer have the restful sleep intrinsic

- Nymph
    - No longer start with punishment, nor get randomly punished.
    - Now spawn as correct undead (Undead Nymph)

- Drow
    - Now spawn as correct undead (Drow Mummy/Zombie)

- Ogro
    - Now spawn as correct undead (Ogre Mummy/Zombie)

### Objects

#### Artifacts

- Shadowblade (Convict sacrifice gift - replaces Luck Blade)
    - Chaotic aligned knife, grants stealth while wielded.  +5 to-hit and +6 damage.  #Invoke to toggle invisibility.

- Itlachiayaque (Archeologist sacrifice gift)
    - Neutral aligned Shield of Reflection.  Half Spell Damage and Fire Resistance while carried.  #Invoke for Smoke Cloud.

- Debugger (Electric Mage sacrifice gift)
    - Neutral aligned Athame.  +5 to-hit and +5 shock damage, grants shock resistance while wielded.

- Acidtester (Acid Mage sacrifice gift)
    - Neutral aligned Athame.  +5 to-hit and +5 acid damage, grants acid resistance while wielded.

- Straightshot (Ranger sacrifice gift)
    - Neutral aligned Bow.  +10 to-hit and +3 damage.

- Shimmerstaff (Bard sacrifice gift)
    - Unaligned Silver Capped Staff.  +8 to-hit and +5 damage.

- Full Metal Jacket (Rocker sacrifice gift)
    - Neutral aligned Leather Jacket.  Grants Protection, Hallucination resistance and Fire resistance while worn.

- Tesla's Coat (Scientist sacrifice gift)
    - Neutral aligned Lab Coat.  Grants Protection, Half Physical Damage and Shock resistance while worn.

- Nightingale (Ninja sacrifice gift)
    - Chaotic aligned Katana.  +2 to-hit and +3 damage.  Has a 10% chance of dealing an extra 5d6 damage.  #Invoke to toggle invisibility.

- Crown Royal Cloak (Drunk sacrifice gift)
    - Neutral aligned Leather Cloak.  Grants Protection and Acid Resistance while worn.  Acts as a Luckstone.

- Liontamer (Zookeeper sacrifice gift - formally Quest Artifact)
    - Neutral aligned Bullwhip.  +5 to-hit and +8 damage to all Felines.

- Kiku-ichimonj (Samurai sacrifice gift - replaces Snickersnee) [from dNethack]
    - Lawful aligned Katana.  +4 to-hit and +12 damage.

- Icebiter (Firefighter sacrifice gift)
    - Lawful aligned Axe.  +6 to-hit and +6 cold damage.  Grants cold resistance while wielded.

- Bloodmarker (Bleeder sacrifice gift)
    - Chaotic aligned Knife.  +3 to-hit and +7 damage.

- Peacekeeper (Officer sacrifice gift)
    - Lawful aligned Pistol (or Club, if FIREARMS isn't defined).  +4 to-hit and +9 damage to all cross aligned monsters.

- Poseidon's Treasure (Diver sacrifice gift)
    - Neutral aligned Trident.  +10 to-hit and +10 shock damage.

- Snakeskin
    - Neutral aligned robe, grants hallucination resistance and acid resistance while worn.

- Fungisword
    - Lawful aligned Long Sword.  +10 to-hit and double damage against all Fungi monsters.  Grants Hallucination Resistance while wielded.

- Diplomat
    - Neutral aligned Golden Saber.  +10 to-hit and +25 damage to all PNAME monsters.

- Petslayer
    - Chaotic aligned Club.  +5 to-hit and +10 damage to all domestic monsters.

- Yoichi no Yumi [from dNethack]
    - Lawful aligned Yumi.  +20 to hit, double damage.  #Invoke to create Ya.

- Slow Blade (formally Acid Mage quest artifact)
    - Chaotic aligned Orcish Short Sword.  +1 to-hit and +9 damage.

- Spineseeker
    - Chaotic aligned Stiletto.  +5 to-hit and +5 damage.  25% chance of stunning enemy d5 turns.

- Vampire Killer (formally Lunatic quest artifact)
    - Lawful aligned Bullwhip.  +10 to-hit and +20 damage to all Undead, Demons and Werecreatures.  Grants Drain Resistance while wielded.  #Invoke for Blessing.
        - No longer automatically kills Vampires.

- Silvershard
    - Unaligned Silver Dagger.  +2 to-hit and double damage.

- Warforger
    - Unaligned Heavy Hammer.  +15 to-hit and +15 damage.  Grants Fire Resistance while wielded.  15% chance of doing an extra 6d6 damage, with a small chance of destroying potions, scrolls and spellbooks.  Non-wishable, only generated on the Blacksmith.

- Fuma-itto no Ken (Ninja Quest Artifact - Replaces The Dragonclan Sword) [from dNethack]
    - Chaotic aligned Broadsword.  +8 to-hit and +8 damage.  Grants Drain Resistance while carried.  #Invoke to create Shuriken.

- The Gourd of Infinity (Drunk Quest Artifact - Replaces The Cloak of Neutrality.)
    - Neutral aligned Horn of Plenty.  Grants ESP, Half Physical Damage, Hallucination Resistance and Drain Resistance while carried.  #Invoke to charge an object.

- The Flute of Slime (Acid Mage Quest Artifact - Replaces Slow Blade)
    - Neutral aligned Magic Flute, Grants Shock Resistance, Warning and Teleport Control while carried.  #Invoke to summon a tame (greater) earth elemental.

- The Harp of Lightning (Electric mage Quest Artifact - Replaces Thunder Whistle)
    - Neutral aligned Magic Harp.  Grants Acid Resistance, Warning and Teleport Control while carried.  #Invoke to summon a tame (grater) air elemental.

- The Sling of David (Rocker Quest Artifact - Replaces Rocker Sling)
    - Neutral aligned Sling.  +5 to-hit and double damage.  Grants Half Physical Damage while wielded.  #Invoke for Phasing.  Will always instant kill any Giant.

- The Golden Whistle of Nora (Zookeeper Quest Artifact - Replaces Liontamer) [from SLASH 6 des file]
    - Neutral aligned Magic Whistle.  Grants Warning, Half Physical Damage and Telepathy while carried.  #Invoke for Taming.

- The Mantle of Knowledge (Psion Quest Artifact - Replaces Gauntlets of Offense)
    - Neutral aligned Helm of Brilliance.  Grants ESP and Half Physical Damage while worn, grants Magic Resistance while carried.  #Invoke for Energy Boost.

- The Staff of Withering (Lunatic Quest Artifact - replaces Vampire Killer)
    - Chaotic aligned Quarterstaff.  +3 to-hit and +5 damage (+d8 drain) to all cross aligned monsters.  Grants Cold Resistance while carried.  #Invoke for Energy Boost.

- The Hand Mirror of Cthylla (Binder Quest Artifact - replaces Pen of the Void)
    - Unaligned Mirror.  Grants Searching, Teleport Control, and Magic Resistance while carried.  #Invoke for Energy Boost.

- The Fire Chief Helmet (Firefighter Quest Artifact - replaces Fire Brigade Reel) [from SLASH 6 des file]
    - Lawful aligned Fire Helmet.  Grants Protection and Warning while worn. Grants Half Spell Damage and Half Physical Damage while Carried.  #Invoke to summon a (greater) water elemental.

- The Scalpel of the Bloodletter (Bleeder Quest Artifact - replaces Bloody Beauty)
    - Neutral aligned Scalpel.  +9 to-hit and +10 damage (+d8 drain).  Grants Regeneration while carried, and Half Physical Damage while wielded.

- The Whistle of the Warden (Officer Quest Artifact - replaces Shiny Magnum)
    - Lawful aligned Magic Whistle.  Grants Telepathy, Teleport Control and Drain Resistance while carried.  #Invoke for Leadership.

- Revived the Newhon artifacts from SLASH 6 (if NEWHON_ARTIFACTS is defined in config.h)
    - Mouser's Scalpel: Neutral aligned Rapier, +5 to-hit and +1 damage.  50% chance of striking multiple times, each strike doing an extra d8 + 1 + enchantment. [from Slashem-up]
    - Graywand: Neutral aligned Two-Handed Sword, +3 to-hit and +6 cold damage.
    - Heartseeker: Neutral aligned Showr Sword, +3 to-hit and +3 damage.  15% chance of doing an extra 4d6+4 damage.
    - Cat's Claw: Neutral aligned Dagger, +5 to-hit and +7 damage to all rodents.

- Implemented several new #invoke effects.
    - Summon Earth Elemental
    - Summon Air Elemental
    - Summon Water/Fire Elemental have been updated to have a chance of summoning their greater forms
        - Ice Mages will always summon an Ice Elemental.
    - Smoke Cloud: Acts as a scroll of stinking cloud.
    - Sharpen: Brings enchantment to 1 if blessed, 0 if uncursed, -1 if cursed.
    - Blessing: Blesses, increases enchantment to 3 and erodeproofs the artifact.
    - Leadership: Detects all tame pets on the level and increases the tameness of pets in line of sight.

- Added Acid attack and resistance properties for artifacts.
- Drain resistance is now a valid property for artifacts.
- Added a case for Electric based blade artifacts.
- Quest Nemeses now wear the quest artifact if possible.
- Certain artifacts no longer count against getting sacrifice gifts.
    - Key of Law, Key of Neutrality, Key of Chaos, Nighthorn, The Eye of the Beholder, Hand of Vecna, Thiefbane, Key of Access, Burned Moth Relay, Scales of the Dragon Lord, Warforger.
- Holy Spear of Light is now the sacrifice gift for Undead Slayers.
- Shocker is now the sacrifice gift for Geeks.
    - Also fixed Shocker where it correctly does electric damage.
- Deluder is now the sacrifice gift for Jedi.
- Deathsword is now the sacrifice gift for Warriors.
- Doomblade is now the sacrifice gift for Lunatics.
- Pen of the Void is now unaligned, and is the sacrifice gift for Binders.
- Reaper is now the sacrifice gift for Chevaliers.
- Marauder's Map always works now.
- The Rod of Lordly Might can now be #Invoked for Leadership.
- Goffic Backpack is an Ugly Backpack instead of a Sack.
- Can no longer wish for The Key of Access or The Scales of the Dragon Lord.

#### Non-Artifacts

- Implemented the Silver Capped Staff from the Lethe patch
    - Quarterstaff skill, d9 large/small with +d20 silver damage.

- Implemented the Sharpened Pencil from Nethack--
    - Dagger skill, Wooden, d2 damage to large/small.  Engraves as an Athame (doesn't dull during engraving)

- Added the Expensive Suit from Nethack--
    - Shirt, +0 AC, Grants MC of 2 and +2 to Charisma.

- Added the Fire Helmet from SLASH 6.
    - Helmet, AC 1 and grants Fire Resistance.

- Added Gauntlets of Reflection
    - Grants reflection, count as silver for unarmed damage.

- Added Rubber Gloves
    - AC 1, Protects worn rings from shock damage.

- Added Gnomish Armor
    - Gnomish Boots: AC 0, appearance "little black boots"
    - Gnomish Helm: AC 0, appearance "little red hat"
    - Gnomish suit: AC 1, appearance "little blue vest"
    - These replace Low/High boots, Helmets, and Leather Armor for gnomes.

- Added Doughnut
    - Weight of 2, Nutrition of 20.

- Added the Ugly Backpack
    - Name and Color swap of Sack.  Not generated Randomly.

- Scrolls, Spellbooks, Potions, Food, Rocks, and Venom are no longer erodible.
- Roles can no longer randomly start with scrolls of root password detection, amulets of data storage, or rings of timely backup.
- Removed chance of uncharged wands spawning.
- Stethoscopes and Fishing Poles no longer have a chance to randomly break.
- T-shirts are the only readable shirts again.
- Ruffled Shirt and Victorian Underwear work the same as they do in dNethack
    - Removed auto-cursing and life saving
- Elemental resistance cloaks no longer have randomized appearances.
- Robes are not Cloth instead of Leather.
- All shirts (except the striped shirt) are no longer cursed in initial inventory.

- Removed the Helm of Steel, Gauntlets of Steel, and Steel Shield.
- Removed the Wand of Banishment.

### Dungeon
- Reverted dungeon probability changes (special levels always show up again)
- Reverted most of the special level dungeon placement changes.
    - Unheck levels now only show up between levels 20 and 39.
    - Hitchhiker, Computer and Key levels only show up between levels 15 and 29.
- Removed random mazes from being generated in the dungeon.
- Disabled large amounts of random dungeon features of being generated (bars, trees, stone).
- Fully re-implemented the Forge from Nethack: The Next Generation
    - The Forge level is generated with a 50% probability between levels 15 and 20.
    - Can pay to get an artifact created for you to use.
        - Only artifacts that are not sacrifice gifts, not set to NOGEN, and are weapons can be made here.
        - If Charisma is below 14, you offer a price and an artifact is created based on what you offered
        - If Charisma is above 14, you can ask for a specific artifact to be made along with bonuses (Enchantment, blessed, erodeproof).
        - Prices are variable based on Charisma (similar to buying from shopkeepers)
    - Blacksmith is much stronger now
        - Generated with a near-ascension kit similar to One-Eyed Sam.
        - Also generated with a unique artifact (See Warforger above)
- The Town branch has been changed
    - Town branch has been shortened from 5 levels to 3
    - Branch no longer has a chance of generating unheck, bigroom, or school levels.
    - The second level is now Pleasantville, a peaceful village with a thieves hideout that contains the staircase to Town.
- Unaligned altars are no longer generated randomly in the dungeon.
- Certain traps only generate at certain dungeon levels and below again.
- Special rooms only show up at certain dungeon levels and below based on difficulty.
- Entering a special room no longer wakes up all monsters in that room.
- Removed the alignment quest levels and planar levels from the heck and unheck files.
- Gnomish Mines walls are colored brown
    - This tends to not apply to minetown shops or the temple, instead it uses the standard (random) colors used in the dungeon.
- The Dragon Lord appears in the Wyrm Caves.
- Merged the Banging Shop with the Gun Shop
    - Removed the Banging Shop as a result.
- Removed the Zorkmid Store.
- Large bookstores and wand shops are converted into general stores again.
- Disabled random underground rivers from being generated
    - Re-enable by un-commenting UNDERGROUND_RIVERS in config.h

### Monsters
- Re-adjusted the probability of certain special effects to some attacks.
- Re-adjusted the probability of gaining intrinsics from eating monster corpses.
- Mindless monsters and animals can no longer wear armor nor use potions/scrolls/wands.
- Mindless pets no longer attack the player randomly.
- Monsters generated after level generation that can hide no longer generate objects to hide under.
    - Only snakes and spiders generate objects to hide under during level generation.
- Certain keystone kops are set to NOGEN.
- All S_EEL monsters lose HP on land again.
- Angels and Demons only say maledictions if they're in your line of sight.
   - Slash'EM monsters that couldn't say maledictions no longer can here.
- (Giant/Dark) Shoggoths now have (constantly forming) eyes
    - They are never blind for longer than one turn as a result.
- Re-enabled kops_gone in shk.c
- Small monsters no longer generate with large objects.
- Implemented Watch Bribery patch by L.

### Misc
- New versioning scheme.
- Added Server support (http://)
    - Implemented the Xlogfile and Livelog patch.
    - Added automatic unix recovery (Implemented by Eduardo Spadolini).
    - Added the FCNTL locking patch (Implemented by Eduardo Spadolini).
    - Added the Dgamelaunch simplemail patch (Implemented by Eduardo Spadolini).
- Fixed a makedef crash on windows
- A general cleanup of files
- Expanded list of Japanese and Pirate object terms.
- Slings gain a damage bonus based on character stregnth.
- Slings can Instakill giants at Expert skill level.
- Removed a large amount of redundant hallucination monster names.
- Removed a pointless graphic line of dialogue from seduction attacks.
- Restored the original gauntlet joke in eat.c and hack.c (Warrior was added)
- Utilize school.c for Teacher and Student dialogue instead of cluttering up sounds.c
- Implemented the Label Reading patch by L
    - Allows reading of Credit Cards, Magic Markers, Runed Objects, Tins, Candy Bars, Cans of Grease
    - Also allows you to flip a coin.
    - [BUG] Cannot read Coins or The Orb of Fate.
- A few upstream fixes
    - Fixed a rare segfault during kicking
    - Fixed a segfault when zapping a wand of digging inside gray newt.
    - Fixed artifacts having unintended effects and segfaults.
- Created an EASY_MODE define in config.h
    - All characters will start with 5 scrolls of healing and 2 scrolls of standard id
    - All monsters have a much higher chance of dropping scrolls of healing and standard id
    - All monsters have a slight chance of dropping cockatrice corpses/eggs and gloves.
    - Experience level requirements are halved.
