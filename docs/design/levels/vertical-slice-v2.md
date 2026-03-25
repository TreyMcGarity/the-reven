# THE REVEN -- Vertical Slice v2: Inildon to Dark Forest
**Created: 2026-03-25**
**Status: DESIGN DOCUMENT -- Pending human approval**
**Scope: Levels 1-10 | Inildon -> River's Bend -> Dark Forest Dungeon**
**Estimated Playtime: 2-4 hours**

---

## 1. OVERVIEW

### What the Player Experiences

The player creates a character and awakens in the Divine throne room of Solthara, where the 11 Divines sit upon their thrones and explain the player's purpose: they are a Reven, sent to live in the mortal realm, find it worth protecting, and defend it. The vision fades. The player opens their eyes standing before 11 stone statues depicting those same thrones -- lifeless but perfectly carved -- in a shrine within the walls of Inildon, capital of the Inil.

From there, the player learns to move, fight, and interact. They pick up their first weapon, step outside Inildon's walls to fight training dummies and wildlife, and hear rumors of the Dred attack on Donrid in the north. The Queen's court directs them east toward River's Bend, the frontline settlement where the Hallowed Knights hold the line against Dred incursions. Along the road, the player encounters wildlife, scattered Dred Minions, and points of interest that reward exploration.

At River's Bend, combat ramps up. The Hallowed Knights need help. The Arcanem is referenced as a nearby source of magical knowledge, and the player discovers their first spells. Side quests introduce crafting materials and basic smithing. By level 10, the player is ready for the Dark Forest dungeon -- a 30-60 minute co-op-capable run through corrupted woodland, Nilvar cultists, giant spiders, and a final boss: a resurrected Nilvar soldier from the old world, animated by Em'Faira's lingering influence.

### Tone

**Skyrim exploration + Destiny 2 co-op.** The world feels vast and lived-in. NPCs have opinions and react to events. Combat is weighty and readable. Co-op feels natural, not forced. Simple to grasp on first contact; depth reveals itself to players who dig deeper. A player who only swings a sword and follows quest markers has a complete experience. A player who explores every cave, reads every lore note, and experiments with spell combinations has a richer one. Both are valid.

### Design Pillars for the Vertical Slice

1. **Teach without tutorials**: The player learns by doing, not by reading walls of text. Tooltip prompts are minimal and contextual.
2. **Reward exploration**: Every detour from the main path has something -- a chest, a lore stone, a hidden spell tome, a crafting node.
3. **Establish the world state**: By the time the player reaches River's Bend, they should understand that Donrid has fallen, the Dred are a real threat, the Nilvar are a separate and ancient evil, and the world needs them.
4. **Prove the combat loop**: Melee, ranged, and magic all feel good by level 10. The dungeon is the final exam.

---

## 2. INILDON (Starting City)

### The Divine Intro Sequence (Pre-Gameplay)

**Duration**: 2-3 minutes. Non-interactive cinematic.

1. Character creation screen. Player designs appearance and confirms.
2. Camera fades in on a vast, luminous hall in Solthara. Eleven thrones arranged in a semicircle. Each throne is distinct: Vaeya's throne radiates warm golden light; Novastra's glimmers with starlight; Kolvari's crackles with elemental energy; Platgree's is stone and earth; and so on for all eleven.
3. The Divines speak (one voice, blended, or Vaeya alone -- TBD in production):
   - *"You are a Reven. We have given you a body to walk the mortal realm."*
   - *"Live among them. Learn what they have built. Find it worth protecting."*
   - *"The darkness grows. You will know it when you see it."*
4. Light intensifies. The player's vision whites out.
5. Camera fades in on the player standing in a stone shrine. Eleven statues depict the thrones exactly as they appeared -- but these are cold stone. Sunlight filters through an arched window. The shrine is inside Inildon's walls.

**Implementation note**: This can be a pre-rendered cutscene or a real-time sequence using the same UE5 level. For a solo dev, a simple camera pan over static meshes with voice-over and text cards is sufficient. The statues become a reusable asset for the shrine interior.

### Tutorial Flow (Levels 1-2)

The tutorial is embedded in the first 15-20 minutes of gameplay, not a separate mode. The player is never locked in a tutorial room.

**Step 1 -- Movement and Camera** (Inside the Shrine)
- Player gains control. WASD movement, mouse camera.
- A prompt appears: "Look around." Player rotates the camera to see the shrine.
- A door is open ahead. Player walks through.

**Step 2 -- Interaction** (Shrine Exterior / Inildon Streets)
- Player steps into a sunlit courtyard. An Inilvan guard stands nearby.
- Prompt: "Press E to interact."
- Guard: *"Another Reven. The Queen's court is through the main gate. You should report there."*
- The player is now free to explore Inildon. There is no invisible wall forcing them to the Queen. The quest marker guides but does not compel.

**Step 3 -- First Weapon Pickup** (Inilvan Command Yard)
- On the way to the Queen's court, the player passes through the Inilvan Command yard (canonical location -- combat training area with a forge).
- A weapon rack holds a basic iron sword, a wooden shield, and a simple bow with arrows.
- Prompt: "Press E to pick up." Player equips their first weapon.
- [NEW NPC] **Drill Sergeant Renna** -- Inilvan combat instructor. Gruff, practical. Stands near training dummies.
  - Renna: *"Reven, yes? Pick up a weapon. Hit the dummy. Show me you know which end is sharp."*

**Step 4 -- Combat Basics** (Training Dummies / Outside Inildon Walls)
- Training dummies in the Inilvan Command yard teach basic attacks.
- Prompt sequence:
  - "Left-click to attack."
  - "Right-click to block."
  - "Shift to dodge." (Consumes stamina.)
  - "Hold right-click and attack after an enemy strikes to parry." (Timing window shown on dummy.)
- After the dummy sequence, Renna directs the player outside the city walls to fight actual creatures.
  - Renna: *"Dummies don't fight back. There are boars in the fields south of the gate. Go learn what a real fight feels like."*
- Outside the walls: 3-5 wild boars (level 1). Low damage, slow attacks. Perfect for learning dodge timing.
- Killing the boars grants XP. The player hits level 2.

### Queen's Court (Main Quest Start)

After the tutorial combat, the player heads to the Queen's court in the central palace.

**Canon NPC: Regina Vaena Heavendawn** -- First Queen of Inil. Sits on the throne. Regal, measured, carries the weight of leadership during wartime.

**Canon NPC: Morhri** -- Lead strategist, Advisory Council. Stands beside the throne. Analytical, terse.

**The Scene**:
- The Queen recognizes the player as Divine-sent. She has seen Revens before (other players).
  - Vaena: *"The Divines send their children in times of need. You arrive at a dark hour."*
- Morhri delivers the world state:
  - *"Donrid has fallen. The Dred took it three weeks ago. Dosram refugees are flooding south through the mountain passes. The western fortress is now theirs."*
  - *"Our forces hold at River's Bend, but Wen Lightbane's Hallowed Knights are stretched thin. The Dred test their lines every night."*
  - *"We need every sword, every bow, every hand willing to fight. River's Bend is east, along the Thway River."*
- The Queen grants the player their first quest: **"The Road to River's Bend"** -- travel east, report to Madam Wilo Lightbane, leader of River's Bend.
- Morhri provides a basic quest reward: a set of Inil leather armor (Head, Body, Gloves, Legs, Boots -- all white/common quality with minimal stats).

### Key NPCs in Inildon

| NPC | Canon? | Location | Role |
|---|---|---|---|
| Regina Vaena Heavendawn | Canon | Queen's Court | Gives main quest, world-state exposition |
| Morhri | Canon | Queen's Court | Strategic briefing, delivers armor reward |
| Quenla | Canon | Inilvan Command | Commander of the Inilvan. Brief appearance. Player sees her addressing troops before she departs for the front. Not a quest giver yet -- she becomes important later in the main story. |
| [NEW NPC] Drill Sergeant Renna | New | Inilvan Command Yard | Tutorial combat instructor |
| [NEW NPC] Smithmaster Toren | New | Inilvan Command Forge | Explains smithing exists. "Come back when you have materials." Does NOT teach smithing yet -- that happens at River's Bend. Sells basic weapons. |
| [NEW NPC] Shrine Keeper Yelin | New | Divine Shrine (starting area) | Provides lore about the Divines if asked. Optional dialogue. |
| [NEW NPC] Tavern Owner Sila | New | Inildon Tavern | Rumors about the world. Dynamic dialogue that changes as the game state progresses. First source of side quests (fetch quests for tavern supplies from outside the walls). |

### Inildon City Layout (Functional Districts)

Inildon is a coastal city with white marble architecture and celestial engravings on walls and archways. It should feel regal but lived-in -- not a museum. People walk streets, guards patrol, merchants call out wares.

**Districts** (from the main gate inward):

1. **Outer Market**: Just inside the main gate. Stalls, merchants, noise. Player can buy/sell basic items. Connects to the gate leading outside the walls.
2. **Inilvan Command**: Military quarter. Training yard with dummies, weapon racks, the forge (Smithmaster Toren). Quenla's command tent. Soldiers drilling. This is where the player picks up their first weapon.
3. **Temple Quarter**: Contains the Divine Shrine where the player spawns. Shrine Keeper Yelin. The Inilium (canonical magic location) is here but its doors are closed during the vertical slice -- a sign reads "Sealed by order of the Advisory Council." [SUGGESTION: This creates a mystery. The Inilium was sealed after the Inilvan split over mages. Opens in a later content phase.]
4. **Queen's Court**: Central palace. Vaena's throne room. Morhri's war table. The Advisory Council chambers.
5. **Residential / Tavern District**: Homes, the tavern (Sila), general stores. Quieter streets. A few NPCs with flavor dialogue about the Donrid attack, fear of the Dred, hope that the Revens will help.
6. **Harbor**: Western edge. Docks, fishing boats, views of the ocean. A few Wend traders with black sails are docked here -- they sell exotic goods at high prices. [SUGGESTION: One Wend trader sells a single spell tome at a premium price, giving players an optional early spell acquisition path.]

**Greybox Scale**: Inildon should feel big enough to be a capital but small enough to navigate in 5 minutes. Approximately 200m x 300m playable area. 15-20 buildings with interiors (key locations only -- the rest are facades). Streets are wide enough for the camera to work in third-person.

---

## 3. JOURNEY: INILDON TO RIVER'S BEND

### Route Description

The road from Inildon to River's Bend follows the Thway River eastward. The terrain transitions from coastal plains (flat, grassy, farms near Inildon) to rolling hills and light forest as the river bends inland. The final stretch before River's Bend follows the river as it turns sharply -- the rapids that give the city its name are audible before the city is visible.

**Total distance (game world)**: Approximately 1.5-2 km of traversable terrain.
**Gameplay time**: 30-60 minutes depending on exploration.
**Level range**: 2-5.

### Terrain Zones

**Zone 1: Inildon Outskirts (Levels 2-3)**
- Flat farmland south and east of the city walls. Stone fences, wheat fields, dirt roads.
- **Wildlife**: Boars (level 1-2), deer (non-hostile, flee when approached), hawks circling overhead (ambient).
- **Enemies**: None initially. After the player passes the first waypoint, 2-3 Dred Minions spawn on the road ahead -- the first hostile enemy encounter. Minions are small, insect-like, fast but weak. They attack in groups of 2-3 and die in 3-4 hits.
- **Point of Interest**: A destroyed farmstead off the main road. Collapsed barn, scorched earth. A [NEW NPC] **Farmer Hedda** hides behind a wall.
  - Side Quest: **"Hedda's Supplies"** -- Retrieve 3 supply crates from the burned barn (guarded by 4 Dred Minions). Reward: 50 XP, a health potion, and Hedda's gratitude (she relocates to River's Bend later).

**Zone 2: Thway River Road (Levels 3-4)**
- The road follows the river's north bank. Trees thicken. Occasional stone bridges over tributary streams.
- **Wildlife**: Wolves (level 3, hostile, hunt in packs of 2-3). River fish visible in the water (ambient / future fishing skill).
- **Enemies**: Dred Minions in groups of 3-4. One encounter with 2 Dred Soldiers (level 4) -- humanoid, armored, wield crude swords. They are noticeably tougher than Minions. This is the first time the player faces enemies that block.
- **Point of Interest**: **Old Guard Camp** -- A small encampment off the road, south of the path. 4-5 tents, a campfire, grizzled veterans in worn Inilvan armor with no celestial markings (they stripped them when they left). They are the Old Guard -- disbanded Inilvan veterans who refused to serve alongside mages.
  - [NEW NPC] **Captain Orvald** -- Old Guard leader at this camp. Bitter, proud, refuses to return to Inildon.
  - Dialogue only (no quest in vertical slice). Orvald explains why they left: *"We won every battle for a thousand years without a single spell. Then they lose one fight at Amanar and decide swords aren't enough. The Inilvan I served in is dead."*
  - [SUGGESTION: The Old Guard becomes a side questline in later phases -- convincing them to rejoin the fight or earning their respect through martial feats, not magic.]

**Zone 3: Approach to River's Bend (Levels 4-5)**
- Hills steepen. The river narrows and accelerates. Rapids visible below cliff edges. Mist rises from the water.
- **Enemies**: Dred Minion patrol (4-5 Minions + 1 Soldier). This is the first multi-type encounter. The Soldier hangs back while Minions rush.
- **Environmental storytelling**: Burned watchtower on a hilltop. Inil banners torn. Bodies of Dred and Inil soldiers. A journal on one of the Inil corpses reads: *"They come from the forest at night. Not the Dred -- something older. The scouts say there are figures in the trees watching us. They do not attack, but they do not leave."* -- This foreshadows the Nilvar and the Dark Forest.
- **Point of Interest**: **River Overlook** -- A natural plateau with a clear view southeast toward the Dark Forest. The tree line is visible in the distance: dark, unnaturally dense, with a faint greenish haze above the canopy. This is the first time the player sees the Dark Forest. A lore stone on the overlook reads: *"Beyond the southern wood lies Nildan, city of the hollow. None who enter return whole."*
- **Crafting Nodes**: Iron ore deposits (2-3 nodes) along the riverbank cliffs. The player can mine these with a pickaxe (provided free at River's Bend or found in a crate nearby). This introduces the gathering loop before the smithing tutorial.

### Enemy Types (Levels 1-5)

| Enemy | Level | Type | HP | Behavior | First Seen |
|---|---|---|---|---|---|
| Wild Boar | 1-2 | Wildlife | Low | Charges in straight line. Telegraphed. | Inildon outskirts |
| Wolf | 3 | Wildlife | Low | Circles, lunges. Pack AI -- one distracts, others flank. | Thway River Road |
| Dred Minion | 2-3 | Dred | Low | Swarms. Fast, weak. Insect-like. No armor. | Inildon outskirts |
| Dred Soldier | 4-5 | Dred | Medium | Humanoid. Blocks, attacks with crude sword. Slower, hits harder. | Thway River Road |

---

## 4. RIVER'S BEND (Second Hub)

### City Overview

River's Bend sits at a hard turn of the Thway River where rapids churn white. It is a frontier settlement -- not a capital. Where Inildon is white marble and elegance, River's Bend is timber walls, stone foundations, muddy streets, and the constant clang of the forge. This is the frontline. The Hallowed Knights were formed here, and they still hold it.

The city is smaller than Inildon (approximately 150m x 200m playable area) but denser. Every building has a function. There are no decorative plazas -- this is a military outpost that became a town.

**Visual Identity**: Dark timber, reinforced stone walls, watchtowers with archers, Hallowed Knight banners (white field with a golden sun, stained from weather and battle). The river is always audible. Mist from the rapids drifts through the eastern streets.

### Key NPCs

| NPC | Canon? | Location | Role |
|---|---|---|---|
| Madam Wilo Lightbane | Canon | Lightbane Hall (central building) | Leader of River's Bend. Main quest giver. |
| Wen Lightbane | Canon | Hallowed Knight Barracks | First Hallowed Knight leader. Gives combat-focused quests. |
| [NEW NPC] Arcanist Syla | New | River's Bend Chapel (repurposed as magic study) | Arcanem-trained mage stationed here. Introduces magic to the player. Sells/gives first spell tomes. |
| [NEW NPC] Smith Gaelen | New | River's Bend Forge | Teaches basic smithing. Smithing tutorial quest. |
| [NEW NPC] Scout Maren | New | Eastern Watchtower | Reports on Dark Forest activity. Gives dungeon breadcrumb quest. |
| [NEW NPC] Healer Iona | New | Infirmary | Sells health potions. Flavor dialogue about wounded Hallowed Knights. |
| Farmer Hedda | New (from journey) | Market area | If her quest was completed, she appears here as a merchant selling food items. |

### Main Quest Progression (Levels 5-8)

**Quest: "Report to the Lightbanes" (Level 5)**
- Player delivers the Queen's message to Madam Wilo Lightbane.
- Wilo: *"Another Reven. Good. We lose soldiers faster than Inildon sends them. My son Wen commands our knights -- speak with him if you want to fight. And speak with Arcanist Syla if you want to learn what Inildon's council is too afraid to teach."*
- Unlocks River's Bend as a hub. All vendors become available.
- Reward: 100 XP, access to River's Bend quests.

**Quest: "The Night Patrol" (Level 5-6)**
- Quest giver: Wen Lightbane.
- Wen: *"The Dred probe our walls every night. Small groups -- Minions mostly, sometimes a Soldier leading them. Join the patrol tonight. Learn how we fight here."*
- Gameplay: The player joins 2 NPC Hallowed Knights on a patrol route outside the eastern wall. 3 waves of Dred attack:
  - Wave 1: 4 Dred Minions.
  - Wave 2: 3 Minions + 2 Soldiers.
  - Wave 3: 5 Minions + 1 Soldier + 1 [NEW ENEMY] Dred Woldred (wolf-like, fast, flanks). The Woldred is the first "special" enemy type.
- Reward: 200 XP, an iron weapon of the player's choice (sword, axe, mace, spear, or bow), and Wen's respect.

**Quest: "Sparks of the Arcanem" (Level 6-7) -- Magic Discovery**
- Quest giver: Arcanist Syla.
- Syla: *"The Arcanem sent me here to study the corruption in the southern forest. But I can teach you what I know. Magic is not evil -- it is a tool, like a sword. The Well of Mana gave us this gift."*
- Syla gives the player ONE spell tome based on dialogue choice:
  - "I want to destroy my enemies." -> **Firebolt** (Fire, Level 1, damage).
  - "I want to protect myself and others." -> **Mend** (Restoration, Level 1, heal).
  - "I want to be versatile." -> **Frost Shard** (Water, Level 1, damage + slow).
- Tutorial: The player learns to equip spells in spell slots (keys 1-5), cast spells, and manage mana.
- Syla then sends the player to find a second spell tome hidden in the wilderness:
  - **"Syla's Lost Cache"**: A supply cache Syla hid when she arrived, containing a **Thorn Lash** tome (Flora, Level 1). Located at an Enul traveler's campsite east of the city, guarded by wolves and 2 Dred Minions.
- Reward: 150 XP, Thorn Lash spell tome, and Syla sells additional spell tomes going forward:
  - **Entangle** (Flora, Level 5) -- 200 gold.
  - **Earthen Shield** (Earth, Level 5) -- 200 gold.
  - **Soul Ward** (Soul, Level 5) -- 200 gold.

**Quest: "Iron and Fire" (Level 6-7) -- Smithing Introduction**
- Quest giver: Smith Gaelen.
- Gaelen: *"You want better gear? Bring me iron ore and I will teach you how to shape it."*
- Player gathers 5 Iron Ore from nodes along the river cliffs (if they have not already).
- Gaelen walks the player through the smithing station:
  - Select recipe (Iron Sword, Iron Shield, Iron Helm, etc.).
  - Consume materials (5 Iron Ore = 1 Iron Ingot; 2 Ingots = 1 weapon; 3 Ingots = 1 armor piece).
  - Craft. Item has random stat rolls within a tier range.
- The player crafts their first item.
- Reward: 100 XP, smithing unlocked at any forge, recipe list for Iron-tier gear.

**Quest: "Shadows in the Trees" (Level 7-8) -- Dark Forest Breadcrumb**
- Quest giver: Scout Maren.
- Maren: *"I have been watching the Dark Forest from the eastern tower for three months. The corruption is spreading. Last week I saw figures -- not Dred. Taller. Wearing armor that looked... Inil. But wrong. Blackened. They carried a body into the forest."*
- Maren asks the player to investigate the forest edge and recover a Hallowed Knight scout who went missing.
- Gameplay: The player travels southeast to the Dark Forest border. The environment shifts: trees become gnarled, light dims, ambient sounds change (birds stop, insects buzz, wind moans). 2 Nilvar Scouts (Level 7, humanoid, corrupted Inil armor, fight with daggers and poison) ambush the player near the forest edge.
- The missing scout is found dead at the entrance to the forest. A journal on the body:
  - *"They worship something in the deep woods. A spirit. Em'Faira. The trees themselves obey her. I followed them as far as a stone gate. Beyond it, the path goes down. I will not follow further alone."*
- The stone gate is visible from here: the Dark Forest dungeon entrance.
- Reward: 200 XP, the dungeon quest marker is placed: **"Into the Dark"**.

### River's Bend Side Content

**Side Quest: "Wilo's Defense" (Level 6)**
- Madam Wilo Lightbane needs the eastern wall reinforced. Gather 10 timber from the nearby forest (non-Dark Forest -- normal woodland to the north). Simple fetch quest.
- Reward: 100 XP, **Entangle** spell tome (if the player has not purchased it from Syla). This ensures every player can get Entangle before the dungeon.

**Exploration: Hallowed Knight Supply Cache**
- Hidden in a cave behind the waterfall at the rapids. Contains: **Frost Shard** spell tome (if not chosen from Syla), a Hallowed Knight shield (better stats than basic iron), 3 health potions.
- No quest marker. Rewarded purely through exploration.

**Crafting Nodes in the Area**:
- Iron Ore: 6-8 nodes along river cliffs and in shallow caves.
- Timber: Harvestable trees in the northern forest.
- Leather: Dropped by wolves and boars.
- Herbs: Growing near the riverbank. Used for health potions at an alchemy table (basic alchemy -- combine 3 herbs = 1 health potion).

### The Arcanem Connection

The Arcanem itself is not in the vertical slice area -- it is further east, past marshes (canonical: "there are marshes between the Arcanem and River's Bend"). However, Arcanist Syla serves as the player's connection to the Arcanem. She references it frequently:
- *"When this war allows it, you should visit the Arcanem. What I teach you here is a fraction of what they offer."*
- *"Zyrax himself -- the First Archmage -- built those halls. Temporal magic. He could see through time."*

This establishes the Arcanem as a future destination without requiring the player to go there in the vertical slice.

### Dark Forest Visibility

From River's Bend's eastern watchtower, the player can see the Dark Forest to the southeast. It is a visible landmark: a wall of unnaturally dark trees, taller than the surrounding forest, with a faint sickly green glow at the canopy. At night, faint lights flicker in the forest -- Nilvar torches or Em'Faira's corruption. This visual landmark draws the player toward the dungeon.

---

## 5. DARK FOREST DUNGEON (Level 10)

### Entrance: Social Area

The dungeon entrance is at the stone gate discovered during "Shadows in the Trees." Outside the gate is a clearing with:
- A campfire (rest point, restores health/stamina/mana to full).
- A weapon rack where players can swap loadouts.
- An NPC: [NEW NPC] **Ranger Cael** -- An Enul ranger who monitors the forest edge. Not a quest giver -- he provides lore and warnings.
  - Cael: *"The Nilvar were Inil once. Before Em'Faira broke them. That spirit has poisoned this forest for a thousand years. The deeper you go, the worse it gets. Stay together if you can."*
- **Multiplayer**: This clearing is a **social area**. Players in the same world instance can see each other here. A "Looking for Group" signpost allows players to flag themselves for co-op (2-4 players). When a group forms and enters the gate, they are placed in a **dungeon instance** -- a private copy of the dungeon for their group.
- Solo players can enter alone. The dungeon is tuned for solo play at level 10. Co-op groups of 2-4 face scaled-up enemy health and damage.

### Dungeon Theme

**Nilvar cult. Corrupted wildlife and insects. Em'Faira's lingering influence.**

The Dark Forest dungeon is not a cave or a fortress -- it is the forest itself, descending into a ravine where the corruption is thickest. The canopy blocks all sunlight. Bioluminescent fungus provides dim green-blue light. Roots twist into unnatural shapes. The air is thick with spores. Occasional Nilvar altars with dark offerings sit beside the path.

The dungeon tells a story without quest text: the Nilvar were Inil who went into the forest and were corrupted by Em'Faira's influence and the dark ore in the mountain. Their armor is Inil armor -- blackened and warped. Their altars have the same celestial engravings as Inildon's walls, but defaced. The deeper the player goes, the more the corruption shows: trees become flesh-like, roots pulse, the ground is soft and warm.

### Layout

The dungeon is linear with optional side rooms. Total run time: 30-60 minutes.

**Section 1: The Outer Wood (Small Encounters)**
- Duration: 10-15 minutes.
- Environment: Dense corrupted forest. Narrow paths between massive tree roots. Webbing across branches (foreshadowing spiders).
- Encounters:
  - Pull 1: 4 Corrupted Beetles (Level 8) -- Large insects, melee only, charge and bite. Low HP.
  - Pull 2: 3 Corrupted Beetles + 2 Nilvar Scouts (Level 8) -- Scouts throw poisoned daggers.
  - Pull 3: 5 Nilvar Scouts -- Ambush from the trees. They drop down from branches.
  - **Optional side room**: A webbed alcove contains a lootable chest (random gear, iron-tier or better) guarded by 2 giant spiderlings (Level 8, smaller versions of the mini-boss).
- Environmental storytelling: A ruined Inil campsite. Journal entry: *"We tracked the Nilvar to this ravine. The forest changed around us. Enira started hearing voices. By the second night, her eyes were black. We had to leave her."*

**Section 2: The Hollow Gorge (Medium Encounters)**
- Duration: 10-15 minutes.
- Environment: The ravine deepens. Cliff walls on both sides. The path descends. Nilvar structures appear: crude bridges, bone totems, dark altars with green fire. Spore clouds drift through the air (environmental hazard: standing in them drains 2% max HP per second -- dodgeable).
- Encounters:
  - Pull 4: 2 Nilvar Warriors (Level 9) -- Melee fighters with corrupted Inil swords and shields. They block and parry. First enemies that require the player to use their own parry/dodge skills.
  - Pull 5: 3 Nilvar Warriors + 1 Nilvar Shaman (Level 9) -- The Shaman stands back and casts Thorn Lash (same spell the player may have). Prioritize the Shaman or get overwhelmed by slows.
  - Pull 6: 6 Corrupted Beetles swarm from a nest in the wall. Fast, chaotic. Tests AoE capability (Flame Wave, Thorn Lash, or just kiting and cleaving).
  - **Optional side room**: A Nilvar altar. Interacting with it shows a vision: a flash of Em'Faira -- a tall female figure made of twisted vines and dark light, her face beautiful and terrible. The vision ends. The altar drops a crafting material: **Dark Ore Fragment** (1 of 3 needed for a special recipe at Smith Gaelen -- a Dark Iron weapon, better than standard iron-tier). [SUGGESTION: Dark Iron gear is the best obtainable gear in the vertical slice. Requires 3 Dark Ore Fragments from the dungeon, encouraging repeat runs.]

**Section 3: Mini-Boss -- The Broodmother (Giant Spider)**
- Duration: 5-8 minutes.
- Arena: A wide cavern beneath tree roots. The ceiling is thick with webbing. Egg sacs line the walls.
- **Mini-Boss: Corrupted Broodmother** (Level 10)
  - HP: Approximately 5x a normal enemy.
  - Attacks:
    - **Lunge Bite**: Charges forward, deals heavy damage. Dodgeable with clear telegraph (rears back, hisses, lunges after 1.5 seconds).
    - **Web Spit**: Ranged attack. Fires a web projectile that slows the player for 3 seconds if hit. Blockable.
    - **Summon Spiderlings**: At 50% HP, the Broodmother shrieks. 4 Spiderlings (Level 8) drop from the ceiling. They must be cleared or they harass the player during the rest of the fight.
  - Weak point: After Lunge Bite, the Broodmother is stunned for 2 seconds. This teaches the player to dodge and punish -- a core combat skill for later bosses.
  - Loot: Random iron-tier or better armor piece, 1 Dark Ore Fragment, health potions.

**Section 4: The Deep Root (Transition to Final Boss)**
- Duration: 5 minutes.
- Environment: After the Broodmother cavern, the path descends into the root system of an enormous tree. The roots are thick as hallways. The walls pulse with green-black veins. The air hums.
- No combat encounters. This is a breather zone.
- Environmental storytelling: Wall carvings in the roots -- Inil-style celestial engravings, but the stars are replaced with eyes. A plaque reads (in Old Inil, translated by the player's UI): *"In Em'Faira's embrace, we are remade. In darkness, we see true. The old kingdom was blind. We have opened our eyes."*
- The path opens into the final arena.

**Section 5: Final Boss -- The Risen Knight**
- Duration: 8-12 minutes.
- Arena: A circular chamber beneath the great tree's root system. A single shaft of corrupted green light falls from above. In the center stands a stone sarcophagus, cracked open. Nilvar banners hang from root pillars.
- **Final Boss: The Risen Knight** -- A resurrected Nilvar soldier from the old world, reanimated by Em'Faira's residual power. Wears ancient Inil plate armor, blackened and overgrown with vines. Wields a two-handed greatsword crackling with dark energy.
  - Level: 10.
  - HP: Approximately 10x a normal enemy.
  - Phases:
    - **Phase 1 (100%-50% HP): Martial Combat**
      - Attacks with deliberate, heavy greatsword swings. Each swing has a 1-2 second wind-up.
      - **Overhead Slam**: Telegraphed 2 seconds. Deals heavy damage in a frontal cone. Dodge sideways.
      - **Sweep**: Horizontal swing. 1.5 second telegraph. Dodge backward or block (drains significant stamina if blocked).
      - **Shield Rush** (if player is at range): Charges forward with the flat of the blade. Knockback. Punishes players who try to only use ranged attacks.
      - After every 3rd attack, the Risen Knight pauses for 2 seconds -- attack window.
    - **Phase 2 (50%-0% HP): Corrupted Fury**
      - The green light intensifies. Vines erupt from the Knight's armor.
      - All Phase 1 attacks are faster (telegraphs reduced by 0.5 seconds).
      - **New Attack -- Vine Eruption**: Roots burst from the ground in a line toward the player. 2-second delay, ground indicator. Deals moderate damage and roots the player for 1.5 seconds.
      - **New Attack -- Dark Shout**: AoE fear. All players in range are knocked back and slowed for 2 seconds. Occurs once at 50% and once at 25%.
      - At 25% HP, the Knight kneels and channels. 4 Nilvar Scouts spawn. If the channel completes (8 seconds), the Knight heals 20% HP. Interrupt by dealing enough damage or killing the Scouts quickly.
  - Death: The Knight collapses. The green light fades. The vines wither. A moment of silence. Then a whisper (Em'Faira's voice): *"This one returns to the earth. But I have many more."*
  - **Loot**:
    - 1 guaranteed rare (blue) quality gear piece -- random slot, higher stats than iron-tier.
    - 1 Dark Ore Fragment.
    - 500 XP.
    - First-time completion bonus: **"Into the Dark"** quest reward from Scout Maren back at River's Bend: 300 additional XP, a title ("Darkwalker"), and a cosmetic cloak (tattered Hallowed Knight cloak with Dark Forest brambles).

### Dungeon Loot Table

| Source | Possible Drops |
|---|---|
| Corrupted Beetles | Chitin Shard (crafting material), nothing |
| Nilvar Scouts | Poisoned Dagger (weapon), Leather Scraps, Dark Herbs (alchemy) |
| Nilvar Warriors | Corrupted Iron Sword/Shield (usable, poor stats), Iron Ore |
| Nilvar Shaman | Spell Tome: Thorn Lash (if player does not have it), Dark Herbs |
| Corrupted Broodmother | Random iron-tier+ armor, 1 Dark Ore Fragment, Spider Silk (crafting) |
| The Risen Knight | 1 guaranteed rare armor piece, 1 Dark Ore Fragment, 500 gold |
| Chests (2 in dungeon) | Random gear, potions, gold, crafting materials |
| Altar vision room | 1 Dark Ore Fragment |

### Repeatable Runs

The dungeon is designed for repeat runs. Reasons to return:
- Farm Dark Ore Fragments (3 needed for Dark Iron weapon at Smith Gaelen).
- Farm rare gear drops from the Risen Knight.
- Farm crafting materials (Chitin, Spider Silk, Dark Herbs).
- Practice boss mechanics before later, harder dungeons.
- Co-op with friends who have not completed it.

Loot resets per run. Boss drops are randomized. Dungeon has a soft cooldown of 10 minutes between runs (prevents instant re-entry).

---

## 6. PROGRESSION (Levels 1-10)

### XP Sources

| Source | XP Range | Notes |
|---|---|---|
| Kill: Wildlife (Boar, Wolf) | 10-20 | Scales with enemy level |
| Kill: Dred Minion | 15-25 | Common enemy |
| Kill: Dred Soldier | 30-50 | Tougher enemy |
| Kill: Nilvar Scout | 30-40 | Dungeon enemy |
| Kill: Nilvar Warrior | 40-60 | Dungeon enemy |
| Kill: Mini-boss | 100-150 | Dungeon mini-boss |
| Kill: Final boss | 300-500 | Dungeon final boss |
| Quest: Main quest step | 100-300 | Major story beats |
| Quest: Side quest | 50-150 | Optional content |
| Exploration: Discover location | 25-50 | First visit to named locations |
| Exploration: Lore stone read | 10-25 | Environmental lore objects |

### XP Curve

| Level | Total XP Required | XP to Next Level | Where Player Should Be |
|---|---|---|---|
| 1 | 0 | 100 | Divine Shrine, Inildon |
| 2 | 100 | 150 | Inildon outskirts (post-tutorial) |
| 3 | 250 | 200 | Road to River's Bend (Zone 1) |
| 4 | 450 | 300 | Road to River's Bend (Zone 2) |
| 5 | 750 | 400 | Arriving at River's Bend |
| 6 | 1,150 | 500 | River's Bend quests |
| 7 | 1,650 | 600 | River's Bend quests + exploration |
| 8 | 2,250 | 750 | Completing River's Bend content |
| 9 | 3,000 | 1,000 | Dark Forest dungeon |
| 10 | 4,000 | -- | Dark Forest boss defeated |

### Stat Allocation

The player has 5 stats from level 1. At each level-up, they receive **3 stat points** to allocate freely.

| Stat | Effect | Primary Armor Slot |
|---|---|---|
| Might | Increases melee and ranged weapon damage. | Head |
| Fortitude | Increases max HP, damage reduction, HP regen rate. | Body |
| Haste | Increases movement speed, stamina regen rate. | Boots |
| Agility | Increases attack speed, dodge distance, dodge i-frame duration. | Gloves |
| Sorcery | Increases spell damage, reduces spell cooldowns, increases mana regen. | Legs |

**Starting stats (Level 1)**: All stats begin at 5. Total: 25.
**Level 10 stats**: 25 base + (9 level-ups x 3 points) = 52 total stat points.

A level 10 player who specializes might look like:
- **Melee build**: Might 14, Fortitude 12, Haste 10, Agility 11, Sorcery 5.
- **Magic build**: Might 5, Fortitude 8, Haste 7, Agility 7, Sorcery 25.
- **Balanced build**: Might 10, Fortitude 10, Haste 10, Agility 10, Sorcery 12.

[SUGGESTION: A respec option should be available at a cost (gold or quest). Not in the vertical slice -- add in Phase 2.]

### Gear Available (Levels 1-10)

**Armor Tiers**:

| Tier | Name | Stat Range per Piece | Source | Visual |
|---|---|---|---|---|
| Common (White) | Leather | 1-3 per stat | Quest reward (Inildon), enemy drops | Plain leather, no decoration |
| Common (White) | Iron | 2-5 per stat | Crafted at forge, enemy drops (River's Bend+) | Basic metal plates, simple |
| Uncommon (Green) | Reinforced Iron | 4-7 per stat | Crafted with extra materials, rare drops | Metal with leather underlayer, slightly polished |
| Rare (Blue) | Dark Iron | 6-10 per stat | Dark Forest dungeon boss, crafted with Dark Ore | Black-tinted metal with faint green veins |

**Stat Rolls**: Each armor piece rolls 2-3 stats randomly from the 5 core stats. The values are random within the tier range. Example: An Iron Helm might roll Might +3, Fortitude +4. Another Iron Helm might roll Agility +2, Sorcery +5, Haste +2. This creates loot diversity and encourages farming.

**5 Armor Slots**: Head, Body, Gloves, Legs, Boots. Each piece has a primary stat affinity (Head = Might, Body = Fortitude, etc.) but random rolls can override this. Primary stat has a higher chance of rolling high.

### Weapon Types Available (Levels 1-10)

**3 Weapon Slots**: Main Hand, Off Hand, Back.

| Weapon | Slot | Speed | Damage | Special |
|---|---|---|---|---|
| Sword (1H) | Main | Medium | Medium | Balanced. Can combo 3 hits. |
| Axe (1H) | Main | Slow | High | Cleave. Hits multiple enemies in arc. |
| Mace (1H) | Main | Slow | High | Stagger. Higher chance to break enemy block. |
| Dagger (1H) | Main | Fast | Low | Quick combos. Bonus damage from behind. |
| Shield | Off | -- | -- | Block. Absorbs damage based on Fortitude. Parry window. |
| Torch | Off | -- | -- | Light source in dark areas. Minimal fire damage. |
| Bow | Main (2H) | Slow | Medium-High | Ranged. Requires arrows. Charge shot mechanic. |
| Greatsword | Back (2H) | Very Slow | Very High | Sweeping attacks. Highest single-hit damage. |
| Spear | Back (2H) | Medium | Medium | Long reach. Thrust attacks. Good vs. single target. |

**Weapon tiers follow the same Common/Uncommon/Rare pattern as armor**, with damage scaling instead of stat bonuses. Weapons also roll 1-2 random stat bonuses.

### Spells Available by Level 10

From the spell remediation document, the following spells are available by level 10 and placed in the vertical slice:

| Spell | School | Level Req | How Acquired |
|---|---|---|---|
| Firebolt | Fire | 1 | Syla (choice) |
| Mend | Restoration | 1 | Syla (choice) |
| Frost Shard | Water | 1 | Syla (choice) or Hallowed Knight cache |
| Thorn Lash | Flora | 1 | Syla's Lost Cache quest or dungeon Shaman drop |
| Entangle | Flora | 5 | Purchased from Syla (200g) or Wilo's Defense quest reward |

**Maximum spells equipped**: 5 (keys 1-2-3-4-5). By level 10, a thorough explorer will have found all 5 of the above and can fill every slot.

Additional spells exist at or below level 10 (Stone Shot, Beast Sense, Earthen Shield, Soul Ward, Slow Field, Flame Wave, Mist Step, Feral Strike, Purify, Life Drain, Blood Price, Summon Shade, Spirit Bolt) but are intentionally NOT placed in the vertical slice area. They are reserved for the Arcanem, Enul lands, and later zones.

### Smithing/Crafting (Basic Only)

**Available in vertical slice**:
- **Smithing**: Iron-tier and Dark Iron-tier weapons and armor. Forge stations at Inildon (Smithmaster Toren) and River's Bend (Smith Gaelen). Requires ore + fuel (wood/charcoal).
- **Basic Alchemy**: Combine 3 herbs = 1 health potion. Alchemy table at River's Bend infirmary (Healer Iona).

**NOT available in vertical slice**:
- **Enchanting**: Locked until the Arcanem questline (Phase 2). Enchanting tables exist in the world but are non-interactive. Tooltip: "Requires Arcanem training."
- **Advanced Alchemy**: Mana potions, stamina potions, buff potions -- all deferred.
- **Advanced Smithing**: Steel-tier and above -- deferred to later material zones.

---

## 7. COMBAT (Levels 1-10)

### Combat Introduction Curve

| Level | What the Player Has | What They Face |
|---|---|---|
| 1 | Melee weapon only (sword, axe, or mace). Block, dodge, attack. | Training dummies, boars. |
| 2-3 | Melee + first ranged option (bow from Inilvan Command). Stamina management matters. | Wolves (flanking), Dred Minions (swarming). |
| 4-5 | Melee or ranged + shield option. Parry becomes relevant. | Dred Soldiers (blocking enemies). |
| 6-7 | First spells (1-2 equipped). Magic supplements melee/ranged. | Dred Woldreds (fast flankers), night patrol waves. |
| 8-9 | 3-5 spells available. Player choosing a combat identity. Crafted gear matters. | Nilvar Scouts (poison, ambush), Nilvar Warriors (parry-capable). |
| 10 | Full combat toolkit. All 3 combat styles accessible. | Dungeon enemies, mini-boss, final boss. |

### Core Melee Mechanics

**Light Attack** (Left Click): Quick swing. Low damage. Can chain into a 3-hit combo (tap-tap-tap). Third hit deals bonus damage.

**Heavy Attack** (Hold Left Click): Charged swing. High damage. Slow. Breaks enemy guard if they are blocking. Consumes 20 stamina.

**Block** (Hold Right Click): Raises shield or weapon guard. Absorbs damage at the cost of stamina. Each blocked hit drains stamina proportional to the attack's power. If stamina reaches 0 while blocking, the block breaks and the player is staggered for 1 second.

**Parry** (Block + Attack at the moment of incoming hit): Timing window of 0.3 seconds. Successful parry deflects all damage, costs no stamina, and staggers the attacker for 1.5 seconds -- opening them to a riposte (free heavy attack equivalent).

**Dodge** (Shift): Quick roll in movement direction. 0.3 second invincibility window at the start of the roll. Costs 25 stamina. Can dodge in any direction. 0.5 second recovery before the player can act again.

### Stamina Management

- **Base stamina**: 100 at level 1. Increases with Haste stat.
- **Stamina costs**: Dodge (25), Heavy Attack (20), Sprint (drains 5/sec), Block (variable per hit absorbed).
- **Stamina regen**: 10/sec base. Increases with Haste. Regen pauses for 1 second after any stamina-consuming action.
- **Stamina Break**: If stamina hits 0 from an enemy attack breaking guard, the player enters an exhaustion state for 2 seconds: cannot dodge, attack, or block. Stamina regens at half rate during exhaustion. This teaches players not to over-commit -- blocking everything is not viable. Dodge when you can, block when you must, parry when you are skilled enough.

### Ranged Combat

- **Bow**: Hold right click to draw. Release to fire. Longer draw = more damage (3 tiers: quick, half, full). Full draw takes 1.5 seconds. Arrows have travel time and arc.
- **Ammo**: Arrows are crafted (5 timber = 20 arrows) or purchased. Carried in stacks. Not unlimited -- ammo management is intentional.
- **Crossbow**: [NOT in vertical slice. Introduced at Crossroads/Ashpeak.] Noted for future reference.

### Magic Combat

- **Casting**: Press spell slot key (1-5). Some spells are instant-cast (Firebolt, Frost Shard); others have a 0.5-1 second cast time (Entangle, Mend).
- **Mana**: Base 100. Increases with Sorcery. Spells cost mana (see spell list). Mana regens at 5/sec base, increased by Sorcery.
- **Cooldowns**: Each spell has an individual cooldown. Vertical slice spells: Firebolt (3 sec), Mend (8 sec), Frost Shard (3 sec), Thorn Lash (4 sec), Entangle (10 sec). Sorcery stat reduces cooldowns by a percentage.
- **No swapping in combat**: Spell loadout is locked once combat begins.

### Enemy Difficulty Curve

| Enemy | Level | Threat | Key Mechanic Taught |
|---|---|---|---|
| Boar | 1 | Trivial | Basic attack timing |
| Wolf | 3 | Low | Flanking awareness. Multiple enemies. |
| Dred Minion | 2-3 | Low | Swarming. Prioritize targets. |
| Dred Soldier | 4-5 | Medium | Enemy blocking. Need heavy attacks or parry. |
| Dred Woldred | 5-6 | Medium | Fast flanker. Tests dodge reflexes. |
| Nilvar Scout | 7-8 | Medium | Ranged enemy. Poison DOT. Prioritize or close distance. |
| Nilvar Warrior | 8-9 | High | Enemy parries. Requires varied attack patterns. |
| Nilvar Shaman | 9 | High | Enemy caster. Must interrupt or kill quickly. |
| Corrupted Broodmother | 10 | Boss | Dodge windows, add management, punish windows. |
| The Risen Knight | 10 | Boss | Phase transitions, multiple attack types, interrupt mechanic. |

### Dred Types in Vertical Slice

| Type | Description | Canon Reference |
|---|---|---|
| Minion | Small, insect-like. 4-6 legs. Fast, weak. Attacks in groups. No armor. | Canon: "Insect-like" Dred type |
| Soldier | Humanoid. Crude iron armor. Sword and shield. Blocks. | Canon: "Soldiers" type |
| Woldred | Wolf-like. Four-legged. Armored plates on back. Fast, flanking behavior. | Canon: "Smaller agile dreds, much like wolves or armored hellhounds" |

Higher Dred types (Tankers, Dregs, Draiths, Dralls, Draids, Draiders) are reserved for later content zones (levels 10+).

---

## 8. MULTIPLAYER

### Open World

- Players in the same server instance can see each other in the open world (Inildon, the road, River's Bend).
- No PvP in the open world. Players cannot damage each other.
- Players can wave, emote, and trade items.
- No party system is required for overworld coexistence -- players simply exist in the same space.
- NPC quest states are per-player. Two players can talk to the same NPC and be on different quest steps.

### Dungeon Co-op (2-4 Players)

- The Dark Forest dungeon is the player's **first co-op experience**.
- At the dungeon entrance social area, players can form a group via the signpost UI or by inviting nearby players.
- Group size: 2-4 players. No role requirements (no "need a healer" gatekeeping). Any combination of builds works.
- When the group enters the dungeon, a private instance is created.
- Enemy scaling in co-op:
  - 2 players: Enemy HP x1.5, enemy damage x1.2.
  - 3 players: Enemy HP x2.0, enemy damage x1.3.
  - 4 players: Enemy HP x2.5, enemy damage x1.4.
- Loot is per-player. Each player sees their own drops. No loot competition.
- If a player dies in co-op, they respawn at the last checkpoint (campfire at entrance, or mid-dungeon checkpoint after the Broodmother). Respawn timer: 10 seconds. If all players die, the group wipes and respawns at the entrance.
- Voice chat and text chat are enabled in groups.

### PvP

- **Not in the vertical slice.** PvP is a separate system with its own maps and queue. It does not exist in the levels 1-10 experience.
- The groundwork for PvP (damage dealt to players, respawn logic) can be built during the combat system phase (M2) but is not testable in the vertical slice.

### Grouping Mechanics

- **Party invite**: Target a player, press a key to invite. They accept or decline.
- **Party UI**: Shows party member names, HP bars, and location indicators.
- **Party chat**: Separate chat channel for the party.
- **Scaling**: Party members share XP from kills (split evenly). Quest XP is not split -- each player gets full quest XP.
- **Level disparity**: If a party member is more than 3 levels above an enemy, that enemy gives 0 XP to that player (prevents power-leveling).

---

## 9. TECHNICAL REQUIREMENTS

### Level Streaming

- **Inildon**: Loaded as a separate streaming level. When the player approaches the city gate, the interior level streams in. When the player leaves and is 200m+ away, it streams out. This prevents Inildon's building interiors, NPCs, and props from consuming memory while the player is in the overworld.
- **Overworld**: One persistent level for the road from Inildon to River's Bend. Terrain, foliage, and enemy spawn points are all in this level. Use UE5 World Partition to break the overworld into grid cells that load/unload based on player proximity.
- **River's Bend**: Separate streaming level, same approach as Inildon.

### Interior Instancing

- Key buildings in Inildon and River's Bend (Queen's Court, tavern, forge, Lightbane Hall) are interior sub-levels that stream in when the player enters and stream out when they exit.
- Non-key buildings are facades only -- no interiors. This saves significant production time.
- Interior doors use a trigger volume: player walks through, brief fade-to-black (0.5 sec), interior loads, player appears inside. This hides the streaming seam.

### Dungeon Instancing

- The Dark Forest dungeon is a **separate level** loaded when the player (or group) enters.
- Multiplayer: Each group gets their own instance. The server spawns a new copy of the dungeon level for each group.
- For solo dev / early builds: Dungeon can be a simple level transition (load screen). True seamless instancing is a Phase 6 (networking) task.
- Dungeon state resets on each entry. No persistent dungeon state between runs.

### UE5 Systems

| System | Usage |
|---|---|
| **Enhanced Input** | All player input. Action mappings for movement, combat, spells (1-5), items (6-0), interact (E), dodge (Shift), block (RMB), attack (LMB). Context-sensitive input (can't cast while in menu). |
| **Niagara** | All VFX: spell effects, campfire particles, river mist, spore clouds, corruption glow, boss phase transition effects. |
| **Behavior Trees** | All enemy AI. States: Idle, Patrol, Detect (player in range), Chase, Attack, Flee (low HP Minions only), Die. Boss AI uses custom BT with phase-based logic. NPC ambient AI: Idle, Patrol path, Interact (when player engages). |
| **UMG Widgets** | All UI: HUD (HP/Stamina/Mana bars, spell slots, item slots), inventory screen, stat allocation screen, dialogue UI, quest tracker, minimap, loot popup, crafting UI, party UI. |
| **Gameplay Ability System (GAS)** | [SUGGESTION: Use GAS for spells and abilities if comfortable with C++. If Blueprint-first, a simpler custom ability system is fine for the vertical slice. GAS pays off long-term but has a steep setup cost.] |
| **UE5 World Partition** | Overworld terrain streaming. Divide the Inildon-to-River's-Bend overworld into grid cells. |
| **Data Tables** | Store all item stats, enemy stats, XP tables, loot tables, spell data. Data-driven design keeps balance tuning out of Blueprints. |

### Blueprint vs C++ Guidance

The project is Blueprint-first, C++ later. For the vertical slice:

**Blueprint**:
- Character movement and camera.
- Combat inputs (attack, block, dodge, parry detection).
- NPC dialogue system (simple text UI with branching).
- Inventory and equipment UI.
- Quest tracking (simple flag-based system).
- Crafting UI.
- Enemy AI behavior trees.
- All VFX triggers.

**C++ (defer until needed)**:
- Damage formula calculation (when PvP balance requires precision).
- Networking / replication (Phase 6).
- Performance-critical systems (large enemy counts, ability system optimization).
- Save/load serialization.

**Rule of thumb**: If it touches UI or game logic, do it in Blueprint first. If it needs to run 60+ times per frame or handle network replication, plan for C++ later.

---

## 10. ASSET REQUIREMENTS

### Environment Art

**Inildon**:
- Visual: White marble walls and columns with celestial engravings. Paved stone streets. Blue and gold banners. Clean, regal, Mediterranean-bright.
- Key structures: City walls with gate, Queen's Court (large building with throne room), Divine Shrine (medium building with 11 statues), Inilvan Command (open yard with forge building), tavern, market stalls (6-8), residential buildings (8-10 facades, 2-3 with interiors), harbor with docks.
- Props: Weapon racks, training dummies, market goods (crates, barrels, cloth), lanterns, benches, fountains.

**Overworld (Inildon to River's Bend)**:
- Visual: Coastal plains transitioning to rolling hills and light forest. The Thway River runs alongside the path. Stone bridges over streams. Farmland near Inildon, wild terrain near River's Bend.
- Key structures: Destroyed farmstead (1), Old Guard camp (4-5 tents, campfire), burned watchtower (1), river overlook (natural plateau with lore stone), small caves (2-3, containing ore nodes or chests).
- Props: Fences, haystacks, road markers, rock formations, ore nodes (interactable), fallen trees, animal skeletons.

**River's Bend**:
- Visual: Dark timber and reinforced stone. Military frontier post. Watchtowers. Muddy streets. Hallowed Knight banners (white + golden sun). River rapids visible and audible from the eastern edge.
- Key structures: Lightbane Hall (central, large timber building), Hallowed Knight barracks, forge, chapel (repurposed for Arcanist Syla), infirmary, eastern watchtower, market area (smaller than Inildon), city walls with gates.
- Props: Weapon racks, armor stands, medical supplies, campfires, patrol route markers, sandbags, wooden barricades.

**Dark Forest Dungeon**:
- Visual: Dense corrupted forest. Gnarled trees with black bark. Bioluminescent green-blue fungus. Webbing across branches. Spore clouds (particle effect). Nilvar altars (stone with green fire). Deep root system with pulsing veins.
- Key structures: Stone gate entrance, Broodmother cavern (wide, web-covered ceiling), root tunnel (transition), Risen Knight arena (circular, single shaft of green light).
- Props: Bone totems, Nilvar offerings (skulls, corrupted ore), egg sacs (Broodmother arena), sarcophagus (boss arena), webbed corpses, ruined Inil equipment.

### Character Models

| Category | Model | Count | Notes |
|---|---|---|---|
| Player | Player character (male/female) | 1 rigged model with modular armor | All armor slots must be visually swappable |
| NPC - Inil | Inil civilian (male/female) | 2 base models | Clothing variants for merchants, villagers |
| NPC - Inil | Inilvan soldier | 1 model | Steel armor with celestial engravings |
| NPC - Inil | Queen Vaena | 1 unique model | Royal armor/dress, crown, sits on throne |
| NPC - Inil | Hallowed Knight | 1 model | Distinct from standard Inilvan. White/gold heavy armor |
| NPC - Named | Morhri, Renna, Toren, Yelin, Sila, Gaelen, Syla, Maren, Iona, Orvald, Hedda, Cael | Variants of base civilian/soldier models | Unique accessories or color to distinguish |
| Enemy - Dred | Minion | 1 model | Insect-like, 4-6 legs, small, no armor |
| Enemy - Dred | Soldier | 1 model | Humanoid, crude iron armor, sword+shield |
| Enemy - Dred | Woldred | 1 model | Wolf-like, four legs, armored plates |
| Enemy - Nilvar | Scout | 1 model | Corrupted Inil armor (blackened). Daggers. |
| Enemy - Nilvar | Warrior | 1 model | Corrupted Inil plate. Sword+shield. |
| Enemy - Nilvar | Shaman | 1 model | Robed, staff with green glow |
| Enemy - Wildlife | Boar | 1 model | Standard boar |
| Enemy - Wildlife | Wolf | 1 model | Standard wolf |
| Enemy - Dungeon | Corrupted Beetle | 1 model | Large insect, mandibles, chitin plates |
| Enemy - Dungeon | Spiderling | 1 model | Smaller spider, swarm enemy |
| Boss | Corrupted Broodmother | 1 model | Giant spider, web spit, egg sac |
| Boss | The Risen Knight | 1 model | Ancient Inil plate (blackened), vine overgrowth, greatsword |
| **Total unique models** | | **~20** | Some share rigs (humanoids share skeleton) |

### Animation List

**Player Character**:
- Idle, Walk, Run, Sprint
- Dodge Roll (4 directions)
- Sword: Light Attack x3 (combo), Heavy Attack, Block, Parry
- Axe: Light Attack x2, Heavy Attack, Block
- Mace: Light Attack x2, Heavy Attack, Block
- Dagger: Light Attack x4 (fast combo), Heavy Attack
- Greatsword: Light Attack x2, Heavy Attack (overhead slam)
- Spear: Light Attack x3 (thrust combo), Heavy Attack (sweep)
- Bow: Draw, Hold, Release, Idle with bow
- Shield: Block stance, Parry react
- Spell Cast: Generic cast (arm forward), Channel (sustained), Self-buff (arms inward)
- Hit React (front, back), Death, Revive
- Interact (press button, pick up item)
- **Total: ~35-40 animations**

**NPC**:
- Idle, Walk, Talk (gesture variants x2-3)
- Sit (throne, chair, bench)
- Forge hammer (Smith NPCs)
- Drill/instruct (Renna)
- **Total: ~10 animations**

**Enemies (Humanoid - Dred Soldier, Nilvar Scout/Warrior/Shaman, Risen Knight)**:
- Idle, Walk, Run, Chase
- Attack x2-3 variants
- Block (Soldier, Warrior, Risen Knight)
- Hit React, Death
- Patrol (walk with head scanning)
- Risen Knight: Phase transition (vine eruption), Vine Eruption attack, Dark Shout, Channel heal, Death (collapse and wither)
- **Total: ~20 animations (shared skeleton)**

**Enemies (Creature - Minion, Woldred, Boar, Wolf, Beetle, Spiderling, Broodmother)**:
- Idle, Walk/Scuttle, Run/Charge
- Attack x1-2
- Hit React, Death
- Broodmother: Lunge Bite, Web Spit, Shriek (summon), Death
- **Total: ~15-20 animations across creature types**

### VFX List

| Category | Effect | Spell/Source |
|---|---|---|
| Combat | Sword slash trail | Player melee |
| Combat | Shield block impact (sparks) | Player block |
| Combat | Parry flash (bright white) | Successful parry |
| Combat | Dodge dust puff | Player dodge |
| Combat | Hit blood splash (red for player/NPC, black for Dred) | All damage |
| Combat | Stamina break stagger lines | Exhaustion state |
| Spell | Fire projectile + burn DOT flames | Firebolt |
| Spell | Green heal particles rising | Mend |
| Spell | Ice shard projectile + frost on target | Frost Shard |
| Spell | Vine whip forward | Thorn Lash |
| Spell | Root eruption from ground | Entangle |
| Environment | River mist (Niagara) | River's Bend |
| Environment | Campfire flames + smoke | Rest points |
| Environment | Dungeon spore cloud (green particles) | Dark Forest |
| Environment | Corruption glow (green-black pulsing) | Dark Forest |
| Environment | Bioluminescent fungus glow | Dark Forest |
| Boss | Web projectile | Broodmother |
| Boss | Spiderling spawn burst | Broodmother 50% HP |
| Boss | Vine eruption line | Risen Knight Phase 2 |
| Boss | Dark shout shockwave | Risen Knight Phase 2 |
| Boss | Green light pillar (channel) | Risen Knight 25% HP |
| Boss | Death wither (vines dry, light fades) | Risen Knight death |
| UI | Level-up flash | Player level-up |
| UI | Loot glow (white/green/blue by rarity) | Item drops |
| **Total unique VFX** | **~24** | Shared Niagara archetypes reduce this to ~10 base systems with variants |

### Audio Needs

Audio can be placeholder for the vertical slice. Priority list:

**Must Have (Placeholder OK)**:
- Sword swing (whoosh), impact (clang on metal, thud on flesh)
- Shield block (metal clang)
- Dodge roll (cloth rustle)
- Footsteps (stone, dirt, grass, wood)
- UI sounds (menu open/close, item pickup, level-up, quest complete)
- Ambient: wind, birds, river, forest insects, dungeon dripping
- Enemy: Dred hiss/screech (Minions), grunt (Soldiers), wolf howl (Woldreds), spider hiss
- Boss: Broodmother screech, Risen Knight sword scrape on stone, dark shout echo

**Nice to Have**:
- Spell cast sounds per element (fire whoosh, ice crack, vine grow, heal chime)
- NPC voice barks (greetings, combat callouts) -- can be text-only in vertical slice
- Music: ambient exploration tracks (calm for Inildon, tense for overworld, dark for dungeon), boss combat music

**Deferred**:
- Full voice acting
- Dynamic music system (layers based on combat state)
- Environmental interaction sounds (crafting, mining, etc.)

---

## 11. BUILD MILESTONES

Each milestone is scoped for 1-2 weeks of solo development. The order is intentional: each milestone builds on the previous one. Milestones are independent deliverables -- each one produces a testable result.

### M1: Third-Person Character Controller + Camera (Week 1-2)

**Deliverable**: A player character that moves, jumps, and rotates in a greybox level with a follow camera.

- UE5 project setup. Third-person character Blueprint.
- Enhanced Input: WASD movement, mouse camera, sprint (Shift hold).
- Camera: Over-the-shoulder, slight offset. Collision handling so it does not clip through walls. Zoom adjustment (optional).
- Greybox: A flat plane with some walls and ramps to test movement.
- **Test**: Player can run around a greybox space smoothly. Camera feels good.

### M2: Basic Melee Combat (Week 3-4)

**Deliverable**: The player can attack, block, dodge, and parry. Stamina bar works.

- Light attack (LMB): 3-hit combo with animation montages.
- Heavy attack (Hold LMB): Charged swing, stamina cost.
- Block (RMB Hold): Damage absorption, stamina drain per hit.
- Parry (RMB + LMB on incoming): Timing window detection, stagger response.
- Dodge (Shift): Roll with i-frames.
- Stamina system: Bar, drain, regen, exhaustion state.
- HUD: HP bar, Stamina bar (UMG widget).
- Greybox: Training dummy that attacks on a timer for parry practice.
- **Test**: Combat feels responsive. Parry timing is learnable. Stamina forces decisions.

### M3: Enemy AI (Week 5-6)

**Deliverable**: Enemies patrol, detect the player, chase, attack, and die.

- Behavior Tree: Patrol (waypoints), Detect (perception component, sight/sound), Chase (NavMesh pathfinding), Attack (melee range, swing), Die (ragdoll or death anim).
- Enemy Blueprint base class with HP, damage, and death logic.
- 2 enemy types: Melee rusher (Minion behavior) and Melee blocker (Soldier behavior).
- Aggro system: Enemies aggro when player is in range. De-aggro if player runs far enough.
- Hit reactions: Enemies flinch when hit. Stagger on heavy attacks.
- Spawn system: Enemies placed in the level, respawn after X seconds.
- **Test**: Can fight 3+ enemies simultaneously. Enemies feel distinct. Death feels satisfying.

### M4: NPC Dialogue System (Week 7-8)

**Deliverable**: The player can talk to NPCs. Text appears on screen with choices.

- Interact system: Press E near NPC to start dialogue.
- Dialogue UI (UMG): NPC name, portrait (placeholder), dialogue text, response options (1-4 choices).
- Dialogue data: Data Table or struct array. Each node has text, responses, and next-node links.
- Basic quest flag integration: Dialogue can check flags ("HasCompletedQuest_X") and branch.
- Camera: Slight zoom on NPC during dialogue. Player input locked except dialogue choices.
- **Test**: Can have a multi-step conversation with branching paths. Feels like talking to a character.

### M5: Inventory + Equipment System (Week 9-10)

**Deliverable**: The player can pick up items, equip armor and weapons, and see stats change.

- Inventory: Grid or list-based. Holds items with name, icon, stats, rarity.
- Equipment screen: 5 armor slots, 3 weapon slots. Drag-and-drop or click-to-equip.
- Item data: Data Table with all item definitions (name, mesh, stats, rarity, slot type).
- Stat display: Show current total stats (base + gear).
- Loot pickup: Items drop as world actors. Press E to pick up. Notification popup.
- Weapon swapping: Equipping a weapon changes the player's visible mesh and attack montages.
- Armor visual: Equipping armor changes the player's visible mesh per slot.
- Spell slots (1-5) and item/consumable slots (6-0) shown on HUD.
- **Test**: Can find a sword, equip it, see damage change. Can swap full armor sets.

### M6: XP + Leveling (Week 11-12)

**Deliverable**: The player gains XP, levels up, and allocates stat points.

- XP system: Earn from kills and quests. Track total XP. Level thresholds from the XP table.
- Level-up: Notification, VFX flash, sound. Grants 3 stat points.
- Stat allocation UI (UMG): Shows 5 stats with +/- buttons. Unspent points displayed.
- Stat effects: Connect Might to damage, Fortitude to HP and damage reduction, Haste to move speed and stamina regen, Agility to attack speed and dodge distance, Sorcery to spell damage and mana regen.
- Display: Current level and XP bar on HUD.
- **Test**: Kill enemies, see XP bar fill, level up, allocate points, feel the difference.

### M7: Inildon Greybox Level with NPCs (Week 13-14)

**Deliverable**: A walkable greybox Inildon with placed NPCs, the Divine intro, and tutorial flow.

- Greybox the city layout (6 districts). BSP brushes or simple meshes for buildings.
- Place NPCs: Queen Vaena, Morhri, Quenla, Renna, Toren, Yelin, Sila. All have dialogue.
- Divine intro: Camera sequence with text cards (pre-rendered or in-engine).
- Tutorial flow: Shrine -> streets -> Inilvan Command (weapon pickup) -> training dummies -> boars outside walls -> Queen's Court.
- Quest system: "The Road to River's Bend" quest accepted. Quest tracker on HUD.
- Level streaming: Inildon as a sub-level that loads when approaching the city gate.
- **Test**: A new player can go from character creation through the entire Inildon experience in 15-20 minutes.

### M8: Overworld Path to River's Bend with Enemies (Week 15-16)

**Deliverable**: The overworld path from Inildon to River's Bend, populated with enemies and points of interest.

- Terrain: 1.5-2 km of landscape. Sculpt terrain, place foliage, add the Thway River (water plane + particle mist).
- 3 zones: Inildon outskirts, Thway River Road, Approach to River's Bend.
- Enemy spawns: Boars, Wolves, Dred Minions, Dred Soldiers.
- Points of interest: Destroyed farmstead (Hedda quest), Old Guard camp (Orvald dialogue), burned watchtower (lore journal), River Overlook (Dark Forest vista + lore stone).
- Crafting nodes: Iron ore, timber, herbs placed along the path.
- World Partition: Break terrain into streaming cells.
- **Test**: Walking from Inildon to River's Bend takes 30-60 minutes. Encounters feel natural. XP gain puts the player at level 4-5 on arrival.

### M9: River's Bend Greybox + Quests (Week 17-18)

**Deliverable**: A walkable greybox River's Bend with NPCs, quests, and the combat/magic/crafting tutorials.

- Greybox River's Bend (smaller than Inildon). Timber walls, watchtowers, key buildings.
- Place NPCs: Wilo Lightbane, Wen Lightbane, Syla, Gaelen, Maren, Iona, Cael.
- Quests: "Report to the Lightbanes", "The Night Patrol" (wave defense), "Sparks of the Arcanem" (spell acquisition), "Iron and Fire" (smithing tutorial), "Shadows in the Trees" (dungeon breadcrumb), "Wilo's Defense" (side quest).
- Night Patrol gameplay: Scripted wave encounter outside the walls.
- Forge station: Functional crafting UI.
- Alchemy table: Functional basic potion crafting.
- **Test**: Player arrives at River's Bend and has 1-2 hours of content. Leaves at level 8-9.

### M10: Spell System (Week 19-20)

**Deliverable**: The player can equip and cast 5 spells. Spell slot UI works.

- Spell system: 5 slots (keys 1-2-3-4-5). Equip from inventory. Cast consumes mana. Cooldown per spell.
- Spell data: Data Table with name, mana cost, cooldown, damage, effect type, VFX reference.
- Implement 5 spells: Firebolt (projectile), Mend (self-heal), Frost Shard (projectile + slow), Thorn Lash (cone + slow), Entangle (ground AoE + root).
- VFX: Basic Niagara effects for each spell. 3 archetypes: projectile, self-buff particles, ground AoE.
- Mana system: Mana bar on HUD. Regen rate. Sorcery scaling.
- Spell tomes: World items that teach spells when used.
- **Test**: Can cast all 5 spells in combat. Mana management matters. Spells feel impactful.

### M11: Dark Forest Dungeon (Week 21-23)

**Deliverable**: The complete Dark Forest dungeon, playable solo from entrance to final boss.

- Build dungeon level: 5 sections (Outer Wood, Hollow Gorge, Broodmother Arena, Deep Root, Risen Knight Arena).
- Dungeon entrance: Social area with campfire, weapon rack, Ranger Cael.
- Enemy types: Corrupted Beetle, Spiderling, Nilvar Scout, Nilvar Warrior, Nilvar Shaman.
- Mini-boss: Corrupted Broodmother (Lunge Bite, Web Spit, Summon Spiderlings at 50%).
- Final boss: The Risen Knight (Phase 1 martial, Phase 2 corrupted fury, add spawn at 25%).
- Loot system: Enemy drop tables, chest loot, boss guaranteed rare drop.
- Dungeon instancing: Separate level load on entry.
- Environmental storytelling: Journal entries, Nilvar altars, vision sequence, wall carvings.
- **Test**: Solo playthrough takes 30-60 minutes. Boss fights are challenging but fair. Loot feels rewarding.

### M12: Crafting Basics (Week 24-25)

**Deliverable**: Smithing station works. Player can gather materials and craft iron-tier and dark iron-tier gear.

- Gathering: Pickaxe for ore, axe for timber. Interact with nodes, short gathering animation, item added to inventory.
- Smithing UI: Select recipe -> shows required materials -> craft -> item created with random stats.
- Recipe list: Iron weapons (sword, axe, mace, dagger, greatsword, spear, bow, shield), Iron armor (5 slots), Dark Iron weapons and armor (requires Dark Ore Fragments).
- Basic alchemy: Herb combination at alchemy table = health potion.
- Material storage: Materials stack in inventory.
- **Test**: Player can mine iron, craft a sword, and equip it. Dark Iron gear feels like meaningful progression.

### M13: Polish Pass (Week 26-28)

**Deliverable**: The entire vertical slice plays well. VFX, audio, UI, and game feel are polished.

- VFX pass: Ensure all spells, combat hits, environmental effects are visually clear and satisfying.
- Audio pass: Place placeholder SFX for all combat, UI, ambient, and boss events.
- UI pass: Ensure all HUD elements are readable, inventory is intuitive, dialogue UI is clean.
- Game feel: Tune hit-pause on melee impacts (2-3 frame freeze), camera shake on heavy hits, screen flash on damage taken.
- Bug fixing: Address all known issues from M1-M12.
- Balance pass: Tune XP curve, enemy HP/damage, spell cooldowns/costs, loot drop rates.
- Quest flow: Ensure the entire main quest path (Inildon -> River's Bend -> Dark Forest) flows without breaks or confusion.
- **Test**: Hand the build to someone who has never played it. Watch them play. Note every point of confusion, frustration, or boredom. Fix those.

### M14: Co-op Dungeon Networking (Week 29-32) -- If Phase 6 Reached

**Deliverable**: 2-4 players can run the Dark Forest dungeon together.

- UE5 networking: Replicate player character, enemies, and loot across a listen server or dedicated server.
- Party system: Invite, accept, group UI.
- Enemy scaling: HP and damage multipliers based on group size.
- Per-player loot: Each player sees independent drops.
- Death/respawn in co-op: Respawn at checkpoint with timer. Wipe on full party death.
- Social area: Multiple players visible at dungeon entrance. LFG signpost.
- Stress test: 4 players in dungeon simultaneously. Ensure no desyncs, rubberbanding, or crash.
- **Test**: 4 players complete the dungeon together. It is more fun than solo. Nobody disconnects.

**Note**: M14 is a stretch goal. Networking is the single hardest technical challenge in the project. The vertical slice is complete and shippable after M13. Networking can be deferred to a later development phase without impacting the single-player experience.

---

## Appendix A: Quest Summary Table

| Quest | Giver | Location | Level | Type | Reward |
|---|---|---|---|---|---|
| The Road to River's Bend | Queen Vaena | Inildon | 2 | Main | Inil leather armor set, 100 XP |
| Hedda's Supplies | Farmer Hedda | Inildon Outskirts | 3 | Side | 50 XP, health potion |
| Report to the Lightbanes | Madam Wilo Lightbane | River's Bend | 5 | Main | 100 XP, hub access |
| The Night Patrol | Wen Lightbane | River's Bend | 5-6 | Main | 200 XP, iron weapon choice |
| Sparks of the Arcanem | Arcanist Syla | River's Bend | 6-7 | Main | 150 XP, 2 spell tomes |
| Iron and Fire | Smith Gaelen | River's Bend | 6-7 | Main | 100 XP, smithing unlocked |
| Shadows in the Trees | Scout Maren | River's Bend | 7-8 | Main | 200 XP, dungeon quest unlocked |
| Wilo's Defense | Madam Wilo | River's Bend | 6 | Side | 100 XP, Entangle spell tome |
| Into the Dark | Auto (from Maren) | Dark Forest | 10 | Main | 300 XP, "Darkwalker" title, cosmetic cloak |

## Appendix B: New NPC Registry

All NPCs marked [NEW NPC] in this document. These are not canon characters and should be reviewed for lore consistency.

| Name | Location | Role | Notes |
|---|---|---|---|
| Drill Sergeant Renna | Inildon, Inilvan Command | Tutorial combat instructor | Gruff, practical |
| Smithmaster Toren | Inildon, Inilvan Command Forge | Basic weapon vendor, smithing teaser | Does not teach smithing -- that is Gaelen |
| Shrine Keeper Yelin | Inildon, Divine Shrine | Optional lore about the Divines | Quiet, reverent |
| Tavern Owner Sila | Inildon, Tavern | Rumors, side quests, dynamic dialogue | Social hub character |
| Farmer Hedda | Inildon Outskirts -> River's Bend | Side quest giver, relocates if quest completed | Adds world continuity |
| Captain Orvald | Old Guard Camp (overworld) | Dialogue only -- Old Guard exposition | Bitter, proud, anti-mage |
| Arcanist Syla | River's Bend, Chapel | Magic introduction, spell tome vendor | Arcanem representative |
| Smith Gaelen | River's Bend, Forge | Smithing tutorial, forge vendor | Practical, weathered |
| Scout Maren | River's Bend, Eastern Watchtower | Dark Forest reconnaissance, dungeon breadcrumb | Observant, cautious |
| Healer Iona | River's Bend, Infirmary | Health potion vendor, alchemy table access | Compassionate, overworked |
| Ranger Cael | Dark Forest Entrance | Dungeon lore and warnings | Enul ranger, quiet |

## Appendix C: Lore Suggestions (Non-Canon, Flagged for Review)

These are suggestions made within this document that are NOT part of established canon. They require human approval before being treated as lore.

1. **Inilium sealed**: The Inilium in Inildon is sealed by the Advisory Council during the vertical slice. Reason: the council's distrust of magic after the Inilvan split. Creates mystery, opens later.
2. **Wend spell tome vendor**: A Wend trader in Inildon harbor sells a spell tome at a premium price, giving players an optional early acquisition path.
3. **Old Guard questline**: The Old Guard (Captain Orvald) becomes a side questline in later phases -- earning their respect through martial feats.
4. **Dark Iron crafting**: Dark Ore Fragments from the dungeon craft into Dark Iron gear at Smith Gaelen -- the best vertical slice equipment, encouraging dungeon repeats.
5. **Em'Faira whisper**: Em'Faira speaks after the Risen Knight dies. Her voice establishes her as a lurking threat for future content.
6. **Stat respec**: A respec option available at a cost (gold or quest) in Phase 2 or later.
