# Spell Remediation -- The Reven
**Created: 2026-03-24**
**Status: DESIGN DRAFT -- Pending human approval**

This document replaces the 60+ spell list from Make it a Game.docx with a focused, buildable spell set aligned to the confirmed 10-school magic system. Arcane/Ward is not a school; those effects are redistributed or made inherent.

---

## Design Constraints

- 10 schools across 3 tiers (Terrestrial, Astral, Baleful)
- 3-5 spells per school, 35-50 spells total
- 5 spell slots (keys 1-2-3-4-5)
- Spells acquired via world tomes or Arcanem questline vendors
- Vertical slice (River's Bend, levels 1-10) needs a playable subset
- Solo dev in UE5 -- every spell must justify its VFX and code cost

---

## Arcane/Ward Resolution

Arcane/Ward is NOT a formal school. Former ward-type effects are handled as follows:

| Old Effect | New Home | Rationale |
|---|---|---|
| Personal magic shield | **Soul** (Astral) | Soul magic governs the self and personal essence |
| Physical barrier / wall | **Earth** (Terrestrial) | Earth magic creates physical structures |
| Spell reflection | **Inherent ability** | All players can parry spells with timed block at higher levels |
| Auto-ward / passive protection | **Enchantment** | Applied to armor, not cast as a spell |
| Anti-magic zone | **Temporal** (Astral) | Temporal disruption nullifies active magic in an area |

---

## Complete Spell List by School

### TERRESTRIAL TIER

#### Flora

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Thorn Lash | 1 | Whips a vine forward dealing damage and briefly slowing the target. | Damage/CC |
| 2 | Entangle | 5 | Roots erupt from the ground, immobilizing enemies in a small area for 3 seconds. | CC |
| 3 | Briarwall | 12 | Grows a thorny hedge that blocks movement and damages enemies who touch it. | Utility/Damage |
| 4 | Spore Cloud | 20 | Releases a toxic cloud that poisons enemies over time and reduces their accuracy. | Debuff/Damage |
| 5 | Wild Growth | 30 | Rapidly regenerates health for the caster and nearby allies while standing still. | Heal |

#### Earth

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Stone Shot | 1 | Launches a chunk of rock that deals physical damage at range. | Damage |
| 2 | Earthen Shield | 5 | Raises a stone slab from the ground that absorbs incoming damage for 5 seconds. | Utility |
| 3 | Tremor | 15 | Slams the ground, staggering all nearby enemies and dealing moderate damage. | Damage/CC |
| 4 | Iron Skin | 22 | Coats the caster in stone, increasing Fortitude and reducing incoming damage for 10 seconds. | Buff |
| 5 | Rupture | 30 | Tears open a line of earth that erupts upward, dealing heavy damage in a straight path. | Damage |

#### Fire

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Firebolt | 1 | Hurls a bolt of fire that deals damage and applies a brief burn. | Damage |
| 2 | Flame Wave | 8 | Sends a cone of fire outward, hitting all enemies in front of the caster. | Damage |
| 3 | Ignite | 15 | Sets a target ablaze, dealing heavy damage over time for 6 seconds. | Damage/Debuff |
| 4 | Ash Veil | 22 | Creates a cloud of hot ash that obscures vision and damages enemies who enter. | Utility/Damage |
| 5 | Inferno | 30 | Calls down a pillar of flame on a target area, dealing massive burst damage. | Damage |

#### Water

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Frost Shard | 1 | Fires a shard of ice that damages and slows the target. | Damage/CC |
| 2 | Mist Step | 8 | The caster dissolves into mist and reappears a short distance away, evading attacks. | Utility |
| 3 | Torrent | 15 | Blasts a stream of pressurized water that knocks enemies back and deals damage. | Damage/CC |
| 4 | Frozen Ground | 22 | Coats the ground in ice, slowing all enemies in the area and reducing their Agility. | CC/Debuff |
| 5 | Deluge | 30 | Summons a localized downpour that heals allies and damages enemies in the area. | Heal/Damage |

#### Wild

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Beast Sense | 3 | Highlights nearby enemies and creatures through walls for 15 seconds. | Utility |
| 2 | Feral Strike | 8 | Enhances the caster's next melee attack with animal ferocity, dealing bonus damage. | Buff/Damage |
| 3 | Summon Wolf | 18 | Calls a spectral wolf that fights alongside the caster for 20 seconds. | Damage |
| 4 | Stampede | 28 | Charges forward with the force of a beast, knocking aside enemies in the path. | Damage/CC |

---

### ASTRAL TIER

#### Restoration

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Mend | 1 | Heals the caster for a moderate amount of health over 3 seconds. | Heal |
| 2 | Purify | 8 | Removes one negative status effect from the caster or a targeted ally. | Utility |
| 3 | Healing Pulse | 15 | Sends out a wave that heals all allies within close range. | Heal |
| 4 | Revitalize | 25 | Greatly increases health regeneration and stamina regeneration for 10 seconds. | Buff |

#### Temporal

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Slow Field | 5 | Creates a zone that slows all enemies inside by 50% for 5 seconds. | CC |
| 2 | Blink | 12 | Instantly teleports the caster a short distance in the direction they are moving. | Utility |
| 3 | Chrono Trap | 20 | Places an invisible trap that freezes the first enemy who triggers it for 3 seconds. | CC |
| 4 | Rewind | 30 | Returns the caster to their position and health from 3 seconds ago. | Utility |

#### Soul

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Soul Ward | 5 | Wraps the caster in protective energy, absorbing a set amount of damage before breaking. | Buff |
| 2 | Spirit Bolt | 10 | Fires a bolt of soul energy that deals damage and briefly disorients the target. | Damage/CC |
| 3 | Fortify | 18 | Strengthens an ally's resolve, increasing their damage resistance for 10 seconds. | Buff |
| 4 | Soul Rend | 28 | Tears at the target's essence, dealing heavy damage and reducing their healing received for 6 seconds. | Damage/Debuff |

---

### BALEFUL TIER

#### Necromancy

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Life Drain | 8 | Channels a beam that steals health from the target and gives it to the caster. | Damage/Heal |
| 2 | Raise Corpse | 15 | Reanimates a nearby dead enemy to fight for the caster for 20 seconds. | Utility |
| 3 | Death's Grasp | 22 | Skeletal hands erupt from the ground, rooting and damaging enemies in a small area. | CC/Damage |
| 4 | Corpse Explosion | 30 | Detonates a reanimated or dead enemy, dealing heavy area damage around it. | Damage |

#### Conjuration

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Summon Shade | 10 | Conjures a minor shade from the Second Realm that attacks nearby enemies for 15 seconds. | Damage |
| 2 | Void Bolt | 15 | Fires a bolt of Second Realm energy that deals damage and reduces the target's armor. | Damage/Debuff |
| 3 | Dark Tether | 22 | Links the caster to a target, siphoning their stamina over 5 seconds. | Debuff |
| 4 | Portal Rift | 30 | Opens a small rift that pulls nearby enemies toward its center for 4 seconds. | CC |

#### Blood

| # | Name | Level Req | Description | Role |
|---|---|---|---|---|
| 1 | Blood Price | 10 | Sacrifices a portion of health to massively boost the caster's next spell's damage. | Buff |
| 2 | Sanguine Burst | 18 | Deals damage in an area around the caster, healing for a percentage of damage dealt. | Damage/Heal |
| 3 | Hemorrhage | 25 | Causes the target to bleed heavily, dealing escalating damage over 8 seconds. | Damage/Debuff |
| 4 | Blood Pact | 30 | Links the caster to an ally; damage taken is split between them, and both gain increased damage. | Buff/Utility |

---

## Spell Count Summary

| School | Count | Available by Lvl 10 |
|---|---|---|
| Flora | 5 | 2 (Thorn Lash, Entangle) |
| Earth | 5 | 2 (Stone Shot, Earthen Shield) |
| Fire | 5 | 2 (Firebolt, Flame Wave) |
| Water | 5 | 2 (Frost Shard, Mist Step) |
| Wild | 4 | 1 (Beast Sense) |
| Restoration | 4 | 2 (Mend, Purify) |
| Temporal | 4 | 1 (Slow Field) |
| Soul | 4 | 1 (Soul Ward) |
| Necromancy | 4 | 1 (Life Drain) |
| Conjuration | 4 | 1 (Summon Shade) |
| Blood | 4 | 1 (Blood Price) |
| **Total** | **48** | **16** |

---

## Vertical Slice: River's Bend (Levels 1-10)

### What the Player Has Access To

The vertical slice covers Inildon (tutorial) through River's Bend and the Dark Forest dungeon. By level 10, the player should have discovered 3-5 spells maximum through:

- **Inildon tutorial**: Player receives ONE starter spell tome as part of the magic discovery sequence. Recommended options based on playstyle:
  - Firebolt (offensive)
  - Mend (defensive)
  - Stone Shot (balanced)

- **River's Bend exploration**: 2-3 spell tomes hidden in the area:
  - One in River's Bend proper (e.g., Frost Shard from a Hallowed Knight supply cache)
  - One on the path to the Dark Forest (e.g., Thorn Lash from an Enul traveler's camp)
  - One as a reward from a side quest (e.g., Entangle from Madam Wilo Lightbane after a defense mission)

- **Dark Forest dungeon**: 1 tome as dungeon loot (e.g., Beast Sense or Life Drain depending on dungeon branch taken)

### Vertical Slice Spell Pool (Available for Placement)

These 16 spells exist at level 10 or below and are candidates for placement in the vertical slice area. Not all should be findable -- pick 4-6 to keep the vertical slice focused:

| Spell | School | Level | Recommended for VS? |
|---|---|---|---|
| Thorn Lash | Flora | 1 | YES -- simple, satisfying |
| Stone Shot | Earth | 1 | YES -- ranged basic |
| Firebolt | Fire | 1 | YES -- classic starter |
| Frost Shard | Water | 1 | YES -- shows CC mechanic |
| Mend | Restoration | 1 | YES -- essential heal |
| Beast Sense | Wild | 3 | MAYBE -- good utility but niche |
| Entangle | Flora | 5 | YES -- teaches crowd control |
| Earthen Shield | Earth | 5 | MAYBE -- defensive option |
| Soul Ward | Soul | 5 | MAYBE -- defensive option |
| Slow Field | Temporal | 5 | NO -- save for Arcanem |
| Flame Wave | Fire | 8 | NO -- save for later |
| Mist Step | Water | 8 | NO -- save for Arcanem |
| Feral Strike | Wild | 8 | NO -- save for Enul |
| Purify | Restoration | 8 | NO -- no debuffs in VS |
| Life Drain | Necromancy | 8 | NO -- save for Dark Forest dungeon reward |
| Blood Price | Blood | 10 | NO -- advanced, save for Arcanem |
| Summon Shade | Conjuration | 10 | NO -- save for Barrenlands |
| Spirit Bolt | Soul | 10 | NO -- save for Arcanem |

**Recommended vertical slice loadout**: Firebolt, Mend, Frost Shard, Thorn Lash, Entangle. This gives the player damage, healing, and CC to fill all 5 slots if they explore thoroughly.

---

## Spell Progression: What a Player Looks Like at Key Levels

### Level 1 (Inildon -- Tutorial)
- **Spell slots used**: 1 of 5
- **Spells known**: 1 starter tome (Firebolt, Mend, or Stone Shot)
- **Combat identity**: Mostly melee/ranged. Magic is a supplement, not the focus.
- **Feeling**: "I just got my first spell. This is new."

### Level 10 (Dark Forest Dungeon Complete)
- **Spell slots used**: 3-5 of 5
- **Spells known**: 4-6 Terrestrial/Restoration basics
- **Combat identity**: Player is choosing whether to lean into magic or keep it as a backup. A magic-focused player might have all 5 slots filled. A melee player might have 2-3 situational spells.
- **Feeling**: "I have options. Spells complement my weapons."

### Level 20 (Post-Arcanem Questline Start)
- **Spell slots used**: 5 of 5
- **Spells known**: 8-12 spells (swapping based on situation)
- **Combat identity**: Clear build direction emerging. Player is making hard choices about which 5 spells to equip. Astral spells becoming available (Blink, Healing Pulse).
- **Feeling**: "I can't equip everything. I need to specialize."

### Level 30 (Approaching Dosram / NW Dred Fortress)
- **Spell slots used**: 5 of 5
- **Spells known**: 15-20 spells across multiple schools
- **Combat identity**: Defined build. Enchantments are modifying how spells behave. Baleful spells available for players who pursued darker questlines. High-tier spells (Inferno, Rewind, Portal Rift) unlocking.
- **Feeling**: "My build is MINE. No one else plays like this."

---

## 5 Spell Slots and the Combat Triangle

### How Slots Interact with Melee / Ranged / Magic

The 5 spell slots are available to ALL players regardless of combat focus. The combat triangle determines how heavily the player relies on them:

**Melee-Focused Player** (1-2 spell slots active)
- Typical loadout: Mend (emergency heal), Iron Skin (pre-fight buff), and 3 empty/situational slots
- Spells are panic buttons and pre-fight preparation
- Stamina and weapon damage are the primary resources
- Example: Sword-and-shield fighter who pops Iron Skin before a boss, Mend when health gets low

**Ranged-Focused Player** (2-3 spell slots active)
- Typical loadout: Beast Sense (tracking), Entangle (create distance), Mend (sustain), and 2 weapon-focused slots or utility
- Spells create space and provide awareness
- Ammo and positioning are the primary concerns
- Example: Crossbow user who roots enemies with Entangle, then fires from range

**Magic-Focused Player** (4-5 spell slots active)
- Typical loadout: Full offensive/utility spread. Firebolt, Frost Shard, Flame Wave, Mend, Blink
- Spells are the primary damage source
- Mana management and cooldowns define the rhythm
- Sorcery stat is the highest priority
- Example: Pure caster rotating through cooldowns, using Blink to reposition

**Hybrid Player** (3 spell slots active)
- The intended sweet spot. Most players will mix weapon combat with spells.
- Example: Melee with Fire and Earth magic -- Firebolt to open, charge in with sword, Tremor when surrounded, Iron Skin for tough fights, Mend to recover

### Slot Management Rules
- Spells can be swapped at any time OUTSIDE of combat
- No swapping during combat (once you engage, you fight with what you have)
- Empty spell slots are valid -- no penalty for leaving slots unfilled
- Spell slots are separate from item/consumable slots (keys 6-0)

---

## Enchantment System: Playstyle Effects

Enchantments change HOW you play, not just how much damage you deal. They are applied to armor through the crafting system (learn enchantment, disenchant gear to extract, apply with essence + gold).

### Example Enchantments That Affect Playstyle

| Enchantment | Slot | Effect | Playstyle Change |
|---|---|---|---|
| Spell Echo | Gloves | 15% chance a spell casts twice at half power. | Rewards spell spamming over careful timing. Changes mana economy. |
| Thorn Mantle | Body | Enemies who hit you in melee take damage back. | Makes getting hit a viable strategy. Tanks become punishing to attack. |
| Windrunner | Boots | Dodging grants 3 seconds of increased movement speed. | Encourages aggressive dodge-then-reposition gameplay. Hit-and-run. |
| Mana Siphon | Legs | Melee kills restore a percentage of max mana. | Incentivizes melee-focused players to use spells more by solving their mana problem. |
| Blood Resonance | Head | Spells cost health instead of mana when mana is depleted. | Allows casters to keep casting in emergencies at the cost of health. Risk/reward. |
| Grounded | Boots | Cannot be knocked back. Movement speed reduced by 10%. | Slower but immovable. Changes positioning from dodge-based to stand-and-fight. |
| Hunter's Mark | Gloves | Enemies you hit with a ranged weapon take 20% more spell damage for 3 seconds. | Creates a weapon-to-spell combo loop. Rewards hybrid ranged/magic builds. |
| Last Stand | Body | Below 25% health, damage dealt increases by 30%. | Encourages risky play. Changes heal timing -- do you heal or push damage? |

### Enchantment Design Rules
- One enchantment per armor piece (5 max active)
- Enchantments should create decisions, not just numbers
- No enchantment should be mandatory for any build
- Enchantments that interact with specific schools should be rare (endgame loot)

---

## Implementation Notes for Solo Dev

### VFX Priority
Every spell needs a visual effect. To keep scope manageable, group spells by VFX archetype:

| VFX Archetype | Spells That Share It | Asset Count |
|---|---|---|
| Projectile (straight line) | Firebolt, Stone Shot, Frost Shard, Spirit Bolt, Void Bolt | 1 base + color/material variants |
| Cone / Wave (fan out) | Flame Wave, Torrent, Thorn Lash | 1 base + variants |
| Ground AoE (circle on floor) | Entangle, Tremor, Frozen Ground, Death's Grasp, Slow Field | 1 base + variants |
| Self Buff (glow on player) | Iron Skin, Soul Ward, Fortify, Blood Price, Revitalize | 1 base + color variants |
| Heal (particle rise) | Mend, Healing Pulse, Wild Growth, Deluge | 1 base + variants |
| Summon (spawn in) | Summon Wolf, Raise Corpse, Summon Shade | 1 base + model swap |
| Beam (sustained channel) | Life Drain, Dark Tether | 1 base + variants |
| Teleport (disappear/reappear) | Blink, Mist Step, Rewind | 1 base + variants |
| Placed Object (persistent) | Briarwall, Earthen Shield, Chrono Trap, Ash Veil, Spore Cloud | 1 base + variants |
| Pillar / Line (vertical burst) | Inferno, Rupture, Sanguine Burst, Corpse Explosion | 1 base + variants |

**Total unique VFX archetypes: 10.** With material/color swaps, this covers all 48 spells.

### Build Order for Spells
1. **First**: Firebolt, Stone Shot, Frost Shard, Mend (4 spells = vertical slice minimum)
2. **Second**: Entangle, Earthen Shield, Flame Wave, Thorn Lash, Beast Sense (round out VS)
3. **Third**: All remaining Terrestrial and Restoration spells (fills levels 1-20)
4. **Fourth**: Astral and Baleful spells (levels 15-30 content)
5. **Last**: High-level spells and niche utility (Rewind, Portal Rift, Blood Pact, Corpse Explosion)

---

## Phased-Out Spells

The following categories from Make it a Game.docx are explicitly removed or deferred:

- **All Ward/Arcane spells** -- Redistributed per the resolution table above
- **Duplicate elemental variants** -- One damage spell per element at early levels, not three
- **Passive aura spells** -- Replaced by the enchantment system
- **Faction-locked spells** -- All spells are available to all players via tomes or Arcanem vendors
- **PvP-only spells** -- No spells are PvP-exclusive; PvP balance comes from calibration modifiers
- **Spells requiring NPC AI companions** -- Summons are temporary, not permanent companions

---

## Open Questions

1. **Baleful spell acquisition**: Should Necromancy/Conjuration/Blood spells require specific questlines (e.g., finding a warlock mentor), or are they available at the Arcanem like everything else? Restricting them adds flavor but also gates content.
2. **Spell upgrades**: Should spells level up with use (Skyrim-style), or do you find stronger versions as separate tomes? The former is simpler to implement; the latter gives more loot variety.
3. **Cooldown values**: Not defined here. Need playtesting. Suggest starting with: damage spells 3-5 sec, CC spells 8-12 sec, heals 6-10 sec, utility 15-20 sec.
4. **Mana costs**: Not defined here. Suggest a simple formula: mana cost = level requirement x 2. Firebolt (lvl 1) = 2 mana, Inferno (lvl 30) = 60 mana.
