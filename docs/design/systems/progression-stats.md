# THE REVEN -- Progression and Stats System Specification
**Created: 2026-03-25**
**Status: DESIGN DOCUMENT -- Pending human approval**
**Scope: Full game (levels 1-100), with vertical slice (levels 1-10) callouts**

---

## 1. STAT SYSTEM

Five core stats, each ranging from 0 to 100. All stats begin at 5 at level 1 (25 total base points). Stats increase through player-allocated stat points earned on level-up. Gear provides additional stat bonuses on top of allocated values.

### Stat Definitions and Scaling Formulas

#### Might
Primary effect: melee and ranged weapon damage, carry capacity (if implemented).
Primary armor slot: Head.

| Derived Value | Formula | At Might 0 | At Might 50 | At Might 100 |
|---|---|---|---|---|
| Melee Damage | `base_weapon_damage * (1 + Might * 0.005)` | 1.0x | 1.25x | 1.50x |
| Ranged Damage | `base_weapon_damage * (1 + Might * 0.004)` | 1.0x | 1.20x | 1.40x |
| Carry Capacity | `base_capacity * (1 + Might * 0.005)` | 1.0x | 1.25x | 1.50x |

Design note: Might scales melee slightly more than ranged. A pure Might build at 100 gets 50% bonus melee damage -- meaningful but not game-breaking. Ranged caps at 40% to keep ranged builds dependent on Agility for accuracy.

#### Fortitude
Primary effect: damage reduction, health regeneration, block efficiency, proximity map range.
Primary armor slot: Body.

| Derived Value | Formula | At Fort 0 | At Fort 50 | At Fort 100 |
|---|---|---|---|---|
| Damage Reduction | `Fortitude * 0.003` (capped at 0.30 = 30%) | 0% | 15% | 30% |
| Health Regen Rate | `base_regen * (1 + Fortitude / 200)` | 1.0x | 1.25x | 1.50x |
| Block Efficiency | `base_block * (1 + Fortitude * 0.004)` | 1.0x | 1.20x | 1.40x |
| Proximity Map Range | `base_range * (1 + Fortitude * 0.003)` | 1.0x | 1.15x | 1.30x |

Design note: Damage reduction is capped at 30% to prevent unkillable tank builds. Block efficiency reduces stamina cost of blocking, not damage absorbed directly. A shield with Fortitude 100 drains 40% less stamina per blocked hit.

#### Haste
Primary effect: movement speed, stamina regeneration, sprint speed.
Primary armor slot: Boots.

| Derived Value | Formula | At Haste 0 | At Haste 50 | At Haste 100 |
|---|---|---|---|---|
| Movement Speed | `base_speed * (1 + Haste * 0.002)` | 1.0x | 1.10x | 1.20x |
| Stamina Regen Rate | `base_regen * (1 + Haste / 200)` | 1.0x | 1.25x | 1.50x |
| Sprint Speed | `base_sprint * (1 + Haste * 0.003)` | 1.0x | 1.15x | 1.30x |

Design note: Movement speed is intentionally conservative (20% max). Faster movement trivializes encounter design. Sprint speed is slightly more generous because sprinting consumes stamina. Stamina regen is the primary PvE value of Haste.

#### Agility
Primary effect: attack speed, dodge distance, ranged accuracy.
Primary armor slot: Gloves.

| Derived Value | Formula | At Agi 0 | At Agi 50 | At Agi 100 |
|---|---|---|---|---|
| Attack Speed | `base_speed * (1 + Agility * 0.003)` | 1.0x | 1.15x | 1.30x |
| Dodge Distance | `base_distance * (1 + Agility * 0.003)` | 1.0x | 1.15x | 1.30x |
| Ranged Accuracy | `base_accuracy * (1 + Agility * 0.004)` | 1.0x | 1.20x | 1.40x |

Design note: Attack speed caps at 30% bonus to prevent animation distortion. At Agility 100, attacks are noticeably faster but still readable. Ranged accuracy reduces projectile spread and arrow arc deviation -- bows become more precise, not faster.

#### Sorcery
Primary effect: spell damage, mana regeneration, spell cast speed.
Primary armor slot: Legs.

| Derived Value | Formula | At Sorc 0 | At Sorc 50 | At Sorc 100 |
|---|---|---|---|---|
| Spell Damage | `base_spell_damage * (1 + Sorcery * 0.006)` | 1.0x | 1.30x | 1.60x |
| Mana Regen Rate | `base_regen * (1 + Sorcery / 200)` | 1.0x | 1.25x | 1.50x |
| Spell Cast Speed | `base_cast * (1 + Sorcery * 0.003)` | 1.0x | 1.15x | 1.30x |

Design note: Sorcery scales spell damage more aggressively than Might scales weapon damage (60% vs 50%). This compensates for mana costs and cooldowns -- melee has no resource cost per swing, so magic needs a higher damage ceiling to stay competitive. Cast speed is conservative to preserve spell readability.

### Stat Interaction with Gear

Gear provides flat stat bonuses (e.g., Iron Helm: Might +3, Fortitude +4). These bonuses add directly to the player's stat total before formula calculation. A player with 20 allocated Might and +10 Might from gear has an effective Might of 30.

Stats from gear are NOT capped at 100. A player with 100 allocated Might can equip gear with +15 Might for an effective Might of 115. However, the scaling formulas use diminishing returns above 100 naturally (linear scaling means each additional point above 100 provides the same flat bonus, which is a smaller percentage of the total).

---

## 2. DYNAMIC STATS

Three resource pools that define moment-to-moment survival.

### Health

| Property | Value |
|---|---|
| Base | 100 |
| Level Scaling | +10 per level |
| Fortitude Scaling | +2 per point of Fortitude |
| Formula | `max_health = 100 + (level * 10) + (Fortitude * 2)` |

Examples:
- Level 1, Fortitude 5: 100 + 10 + 10 = **120 HP**
- Level 10, Fortitude 12: 100 + 100 + 24 = **224 HP**
- Level 50, Fortitude 60: 100 + 500 + 120 = **720 HP**
- Level 100, Fortitude 100: 100 + 1000 + 200 = **1,300 HP**

### Stamina

| Property | Value |
|---|---|
| Base | 100 |
| Level Scaling | None |
| Haste Scaling | None (Haste affects regen rate only) |
| Formula | `max_stamina = 100` |

Stamina is a fixed pool. It does not grow. This is intentional: stamina management remains a constant pressure throughout the game. A level 100 player still has 100 stamina. What changes is how fast it regenerates (Haste) and how efficiently it is spent (Agility for dodge cost, Fortitude for block cost). Players never outgrow stamina management.

Stamina costs (fixed, do not scale):
- Dodge roll: 25
- Heavy attack: 20
- Sprint: 5 per second
- Block: variable per hit absorbed (proportional to attack power, reduced by block efficiency)

### Mana

| Property | Value |
|---|---|
| Base | 100 |
| Level Scaling | +5 per level |
| Sorcery Scaling | +1.5 per point of Sorcery |
| Formula | `max_mana = 100 + (level * 5) + (Sorcery * 1.5)` |

Examples:
- Level 1, Sorcery 5: 100 + 5 + 7.5 = **112 Mana** (rounded to 113)
- Level 10, Sorcery 25: 100 + 50 + 37.5 = **188 Mana**
- Level 50, Sorcery 60: 100 + 250 + 90 = **440 Mana**
- Level 100, Sorcery 100: 100 + 500 + 150 = **750 Mana**

Mana cost reference (from spell-remediation.md): `mana_cost = level_requirement * 2`. Firebolt (level 1) costs 2 mana. Inferno (level 30) costs 60 mana. At level 100 with 750 mana, a pure caster can fire 12 Infernos before running dry -- assuming no regen. In practice, regen fills a significant portion between cooldowns.

---

## 3. REGENERATION

All regeneration values are expressed as percentage of maximum pool per second. Regeneration ticks every 0.5 seconds (value divided by 2 per tick) for smooth visual feedback.

### Health Regeneration

| Property | Value |
|---|---|
| Base Rate | 1% of max HP per second |
| Fortitude Scaling | `rate = 0.01 * max_hp * (1 + Fortitude / 200)` |
| At Fortitude 0 | 1.0% per second |
| At Fortitude 50 | 1.25% per second |
| At Fortitude 100 | 1.50% per second |

Example: Level 10, Fortitude 12, max HP 224. Regen = 0.01 * 224 * (1 + 12/200) = 2.24 * 1.06 = **2.37 HP/sec** in combat.

### Stamina Regeneration

| Property | Value |
|---|---|
| Base Rate | 3% of max stamina per second (3 stamina/sec, since max is always 100) |
| Haste Scaling | `rate = 0.03 * max_stamina * (1 + Haste / 200)` |
| At Haste 0 | 3.0 stamina/sec |
| At Haste 50 | 3.75 stamina/sec |
| At Haste 100 | 4.5 stamina/sec |
| Regen Pause | 1 second after any stamina-consuming action |

Note: The vertical slice document states 10/sec base stamina regen. This is a design conflict. **Recommendation: use 10/sec as the effective in-game base** (matching the VS doc), which means the base rate is actually 10% per second, not 3%. If 10/sec is adopted:

| Property | Revised Value |
|---|---|
| Base Rate | 10% of max stamina per second (10 stamina/sec) |
| Haste Scaling | `rate = 0.10 * max_stamina * (1 + Haste / 200)` |
| At Haste 0 | 10 stamina/sec |
| At Haste 50 | 12.5 stamina/sec |
| At Haste 100 | 15 stamina/sec |

**Decision needed**: 3/sec (punishing, Dark Souls-like) or 10/sec (generous, action RPG). The vertical slice document uses 10/sec. This spec defers to the VS value unless overridden.

### Mana Regeneration

| Property | Value |
|---|---|
| Base Rate | 2% of max mana per second |
| Sorcery Scaling | `rate = 0.02 * max_mana * (1 + Sorcery / 200)` |
| At Sorcery 0 | 2.0% per second |
| At Sorcery 50 | 2.5% per second |
| At Sorcery 100 | 3.0% per second |

Example: Level 10, Sorcery 25, max mana 188. Regen = 0.02 * 188 * (1 + 25/200) = 3.76 * 1.125 = **4.23 mana/sec**. Firebolt costs 2 mana with a 3-second cooldown. The player regens ~12.7 mana between Firebolt casts -- effectively infinite Firebolts at this level. Higher-cost spells like Entangle (10 mana, 10-sec cooldown) regen ~42 mana between casts. Mana pressure increases at higher levels when casting multiple expensive spells in sequence.

Note: The vertical slice document states 5/sec base mana regen. Similar conflict to stamina. At level 1 with 113 max mana, 2% = 2.26/sec vs the VS doc's 5/sec. **Recommendation: use the percentage-based system** (scales with level), but set the percentage such that level-1 effective regen is approximately 5/sec. That would require a base rate of ~4.4% (5/113). Alternatively, use flat 5/sec at level 1 and add Sorcery scaling. **Decision needed.**

### Out-of-Combat Regeneration

| Property | Value |
|---|---|
| Trigger | 5 seconds of no damage taken AND no damage dealt |
| Multiplier | 3x normal regeneration rate |
| Applies To | Health, Stamina, and Mana |
| Visual Indicator | Faint golden glow around player when OOC regen is active |

At level 10 with balanced stats, out-of-combat regen:
- Health: ~7 HP/sec (full heal in ~30 seconds)
- Stamina: ~30 stamina/sec (full in ~3 seconds)
- Mana: ~13 mana/sec (full in ~15 seconds)

This eliminates downtime between encounters without trivializing in-combat resource management. The 5-second delay prevents abuse during boss fights (any DOT or summoned creature resets the timer).

---

## 4. LEVELING

### Core Parameters

| Property | Value |
|---|---|
| Max Level | 100 |
| XP Curve | Quadratic: `xp_needed = 100 * level^1.5` |
| Stat Points Per Level | 3 |
| Total Stat Points at Level 100 | 297 (99 level-ups * 3) + 25 base = **322 total** |
| Max Possible in One Stat | 100 (with 222 spread across remaining 4) |
| Points to Max All 5 Stats | 500 (impossible -- forces build identity) |

At level 100 with 322 total stat points, a player could theoretically put 100 in their primary stat, 100 in their secondary, and have 122 remaining for three stats (avg ~41 each). Or spread 64-65 across all five. Neither build maxes everything -- there is always a tradeoff.

### XP Sources

| Source | XP Value | Notes |
|---|---|---|
| Kill: Wildlife (at-level) | 10-20 | Boar, wolf, deer |
| Kill: Common Enemy (at-level) | 15-25 | Dred Minion |
| Kill: Standard Enemy (at-level) | 30-50 | Dred Soldier, Nilvar Scout |
| Kill: Elite Enemy (at-level) | 40-60 | Nilvar Warrior |
| Kill: Mini-boss | 100-150 | Dungeon mid-boss |
| Kill: Final Boss | 300-500 | Dungeon end boss |
| Quest: Main Quest Step | 100-300 | Major story beats |
| Quest: Side Quest | 50-150 | Optional content |
| Exploration: Discover Location | 25-50 | First visit to named location |
| Exploration: Lore Stone | 10-25 | Environmental lore objects |

### Kill XP Scaling

XP from kills scales based on enemy level relative to player level:

```
level_diff = enemy_level - player_level

if level_diff >= -5:
    xp_multiplier = 1.0
elif level_diff >= -10:
    xp_multiplier = 0.10  (10% -- anti-grind)
else:
    xp_multiplier = 0.0   (no XP from trivial enemies)

if level_diff > 0:
    xp_multiplier = 1.0 + (level_diff * 0.10)  (10% bonus per level above, capped at 1.5x)
```

Enemies more than 5 levels below the player give 10% XP. Enemies more than 10 levels below give nothing. Enemies above the player give bonus XP up to 50% extra, rewarding players who tackle harder content.

### Quest XP

Quest XP is fixed and does not scale with level. The quest rewards listed in the vertical slice document are the canonical values. Higher-level quests in later zones provide proportionally more XP to match the steeper curve.

### Exploration XP

Discovery XP is granted once per location per character. Named locations (cities, camps, landmarks, dungeon entrances) each have a discovery XP value. This rewards players who explore off the beaten path.

---

## 5. LEVEL 1-10 PROGRESSION TABLE

XP curve values are taken from the vertical slice document (which uses a custom curve tuned for the 1-10 experience). The quadratic formula `100 * level^1.5` produces similar values and governs levels 11+.

| Level | XP to Next | Total XP | Max HP | Max Mana | Stat Points Available | Expected Stats (Melee Build) | Milestone |
|---|---|---|---|---|---|---|---|
| 1 | 100 | 0 | ~120 | ~113 | 0 (base 25) | M5/F5/H5/A5/S5 | Awaken in Inildon. Pick up first weapon. Kill boars. |
| 2 | 150 | 100 | ~130 | ~118 | 3 | M7/F6/H5/A6/S5 | Tutorial complete. Leave Inildon. First Dred Minions. |
| 3 | 200 | 250 | ~140 | ~123 | 6 | M9/F7/H6/A7/S5 | Road Zone 1. Hedda's quest. Wolves appear. |
| 4 | 300 | 450 | ~150 | ~128 | 9 | M11/F8/H6/A8/S5 | Road Zone 2. Dred Soldiers. Old Guard camp. |
| 5 | 400 | 750 | ~160 | ~133 | 12 | M12/F9/H7/A9/S5 | Arrive River's Bend. Meet the Lightbanes. Hub opens. |
| 6 | 500 | 1,150 | ~170 | ~138 | 15 | M14/F10/H7/A10/S5 | Night Patrol. Iron weapon choice. Wilo's Defense. |
| 7 | 600 | 1,650 | ~180 | ~143 | 18 | M15/F11/H8/A10/S5 | First spells (Firebolt/Mend + Thorn Lash). Smithing tutorial. |
| 8 | 750 | 2,250 | ~190 | ~148 | 21 | M17/F11/H8/A11/S5 | Crafted iron gear. Shadows in the Trees quest. Nilvar Scouts. |
| 9 | 1,000 | 3,000 | ~200 | ~153 | 24 | M18/F12/H9/A11/S5 | Enter Dark Forest dungeon. Full combat toolkit. |
| 10 | -- | 4,000 | ~210 | ~158 | 27 | M20/F12/H9/A12/S5 | Risen Knight defeated. Dark Iron gear. 3-5 spells equipped. |

Notes on the melee build column:
- Prioritizes Might and Agility (damage and attack speed).
- Fortitude secondary (survivability).
- Haste tertiary (stamina regen).
- Sorcery untouched (uses spells as utility only, base mana is sufficient for Mend).

A magic build at level 10 would look more like: M5/F8/H7/A7/S25 -- dumping nearly every point into Sorcery for spell damage and mana. A balanced build: M10/F10/H10/A10/S12.

### What Each Level Feels Like

**Level 1**: The player is weak. Iron sword hits a boar for ~15 damage. Boars have ~40 HP. Three hits. Feels fair. Dred Minions hit the player for ~10 damage against 120 HP -- not threatening individually but dangerous in groups of 4+.

**Level 3**: The player has gear and a few stat points. Sword damage is ~18. Wolves with ~60 HP take 3-4 hits. Wolves deal ~15 damage per lunge. A pack of 3 wolves is a real fight.

**Level 5**: Player arrives at River's Bend. Sword damage is ~20-22. Dred Soldiers have ~120 HP. Takes 5-6 hits. Soldiers deal ~20 damage per swing and can block. Player needs to use parry, dodge, and stamina management.

**Level 7**: First spells. Firebolt deals ~25 damage (Sorcery 5, minimal scaling). With Sorcery 5, spells are utility, not primary damage. A magic build with Sorcery 25 deals ~40 damage with Firebolt.

**Level 10**: Full toolkit. Iron sword with Might 20 deals ~26 damage. Dred Soldiers (level 8-9) have ~150 HP -- 6 hits. Nilvar Warriors (level 9) have ~200 HP -- 8 hits. The Risen Knight has ~2,000 HP (10x a standard enemy). Solo fight lasts 2-3 minutes with good play.

---

## 6. GEAR SCORE / POWER LEVEL

### Formula

```
gear_score = sum of all base stat values on all equipped items
```

Each equipped item contributes its total stat bonuses to the gear score. A full set of Iron gear with average rolls (~3.5 per stat, ~2.5 stats per piece, 8 equipment slots) yields a gear score of approximately 70. Dark Iron gear (avg ~8 per stat) yields approximately 160.

### Gear Score Tiers

| Tier | Gear Score Range | Expected Content |
|---|---|---|
| Starter | 0-30 | Inildon, basic leather |
| Iron | 30-80 | Road to River's Bend |
| Reinforced | 80-140 | River's Bend, approach to dungeon |
| Dark Iron | 140-220 | Dark Forest dungeon farming |
| Steel (Phase 2+) | 220-350 | Arcanem, Crossroads |
| Hallowed Steel (Phase 3+) | 350-500 | Dosram, high-level dungeons |
| Masterwork (Phase 4+) | 500-700 | Endgame crafting |
| Legendary (Phase 5+) | 700+ | Raid equivalents, world bosses |

### Usage

- **Content gating**: Dungeons display a recommended gear score. Below threshold, enemies hit noticeably harder. Not a hard lock -- players can attempt undergeared, but the game warns them.
- **Matchmaking (future)**: PvP and co-op matchmaking can use gear score brackets to create fair matches.
- **UI**: Gear score is NOT prominently displayed in the vertical slice. It exists in the character sheet as a small number. It becomes more visible in Phase 4+ when multiplayer features expand.
- **Destiny 2 compatibility**: The system is designed to support future power-level gating similar to Destiny 2's light level. Content can require minimum gear scores, and seasonal gear can raise the cap. This is deferred to live-service design decisions.

---

## 7. FACTION REPUTATION

### Factions

| Faction | Starting Rep | Capital | Region |
|---|---|---|---|
| Inil | Friendly (500/1000) | Inildon | Inilgree |
| Dosram | Neutral (250/1000) | Donrid (destroyed) | Dosramgree |
| Enul | Neutral (250/1000) | Lonun | Enulgree |

### Reputation Scale

| Tier | Rep Range | Label | Effect |
|---|---|---|---|
| 1 | 0-99 | Hostile | Vendors refuse service. Guards warn, then attack on repeated trespass. Prices 2x. |
| 2 | 100-249 | Unfriendly | Limited vendor access (basic goods only). Prices 1.5x. |
| 3 | 250-499 | Neutral | Standard vendor access. Standard prices. |
| 4 | 500-749 | Friendly | Full vendor access. Prices 0.9x. Faction-specific side quests available. |
| 5 | 750-999 | Honored | Faction gear vendor unlocks (cosmetic + stat gear). Prices 0.8x. Additional dialogue options. |
| 6 | 1000 | Exalted | All faction content unlocked. Best prices (0.7x). Unique title. Faction leader acknowledges player by name. |

### Reputation Sources

| Action | Rep Change | Notes |
|---|---|---|
| Complete faction main quest | +50 to +100 | Major story beats |
| Complete faction side quest | +25 to +50 | Optional content |
| Kill enemies in faction territory | +5 per kill | Defending their land |
| Donate materials to faction NPC | +10 per donation | Gold sink / material sink |
| Negative actions (steal, harm NPCs) | -25 to -100 | NOT in vertical slice |

### Design Rules

- **NOT blocking**: Players can always access all geographic areas regardless of reputation. Hostile reputation means NPCs are unfriendly and prices are high, not that the player is locked out of zones.
- **Rep affects costs**: The price multiplier is the primary mechanical effect. Friendly factions sell cheaper. Hostile factions gouge. This creates a soft economic incentive to build reputation.
- **Cross-faction dynamics**: Raising one faction's rep does NOT lower another's. Factions are independent. A player can be Exalted with all three. Faction conflict is narrative, not mechanical.
- **Vertical slice**: Only Inil reputation is active. Player starts Friendly (500) because they appear in Inildon as a Reven sent by the Divines -- the Inil trust them immediately. Dosram and Enul rep exist in the data but have no active content until later phases.

### Faction Gear (Per Faction)

Each faction has a themed gear set available at Honored (750) reputation:

- **Inil**: Celestial Guard set. White steel with golden engravings. Bonuses to Fortitude and Sorcery.
- **Dosram**: Ironhide set. Heavy furs over dark iron plate. Bonuses to Might and Fortitude.
- **Enul**: Greenward set. Natural fiber and shaped wood. Bonuses to Agility and Haste.

Faction gear is competitive with same-level dungeon drops but visually distinct. It provides a non-RNG path to good equipment for players who prefer questing over dungeon farming.

---

## 8. SKILL PROGRESSION

### Core Concept

Skills level up through use. Swinging a sword levels sword skill. Casting fire spells levels fire magic. Smithing items levels smithing. The system is invisible in the vertical slice -- numbers go up in the background, but the UI does not display skill levels until Phase 4.

### Skill Categories

#### Combat Skills (Level Through Use)

| Skill | Levels With | Passive Bonus (per 10 levels) |
|---|---|---|
| Sword | Hitting enemies with a sword | +3% sword damage |
| Axe | Hitting enemies with an axe | +3% axe damage |
| Mace | Hitting enemies with a mace | +3% mace damage |
| Dagger | Hitting enemies with a dagger | +3% dagger damage |
| Greatsword | Hitting enemies with a greatsword | +3% greatsword damage |
| Spear | Hitting enemies with a spear | +3% spear damage |
| Bow | Hitting enemies with a bow | +3% bow damage, +2% accuracy |
| Shield | Blocking attacks with a shield | +3% block efficiency, -2% block stamina cost |

#### Magic Skills (Level Through Use)

| Skill | Levels With | Passive Bonus (per 10 levels) |
|---|---|---|
| Flora Magic | Casting Flora spells | +3% Flora spell damage |
| Earth Magic | Casting Earth spells | +3% Earth spell damage |
| Fire Magic | Casting Fire spells | +3% Fire spell damage |
| Water Magic | Casting Water spells | +3% Water spell damage |
| Wild Magic | Casting Wild spells | +3% Wild spell effectiveness |
| Restoration | Casting Restoration spells | +3% healing amount |
| Temporal | Casting Temporal spells | +5% Temporal spell duration |
| Soul | Casting Soul spells | +3% Soul spell damage/shield amount |
| Necromancy | Casting Necromancy spells | +3% Necromancy damage, +2% summon duration |
| Conjuration | Casting Conjuration spells | +3% Conjuration damage, +2% summon duration |
| Blood | Casting Blood spells | +3% Blood spell damage |

#### Crafting Skills (Level Through Use)

| Skill | Levels With | Passive Bonus (per 10 levels) |
|---|---|---|
| Smithing | Crafting items at a forge | +5% chance of bonus stat roll on crafted items |
| Alchemy | Crafting potions at an alchemy table | +10% potion effectiveness |
| Enchanting | Applying enchantments | +5% enchantment potency |

### Skill XP Per Action

```
skill_xp_per_action = base_action_xp * (1 + enemy_level_diff * 0.1)
```

- Sword swing that hits an at-level enemy: 1 skill XP
- Sword swing that hits an enemy 5 levels above: 1.5 skill XP
- Sword swing that hits an enemy 5 levels below: 0.5 skill XP
- Spell cast that hits an at-level enemy: 2 skill XP (spells are cast less frequently than swings)
- Crafting an item: 5-20 skill XP (based on item tier)

### Skill Level Curve

```
skill_xp_needed = 50 * skill_level^1.3
```

Skill level 10 requires ~1,000 total skill XP. Skill level 50 requires ~25,000 total skill XP. Skill level 100 requires ~100,000 total skill XP.

A player using a sword as their primary weapon will naturally reach sword skill 10-15 by level 10. They will not notice this -- there is no UI. The 3% damage bonus at skill 10 is baked into their damage output silently.

### Mastery Milestone: Might 100 + Sword Skill 100

At Might 100 AND sword skill 100, the player unlocks the ability to wield a two-handed sword in one hand. This is a canon ability -- the pinnacle of martial mastery. The off-hand becomes free for a shield, a torch, or a second one-handed weapon (dual wield, if implemented).

This milestone is communicated through a special notification: *"Your mastery of the blade transcends convention. Two-handed swords bend to your will."* No skill UI is needed -- the unlock simply appears.

Additional mastery milestones (examples, not exhaustive):
- **Fortitude 100 + Shield Skill 100**: Shield bash becomes a stun (1.5 seconds). Knockback on all blocked hits.
- **Agility 100 + Dagger Skill 100**: Backstab damage doubled. Silent movement while crouching.
- **Sorcery 100 + Fire Magic 100**: Fire spells leave a burning ground area for 3 seconds after impact.
- **Haste 100 + Bow Skill 100**: Fully charged bow shots pierce through the first target.

### Visibility

- **Vertical slice (Phase 1)**: Skills level up silently. No UI. Players feel their character getting slightly better without knowing why.
- **Phase 4**: Skill levels displayed in character sheet. Milestone progress shown. Players can now see and chase specific milestones.
- **Rationale**: Showing complex skill numbers to new players in the first 2 hours overwhelms them. Let the stat system (5 simple stats) be the visible progression. Skills are the hidden depth layer.

---

## 9. PVP NORMALIZATION

PvP is not in the vertical slice. These rules are designed now to ensure the stat system supports PvP when it is implemented (Phase 6+).

### Core Principle

**Skill > gear in PvP, but gear still matters.** A level 50 player should beat a level 10 player most of the time, but the level 10 player should be able to land meaningful hits and occasionally win through superior play. A level 50 player should NEVER one-shot a level 10 player.

### Normalization Rules

| Rule | PvE Value | PvP Value | Notes |
|---|---|---|---|
| Damage Multiplier | 1.0x | 0.5x | All player-to-player damage halved |
| Stat Range | 0-100+ | Compressed to 20-80 effective | Stats below 20 are treated as 20. Stats above 80 are treated as 80. |
| CC Duration | Up to 5 seconds | 2 seconds max | All crowd control durations capped |
| Healing Effectiveness | 100% | 50% | Prevents unkillable healer builds |
| DOT Damage | 100% | 40% | Prevents cheese kills from stacking DOTs |
| Damage Reduction Cap | 30% | 20% | Prevents unkillable tank builds in PvP |

### Stat Compression Formula

In PvP, effective stats are compressed:

```
pvp_stat = 20 + (raw_stat - 20) * (60 / 80)  [clamped to 20-80 range]
```

A player with 100 Might has an effective PvP Might of 80. A player with 5 Might has an effective PvP Might of 20. The gap between them is 60 effective stat points instead of 95 raw stat points -- a 37% compression. This means a high-level player is still stronger, but not overwhelmingly so.

### PvP-Specific Mechanics

- **Diminishing returns on CC**: The first CC on a player lasts full duration (up to 2 seconds). The second CC within 10 seconds lasts 50% duration. The third and subsequent CCs within 10 seconds last 25% duration. After 10 seconds without CC, the counter resets.
- **PvP resilience**: A hidden stat that reduces PvP damage taken by 5% per PvP match completed (capped at 25%). This gives new PvP players a ramp-up period where they take slightly less damage as they learn. Resets weekly.
- **Gear score brackets**: PvP matchmaking groups players by gear score within ~50 points of each other. Players can opt in to open brackets (no gear restriction) for competitive play.

---

## 10. UE5 IMPLEMENTATION

### Component Architecture

All progression and stat systems are implemented as Actor Components attached to the player character. This keeps the systems modular and testable.

#### StatsComponent (UStatsComponent)

Holds all 5 core stats, dynamic stats, and derived values.

```
Properties:
- CoreStats[5]: int32 (Might, Fortitude, Haste, Agility, Sorcery)
- AllocatedStats[5]: int32 (points the player has allocated)
- GearStats[5]: int32 (sum of all equipped gear bonuses)
- EffectiveStats[5]: int32 (Allocated + Gear, computed)
- MaxHealth: float (computed from formula)
- CurrentHealth: float
- MaxStamina: float (always 100)
- CurrentStamina: float
- MaxMana: float (computed from formula)
- CurrentMana: float
- HealthRegenRate: float (computed)
- StaminaRegenRate: float (computed)
- ManaRegenRate: float (computed)
- DamageReduction: float (computed from Fortitude)
- MoveSpeedMultiplier: float (computed from Haste)
- AttackSpeedMultiplier: float (computed from Agility)
- SpellDamageMultiplier: float (computed from Sorcery)
- bIsInCombat: bool
- TimeSinceLastCombatAction: float
- bOOCRegenActive: bool

Functions:
- RecalculateDerivedStats(): recomputes all derived values from core stats
- ApplyDamage(float amount, DamageType type): reduces HP, resets OOC timer
- ConsumeStamina(float amount): reduces stamina, starts regen pause
- ConsumeMana(float amount): reduces mana
- Tick(): handles regeneration ticks (every 0.5 sec)
```

#### LevelingComponent (ULevelingComponent)

Tracks XP, level, and stat point allocation.

```
Properties:
- CurrentLevel: int32
- CurrentXP: int32
- XPToNextLevel: int32 (computed from curve)
- UnspentStatPoints: int32
- TotalStatPointsEarned: int32

Functions:
- AddXP(int32 amount): adds XP, checks for level-up
- LevelUp(): increments level, grants 3 stat points, recalculates XP threshold
- AllocateStatPoint(ECoreStat stat): spends 1 unspent point into chosen stat
- GetXPForLevel(int32 level): returns XP needed (100 * level^1.5)

Events:
- OnLevelUp: broadcast when player levels up (triggers VFX, sound, UI)
- OnXPGained: broadcast when XP is added (updates XP bar)
```

#### ReputationComponent (UReputationComponent)

Tracks per-faction reputation values.

```
Properties:
- FactionRep: TMap<EFaction, int32> (Inil, Dosram, Enul)
- FactionTier: TMap<EFaction, ERepTier> (computed from rep value)

Functions:
- AddReputation(EFaction faction, int32 amount): modifies rep, recalculates tier
- GetRepTier(EFaction faction): returns current tier (Hostile through Exalted)
- GetPriceMultiplier(EFaction faction): returns cost modifier based on tier
- CanAccessVendor(EFaction faction, EVendorType type): checks if vendor is available at current tier
```

#### SkillProgressionComponent (USkillProgressionComponent)

Tracks weapon, magic, and crafting skill levels.

```
Properties:
- SkillLevels: TMap<ESkillType, int32>
- SkillXP: TMap<ESkillType, int32>
- UnlockedMilestones: TSet<ESkillMilestone>

Functions:
- AddSkillXP(ESkillType skill, int32 amount): adds skill XP, checks for level-up
- GetSkillLevel(ESkillType skill): returns current level
- GetPassiveBonus(ESkillType skill): returns cumulative passive bonus (3% per 10 levels)
- CheckMilestones(): evaluates all milestone conditions (e.g., Might 100 + Sword 100)

Events:
- OnSkillLevelUp: broadcast when a skill levels up (silent in vertical slice)
- OnMilestoneUnlocked: broadcast when a mastery milestone is achieved
```

### Data Tables

All tuning values live in UE5 Data Tables, NOT hardcoded in Blueprints. This allows designers to adjust balance without touching logic.

| Data Table | Contents |
|---|---|
| DT_StatScaling | Per-stat scaling coefficients for all derived values |
| DT_XPCurve | XP thresholds per level (1-100) |
| DT_EnemyXP | Base XP values per enemy type |
| DT_FactionRep | Rep tier thresholds and price multipliers |
| DT_SkillXP | Skill XP curve, passive bonus values per skill |
| DT_SkillMilestones | Milestone conditions and unlock effects |
| DT_GearScoreTiers | Gear score brackets and content recommendations |
| DT_PvPNormalization | PvP damage multipliers, stat compression values, CC caps |

### Stat Allocation UI

Level-up screen (UMG Widget):

```
Layout:
- Title: "LEVEL UP" with new level number
- 5 rows, one per stat:
  [ Stat Name ] [ Current Value ] [ - ] [ + ] [ Effect Preview ]
- Unspent points counter at top
- "Confirm" button (grayed out until all points spent or player chooses to save them)
- "Reset" button (returns to pre-allocation state, not a respec)

Behavior:
- [+] adds 1 point to that stat, decrements unspent counter
- [-] removes 1 point added THIS session only (cannot remove previously allocated points)
- Effect Preview shows the delta: "Melee Damage: 1.10x -> 1.115x"
- Player can bank unspent points (close the screen with points remaining)
- Banked points persist and can be spent later via the character sheet
```

---

## 11. BALANCE CHECKPOINTS

### Level 1 -- Inildon Start

| Property | Value |
|---|---|
| HP | ~120 |
| Mana | ~113 |
| Stamina | 100 |
| Stats | 5/5/5/5/5 (all base) |
| Weapon | Basic Iron Sword (base damage 15) |
| Armor | None (or Inil Leather after Queen's court: ~5 gear score) |
| Spells | None |
| Damage Output | ~15 per swing (iron sword, no Might bonus) |
| Damage Taken | ~10 per boar charge, ~8 per Dred Minion hit |
| Enemies Nearby | Boars (40 HP), Dred Minions (30 HP) |
| Kills to Level | Kill ~5 boars + interact with tutorial NPCs = level 2 |

**Feel**: The player is new and fragile but not helpless. 3 hits kill a boar. A group of 4 Dred Minions is dangerous (120 total damage if all hit vs 120 HP). Dodge and block matter immediately.

### Level 5 -- Arrive at River's Bend

| Property | Value |
|---|---|
| HP | ~160 |
| Mana | ~133 |
| Stamina | 100 |
| Stats | ~12/9/7/9/5 (melee build) |
| Weapon | Iron Sword (base damage 15, effective ~16.5 with Might 12) |
| Armor | Inil Leather set (~25 gear score) |
| Spells | None yet (acquired at River's Bend) |
| Damage Output | ~17 per swing |
| Damage Taken | ~15 per Dred Soldier hit (before DR ~2.7%) |
| Enemies Nearby | Dred Soldiers (120 HP), Wolves (60 HP) |
| Hits to Kill Dred Soldier | ~7 swings |
| Player Deaths Expected | 0-2 on the road (wolves or Soldier ambush) |

**Feel**: The player is competent. They have survived the road. Dred Soldiers are a fair fight -- blocking and dodging required. Arriving at River's Bend feels like reaching safety.

### Level 10 -- Post-Dungeon

| Property | Value |
|---|---|
| HP | ~210 |
| Mana | ~158 |
| Stamina | 100 |
| Stats | ~20/12/9/12/5 (melee build) |
| Weapon | Iron Sword (base 15) or crafted Dark Iron Sword (base 22) |
| Armor | Iron set (~70 gear score) or partial Dark Iron (~120 gear score) |
| Spells | 3-5 equipped (Firebolt, Mend, Frost Shard, Thorn Lash, Entangle) |
| Damage Output (Iron Sword) | ~16.5 per swing (15 * 1.10 Might bonus) |
| Damage Output (Dark Iron Sword) | ~24 per swing (22 * 1.10) |
| Damage Taken | ~20 per Nilvar Warrior hit (before DR ~3.6%) |
| Risen Knight HP | ~2,000 |
| Hits to Kill Risen Knight (Iron) | ~121 swings (supplemented by spells and combos) |
| Hits to Kill Risen Knight (Dark Iron) | ~83 swings |
| Expected Boss Fight Duration | 2-3 minutes with active play |

**Feel**: The player has a defined build. They have made meaningful choices about stats and gear. The dungeon boss is a legitimate challenge. Dark Iron gear feels like a significant power jump. Spells add tactical depth even for melee builds (Mend for sustain, Entangle for crowd control).

### Level 30 -- Post-Dosram / NW Dred Fortress

| Property | Value |
|---|---|
| HP | ~470 (100 + 300 + Fort 35 * 2) |
| Mana | ~350 (100 + 150 + Sorc 20 * 1.5 -- melee build with low Sorc) |
| Stamina | 100 |
| Stats | ~55/35/25/30/20 (melee specialist) |
| Weapon | Hallowed Steel Greatsword (base damage ~45) |
| Armor | Full Steel/Hallowed Steel mix (~400 gear score) |
| Spells | 5 equipped, 15-20 known. Mix of Terrestrial and Astral. |
| Damage Output | ~57 per greatsword swing (45 * 1.275 Might bonus) |
| Damage Taken | ~35 per Dred Tanker hit (before DR ~10.5%) |
| Enemies | Dred Tankers (800 HP), Dregs (500 HP), Nilvar elite patrols |
| NW Fortress Boss HP | ~8,000 |
| Expected Boss Fight Duration | 4-5 minutes |

**Feel**: The player is powerful. Common enemies die in 2-3 hits. Elite enemies require full engagement. The player has a clear combat identity -- their build is THEIRS. Spell rotation is practiced and optimized. Gear farming is a known loop. Faction gear from Inil or Dosram supplements dungeon drops. The world is opening up -- multiple quest paths, multiple zones accessible.

### Level 100 -- Endgame Power Ceiling

| Property | Value |
|---|---|
| HP | ~1,300 (100 + 1000 + Fort 100 * 2) |
| Mana | ~750 (100 + 500 + Sorc 100 * 1.5) -- pure caster; melee build ~350 |
| Stamina | 100 |
| Stats (melee) | 100/80/50/60/32 (322 total) |
| Stats (caster) | 30/50/40/32/100 (252 allocated + 70 from gear) |
| Stats (balanced) | 65/65/64/64/64 |
| Weapon | Legendary Greatsword (base damage ~80) or Masterwork equivalents |
| Armor | Full Legendary set (~750 gear score) |
| Damage Output (melee, Legendary GS) | ~120 per swing (80 * 1.50 Might) before skill bonuses |
| Damage Output (caster, Inferno) | ~320 per cast (200 base * 1.60 Sorcery) before skill bonuses |
| Damage Reduction (Fort 80) | ~24% |
| Health Regen (OOC) | ~58.5 HP/sec (1,300 * 0.015 * 3x OOC) |
| Mana Regen (OOC, Sorc 100) | ~67.5 mana/sec (750 * 0.03 * 3x OOC) |

**Feel**: The player is a force. They have mastered their build. Common enemies evaporate. Endgame content (final dungeon, world bosses, PvP) provides challenge through mechanics, not raw numbers. The player's power fantasy is fully realized:
- A Might 100 warrior wields a greatsword one-handed, cleaving through Dred ranks.
- A Sorcery 100 mage rains Inferno on the battlefield, Blinks away from danger, Rewinds when caught.
- A balanced build adapts to any situation -- sword for melee, spells for range, enough health to tank a hit, enough dodge to avoid the next.

The gap between a level 1 player and a level 100 player:
- HP: 120 vs 1,300 (10.8x)
- Melee damage: 15 vs 120 (8x)
- Spell damage: N/A vs 320
- Damage reduction: 0% vs 24%

This is a large but not insurmountable gap. In PvP with normalization, it compresses to approximately 3-4x effective power difference -- enough to feel meaningful, not enough to make fights hopeless.

### Damage Reference Table

| Scenario | Damage | Target HP | Hits to Kill |
|---|---|---|---|
| Lv1 player (iron sword) vs Boar (lv1) | ~15 | 40 | 3 |
| Lv1 player (iron sword) vs Dred Minion (lv2) | ~15 | 30 | 2 |
| Lv5 player (iron sword) vs Dred Soldier (lv5) | ~17 | 120 | 7 |
| Lv10 player (iron sword) vs Dred Soldier (lv8) | ~17 | 150 | 9 |
| Lv10 player (Dark Iron sword) vs Dred Soldier (lv8) | ~24 | 150 | 7 |
| Lv10 player (iron sword) vs Risen Knight (lv10) | ~17 | 2,000 | ~118 |
| Lv10 player (Dark Iron sword) vs Risen Knight (lv10) | ~24 | 2,000 | ~84 |
| Lv10 player Firebolt (Sorc 5) vs Dred Soldier (lv8) | ~25 | 150 | 6 casts |
| Lv10 player Firebolt (Sorc 25) vs Dred Soldier (lv8) | ~40 | 150 | 4 casts |
| Lv30 player (Hallowed Steel GS) vs Dred Tanker (lv30) | ~57 | 800 | 14 |
| Lv100 player (Legendary GS) vs endgame elite | ~120 | 5,000 | ~42 |
| Lv100 player Inferno (Sorc 100) vs endgame elite | ~320 | 5,000 | ~16 casts |

---

## Appendix A: Full XP Curve (Levels 1-100)

The quadratic formula `xp_needed = 100 * level^1.5` (rounded to nearest 10) governs levels 11+. Levels 1-10 use the hand-tuned values from the vertical slice document.

| Level | XP to Next | Total XP | Notes |
|---|---|---|---|
| 1 | 100 | 0 | Tutorial |
| 2 | 150 | 100 | |
| 3 | 200 | 250 | |
| 4 | 300 | 450 | |
| 5 | 400 | 750 | River's Bend |
| 6 | 500 | 1,150 | |
| 7 | 600 | 1,650 | |
| 8 | 750 | 2,250 | |
| 9 | 1,000 | 3,000 | |
| 10 | 1,000 | 4,000 | Dungeon |
| 15 | 5,810 | ~18,000 | Arcanem |
| 20 | 8,940 | ~48,000 | Amanar dungeon |
| 25 | 12,500 | ~100,000 | |
| 30 | 16,430 | ~175,000 | NW Dred Fortress |
| 40 | 25,300 | ~390,000 | |
| 50 | 35,360 | ~700,000 | Mid-game |
| 60 | 46,480 | ~1,100,000 | |
| 70 | 58,570 | ~1,650,000 | |
| 80 | 71,550 | ~2,300,000 | |
| 90 | 85,380 | ~3,100,000 | |
| 100 | -- | ~4,000,000 | Endgame |

Note: Intermediate values are approximations. The exact Data Table will contain all 100 rows computed from the formula. Total XP to reach level 100 is approximately 4 million.

## Appendix B: Stat Scaling Quick Reference

All formulas in one place for implementation:

```
--- DAMAGE ---
melee_damage    = base_weapon_damage * (1 + Might * 0.005)
ranged_damage   = base_weapon_damage * (1 + Might * 0.004)
spell_damage    = base_spell_damage  * (1 + Sorcery * 0.006)

--- DEFENSE ---
damage_reduction = Fortitude * 0.003  (cap 0.30)
block_efficiency = base_block * (1 + Fortitude * 0.004)

--- POOLS ---
max_health  = 100 + (level * 10) + (Fortitude * 2)
max_stamina = 100  (fixed)
max_mana    = 100 + (level * 5) + (Sorcery * 1.5)

--- REGEN (per second, in combat) ---
health_regen  = 0.01 * max_health  * (1 + Fortitude / 200)
stamina_regen = 10  (base, per VS doc) * (1 + Haste / 200)
mana_regen    = 0.02 * max_mana    * (1 + Sorcery / 200)

--- REGEN (out of combat: 5 sec no damage) ---
ooc_multiplier = 3.0

--- MOVEMENT ---
move_speed   = base_speed  * (1 + Haste * 0.002)
sprint_speed = base_sprint * (1 + Haste * 0.003)

--- COMBAT SPEED ---
attack_speed = base_speed    * (1 + Agility * 0.003)
cast_speed   = base_cast     * (1 + Sorcery * 0.003)
dodge_dist   = base_distance * (1 + Agility * 0.003)

--- ACCURACY ---
ranged_accuracy = base_accuracy * (1 + Agility * 0.004)

--- SKILL BONUSES ---
weapon_skill_bonus = floor(skill_level / 10) * 0.03
magic_skill_bonus  = floor(skill_level / 10) * 0.03

--- XP ---
xp_needed = 100 * level^1.5  (levels 11+; levels 1-10 hand-tuned)
kill_xp   = base_xp * level_diff_multiplier

--- GEAR SCORE ---
gear_score = sum(all equipped item base stat values)

--- PVP ---
pvp_damage = pve_damage * 0.5
pvp_stat   = clamp(20 + (raw_stat - 20) * 0.75, 20, 80)
pvp_cc_cap = 2.0 seconds
pvp_heal   = pve_heal * 0.5
```

## Appendix C: Open Questions

1. **Stamina regen rate conflict**: This spec proposes 3%/sec base; the vertical slice document uses 10/sec flat. These produce very different combat feels. Needs playtesting to resolve.
2. **Mana regen rate conflict**: Similar issue. Percentage-based (2%/sec) vs flat (5/sec from VS doc). Percentage scales with level; flat does not.
3. **Respec cost**: How much should a stat respec cost? Suggestion: 500 gold at level 10, scaling with level (50 * level). Available from a specific NPC (Arcanist Syla or an Arcanem NPC in Phase 2).
4. **Skill level visibility timing**: Phase 4 is recommended, but some players may want earlier feedback. Consider a toggle in settings: "Show skill levels" (default off).
5. **Gear stat overflow**: Should gear push stats above 100? Current design says yes (no cap). This means endgame gear can push a maxed stat to 130+, which provides linear (not diminishing) returns. May need a soft cap at 120 with reduced scaling above.
6. **Faction rep decay**: Should reputation decay over time if the player ignores a faction? Current design says no -- once earned, reputation is permanent. Decay adds realism but also grind.
7. **Crafting skill impact on stat rolls**: At Smithing 100, the +50% chance of a bonus stat roll is very powerful. Is this too strong? Should the bonus be capped or curve differently?
