# THE REVEN -- Inventory & Equipment System
**Created: 2026-03-25**
**Status: DESIGN DOCUMENT -- Pending human approval**
**Scope: Full system spec with vertical slice priorities marked**
**Implementation: UE5 Blueprint-first, solo dev**

---

## 1. SLOT LAYOUT

### Armor Slots (5)

| Slot | Primary Stat | Key Bind |
|---|---|---|
| Head | Might | Equipped via inventory UI |
| Body | Fortitude | Equipped via inventory UI |
| Gloves | Agility | Equipped via inventory UI |
| Legs | Sorcery | Equipped via inventory UI |
| Boots | Haste | Equipped via inventory UI |

All five armor slots are visible on the character paperdoll in the equipment panel. Each slot accepts only its armor type -- a helmet cannot go in the boots slot.

### Weapon Slots (3)

| Slot | Accepts | Notes |
|---|---|---|
| Main Hand | Sword, Axe, Mace, Dagger, Bow (2H) | Active weapon. Left-click attacks use this. |
| Off Hand | Shield, Torch | Active off-hand. Only usable with 1H main hand. Auto-unequips if a 2H weapon is in Main Hand. |
| Back | Greatsword, Glaive, Spear | 2H weapons only. Visually shown on the character's back when not active. |

### Weapon Switching

The player has two loadouts: **Active** (Main Hand + Off Hand) and **Back**.

- **Press X** (rebindable): Swap loadouts. The Main Hand + Off Hand weapons go on the back, and the Back weapon comes to the hands.
- When a 2H weapon is active (from Back slot), Off Hand is disabled. The player cannot block with a shield while wielding a greatsword.
- When a Bow is in Main Hand, Off Hand is also disabled. The bow occupies both hands.
- Swapping has a 0.5-second animation (weapon sheathe/draw). The player cannot attack during the swap but can dodge-cancel out of it.
- If the Back slot is empty, pressing X does nothing.

**Example loadouts:**
- Sword + Shield (active) / Greatsword (back) -- press X to swap between defensive 1H and offensive 2H.
- Dagger + Torch (active) / Spear (back) -- fast melee with light source, swap to reach weapon.
- Bow (active, 2H) / Glaive (back) -- ranged to melee swap.

### Spell Slots (5)

| Slot | Key | Notes |
|---|---|---|
| Spell 1 | 1 | Assigned from known spells in the spell menu |
| Spell 2 | 2 | |
| Spell 3 | 3 | |
| Spell 4 | 4 | |
| Spell 5 | 5 | |

Spells are learned permanently from Spell Tomes (consumed on use). Learned spells go into a spell book. The player drags spells from the spell book into slots 1-5. Spell loadout cannot be changed during combat (combat state = any enemy aggro within 30m).

### Item / Consumable Slots (5)

| Slot | Key | Notes |
|---|---|---|
| Item 1 | 6 | |
| Item 2 | 7 | |
| Item 3 | 8 | |
| Item 4 | 9 | |
| Item 5 | 0 | |

Accepts: Health potions, mana potions, stamina potions, food, arrows (for quick-reference ammo count), throwables. Press the key to use. Items with stacks show the count on the HUD slot icon. Consumables are used instantly (0.5-sec use animation, interruptible by taking damage).

---

## 2. INVENTORY GRID

### Grid Layout

- **30 slots** at game start, arranged as a 6-column x 5-row grid.
- Every item occupies exactly 1 slot (no multi-slot Tetris items -- keeps Blueprint implementation simple).
- Expandable: additional rows of 6 unlock via crafting a "Traveler's Pack" (Phase 4). Max 60 slots (6x10).

### Auto-Sort

Press a "Sort" button in the inventory UI to auto-sort items by category, then by rarity (highest first), then alphabetically within rarity.

**Sort order by category:**
1. Weapons
2. Armor
3. Consumables (potions, food, arrows)
4. Materials (ore, leather, timber, herbs, essence)
5. Artifacts (quest items, spell tomes, keys)

Quest items and key items are non-droppable and non-sellable. They display a lock icon.

### Stack Sizes

| Item Type | Max Stack |
|---|---|
| Materials (ore, timber, leather, herbs) | 99 |
| Consumables (potions, food) | 20 |
| Arrows | 99 |
| Essence (from deconstruction) | 999 |
| Gold | Not an inventory item -- tracked as a currency counter on the UI |
| Weapons | 1 (no stacking) |
| Armor | 1 (no stacking) |

### Weight Limit

**No weight limit.** Inventory is constrained by slot count only. This avoids the tedium of weight management and keeps the system simple. If the player's inventory is full, they cannot pick up new items until they sell, deconstruct, or drop something. A "Your inventory is full" message appears on failed pickup.

### Search / Filter (Phase 4)

Not in the vertical slice. When implemented:
- Text search bar at the top of the inventory.
- Filter buttons per category (weapon icon, armor icon, potion icon, material icon, artifact icon).
- Clicking a filter toggles visibility of that category.

---

## 3. GEAR STATS & RANDOM ROLLS

### How Armor Stats Work

Every armor piece has:
1. **Base Armor Value** -- determined by material tier (see Section 4). Reduces incoming physical damage.
2. **Primary Stat** -- determined by the armor slot. Head always rolls Might, Body always rolls Fortitude, etc. The primary stat is guaranteed.
3. **Secondary Stats** -- random rolls from the remaining 4 stats. Count and range depend on rarity.

### Rarity Tiers

| Rarity | Color | Primary Stat | Secondary Stats | Value Range Multiplier | Drop Rate |
|---|---|---|---|---|---|
| Common | White | Yes (low roll) | 0 | 1.0x | 60% |
| Uncommon | Green | Yes (mid roll) | 1 random | 1.25x | 25% |
| Rare | Blue | Yes (high roll) | 2 random | 1.5x | 10% |
| Epic | Purple | Yes (very high roll) | 3 random | 2.0x | 4% |
| Legendary | Gold | Fixed unique stats | Special effect | Hand-crafted | 1% |

### Stat Ranges by Material Tier and Rarity (Levels 1-10)

**Primary Stat Ranges:**

| Material Tier | Common | Uncommon | Rare | Epic |
|---|---|---|---|---|
| Leather (T1) | 1-2 | 2-3 | 3-4 | 4-5 |
| Iron (T2) | 2-3 | 3-5 | 4-6 | 5-7 |
| Reinforced Iron (T3) | 3-5 | 5-7 | 6-8 | 7-10 |
| Dark Iron (T4) | 5-7 | 7-9 | 8-11 | 10-13 |

**Secondary Stat Ranges** (when rolled):

| Material Tier | Uncommon (1 stat) | Rare (2 stats each) | Epic (3 stats each) |
|---|---|---|---|
| Leather (T1) | 1 | 1 | 1-2 |
| Iron (T2) | 1-2 | 1-2 | 1-3 |
| Reinforced Iron (T3) | 1-3 | 2-3 | 2-4 |
| Dark Iron (T4) | 2-4 | 3-5 | 3-6 |

### How Random Rolls Are Generated

When an item drops or is crafted, the game rolls stats using this procedure (all in Blueprint):

```
1. Determine rarity (roll against drop rate table, or fixed for crafting).
2. Look up material tier from the item's DataTable row.
3. Roll PRIMARY stat:
   - Get min/max from the Primary Stat Range table for this tier + rarity.
   - Roll a random integer in [min, max] (uniform distribution).
   - Assign to the slot's primary stat (Head -> Might, etc.).
4. Roll SECONDARY stats (if rarity allows):
   - Build pool of 4 remaining stats (exclude primary).
   - For each secondary slot (1 for Uncommon, 2 for Rare, 3 for Epic):
     a. Pick a random stat from the pool (weighted -- see below).
     b. Remove it from the pool (no duplicate stats on one item).
     c. Roll value in [min, max] from Secondary Stat Range table.
5. Store the final stat rolls in the item's FInventoryItem struct.
```

**Secondary stat weighting:**
Each of the 4 remaining stats has equal weight (25% each). No bias. This keeps the system fair and simple. If playtesting reveals certain stats are undervalued, weights can be adjusted in a DataTable without changing Blueprint logic.

### Legendary Items

Legendaries do NOT use random rolls. Each legendary is a hand-crafted DataTable row with:
- Fixed name and description.
- Fixed stats (primary + all 3 secondaries at set values).
- A unique **Special Effect** (passive ability). Examples:
  - "Risen Knight's Helm" -- +8 Might, +5 Fortitude, +3 Agility. Special: "Unyielding: Stamina break duration reduced by 50%."
  - "Em'Faira's Embrace" (Legs) -- +10 Sorcery, +6 Haste, +4 Fortitude. Special: "Corrupted Bloom: Thorn Lash spells heal the caster for 15% of damage dealt."

Legendaries are NOT in the vertical slice. They require the Special Effect system, which is deferred to Phase 5.

---

## 4. MATERIAL TIERS (Vertical Slice: Levels 1-10)

| Tier | Name | Level Range | Base Armor Value | Source | Visual Description |
|---|---|---|---|---|---|
| T1 | Leather | 1-3 | 3 | Quest reward (Morhri), wildlife drops, vendor | Plain brown leather. No decoration. Stitched seams visible. |
| T2 | Iron | 4-6 | 6 | Crafted from Iron Ore at forge, enemy drops | Basic grey metal plates over leather. Simple rivets. Unpolished. |
| T3 | Reinforced Iron | 7-9 | 10 | Crafted with extra materials, rare enemy drops | Layered metal plates with leather underlayer. Slightly polished. Visible reinforcement bands. |
| T4 | Dark Iron | 10 | 15 | Crafted from Dark Ore Fragments (dungeon), dungeon boss drops | Black-tinted metal with faint green veins. Jagged edges. Corrupted but powerful. |

### Base Armor Value Effect

Base Armor Value provides flat physical damage reduction. At level 10 with a full set of 5 armor pieces:
- Full Leather: 15 total armor (reduces incoming physical damage by ~10%)
- Full Iron: 30 total armor (~18%)
- Full Reinforced Iron: 50 total armor (~28%)
- Full Dark Iron: 75 total armor (~38%)

Exact damage reduction formula: `Reduction% = Armor / (Armor + 100) * 100`. This gives diminishing returns at high armor values, preventing armor from ever reaching 100% reduction.

### Crafting Recipes per Tier

**T1 Leather:**
- Not craftable. Drops from wildlife (boars, wolves) or given as quest reward.

**T2 Iron:**
- 5 Iron Ore -> 1 Iron Ingot (at forge)
- 2 Iron Ingots -> 1 Iron Weapon (sword, axe, mace, dagger, shield, bow, greatsword, spear)
- 3 Iron Ingots -> 1 Iron Armor piece (head, body, gloves, legs, boots)

**T3 Reinforced Iron:**
- 3 Iron Ingots + 2 Leather + 1 Timber -> 1 Reinforced Iron Armor piece
- 3 Iron Ingots + 1 Leather -> 1 Reinforced Iron Weapon
- Requires Smithing Level 3+

**T4 Dark Iron:**
- 3 Dark Ore Fragments + 2 Iron Ingots -> 1 Dark Iron Weapon
- 3 Dark Ore Fragments + 3 Iron Ingots + 2 Leather -> 1 Dark Iron Armor piece
- Requires Smithing Level 5+
- Dark Ore Fragments only drop in the Dark Forest dungeon (3 per full clear: Broodmother, altar, Risen Knight)

### Visual Variants

Each material tier needs distinct mesh or material instance variants for all 5 armor slots. This means 4 tiers x 5 slots = 20 armor visual variants. To reduce workload:
- Use a single base mesh per slot with material instance swaps (change base color, roughness, emissive for the green veins on Dark Iron).
- T1 and T2 can share a mesh with different materials.
- T3 adds geo detail (reinforcement bands) -- can be a material-driven normal map.
- T4 uses T3 mesh with Dark Iron material + emissive green channel.

---

## 5. WEAPON DATA

### Weapon Types

| Weapon | Slot | Hands | Base Damage (T2 Iron) | Attack Speed | Stamina Cost (Light/Heavy) | Special |
|---|---|---|---|---|---|---|
| Sword | Main Hand | 1H | 12 | 1.0x (baseline) | 10 / 20 | 3-hit combo. Third hit deals 1.5x damage. |
| Axe | Main Hand | 1H | 16 | 0.8x | 12 / 25 | Cleave: hits all enemies in a 120-degree arc. |
| Mace | Main Hand | 1H | 18 | 0.7x | 15 / 28 | Armor Break: 25% bonus damage to armored enemies. 1.5x guard break multiplier on heavy attack. |
| Dagger | Main Hand | 1H | 8 | 1.5x | 5 / 12 | 4-hit combo. Backstab: 2x damage when hitting from behind. |
| Shield | Off Hand | 1H | 0 | -- | 0 (blocking drains per hit) | Block absorbs damage. Parry window: 0.3 sec. Damage absorbed scales with Fortitude. |
| Torch | Off Hand | 1H | 3 (fire) | 0.6x | 8 / -- | Light source (10m radius). Minimal fire damage. Burns out after 10 minutes real-time; replaceable. |
| Greatsword | Back | 2H | 28 | 0.5x | 18 / 35 | Wide sweep: 180-degree arc. Highest single-hit damage. Overhead slam heavy attack. |
| Glaive | Back | 2H | 20 | 0.75x | 14 / 28 | Reach: 1.5x attack range. Thrust + sweep combos. Good crowd control. |
| Spear | Back | 2H | 16 | 0.85x | 10 / 22 | Longest reach (2x range). Thrust-only. Strong vs single target. Can attack while backpedaling. |
| Bow | Main Hand | 2H | 14-22 (charge) | Variable | 15 (full draw) | Ranged. 3 charge tiers (0.5s/1.0s/1.5s). Requires arrows. Projectile has travel time + arc. |

### Weapon Damage Scaling by Material Tier

Multiply the T2 Iron base damage by these factors:

| Tier | Multiplier |
|---|---|
| T1 Leather/Wood (starter) | 0.6x |
| T2 Iron | 1.0x (baseline) |
| T3 Reinforced Iron | 1.4x |
| T4 Dark Iron | 1.8x |

Example: A Dark Iron Greatsword deals 28 * 1.8 = 50 base damage.

### Weapon Stat Rolls

Weapons also roll bonus stats, same rarity system as armor:
- Common: No bonus stats.
- Uncommon: +1 random stat.
- Rare: +2 random stats.
- Epic: +3 random stats.

Weapon secondary stat ranges follow the same table as armor secondary stats (Section 3).

### Weapon DataTable Schema (FWeaponDefinition)

```
Row Name: weapon_id (FName) -- e.g., "WPN_Iron_Sword", "WPN_DarkIron_Greatsword"

| Column | Type | Example | Notes |
|---|---|---|---|
| weapon_id | FName | WPN_Iron_Sword | Unique ID, matches row name |
| display_name | FText | "Iron Sword" | Localized display name |
| description | FText | "A standard iron blade." | Tooltip description |
| weapon_type | EWeaponType (Enum) | Sword | Sword, Axe, Mace, Dagger, Shield, Torch, Greatsword, Glaive, Spear, Bow |
| slot | EWeaponSlot (Enum) | MainHand | MainHand, OffHand, Back |
| hands | int32 | 1 | 1 or 2 |
| base_damage | float | 12.0 | Base damage before stat scaling |
| attack_speed | float | 1.0 | Multiplier (1.0 = baseline) |
| stamina_cost_light | float | 10.0 | Stamina per light attack |
| stamina_cost_heavy | float | 20.0 | Stamina per heavy attack |
| required_level | int32 | 4 | Minimum player level to equip |
| material_tier | EMaterialTier (Enum) | Iron | Leather, Iron, ReinforcedIron, DarkIron |
| rarity | ERarity (Enum) | Common | Common, Uncommon, Rare, Epic, Legendary |
| mesh | TSoftObjectPtr<UStaticMesh> | -- | Weapon mesh reference |
| icon | TSoftObjectPtr<UTexture2D> | -- | Inventory icon |
| special_text | FText | "" | Special effect description (Legendary only) |
```

---

## 6. LOOT DROPS

### Enemy Loot Tables

Each enemy type has a loot table defined in a DataTable. On kill, the game rolls against the table to determine drops.

**Loot Table Schema (FLootTableRow):**

```
| Column | Type | Notes |
|---|---|---|
| enemy_type | EEnemyType | Which enemy uses this table |
| item_id | FName | Reference to the item DataTable |
| drop_chance | float | 0.0 - 1.0 probability |
| min_quantity | int32 | Minimum stack if dropped |
| max_quantity | int32 | Maximum stack if dropped |
| rarity_override | ERarity | If set, forces this rarity. If "None", uses global rarity roll. |
```

### Vertical Slice Enemy Loot Tables

**Wild Boar (Level 1-2):**

| Drop | Chance | Quantity |
|---|---|---|
| Leather | 80% | 1-2 |
| Raw Meat (food) | 40% | 1 |
| Nothing | 20% | -- |

**Wolf (Level 3):**

| Drop | Chance | Quantity |
|---|---|---|
| Leather | 90% | 1-3 |
| Wolf Fang (material) | 25% | 1 |
| Nothing | 10% | -- |

**Dred Minion (Level 2-3):**

| Drop | Chance | Quantity |
|---|---|---|
| Chitin Shard | 50% | 1 |
| Gold | 70% | 3-8 |
| Nothing | 30% | -- |

**Dred Soldier (Level 4-5):**

| Drop | Chance | Quantity |
|---|---|---|
| Iron Ore | 40% | 1-2 |
| Leather Armor piece (random slot) | 15% | 1 |
| Iron Weapon (random) | 10% | 1 |
| Gold | 85% | 8-20 |

**Dred Woldred (Level 5-6):**

| Drop | Chance | Quantity |
|---|---|---|
| Leather | 60% | 2-3 |
| Chitin Shard | 30% | 1-2 |
| Gold | 80% | 10-25 |

**Nilvar Scout (Level 7-8, Dungeon):**

| Drop | Chance | Quantity |
|---|---|---|
| Poisoned Dagger (weapon) | 15% | 1 |
| Leather Scraps | 50% | 1-2 |
| Dark Herbs | 40% | 1-2 |
| Gold | 75% | 12-30 |

**Nilvar Warrior (Level 8-9, Dungeon):**

| Drop | Chance | Quantity |
|---|---|---|
| Iron Weapon (random) | 20% | 1 |
| Iron Armor piece (random) | 15% | 1 |
| Iron Ore | 50% | 1-3 |
| Gold | 85% | 15-35 |

**Nilvar Shaman (Level 9, Dungeon):**

| Drop | Chance | Quantity |
|---|---|---|
| Spell Tome: Thorn Lash | 30% (only if player does not know it) | 1 |
| Dark Herbs | 60% | 2-3 |
| Gold | 90% | 20-40 |

### Rarity Roll (Global)

When an enemy drops a gear piece (weapon or armor), a separate rarity roll determines its quality:

| Rarity | Weight | Cumulative |
|---|---|---|
| Common (White) | 60% | 60% |
| Uncommon (Green) | 25% | 85% |
| Rare (Blue) | 10% | 95% |
| Epic (Purple) | 4% | 99% |
| Legendary (Gold) | 1% | 100% |

Implementation: Roll a random float [0.0, 1.0]. If < 0.60 -> Common. If < 0.85 -> Uncommon. If < 0.95 -> Rare. If < 0.99 -> Epic. Else -> Legendary.

**Note:** Legendary drops are disabled in the vertical slice (no legendary items exist yet). Any legendary roll becomes Epic instead.

### Boss Loot

**Corrupted Broodmother (Mini-Boss):**
- 1x guaranteed armor piece (random slot), rarity = Rare or better (roll: 70% Rare, 25% Epic, 5% Legendary->Epic)
- 1x Dark Ore Fragment (100%)
- 2-3x Health Potions (100%)
- 30-50 Gold (100%)
- 1x Spider Silk (100%, crafting material)

**The Risen Knight (Final Boss):**
- 1x guaranteed armor piece (random slot), rarity = Rare or better (50% Rare, 40% Epic, 10% Legendary->Epic)
- 1x Dark Ore Fragment (100%)
- 500 Gold (100%)
- 1x chance at a second gear piece (30%, uses standard rarity roll)

### Gathering Nodes

| Node Type | Location | Yield | Respawn Time |
|---|---|---|---|
| Iron Ore Deposit | River cliffs, shallow caves | 2-4 Iron Ore | 5 minutes (real-time) |
| Timber (harvestable tree) | Northern forest near River's Bend | 2-3 Timber | 5 minutes |
| Herb Plant | Riverbanks, forest clearings | 1-2 Herbs | 3 minutes |
| Dark Ore Vein | Dark Forest dungeon only | 1 Dark Ore Fragment | Per dungeon run (not respawning) |

### Gold Sources

| Source | Gold Amount |
|---|---|
| Dred Minion kill | 3-8 |
| Dred Soldier kill | 8-20 |
| Nilvar kill | 12-40 |
| Boss kill | 30-500 |
| Quest reward (main) | 50-200 |
| Quest reward (side) | 25-100 |
| Selling Common gear | 5-15 |
| Selling Uncommon gear | 20-40 |
| Selling Rare gear | 50-100 |
| Selling Epic gear | 100-250 |

---

## 7. ITEM COMPARISON

### Tooltip Layout

When hovering over an item in the inventory (or looking at a loot drop), a tooltip appears showing:

```
+----------------------------------+
| [ICON]  ITEM NAME          [RARITY COLOR BORDER]
| "Iron Helm"
| Rarity: Uncommon
| Material: Iron (Tier 2)
| Required Level: 4
|----------------------------------|
| Base Armor: 6
|----------------------------------|
| Might: +4        [GREEN UP ARROW +2]
| Fortitude: +2    [RED DOWN ARROW -1]
|----------------------------------|
| [Currently Equipped: Leather Helm]
| [Click to Equip]
+----------------------------------+
```

### Comparison Arrows

When the player has a piece equipped in the same slot, the tooltip shows comparison arrows next to each stat:

| Symbol | Color | Meaning |
|---|---|---|
| UP ARROW (+N) | Green (#00FF00) | This item has N more of this stat than equipped |
| DOWN ARROW (-N) | Red (#FF4444) | This item has N less of this stat than equipped |
| EQUALS (=) | Yellow (#FFFF00) | Same value, OR trade-off (one stat up, another down by similar amount) |
| (no arrow) | Grey | Stat not present on either item |

### Sidegrade Detection

If the total stat difference (sum of all stat changes) is between -2 and +2 but individual stats differ, show a yellow "SIDEGRADE" label below the comparison. This tells the player the item is roughly equivalent but different -- a build choice, not an upgrade or downgrade.

### Implementation (Blueprint)

1. On hover, get the hovered item's FInventoryItem.
2. Look up the corresponding equipped item in the same slot from EquipmentComponent.
3. For each stat on the hovered item, subtract the equipped item's value for that stat.
4. Display delta with colored arrow widget.
5. If no item is equipped in that slot, show all stats as green (any gear > no gear).

---

## 8. CRAFTING INTEGRATION

### Smithing Station

Smithing stations are placed at forges in Inildon (Smithmaster Toren) and River's Bend (Smith Gaelen). The player interacts with the forge (press E) to open the Smithing UI.

**Smithing UI Layout:**
```
+-----------------------------------------------+
| SMITHING                    [Smithing Level: 1]|
|-----------------------------------------------|
| CRAFT  |  UPGRADE  |  DECONSTRUCT             |
|-----------------------------------------------|
| Recipe List:                | Preview:         |
| > Iron Sword     [2 Ingots] | [Item Icon]     |
| > Iron Axe       [2 Ingots] | Iron Sword      |
| > Iron Mace      [2 Ingots] | Damage: 12      |
| > Iron Shield    [2 Ingots] | Speed: 1.0x     |
| > Iron Helm      [3 Ingots] | Stats: [random] |
| > Iron Body      [3 Ingots] | Materials:      |
| > Iron Gloves    [3 Ingots] |  2x Iron Ingot  |
| > Iron Legs      [3 Ingots] |                 |
| > Iron Boots     [3 Ingots] | [CRAFT]         |
| > Smelt: Iron Ore -> Ingot  |                 |
+-----------------------------------------------+
```

### Crafting

- Select a recipe from the list.
- If the player has sufficient materials, the CRAFT button is enabled.
- Press CRAFT. A 2-second progress bar plays (cancellable).
- The item is created with a random rarity roll (weighted toward Common for low smithing levels):
  - Smithing Level 1-2: 80% Common, 15% Uncommon, 5% Rare
  - Smithing Level 3-4: 60% Common, 25% Uncommon, 12% Rare, 3% Epic
  - Smithing Level 5+: 40% Common, 30% Uncommon, 20% Rare, 8% Epic, 2% Epic (better rolls)
- Stat rolls are generated per Section 3 rules.
- Crafting grants Smithing XP (see below).

### Upgrading

- Select an owned item in the UPGRADE tab.
- Spend materials to increase one stat on the item by 1-2 points.
- Cost: 1 Iron Ingot per upgrade (T2), 1 Dark Ore Fragment per upgrade (T4).
- Each item can be upgraded a maximum of 3 times. A counter shows "Upgrades: 1/3".
- Each upgrade increases one random existing stat by +1 (or +2 at Smithing Level 5+).
- Upgrading grants Smithing XP.

### Deconstructing

- Select an owned item in the DECONSTRUCT tab.
- The item is destroyed. The player receives:
  - 50% of the crafting materials (rounded down, minimum 1).
  - Essence based on rarity: Common = 1, Uncommon = 3, Rare = 8, Epic = 20, Legendary = 50.
- Deconstructing does NOT grant Smithing XP (prevents exploiting the craft-deconstruct loop).
- A confirmation dialog appears: "Deconstruct Iron Sword? This cannot be undone. You will receive: 1x Iron Ingot, 1x Essence."

### Smithing Level

| Smithing Level | XP Required | Unlocks |
|---|---|---|
| 1 | 0 (starting) | Iron-tier recipes. Smelting. |
| 2 | 100 | Improved rarity odds when crafting. |
| 3 | 300 | Reinforced Iron recipes. |
| 4 | 600 | Upgrading items. |
| 5 | 1000 | Dark Iron recipes. Better upgrade rolls (+2 instead of +1). |

**Smithing XP Sources:**
- Smelt ore to ingot: 5 XP
- Craft a weapon: 15 XP
- Craft an armor piece: 20 XP
- Upgrade an item: 25 XP

### Enchanting (Phase 5 -- NOT in Vertical Slice)

Enchanting is locked until the Arcanem questline (level 15+). The system when implemented:

- **Learn enchantments** by deconstructing enchanted gear at an Enchanting Table. The enchantment is learned permanently.
- **Apply enchantments** at an Enchanting Table: select a gear piece + a known enchantment. Cost: Essence + Gold.
- **Enchantment examples** (future content): Fire Damage +5, Frost Resistance +10%, Mana Regen +2/sec, Life Steal 3%.
- Enchanting Tables exist in the vertical slice world but are non-interactive. Tooltip: "Requires Arcanem training."
- Each item can hold 1 enchantment. Applying a new one overwrites the old one.

---

## 9. UE5 IMPLEMENTATION

### Core Structs

**FItemDefinition** (DataTable row struct -- defines what an item IS):

```cpp
USTRUCT(BlueprintType)
struct FItemDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere) FName ItemID;                    // "ITM_Iron_Helm"
    UPROPERTY(EditAnywhere) FText DisplayName;               // "Iron Helm"
    UPROPERTY(EditAnywhere) FText Description;               // "A sturdy iron helmet."
    UPROPERTY(EditAnywhere) EItemCategory Category;          // Weapon, Armor, Consumable, Material, Artifact
    UPROPERTY(EditAnywhere) EItemSubType SubType;            // Head, Body, Gloves, Legs, Boots, Sword, Axe, etc.
    UPROPERTY(EditAnywhere) EEquipSlot EquipSlot;            // Head, Body, Gloves, Legs, Boots, MainHand, OffHand, Back, None
    UPROPERTY(EditAnywhere) EMaterialTier MaterialTier;      // Leather, Iron, ReinforcedIron, DarkIron
    UPROPERTY(EditAnywhere) ERarity MaxRarity;               // Highest rarity this item can roll
    UPROPERTY(EditAnywhere) int32 RequiredLevel;             // Minimum level to equip
    UPROPERTY(EditAnywhere) float BaseArmorValue;            // For armor pieces
    UPROPERTY(EditAnywhere) float BaseDamage;                // For weapons
    UPROPERTY(EditAnywhere) float AttackSpeed;               // For weapons (multiplier)
    UPROPERTY(EditAnywhere) float StaminaCostLight;          // For weapons
    UPROPERTY(EditAnywhere) float StaminaCostHeavy;          // For weapons
    UPROPERTY(EditAnywhere) int32 MaxStackSize;              // 1 for gear, 20/99 for consumables/materials
    UPROPERTY(EditAnywhere) int32 SellValue;                 // Gold when sold to vendor
    UPROPERTY(EditAnywhere) bool bIsQuestItem;               // Cannot be dropped or sold
    UPROPERTY(EditAnywhere) TSoftObjectPtr<UTexture2D> Icon; // Inventory icon
    UPROPERTY(EditAnywhere) TSoftObjectPtr<UStaticMesh> WorldMesh; // Mesh when dropped in world
    UPROPERTY(EditAnywhere) TSoftObjectPtr<USkeletalMesh> EquippedMesh; // Mesh when worn/held
    UPROPERTY(EditAnywhere) FText SpecialEffectText;         // Legendary effect description
};
```

**FInventoryItem** (instance of an item in a player's inventory -- the actual rolled item):

```cpp
USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere) FName ItemID;                    // References FItemDefinition row
    UPROPERTY(EditAnywhere) int32 Quantity;                  // Stack count (1 for gear)
    UPROPERTY(EditAnywhere) ERarity Rarity;                  // Rolled rarity
    UPROPERTY(EditAnywhere) TMap<EStat, float> StatRolls;    // Rolled stats: {Might: 4, Fortitude: 2}
    UPROPERTY(EditAnywhere) int32 UpgradeCount;              // 0-3
    UPROPERTY(EditAnywhere) FName EnchantmentID;             // "None" until enchanting is implemented
    UPROPERTY(EditAnywhere) FGuid UniqueID;                  // Unique instance ID for save/load
};
```

**Enums:**

```cpp
UENUM(BlueprintType)
enum class EStat : uint8
{
    Might,
    Fortitude,
    Haste,
    Agility,
    Sorcery
};

UENUM(BlueprintType)
enum class ERarity : uint8
{
    Common,      // White
    Uncommon,    // Green
    Rare,        // Blue
    Epic,        // Purple
    Legendary    // Gold
};

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    Weapon,
    Armor,
    Consumable,
    Material,
    Artifact
};

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
    None,
    Head,
    Body,
    Gloves,
    Legs,
    Boots,
    MainHand,
    OffHand,
    Back
};

UENUM(BlueprintType)
enum class EMaterialTier : uint8
{
    Leather,
    Iron,
    ReinforcedIron,
    DarkIron
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Sword,
    Axe,
    Mace,
    Dagger,
    Shield,
    Torch,
    Greatsword,
    Glaive,
    Spear,
    Bow
};
```

### Components

**UInventoryComponent** (attached to PlayerCharacter):

```
Properties:
  - InventorySlots: TArray<FInventoryItem> (max size 30, expandable to 60)
  - MaxSlots: int32 (default 30)
  - Gold: int32

Functions:
  - AddItem(FInventoryItem Item) -> bool
      Returns false if inventory is full. Handles stacking automatically.
  - RemoveItem(int32 SlotIndex, int32 Quantity) -> bool
  - GetItem(int32 SlotIndex) -> FInventoryItem
  - FindItemByID(FName ItemID) -> int32 (slot index, -1 if not found)
  - SortInventory()
      Sorts by category, rarity, name.
  - GetFreeSlotCount() -> int32
  - IsInventoryFull() -> bool
  - AddGold(int32 Amount)
  - RemoveGold(int32 Amount) -> bool
      Returns false if insufficient gold.

Events/Delegates:
  - OnInventoryChanged (broadcast when any slot changes -- UI binds to this)
  - OnGoldChanged
```

**UEquipmentComponent** (attached to PlayerCharacter):

```
Properties:
  - EquippedItems: TMap<EEquipSlot, FInventoryItem>
      Keys: Head, Body, Gloves, Legs, Boots, MainHand, OffHand, Back
  - ActiveLoadout: ELoadout (enum: PrimaryLoadout, BackLoadout)

Functions:
  - EquipItem(FInventoryItem Item, EEquipSlot Slot) -> bool
      Validates slot compatibility. If an item is already in that slot,
      unequips it first (sends back to inventory). Returns false if
      inventory is full and unequip would fail.
  - UnequipItem(EEquipSlot Slot) -> bool
      Moves equipped item back to inventory. Returns false if inventory full.
  - SwapLoadout()
      Swaps MainHand+OffHand with Back. Triggers weapon swap animation.
      If Back slot is empty, does nothing.
  - GetEquippedItem(EEquipSlot Slot) -> FInventoryItem
  - GetTotalStatBonus(EStat Stat) -> float
      Sums the given stat across all equipped items. Used by the stat system.
  - GetTotalArmorValue() -> float
      Sums BaseArmorValue across all equipped armor pieces.
  - RecalculateVisuals()
      Updates character mesh attachments based on currently equipped items.

Events/Delegates:
  - OnEquipmentChanged(EEquipSlot Slot)
      Triggers mesh update, stat recalculation.
```

### DataTables Needed

| DataTable | Row Struct | Purpose | Estimated Rows (Vertical Slice) |
|---|---|---|---|
| DT_ItemDefinitions | FItemDefinition | All item base definitions | ~80 (weapons + armor + materials + consumables) |
| DT_LootTables | FLootTableRow | Enemy drop tables | ~30 (3-4 rows per enemy type) |
| DT_CraftingRecipes | FCraftingRecipe | Smithing recipes | ~25 |
| DT_StatRanges | FStatRangeRow | Min/max stat values per tier + rarity | ~16 (4 tiers x 4 rarities) |

**FCraftingRecipe struct:**

```cpp
USTRUCT(BlueprintType)
struct FCraftingRecipe : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere) FName RecipeID;
    UPROPERTY(EditAnywhere) FName OutputItemID;              // References DT_ItemDefinitions
    UPROPERTY(EditAnywhere) int32 OutputQuantity;
    UPROPERTY(EditAnywhere) TArray<FCraftingIngredient> Ingredients; // Array of {ItemID, Quantity}
    UPROPERTY(EditAnywhere) int32 RequiredSmithingLevel;
    UPROPERTY(EditAnywhere) int32 SmithingXPReward;
};

USTRUCT(BlueprintType)
struct FCraftingIngredient
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere) FName ItemID;
    UPROPERTY(EditAnywhere) int32 Quantity;
};
```

### UI (UMG Widgets)

**Vertical Slice UI Widgets:**

| Widget | Description | Input |
|---|---|---|
| WBP_HUD | Main gameplay HUD. HP/Stamina/Mana bars, spell slots (1-5) with cooldown overlays, item slots (6-0) with stack counts, minimap placeholder, quest tracker. | Always visible. |
| WBP_InventoryScreen | 6x5 grid of item slot widgets. Equipment paperdoll on the left (8 slots). Stats panel showing current totals. Sort button. | Toggle with I key or Tab. |
| WBP_ItemSlot | Single grid cell. Shows item icon, rarity border color, stack count. On hover: shows WBP_ItemTooltip. On click: context menu (Equip / Use / Drop / Deconstruct). | Click, hover. |
| WBP_ItemTooltip | Popup showing item name, rarity, stats, comparison arrows vs equipped. | Auto-show on hover. |
| WBP_LootPopup | Small notification in bottom-right: "[Icon] Iron Sword acquired" with rarity color. Fades after 3 seconds. Multiple pickups stack vertically. | Auto-show on loot pickup. |
| WBP_SmithingScreen | Crafting UI with recipe list, material preview, craft/upgrade/deconstruct tabs. | Open when interacting with forge. |
| WBP_SpellBook | List of learned spells. Drag (Phase 4) or click to assign to slots 1-5. | Open from inventory or separate key. |

**Vertical Slice Interaction: Click-to-Equip.** The player opens inventory, clicks an equippable item, and selects "Equip" from a context menu. The item goes to the appropriate slot. Full drag-and-drop is deferred to Phase 4.

**Loot Pickup in World:** Items dropped by enemies appear as glowing orbs on the ground (color = rarity). Player walks over them to auto-pickup (if inventory has space) or presses E to pickup (if auto-loot is disabled in settings). A WBP_LootPopup notification appears.

### Save/Load (Phase 4 -- NOT in Vertical Slice)

When implemented:
- Serialize InventoryComponent and EquipmentComponent to a JSON structure.
- Save to a local file using UE5's SaveGame system.
- Structure:

```json
{
  "inventory": [
    {
      "item_id": "ITM_Iron_Sword",
      "quantity": 1,
      "rarity": "Uncommon",
      "stat_rolls": {"Might": 3, "Agility": 2},
      "upgrade_count": 1,
      "enchantment_id": "None",
      "unique_id": "a1b2c3d4-..."
    }
  ],
  "equipment": {
    "Head": { "item_id": "ITM_Iron_Helm", ... },
    "MainHand": { "item_id": "ITM_Iron_Sword", ... },
    ...
  },
  "gold": 350,
  "smithing_level": 3,
  "smithing_xp": 245
}
```

For the vertical slice, inventory resets on game close. This is acceptable for a prototype.

---

## 10. WHAT TO BUILD FOR VERTICAL SLICE vs LATER

### Vertical Slice (Phase 1-2, Levels 1-10)

**MUST HAVE:**
- [ ] FItemDefinition DataTable with all T1-T4 items defined (~80 rows)
- [ ] FInventoryItem struct with stat rolls
- [ ] InventoryComponent: AddItem, RemoveItem, stack management
- [ ] EquipmentComponent: Equip, Unequip, stat calculation, loadout swap
- [ ] Random stat roll generation (Blueprint function: input tier + rarity, output FInventoryItem)
- [ ] Inventory grid UI (6x5, click-to-equip)
- [ ] Item tooltip with comparison arrows
- [ ] Loot pickup from world (walk-over auto-loot OR press E)
- [ ] Loot popup notification
- [ ] Weapon swap (X key) between Main+Off and Back
- [ ] Enemy loot tables (DataTable, roll on kill)
- [ ] Gathering nodes (Iron Ore, Timber, Leather drops, Herbs)
- [ ] 4 material tiers with distinct visuals (material instance swaps minimum)
- [ ] Basic smithing: smelt ore, craft Iron and Dark Iron gear, simple UI
- [ ] Sell items to vendors for gold
- [ ] Buy items from vendors with gold
- [ ] Consumable use from item slots (6-0 keys)
- [ ] Spell slot assignment (assign learned spells to keys 1-5)
- [ ] Auto-sort button
- [ ] HUD showing spell slots with cooldowns and item slots with stack counts

### Phase 4 (Post-Vertical Slice)

- [ ] Drag-and-drop inventory UI
- [ ] Search and filter in inventory
- [ ] Save/load inventory to JSON
- [ ] Inventory expansion (Traveler's Pack, 30 -> 60 slots)
- [ ] Item trading between players
- [ ] Auction house / marketplace
- [ ] Expanded material tiers (Steel, Mithril, etc. for levels 11-30+)
- [ ] More crafting recipes
- [ ] Alchemy expansion (mana potions, stamina potions, buff potions)
- [ ] Upgrade system at smithing station
- [ ] Deconstruct system at smithing station

### Phase 5 (Late Development)

- [ ] Enchanting system (learn, apply, overwrite)
- [ ] Legendary items with special effects
- [ ] Set bonuses (wearing 3+ pieces of a named set grants a bonus)
- [ ] Cosmetic transmog system (change appearance without changing stats)
- [ ] Item durability and repair (if desired -- currently NOT planned)

---

## APPENDIX A: COMPLETE ITEM LIST (Vertical Slice)

### Weapons

| ID | Name | Type | Tier | Req Level | Base Damage |
|---|---|---|---|---|---|
| WPN_Starter_Sword | Training Sword | Sword | Leather | 1 | 7 |
| WPN_Starter_Bow | Training Bow | Bow | Leather | 1 | 8-13 |
| WPN_Starter_Shield | Wooden Shield | Shield | Leather | 1 | 0 (block) |
| WPN_Iron_Sword | Iron Sword | Sword | Iron | 4 | 12 |
| WPN_Iron_Axe | Iron Axe | Axe | Iron | 4 | 16 |
| WPN_Iron_Mace | Iron Mace | Mace | Iron | 4 | 18 |
| WPN_Iron_Dagger | Iron Dagger | Dagger | Iron | 4 | 8 |
| WPN_Iron_Shield | Iron Shield | Shield | Iron | 4 | 0 (block) |
| WPN_Iron_Greatsword | Iron Greatsword | Greatsword | Iron | 4 | 28 |
| WPN_Iron_Glaive | Iron Glaive | Glaive | Iron | 4 | 20 |
| WPN_Iron_Spear | Iron Spear | Spear | Iron | 4 | 16 |
| WPN_Iron_Bow | Iron Bow | Bow | Iron | 4 | 14-22 |
| WPN_RIron_Sword | Reinforced Iron Sword | Sword | Reinforced Iron | 7 | 17 |
| WPN_RIron_Axe | Reinforced Iron Axe | Axe | Reinforced Iron | 7 | 22 |
| WPN_RIron_Mace | Reinforced Iron Mace | Mace | Reinforced Iron | 7 | 25 |
| WPN_RIron_Greatsword | Reinforced Iron Greatsword | Greatsword | Reinforced Iron | 7 | 39 |
| WPN_DkIron_Sword | Dark Iron Sword | Sword | Dark Iron | 10 | 22 |
| WPN_DkIron_Axe | Dark Iron Axe | Axe | Dark Iron | 10 | 29 |
| WPN_DkIron_Mace | Dark Iron Mace | Mace | Dark Iron | 10 | 32 |
| WPN_DkIron_Greatsword | Dark Iron Greatsword | Greatsword | Dark Iron | 10 | 50 |
| WPN_Nilvar_Dagger | Poisoned Nilvar Dagger | Dagger | Iron | 7 | 10 (+poison DOT) |
| WPN_HK_Shield | Hallowed Knight Shield | Shield | Reinforced Iron | 5 | 0 (enhanced block) |

### Armor (per slot -- multiply by 5 slots)

| Tier | Example ID (Helm) | Base Armor |
|---|---|---|
| Leather | ARM_Leather_Head | 3 |
| Iron | ARM_Iron_Head | 6 |
| Reinforced Iron | ARM_RIron_Head | 10 |
| Dark Iron | ARM_DkIron_Head | 15 |

Total armor items: 4 tiers x 5 slots = 20 base definitions. Each can roll any rarity.

### Materials

| ID | Name | Stack | Source |
|---|---|---|---|
| MAT_IronOre | Iron Ore | 99 | Gathering nodes, enemy drops |
| MAT_IronIngot | Iron Ingot | 99 | Smelted from 5 Iron Ore |
| MAT_DarkOreFragment | Dark Ore Fragment | 99 | Dungeon only |
| MAT_Leather | Leather | 99 | Boar, wolf drops |
| MAT_Timber | Timber | 99 | Harvestable trees |
| MAT_Herb | Herbs | 99 | Riverbank plants |
| MAT_ChitinShard | Chitin Shard | 99 | Dred Minion drops, beetles |
| MAT_SpiderSilk | Spider Silk | 99 | Broodmother drop |
| MAT_WolfFang | Wolf Fang | 99 | Wolf drops |
| MAT_DarkHerbs | Dark Herbs | 99 | Nilvar drops, dungeon |
| MAT_Essence | Essence | 999 | Deconstruction |

### Consumables

| ID | Name | Stack | Effect |
|---|---|---|---|
| CON_HealthPotion | Health Potion | 20 | Restores 40 HP instantly |
| CON_RawMeat | Raw Meat | 20 | Restores 10 HP over 5 seconds |
| CON_Arrows | Iron Arrows | 99 | Ammo for bows |

---

## APPENDIX B: BLUEPRINT IMPLEMENTATION ORDER

For a solo dev, build these systems in this order:

1. **FItemDefinition DataTable** -- Define all items as data first. No code needed, just the table.
2. **FInventoryItem struct** -- Create the struct in Blueprint (or C++ if preferred for the TMap).
3. **InventoryComponent** -- AddItem/RemoveItem/stacking. Test by spawning items via console command.
4. **WBP_InventoryScreen** -- Basic grid UI. Bind to InventoryComponent. Test: open inventory, see items.
5. **EquipmentComponent** -- Equip/Unequip. Connect to InventoryComponent (equipping removes from inventory, unequipping adds back).
6. **WBP_ItemTooltip** -- Stat display and comparison arrows.
7. **Loot pickup** -- World item actor (BP_WorldItem) with mesh + collision. On overlap, calls InventoryComponent.AddItem.
8. **Enemy loot tables** -- On enemy death, roll against DataTable, spawn BP_WorldItem(s).
9. **Weapon swap** -- EquipmentComponent.SwapLoadout bound to X key. Swap animation.
10. **HUD integration** -- Spell slots, item slots, cooldown overlays.
11. **Smithing UI** -- Recipe list, material check, craft function, stat roll generation.
12. **Vendor UI** -- Buy/sell with gold.
13. **Gathering nodes** -- Interactable actors that give materials on E press, then despawn and respawn on timer.

Estimated implementation time for a solo Blueprint dev: 3-4 weeks for the vertical slice feature set.
