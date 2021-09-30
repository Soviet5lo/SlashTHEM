/*	SCCS Id: @(#)engrave.c	3.4	2001/11/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"
#include <ctype.h>

STATIC_VAR NEARDATA struct engr *head_engr;

#ifdef OVLB
/* random engravings */
static const char *random_mesg[] = {
	"Elbereth",
	/* trap engravings */
	"Vlad was here", "ad aerarium",
	/* take-offs and other famous engravings */
	"Owlbreath", "Galadriel",
	"Kilroy was here",
	"A.S. ->", "<- A.S.", /* Journey to the Center of the Earth */
	"You won't get it up the steps", /* Adventure */
	"Lasciate ogni speranza o voi ch'entrate.", /* Inferno */
	"Well Come", /* Prisoner */
	"We apologize for the inconvenience.", /* So Long... */
	"See you next Wednesday", /* Thriller */
	"notary sojak", /* Smokey Stover */
	"For a good time call 8?7-5309",
	"Please don't feed the animals.", /* Various zoos around the world */
	"Madam, in Eden, I'm Adam.", /* A palindrome */
	"Two thumbs up!", /* Siskel & Ebert */
	"Hello, World!", /* The First C Program */
	"You've got mail!", /* AOL */
	"May the force be with you!",
	"As if!", /* Clueless */
	/* [Tom] added these */
	"Y?v?l s??ks!", /* just kidding... */
	"T?m ?as h?r?",
	/* Tsanth added these */
	"Gazortenplatz", /* Tribute to David Fizz */
	"John 3:16", /* You see this everywhere; why not here? */
	"Exhale! Exhale! Exhale!", /* Prodigy */
	"All you need is love.", /* The Beatles */
	"Please don't feed the animals.", /* Various zoos around the world */
	"....TCELES B HSUP   A magic spell?", /* Final Fantasy I (US) */
	"Madam, in Eden, I'm Adam.", /* A palindrome */
	"Two thumbs up!", /* Siskel & Ebert */
	"Hello, World!", /* The First Program line */
	"Turn around.", /* Various people at various times in history */
	"You've got mail!", /* AOL */
	"UY WUZ HERE", /* :] */
	"Time flies when you're having fun.", /* Who said this first, anyway? */
	"As if!", /* Clueless */
	"How bizarre, how bizarre.", /* OMC */
	"Silly rabbit, Trix are for kids!", /* Trix */
 	"I'll be back!", /* Terminator */
 	"That is not dead which can eternal lie.", /* HPL */
	"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon",
	"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon",
	"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon cafe",
	"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon cafe",
	"**FR33 P0T10N 0F FULL H34L1NG**",
	"An HTML attachment was scrubbed...",
	"Are YOU tired of the same old messages? Go to http://alt.org/nethack/addmsgs/ now!",
	"Badger badger badger badger badger badger MUSHROOM MUSHROOM!",
	"CONGRATULATIONS! you are now the proud owner of 40 newts.",
	"Christmas Greetings!",
	"Crush the enemy, see them driven before you, and hear the lamentations of their women!",
	"Dear $playername, how do you spell Elbereth? Singed- A friend",
	"Dear $playername, your $fruit is in another castle.",
	"Dear Dudley, what would you say are your greatest weaknesses?",
	"Do you want to change that garter snake into a raging python?",
	"Do you want to start? Take the 5:45 Shibuya bound train from Jiyuugaoka Station",
	"Engraving Moloch works as engraving Elbereth. Just see how that a turns to flee from here!",
	"Find a small token to represent your Owlbear",
	"HOT inside stock tip - Yendor Industries (YNDR) STRONG BUY!",
	"Has anyone really been far even as decided to use even go want to do look more like?",
	"Have you heard of a spell called Explosive Runes?",
	"Hello.",
	"How are you gentlemen?",
	"Spam Hummmmmmmmmmmmmmmmmmmmmmm...",
	"I am Nigerian prince HORRE YED MOBUTU contacting you regarding the transfer of $10000000 zorkmids",
	"I've had it with you $playername !",
	"IM in ur base, killing ur doods!!!111",
	"LOL ROFL LMAO",
	"Lolol!",
	"MAKE ZORKMIDS FAST, GUARANTEED!",
	"Made you look!",
	"Magic-B-Here sells wands of not exactly finest quality! Bye today!",
	"Make a contract with me to become a puella magi!",
	"Me, too!",
	"PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS",
	"Play FREE roguelike game - http://crawl.akrasiac.org/",
	"Please return to sender (Asmodeus).",
	"Please send 15300 zorkmids security deposit to help me move 11 million (Z) out of Ludios!",
	"Pole Arm Wax! Only 350 Zk.",
	"Potion of generic C1al1s, only 10 zorkmids, satisfaction guaranteed!",
	"Red in the morning, sailor's warning.",
	"SEVEN DAYS",
	"Sale @ Izchak's! The first five visitors will get a free lamp!",
	"Scroll of punishment. Apply directly to the forehead.",
	"Send $5 to the return address, and get a free subscription to Thumbs magazine!",
	"Slash'EM Free Forever, Come Play My Lord *INSERT SUCCUBUS PIC HERE*",
	"Sword enlargement scrolls! Base price: 60 zorkmids!",
	"System shut down in 1 minute(s)",
	"TYPE 111 FOR HOT SUCCUBUS LOVIN'",
	"Thanx Maud",
	"The WOW starts now - http://www.microsoft.com/windowsvista/",
	"The best escort services! Meet the most intelligent succubi and incubi! Call 1-800-HOTDATE!",
	"This is Spam Enjoy!",
	"This mail complies with the Yendorian Anti-Spam Act (YASA).",
	"This message has been bounced. If you recieved this successfully, ignore this.",
	"This scroll was made with cockatrice leather. Have a nice day.",
	"This space intentionally left blank.",
	"Thou needst no longer toil with a mere quarterstaff!",
	"Try new magic Scroll! Up to three inches GUARANTEED!",
	"Undeliverable mail returned to sender: no such address <rodney@dungeons.doom>",
	"WAN7 +0 EN L4GE Y UR PEN???",
	"Watch out! The-Gnome-With-The-Wand-Of-Death is behind that door!",
	"While your character doesn't need socialising, remember that you do. Don't be a tool.",
	"YENDOR EATS CHEEZE WIZZ",
	"Yo Momma!",
	"You are about to die... so why not get a cheap LIFE INSURANCE?",
	"You are the 100 millionth person to read this message! Wait 500 turns for your FREE Iron Chain.",
	"You got mail. From AOL!",
	"You may already have won $1000!",
	"You're doing great! Keep up the good work!",
	"You've been pre-approved for the Platinum Yendorian Express card!",
	"Your Dad On A Sybian",
	"Your porn stash has been located. Have a good day.",
	"g3t bl3ss3d skrol1z of 3nch4n7 w34pon ch33p! add inchez to your Sword!",
	"http://www.tvtropes.org/",
	"qwertyuiopasdfghjklzxcvbnm",
	"wow. such meme. so epic. much random.",
	"Engrave a wise saying and your name will live forever. -- Anonymous",
	"You smite the vampire. It explodes! You seem unaffected by it.",
	"dilbert aint gay - tupac",
	"$playername sux!",
	"+7 Magicbane. Do you want to kill monsters, or do you want to fool around?",
	"--More--",
	"/join #nethack",
	"1/0",
	"1337",
	"2, 12, 1112, 3112, 132112, 1113122112, ...",
	"3ngr@Vr VVu$ }{3@r 3|-|d }{e 1eft $u/V-133tsp3@k!!!!1111!!!oneoneone",
	"4, 8, 15, 16, 23, 42",
	"666 420 Tongue Kiss Your Father",
	"<-- Go that way!",
	">:D",
	"A - cursed loadstone.",
	"A horse is a horse of course...",
	"Aaoooooooooooo! Werewolves of Yendor!",
	"All work and no play makes Jack a dull boy",
	"Allburrith",
	"Always Be Closing!",
	"At last... the amulet ^n my grasp..... hidden in the sec+et room on th! %3rd level...",
	"Bark!",
	"Beware the exploding cow!",
	"Blessed Greased +5 silver dragon scale mail buried here",
	"CAPS LOCK IS CRUISE CONTROL FOR COOL",
	"Call Before You Dig",
	"Cancel this engraving",
	"Chainsaw was here! RAAAAAAAGH!",
	"Clothed for inventory",
	"Come on, $playername, don't dawdle! You have almost missed the train!",
	"Computers: Can't live with 'em, can't play Nethack without 'em.",
	"Cool story, bro.",
	"Destroy the amulet on this level.",
	"Did you know the witch-woman jenka had a brother?",
	"Dig Here for a +5 pair of Speed Boots!",
	"Dig down!",
	"Dig for Victory here",
	"Dig here --- X",
	"Dig here for Iraq's WMDs",
	"Dig here.",
	"Do not feed the pink elephant",
	"Do not move forward",
	"Do not taunt happy fun cockatrice",
	"Don't Panic",
	"Don't go that way.",
	"Don't look behind you.",
	"Don't worry. You're still illiterate.",
	"Duck!",
	"E=mc ... no, that's not it.",
	"ERAU QSSI DRLO WEHT",
	"ETAOIN SHRDLU",
	"Eat at Joe's",
	"Elbcreth",
	"Elber",
	"Elberek? Albereth? Albert?",
	"Elberet",
	"Elbereth",
	"Elbereth",
	"Elbereth was engraved here, but it's no longer engraved here. You're too late, loser!",
	"Elberith Elbireth Elberth Lbereth Elbreth",
	"Elebird",
	"Eleburff",
	"Elf Breath",
	"Engraving with a hammer is hard!",
	"Environmentally sensitive area -- no farming",
	"Even in your NetHack game, F.O.E.!",
	"Find the computer room!",
	"First!",
	"For a good time call 867-5309",
	"For a good time, call 1-900-FOOCUBI!",
	"Friendlydads.net friendliest dads on the Net!",
	"Gaius Julius Primigenius was here. Why are you late?",
	"Galadriel",
	"Gandalf for president!",
	"Gatorade. Is it in you?",
	"George plonks ferrets.",
	"Get A BRAIN! MORANS",
	"Gilthoniel",
	"Go Left -->",
	"Go around! Go around and around and around! What fun! I'm so happy!",
	"Go see Snakes on a Plane.",
	"Got milk?",
	"Haermund Hardaxe carved these runes",
	"Hastur Hastur H%s+ur",
	"He who is valiant and pure of spirit may find the Holy Grail in the Castle of Arrrgh",
	"HeadOn! Apply directly to the forehead! HeadOn! Apply directly to the forehead!",
	"Hello world!",
	"Here be dragons.",
	"Here lies Bob; he tried an acid blob",
	"Here lies all hopes of a cheedar cheese pie.",
	"Here there be soldier ants.",
	"Hey, you! Stupid-looking kid! Look down here!",
	"Hoc est qui sumus.",
	"How much wood could a w**dc***k chuck if a w**dc****k could chuck wood?",
	"Htereble",
	"I AM ERROR.",
	"I am a fish.",
	"I bet you're wishing that you had the ability to press ctrl + w",
	"I couldn't afford a stone!",
	"I love you Stacy, please marry me! AArrghh!",
	"I prepared Exploding Runes this morning.",
	"I prepared Explosive Runes this morning.",
	"I warned you about stairs bro!",
	"I wasn't here!",
	"I wasn't kidding when I said pay! Now, you'll pay the ultimate price!!",
	"I went that way! -->",
	"I wonder what's for dinner...",
	"I'm George W. Bush, and I approve this message.",
	"I'm sorry, but the Amulet of Yendor is in another plane!",
	"IT HIURTS",
	"Ia! Ia! Cthulhu Fhtagn!",
	"If you can read this, you're standing too close",
	"If you find silver dragon scale mail on this spot, consider yourself lucky.",
	"In Soviet Russia, the amulet ascends with you!",
	"It explodes!",
	"It hits! It hits! You regret eating that one more quesadilla! You die... --More--",
	"It is dark. You may be eaten by a grue.",
	"It's a trick. Get an axe.",
	"Jesus was here 51 BC",
	"Joseph of Arimathea was here!",
	"June 5, 2009: I've been wandering these dungeons for days... I want to see my $playername again",
	"Keep off the grass!",
	"Kilroy was here.",
	"Klaatu barada nikto!",
	"Klein bottle for sale -- enquire within.",
	"Look behind you, a three-headed monkey!",
	"Look! Over there!",
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut...",
	"Madam, I'm Adam!",
	"Marvin was here.",
	"Maxim 1: Pillage, Then Burn",
	"Need a light? Come visit the Minetown branch of Izchak's Lighting Store!",
	"Need more time? Pray for the Time Goddess! **REQUIRES MONEY**",
	"No! Not another new-",
	"Nodnol 871 Selim",
	"Not Elbereth",
	"OH SH-",
	"OW! top TREADING ON ME!",
	"On this floor is engraved image of young $playername. The $playername is in fetal position.",
	"On this floor is masterfully engraved image of : and @. The newt bites the hero. The hero dies...",
	"Oo ee oo ah ah ting tang walla walla bing bang",
	"Open for inventory",
	"Owl Breath",
	"Owlbreath",
	"Pay Attention!",
	"Perception changes with perspective",
	"Please insert disk 2 to continue.",
	"Poot dispenser here",
	"Preeeeeeeeeeeeeow.",
	"REDRUM",
	"Rainbow Dash is the best pony.",
	"Real men play female characters.",
	"Rodney was here",
	"Rosebud.",
	"Run for it! It's a rigged bones file!",
	"Save Ferris!",
	"Save now, and do your homework!",
	"See the TURTLE of enormous girth! On his shell he holds the earth.",
	"Snakes on the Astral Plane - Soon in a dungeon near you",
	"Sometimes I feel that people can read me like an engraving...",
	"Sorry, $playername, but The Amulet of Yendor is on another dungeon level!",
	"Stuff to remember: don't kick sinks.",
	"The Gnome-with-a-Wand-of-Death was here.",
	"The Vibrating Square",
	"The Vibrating Square is NOT here",
	"The forgotten pet $dog has come! An overpowered vicious dog. Beware it's deadly bite!",
	"The grass is always greener on the other side of your sunglasses.",
	"The moon rules - #1",
	"The preparations are complete. There is no hope for $playername now.",
	"The treasure is buried uuddlrlrba from here.",
	"There is nothing engraved here. Really.",
	"There was a hole here. It's gone now.",
	"They say god saved the queen",
	"They say that They can never be trusted.",
	"They say that the Spider Caves have a wand of death... Oh wait... That was SLASH'EM, wrong game",
	"They say that there's a stalker behind you",
	"They say that you are what you eat.",
	"They say that you can't trust what they say.",
	"This engraving is totally unextrordinary. Wait! No! It says - no, it's unextrordinary.",
	"This is a pit!",
	"This is a pit!",
	"This is not the dungeon you are looking for.",
	"This message is false.",
	"This space intentionally left blank.",
	"This square deliberately left blank.",
	"This statement is false.",
	"This tile tells you that you need at least AC -15 to clear the castle.",
	"This way for egress.",
	"Toy Story 2 was OK.",
	"Try ?? , : :: /f !, You",
	"Twilight Sparkle has bacon hair.",
	"Unicorn skin tastes likes Jesus.",
	"Vlad wasn't here",
	"W.A.S.T.E.",
	"WARNING! Do not read this message.",
	"WARNING: CONSTRUCTION AHEAD.",
	"WOT, NO GYPSIES?",
	"WTF",
	"Warning, Exploding runes!",
	"Watch out! The-Gnome-With-The-Wand-Of-Death is behind that door!",
	"We are not amused.",
	"Where are the ats of yesteryear?",
	"Who is this foreign host and why does he keep closing my sessions?",
	"Why are you looking down here?",
	"Why are you reading this?",
	"Why's everybody always picking on me?",
	"X marks the spot",
	"X marks the spot.",
	"Yeah, well, y'know, that's just like, your opinion, man.",
	"You are already dead!",
	"You are here.",
	"You are here.",
	"You aren't going to ascend by staring at your shoes.",
	"You can't help being ugly, but you could stay home.",
	"You die...",
	"You die...",
	"You fall into a bottomless pit. You die...",
	"You step on a polymorph trap!",
	"Your Dad On A Sybian",
	"http://www.tvtropes.org/",
	"lol",
	"mde.tv",
	"mexican jumping beans",
	"rerecord don't fade away",
	"romanes eunt domus",
	"romanes eunt domus",
	"shawnz sux.",
	"you are the one millionth visitor on this place! please wait 200 turns for a wand of wishing.",
	/* From NAO */
	"Arooo!  Werewolves of Yendor!", /* gang tag */
	"Dig for Victory here", /* pun, duh */
	"Don't go this way",
	"Gaius Julius Primigenius was here.  Why are you late?", /* pompeii */
	"Go left --->", "<--- Go right",
	"Haermund Hardaxe carved these runes", /* viking graffiti */
	"Here be dragons",
	"Need a light?  Come visit the Minetown branch of Izchak's Lighting Store!",
	"Save now, and do your homework!",
	"Snakes on the Astral Plane - Soon in a dungeon near you!",
	"There was a hole here.  It's gone now.",
	"The Vibrating Square",
	"This is a pit!",
	"This is not the dungeon you are looking for.",
	"This square deliberately left blank.",
	"Warning, Exploding runes!",
	"Watch out, there's a gnome with a wand of death behind that door!",
	"X marks the spot",
	"X <--- You are here.",
	"You are the one millionth visitor to this place!  Please wait 200 turns for your wand of wishing.",

	/* http://www.alt.org/nethack/addmsgs/viewmsgs.php */
	"Badger badger badger badger badger badger MUSHROOM MUSHROOM!",
	"Dig here for a +5 pair of Speed Boots!",
	"If you find speed boots on this spot, consider yourself lucky.",
	"Gandalf for president!",
	"Fnord",
	"\"Engrave a wise saying and your name will live forever.\" -- Anonymous",
	"In Soviet Russia, the amulet ascends with you!",
	"Need a light? Come visit the Minetown branch of Izchak's Lighting Store!",
	"The Vibrating Square is NOT here",

	/* From UnNetHack */
	"She's watching you!", /* Portal */
	"The cake is a lie!", /* Portal */
	"cjohnson/tier3", /* Portal */
	"What's the sitch?", /* Kim Possible */
	"So not the drama.", /* Kim Possible */
	"Sudden decompression sucks!", /* Space Quest 3 death message */
	"Thanks for playing UnNetHackPlus.  As usual, you've been a real pantload!", /* Space Quest 4 death message, paraphrased */
	"Colorless green ideas sleep furiously.", /* Noam Chomsky */
	"Time flies like an arrow.", /* linguistic humor */
	"All Your Base Are Belong To Us!", /* Zero Wing */
	"Numa Numa", /* Internet Meme */
	"I want to believe!", /* X-Files */
	"Trust No One", /* X-Files */
	"The Truth is Out There", /* X-Files */
	"Look behind you, a Three-Headed Monkey!", /* Monkey Island */
	"Ifnkovhgroghprm", /* Kings Quest I */
	"Cookies are delicious delicacies", /* Mozilla Firefox */

	/* From UnNetHackPlus */
	"Losing is fun!", /* Dwarf Fortress */
	/* Murphy's laws */
	"Cheer up, the worst is yet to come...",
	"Computers let you waste time efficiently",
	"If something cannot go wrong at all, it will go wrong in a very spectacular way.",
	"If everything seems to be going well, you have obviously overlooked something",
	"If anything can go wrong, it will",
	"When things go from bad to worse, the cycle repeats.",
	"Being dead right, won't make you any less dead.",
	"If all else fails, hit it with a big hammer.",
	"If many things can go wrong, they will all go wrong at the same time.",
	/* end of Murphy's laws */
	"That is not dead which can eternal lie, and with strange aeons even death may die.", /* H. P. Lovecraft */
	"Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn.", /* H. P. Lovecraft */
	"Conducts are about player skill, not about obsessive-compulsive disorder", /* TJR on RGRN */
	"That is not dead which can eternal lie, and with strange aeons even vanilla might get released.", /* bhaak on http://nhqdb.alt.org/?301 */
	/* http://donjon.bin.sh/fantasy/random/#dungeon_graffiti */
	"When song becomes silence, the Wand of Aether shall be destroyed",
	"Upon the Night of Dweomers, when iron is made flesh and the Golden Scepter is wreathed in flames, the Copper Tower shall be restored",
	"Run away!",
	"<--",
	"-->",
	"This door leads to the women's locker room. Use a wand of make invisible on yourself before entering.",
	"You stand at the entrance to XXX SEX SHOP!!!",
	"Draw it! Draw it!",
	"Pong begin broadcasting!",
	"TEAM JACOB MOTHERFUCKERS. Gotta banish that douchebag Edward to Gehennom for all eternity.",
	"The effect of spray disappears.",
	"There is the tree with fruit!",
	"The sound is loud, water aboves fall!",
	"The door is locked. Can't be opened.",
	"You win? Very good. Now I believe the girls is good too.",
	"You ride a bike to advertise?",
	"A bright bike!",
	"Very lovrly!",
	"My son collect too, understand the monster, Zhenqian says so. The aid center of monster just called me.",
	"It's by my brother just now I wanted to tell you his tel number!",
	"The left passages do not have the ticket, how about them?",
	"Troubled you? Xiaojin comes welcome to take again!",
	"It is nothing the olden-tortoise water-pot is belonded to you.",
	"This time you will take it to the center of monster to be treated.",
	"Welcome! This is Xiaojin Store!",
	"There are sorts of things no if the game shop in this city don't go out to exchange.",
	"Ground floor is service counter for free.",
	"The second is a market store.",
	"The fourth is a drugstore.",
	"The fifth is the skill machine counter.",
	"The sixth is a square.",
	"The garret is watching tower.",
	"You can register ten with pocket gearing.",
	"Do you turn to the 'lucky channel' programme?",
	"You are very good!",
	"The coins increasing,and laughing endless!",
	"Welcome! Play the game, put the coin into there, exchange the goods!",
	"Machine is interesting compared with worm machine.",
	"This is a life games rotate quickly.",
	"Card machine, you can imagine easily!",
	"The cup to drink is there, it has a sweet taste!",
	"Welcome to the trade center here, the people who are stranger each other, exchange monaster casually.",
	"Does the monaster owned each other to be exchange now?",
	"It needs times to find the exchanged object, come after a while!",
	"You sit on the cockatrice corpse. It is not very comfortable.",
	"The mystic egg is prepared for you specially send it at once there are many rooms in the center.",
	"Make preparations now wait for a moment please!",
	"Before finding the news wrote a report please!",
	"Do you know the news?",
	"The data of the news is wrong - see you next!",
	"Then the monster you want will let her send it out. If everybody do so, we can communicate with various of people.",
	"It's a female cat called Anxiong whose name is as same as father's.",
	"The palace is writed how to use the radio.",
	"We will realize the wish of the coachs all over the world. Let's work hard!",
	"Do you pay homage to the spirit of the forest? The forest spirit surpasses space and time to protect the forest.",
	"She is the grass monster originally.",
	"It looks like the picture on the relic wall, if it is like as a monster, according to the reason there are many.",
	"The front.... En?What? You use the secret sign!",
	"I like to think of the question about the relic.",
	"This relic built 1500 years ago! But,who had built it.",
	"The libyan relic!",
	"The graduate school of libyan relic the question about relic!",
	"It is made patterning foregone monsters.",
	"This is the appearance drawn on the wall!",
	"There is a big hole on the wall!",
	"Welcome to relic small house! There are the appearance of monsters drawn remote time ago. Move it to look please!",
	"There is nothing just before you come there to look at the wall!",
	"Under your help to our investigation about the relic, we have a father understanding.",
	"The thing made patterning foregone monsters.",
	"A big room but nothing!",
	"A holy appearance, it is a mystic place!",
	"Good! The alteration for monsters badge is achieved! Supplement a unknown badge.",
	"Under your help to investigate we have known the relic, as if the relic was built for the monsters to inhabit!",
	"This is the first superintendent for libya graduate school the photo of Namuluoji Dr!",
	"Many profound books. 'Archaian relic' 'Riddle of archaian civilization'",
	"That is the soul of coach!",
	"It is yedong's tail to be cut off! There are some mails,do you want to read?",
	/* contributed by members of NetHackWiki */
	"Please disable the following security protocols: 2-18, 1-12, 1-0", /* the Submachine series */
	"Our coil is broken, and we don't have a replacement. It's useless. It's so useless.", 
	
	/* contributed by members of the Nethack Usenet group, rec.games.roguelike.nethack */
	"Which way did he go, George, which way did he go?",
	"Until you stalk and overrun, you can't devour anyone.",
	"Fee.  Fie.  Foe.  Foo.",
	"Violence is the last refuge of the incompetent.",
	"Be vewy vewy quiet.  I'm hunting wabbits.",
	"Eh, what's up, doc?",
	"We prefer your extinction to the loss of our job.",
	"You're weird, sir.",
	"I do not know for what reason the enemy is pursuing you, but I perceive that he is, strange indeed though that seems to me.",
	"When it is falling on your head, then you are knowing it is a rock.",
	"The very best mail is made from corbomite.",
	"Woah, duude, look at all the colors.",
	"I'm bringing home a baby bumblebee.",
	"Now, if you'll permit me, I'll continue my character assassination unimpeded.",
	"Cry havoc, and let slip the dogs of war.",
	"Virtue needs some cheaper thrills.",
	"I just saw Absolom hanging in an oak tree!",
	"And I may do anything I wish as long as I squeeze?",
	"Marsey Dotesin Dosey Dotesin Diddley Damsey Divey.",
	"I knew I should have taken that left turn at Albuquerque.",
	"YOU BROKE MY SWORD!",
	"An atom-blaster is a good weapon, but it can point both ways.",
	"You may have already won ten million zorkmids.",
	"They say the satisfaction of teaching makes up for the lousy pay.",
	"Tharr she blows!",
	"Perhaps today is a good day to die.",
	"Sufferin' succotash.",
	"To succeed, planning alone is insufficient. One must improvise as well.",
	"I love it when a plan comes together.",
	"Very fond of rumpots, crackpots, and how are you Mr. Wilson?",
	"No matter how valuable it might be, any man's life is still worth more than any animal's.",
	"No clock for this.  Flyin' now.",
	"Of course, you realize, this means war.",
	"She's fast enough for you, old man. What's the cargo?",
	"There're few who haven't heard the name of Gurney Halleck.",
	"Do you like mayonnaise and corn on your pizza?",
	"I am that merry wanderer of the night.",
	"Don't try any games.  Remember I can see your face if you can't see mine.",
	"Ah hates rabbits.",
	"Captain, there be whales here!",
	"Mimesis is a plant.  Go Bucks!",
	"That's all we live for, isn't it?  For pleasure, for titillation?",
	"The only brew for the brave and true comes from the Green Dragon.",
	"There is an old Vulcan proverb: only Nixon could go to China.",
	"Yo, put me in a trebuchet and launch me to the stratosphere.",
	"Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua.",
	"Sola lingua bona lingua mortua est.",
	"Mairzy Doats And Dozy Doats And Liddle Lamzy Divey A kiddley divey too, wouldn't you?",
	"Madam, I may be drunk, but you are ugly, and in the morning I shall be sober.",
	"Outside of a dog, a book is a man's best friend.",
	"Inside of a dog, it's too dark to read." ,
	"You are superior in only one aspect. You are better at dying.",
	"No McFly ever amounted to anything in the history of Hill Valley!",
	"You've got to ask yourself one question: 'Do I feel lucky?' Well, do ya, punk?",
	"In this pearl it is hard to see a Strange Loop.",
	"That is because the Strange Loop is buried in the oyster -- the proof.",
	"I love the smell of napalm in the morning.",
	"Et tu, Brute?",
	"The conditions were difficult. Several contestants were maimed, but... I was triumphant. I won Champion Standing.",
	"We're not gonna take it. No, we ain't gonna take it. We're not gonna take it anymore.",
	"Absurd!  Your 'MU' is as silly as a cow's moo.",
	"Hasta la vista, baby.",
	"I see...  I see...  I see a lavender man...  bearing tidings.",
	"It is a sign of creative genius to reconcile the seemingly disparate.",
	"It pays to be obvious, especially if you have a reputation for subtlety.",
	"Bother. I've left my new torch in Narnia",
	"If you put into the machine wrong figures, will the right answers come out?",
	"I am not able rightly to apprehend the kind of confusion of ideas that could provoke such a question.",
	"Colorless green ideas sleep furiously",
	"We fear a few dwarf watercraft stewardesses were regarded as desegregated after a great Texas tweezer war.",
	"I'll kill him in Honolulu, you unhook my pink kimono.",
	"The clown can stay, but the Ferengi in the gorilla suit has got to go.",
	"All roads lead to Trantor, and that is where all stars end.",
	"It is a far, far better thing I do than I have ever done. It is a far, far better rest I go to than I have ever known.",
	"Yes, this is Network 23. The net-net-net-net-network that puts it right... where they want you to be!",
	"You're listening to the E.I.B Radio Network.",
	"We shall this day light such a candle, by God's grace, in England, as I trust shall never be put out.",
	"How about a little fire, scarecrow?",
	"And if you're the kind of person who parties with a bathtub full of pasta, I suspect you don't care much about cholesterol anyway.",
	"I AM A SEXY SHOELESS GOD OF WAR!!!",
	
	"Bad Wolf", /* Dr. Who */
	"It's only the end if you want it to be.", /*Batgirl*/
	"Mistakes were made.",
	"But at what cost?",
	"For we wrestle not against flesh and blood, but against principalities and powers.", /* Ephesians 6:12 */
	"Life is truth, and never a dream...", /* Persona 4 */
	"fire, walk with me.", /*Twin Peaks*/
	"redefine happiness", /* Blue Oyster Cult */
	"no more sadness, no more anger, no more envy...", /*Halo*/
	"Even false things are true",	"FIVE TONS OF FLAX!",	"Fnord", /*Discordianism*/
	"Ash nazg durbatuluk, ash nazg gimbatul, ash nazg thrakatuluk, agh burzum-ishi krimpatul", /*the Lord of the Rings*/
	"No Fate", /*Terminator*/
	"This world can have but one Emperor.", /*FF II*/
	"We shall devour your light, and use it to return this world to the Void!", /*FFIII*/
	"My hatred will not be stanched until it has consumed all else!", /*FFV*/
	"Life... dreams... hope... Where do they come from? And where do they go...?", /*FFVI*/
	"I will NEVER be a memory.", /*FFVII Advent Children*/
	"All the world will be your enemy, Prince With A Thousand Enemies...", /*Watership Down*/
	"...and when they catch you they will kill you... but first they must catch you.",
	"This statement is false.", /*Logical contradiction*/
	"Quod Erat Demonstrandum",	"anno Domini",	"per centum", /* QED, AD, percent */
	"I was so surprised, I CLASSIFIED INFORMATION, I really did!", /*Haruhi Suzumiya*/
	"[REDACTED]",	"[DATA EXPUNGED]",	"[DATA PLUNGED]",	"[DATA EXPANDED]",	"I am a toaster!", /* SCP Foundation */
	"I prepared Explosive Runes this morning.", /*Order of the Stick*/
	"In any battle, there's always a level of force against which no tactics can succeed.",
	"1002: He is Not that Which He Says He Is.", /*Good Omens*/
	"3001: Behinde the Eagle's Neste a grate Ash hath fellen.",
	"VI. You Shall Not Subject Your God To Market Forces!",	/*Discworld*/
	"Nae quin! Nae laird! Nae master! We willna be fooled again!"
	"Morituri Nolumus Mori"
	"End-of-the-World Switch. PLEASE DO NOT TOUCH"
	"Give a man a fire and he's warm for a day, but set fire to him and he's warm for the rest of his life."
	"Do not, under any circumstances, open this door"
	"WORDS IN THE HEART CANNOT BE TAKEN"
	"Slab: Jus' say \"AarrghaarrghpleeassennononoUGH\""
	"Ahahahahaha! Ahahahaha! Aahahaha!\nBEWARE!!!!!\nYrs sincerely\nThe Opera Ghost"
	"Bee There Orr Bee A Rectangular Thyng"
	"NOT A Secret Base", /*Gunnerkrigg Court*/
	"Short to long term memory impaired. Go to Robotics Building. Explain about Bowman's architecture. Write down everything.", /*Freefall*/
	"I SAW THE EYE. Over the horizon, like a rising sun!", /* Dresden Codak */
	"That's Science, isn't it? The perpetual horizon.\nBut we found a way, haha, we found a way around that!",
	"What would YOU give up to... to see it through?",
	"Don't trust the skull.", /* Cynarfpncr: Gbezrag (mild spoiler) */
	"We do not threaten or warn or advise. We tell the truth.", /* The Mansion of E */
	"What you stole is even more dangerous and precious than you know.",
	"In the end, you will fly away and live forever, and you will leave the world shattered in your wake.",
	"You will return to where you started, and you will become what you hate.",
	"And in doing so, you will restore the world to what it was.",
	"Fall in a more hardboiled manner.", /* MS Paint Adventures */
	"Since you are reading this, chances are you have already installed this game on your computer.\nIf this is true, you have just participated in bringing about the end of the world.\nBut don't beat yourself up about it.",
	"YOU HATE TIME TRAVEL YOU HATE TIME TRAVEL YOU HATE....",
	"It begins to dawn on you that everything you just did may have been a colossal waste of time.",
	"We will BUILD heroes!", /* THE PROTOMEN */
	"If you replace the working parts, you get a different machine.",
	"If you destroy the working parts, what you get is a broken machine.",
	"Never tell me the odds!" /* Starwars */,
	"Needle-like the new log to know that SlashEM Extended are you ...",
};

char *
random_engraving(outbuf)
char *outbuf;
{
	const char *rumor;

	/* a random engraving may come from the "rumors" file,
	   or from the list above */
	if (!rn2(4) || !(rumor = getrumor(0, outbuf, TRUE)) || !*rumor)
	    Strcpy(outbuf, random_mesg[rn2(SIZE(random_mesg))]);

	wipeout_text(outbuf, (int)(strlen(outbuf) / 8), 0);
	return outbuf;
}

/* Partial rubouts for engraving characters. -3. */
static const struct {
	char		wipefrom;
	const char *	wipeto;
} rubouts[] = {
	{'A', "^"},     {'B', "Pb["},   {'C', "("},     {'D', "|)["},
	{'E', "|FL[_"}, {'F', "|-"},    {'G', "C("},    {'H', "|-"},
	{'I', "|"},     {'K', "|<"},    {'L', "|_"},    {'M', "|"},
	{'N', "|\\"},   {'O', "C("},    {'P', "F"},     {'Q', "C("},
	{'R', "PF"},    {'T', "|"},     {'U', "J"},     {'V', "/\\"},
	{'W', "V/\\"},  {'Z', "/"},
	{'b', "|"},     {'d', "c|"},    {'e', "c"},     {'g', "c"},
	{'h', "n"},     {'j', "i"},     {'k', "|"},     {'l', "|"},
	{'m', "nr"},    {'n', "r"},     {'o', "c"},     {'q', "c"},
	{'w', "v"},     {'y', "v"},
	{':', "."},     {';', ","},
	{'0', "C("},    {'1', "|"},     {'6', "o"},     {'7', "/"},
	{'8', "3o"}
};

void
wipeout_text(engr, cnt, seed)
char *engr;
int cnt;
unsigned seed;		/* for semi-controlled randomization */
{
	char *s;
	int i, j, nxt, use_rubout, lth = (int)strlen(engr);

	if (lth && cnt > 0) {
	    while (cnt--) {
		/* pick next character */
		if (!seed) {
		    /* random */
		    nxt = rn2(lth);
		    use_rubout = rn2(4);
		} else {
		    /* predictable; caller can reproduce the same sequence by
		       supplying the same arguments later, or a pseudo-random
		       sequence by varying any of them */
		    nxt = seed % lth;
		    seed *= 31,  seed %= (BUFSZ-1);
		    use_rubout = seed & 3;
		}
		s = &engr[nxt];
		if (*s == ' ') continue;

		/* rub out unreadable & small punctuation marks */
		if (index("?.,'`-|_", *s)) {
		    *s = ' ';
		    continue;
		}

		if (!use_rubout)
		    i = SIZE(rubouts);
		else
		    for (i = 0; i < SIZE(rubouts); i++)
			if (*s == rubouts[i].wipefrom) {
			    /*
			     * Pick one of the substitutes at random.
			     */
			    if (!seed)
				j = rn2(strlen(rubouts[i].wipeto));
			    else {
				seed *= 31,  seed %= (BUFSZ-1);
				j = seed % (strlen(rubouts[i].wipeto));
			    }
			    *s = rubouts[i].wipeto[j];
			    break;
			}

		/* didn't pick rubout; use '?' for unreadable character */
		if (i == SIZE(rubouts)) *s = '?';
	    }
	}

	/* trim trailing spaces */
	while (lth && engr[lth-1] == ' ') engr[--lth] = 0;
}

boolean
can_reach_floor()
{
	return (boolean)(!u.uswallow &&
#ifdef STEED
			/* Restricted/unskilled riders can't reach the floor */
			!(u.usteed && P_SKILL(P_RIDING) < P_BASIC) &&
#endif
			 (!Levitation ||
			  Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)));
}
#endif /* OVLB */
#ifdef OVL0

const char *
surface(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];

	if ((x == u.ux) && (y == u.uy) && u.uswallow &&
		is_animal(u.ustuck->data))
	    return "maw";
	else if (IS_AIR(lev->typ) && Is_airlevel(&u.uz))
	    return "air";
	else if (is_pool(x,y, FALSE))
	    return (Underwater && !Is_waterlevel(&u.uz)) ? "bottom" : "water";
	else if (is_ice(x,y))
	    return "ice";
	else if (is_lava(x,y))
	    return "lava";
	else if (lev->typ == DRAWBRIDGE_DOWN)
	    return "bridge";
	else if(IS_ALTAR(levl[x][y].typ))
	    return "altar";
	else if(IS_GRAVE(levl[x][y].typ))
	    return "headstone";
	else if(IS_FOUNTAIN(levl[x][y].typ))
	    return "fountain";
	else if(IS_PUDDLE(levl[x][y].typ))
	    return "muddy floor";
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
	    return "floor";
	else
	    return "ground";
}

const char *
ceiling(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];
	const char *what;

	/* other room types will no longer exist when we're interested --
	 * see check_special_room()
	 */
	if (*in_rooms(x,y,VAULT))
	    what = "vault's ceiling";
	else if (*in_rooms(x,y,TEMPLE))
	    what = "temple's ceiling";
	else if (*in_rooms(x,y,SHOPBASE))
	    what = "shop's ceiling";
	else if (IS_AIR(lev->typ))
	    what = "sky";
	else if (Underwater)
	    what = "water's surface";
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
	    what = "ceiling";
	else
	    what = "rock above";

	return what;
}

struct engr *
engr_at(x, y)
xchar x, y;
{
	register struct engr *ep = head_engr;

	while(ep) {
		if(x == ep->engr_x && y == ep->engr_y)
			return(ep);
		ep = ep->nxt_engr;
	}
	return((struct engr *) 0);
}

#ifdef ELBERETH
/* Decide whether a particular string is engraved at a specified
 * location; a case-insensitive substring match used.
 * Ignore headstones, in case the player names herself "Elbereth".
 */
int
sengr_at(s, x, y)
	const char *s;
	xchar x, y;
{
	register struct engr *ep = engr_at(x,y);

	return (ep && ep->engr_type != HEADSTONE &&
		ep->engr_time <= moves && strstri(ep->engr_txt, s) != 0);
}
#endif /* ELBERETH */

#endif /* OVL0 */
#ifdef OVL2

void
u_wipe_engr(cnt)
register int cnt;
{
	if (can_reach_floor())
		wipe_engr_at(u.ux, u.uy, cnt);
}

#endif /* OVL2 */
#ifdef OVL1

void
wipe_engr_at(x,y,cnt)
register xchar x,y,cnt;
{
	register struct engr *ep = engr_at(x,y);

	/* Headstones are indelible */
	if(ep && ep->engr_type != HEADSTONE){
	    if(ep->engr_type != BURN || is_ice(x,y)) {
		if(ep->engr_type != DUST && ep->engr_type != ENGR_BLOOD) {
			cnt = rn2(1 + 50/(cnt+1)) ? 0 : 1;
		}
		wipeout_text(ep->engr_txt, (int)cnt, 0);
		while(ep->engr_txt[0] == ' ')
			ep->engr_txt++;
		if(!ep->engr_txt[0]) del_engr(ep);
	    }
	}
}

#endif /* OVL1 */
#ifdef OVL2

boolean
sense_engr_at(x,y,read_it)
register int x,y;
boolean read_it; /* Read any sensed engraving */
{
	register struct engr *ep = engr_at(x,y);
	register int	sensed = 0;
	char buf[BUFSZ];
	
	/* Sensing an engraving does not require sight,
	 * nor does it necessarily imply comprehension (literacy).
	 */
	if(ep && ep->engr_txt[0]) {
	    switch(ep->engr_type) {
	    case DUST:
		if(!Blind) {
			sensed = 1;
			pline("%s is written here in the %s.", Something,
				is_ice(x,y) ? "frost" : IS_PUDDLE(levl[x][y].typ) ? "mud" : "dust");
		}
		break;
	    case ENGRAVE:
	    case HEADSTONE:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
			pline("%s is engraved here on the %s.",
				Something,
				surface(x,y));
		}
		break;
	    case BURN:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
			pline("Some text has been %s into the %s here.",
				is_ice(x,y) ? "melted" : "burned",
				surface(x,y));
		}
		break;
	    case MARK:
		if(!Blind) {
			sensed = 1;
			pline("There's some graffiti on the %s here.",
				surface(x,y));
		}
		break;
	    case ENGR_BLOOD:
		/* "It's a message!  Scrawled in blood!"
		 * "What's it say?"
		 * "It says... `See you next Wednesday.'" -- Thriller
		 */
		if(!Blind) {
			sensed = 1;
			You("see a message scrawled in blood here.");
		}
		break;
	    default:
		impossible("%s is written in a very strange way.",
				Something);
		sensed = 1;
	    }
	    if (sensed && !read_it &&
			    flags.suppress_alert < FEATURE_NOTICE_VER(0,0,7)) {
		pline("Use \"r.\" to read it.");
	    } else if (sensed && read_it) {
	    	char *et;
	    	unsigned len, maxelen = BUFSZ - sizeof("You feel the words: \"\". ");
	    	len = strlen(ep->engr_txt);
	    	if (len > maxelen) {
	    		(void)strncpy(buf,  ep->engr_txt, (int)maxelen);
			buf[maxelen] = '\0';
			et = buf;
		} else
			et = ep->engr_txt;

		/* If you can engrave an 'x', you can "read" it --ALI */
		if (len != 1 || (!index(et, 'x') && !index(et, 'X')))
			u.uconduct.literate++;

		You("%s: \"%s\".",
		      (Blind) ? "feel the words" : "read",  et);
		if(flags.run > 1) nomul(0, 0);
		return TRUE;
	    }
	}
	return FALSE;
}

#endif /* OVL2 */
#ifdef OVLB

void
make_engr_at(x,y,s,e_time,e_type)
register int x,y;
register const char *s;
register long e_time;
register xchar e_type;
{
	register struct engr *ep;

	if ((ep = engr_at(x,y)) != 0)
	    del_engr(ep);
	ep = newengr(strlen(s) + 1);
	ep->nxt_engr = head_engr;
	head_engr = ep;
	ep->engr_x = x;
	ep->engr_y = y;
	ep->engr_txt = (char *)(ep + 1);
	Strcpy(ep->engr_txt, s);
	/* engraving Elbereth shows wisdom */
	if (!in_mklev && !strcmp(s, "Elbereth")) exercise(A_WIS, TRUE);
	ep->engr_time = e_time;
	ep->engr_type = e_type > 0 ? e_type : rnd(N_ENGRAVE-1);
	ep->engr_lth = strlen(s) + 1;
}

/* delete any engraving at location <x,y> */
void
del_engr_at(x, y)
int x, y;
{
	register struct engr *ep = engr_at(x, y);

	if (ep) del_engr(ep);
}

/*
 *	freehand - returns true if player has a free hand
 */
int
freehand()
{
	return(!uwep || !welded(uwep) ||
	   (!bimanual(uwep) && (!uarms || !uarms->cursed)));
/*	if ((uwep && bimanual(uwep)) ||
	    (uwep && uarms))
		return(0);
	else
		return(1);*/
}

static NEARDATA const char styluses[] =
	{ ALL_CLASSES, ALLOW_NONE, TOOL_CLASS, WEAPON_CLASS, WAND_CLASS,
	  GEM_CLASS, RING_CLASS, 0 };

/* Mohs' Hardness Scale:
 *  1 - Talc		 6 - Orthoclase
 *  2 - Gypsum		 7 - Quartz
 *  3 - Calcite		 8 - Topaz
 *  4 - Fluorite	 9 - Corundum
 *  5 - Apatite		10 - Diamond
 *
 * Since granite is a igneous rock hardness ~ 7, anything >= 8 should
 * probably be able to scratch the rock.
 * Devaluation of less hard gems is not easily possible because obj struct
 * does not contain individual oc_cost currently. 7/91
 *
 * steel     -	5-8.5	(usu. weapon)
 * diamond    - 10			* jade	     -	5-6	 (nephrite)
 * ruby       -  9	(corundum)	* turquoise  -	5-6
 * sapphire   -  9	(corundum)	* opal	     -	5-6
 * topaz      -  8			* glass      - ~5.5
 * emerald    -  7.5-8	(beryl)		* dilithium  -	4-5??
 * aquamarine -  7.5-8	(beryl)		* iron	     -	4-5
 * garnet     -  7.25	(var. 6.5-8)	* fluorite   -	4
 * agate      -  7	(quartz)	* brass      -	3-4
 * amethyst   -  7	(quartz)	* gold	     -	2.5-3
 * jasper     -  7	(quartz)	* silver     -	2.5-3
 * onyx       -  7	(quartz)	* copper     -	2.5-3
 * moonstone  -  6	(orthoclase)	* amber      -	2-2.5
 */

/* return 1 if action took 1 (or more) moves, 0 if error or aborted */
int
doengrave()
{
	boolean dengr = FALSE;	/* TRUE if we wipe out the current engraving */
	boolean doblind = FALSE;/* TRUE if engraving blinds the player */
	boolean doknown = FALSE;/* TRUE if we identify the stylus */
	boolean eow = FALSE;	/* TRUE if we are overwriting oep */
	boolean jello = FALSE;	/* TRUE if we are engraving in slime */
	boolean ptext = TRUE;	/* TRUE if we must prompt for engrave text */
	boolean teleengr =FALSE;/* TRUE if we move the old engraving */
	boolean zapwand = FALSE;/* TRUE if we remove a wand charge */
	xchar type = DUST;	/* Type of engraving made */
	char buf[ENGBUFSZ];	/* Buffer for final/poly engraving text */
	char ebuf[ENGBUFSZ];	/* Buffer for initial engraving text */
	char qbuf[QBUFSZ];	/* Buffer for query text */
	char post_engr_text[ENGBUFSZ]; /* Text displayed after engraving prompt */
	const char *everb;	/* Present tense of engraving type */
	const char *eloc;	/* Where the engraving is (ie dust/floor/...) */
	char *sp;		/* Place holder for space count of engr text */
	int len;		/* # of nonspace chars of new engraving text */
	int maxelen;		/* Max allowable length of engraving text */
	struct engr *oep = engr_at(u.ux,u.uy);
				/* The current engraving */
	struct obj *otmp;	/* Object selected with which to engrave */
	char *writer;

	multi = 0;		/* moves consumed */
	nomovemsg = (char *)0;	/* occupation end message */

	buf[0] = (char)0;
	ebuf[0] = (char)0;
	post_engr_text[0] = (char)0;
	maxelen = ENGBUFSZ; /* a maximum of 31 active Elbereths on a single square --Amy */
	if (is_demon(youmonst.data) || youmonst.data->mlet == S_VAMPIRE)
	    type = ENGR_BLOOD;

	/* Can the adventurer engrave at all? */

	if(u.uswallow) {
		if (is_animal(u.ustuck->data)) {
			pline("What would you write?  \"Jonah was here\"?");
			return(0);
		} else if (is_whirly(u.ustuck->data)) {
			You_cant("reach the %s.", surface(u.ux,u.uy));
			return(0);
		} else
			jello = TRUE;
	} else if (is_lava(u.ux, u.uy)) {
		You_cant("write on the lava!");
		return(0);
	} else if (is_pool(u.ux,u.uy, FALSE) || IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		You_cant("write on the water!");
		return(0);
	}
	if(Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)/* in bubble */) {
		You_cant("write in thin air!");
		return(0);
	}
	if(IS_AIR(levl[u.ux][u.uy].typ)) {
		You_cant("write on an unsolid surface!");
		return(0);
	}
	if (cantwield(youmonst.data)) {
		You_cant("even hold anything!");
		return(0);
	}
	if (check_capacity((char *)0)) return (0);

	/* One may write with finger, or weapon, or wand, or..., or...
	 * Edited by GAN 10/20/86 so as not to change weapon wielded.
	 */

	otmp = getobj(styluses, "write with");
	if(!otmp) return(0);		/* otmp == zeroobj if fingers */

	if (otmp == &zeroobj) writer = makeplural(body_part(FINGER));
	else writer = xname(otmp);

	/* There's no reason you should be able to write with a wand
	 * while both your hands are tied up.
	 */
	if (!freehand() && otmp != uwep && !otmp->owornmask) {
		You("have no free %s to write with!", body_part(HAND));
		return(0);
	}

	if (jello) {
		You("tickle %s with your %s.", mon_nam(u.ustuck), writer);
		Your("message dissolves...");
		return(0);
	}
	if (otmp->oclass != WAND_CLASS && !can_reach_floor()) {
		You_cant("reach the %s!", surface(u.ux,u.uy));
		return(0);
	}
	if (IS_ALTAR(levl[u.ux][u.uy].typ)) {
		You("make a motion towards the altar with your %s.", writer);
		altar_wrath(u.ux, u.uy);
		return(0);
	}
	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (otmp == &zeroobj) { /* using only finger */
		You("would only make a small smudge on the %s.",
			surface(u.ux, u.uy));
		return(0);
	    } else if (!levl[u.ux][u.uy].disturbed) {
		You("disturb the undead!");
		levl[u.ux][u.uy].disturbed = 1;
		(void) makemon(&mons[PM_GHOUL], u.ux, u.uy, NO_MM_FLAGS);
		exercise(A_WIS, FALSE);
		return(1);
	    }
	}

	/* SPFX for items */

	switch (otmp->oclass) {
	    default:
	    case AMULET_CLASS:
	    case CHAIN_CLASS:
	    case POTION_CLASS:
	    case COIN_CLASS:
		break;

	    case RING_CLASS:
		/* "diamond" rings and others should work */
	    case GEM_CLASS:
		/* diamonds & other hard gems should work */
		if (objects[otmp->otyp].oc_tough) {
			type = ENGRAVE;
			break;
		}
		break;

	    case ARMOR_CLASS:
		if (is_boots(otmp)) {
			type = DUST;
			break;
		}
		/* fall through */
	    /* Objects too large to engrave with */
	    case BALL_CLASS:
	    case ROCK_CLASS:
		You_cant("engrave with such a large object!");
		ptext = FALSE;
		break;

	    /* Objects too silly to engrave with */
	    case FOOD_CLASS:
	    case SCROLL_CLASS:
	    case SPBOOK_CLASS:
		Your("%s would get %s.", xname(otmp),
			is_ice(u.ux,u.uy) ? "all frosty" : "too dirty");
		ptext = FALSE;
		break;

	    case RANDOM_CLASS:	/* This should mean fingers */
		break;

	    /* The charge is removed from the wand before prompting for
	     * the engraving text, because all kinds of setup decisions
	     * and pre-engraving messages are based upon knowing what type
	     * of engraving the wand is going to do.  Also, the player
	     * will have potentially seen "You wrest .." message, and
	     * therefore will know they are using a charge.
	     */
	    case WAND_CLASS:
		if (zappable(otmp)) {
		    check_unpaid(otmp);
		    zapwand = TRUE;
		    if (Levitation) ptext = FALSE;

		    switch (otmp->otyp) {
		    /* DUST wands */
		    default:
			break;

			/* NODIR wands */
		    case WAN_LIGHT:
		    case WAN_SECRET_DOOR_DETECTION:
		    case WAN_CREATE_MONSTER:
		    case WAN_CREATE_HORDE:
		    case WAN_ENLIGHTENMENT:
		    case WAN_MAGIC_MAPPING:
		    case WAN_DARKNESS:
		    case WAN_IDENTIFY:
		    case WAN_WISHING:
		    case WAN_ACQUIREMENT:
		    case WAN_BUGGING:
			zapnodir(otmp);
			break;

			/* IMMEDIATE wands */
			/* If wand is "IMMEDIATE", remember to affect the
			 * previous engraving even if turning to dust.
			 */
		    case WAN_STRIKING:
			Strcpy(post_engr_text,
			"The wand unsuccessfully fights your attempt to write!"
			);
			break;
		    case WAN_ACID:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s seem to be covered with goo!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_WIND:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the floor are blown away!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SOLAR_BEAM:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The engraving on the %s shines brightly for a moment!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SLOW_MONSTER:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s slow down!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SPEED_MONSTER:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s speed up!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_HEALING:
		    case WAN_EXTRA_HEALING:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s look healthier!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_FEAR:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s run away!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_POLYMORPH:
			if(oep)  {
			    if (!Blind) {
				type = (xchar)0;	/* random */
				(void) random_engraving(buf);
			    }
			    dengr = TRUE;
			}
			break;
		    case WAN_DRAINING:	/* KMH */
			if (oep) {
			    /*
			     * [ALI] Wand of draining give messages like
			     * either polymorph or cancellation/make
			     * invisible depending on whether the
			     * old engraving is completely wiped or not.
			     * Note: Blindness has slightly different
			     * effect than with wand of polymorph.
			     */
			    u_wipe_engr(5);
			    oep = engr_at(u.ux,u.uy);
			    if (!Blind) {
				if (!oep)
				    pline_The("engraving on the %s vanishes!",
				      surface(u.ux,u.uy));
				else {
				    strcpy(buf, oep->engr_txt);
				    dengr = TRUE;
				}
			    }
			}
			break;
		    case WAN_CLONE_MONSTER:
			if (oep) {
			    if (!Blind)
				    pline_The("engraving is blurred, and you think you can see it twice!");
			    }
			break;
		    case WAN_NOTHING:
		    case WAN_UNDEAD_TURNING:
		    case WAN_OPENING:
		    case WAN_LOCKING:
		    case WAN_PROBING:
			break;

			/* RAY wands */
		    case WAN_MAGIC_MISSILE:
			ptext = TRUE;
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The %s is riddled by bullet holes!",
				   surface(u.ux, u.uy));
			}
			break;

		    /* can't tell sleep from death - Eric Backus */
		    case WAN_SLEEP:
		    case WAN_DEATH:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s stop moving!",
				   surface(u.ux, u.uy));
			}
			break;

		    case WAN_COLD:
			if (!Blind)
			    Strcpy(post_engr_text,
				"A few ice cubes drop from the wand.");
			if(!oep || (oep->engr_type != BURN))
			    break;
		    case WAN_CANCELLATION:
		    case WAN_MAKE_INVISIBLE:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
				pline_The("engraving on the %s vanishes!",
					surface(u.ux,u.uy));
			    dengr = TRUE;
			}
			break;
		    case WAN_TELEPORTATION:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
				pline_The("engraving on the %s vanishes!",
					surface(u.ux,u.uy));
			    teleengr = TRUE;
			}
			break;

		    /* type = ENGRAVE wands */
		    case WAN_DIGGING:
			ptext = TRUE;
			type  = ENGRAVE;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of digging!",
				xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind)
			    Strcpy(post_engr_text,
				IS_GRAVE(levl[u.ux][u.uy].typ) ?
				"Chips fly out from the headstone." :
				is_ice(u.ux,u.uy) ?
				"Ice chips fly up from the ice surface!" :
				IS_PUDDLE(levl[u.ux][u.uy].typ) ?
				"Mud and water fly up from the puddle." :
				"Gravel flies up from the floor.");
			else
			    Strcpy(post_engr_text, "You hear drilling!");
			break;

		    /* type = BURN wands */
		    case WAN_FIRE:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
			    pline("This %s is a wand of fire!", xname(otmp));
			    doknown = TRUE;
			}
			Strcpy(post_engr_text,
				Blind ? "You feel the wand heat up." :
					"Flames fly from the wand.");
			break;
		    case WAN_FIREBALL:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
			    pline("This %s is a wand of fireballs!", xname(otmp));
			    doknown = TRUE;
			}
			Strcpy(post_engr_text,
				Blind ? "You feel the wand heat up." :
					"Flames fly from the wand.");
			break;
		    case WAN_LIGHTNING:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of lightning!",
					xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind) {
			    Strcpy(post_engr_text,
				    "Lightning arcs from the wand.");
			    doblind = TRUE;
			} else
			    Strcpy(post_engr_text, "You hear crackling!");
			break;

		    /* type = MARK wands */
		    /* type = ENGR_BLOOD wands */
		    }
		} else /* end if zappable */
		    if (!can_reach_floor()) {
			You_cant("reach the %s!", surface(u.ux,u.uy));
			return(0);
		    }
		break;

	    case WEAPON_CLASS:
		if(otmp->otyp == SHARPENED_PENCIL) {
			if ((int)otmp->spe > -1){
//				if(!otmp->blessed && !rn2(5+(otmp->spe))) {
				if(!otmp->blessed && (rnl(10) > 5)) {
				    Your("Pencil lead breaks!");
				    otmp->spe = -1;
				    return(0);
				}
				else
				    type = ENGRAVE;
			}
			else {
				Your("pencil lead is broken!");
				return(0);
			}
		}
		else if(is_blade(otmp)) {
		    if ((int)otmp->spe > -3)
			type = ENGRAVE;
		    else
			Your("%s too dull for engraving.", aobjnam(otmp,"are"));
		}
		break;

	    case TOOL_CLASS:
		if(otmp == ublindf) {
		    pline(
		"That is a bit difficult to engrave with, don't you think?");
		    return(0);
		}

#ifdef LIGHTSABERS
		if (is_lightsaber(otmp)) {
		    if (otmp->lamplit) type = BURN;
		    else Your("%s is deactivated!", aobjnam(otmp,"are"));
		} else
#endif
		switch (otmp->otyp)  {
		    case MAGIC_MARKER:
			if (otmp->spe <= 0)
			    Your("marker has dried out.");
			else
			    type = MARK;
			break;
		    case TOWEL:
			/* Can't really engrave with a towel */
			ptext = FALSE;
			if (oep)
			    if ((oep->engr_type == DUST ) ||
				(oep->engr_type == ENGR_BLOOD) ||
				(oep->engr_type == MARK )) {
				if (!Blind)
				    You("wipe out the message here.");
				else
				    Your("%s %s %s.", xname(otmp),
					 otense(otmp, "get"),
					 is_ice(u.ux,u.uy) ?
					 "frosty" : "dusty");
				dengr = TRUE;
			    } else
				Your("%s can't wipe out this engraving.",
				     xname(otmp));
			else
			    Your("%s %s %s.", xname(otmp), otense(otmp, "get"),
				  is_ice(u.ux,u.uy) ? "frosty" : "dusty");
			break;
		    default:
			break;
		}
		break;

	    case VENOM_CLASS:
#ifdef WIZARD
		if (wizard) {
		    pline("Writing a poison pen letter??");
		    break;
		}
#endif
	    case ILLOBJ_CLASS:
		impossible("You're engraving with an illegal object!");
		break;
	}

	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (type == ENGRAVE || type == 0)
		type = HEADSTONE;
	    else {
		/* ensures the "cannot wipe out" case */
		type = DUST;
		dengr = FALSE;
		teleengr = FALSE;
		buf[0] = (char)0;
	    }
	}

	/* End of implement setup */

	/* Identify stylus */
	if (doknown) {
	    makeknown(otmp->otyp);
	    more_experienced(0,10);
	}

	if (teleengr) {
	    rloc_engr(oep);
	    oep = (struct engr *)0;
	}

	if (dengr) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Something has changed the engraving here */
	if (*buf) {
	    make_engr_at(u.ux, u.uy, buf, moves, type);
	    pline_The("engraving looks different now.");
	    ptext = FALSE;
	}

	if (zapwand && (otmp->spe < 0)) {
	    pline("%s %sturns to dust.",
		  The(xname(otmp)), Blind ? "" : "glows violently, then ");
	    if (!IS_GRAVE(levl[u.ux][u.uy].typ))
		You("are not going to get anywhere trying to write in the %s with your dust.",
		    is_ice(u.ux,u.uy) ? "frost" : "dust");
	    useup(otmp);
	    otmp = 0; /* wand is now gone */
	    ptext = FALSE;
	}

	if (!ptext) {		/* Early exit for some implements. */
	    if (otmp && otmp->oclass == WAND_CLASS && !can_reach_floor())
		You_cant("reach the %s!", surface(u.ux,u.uy));
	    return(1);
	}

	/* Special effects should have deleted the current engraving (if
	 * possible) by now.
	 */

	if (oep) {
	    register char c = 'n';

	    /* Give player the choice to add to engraving. */

	    if (type == HEADSTONE) {
		/* no choice, only append */
		c = 'y';
	    } else if ( (type == oep->engr_type) && (!Blind ||
		 (oep->engr_type == BURN) || (oep->engr_type == ENGRAVE)) ) {
		c = yn_function("Do you want to add to the current engraving?",
				ynqchars, 'y');
		if (c == 'q') {
		    pline(Never_mind);
		    return(0);
		}
	    }

	    if (c == 'n' || Blind) {

		if( (oep->engr_type == DUST) || (oep->engr_type == ENGR_BLOOD) ||
		    (oep->engr_type == MARK) ) {
		    if (!Blind) {
			You("wipe out the message that was %s here.",
			    ((oep->engr_type == DUST)  ? "written in the dust" :
			    ((oep->engr_type == ENGR_BLOOD) ? "scrawled in blood"   :
							 "written")));
			del_engr(oep);
			oep = (struct engr *)0;
		    } else
		   /* Don't delete engr until after we *know* we're engraving */
			eow = TRUE;
		} else
		    if ( (type == DUST) || (type == MARK) || (type == ENGR_BLOOD) ) {
			You(
			 "cannot wipe out the message that is %s the %s here.",
			 oep->engr_type == BURN ?
			   (is_ice(u.ux,u.uy) ? "melted into" : "burned into") :
			   "engraved in", surface(u.ux,u.uy));
			return(1);
		    } else
			if ( (type != oep->engr_type) || (c == 'n') ) {
			    if (!Blind || can_reach_floor())
				You("will overwrite the current message.");
			    eow = TRUE;
			}
	    }
	}

	eloc = surface(u.ux,u.uy);
	switch(type){
	    default:
		everb = (oep && !eow ? "add to the weird writing on" :
				       "write strangely on");
		break;
	    case DUST:
		everb = (oep && !eow ? "add to the writing in" :
				       "write in");
		eloc = (is_ice(u.ux,u.uy) ? "frost" : IS_PUDDLE(levl[u.ux][u.uy].typ) ? "mud" : "dust");
		break;
	    case HEADSTONE:
		everb = (oep && !eow ? "add to the epitaph on" :
				       "engrave on");
		break;
	    case ENGRAVE:
		everb = (oep && !eow ? "add to the engraving in" :
				       "engrave in");
		break;
	    case BURN:
		everb = (oep && !eow ?
			( is_ice(u.ux,u.uy) ? "add to the text melted into" :
					      "add to the text burned into") :
			( is_ice(u.ux,u.uy) ? "melt into" : "burn into"));
		break;
	    case MARK:
		everb = (oep && !eow ? "add to the graffiti on" :
				       "scribble on");
		break;
	    case ENGR_BLOOD:
		everb = (oep && !eow ? "add to the scrawl on" :
				       "scrawl on");
		break;
	}

	/* Tell adventurer what is going on */
	if (otmp != &zeroobj)
	    You("%s the %s with %s.", everb, eloc, doname(otmp));
	else
	    You("%s the %s with your %s.", everb, eloc,
		makeplural(body_part(FINGER)));

	/* Prompt for engraving! */
	Sprintf(qbuf,"What do you want to %s the %s here?", everb, eloc);
	getlin(qbuf, ebuf);

	/* Count the actual # of chars engraved not including spaces */
	len = strlen(ebuf);
	for (sp = ebuf; *sp; sp++) if (isspace(*sp)) len -= 1;

	if (len == 0 || index(ebuf, '\033')) {
	    if (zapwand) {
		if (!Blind)
		    pline("%s, then %s.",
			  Tobjnam(otmp, "glow"), otense(otmp, "fade"));
		return(1);
	    } else {
		pline(Never_mind);
		return(0);
	    }
	}

	/* A single `x' is the traditional signature of an illiterate person */
	if (len != 1 || (!index(ebuf, 'x') && !index(ebuf, 'X')))
	    u.uconduct.literate++;

	/* Mix up engraving if surface or state of mind is unsound.
	   Note: this won't add or remove any spaces. */

	/* The game is difficult enough already. Let's improve the player's chances to engrave. --Amy */

	for (sp = ebuf; *sp; sp++) {
	    if (isspace(*sp)) continue;
	    if (((type == DUST || type == ENGR_BLOOD) && !rn2(30)) ||
		    (Blind && !rn2(25)) || (Confusion && !rn2(20)) ||
		    (Stunned && !rn2(15)) || (Hallucination && !rn2(18)))
		*sp = ' ' + rnd(96 - 2);	/* ASCII '!' thru '~'
						   (excludes ' ' and DEL) */
	}

	/* Previous engraving is overwritten */
	if (eow) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Figure out how long it took to engrave, and if player has
	 * engraved too much.
	 */
	switch(type){
	    default:
		multi = -(len/10);
		if (multi) nomovemsg = "You finish your weird engraving.";
		break;
	    case DUST:
		multi = -(len/10);
		if (multi) nomovemsg = "You finish writing in the dust.";
		break;
	    case HEADSTONE:
	    case ENGRAVE:
		multi = -(len/10);
		if (otmp->otyp == WAN_DIGGING) multi /= 5; /* otherwise engraving with them would be useless --Amy */
		if ((otmp->oclass == WEAPON_CLASS) &&
		    ( ((otmp->otyp != ATHAME) && (!is_lightsaber(otmp)) && (otmp->otyp !=SHARPENED_PENCIL)) || otmp->cursed)) {
		    multi = -len;
		    maxelen = ((otmp->spe + 3) * 2) + 1;
			/* -2 = 3, -1 = 5, 0 = 7, +1 = 9, +2 = 11
			 * Note: this does not allow a +0 anything (except
			 *	 an athame) to engrave "Elbereth" all at once.
			 *	 However, you could now engrave "Elb", then
			 *	 "ere", then "th".
			 */
		    Your("%s dull.", aobjnam(otmp, "get"));
		    if (otmp->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
			    You("damage it, you pay for it!");
			    bill_dummy_object(otmp);
			}
		    }
		    if (len > maxelen) {
			multi = -maxelen;
			otmp->spe = -3;
		    } else if (len > 1)
			otmp->spe -= len >> 1;
		    else otmp->spe -= 1; /* Prevent infinite engraving */
		} else
		    if ( (otmp->oclass == RING_CLASS) ||
			 (otmp->oclass == GEM_CLASS) )
			multi = -len;
		if (multi) nomovemsg = "You finish engraving.";
		break;
	    case BURN:
		multi = -(len/10);
		if (multi)
		    nomovemsg = is_ice(u.ux,u.uy) ?
			"You finish melting your message into the ice.":
			"You finish burning your message into the floor.";
		break;
	    case MARK:
		multi = -(len/10);
		if ((otmp->oclass == TOOL_CLASS) &&
		    (otmp->otyp == MAGIC_MARKER)) {
		    maxelen = (otmp->spe) * 2; /* one charge / 2 letters */
		    if (len > maxelen) {
			Your("marker dries out.");
			otmp->spe = 0;
			multi = -(maxelen/10);
		    } else
			if (len > 1) otmp->spe -= len >> 1;
			else otmp->spe -= 1; /* Prevent infinite grafitti */
		}
		if (multi) nomovemsg = "You finish defacing the dungeon.";
		break;
	    case ENGR_BLOOD:
		multi = -(len/10);
		if (multi) nomovemsg = "You finish scrawling.";
		break;
	}

	/* Chop engraving down to size if necessary */
	if (len > maxelen) {
	    for (sp = ebuf; (maxelen && *sp); sp++)
		if (!isspace((int)*sp)) maxelen--;
	    if (!maxelen && *sp) {
		*sp = (char)0;
		if (multi) nomovemsg = "You cannot write any more.";
		You("only are able to write \"%s\"", ebuf);
	    }
	}

	/* Add to existing engraving */
	if (oep) Strcpy(buf, oep->engr_txt);

	(void) strncat(buf, ebuf, (ENGBUFSZ - (int)strlen(buf) - 1));

	make_engr_at(u.ux, u.uy, buf, (moves - multi), type);

	if (post_engr_text[0]) pline(post_engr_text);

	if (doblind && !resists_blnd(&youmonst)) {
	    You("are blinded by the flash!");
	    make_blinded((long)rnd(20),FALSE);
	    if (!Blind) Your(vision_clears);
	}

	return(1);
}

void
save_engravings(fd, mode)
int fd, mode;
{
	register struct engr *ep = head_engr;
	register struct engr *ep2;
	unsigned no_more_engr = 0;

	while (ep) {
	    ep2 = ep->nxt_engr;
	    if (ep->engr_lth && ep->engr_txt[0] && perform_bwrite(mode)) {
		bwrite(fd, (genericptr_t)&(ep->engr_lth), sizeof(ep->engr_lth));
		bwrite(fd, (genericptr_t)ep, sizeof(struct engr) + ep->engr_lth);
	    }
	    if (release_data(mode))
		dealloc_engr(ep);
	    ep = ep2;
	}
	if (perform_bwrite(mode))
	    bwrite(fd, (genericptr_t)&no_more_engr, sizeof no_more_engr);
	if (release_data(mode))
	    head_engr = 0;
}

void
rest_engravings(fd)
int fd;
{
	register struct engr *ep;
	unsigned lth;

	head_engr = 0;
	while(1) {
		mread(fd, (genericptr_t) &lth, sizeof(unsigned));
		if(lth == 0) return;
		ep = newengr(lth);
		mread(fd, (genericptr_t) ep, sizeof(struct engr) + lth);
		ep->nxt_engr = head_engr;
		head_engr = ep;
		ep->engr_txt = (char *) (ep + 1);	/* Andreas Bormann */
		/* mark as finished for bones levels -- no problem for
		 * normal levels as the player must have finished engraving
		 * to be able to move again */
		ep->engr_time = moves;
	}
}

void
del_engr(ep)
register struct engr *ep;
{
	if (ep == head_engr) {
		head_engr = ep->nxt_engr;
	} else {
		register struct engr *ept;

		for (ept = head_engr; ept; ept = ept->nxt_engr)
		    if (ept->nxt_engr == ep) {
			ept->nxt_engr = ep->nxt_engr;
			break;
		    }
		if (!ept) {
		    impossible("Error in del_engr?");
		    return;
		}
	}
	dealloc_engr(ep);
}

/* randomly relocate an engraving */
void
rloc_engr(ep)
struct engr *ep;
{
	int tx, ty, tryct = 200;

	do  {
	    if (--tryct < 0) return;
	    tx = rn1(COLNO-3,2);
	    ty = rn2(ROWNO);
	} while (engr_at(tx, ty) ||
		!goodpos(tx, ty, (struct monst *)0, 0));

	ep->engr_x = tx;
	ep->engr_y = ty;
}


/* Epitaphs for random headstones */
static const char *epitaphs[] = {
	"Rest in peace",
	"R.I.P.",
	"Rest In Pieces",
	"Note -- there are NO valuable items in this grave",
	"1994-1995. The Longest-Lived Hacker Ever",
	"The Grave of the Unknown Hacker",
	"We weren't sure who this was, but we buried him here anyway",
	"Sparky -- he was a very good dog",
	"Beware of Electric Third Rail",
	"Made in Taiwan",
	"Og friend. Og good dude. Og died. Og now food",
	"Beetlejuice Beetlejuice Beetlejuice",
	"Look out below!",
	"Please don't dig me up. I'm perfectly happy down here. -- Resident",
	"Postman, please note forwarding address: Gehennom, Asmodeus's Fortress, fifth lemure on the left",
	"Mary had a little lamb/Its fleece was white as snow/When Mary was in trouble/The lamb was first to go",
	"Be careful, or this could happen to you!",
	"Soon you'll join this fellow in hell! -- the Wizard of Yendor",
	"Caution! This grave contains toxic waste",
	"Sum quod eris",
	"Here lies an Atheist, all dressed up and no place to go",
	"Here lies Ezekiel, age 102.  The good die young.",
	"Here lies my wife: Here let her lie! Now she's at rest and so am I.",
	"Here lies Johnny Yeast. Pardon me for not rising.",
	"He always lied while on the earth and now he's lying in it",
	"I made an ash of myself",
	"Soon ripe. Soon rotten. Soon gone. But not forgotten.",
	"Here lies the body of Jonathan Blake. Stepped on the gas instead of the brake.",
	"Go away!",
	/* From SLASH'EM */
	"This old man, he played one, he played knick-knack on my thumb.",
	"Don't be daft, they couldn't hit an elephant at this dist-",
	"I wonder what deleting 'system32' does?",
	"If wizards can do the tengu shuffle, I bet tourists can do the fox-trot...",
	"Kai su, teknon?",
	"So this is what it feels like to be buried alive. - Last words of Indiana Jones 1889-1946",
	"This is what happens to you if you try to kill me - The Chromatic Dragon.",
	"Wonder what this mushroom does?...",
	"Y'all watch this!",
	"You call that a punch? I'll show you a punch--",
	"$dog",
	"$playername experienced YASD",
	"$playername took war to Team Ant and bought this fine track of land as benefits.",
	"$playername, Hand of Elbereth",
	"$playername, Killed at the hands of the Random Number God.",
	"$playername, demonstrating the value of not being seen.",
	"$playername, killed by a time-travel accident",
	"$playername, killed by an ambush, while reading",
	"$playername, killed by one-more-hit syndrome",
	"'Tis but a scratch!",
	"'Tis just a flesh wound...",
	"(This space for sale.)",
	"*Reserved for You*",
	"--More--",
	"...",
	"...for famous men have the whole earth as their memorial",
	"0AJQp",
	"0B6x8",
	"0EfGr",
	"0O7dW",
	"0X5mr",
	"2Bnxa",
	"2zr4S",
	"3sDlZ",
	"4ba9W",
	"4iyBA",
	"4nLCG",
	"4oY9C",
	"5cfQi",
	"5h5J0",
	"6dq9u",
	"6i9ma",
	"6q909",
	"6sEe6",
	"6z28Z",
	"7WRTy",
	"7jN66",
	"83gTA",
	"9h5Gq",
	"9qdzJ",
	"9wRBD",
	"<Privet!>!",
	"A is for Amy who fell down the stairs.",
	"A last bastion of rationality in a world where imbecility reigns supreme.",
	"A man of few words to the very end, now a man of even fewer words.",
	"A man's character is his fate",
	"A warning: Avoid the oysters at the local buffet line.",
	"ADkS1",
	"AFK",
	"ALL YOUR BASE ARE BELONG TO US",
	"ASYcW",
	"About the Zote what can be said? There was just one and now it's dead.",
	"Ach, Hans, run! It's the lhurgoyf!",
	"Acta est fabula.",
	"Ag friend. Eg good dude. Ig died. Og now Ig",
	"Alas fair Death, 'twas missed in life - some peace and quiet from my wife",
	"Alas, poor Bob. He almost asended. Only to fall into a pit of spikes.",
	"Alas, poor Yorick!",
	"Always look on the bright side of life",
	"Another one bites the dust.",
	"Any advice on risk management?",
	"Anyone wanna switch places?",
	"Apparently, you *can't* kill huge green fierce dragons with your bare hands.",
	"Applaud, my friends, the comedy is finished.",
	"As you are now, I once was. As I am now, you shall be. Um, unless you're immortal.",
	"At last... a nice long sleep.",
	"At least I didn't get eaten by $dog or $cat.",
	"Ate that little packet of silica gel labelled Do Not Eat.",
	"Audi Partem Alteram",
	"Aut neca aut necatis eris",
	"B is for Basil, assaulted by bears",
	"B0spY",
	"BPEsx",
	"Beetlejuice Beetlejuice Beetlejuice",
	"Beloved sister, devoted friend. She saved the world a lot.",
	"Beware the killer bunny!",
	"Blasted here by a SLASH'EM rocket.",
	"Bulk deals on gravestones - Die today!",
	"Burninated.",
	"COME AT ME BRO",
	"Casper the Friendly Boy",
	"Clint Eastwood",
	"Closed for inventory",
	"Come on Mary, shoot! You couldn't hit the broad side of a barn!",
	"Cut off the 'Do not remove under penalty of law' tag on a matress. Got death penalty.",
	"CzXSY",
	"David Blaine 1973 -",
	"Dead Men Tell no Tales",
	"Death is but a door. Time is but a window. I'll be back!",
	"Death seeketh us all.",
	"Deleted system32",
	"Desynchronized...",
	"Did you know that death rays bounce?",
	"Die? [yn] (n)",
	"Died by fighting what I thought that purple h was a dwarf king was actually a mind flayer.",
	"Died by losing the game.",
	"Died? Again? Damn.",
	"Dig me up for a good time",
	"Digging up this grave can create arch-lich",
	"Disco - July 12, 1979",
	"DjjFu",
	"Do not open until Christmas",
	"Do not play while drinking.",
	"Don't Try",
	"Don't do whatever it was I did wrong.",
	"Don't even think about reaching for that pickaxe shorty! or I'll havta bite ya.",
	"Don't forget to stop and smell the roses",
	"Don't let this happen to you!",
	"Don't try the fish",
	"Dulce et decorum est pro patria mori",
	"E6Cdf",
	"EAT AT JOES",
	"ECCPz",
	"EGE6X",
	"ET IN ARCADIA EGO",
	"Eaten by a Grue",
	"EdQji",
	"Enter here for invisible zombie rave!",
	"Fatty and skinny went to bed. Fatty rolled over and skinny was dead. Skinny Smith 1983-2000.",
	"Feed me. I'm starving.... come to think of it... i havn't eatin' since 2002.",
	"Fgsfds",
	"Finally I am becoming stupider no more",
	"Flooring inspector.",
	"Follow me to hell.",
	"Follow me to hell.",
	"Fool me once, shame on you. Fool me twice, ...",
	"For you, scum; only 700,000,000 for this grave reserve.",
	"GE8BT",
	"Game over, man. Game over.",
	"Ganked again...",
	"George W. Bush had everything and look where he ended up. No.. not up there u idiot... down here.",
	"Go Team Ant!",
	"Go Team Ant!",
	"Go away",
	"Go away! I'm trying to take a nap in here! Bloody adventurers...",
	"Go to Hell - Do not pass go, do not collect 200 zorkmids",
	"God, Killed by Logic",
	"Gone fishin'",
	"Good night, sweet prince: And flights of angels sing thee to thy rest!",
	"Greg didn't fit anywhere, accept in this grave!",
	"Greg the Ghoul's All Nite Diner: All you can eat, under your feet",
	"Guybrush Threepwood, Mitey Pirate",
	"GwNmL",
	"H is for Hector done in by a thug",
	"HEEM SLEEPY",
	"Hanniwa",
	"Haters gonna hate",
	"He died at the console, of hunger and thirst. Next day he was buried, face-down, nine-edge first.",
	"He farmed his way here",
	"He shouldn't have fed it.",
	"He waited too long",
	"Help! I'm trapped in a gravestone factory!",
	"Here Lies Patient Zero",
	"Here Rest The Three Stooges",
	"Here lays Butch, we planted him raw, he was quick on the trigger but slow on the draw.",
	"Here lies $cat killed by $playername",
	"Here lies $playername. He never scored.",
	"Here lies /usr/bin/nethack, killed by SIGSEGV.",
	"Here lies @",
	"Here lies Acid blob killed by bob",
	"Here lies Balin, son of Fundin, Lord of Moria",
	"Here lies Bob / decided to try an acid blob",
	"Here lies Bugs Bunny. His doc couldn't figure out what was up.",
	"Here lies Chester Moores, killed by a poisoned corpse. Stayed dead - his killer has no re-Moores.",
	"Here lies Curious George. Reached for a candy bar, crushed by a morning star.",
	"Here lies Dudley, ate a ring of death while polymorphed (ever tried?)",
	"Here lies Dudley, killed by an exploding ring of full warning",
	"Here lies Dudley, killed by another %&#@#& newt.",
	"Here lies Dudley. Drank a potion of death (ever found one?)",
	"Here lies Ed. He was a dork.",
	"Here lies Edd. He was an even bigger dork.",
	"Here lies Eddie. He lied, he cheated, he stole. Sadly he's dead, God bless his soul.",
	"Here lies Elric killed by touching Stormbringer",
	"Here lies Elric, killed by touching Stormbringer",
	"Here lies Elvis. Yes, he's really dead.",
	"Here lies Ezekiel. Died of cholera.",
	"Here lies Fred/ Fred the dead/ Fred was in bed/ Then he ate Bread/ Now Fred is dead.",
	"Here lies God, killed by Nietzsche",
	"Here lies Gregg. Choked on an egg.",
	"Here lies Gronkh, blasted by a Creeper",
	"Here lies Henry. May he find that place called Paradise.",
	"Here lies Heywood Ucuddleme, a orc who never had a hug.",
	"Here lies Izchak, shopkeeper and shopmaker.",
	"Here lies Jeff, he angered the shopkeeper, again",
	"Here lies Joe Hacker, starved to death while playing nethack.",
	"Here lies Johnny Blake/ Stepped on the gas instead of the brake.",
	"Here lies Lament. Killed by nerdrage over a wand of wishing.",
	"Here lies Lester Moore. 4 shots from a .44. No less... No Moore.",
	"Here lies Lies. It's True.",
	"Here lies Mallory Pike. She had a mean serve.",
	"Here lies Nietzsche, killed by Gog",
	"Here lies Original Bubs. He can see you from here.",
	"Here lies Owen Moore; gone away, owin' more than he could pay.",
	"Here lies Paul. You knew this was coming.",
	"Here lies Poot. He is dead.",
	"Here lies Sgeo, killed while reading a gravestone.",
	"Here lies Steve Irwin: Rememberance for his incredible journeys.",
	"Here lies Strong Bad, Checking e-mails and kicking Cheats in the hereafter,",
	"Here lies The Lady's maid, died of a Vorpal Blade.",
	"Here lies Tom killed by Jerry",
	"Here lies Urist McDwarf. Do NOT strike the earth here, please.",
	"Here lies Urist McMiner, killed by falling multiple z-levels.",
	"Here lies a hacker, buried alive.",
	"Here lies a happy hacker, killed by a hallucinogen-distorted succubus, while helpless.",
	"Here lies a hobbit of the Shire, died of a wand of fire.",
	"Here lies a jerk who forgot to pay.",
	"Here lies a man who never backed down.",
	"Here lies a man. His name is Robert Paulson.",
	"Here lies a man. His name is Robert Paulson.",
	"Here lies a programmer. Killed by a fatal error.",
	"Here lies a samurai named Ken.",
	"Here lies an Irishman, a good lawyer and an honest man. It's pretty crowded down there.",
	"Here lies andy. Peperony and chease.",
	"Here lies foo. Killed by 4chan.",
	"Here lies good old Ned. A great big rock fell on his head.",
	"Here lies ron3090, killed by kicking a ******* wall.",
	"Here lies ron3090, killed by kicking a fucking wall.",
	"Here lies scwizard, killed while tying to assent.",
	"Here lies the Gnome with a Wand of Death, vanquished by a black dragon's breath. Damn RNG.",
	"Here lies the body of Johann Gambolputty de von Ausfern-schplenden-schlitter-crasscrenbon --More--",
	"Here lies the dumbest son-of-a-b**** ever to set foot in the Dungeons of Doom.",
	"Here lies the dumbest son-of-a-bitch ever to set foot in the Dungeons of Doom.",
	"Here lies the english language, killed by instant messaging",
	"Here lies the left foot of Jack, killed by a land mine. Let us know if you find any more of him.",
	"Here lies the man from Nantucket. He was attacked by a orc, what was left was put into a bucket.",
	"Here lies the planet earth. don't dig it.",
	"Here lies yetanotherhacker, killed by the wrath of RNG",
	"Here lies you, in preparation.",
	"Hey, get off my grave!",
	"His last words? 'What throne room?'",
	"Hmm... What's tha--",
	"I Used Up All My Sick Days, So I Called in Dead",
	"I admit no wrongdoing!",
	"I ain't even mad.",
	"I am not human. This mind and body are constructs. Yes, as is this sorrow.",
	"I drank *what*, now?",
	"I iz ded. Ded kittehs dont need bafs.",
	"I need rest...",
	"I never could get the hang of Thursdays.",
	"I said I'm not dead yet!",
	"I told him it wasn't edible...",
	"I told you I was ill",
	"I told you I was sick!",
	"I took both pills!",
	"I used to search for the Amulet of Yendor, but then I took an arrow to the knee.",
	"I was thirsty.",
	"I will survive!",
	"I wonder what cockatrice tastes like..",
	"I wonder what that little ^ sign is...",
	"I wonder what this button does?",
	"I wonder what this potion does...",
	"I wonder what this potion does...",
	"I'd rather be sailing.",
	"I'll be back.",
	"I'll live to die another day... or not.",
	"I'm actually quite busy on the cellular level.",
	"I'm gonna make it!",
	"I've really been a fool",
	"IWwKl",
	"If a man's deeds do not outlive him, of what value is a mark in stone?",
	"If only I had worn a hard hat . . .",
	"If you were dead, you'd be home by now!",
	"In honor of Dudley, choked to death on a ring of invisibility",
	"In remembrance of Spike Milligan: He told you he was sick.",
	"It looks like I'm going to have to jump...",
	"It's nothing... it's nothing...",
	"It's only a flesh wound; nothing serious.",
	"James Brown is dead.",
	"Jeanne -- She died and we did nothing.",
	"JoXoD",
	"JrDSt",
	"Just one more hit . . .",
	"Just resting a while.",
	"KYNMh",
	"Killed by a black dragon -- This grave is empty",
	"Killed by a succubus with AIDS.",
	"Killed by an improbability",
	"Killed by playing SLASH'EM",
	"Killed in a teleporting accident.",
	"Killed over a dilithium crystal.",
	"Last Words: Cheeseburger Please.",
	"Last words are for wimps",
	"Leeroy Jenkins - Killed by overconfidence",
	"Let me out of here!",
	"Life's a piece of shit / when you look at it / life's a laugh and death's a joke, it's true.",
	"Lions and Tigers and Bears! Oh My!",
	"Little Johnny was a chemist. Now he is no more. 'Cause what he thought was H20, was H2SO4.",
	"Lm1xg",
	"LnAwY",
	"Lookin' good, Medusa.",
	"Lorem Ipsum",
	"LqAWq",
	"M-M-M-M-M-MONSTER KILL!",
	"MDrdP",
	"MMg5P",
	"Mental note: Hold sword by blunt end.",
	"Mistakenly performed a sword swallowing stunt with a vorpal blade.",
	"Mrs. Smith, choked on an apple. She left behind grieving husband, daughter, and granddaughter.",
	"My only regret was eating my 40th pancake",
	"My only regret was not having had more soda. -- Keith Forbes",
	"Myrnd",
	"N is for Nevill who died of ennui",
	"N9MDx",
	"NONE OF YOU ARE SAFE",
	"NOOOOOOOOOOOOOOOOOOOOOOOOOOOO!!!!!!!!",
	"Nah, that scorpion wasn't poisonous, i'm sure of it.",
	"National Park. Digging is prohibited.",
	"Never get involved in a land war in Asia.",
	"Never go in against a Sicilian when death is on the line.",
	"Newb",
	"Nietzsche is dead. -- God",
	"NmcOi",
	"No! I don't want to see my damn conduct!",
	"Nobody believed her when she said her feet were killing her",
	"Not another ******* floating eye.",
	"Not another fucking floating eye.",
	"Nqtb1",
	"OMG LAG",
	"OYl1A",
	"Oh Zed, Woe Zed. No where left to go Zed. All alone, and so dead. Poor Zed.",
	"Oh man I am so psyched for this.",
	"Oh, no. Not again.",
	"Omae wa mo shindeiru!",
	"On Vacation",
	"On the whole, I'd rather be in Minetown.",
	"One corpse, sans head.",
	"Only YOU can prevent fountain overflow",
	"Oooh, what's this scroll do?",
	"Oops.",
	"Out to Lunch",
	"PLK0H",
	"Paula Schultz",
	"PhbRM",
	"Pwnd!",
	"Q57Et",
	"Q8Qwi",
	"QBywK",
	"Quidquid Latine dictum sit, altum sonatur.",
	"Quintili Vare, legiones redde!",
	"R.I.P John Jacob Jingleheimer Schmidt - His name was my name too",
	"R3PmA",
	"RIP @",
	"RIP Bort Simpesson",
	"RIP Chingy, fatally killed by wasps.",
	"RIP Chingy, stung by wasps.",
	"RIP Guy Fieri, stung by wasps.",
	"RIP Stebe Job, stung to death by wasps.",
	"RIP, better you than me",
	"Repeat after me: Hastur Hastur Hastur!",
	"RmR0w",
	"Roses are red, Violets are blue, Omae Wa Mo Shindeiru.",
	"RtEjE",
	"S6qzn",
	"S8MXt",
	"SOLD",
	"Save me Jebus!",
	"Segmentation fault: core dumped.",
	"Shouldn't have given away my last weapon.",
	"Sic Transit Gloria Mundi",
	"Since the day of my birth, my death began its walk. It is walking towards me, without hurrying.",
	"Since when does a failed polymorph kill you?",
	"Slipped while mounting a succubus.",
	"Snape, killed by a Giant Mummy. The Giant Mummy hits! You die...",
	"So much time, so little to do",
	"Some crazy bastard waved me over so he could hit me. Why did I go over there?",
	"Someone set us up the bomb!",
	"Something in my inventory could have saved me.",
	"SsHwF",
	"Stan's Kozy Krypts: A Place To Spend Eternity, Not A Fortune.",
	"Stan's Kozy Krypts: We bury the dead, for a lot less bread.",
	"Steven Paul Jobs, 1955-2011. Thanks for the future.",
	"TO LET",
	"TONIGHT! WE DINE! IN HELL!",
	"TVTropes ruined my life",
	"TXfJ9",
	"Take my stuff- I don't need it anymore.",
	"Taking a year dead for tax reasons.",
	"Ted Danson",
	"Teracotta Warrior",
	"That is not dead which can eternal lie, and in successful ascensions even Death may die.",
	"That is not dead which can eternal lie. I, on the other hand...",
	"That is not dead which can eternal lie. And with strange aeons even death may die.",
	"That zombie had armor. I wanted armor.",
	"That zombie had armor. I wanted armor.",
	"The cake is a lie!",
	"The invisible jabberwock hits! You die...",
	"The keys were right next to each other",
	"The reports of my demise are completely accurate.",
	"The statue got me high.",
	"There goes the neighborhood.",
	"This Space Reserved",
	"This could by YOU!",
	"This grave is reserved - for you!",
	"This gravestone does not indicate a bones file.",
	"This gravestone provided to you by FreeStone.",
	"This headstone intentionally left blank.",
	"This is a gravestone, not an altar Trololololololol",
	"This is on me.",
	"This man died because he sucked at nethack.",
	"This space for rent; go to http://www.alt.org/nethack/addmsgs/",
	"This space intentionally left blank.",
	"This was a triumph.",
	"This was actually just a pit, but since there was a corpse, we filled it.",
	"This way to the crypt.",
	"TlK5K",
	"To the strongest!",
	"Trolls just want to be loved!",
	"Tu quoque, Brute?",
	"Turns out, you really shouldn't use the phone during a thunderstorm.",
	"Twilight Sparkle, killed while fighting sapient $fruit.",
	"Uisne Agnoscere Mercem?",
	"UsiJF",
	"VACANCY",
	"WHO'S THEY AND WHY DO THEY KEEP SAYING IT? - Died to bad advice.",
	"Wait for that wisest of all counselors, Time",
	"We introduced Eg to the giant, but he misunderstood.",
	"We wonder what kil",
	"Welcome!",
	"What could possibly go wrong?",
	"What happen when I press the button?",
	"What the hell is genocidal confusion?",
	"When you find yourself in a hole, stop digging.",
	"When you zap a wand of death, make sure you're pointing it the right way.",
	"Where were you Kos?",
	"Wheres the beef?!",
	"While we are postponing, life speeds by.",
	"Whoops.",
	"Whoops....",
	"Why, oh why didn't I take the BLUE pill?",
	"Wipe your feet before entering",
	"Wish me best of luck in my future endeavors....",
	"Wish you were here!",
	"Worse things happen at sea, you know.",
	"WqLjM",
	"X66P9",
	"XXxyZ",
	"Y is for Yorick whose head was knocked in",
	"Y.A.S.D.",
	"YAAD",
	"YASD",
	"YASD'd!",
	"Yea, it got me too.",
	"You asked me if it would kill me to be civil...well, now you know.",
	"You come from nothing, you're going back to nothing. What have you lost? Nothing!",
	"You die... --More--",
	"You should see the other guy.",
	"You're never too dead to learn.",
	"You're standing on my head.",
	"Your very silence shows you agree",
	"Zed's dead, baby.",
	"Zed's dead, baby. Zed's dead.",
	"addFU",
	"asasa",
	"asdg",
	"brb",
	"bvOwX",
	"cAWZB",
	"cGf2N",
	"cg5Vx",
	"confusion will be my epitaph",
	"cxjUZ",
	"cy4Wa",
	"doQmo",
	"dxekm",
	"edw",
	"eee3e",
	"enPWu",
	"fW6Ye",
	"fj2In",
	"fopBQ",
	"g3cwM",
	"g5NJc",
	"gIWDx",
	"gwNNZ",
	"hVhfq",
	"here lies Krass the cheapskate... still owes me 10 Gold for engraving this gravestone...",
	"here lies andy -- peperoni and cheese",
	"hukQY",
	"j16IV",
	"jVHLs",
	"jeBrG",
	"kGIOZ",
	"lgALT",
	"logout;",
	"lol",
	"lzBU6",
	"m4xlc",
	"mde.tv",
	"mkelO",
	"mv170",
	"mxQ0M",
	"n8jyF",
	"never try to kill a shopkeeper who has a wand of sleep, it'll be the last thing you do.",
	"njnm",
	"oPbQR",
	"oPlHb",
	"ojN8O",
	"on7ZP",
	"or2SE",
	"p0RFR",
	"pWRbl",
	"pz6Yk",
	"q4JhO",
	"qJOkM",
	"qnnZA",
	"quit, on",
	"rACim",
	"rest in peace dudley killed by a newt AGAIN!!!!",
	"rlDrk",
	"romanes eunt domus",
	"ruNc2",
	"sX2l3",
	"sbpzV",
	"slXdb",
	"swyyI",
	"test",
	"test test",
	"tlbto",
	"tm5QP",
	"try #quit",
	"u5fPY",
	"uE54o",
	"uWD2q",
	"uoZXt",
	"wK3qm",
	"wKJBc",
	"wKiLp",
	"wc8Zl",
	"wzURY",
	"x982a",
	"xS3K6",
	"xdwdwewewrwerfew",
	"xyO0Y",
	"yY5OU",
	"zKKBC",
	"zZIk8",
	"zmtgs",
	"Alas fair Death, 'twas missed in life - some peace and quiet from my wife",
	"Applaud, my friends, the comedy is finished.",
	"At last... a nice long sleep.",
	"Audi Partem Alteram",
	"Basil, assaulted by bears",
	"Burninated",
	"Confusion will be my epitaph",
	"Do not open until Christmas",
	"Don't be daft, they couldn't hit an elephant at this dist-",
	"Don't forget to stop and smell the roses",
	"Don't let this happen to you!",
	"Dulce et decorum est pro patria mori",
	"Et in Arcadia ego",
	"Fatty and skinny went to bed.  Fatty rolled over and skinny was dead.  Skinny Smith 1983-2000.",
	"Finally I am becoming stupider no more",
	"Follow me to hell",
	"...for famous men have the whole earth as their memorial",
	"Game over, man.  Game over.",
	"Go away!  I'm trying to take a nap in here!  Bloody adventurers...",
	"Gone fishin'",
	"Good night, sweet prince: And flights of angels sing thee to thy rest!",
	"Go Team Ant!",
	"He farmed his way here",
	"Here lies Lies. It's True",
	"Here lies the left foot of Jack, killed by a land mine.  Let us know if you find any more of him",
	"He waited too long",
	"I'd rather be sailing",
	"If a man's deeds do not outlive him, of what value is a mark in stone?",
	"I'm gonna make it!",
	"I took both pills!",
	"I will survive!",
	"Killed by a black dragon -- This grave is empty",
	"Let me out of here!",
	"Lookin' good, Medusa.",
	"Mrs. Smith, choked on an apple.  She left behind grieving husband, daughter, and granddaughter.",
	"Nobody believed her when she said her feet were killing her",
	"No!  I don't want to see my damn conduct!",
	"One corpse, sans head",
	"On the whole, I'd rather be in Minetown",
	"On vacation",
	"Oops.",
	"Out to Lunch",
	"SOLD",
	"Someone set us up the bomb!",
	"Take my stuff, I don't need it anymore",
	"Taking a year dead for tax reasons",
	"The reports of my demise are completely accurate",
	"(This space for sale)",
	"This was actually just a pit, but since there was a corpse, we filled it",
	"This way to the crypt",
	"Tu quoque, Brute?",
	"VACANCY",
	"Welcome!",
	"Wish you were here!",
	"Yea, it got me too",
	"You should see the other guy",
	"...and they made me engrave my own headstone too!",
	"<Expletive Deleted>",
	"Adapt. Enjoy. Survive.",
	"Adventure, hah! Excitement, hah!",
	"After all, what are friends for...",
	"After this, nothing will shock me",
	"Age and treachery will always overcome youth and skill",
	"Ageing is not so bad.  The real killer is when you stop.",
	"Ain't I a stinker?",
	"Algernon",
	"All else failed...",
	"All hail RNG",
	"All right, we'll call it a draw!",
	"All's well that end well",
	"Alone at last!",
	"Always attack a floating eye from behind!",
	"Am I having fun yet?",
	"And I can still crawl, I'm not dead yet!",
	"And all I wanted was a free lunch",
	"And all of the signs were right there on your face",
	"And don't give me that innocent look either!",
	"And everyone died.  Boo hoo hoo.",
	"And here I go again...",
	"And nobody cares until somebody famous dies...",
	"And so it ends?",
	"And so... it begins.",
	"And sometimes the bear eats you.",
	"And then 'e nailed me 'ead to the floor!",
	"And they said it couldn't be done!",
	"And what do I look like?  The living?",
	"And yes, it was ALL his fault!",
	"And you said it was pretty here...",
	"Another lost soul",
	"Any day above ground is a good day!",
	"Any more of this and I'll die of a stroke before I'm 30.",
	"Anybody seen my head?",
	"Anyone for deathmatch?",
	"Anything for a change.",
	"Anything that kills you makes you ... well, dead",
	"Anything worth doing is worth overdoing.",
	"Are unicorns supposedly peaceful if you're a virgin?  Hah!",
	"Are we all being disintegrated, or is it just me?",
	"At least I'm good at something",
	"Attempted suicide",
	"Auri sacra fames",
	"Auribus teneo lupum",
	"Be prepared",
	"Beauty survives",
	"Been Here. Now Gone. Had a Good Time.",
	"Been through Hell, eh? What did you bring me?",
	"Beg your pardon, didn't recognize you, I've changed a lot.",
	"Being dead builds character",
	"Beloved daughter, a treasure, buried here.",
	"Best friends come and go...  Mine just die.",
	"Better be dead than a fat slave",
	"Better luck next time",
	"Beware the ...",
	"Bloody Hell...",
	"Bloody barbarians!",
	"Buried the cat.  Took an hour.  Damn thing kept fighting.",
	"But I disarmed the trap!",
	"Can YOU fly?",
	"Can you believe that thing is STILL moving?",
	"Can you come up with some better ending for this?",
	"Can you feel anything when I do this?",
	"Can you give me mouth to mouth, you just took my breath away.",
	"Can't I just have a LITTLE peril?",
	"Can't eat, can't sleep, had to bury the husband here.",
	"Can't you hit me?!",
	"Chaos, panic and disorder.  My work here is done.",
	"Check enclosed.",
	"Check this out!  It's my brain!",
	"Chivalry is only reasonably dead",
	"Coffin for sale.  Lifetime guarantee.",
	"Come Monday, I'll be all right.",
	"Come and see the violence inherent in the system",
	"Come back here!  I'll bite your bloody knees off!",
	"Commodore Business Machines, Inc.   Died for our sins.",
	"Complain to one who can help you",
	"Confess my sins to god?  Which one?",
	"Confusion will be my epitaph",
	"Cooties?  Ain't no cooties on me!",
	"Could somebody get this noose off me?",
	"Could you check again?  My name MUST be there.",
	"Could you please take a breath mint?",
	"Couldn't I be sedated for this?",
	"Courage is looking at your setbacks with serenity",
	"Cover me, I'm going in!",
	"Crash course in brain surgery",
	"Cross my fingers for me.",
	"Curse god and die",
	"Dead Again?  Pardon me for not getting it right the first time!",
	"Dead and loving every moment!",
	"Dear wife of mine. Died of a broken heart, after I took it out of her.",
	"Don't tread on me!",
	"Dragon? What dragon?",
	"Drawn and quartered",
	"Either I'm dead or my watch has stopped.",
	"Eliza -- Was I really alive, or did I just think I was?",
	"Elvis",
	"Enter not into the path of the wicked",
	"Eris?  I don't need Eris",
	"Eternal Damnation, Come and stay a long while!",
	"Even The Dead pay taxes (and they aren't Grateful).",
	"Even a tomb stone will say good things when you're down!",
	"Ever notice that live is evil backwards?",
	"Every day is starting to look like Monday",
	"Every day, in every way, I am getting better and better.",
	"Every survival kit should include a sense of humor",
	"Evil I did dwell;  lewd did I live",
	"Ex post fucto",
	"Excellent day to have a rotten day.",
	"Excuse me for not standing up.",
	"Experience isn't everything. First, You've got to survive",
	"First shalt thou pull out the Holy Pin",
	"For a Breath, I Tarry...",
	"For recreational use only.",
	"For sale: One soul, slightly used. Asking for 3 wishes.",
	"For some moments in life, there are no words.",
	"Forget Disney World, I'm going to Hell!",
	"Forget about the dog, Beware of my wife.",
	"Funeral - Real fun.",
	"Gawd, it's depressing in here, isn't it?",
	"Genuine Exploding Gravestone.  (c)Acme Gravestones Inc.",
	"Get back here!  I'm not finished yet...",
	"Go ahead, I dare you to!",
	"Go ahead, it's either you or him.",
	"Goldilocks -- This casket is just right",
	"Gone But Not Forgotten",
	"Gone Underground For Good",
	"Gone away owin' more than he could pay.",
	"Gone, but not forgiven",
	"Got a life. Didn't know what to do with it.",
	"Grave?  But I was cremated!",
	"Greetings from Hell - Wish you were here.",
	"HELP! It's dark in here... Oh, my eyes are closed - sorry",
	"Ha! I NEVER pay income tax!",
	"Have you come to raise the dead?",
	"Having a good time can be deadly.",
	"Having a great time. Where am I exactly??",
	"He died of the flux.",
	"He died today... May we rest in peace!",
	"He got the upside, I got the downside.",
	"He lost his face when he was beheaded.",
	"He missed me first.",
	"He's not dead, he just smells that way.",
	"Help! I've fallen and I can't get up!",
	"Help, I can't wake up!",
	"Here lies Pinocchio",
	"Here lies the body of John Round. Lost at sea and never found.",
	"Here there be dragons",
	"Hey, I didn't write this stuff!",
	"Hodie mihi, cras tibi",
	"Hold my calls",
	"Home Sweet Hell",
	"I KNEW this would happen if I lived long enough.",
	"I TOLD you I was sick!",
	"I ain't broke but I am badly bent.",
	"I ain't old. I'm chronologically advantaged.",
	"I am NOT a vampire. I just like to bite..nibble, really!",
	"I am here. Wish you were fine.",
	"I am not dead yet, but watch for further reports.",
	"I believe them bones are me.",
	"I broke his brain.",
	"I can feel it.  My mind.  It's going.  I can feel it.",
	"I can't go to Hell. They're afraid I'm gonna take over!",
	"I can't go to hell, they don't want me.",
	"I didn't believe in reincarnation the last time, either.",
	"I didn't mean it when I said 'Bite me'",
	"I died laughing",
	"I disbelieved in reincarnation in my last life, too.",
	"I hacked myself to death",
	"I have all the time in the world",
	"I knew I'd find a use for this gravestone!",
	"I know my mind. And it's around here someplace.",
	"I lied!  I'll never be alright!",
	"I like it better in the dark.",
	"I like to be here when I can.",
	"I may rise but I refuse to shine.",
	"I never get any either.",
	"I said hit HIM with the fireball, not me!",
	"I told you I would never say goodbye.",
	"I used to be amusing. Now I'm just disgusting.",
	"I used up all my sick days, so now I'm calling in dead.",
	"I was killed by <illegible scrawl>",
	"I was somebody. Who, is no business of yours.",
	"I will not go quietly.",
	"I'd give you a piece of my mind... but I can't find it.",
	"I'd rather be breathing",
	"I'll be back!",
	"I'll be mellow when I'm dead. For now, let's PARTY!",
	"I'm doing this only for tax purposes.",
	"I'm not afraid of Death!  What's he gonna do? Kill me?",
	"I'm not getting enough money, so I'm not going to engrave anything useful here.",
	"I'm not saying anything.",
	"I'm weeth stupeed --->",
	"If you thought you had problems...",
	"Ignorance kills daily.",
	"Ignore me... I'm just here for my looks!",
	"Ilene Toofar -- Fell off a cliff",
	"Is that all?",
	"Is there life before Death?",
	"Is this a joke, or a grave matter?",
	"It happens sometimes. People just explode.",
	"It must be Thursday. I never could get the hang of Thursdays.",
	"It wasn't a fair fight",
	"It wasn't so easy.",
	"It's Loot, Pillage and THEN Burn...",
	"Just doing my job here",
	"Killed by diarrhea of mouth and constipation of brain.",
	"Let her RIP",
	"Let it be; I am dead.",
	"Let's play Hide the Corpse",
	"Life is NOT a dream",
	"Madge Ination -- It wasn't all in my head",
	"Meet me in Heaven",
	"Move on, there's nothing to see here.",
	"My heart is not in this",
	"No one ever died from it",
	"No, you want room 12A, next door.",
	"Nope.  No trap on that chest.  I swear.",
	"Not again!",
	"Not every soil can bear all things",
	"Now I have a life",
	"Now I lay thee down to sleep... wanna join me?",
	"Obesa Cantavit",
	"Oh! An untimely death.",
	"Oh, by the way, how was my funeral?",
	"Oh, honey..I missed you! She said, and fired again.",
	"Ok, so the light does go off. Now let me out of here.",
	"One stone brain",
	"Ooh! Somebody STOP me!",
	"Oops!",
	"Out for the night.  Leave a message.",
	"Ow!  Do that again!",
	"Pardon my dust.",
	"Part of me still works.",
	"Please, not in front of those orcs!",
	"Prepare to meet me in Heaven",
	"R2D2 -- Rest, Tin Piece",
	"Relax.  Nothing ever happens on the first level.",
	"Res omnia mea culpa est",
	"Rest In Pieces",
	"Rest, rest, perturbed spirit.",
	"Rip Torn",
	"She always said her feet were killing her but nobody believed her.",
	"She died of a chest cold.",
	"So let it be written, so let it be done!",
	"So then I says, How do I know you're the real angel of death?",
	"Some patients insist on dying.",
	"Some people have it dead easy, don't they?",
	"Some things are better left buried.",
	"Sure, trust me, I'm a lawyer...",
	"Thank God I wore my corset, because I think my sides have split.",
	"That is all",
	"The Gods DO have a sense of humor: I'm living proof!",
	"This dungeon is a pushover",
	"This elevator doesn't go to Heaven",
	"This gravestone is shareware. To register, please send me 10 zorkmids",
	"This gravestone provided by The Yendorian Grave Services Inc.",
	"This is not an important part of my life.",
	"This one's on me.",
	"This side up",
	"Tone it down a bit, I'm trying to get some rest here.",
	"Virtually Alive",
	"We Will Meet Again.",
	"Weep not, he is at rest",
	"Welcome to Dante's.  What level please?",
	"Well, at least they listened to my sermon...",
	"Went to be an angel.",
	"What are you doing over there?",
	"What are you smiling at?",
	"What can you say, Death's got appeal...!",
	"What pit?",
	"When the gods want to punish you, they answer your prayers.",
	"Where's my refund?",
	"Will let you know for sure in a day or two...",
	"Wizards are wimps",
	"Worms at work, do not disturb!",
	"Would you mind moving a bit?  I'm short of breath down here.",
	"Would you quit being evil over my shoulder?",
	"Yes Dear, just a few more minutes...",
	"You said it wasn't poisonous!",

	/* from http://www.alt.org/nethack/addmsgs/viewmsgs.php */
	"Balin, son of Fundin, Lord of Moria",
	"Memento mori",
	"Help! I'm trapped in a gravestone factory!",
	"This grave is reserved - for you!",
	"This gravestone does not indicate a bones file.",
	"This space for rent; go to http://www.alt.org/nethack/addmsgs/"
	"Here lies Sgeo, killed while reading a gravestone.",
	"Quidquid Latine dictum sit, altum videtur.",
	"Death is but a door. Time is but a window. I'll be back!",
	"The reports of my demise are completely accurate.",
	"Bulk deals on gravestones - Die today!",

	/* from UnNetHack */
	"Hack 1984-1985",
	"NetHack 1987-2003",

	/* from UnNetHackPlus */
	"SporkHack 2007-2010",
	"SLASH'EM 1997-2006",
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
	"Lost to the guy who is passway!",
	"Went through cave just now. Still spiritful.",
	"Tie with rope!",
	"Finally it will be return to the center!",
	"I will not suspend it.",
	"But the bag is full!",
	"We are defeated by Shangdao team!",
	"I will try best not to incommode. Everybody aftertime",
	"The wave is small, may is urf?",
	"Great waterfall! Can I drift?",
	"OH! OH! Draw it! Put it down it is excessively too!!",
	"Put it down what to do? Oh the badge... sorry for having forgotten it want to get a regularity badge.",
	"Give it to you after a while!",
	"There is some dimness inside. The skill is to experience the charm of pocket monster. Is it fit to me?",
	"I defeated her. I try my best, but still not strong and no confidence. If lost out try again nothing at all!",
	"I will not equest sister. It is good in this status, dear!",
	"I go to Xiaojin specially the bike isn't really solded, isn't it?",
	"It is safe for man too!",
	"It always lost in the game of monster. Always let it die because of it isn't closed with me",
	"Nobody is care of it/it's impossible/dislike to go out to play/it's good to play together",
	"Once, my husband has done nothing. Only fond of entertainment how can I do?",
	"I didn't call him, he had gone to game-shop.",
	"My husband is regarded as a monster cracy Zhenqian is like to his father!",
	"Oh!Loged on such at most!",
	"The trolley isn't in, it's impossible to carry the guest on my back to walk!",
	"Once I walked in darkness I saw the moving-tree.",
	"My sister felt funny and go to see it, I afraid that it was dangerous!",
	"I told my elder sister about the moving-tree, she said it was dangerous, don't let me go unless I defeated Miss Yu.",
	"Today many goods to be bought!",
	"I wondered whose tel no. to be longed no?",
	"I feel Xiaojin is in a big city while going to its store.",
	"Buy a gift for my friends go to the second floor:Market!",
	"Isn't it improved the gear while playing?",
	"The feeling is very good when the special ability is improved!",
	"A magical gift to send gift!",
	"Elder sister will come on sunday to look the state of monster, take the skill machine!",
	"The monster taken from others whose name isn't altered, because the name includes the feeling of a great man!",
	"Tired while shopping. Go to the top floor to have a rest.",
	"There is a thing I want very much, but the money isn't enough!",
	"Look the distant with telescope. Our house maybe seen.Is it the green roof?",
	"But the percentage of lose is low in fact, I like the card machine.",
	"Losed to the coin machine losed to the card machine too!",
	"It's regretted very much, don't find the object for exchange take the monster being kept!",
	"No prop but mail!",
	"It is a regret cannn't found the object for exchanging because of keeping for a long time!",
	"Before contacting with the center, wrote a report.",
	"This is a mystic egg be careful, please!",
	"Its content is your note about exploration,and character synopsis. The telephone hasn't sent.",
	"What a big monster's center! Very good, haveing so many new machines! As if it has been set up just!",
	"An unknown girl gave me a cat just now. Do you believe that it will be fine when exchange with?",
	"I was afraided when found my friend's name merge in the news!",
	"After the that we can connect with various of people certainly.",
	"The facility there can't be used now. It will feel good to go to the hearsaid place earlier!",
	"Becoming famous, after the name merged in the news. All right, how to make one's name merge in the news?",
	"It is excited to communicate! With this means first?",
	"When exchanging at the exchange department, and seeing the news about monsters.",
	"The main comes to manage center. Look out everybody, now the monster coachs around the countryuse the electric wave to communicate.",
	"The forest looks strange. It isn't good to walk now.",
	"There is sweet taste inside! The monster seems to go out.",
	"Shangdao themselves look down on us, how to stand it!",
	"Drive in the bikeway without hands the feeling is good!",
	"Ssee the relic, there is a sense suddenly!",
	"I think there is a great secret hidden in the relic!",
	"The shaking just frightens me!But we are care about the wall very much.",
	"Did the monster appear in the relic!? This is a great invent! We start to investigate it at once!",
	"How many sort monsters are there in the relic?",
	"The riddle's appearance has some causes certainly. Now we are invertigating this matter.",
	"My result is the riddle's appearance appeared after the communion center had been built.",
	"Clear up the rogatory content it is going to be published. If so,I'll be as famous as Wuzhimu Dr.",
	"I am living there if you like,you may come to reside. There are many empty places.",
	"Every week one monster always comes to waterside. Want to see the monster very much!",
	"I droped from the rope and brode my waist, I cann't move.",
	"Do well, the Missile Bombs escapes. My waist becomes well! We arw going to go back!",

};

/* Create a headstone at the given location.
 * The caller is responsible for newsym(x, y).
 */
void
make_grave(x, y, str)
int x, y;
const char *str;
{
	/* Can we put a grave here? */
	if ((levl[x][y].typ != ROOM && levl[x][y].typ != GRAVE) || t_at(x,y)) return;

	/* Make the grave */
	levl[x][y].typ = GRAVE;

	/* Engrave the headstone */
	if (!str) str = epitaphs[rn2(SIZE(epitaphs))];
	del_engr_at(x, y);
	make_engr_at(x, y, str, 0L, HEADSTONE);
	return;
}


#endif /* OVLB */

/*engrave.c*/
