# THE REVEN -- Crafting & Materials System
**Created: 2026-03-25**
**Status: DESIGN DOCUMENT -- Pending human approval**
**Scope: Full system design. Vertical slice implementation marked explicitly.**

---

## 1. MATERIAL TYPES

### Vertical Slice Materials (5 types)

| Material | Type | Source | Stack Limit |
|---|---|---|---|
| Iron Ore | Ore | Mining nodes (river cliffs, caves, mountains) | 99 |
| Animal Hide | Leather | Wildlife kills (boars, wolves) | 99 |
| Timber | Wood | Marked harvestable trees (not all trees) | 99 |
| Essence | Magical | Deconstructing enchanted gear, Dred drops (rare) | 99 |
| Gold | Currency | Enemy drops, quest rewards, selling items | No cap |

### Intermediate Materials (crafted from raw)

| Material | Recipe | Used For |
|---|---|---|
| Iron Ingot | 5 Iron Ore | Weapons, armor |
| Cured Leather | 3 Animal Hide | Armor, grips |
| Plank | 3 Timber | Shields, bows, arrows, structural |

### Dungeon-Exclusive Materials (Vertical Slice)

| Material | Source | Used For |
|---|---|---|
| Dark Ore Fragment | Dark Forest dungeon (altar, Broodmother, Risen Knight) | Dark Iron gear |
| Chitin Shard | Corrupted Beetles | Reinforced armor variants |
| Spider Silk | Corrupted Broodmother | Lightweight armor variants |
| Dark Herbs | Nilvar Scouts, Nilvar Shamans | Alchemy |

Dark Iron gear requires 3 Dark Ore Fragments per weapon or 5 per armor set, encouraging dungeon repeat runs as established in the vertical slice document.

### Future Phase Materials

| Phase | Materials | Source Region |
|---|---|---|
| Phase 3 (Ashpeak) | Steel Ore, Hardite | Ashpeak mines, mountain caves |
| Phase 4 (Arcanem) | Arcane Dust, Starlight Crystal | Arcanem questline, astral creatures |
| Phase 5 (Enul/Barrenlands) | Runemetal, Ironbark, Voidglass | Enul groves, Barrenlands deposits |
| Phase 6 (Endgame) | Celestium, Thuun Alloy | Thuunrow trade, endgame dungeons |

These follow the lore: Runemetal from ancient Inil forges, Ironbark from Enul druidcraft, Voidglass from Barrenlands corruption, Celestium from Divine-touched ore.

---

## 2. GATHERING

### Design Principle

Gathering is simple. Walk up, press interact, get materials. No minigame, no timing mechanic, no tool durability. The player already has combat to master -- gathering is a resource loop, not a skill challenge.

### Gathering Methods

**Mining (Ore)**
- Ore veins appear as glowing rock formations on cliff faces, cave walls, and mountain bases.
- Interaction: walk up, press E, short 2-second animation, receive 2-4 Iron Ore per node.
- Nodes respawn after 5 minutes (real time). In dungeons, nodes respawn per run.
- The player receives a free pickaxe during the "Iron and Fire" quest from Smith Gaelen. It occupies no inventory slot -- it is a permanent tool unlock.
- Visual: ore veins shimmer slightly so they are visible without a UI marker. Close range shows an interact prompt.

**Skinning (Leather)**
- Any wildlife kill (boar, wolf) can be skinned by interacting with the corpse.
- Yields 1-2 Animal Hide per corpse.
- Corpses despawn after 60 seconds. Unskinned corpses are not recoverable.

**Woodcutting (Timber)**
- Specific trees marked with a subtle axe-blaze texture (a carved notch in the bark). Not every tree -- approximately 1 in 10 trees in forested areas.
- Interaction: press E, 2-second animation, receive 2-3 Timber.
- Trees respawn after 5 minutes.
- Harvestable trees are slightly larger and more distinct than background trees so players learn to spot them.

**Herb Gathering (Alchemy)**
- Small flowering plants near riverbanks and in clearings. Glow faintly in low light.
- Press E, instant pickup, receive 1 herb.
- Two herb types in vertical slice: Riverbloom (blue flower, near water), Thornleaf (green shrub, near rocks).
- Used for basic potions only.

### Gathering XP

- Every gather action grants 5 crafting XP (applies to smithing level or alchemy level depending on material type).
- This is intentionally small. Crafting is the primary XP source for crafting levels, not gathering.

---

## 3. SMITHING

### Overview

Smithing creates and upgrades weapons and armor. A smithing station is required. The system uses recipes, materials, and random stat rolls to produce gear with variety.

### Smithing Stations

| Location | Available In | Notes |
|---|---|---|
| Inildon -- Inilvan Command Forge | Vertical Slice | Smithmaster Toren. Sells basic weapons. Does not teach smithing -- that happens at River's Bend. |
| River's Bend -- Forge | Vertical Slice | Smith Gaelen. Teaches smithing via "Iron and Fire" quest. Full crafting access after quest. |
| Ashpeak | Phase 3 | Best smithing station. Higher-tier recipes. Smithing-focused city. |
| Arcanem | Phase 4 | Enchanting integration (enchant while smithing). |

### Smithing Level (1-100)

| Level Range | Capability |
|---|---|
| 1-10 | Iron-tier gear. Upgrade to +1. |
| 11-20 | Reinforced Iron-tier. Upgrade to +2. |
| 21-30 | Dark Iron-tier. Upgrade to +3. |
| 31-50 | Steel-tier (Phase 3). Upgrade to +4. |
| 51-70 | Runemetal-tier (Phase 5). Upgrade to +5. |
| 71-100 | Celestium-tier (Phase 6). Legendary crafting. |

**XP Sources for Smithing Level:**

| Action | Smithing XP |
|---|---|
| Craft a weapon | 25 |
| Craft an armor piece | 20 |
| Upgrade an item (+1, +2, etc.) | 15 |
| Deconstruct an item | 10 |
| Gather ore/hide/timber | 5 |

The vertical slice caps practical smithing around level 20-25 based on available materials and recipes.

### Crafting: New Gear

**Process:**
1. Interact with a smithing station. The crafting UI opens.
2. Browse available recipes (filtered by smithing level).
3. Select a recipe. UI shows required materials and current inventory.
4. If materials are sufficient, press "Craft." A 2-second animation plays.
5. The item is created with random stat rolls within the tier range.

**Stat Rolls:**
- Each crafted item rolls 2-3 stats from the 5 core stats (Might, Fortitude, Haste, Agility, Sorcery).
- Roll ranges are determined by the item tier (see Gear Tiers below).
- Higher smithing level improves the floor of the roll range. At smithing level 1, an Iron Helm rolls 2-5 per stat. At smithing level 15, it rolls 3-5. The ceiling stays the same -- skill raises the minimum, not the maximum.

### Crafting: Upgrade Existing Gear

**Process:**
1. Interact with a smithing station. Select "Upgrade" tab.
2. Place an existing item in the upgrade slot.
3. Spend materials to increase the item's base stats.
4. Each upgrade level (+1, +2, +3) adds a flat bonus to all stats on the item.

**Upgrade Costs:**

| Upgrade | Material Cost | Stat Bonus | Required Smithing Level |
|---|---|---|---|
| +1 | 1 Ingot + 50 Gold | +1 to all stats on item | 5 |
| +2 | 2 Ingots + 150 Gold | +2 to all stats on item | 15 |
| +3 | 3 Ingots + 300 Gold | +3 to all stats on item | 25 |
| +4 | 4 Ingots + 500 Gold (Phase 3) | +4 to all stats on item | 40 |
| +5 | 5 Ingots + 1000 Gold (Phase 5) | +5 to all stats on item | 60 |

Upgrades are cumulative. A +3 item has +6 total to each stat (+1 + +2 + +3). The ingot type must match or exceed the item tier (Iron Ingot for Iron-tier, Dark Ore Fragment for Dark Iron-tier, etc.).

### Vertical Slice Recipes

**Weapons (Iron-tier):**

| Recipe | Materials | Smithing Req | Output |
|---|---|---|---|
| Iron Sword | 2 Iron Ingots, 1 Cured Leather | 1 | Iron Sword (1H, Medium speed, Medium damage) |
| Iron Axe | 2 Iron Ingots, 1 Plank | 1 | Iron Axe (1H, Slow, High damage, Cleave) |
| Iron Mace | 3 Iron Ingots | 1 | Iron Mace (1H, Slow, High damage, Stagger) |
| Iron Shield | 2 Iron Ingots, 1 Plank | 1 | Iron Shield (Off Hand, Block) |
| Iron Greatsword | 4 Iron Ingots, 1 Cured Leather | 5 | Iron Greatsword (2H, Very Slow, Very High damage) |
| Wooden Bow | 2 Planks, 1 Cured Leather | 1 | Wooden Bow (2H, Ranged) |
| Iron Arrows (x20) | 1 Iron Ingot, 1 Plank | 1 | 20 Iron Arrows |

**Armor (Iron-tier, one recipe per slot):**

| Recipe | Materials | Smithing Req | Output |
|---|---|---|---|
| Iron Helm | 2 Iron Ingots | 1 | Iron Helm (Head) |
| Iron Cuirass | 3 Iron Ingots, 1 Cured Leather | 1 | Iron Cuirass (Body) |
| Iron Gauntlets | 1 Iron Ingot, 1 Cured Leather | 1 | Iron Gauntlets (Gloves) |
| Iron Greaves | 2 Iron Ingots, 1 Cured Leather | 1 | Iron Greaves (Legs) |
| Iron Boots | 2 Iron Ingots | 1 | Iron Boots (Boots) |

**Dark Iron-tier (dungeon materials):**

| Recipe | Materials | Smithing Req | Output |
|---|---|---|---|
| Dark Iron Weapon | 3 Dark Ore Fragments, 2 Iron Ingots | 15 | Dark Iron variant of any weapon type |
| Dark Iron Armor (per slot) | 1 Dark Ore Fragment, 2 Iron Ingots, 1 Chitin Shard | 15 | Dark Iron variant of any armor slot |

**Recipe Unlocks:**
- Iron-tier: Learned automatically when smithing unlocks (after "Iron and Fire" quest).
- Dark Iron-tier: Learned from Smith Gaelen after the player brings their first Dark Ore Fragment. Gaelen: *"Dark ore... from the forest? I have heard of this metal. Let me study it."* -- Returns the fragment and teaches Dark Iron recipes.

### Gear Rarity and Visual Assets

| Rarity | Color | Stat Range | Visual Difference | Source |
|---|---|---|---|---|
| Common (White) | White | Low (1-5 per stat) | Plain materials, no decoration. Leather is brown, iron is dull grey. | Crafting, basic drops |
| Uncommon (Green) | Green | Mid (4-7 per stat) | Polished metal, leather underlayer visible, minor engravings. | Crafting with higher smithing, upgraded gear |
| Rare (Blue) | Blue | High (6-10 per stat) | Distinct silhouette. Dark Iron has black tint with green veins. Celestial engravings for Inil-style. | Dungeon boss drops, Dark Iron crafting |
| Epic (Purple) | Purple | Very High (8-13 per stat) | Unique model with glowing elements (runes, energy lines). Faction-specific design. | Phase 4+ quest rewards, endgame crafting |
| Legendary (Gold) | Gold | Exceptional (12-18 per stat) | Completely unique model. Particle effects. Named items with lore. | Phase 6 endgame only |

**Visual asset guidance for the solo dev:** Common and Uncommon share the same base mesh with different material instances (texture swap: plain vs. polished). Rare uses a modified mesh (added geometry: vine tendrils for Dark Iron, engraving normal maps for Inil-style). Epic and Legendary are unique meshes built when those tiers become relevant. For the vertical slice, only Common, Uncommon, and Rare are needed -- that is 2 base meshes per slot (plain + Dark Iron variant) with material instance swaps for Common vs. Uncommon.

---

## 4. DECONSTRUCTING

### Overview

Deconstructing breaks gear into raw materials and essence. It is performed at any smithing station. This is the primary source of essence in the early game and the main way players recycle unwanted gear.

### Process

1. At a smithing station, select the "Deconstruct" tab.
2. Place any weapon or armor piece in the deconstruct slot.
3. Press "Deconstruct." A 1-second animation plays. The item is destroyed.
4. Materials and essence are added to inventory.

### Returns

| Item Rarity | Materials Returned | Essence Returned |
|---|---|---|
| Common (White) | ~50% of crafting cost (rounded down, minimum 1) | 1 Essence |
| Uncommon (Green) | ~50% of crafting cost | 2 Essence |
| Rare (Blue) | ~50% of crafting cost | 5 Essence |
| Epic (Purple) | ~50% of crafting cost | 10 Essence |
| Legendary (Gold) | ~50% of crafting cost | 25 Essence |

**Enchanted items** return double the essence listed above. This creates a meaningful decision: use the enchanted item, or destroy it for essence (and in Phase 4+, learn the enchantment).

**Examples:**
- Deconstructing an Iron Sword (cost: 2 Iron Ingots, 1 Cured Leather) returns 1 Iron Ingot + 1 Essence.
- Deconstructing a Rare Dark Iron Helm (enchanted) returns 1 Dark Ore Fragment, 1 Iron Ingot + 10 Essence.

### Smithing XP from Deconstructing

Deconstructing grants 10 smithing XP per item, regardless of rarity. This is less than crafting (25 XP) but provides a way to level smithing while clearing inventory.

---

## 5. ENCHANTING (Phase 4 -- NOT in Vertical Slice)

### Overview

Skyrim-style enchanting: find enchanted items in the world, deconstruct them to learn the enchantment permanently, then apply known enchantments to gear at enchanting tables. Cost: essence + gold.

### Learning Enchantments

1. **Find an enchanted item** -- drops from dungeon bosses, quest rewards, rare world drops. Enchanted items glow faintly and have the enchantment name in their tooltip.
2. **Deconstruct the enchanted item** at an enchanting table (not a smithing station). This destroys the item but permanently teaches the enchantment to the player.
3. **The enchantment is now known.** It appears in the player's enchantment list forever. It cannot be unlearned.

Alternative: some enchantments are learned as quest rewards (delivered as "Enchantment Tomes" -- consume to learn, no deconstruction needed).

### Applying Enchantments

1. Interact with an enchanting table.
2. Place a weapon or armor piece in the enchanting slot.
3. Select a known enchantment from the list.
4. Pay the cost (essence + gold).
5. The enchantment is applied. The item gains the enchantment effect and a faint visual glow matching the enchantment type.

An item can only be enchanted once. Adding a new enchantment overwrites the previous one (until higher enchanting levels unlock multiple slots).

### Enchanting Stations

| Location | Available In |
|---|---|
| Arcanem | Phase 4 (first access, tied to Arcanem questline) |
| Inildon -- Inilium | Phase 4 (unlocked after Arcanem quest progress) |
| Ashpeak | Phase 4 |
| Crossroads | Phase 5 |

### Enchanting Level (1-100)

Enchanting level increases by applying enchantments (20 XP per enchant) and deconstructing enchanted items at an enchanting table (15 XP per item).

| Level Range | Capability |
|---|---|
| 1-25 | One enchantment per item. Base power enchantments. |
| 26-50 | Two enchantments per item. Base power. |
| 51-75 | Two enchantments. Enhanced power (roughly 1.5x base values). |
| 76-100 | Two enchantments. Maximum power (roughly 2x base values). |

### Enchanting Costs

| Enchantment Power | Essence Cost | Gold Cost |
|---|---|---|
| Base (Level 1-50) | 5 Essence | 200 Gold |
| Enhanced (Level 51-75) | 10 Essence | 500 Gold |
| Maximum (Level 76-100) | 20 Essence | 1000 Gold |

---

## 6. PLAYSTYLE-CHANGING ENCHANTMENTS

### Design Philosophy

Every enchantment must open a build identity, not just make numbers bigger. A player reading an enchantment description should immediately think "I could build around that." The enchantments below are designed to synergize with the combat triangle (melee, ranged, magic) and the 5 core stats to create distinct playstyles.

### Enchantment List

**Offensive Enchantments:**

| Name | Effect | Build Identity | Applies To |
|---|---|---|---|
| Spell Echo | 15% chance to auto-cast the last spell again (free, no mana cost, no cooldown) | Spell spam caster. Stack Sorcery, cast fast, let RNG double your output. | Gloves, Head |
| Frost Bite | Melee attacks have 10% chance to apply 3-second slow | Crowd control melee. Pair with Haste to kite slowed enemies. Turns a brawler into a controller. | Weapons (melee) |
| Ember Core | Fire spells leave burning ground for 3 seconds (5% weapon damage/sec) | Area denial mage. Cast Firebolt at chokepoints. Punish enemies for standing still. Forces repositioning in PvP. | Weapons (any), Head |
| Blood Resonance | Blood spells heal for 20% of damage dealt | Sustain-through-offense battlemage. No need for Mend -- your damage IS your healing. Rewards aggression. | Body, Legs |

**Defensive Enchantments:**

| Name | Effect | Build Identity | Applies To |
|---|---|---|---|
| Thorn Mantle | Reflect 10% of melee damage received back to the attacker | Tank build. Stack Fortitude, let enemies kill themselves hitting you. Punishes Dred swarms. | Body, Head |
| Windrunner | Dodge costs 30% less stamina | Agility build. Dodge constantly, never get hit. Pairs with Agility stat for longer i-frames and faster recovery. | Boots, Legs |
| Soul Tether | Killing an enemy extends all active buff durations by 3 seconds | Chain-killer. Keep food buffs, potion effects, and spell buffs rolling by never stopping. Rewards aggressive pacing. | Gloves, Boots |

**Hybrid Enchantments:**

| Name | Effect | Build Identity | Applies To |
|---|---|---|---|
| Mana Siphon | Melee kills restore 5% max mana | Battle mage. Swing sword to fuel spells. Eliminates the melee-or-magic choice -- you do both. | Weapons (melee) |
| Leech Strike | 3% of melee damage dealt is returned as health | Sustain melee. Less healing than Blood Resonance but works with any weapon, no spells needed. Pure fighters. | Weapons (melee) |
| Arcane Momentum | Casting a spell increases movement speed by 15% for 4 seconds | Mobile caster. Cast and reposition. Kite melee enemies. Hit-and-run magic build. | Boots, Gloves |
| Storm Conduit | Lightning damage from any source chains to 1 nearby enemy at 30% power | AoE caster. Every single-target lightning spell becomes AoE. Transforms spell selection priorities. | Head, Weapons |

**Utility Enchantments:**

| Name | Effect | Build Identity | Applies To |
|---|---|---|---|
| Gatherer's Touch | Gathering yields 50% more materials | Crafter identity. Not a combat build -- a lifestyle build. Players who want to be the group's smith. | Gloves |
| Night Eye | See clearly in darkness. No torch needed. | Explorer. Free up the off-hand slot (no torch), use a shield or dual-wield instead in dark dungeons. | Head |

### Enchantment Scaling with Enchanting Level

Using Mana Siphon as an example:

| Enchanting Level | Effect |
|---|---|
| 1-25 | Melee kills restore 5% max mana |
| 26-50 | Melee kills restore 5% max mana (same power, but can add a second enchantment) |
| 51-75 | Melee kills restore 8% max mana |
| 76-100 | Melee kills restore 10% max mana |

All enchantments follow this pattern: base value at low levels, ~1.5x at enhanced, ~2x at maximum.

---

## 7. ALCHEMY (Simplified)

### Design Principle

Alchemy is the simplest crafting system. Combine herbs at an alchemy table, get potions. No recipe discovery, no experimentation, no failures. The player sees a recipe list, has the herbs or does not, and crafts.

### Alchemy Stations

| Station | Location | Available In |
|---|---|---|
| Alchemy Table | River's Bend Infirmary (Healer Iona) | Vertical Slice |
| Campfire | Various world locations, dungeon entrance | Vertical Slice |
| Advanced Alchemy Lab | Arcanem | Phase 4 |

### Alchemy Level (1-100)

| Level Range | Capability |
|---|---|
| 1-10 | Basic potions (Health I, Stamina I) |
| 11-25 | Improved potions (Health II, Stamina II, Mana I) |
| 26-50 | Advanced potions (Mana II, resistance potions) |
| 51-100 | Master potions (Health III, combination effects, long-duration buffs) |

**XP Sources:**

| Action | Alchemy XP |
|---|---|
| Craft a potion | 15 |
| Gather an herb | 5 |

### Vertical Slice Recipes

**Potions (Alchemy Table):**

| Recipe | Ingredients | Alchemy Req | Effect |
|---|---|---|---|
| Health Potion I | 3 Riverbloom | 1 | Restore 30% max HP over 5 seconds |
| Stamina Potion I | 3 Thornleaf | 1 | Restore 30% max Stamina instantly |

**Food (Campfire):**

| Recipe | Ingredients | Alchemy Req | Effect | Duration |
|---|---|---|---|---|
| Roasted Meat | 1 Animal Hide (represents raw meat from the same kill) | 1 | +5% max HP | 5 minutes |
| Herb Bread | 1 Riverbloom + 1 Thornleaf | 1 | +5% stamina regen | 5 minutes |

Food buffs are minor and short. They do not stack with each other -- only one food buff active at a time. They reward players who engage with the system without punishing those who ignore it.

### Future Phase Recipes

| Phase | Recipes Added |
|---|---|
| Phase 3 | Mana Potion I, Stamina Potion II |
| Phase 4 | Health Potion II, Mana Potion II, Fire Resistance Potion, Frost Resistance Potion |
| Phase 5 | Health Potion III, combination potions (health + stamina), poison coating for weapons |
| Phase 6 | Master potions, long-duration buffs, elixirs (permanent minor stat boosts, expensive) |

---

## 8. UE5 IMPLEMENTATION

### Architecture

**CraftingComponent (UActorComponent)**
- Attached to the player character.
- Manages: smithing level, alchemy level, enchanting level, known recipes, known enchantments.
- Handles all crafting logic: validate materials, roll stats, produce output.

### Core Data Structures

```cpp
// Material enum -- simple, extensible
UENUM(BlueprintType)
enum class EMaterial : uint8
{
    IronOre,
    AnimalHide,
    Timber,
    Essence,
    Gold,
    DarkOreFragment,
    ChitinShard,
    SpiderSilk,
    Riverbloom,
    Thornleaf,
    IronIngot,       // intermediate
    CuredLeather,    // intermediate
    Plank,           // intermediate
    // Phase 3+
    SteelOre,
    ArcaneDust,
    Runemetal,
    Ironbark,
    Voidglass,
    Celestium
};

// Crafting recipe struct
USTRUCT(BlueprintType)
struct FCraftingRecipe
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName RecipeID;

    UPROPERTY(EditAnywhere)
    TMap<EMaterial, int32> RequiredMaterials;

    UPROPERTY(EditAnywhere)
    FName OutputItemID;

    UPROPERTY(EditAnywhere)
    int32 RequiredSmithingLevel;

    UPROPERTY(EditAnywhere)
    ECraftingCategory Category; // Weapon, Armor, Potion, Food, Arrows, Intermediate
};

// Crafting category enum
UENUM(BlueprintType)
enum class ECraftingCategory : uint8
{
    Weapon,
    Armor,
    Potion,
    Food,
    Arrows,
    Intermediate  // ingots, planks, cured leather
};
```

### Data Tables

**DT_CraftingRecipes** (DataTable using FCraftingRecipe)
- One row per recipe.
- For the vertical slice: approximately 20 rows (7 weapons + 5 armor + 2 Dark Iron variants + 2 potions + 2 food + 3 intermediates).

**DT_DeconstructReturns** (DataTable)
- Maps item IDs to material returns and essence yields.
- Can be auto-calculated from crafting cost (50% rounded down) or manually overridden.

**DT_Enchantments** (DataTable, Phase 4)
- Enchantment ID, display name, description, effect type (enum), effect value, allowed slot types, essence cost, gold cost, required enchanting level.

### Crafting UI

**Widget: WBP_CraftingMenu**
- Opens when the player interacts with a smithing station, alchemy table, or enchanting table.
- Tabs: Craft | Upgrade | Deconstruct (smithing station), Brew | Cook (alchemy/campfire), Enchant | Learn (enchanting table).

**Craft Tab:**
- Left panel: scrollable list of available recipes, filtered by category (All, Weapons, Armor, Arrows, Materials).
- Recipes the player cannot craft (missing materials or insufficient level) are grayed out and sorted to the bottom.
- Right panel: selected recipe detail -- item name, required materials (with current/needed counts), smithing level requirement, and a preview of the output item.
- "Craft" button. Disabled if requirements are not met.

**Upgrade Tab:**
- Single slot for an item from inventory.
- Shows current upgrade level and cost for the next upgrade.
- "Upgrade" button.

**Deconstruct Tab:**
- Single slot for an item from inventory.
- Shows what materials and essence will be returned.
- "Deconstruct" button with a confirmation prompt (prevents accidental destruction).

### Material Inventory

- Materials are stored in a separate section of the player's inventory (not mixed with gear).
- Materials auto-stack. No manual stacking required.
- The material section is a simple list: icon, name, quantity. No grid -- just a vertical list.
- Materials cannot be equipped, only used, sold, or traded.

### Stat Roll Implementation

```cpp
// Simplified stat roll logic
void UCraftingComponent::RollItemStats(FItemData& OutItem, const FCraftingRecipe& Recipe)
{
    // Determine number of stats to roll (2-3)
    int32 NumStats = FMath::RandRange(2, 3);

    // Get tier stat range
    int32 MinStat = GetTierMinStat(Recipe.Tier, SmithingLevel);
    int32 MaxStat = GetTierMaxStat(Recipe.Tier);

    // Smithing level raises the floor
    // At level 1: MinStat = TierBase. At level 20: MinStat = TierBase + 2.
    MinStat += FMath::Clamp(SmithingLevel / 10, 0, MaxStat - MinStat);

    // Roll stats
    TArray<ECoreStat> AvailableStats = { Might, Fortitude, Haste, Agility, Sorcery };
    for (int32 i = 0; i < NumStats; i++)
    {
        int32 StatIndex = FMath::RandRange(0, AvailableStats.Num() - 1);
        ECoreStat Stat = AvailableStats[StatIndex];
        int32 Value = FMath::RandRange(MinStat, MaxStat);
        OutItem.Stats.Add(Stat, Value);
        AvailableStats.RemoveAt(StatIndex); // no duplicate stats
    }

    // Primary stat affinity: armor slot's primary stat gets +1 bonus if rolled
    if (OutItem.Stats.Contains(GetPrimaryStatForSlot(OutItem.ArmorSlot)))
    {
        OutItem.Stats[GetPrimaryStatForSlot(OutItem.ArmorSlot)] += 1;
    }
}
```

### Gathering Node Implementation

**BP_GatherNode** (Blueprint Actor)
- Variables: EMaterial MaterialType, int32 MinYield, int32 MaxYield, float RespawnTime.
- On interact: play animation montage on player, add materials to inventory, disable mesh + collision, start respawn timer.
- On respawn: re-enable mesh + collision.
- Subclasses: BP_OreNode, BP_HarvestTree, BP_HerbNode (each with appropriate mesh and particle effects).

### Blueprint-First Approach

For a solo dev using Blueprint-first (per CURRENT_STATE.md):
- Implement CraftingComponent, material inventory, and recipes entirely in Blueprint for the vertical slice.
- Use DataTable assets for recipes (editable in-editor, no code changes needed to add recipes).
- Move to C++ only if Blueprint performance becomes an issue (unlikely for crafting -- it is not frame-critical code).
- The C++ structs above serve as reference for the data model. In Blueprint, these become custom structs defined in the editor.

---

## 9. VERTICAL SLICE SCOPE

### Build NOW (Phase 1-2)

| Feature | Details | Priority |
|---|---|---|
| 5 material types | Iron Ore, Animal Hide, Timber, Essence, Gold | Must have |
| 4 dungeon materials | Dark Ore Fragment, Chitin Shard, Spider Silk, Dark Herbs | Must have |
| 3 intermediate materials | Iron Ingot, Cured Leather, Plank | Must have |
| 2 herb types | Riverbloom, Thornleaf | Must have |
| Gathering nodes | BP_OreNode, BP_HarvestTree, BP_HerbNode placed in world | Must have |
| Skinning from corpses | Interact with dead wildlife for Animal Hide | Must have |
| 1 smithing station | River's Bend Forge (Smith Gaelen) | Must have |
| 1 non-teaching forge | Inildon Forge (Smithmaster Toren) -- sells weapons, unlocks for crafting after Gaelen teaches | Must have |
| 7 weapon recipes (Iron) | Sword, Axe, Mace, Shield, Greatsword, Bow, Arrows | Must have |
| 5 armor recipes (Iron) | Helm, Cuirass, Gauntlets, Greaves, Boots | Must have |
| 2 Dark Iron recipes | Dark Iron Weapon (any type), Dark Iron Armor (any slot) | Must have |
| 3 intermediate recipes | Iron Ingot, Cured Leather, Plank | Must have |
| Upgrade system (+1/+2/+3) | Spend materials to increase existing item stats | Must have |
| Deconstruct system | Break gear into ~50% materials + essence | Must have |
| 2 potion recipes | Health Potion I, Stamina Potion I | Must have |
| 2 food recipes | Roasted Meat, Herb Bread | Nice to have |
| Crafting UI | Craft, Upgrade, Deconstruct tabs | Must have |
| Material inventory section | Auto-stacking, separate from gear | Must have |
| Smithing level tracking | Level 1-100, XP from crafting/upgrading/deconstructing | Must have |
| Alchemy level tracking | Level 1-100, XP from potions and herb gathering | Nice to have |
| Random stat rolls | 2-3 stats per crafted item, tier-appropriate ranges | Must have |

### Build LATER

| Feature | Phase | Dependencies |
|---|---|---|
| Enchanting system (learn, apply, enchanting level) | Phase 4 | Arcanem questline, enchanting tables, essence economy |
| Enchantment list (all 14+ enchantments) | Phase 4 | Enchanting system |
| Advanced materials (Steel, Arcane Dust, etc.) | Phase 3-5 | New zones (Ashpeak, Arcanem, Enul, Barrenlands) |
| Advanced alchemy (Mana potions, resistance potions) | Phase 3-4 | New herbs in new zones |
| Weapon poison coatings | Phase 5 | Advanced alchemy |
| Master potions / elixirs | Phase 6 | Endgame materials |
| Legendary crafting | Phase 6 | Celestium, Thuun Alloy, max smithing |
| Auction house / player trading | Phase 6 | Multiplayer infrastructure |

### What is Explicitly NOT in the Vertical Slice

- No enchanting tables (they exist as non-interactive objects with tooltip: "Requires Arcanem training").
- No mana potions.
- No complex alchemy or recipe discovery.
- No crafting minigames.
- No tool durability.
- No gear socketing.
- No transmog / cosmetic overrides (deferred to Phase 5+).

---

## APPENDIX: INTEGRATION WITH VERTICAL SLICE QUEST FLOW

The crafting system integrates with the vertical slice quests as follows:

1. **Inildon (Level 1-2)**: Player picks up first weapon from a rack. Smithmaster Toren mentions crafting exists but does not teach it. No crafting available yet.
2. **Road to River's Bend (Level 2-5)**: Player encounters first gathering nodes (Iron Ore on river cliffs per the vertical slice doc). They can gather but cannot craft yet. Wildlife drops Animal Hide. This builds up a material stockpile before the tutorial.
3. **River's Bend -- "Iron and Fire" quest (Level 6-7)**: Smith Gaelen teaches smithing. Player gathers 5 Iron Ore, learns intermediate recipes (Iron Ingot), and crafts their first item. All Iron-tier recipes unlock. Inildon forge also becomes usable.
4. **River's Bend -- Alchemy (Level 6+)**: Healer Iona's table allows basic potion crafting. Player combines 3 Riverbloom for a Health Potion I. No formal quest -- the UI is self-explanatory.
5. **Dark Forest Dungeon (Level 10)**: Player finds Dark Ore Fragments (3 available per run: altar, Broodmother, Risen Knight). Enemies drop Chitin Shards, Spider Silk, Dark Herbs. After the dungeon, returning to Smith Gaelen with a Dark Ore Fragment teaches Dark Iron recipes.
6. **Repeat dungeon runs**: Player farms Dark Ore Fragments to craft Dark Iron gear -- the best equipment in the vertical slice. This creates the loot-craft-upgrade loop that will carry through the full game.
