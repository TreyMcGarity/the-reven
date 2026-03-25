# THE REVEN -- Combat System Specification
**Created: 2026-03-25**
**Status: BUILD DOCUMENT -- Implementable specification for solo dev in UE5 Blueprints**
**Scope: Full combat system covering melee, ranged, magic, enemy AI, PvP prep, and UE5 architecture**

---

## 1. CORE COMBAT LOOP

### Combat Flow

The player engages enemies using a combination of melee attacks, ranged weapons, spells, and defensive abilities. Every action except basic movement costs stamina or mana, creating a resource management layer on top of mechanical skill.

**Core loop**: Position -> Attack/Cast -> Defend/Evade -> Recover Resources -> Repeat.

### Input Mapping (Enhanced Input System)

All inputs use UE5 Enhanced Input Actions mapped to Input Mapping Contexts. One context for "OnFoot_Combat" handles all combat inputs. A separate "OnFoot_UI" context handles menus and disables combat inputs.

| Input | Key/Button | Enhanced Input Action | Notes |
|---|---|---|---|
| Light Attack | LMB (Click) | IA_LightAttack | Tap for single swing, tap repeatedly for combo |
| Heavy Attack | LMB (Hold 0.4s+) | IA_HeavyAttack | Triggered on release after hold threshold |
| Block | RMB (Hold) | IA_Block | Ongoing trigger while held |
| Parry | RMB tap within 200ms of incoming hit | IA_Block (+ timing check) | Parry is detected in Blueprint: if IA_Block starts within 200ms before a hit lands, it counts as a parry |
| Dodge | Shift | IA_Dodge | Fires once per press, cooldown enforced in Blueprint |
| Sprint | Shift (Hold, no combat) | IA_Sprint | Same key as dodge; sprint activates when held outside combat, dodge fires on tap or in combat |
| Aim (Ranged) | RMB (Hold, bow equipped) | IA_Aim | Overrides block when ranged weapon is active |
| Fire (Ranged) | LMB (while aiming) | IA_RangedFire | Release to fire; hold duration = charge level |
| Spell Slot 1 | 1 | IA_Spell1 | Casts equipped spell |
| Spell Slot 2 | 2 | IA_Spell2 | |
| Spell Slot 3 | 3 | IA_Spell3 | |
| Spell Slot 4 | 4 | IA_Spell4 | |
| Spell Slot 5 | 5 | IA_Spell5 | |
| Interact | E | IA_Interact | Pick up items, talk to NPCs |
| Item Slots | 6-0 | IA_Item1 through IA_Item5 | Consumables (potions, food) |

**Context switching**: When the player opens inventory (Tab) or dialogue (E on NPC), switch to OnFoot_UI context. All combat inputs are disabled. Return to OnFoot_Combat on close.

### Animation States and Transitions

The Animation Blueprint uses two layers:

**Layer 1 -- Locomotion State Machine**:
- States: Idle, Walk, Run, Sprint, Dodge_Roll, Exhausted, Death
- Transitions driven by velocity, booleans (bIsSprinting, bIsExhausted, bIsDodging, bIsDead)
- Locomotion runs on the full body when no combat montage is playing

**Layer 2 -- Combat Montage Slot (Upper Body / Full Body)**:
- Attack montages play on a "DefaultSlot" and override locomotion
- Light attacks use upper body slot (player can move slowly during swings)
- Heavy attacks, dodge, and parry reactions use full body slot (movement locked)
- Block is an additive pose blend, not a montage -- blended on top of locomotion so the player can walk while blocking

**Montage list per weapon type**:

| Weapon | Light 1 | Light 2 | Heavy | Block Pose | Parry React |
|---|---|---|---|---|---|
| Sword (1H) | Slash R | Slash L | Overhead Chop | Shield/Blade Guard | Deflect + Riposte |
| Axe (1H) | Chop R | Chop L | Overhead Slam | Blade Guard | Deflect + Riposte |
| Mace (1H) | Swing R | Swing L | Overhead Crush | Blade Guard | Deflect + Riposte |
| Greatsword (2H) | Wide Slash R | Wide Slash L | Overhead Slam | Two-Hand Guard | Deflect + Riposte |
| Glaive/Spear (2H) | Thrust | Sweep | Lunge Thrust | Shaft Guard | Deflect + Riposte |
| Bow (2H) | N/A (uses aim/fire) | N/A | N/A | N/A | N/A |

### Hit Detection

**Approach**: Sphere traces fired from weapon sockets during AnimNotify windows.

1. Each attack montage has two AnimNotifies: `AN_TraceStart` and `AN_TraceEnd`.
2. Between these notifies, the CombatComponent fires a sphere trace every tick (or every 2nd tick for performance) from the weapon's base socket to its tip socket.
3. Sphere trace radius: 8-12 cm (tunable per weapon type; maces wider, swords narrower).
4. On hit, the trace returns the hit actor. The CombatComponent checks if that actor was already hit during this swing (store in a TArray cleared on AN_TraceStart). If not already hit, apply damage.
5. Multi-hit weapons (axe cleave, greatsword sweep) allow hitting multiple unique actors per swing. Single-target weapons (sword, mace) stop after the first hit.

**Why sphere trace over overlap volumes**: Sphere traces are frame-accurate, simple to debug (DrawDebugLine), and do not require collision presets on weapon meshes. They also handle fast swings without ghosting.

### Damage Calculation

**Melee damage formula**:
```
final_damage = base_weapon_damage * (1 + Might / 100) * combo_multiplier * critical_multiplier
```

| Variable | Source | Example |
|---|---|---|
| base_weapon_damage | Weapon DataTable row | Iron Sword: 15 |
| Might | Player stat (base 5 + gear + level-up points) | 14 at level 10 |
| combo_multiplier | 1.0 for hit 1, 1.0 for hit 2, 1.5 for combo finisher | 1.5 on 2nd light hit |
| critical_multiplier | 1.0 normal, 2.0 on crit (5% base chance + Agility/200) | Rare at low levels |

**Example at level 10**: Iron Sword (15 base) * (1 + 14/100) * 1.0 = 15 * 1.14 = 17.1 -> rounds to 17 damage on a normal hit.

**Ranged damage formula**:
```
final_damage = base_weapon_damage * (1 + Might / 100) * charge_multiplier
```
- charge_multiplier: 0.5 (quick shot), 0.75 (half draw), 1.0 (full draw)

**Spell damage formula**:
```
final_damage = base_spell_damage * (1 + Sorcery / 100)
```

**Damage reduction on target**:
```
damage_taken = final_damage * (1 - damage_reduction%)
damage_reduction% = Fortitude / (Fortitude + 100)
```

At Fortitude 10: reduction = 10/110 = 9.1%. At Fortitude 50: reduction = 50/150 = 33.3%. This formula has diminishing returns and never reaches 100%.

### How Stats Affect Combat

| Stat | Combat Effect | Formula |
|---|---|---|
| Might | Melee/ranged damage multiplier | damage * (1 + Might/100) |
| Fortitude | Damage reduction; max HP bonus; HP regen | DR = Fort/(Fort+100); HP bonus = Fort * 2; HP regen = 1 + Fort/50 per sec |
| Haste | Movement speed; stamina regen rate | Move speed = base * (1 + Haste/200); Stamina regen = 3 + Haste/20 per sec |
| Agility | Attack speed; dodge i-frame duration | Montage play rate = 1.0 + Agility/200; i-frames = 0.3 + Agility/500 sec (cap 0.5s) |
| Sorcery | Spell damage multiplier; mana regen; cooldown reduction | damage * (1 + Sorcery/100); Mana regen = 5 + Sorcery/10 per sec; CD reduction = Sorcery/200 (cap 40%) |

---

## 2. STAMINA SYSTEM

### Base Values

- **Base max stamina**: 100
- **Stamina scales with Haste**: max_stamina = 100 + Haste * 1 (at Haste 10, max is 110)

### Stamina Costs

| Action | Stamina Cost | Notes |
|---|---|---|
| Light Attack | 10 | Per swing in combo |
| Heavy Attack | 25 | On release |
| Dodge Roll | 20 | Per roll |
| Block (per hit absorbed) | 5-30 | Proportional to incoming damage: cost = incoming_damage * 0.5 |
| Block (passive hold) | 0 | Holding block costs nothing; only absorbing hits drains |
| Sprint | 5/sec | Continuous drain while sprinting |
| Parry | 0 | Successful parry costs zero stamina (reward for skill) |

### Regeneration

- **Base regen**: 3% of max stamina per second = 3.0/sec at 100 max
- **Haste scaling**: regen_per_sec = max_stamina * 0.03 * (1 + Haste / 100)
- **Regen delay**: Stamina regen pauses for 1.0 second after any stamina-consuming action
- **During sprint**: Stamina does not regen while sprinting (drain overrides regen)
- **During block hold**: Stamina regens normally while holding block (only pauses on hit absorption)

### Stamina Break

When stamina reaches 0 from any cause:

1. **Exhaustion state** triggers: 1.5 seconds of vulnerability
2. During exhaustion: cannot attack, block, dodge, or sprint
3. Player character plays a stagger/heavy breathing animation
4. Stamina regen resumes at **half rate** during exhaustion (1.5% max/sec instead of 3%)
5. After 1.5 seconds, exhaustion ends and normal regen resumes
6. A visible "cracked shield" VFX and stamina bar flash communicate the state to the player

### Tactical Design Intent

Stamina creates the following decisions:
- **Aggression vs. reserve**: A player who spam-attacks will run out of stamina and be unable to dodge. A player who conserves can dodge-punish.
- **Block vs. dodge**: Blocking drains stamina proportional to damage. Against heavy hitters (bosses, Dred Soldiers), blocking is expensive. Dodging costs a flat 20 but requires timing. Parrying costs nothing but requires precision.
- **Sprint vs. fight-ready**: Sprinting into a fight means arriving with reduced stamina. Approach on foot, sprint to escape.
- **Combo commitment**: A full 2-hit light + heavy finisher costs 10 + 10 + 25 = 45 stamina. That is nearly half the bar. If the enemy dodges, the player is vulnerable.

---

## 3. MELEE COMBAT

### Weapon Types in Vertical Slice

**One-Handed Weapons (Main Hand)**:

| Weapon | Base Damage (Iron) | Speed | Stamina/Light | Stamina/Heavy | Special |
|---|---|---|---|---|---|
| Sword | 15 | 1.0x | 10 | 25 | Balanced. 2-hit light combo + heavy finisher. |
| Axe | 18 | 0.85x | 12 | 28 | Cleave: hits all enemies in a 120-degree arc. |
| Mace | 20 | 0.8x | 12 | 30 | Sunder: heavy attack reduces target's damage reduction by 20% for 5 seconds. |

**Off-Hand**:

| Item | Effect |
|---|---|
| Shield | Block absorbs damage based on shield value + Fortitude. Enables parry. Damage absorbed = shield_value + Fortitude. |
| Torch | Light source in dark areas. Minimal fire damage (3) on melee swing. No block. |
| Empty | Allows two-hand grip on main-hand weapon (10% damage bonus, no block). |

**Two-Handed Weapons (Back Slot)**:

| Weapon | Base Damage (Iron) | Speed | Stamina/Light | Stamina/Heavy | Special |
|---|---|---|---|---|---|
| Greatsword | 28 | 0.65x | 15 | 35 | Sweep: light attacks hit 180-degree arc. Highest raw damage. |
| Glaive/Spear | 22 | 0.85x | 10 | 25 | Reach: 50% longer trace distance. Thrust attacks. Good vs. single targets. |

**Weapon switching**: The player can swap between main-hand+off-hand and back-slot weapon outside of active attack animations. A quick swap animation plays (0.5 sec). Cannot swap mid-combo.

### Combo System

Combos are simple and intentional -- 2 light attacks into 1 heavy finisher. No elaborate fighting-game inputs.

**Sword combo**:
1. LMB -> Light 1 (slash right, 10 stamina, 1.0x damage)
2. LMB -> Light 2 (slash left, 10 stamina, 1.0x damage)
3. Hold LMB -> Heavy Finisher (overhead chop, 25 stamina, 1.5x damage)

**Axe combo**:
1. LMB -> Light 1 (chop right, cleave arc, 12 stamina, 1.0x damage)
2. LMB -> Light 2 (chop left, cleave arc, 12 stamina, 1.0x damage)
3. Hold LMB -> Heavy Finisher (overhead slam, 28 stamina, 1.5x damage, sunder on hit)

**Mace combo**:
1. LMB -> Light 1 (swing right, 12 stamina, 1.0x damage)
2. LMB -> Light 2 (swing left, 12 stamina, 1.0x damage)
3. Hold LMB -> Heavy Finisher (overhead crush, 30 stamina, 1.5x damage, sunder)

**Greatsword combo**:
1. LMB -> Light 1 (wide slash right, 180-degree arc, 15 stamina, 1.0x damage)
2. LMB -> Light 2 (wide slash left, 180-degree arc, 15 stamina, 1.0x damage)
3. Hold LMB -> Heavy Finisher (overhead slam, 35 stamina, 1.8x damage)

**Glaive/Spear combo**:
1. LMB -> Light 1 (thrust, 10 stamina, 1.0x damage)
2. LMB -> Light 2 (sweep, 10 stamina, 1.0x damage)
3. Hold LMB -> Heavy Finisher (lunge thrust, 25 stamina, 1.5x damage, extra range)

**Combo window**: The player has 0.6 seconds after an attack animation ends to input the next attack in the combo chain. If the window expires, the combo resets to Light 1. The combo window is checked in the AnimNotify `AN_ComboWindowOpen` / `AN_ComboWindowClose`.

**Combo buffer**: If the player presses LMB during an active attack animation (before the combo window opens), the input is buffered and fires when the window opens. This makes combos feel responsive without requiring frame-perfect timing.

### Block

- **Activation**: Hold RMB. Player enters block stance (additive pose blend on locomotion).
- **Movement**: Player can walk at 50% speed while blocking. Cannot sprint or dodge while blocking.
- **Damage absorption**: `damage_absorbed = incoming_damage - (shield_value + Fortitude * 0.5)`. Minimum damage taken = 1 (chip damage through block). If no shield equipped, `shield_value = 0` and the weapon provides partial block (50% effectiveness).
- **Stamina drain on block**: Each blocked hit costs `blocked_damage * 0.5` stamina. If the player has a shield, multiply stamina cost by 0.7 (shields are more stamina-efficient).
- **Guard break**: If stamina reaches 0 while blocking, the player enters Stamina Break (1.5 sec exhaustion). The block drops. This means holding block against a boss is not viable -- the player must mix in dodges and parries.

### Parry

- **Input**: Press RMB (start of block). The first 200ms of block activation is the parry window.
- **Detection**: When an enemy attack hits the player during the parry window, the hit is deflected.
- **On success**:
  - Incoming damage is nullified (0 damage taken).
  - No stamina cost.
  - The attacker is staggered for 2.0 seconds (plays stagger animation, cannot act).
  - A bright flash VFX and sharp metallic SFX play to reward the player.
  - The player can follow up with a free riposte attack during the stagger window (equivalent to a heavy attack in damage, but faster animation).
- **On failure**: The block catches the hit normally (damage absorbed, stamina drained). The player is not penalized beyond a missed parry opportunity.
- **Implementation**: The CombatComponent tracks `ParryWindowActive` (bool). Set to true when IA_Block fires, set to false after 200ms (use a timer). When the HealthComponent receives damage, it checks `ParryWindowActive` first. If true, trigger parry logic instead of damage.

### Dodge

- **Input**: Shift key (tap).
- **Direction**: Roll in the current movement direction. If stationary, roll backward.
- **Invincibility frames (i-frames)**: The player is invulnerable for the first 0.3 seconds of the roll animation (total roll duration: ~0.6 seconds). During i-frames, all incoming damage is ignored (collision channel set to ignore during window, or a `bIsInvulnerable` flag checked in damage reception).
- **Agility scaling**: i-frame duration = 0.3 + Agility / 500 seconds, capped at 0.5 seconds.
- **Stamina cost**: 20 per dodge.
- **Recovery**: 0.3 seconds after the roll ends before the player can attack, block, or dodge again. The player can move during recovery.
- **Cooldown**: No hard cooldown. Stamina is the only limiter. At base 100 stamina, the player can dodge 5 times before exhaustion. With regen delay factored in, practical limit is 3-4 rapid dodges.

---

## 4. RANGED COMBAT

### When the Player Gets Ranged Weapons

- **Inildon (Level 1-2)**: A basic bow is available at the Inilvan Command weapon rack during the tutorial. The player can pick it up immediately.
- **River's Bend (Level 5-6)**: Iron-tier bow available as a Night Patrol quest reward (player's choice of weapon). Arrows can be crafted or purchased from Smith Gaelen.
- **Crossbow**: Not in the vertical slice. Introduced at the Crossroads/Ashpeak area (levels 12-15). Noted here for future design.

### Bow Mechanics

**Aiming**:
- Hold RMB with a bow equipped to enter aim mode.
- Camera shifts to a tighter over-the-shoulder view (offset the camera 30cm right, zoom FOV from 90 to 70).
- A crosshair appears at screen center. No lock-on. Aiming is manual (skill-based).
- Movement speed reduced to 40% while aiming.

**Drawing and Firing**:
- While aiming (RMB held), hold LMB to draw the bowstring.
- Three charge tiers based on hold duration:

| Tier | Hold Time | Damage Multiplier | Arrow Speed | Visual Cue |
|---|---|---|---|---|
| Quick Shot | 0-0.4s | 0.5x | 3000 cm/s | No glow, loose string |
| Half Draw | 0.4-1.0s | 0.75x | 4500 cm/s | Faint glow on arrow |
| Full Draw | 1.0s+ | 1.0x | 6000 cm/s | Bright glow, string fully taut, slight camera shake |

- Release LMB to fire. The arrow spawns as a projectile actor at the bow socket.
- If the player holds past 2.5 seconds at full draw, stamina begins draining at 8/sec (arm fatigue). No auto-fire.

**Projectile Physics**:
- Arrows are physics projectiles with gravity. They are NOT hitscan.
- Gravity: -980 cm/s^2 (UE5 default). Arrows arc over distance.
- At full draw (6000 cm/s initial velocity), an arrow drops approximately:
  - 10m range: ~1.3 cm drop (negligible)
  - 30m range: ~12 cm drop (slight aim adjustment)
  - 60m range: ~49 cm drop (noticeable arc, player must aim above target)
- Arrow collision: On hit, the projectile actor stops, attaches to the hit surface/actor, and applies damage. Arrows stick in surfaces for 10 seconds (visual feedback), then despawn.
- Arrow passthrough: Arrows do NOT pass through enemies. One hit per arrow.

**Damage formula (ranged)**:
```
final_damage = base_bow_damage * (1 + Might / 100) * charge_multiplier
```
- Iron Bow base damage: 20
- Full draw at Might 14: 20 * 1.14 * 1.0 = 22.8 -> 23 damage

### Ammo System

- Arrows are inventory items. The player must have arrows to fire.
- **Starting ammo**: The bow from the tutorial comes with 20 arrows.
- **Crafting**: 5 Timber = 20 Arrows (basic recipe, available from tutorial).
- **Purchasing**: Smith Gaelen and Smithmaster Toren sell arrows -- 20 arrows for 10 gold.
- **Stack limit**: 99 arrows per inventory slot.
- **Arrow types in vertical slice**: Only standard arrows. Iron-tipped arrows (1.2x damage) craftable at Iron-tier smithing. Fire arrows, poison arrows deferred to later phases.
- **Ammo display**: Current arrow count shown next to the stamina bar on HUD when bow is equipped.

### Crossbow (Future -- Not in Vertical Slice)

- Point-and-shoot. No charge mechanic. Higher base damage than bow.
- Slower reload (1.5 sec bolt loading animation).
- Bolt projectiles: faster velocity, less arc, shorter max range.
- Bolts are a separate ammo type from arrows.
- Introduced at Crossroads alongside the Wend traders.

### Charge Shot Mechanic

The charge system (detailed above) rewards patience:
- Quick shots for suppression and interrupt (low damage but fast)
- Full draws for openers and punish windows (full damage, satisfying hit)
- Stamina drain on over-hold prevents infinite camping at full draw

---

## 5. MAGIC COMBAT

### Spell Slot System

- 5 spell slots mapped to keys 1-2-3-4-5.
- Spells are equipped from inventory outside of combat. Cannot swap spell loadout during combat (combat = any enemy has aggro on the player).
- Empty slots are valid. No penalty for leaving slots unfilled.
- When a spell slot key is pressed, the equipped spell casts if: (a) the player has enough mana, (b) the spell is not on cooldown, and (c) the player is not in a state that prevents casting (stunned, exhausted, mid-dodge).

### When the Player Gets Magic

- **Level 6-7 (River's Bend)**: Arcanist Syla teaches the player their first spell as part of the "Sparks of the Arcanem" quest.
- **Level 6-10 (River's Bend area)**: Up to 5 spells findable through quests, exploration, and purchase.
- **Level 6-8 (Arcanem questline start)**: Referenced by Syla. The Arcanem itself is beyond the vertical slice boundary but establishes the full magic system unlock.

### Mana System

- **Base max mana**: 100
- **Mana regen**: 5 + Sorcery / 10 per second (at Sorcery 5: 5.5/sec; at Sorcery 25: 7.5/sec)
- **Regen delay**: Mana regen pauses for 1.5 seconds after casting a spell
- **Mana potion**: Not in vertical slice (deferred to advanced alchemy). Players manage mana through regen timing and spell selection.

### Vertical Slice Spells -- Complete Data

| Spell | School | Key Cost | Mana Cost | Cooldown | Base Damage/Heal | Cast Time | Target Type | Effect |
|---|---|---|---|---|---|---|---|---|
| Firebolt | Fire | Any slot | 8 | 3 sec | 25 damage | Instant | Projectile (straight line) | Deals damage + 3 sec burn (4 damage/sec = 12 total DOT) |
| Mend | Restoration | Any slot | 15 | 8 sec | 30 heal | 1.0 sec (channel, interruptible) | Self-cast | Heals over 3 seconds (10/sec) |
| Frost Shard | Water | Any slot | 10 | 3 sec | 20 damage | Instant | Projectile (straight line) | Deals damage + 30% slow for 3 seconds |
| Thorn Lash | Flora | Any slot | 10 | 4 sec | 18 damage | Instant | Cone (8m range, 60-degree arc) | Deals damage + 20% slow for 2 seconds |
| Entangle | Flora | Any slot | 20 | 10 sec | 0 damage | 0.5 sec | Ground AoE (decal, 4m radius, 15m range) | Roots all enemies in area for 3 seconds. Rooted enemies can still attack but cannot move. |

### Casting Mechanics

**Instant-cast spells** (Firebolt, Frost Shard, Thorn Lash):
- Press the spell key. The casting animation plays (arm thrust forward, ~0.3 sec). The spell fires on the AnimNotify `AN_SpellRelease` within the montage.
- The player can move at full speed during instant casts.
- The player can be interrupted by taking a hit during the 0.3 sec animation (the cast cancels, mana is NOT consumed, cooldown does NOT start). This incentivizes casting at safe moments.

**Channeled spells** (Mend):
- Press the spell key. The channel animation plays (arms inward, glowing effect). The effect applies over the channel duration (3 seconds for Mend).
- Player movement speed reduced to 30% during channel.
- Taking damage interrupts the channel. Remaining heal ticks are lost. Mana is consumed at the start. Cooldown starts when the channel begins (not when it ends), so interrupted casts still go on cooldown.
- The player can cancel a channel by pressing the spell key again or pressing dodge.

**Targeted AoE spells** (Entangle):
- Press the spell key. A ground decal (green circle, 4m radius) appears at the cursor/camera crosshair position, projected onto the terrain. The player can aim the decal by looking around.
- Press LMB or the spell key again to confirm placement. The 0.5 sec cast animation plays, then roots erupt at the targeted location.
- Max placement range: 15m from the player. If the cursor target is beyond 15m, the decal snaps to 15m.
- The player can move at 50% speed while aiming the decal. Taking damage cancels the targeting (no mana consumed).

### Spell Targeting Types

| Type | Description | Implementation |
|---|---|---|
| Projectile | Spawns an actor that flies forward from the player's hand. Uses projectile movement component. Collides with first enemy hit. | BP_SpellProjectile with speed, damage, and VFX set per spell. Spawn at hand socket on AN_SpellRelease. |
| Cone | Overlap check in a cone shape in front of the player. Instant. | Use a cone-shaped collision sweep or multiple sphere traces in a fan pattern on AN_SpellRelease. |
| Ground AoE (Decal) | Player aims a ground indicator, confirms, effect spawns at location after cast time. | Decal actor follows cursor raytrace during targeting. On confirm, spawn AoE effect actor at position. AoE effect actor does overlap check on spawn. |
| Self-Cast | Effect applies directly to the caster. No targeting needed. | On AN_SpellRelease, apply effect directly to the player character. |

### Sorcery Stat Scaling

| Effect | Formula |
|---|---|
| Spell damage | base_spell_damage * (1 + Sorcery / 100) |
| Mana regen | 5 + Sorcery / 10 per second |
| Cooldown reduction | Sorcery / 200, capped at 0.4 (40% max CDR) |
| Spell cast speed | Not scaled in vertical slice. Deferred. |

**Example at level 10 (Sorcery 25)**: Firebolt base 25 * (1 + 25/100) = 25 * 1.25 = 31.25 -> 31 damage. Mana regen: 5 + 2.5 = 7.5/sec. CDR: 25/200 = 12.5% -> Firebolt cooldown becomes 3 * 0.875 = 2.625 sec.

---

## 6. DAMAGE TYPES AND RESISTANCES

### Damage Types

| Type | Source | Vertical Slice Usage |
|---|---|---|
| Physical | Melee weapons, arrows, Stone Shot spell | Primary damage type. All weapon attacks. |
| Fire | Firebolt burn DOT, torches | Secondary. Firebolt DOT is fire damage. |
| Nature | Thorn Lash, Entangle (if damage added later), Spore Cloud (dungeon hazard), Broodmother poison | Dungeon environmental hazards and Nilvar Shaman attacks. |
| Dark | Risen Knight Phase 2 attacks, Nilvar corruption effects | Boss-specific. Foreshadows later dark magic enemies. |
| Frost | Frost Shard | Minimal in vertical slice. Frost Shard deals frost-typed damage. |

### Vertical Slice Scope: 3 Resistance Types

To keep scope manageable, only three resistance categories matter in the vertical slice:

1. **Physical Resistance**: Reduces physical damage. Primary stat: Fortitude + armor value.
2. **Elemental Resistance (Fire + Frost + Nature combined)**: A single "Elemental Resistance" stat on gear. Reduces damage from all elemental sources.
3. **Dark Resistance**: Rare stat, only appears on dungeon loot. Reduces dark damage from the Risen Knight and Nilvar corruption.

### How Resistances Work

**Flat reduction model** (simple, no % stacking complexity):

```
damage_taken = max(1, incoming_damage - resistance_value)
```

- Physical resistance = Fortitude / 2 + armor_value (from gear DataTable)
- Elemental resistance = sum of elemental_resist on all equipped gear
- Dark resistance = sum of dark_resist on all equipped gear

**Example**: Player has Fortitude 12 (phys resist = 6) and Iron Body Armor (armor_value = 5, elemental_resist = 2). Total physical resistance = 11. A Dred Soldier hits for 20 physical damage -> player takes 20 - 11 = 9 damage.

**Why flat reduction over percentage**: Flat reduction is simpler to implement, easier to reason about, and creates clear breakpoints ("I need 5 more resistance to survive that boss hit"). Percentage reduction can be migrated to in Phase 4 if scaling demands it.

### Gear Resistance Values (Vertical Slice Ranges)

| Tier | Armor Value (Physical) | Elemental Resist | Dark Resist |
|---|---|---|---|
| Leather (Common) | 2-4 per piece | 0-1 | 0 |
| Iron (Common) | 4-7 per piece | 0-2 | 0 |
| Reinforced Iron (Uncommon) | 6-10 per piece | 1-3 | 0 |
| Dark Iron (Rare) | 8-14 per piece | 2-5 | 2-4 |

Full set of Dark Iron armor: ~50-70 physical resistance, 10-25 elemental, 10-20 dark. This makes the Risen Knight fight significantly easier with dungeon-farmed gear -- intentional as a reward loop.

---

## 7. ENEMY COMBAT AI (Behavior Trees)

All enemies use UE5 Behavior Trees with a shared base structure. Enemy-specific behavior is handled through Behavior Tree subtrees and Blackboard values.

### Shared AI Framework

**Perception Component** (on all enemies):
- Sight: 120-degree FOV, range varies by type (see table below)
- Sound: Detects player footsteps (walk = 300cm radius, sprint = 800cm radius), combat sounds (weapon swings = 500cm)
- Perception is checked every 0.5 seconds (not every frame -- performance)

**Aggro and Leashing**:
- When an enemy detects the player, it sets `bAggroed = true` on its Blackboard.
- **Aggro leash distance**: If the player moves more than 30m from the enemy's spawn point, the enemy breaks aggro and returns to its patrol route.
- **De-aggro timer**: If the enemy loses sight of the player for 8 seconds AND the player is beyond 15m, the enemy de-aggros.
- **De-aggro behavior**: Enemy walks back to spawn point at normal speed, regenerating HP at 10%/sec. Fully heals on reaching spawn. This prevents exploit kiting.

**Shared Behavior Tree Structure**:
```
Root (Selector)
├── [1] Death Check (if HP <= 0 -> Die sequence)
├── [2] Combat Branch (if bAggroed == true)
│   ├── Chase (MoveTo player, if distance > attack range)
│   └── Attack (if distance <= attack range, run attack subtree)
├── [3] Patrol Branch (if has patrol path)
│   └── MoveTo next patrol point, wait 2-5 sec, advance
└── [4] Idle (play idle animation)
```

### Enemy Type Specifications

#### Wild Boar (Level 1-2)

| Property | Value |
|---|---|
| HP | 30 |
| Damage | 5 (Physical) |
| Attack Speed | 1 attack every 2.5 sec |
| Detection Range (Sight) | 10m |
| Detection Range (Sound) | 5m |
| Aggro Leash | 20m |
| Movement Speed | 400 cm/s (charge: 800 cm/s) |
| Behavior | Territorial. Grazes in a small area. Aggros if player enters 6m. Charges in a straight line (1.5s telegraph -- lowers head, scrapes hoof). Charge is dodgeable. After charge, pauses 2s (punish window). De-aggros if player exits 20m. |
| Loot | Leather Scraps (50%), Raw Meat (50%), nothing (25%) |

#### Wolf (Level 3)

| Property | Value |
|---|---|
| HP | 40 |
| Damage | 8 (Physical) |
| Attack Speed | 1 attack every 1.8 sec |
| Detection Range (Sight) | 15m |
| Detection Range (Sound) | 10m |
| Aggro Leash | 25m |
| Movement Speed | 600 cm/s |
| Behavior | Pack AI. Wolves spawn in groups of 2-3. One wolf circles the player (strafes at 8m), the others lunge in alternation. Lunges have a 1s telegraph (crouch, growl). After lunging, the wolf retreats to circling distance. If a wolf drops below 25% HP, it attempts to flee. |
| Pack Coordination | Use a shared Blackboard key: `AttackToken`. Only one wolf in a pack can hold the token at a time. The wolf with the token lunges. Others circle. Token passes every 2-3 seconds. This prevents all wolves from attacking simultaneously. |
| Loot | Leather Scraps (60%), Wolf Fang (20%), nothing (20%) |

#### Dred Minion (Level 2-3)

| Property | Value |
|---|---|
| HP | 25 |
| Damage | 6 (Physical) |
| Attack Speed | 1 attack every 1.5 sec |
| Detection Range (Sight) | 12m |
| Detection Range (Sound) | 8m |
| Aggro Leash | 25m |
| Movement Speed | 550 cm/s |
| Behavior | Swarm. Minions spawn in groups of 3-5. They rush the player directly (no circling). Each minion picks a slightly offset position around the player (front, left, right, behind) to avoid stacking on top of each other. Attacks are fast bites/claw swipes with minimal telegraph (0.5s). Individually trivial, dangerous in groups because they attack from all sides. Do NOT retreat or flee. Fight to the death. |
| Swarm Offset | On aggro, each Minion rolls a random offset angle (0, 72, 144, 216, 288 degrees) around the player and tries to approach from that angle. Recalculate every 3 seconds. |
| Loot | Nothing (70%), Chitin Shard (30%) |

#### Dred Soldier (Level 4-5)

| Property | Value |
|---|---|
| HP | 80 |
| Damage | 12 (Physical) |
| Attack Speed | 1 attack every 2.0 sec |
| Detection Range (Sight) | 15m |
| Detection Range (Sound) | 10m |
| Aggro Leash | 30m |
| Movement Speed | 400 cm/s |
| Behavior | Methodical. Approaches the player at walking speed. Enters melee range (2m). Attacks with 2-hit sword combos (1.5s telegraph each swing). **Blocks**: Raises shield between attacks. Player must use heavy attack to break guard or parry the Soldier's swings. If the player is at range (>8m) for more than 5 seconds, the Soldier charges (sprint speed 700 cm/s) and does a shield bash (knockback + 8 damage). If HP drops below 25%, the Soldier becomes more aggressive (attack speed increases 30%, stops blocking). |
| Loot | Iron Ore (40%), Crude Iron Sword (10%), Crude Iron Shield (10%), nothing (40%) |

#### Dred Woldred (Level 5-6)

| Property | Value |
|---|---|
| HP | 60 |
| Damage | 10 (Physical) |
| Attack Speed | 1 attack every 1.4 sec |
| Detection Range (Sight) | 18m |
| Detection Range (Sound) | 12m |
| Aggro Leash | 30m |
| Movement Speed | 700 cm/s |
| Behavior | Flanker. The Woldred circles the player at 10m, looking for an opening. When the player is attacking another enemy or mid-animation, the Woldred lunges from the side/behind (1.0s telegraph -- crouches, plates on back raise). After a lunge, it retreats to circling distance. If no opening after 5 seconds, lunges anyway. Fights to the death. |
| Loot | Armored Plate (30%), Leather Scraps (40%), nothing (30%) |

#### Nilvar Scout (Level 7-8, Dungeon)

| Property | Value |
|---|---|
| HP | 50 |
| Damage | 8 melee (Physical), 6 ranged (Nature -- poison dagger throw) |
| Detection Range (Sight) | 20m |
| Aggro Leash | 25m (dungeon enemies do not leash far -- corridors are narrow) |
| Movement Speed | 500 cm/s |
| Behavior | Ambusher. Starts hidden (crouched in foliage or on branches). First attack is always a poison dagger throw (6 damage + 3 Nature damage/sec DOT for 4 seconds). After the throw, closes to melee range and fights with dual daggers (fast, low damage). If the player gets within 3m while the Scout is still hidden, the Scout does a surprise melee attack (bonus 1.5x damage). Scouts do NOT block. They dodge sideways after every 3rd melee attack (0.3s i-frames, same as player). |
| Loot | Poisoned Dagger (15%), Dark Herbs (40%), Leather Scraps (30%), nothing (15%) |

#### Nilvar Warrior (Level 8-9, Dungeon)

| Property | Value |
|---|---|
| HP | 100 |
| Damage | 15 (Physical) |
| Detection Range (Sight) | 15m |
| Aggro Leash | 20m |
| Movement Speed | 380 cm/s |
| Behavior | Duelist. Fights 1-on-1 style. Approaches with shield raised (blocking by default). Attacks with 2-hit sword combos (1.2s telegraph). **Can parry the player**: 10% chance on each player light attack to parry (plays deflect animation, player is staggered for 1.0 sec). Will not parry heavy attacks. This teaches the player to use varied attack patterns -- not just spam light attacks. Retreats 3m after a successful parry, then advances again. Does not flee at low HP. |
| Loot | Corrupted Iron Sword (15%), Corrupted Iron Shield (15%), Iron Ore (40%), nothing (30%) |

#### Nilvar Shaman (Level 9, Dungeon)

| Property | Value |
|---|---|
| HP | 60 |
| Damage | 12 per spell (Nature) |
| Detection Range (Sight) | 20m |
| Aggro Leash | 20m |
| Movement Speed | 350 cm/s |
| Behavior | Caster. Stays at 10-15m range. Casts Thorn Lash every 4 seconds (same as the player spell -- cone, damage + slow). If the player closes to melee range (<3m), the Shaman attempts to retreat (backpedal + cast). Has no melee attack -- only spell casting. Very low HP; priority target when fighting mixed groups. If the Shaman's HP drops below 30%, it casts a self-heal (Mend equivalent, 20 HP over 3 seconds). The self-heal channel can be interrupted by hitting the Shaman. |
| Loot | Spell Tome: Thorn Lash (5% if player doesn't have it), Dark Herbs (50%), Staff (10%), nothing (35%) |

### Dungeon Mini-Boss: Corrupted Broodmother (Level 10)

| Property | Value |
|---|---|
| HP | 500 (5x standard enemy) |
| Phase Transition | At 50% HP (250 HP), summons 4 Spiderlings |

**Attack Table**:

| Attack | Damage | Type | Telegraph | Dodge Direction | Cooldown |
|---|---|---|---|---|---|
| Lunge Bite | 30 | Physical | Rears back + hisses (1.5s) | Sideways | 6 sec |
| Web Spit | 0 (applies 3s slow) | Nature | Head tilts, green glow at mandibles (1.0s) | Sideways or block | 8 sec |
| Leg Swipe | 15 | Physical | Raises front legs (0.8s) | Backward | 4 sec |
| Summon Spiderlings | N/A | N/A | Shrieks, egg sacs glow (2.0s) | N/A (clear adds) | Once at 50% HP |

**Behavior Tree (Boss)**:
```
Root (Selector)
├── Death Check
├── Phase Check (if HP <= 50% AND not summoned yet -> Summon Spiderlings)
├── Attack Selector (Sequence)
│   ├── Cooldown Check (pick available attack with lowest remaining CD)
│   ├── If player distance > 8m -> Web Spit (ranged)
│   ├── If player distance 3-8m -> Lunge Bite (gap close)
│   └── If player distance < 3m -> Leg Swipe (melee)
└── Reposition (move to maintain 5m distance if all attacks on CD)
```

**Spiderlings** (Level 8): HP 20, Damage 5, fast, melee only. Die in 2-3 hits. Distraction enemies -- the real threat is losing focus on the Broodmother while dealing with them.

**Punish Window**: After Lunge Bite, the Broodmother is stunned for 2.0 seconds (head stuck in ground animation). This is the primary DPS window. Teaches dodge-then-punish for future bosses.

**Loot**: Random iron-tier or better armor, 1 Dark Ore Fragment, 2 health potions.

### Dungeon Final Boss: The Risen Knight (Level 10)

| Property | Value |
|---|---|
| HP | 1000 (10x standard enemy) |
| Phase 1 | 100%-50% HP: Martial Combat |
| Phase 2 | 50%-0% HP: Corrupted Fury |
| Heal Channel | At 25% HP: channels for 8s, heals 20% HP if not interrupted |

**Phase 1 Attack Table**:

| Attack | Damage | Type | Telegraph | Dodge Direction | Cooldown |
|---|---|---|---|---|---|
| Overhead Slam | 35 | Physical | Raises sword high (2.0s), frontal cone AoE | Sideways | 5 sec |
| Sweep | 25 | Physical | Winds up horizontally (1.5s), 180-degree arc | Backward or dodge through | 4 sec |
| Shield Rush | 20 + knockback | Physical | Crouches, charges forward (1.0s telegraph, then 0.5s charge) | Sideways | 10 sec (used when player >8m) |

**Phase 1 Punish Window**: After every 3rd attack, the Risen Knight pauses for 2.0 seconds (recovers stance). This is the primary damage window.

**Phase 2 Changes** (triggered at 50% HP):
- Cutscene: Green light intensifies, vines erupt from armor. 3 seconds of invulnerability during transition.
- All Phase 1 attack telegraphs reduced by 0.5 seconds (Overhead Slam: 1.5s, Sweep: 1.0s, Shield Rush: 0.5s + 0.5s).
- Two new attacks added:

| Attack | Damage | Type | Telegraph | Dodge Direction | Cooldown |
|---|---|---|---|---|---|
| Vine Eruption | 20 | Nature | Ground cracks glow green in a line toward player (2.0s) | Sideways (perpendicular to line) | 7 sec |
| Dark Shout | 10 + knockback + 2s slow | Dark | Raises arms, dark aura pulses (1.5s), 8m AoE around boss | Run out of range or i-frame through | Once at 50%, once at 25% |

**Heal Channel at 25% HP**:
- The Risen Knight kneels and channels green energy (8 second channel).
- 4 Nilvar Scouts spawn around the arena.
- If the channel completes: Knight heals 200 HP (20%).
- Interrupt by dealing 100+ damage during the channel, OR kill the 4 Scouts (killing all Scouts also interrupts).
- If interrupted: Knight staggers for 3.0 seconds (extended punish window).

**Death Sequence**: Knight collapses. Vines wither. Green light fades. Whisper plays: *"This one returns to the earth. But I have many more."* -- Em'Faira's voice. No gameplay during death sequence (3 seconds).

**Loot**: 1 guaranteed rare (blue) armor piece (random slot), 1 Dark Ore Fragment, 500 gold, 500 XP.

---

## 8. PVP COMBAT ADJUSTMENTS

PvP is not in the vertical slice. These values are documented now so the combat system is designed with PvP compatibility. All PvP adjustments are applied as multipliers on top of PvE values -- no separate damage formulas.

### Damage Normalization

```
pvp_damage = pve_damage * 0.5
```

All player-vs-player damage is halved. This applies to melee, ranged, and spell damage. Without this, PvE-tuned burst damage would create instant kills in PvP.

### Stat Scaling in PvP

Stats have reduced impact in PvP to prevent gear gaps from being insurmountable:

```
pvp_stat_effect = base_stat_effect * 0.6
```

A player with Might 50 in PvP calculates damage as if they had Might 30. This compresses the power gap between undergeared and overgeared players.

### Resource Regeneration

| Resource | PvE Regen | PvP Regen | Rationale |
|---|---|---|---|
| HP | 1 + Fort/50 per sec | 3 + Fort/50 per sec | Faster HP regen prevents one-and-done fights. Encourages sustained engagements. |
| Stamina | 3% max/sec | 5% max/sec | More dodges and attacks per fight. More dynamic combat. |
| Mana | 5 + Sorc/10 per sec | 8 + Sorc/10 per sec | Casters can cast more often. Spell rotations matter more than mana hoarding. |

### Crowd Control Duration Caps

| CC Type | PvE Duration | PvP Max Duration |
|---|---|---|
| Root (Entangle) | 3 sec | 1.5 sec |
| Slow | 3 sec | 2.0 sec |
| Stagger (Parry) | 2.0 sec | 1.0 sec |
| Stun | N/A in current spells | 2.0 sec cap |
| Knockback | Full distance | 50% distance |

### Diminishing Returns on CC

In PvP, if the same CC type is applied to a player within 15 seconds of the last application:
- 2nd application: 50% duration
- 3rd application: 25% duration
- 4th+ application: immune for 15 seconds

This prevents CC-lock chains.

### PvP-Specific Design Notes

- **Parry in PvP**: The 200ms parry window is the same in PvP. A successful parry staggers the opponent for 1.0 sec (halved from PvE 2.0 sec). Riposte damage follows PvP normalization.
- **Dodge in PvP**: Same i-frame duration. Same stamina cost. No changes.
- **Spell interrupts**: Channeled spells (Mend) can be interrupted by damage in PvP. Instant casts cannot be interrupted (0.3s animation is too short to react to).
- **Healing in PvP**: Mend heals for 50% of its PvE value in PvP (15 HP over 3 seconds instead of 30). Prevents heal-tanking.

---

## 9. UE5 IMPLEMENTATION GUIDE

### Component Architecture

The player character (BP_PlayerCharacter) has the following components attached:

| Component | Type | Responsibility |
|---|---|---|
| **CombatComponent** | ActorComponent | Manages attack state, combo tracking, hit detection (sphere traces), damage output calculation, parry window timer, weapon data reference |
| **StaminaComponent** | ActorComponent | Tracks current/max stamina, regen rate, regen delay timer, exhaustion state, Haste stat integration |
| **HealthComponent** | ActorComponent | Tracks current/max HP, receives damage events, handles death, damage reduction calculation, Fortitude stat integration |
| **ManaComponent** | ActorComponent | Tracks current/max mana, regen rate, regen delay timer, Sorcery stat integration |
| **SpellComponent** | ActorComponent | Manages 5 spell slots, handles casting logic, cooldown timers, spell data references, spawns spell actors |
| **StatComponent** | ActorComponent | Stores the 5 core stats (Might, Fortitude, Haste, Agility, Sorcery), provides getter functions, handles level-up point allocation |

**Why separate components instead of one monolithic character Blueprint**: Each component is independently testable, reusable on NPCs/enemies (enemies get HealthComponent + a simplified CombatComponent), and keeps the character Blueprint event graph clean. A solo dev can work on the stamina system without touching spell logic.

### Gameplay Ability System: Use It or Not?

**Recommendation: Do NOT use GAS for the vertical slice.**

Rationale:
- GAS requires C++ setup for Ability System Component, Gameplay Effects, and Attribute Sets. This is a Phase 1/Blueprint-first project.
- GAS is designed for large teams with complex ability interactions. A solo dev implementing 5 spells and basic melee does not need it.
- The custom component approach (above) covers all vertical slice needs and is fully Blueprint-implementable.
- **Migration path**: If the project reaches Phase 4 (post-vertical slice, pre-full game), migrate the StatComponent to a GAS AttributeSet, convert spells to GameplayAbilities, and convert damage to GameplayEffects. The component interfaces can remain the same -- only the internals change. Design for this migration by keeping all combat values in DataTables (Section 10) so the data layer is already GAS-compatible.

### Animation Blueprint Structure

**AnimBP_PlayerCharacter**:

```
AnimGraph
├── Locomotion State Machine (full body base layer)
│   ├── Idle (blend by velocity)
│   ├── Walk/Run (blend space 1D by speed)
│   ├── Sprint (above sprint threshold)
│   ├── Dodge_Roll (triggered by bIsDodging, plays roll anim)
│   ├── Exhausted (triggered by bIsExhausted, heavy breathing)
│   └── Death (triggered by bIsDead)
├── DefaultSlot (Montage slot, layered over locomotion)
│   └── All attack montages, parry reactions, spell cast anims play here
└── Block Additive Layer (blended on top when bIsBlocking)
    └── Block pose (shield raised or weapon guard) blended at alpha 0-1
```

**Key AnimNotifies in attack montages**:
- `AN_TraceStart`: CombatComponent begins sphere tracing for hits
- `AN_TraceEnd`: CombatComponent stops tracing
- `AN_ComboWindowOpen`: Player can buffer/input the next combo attack
- `AN_ComboWindowClose`: Combo window expires, reset combo counter
- `AN_SpellRelease`: SpellComponent spawns the spell actor/applies effect
- `AN_CanDodgeCancel`: Player can dodge-cancel out of the current animation (for responsiveness)

**Enemy AnimBP**: Same structure but simpler. Locomotion state machine (Idle, Walk, Run, Chase) + DefaultSlot for attack montages. No combo system. Block is a state, not an additive blend.

### Hit Detection Implementation

Step-by-step Blueprint flow for a melee attack:

1. Player presses LMB -> Enhanced Input fires IA_LightAttack
2. CombatComponent checks: is the player in a state that allows attacking? (not exhausted, not mid-dodge, not stunned, has enough stamina)
3. CombatComponent deducts stamina (10 for light attack)
4. CombatComponent calls `PlayMontage` on the AnimInstance with the appropriate attack montage (based on equipped weapon type + combo counter)
5. During montage playback, `AN_TraceStart` fires -> CombatComponent sets `bIsTracing = true` and clears the `HitActors` array
6. On each Tick while `bIsTracing`:
   - Get weapon mesh socket locations: `Socket_WeaponBase` and `Socket_WeaponTip`
   - Fire `SphereTraceByChannel` from base to tip, radius 10cm, trace channel "Weapon"
   - If hit and hit actor is NOT in `HitActors` array:
     - Add hit actor to `HitActors`
     - Calculate damage: `base_weapon_damage * (1 + Might/100) * combo_multiplier`
     - Create a `DamageEvent` struct: `{Damage, DamageType, Instigator, HitResult}`
     - Call `ApplyDamage` on the hit actor (goes to their HealthComponent)
     - Spawn hit VFX at hit location (blood splash -- red for humanoids, black for Dred)
     - Apply hit-pause: 2-3 frame freeze on both attacker and target (set `CustomTimeDilation` to 0.0 for 0.05 seconds, then restore to 1.0)
7. `AN_TraceEnd` fires -> CombatComponent sets `bIsTracing = false`
8. `AN_ComboWindowOpen` fires -> CombatComponent sets `bComboWindowOpen = true`, checks input buffer
9. If player pressed LMB during the animation (buffered), advance combo counter and play next montage
10. `AN_ComboWindowClose` fires -> if no input received, reset combo counter to 0

### Damage Flow

```
Attacker (CombatComponent)
    |
    v
DamageEvent {amount, type, instigator, hit_result}
    |
    v
Target Actor -> HealthComponent.ReceiveDamage(DamageEvent)
    |
    ├── Check bIsInvulnerable (dodge i-frames) -> if true, ignore
    ├── Check ParryWindowActive (CombatComponent) -> if true, trigger parry logic
    ├── Check bIsBlocking -> if true, apply block reduction + stamina drain
    ├── Apply resistance reduction: damage -= resistance_value
    ├── Clamp to minimum 1
    ├── Subtract from CurrentHP
    ├── Fire OnDamageTaken event (UI listens to update HP bar)
    ├── Play hit reaction montage on target
    ├── If CurrentHP <= 0 -> Fire OnDeath event
    └── If target is enemy -> grant XP to attacker
```

**UI update**: The HUD widget binds to HealthComponent's `OnDamageTaken` and `OnHeal` events. When these fire, the HP bar widget updates. Floating damage numbers spawn as widget components at the hit location (world-space UMG widget, float upward and fade over 1 second).

---

## 10. DATA-DRIVEN VALUES

### Design Principle

Every tunable combat number lives in a DataTable. No magic numbers in Blueprints. When balance needs adjusting, open the DataTable, change a value, and the game updates without recompiling or editing Blueprint graphs.

### DataTable: DT_Weapons

**Row struct: S_WeaponData**

| Column | Type | Description | Example: Iron Sword | Example: Iron Greatsword |
|---|---|---|---|---|
| WeaponID | FName | Unique identifier | "Iron_Sword" | "Iron_Greatsword" |
| DisplayName | FText | UI display name | "Iron Sword" | "Iron Greatsword" |
| WeaponType | Enum (Sword, Axe, Mace, Greatsword, Glaive, Bow) | Determines montage set | Sword | Greatsword |
| SlotType | Enum (MainHand, OffHand, Back) | Which slot it equips to | MainHand | Back |
| BaseDamage | Float | Raw damage before stat scaling | 15.0 | 28.0 |
| AttackSpeed | Float | Montage play rate multiplier | 1.0 | 0.65 |
| StaminaCostLight | Float | Stamina per light attack | 10.0 | 15.0 |
| StaminaCostHeavy | Float | Stamina per heavy attack | 25.0 | 35.0 |
| ComboHits | Int | Number of light attacks in combo | 2 | 2 |
| HeavyMultiplier | Float | Damage multiplier for heavy finisher | 1.5 | 1.8 |
| TraceRadius | Float | Sphere trace radius (cm) | 10.0 | 14.0 |
| CanCleave | Bool | Hits multiple targets per swing? | false | true |
| BonusStat1 | Enum (Might, Fort, Haste, Agi, Sorc, None) | Random stat bonus 1 | Might | Might |
| BonusStat1Value | Int | Value of stat bonus 1 | 3 | 5 |
| BonusStat2 | Enum | Random stat bonus 2 | Fortitude | None |
| BonusStat2Value | Int | Value of stat bonus 2 | 2 | 0 |
| Rarity | Enum (Common, Uncommon, Rare) | Visual border color in UI | Common | Common |
| Mesh | SoftObjectPtr<UStaticMesh> | Weapon mesh reference | SM_IronSword | SM_IronGreatsword |
| LightMontage1 | SoftObjectPtr<UAnimMontage> | First light attack anim | AM_Sword_Light1 | AM_GS_Light1 |
| LightMontage2 | SoftObjectPtr<UAnimMontage> | Second light attack anim | AM_Sword_Light2 | AM_GS_Light2 |
| HeavyMontage | SoftObjectPtr<UAnimMontage> | Heavy finisher anim | AM_Sword_Heavy | AM_GS_Heavy |

### DataTable: DT_Enemies

**Row struct: S_EnemyData**

| Column | Type | Description | Example: Dred Minion | Example: Risen Knight |
|---|---|---|---|---|
| EnemyID | FName | Unique identifier | "Dred_Minion" | "Risen_Knight" |
| DisplayName | FText | UI name (for health bar) | "Dred Minion" | "The Risen Knight" |
| MaxHP | Float | Hit points | 25.0 | 1000.0 |
| BaseDamage | Float | Base attack damage | 6.0 | 35.0 |
| DamageType | Enum (Physical, Fire, Nature, Dark, Frost) | Primary damage type | Physical | Physical |
| AttackSpeed | Float | Seconds between attacks | 1.5 | 3.0 |
| MovementSpeed | Float | cm/s | 550.0 | 380.0 |
| DetectionRangeSight | Float | Meters | 12.0 | 20.0 |
| DetectionRangeSound | Float | Meters | 8.0 | 15.0 |
| AggroLeashDistance | Float | Meters | 25.0 | 50.0 (boss, doesn't leash easily) |
| DeAggroTime | Float | Seconds without sight to de-aggro | 8.0 | 30.0 |
| PhysicalResistance | Float | Flat damage reduction | 0.0 | 15.0 |
| ElementalResistance | Float | Flat elemental damage reduction | 0.0 | 10.0 |
| DarkResistance | Float | Flat dark damage reduction | 0.0 | 20.0 |
| CanBlock | Bool | Does this enemy block? | false | true |
| CanParry | Bool | Does this enemy parry? | false | false |
| FleeThresholdHP | Float | HP % at which enemy flees (0 = never) | 0.0 | 0.0 |
| IsBoss | Bool | Boss health bar, no leashing | false | true |
| XPReward | Int | XP granted on kill | 20 | 500 |
| BehaviorTree | SoftObjectPtr<UBehaviorTree> | AI behavior tree reference | BT_Swarm | BT_RisenKnight |
| LootTableID | FName | Reference to loot table | "LT_DredMinion" | "LT_RisenKnight" |

### DataTable: DT_Spells

**Row struct: S_SpellData**

| Column | Type | Description | Example: Firebolt | Example: Entangle |
|---|---|---|---|---|
| SpellID | FName | Unique identifier | "Firebolt" | "Entangle" |
| DisplayName | FText | UI name | "Firebolt" | "Entangle" |
| School | Enum (Flora, Earth, Fire, Water, Wild, Restoration, Temporal, Soul, Necromancy, Conjuration, Blood) | Magic school | Fire | Flora |
| LevelRequired | Int | Minimum player level to use | 1 | 5 |
| ManaCost | Float | Mana consumed on cast | 8.0 | 20.0 |
| Cooldown | Float | Seconds | 3.0 | 10.0 |
| BaseDamage | Float | Raw damage before Sorcery scaling | 25.0 | 0.0 |
| BaseHeal | Float | Raw heal amount (if applicable) | 0.0 | 0.0 |
| CastTime | Float | Seconds (0 = instant) | 0.0 | 0.5 |
| ChannelDuration | Float | Seconds (0 = not channeled) | 0.0 | 0.0 |
| TargetType | Enum (Projectile, Cone, GroundAOE, SelfCast, Beam) | How the spell is targeted | Projectile | GroundAOE |
| EffectRadius | Float | AoE radius in meters (0 for projectiles) | 0.0 | 4.0 |
| EffectRange | Float | Max cast range in meters | 50.0 | 15.0 |
| ConeAngle | Float | Degrees (for cone spells) | 0.0 | 0.0 |
| ProjectileSpeed | Float | cm/s (for projectiles) | 5000.0 | 0.0 |
| DOTDamage | Float | Damage per tick of DOT | 4.0 | 0.0 |
| DOTDuration | Float | Seconds of DOT | 3.0 | 0.0 |
| SlowPercent | Float | Movement slow applied to target | 0.0 | 0.0 |
| SlowDuration | Float | Seconds of slow | 0.0 | 0.0 |
| RootDuration | Float | Seconds target is rooted | 0.0 | 3.0 |
| IsInterruptible | Bool | Can damage cancel this cast? | true | true |
| CanMoveWhileCasting | Bool | Player can move during cast? | true | true (at 50% speed) |
| CastMontage | SoftObjectPtr<UAnimMontage> | Casting animation | AM_Cast_Projectile | AM_Cast_GroundAOE |
| VFX | SoftObjectPtr<UNiagaraSystem> | Spell visual effect | NS_Firebolt | NS_Entangle |
| Icon | SoftObjectPtr<UTexture2D> | Spell slot UI icon | T_Icon_Firebolt | T_Icon_Entangle |

### DataTable: DT_StaminaCosts

**Row struct: S_StaminaCost** (global combat tuning values)

| Parameter | Value | Notes |
|---|---|---|
| LightAttack_Base | 10.0 | Overridden by weapon-specific cost if set |
| HeavyAttack_Base | 25.0 | Overridden by weapon-specific cost if set |
| Dodge | 20.0 | Flat cost per dodge |
| Sprint_PerSec | 5.0 | Continuous drain |
| Block_DamageMultiplier | 0.5 | Stamina cost = incoming damage * this |
| Block_ShieldReduction | 0.7 | Shield reduces block stamina cost by this multiplier |
| Regen_BasePercent | 0.03 | 3% of max per second |
| Regen_DelaySeconds | 1.0 | Pause after stamina use |
| Exhaustion_Duration | 1.5 | Seconds of exhaustion on break |
| Exhaustion_RegenMultiplier | 0.5 | Regen rate during exhaustion |

### DataTable: DT_CombatTuning

**Row struct: S_CombatTuning** (global values that affect all combat)

| Parameter | Value | Notes |
|---|---|---|
| ParryWindow_Seconds | 0.2 | 200ms window |
| ParryStagger_Seconds | 2.0 | How long the parried enemy is staggered |
| Dodge_IFrames_Base | 0.3 | Seconds of invulnerability |
| Dodge_IFrames_AgilityScale | 500.0 | Divisor: i-frames = 0.3 + Agility / this |
| Dodge_IFrames_Cap | 0.5 | Maximum i-frame duration |
| Dodge_Recovery | 0.3 | Seconds before player can act after dodge |
| HitPause_Duration | 0.05 | Seconds of time dilation freeze on hit |
| ComboWindow_Seconds | 0.6 | Time after attack to input next combo hit |
| CritChance_Base | 0.05 | 5% base critical hit chance |
| CritChance_AgilityScale | 200.0 | Crit chance = base + Agility / this |
| CritDamage_Multiplier | 2.0 | Critical hits deal 2x damage |
| PvP_DamageMultiplier | 0.5 | PvP damage = PvE damage * this |
| PvP_StatScaling | 0.6 | PvP stat effectiveness multiplier |
| PvP_CCDurationMultiplier | 0.5 | PvP CC durations = PvE * this |

### How to Use DataTables in Blueprints

1. Create the row struct in Blueprint (or C++ struct marked with USTRUCT).
2. Create a DataTable asset using that struct.
3. Fill in rows for each weapon / enemy / spell.
4. In the CombatComponent, when the player attacks: look up the equipped weapon's row in DT_Weapons using its WeaponID. Read BaseDamage, StaminaCostLight, etc.
5. When an enemy spawns: the enemy Blueprint reads its row from DT_Enemies using its EnemyID. Sets MaxHP, MovementSpeed, detection ranges, etc.
6. When a spell is cast: the SpellComponent reads the spell's row from DT_Spells. Gets ManaCost, Cooldown, BaseDamage, TargetType, VFX reference, etc.

**Benefit**: To make swords faster, change one number in DT_Weapons. To make the Risen Knight harder, change his HP in DT_Enemies. No Blueprint editing required. The designer (you) can balance the game in a spreadsheet and import it.

---

## Appendix A: Quick Reference -- All Combat Numbers

### Player Base Values (Level 1)

| Resource | Base Value | Scaling Stat |
|---|---|---|
| HP | 100 + Fortitude * 2 | Fortitude |
| Stamina | 100 + Haste * 1 | Haste |
| Mana | 100 (flat, does not scale with stats in VS) | -- |
| HP Regen | 1 + Fortitude / 50 per sec | Fortitude |
| Stamina Regen | max_stamina * 0.03 * (1 + Haste / 100) per sec | Haste |
| Mana Regen | 5 + Sorcery / 10 per sec | Sorcery |
| Movement Speed | 600 * (1 + Haste / 200) cm/s | Haste |
| Sprint Speed | Movement Speed * 1.5 | Haste |

### Action Stamina Costs

| Action | Cost |
|---|---|
| Light Attack (Sword) | 10 |
| Light Attack (Axe/Mace) | 12 |
| Light Attack (Greatsword) | 15 |
| Light Attack (Glaive) | 10 |
| Heavy Attack (Sword) | 25 |
| Heavy Attack (Axe) | 28 |
| Heavy Attack (Mace) | 30 |
| Heavy Attack (Greatsword) | 35 |
| Heavy Attack (Glaive) | 25 |
| Dodge Roll | 20 |
| Sprint | 5/sec |
| Block (per hit) | incoming_damage * 0.5 (* 0.7 with shield) |
| Parry (successful) | 0 |

### Weapon Damage Table (Iron Tier)

| Weapon | Base Damage | Speed Mult | DPS (approx at 1.0 attack speed) |
|---|---|---|---|
| Sword (1H) | 15 | 1.0x | ~15/sec |
| Axe (1H) | 18 | 0.85x | ~15.3/sec |
| Mace (1H) | 20 | 0.8x | ~16/sec |
| Greatsword (2H) | 28 | 0.65x | ~18.2/sec |
| Glaive (2H) | 22 | 0.85x | ~18.7/sec |
| Bow (Full Draw) | 20 | N/A (1.0s draw) | ~20/sec (theoretical) |

DPS values are approximate. Actual DPS depends on hit-confirm rate, dodging, and positioning. Greatsword and Glaive have higher theoretical DPS but require two hands (no shield) and have slower recovery, making them higher-risk.

### Spell Quick Reference

| Spell | Mana | CD | Damage | Heal | Effect |
|---|---|---|---|---|---|
| Firebolt | 8 | 3s | 25 + 12 DOT | -- | Burn 4/s for 3s |
| Mend | 15 | 8s | -- | 30 | 10/s over 3s channel |
| Frost Shard | 10 | 3s | 20 | -- | 30% slow 3s |
| Thorn Lash | 10 | 4s | 18 | -- | 20% slow 2s, cone |
| Entangle | 20 | 10s | 0 | -- | Root 3s, ground AoE |

### Enemy HP Quick Reference (Vertical Slice)

| Enemy | HP | Damage | Level |
|---|---|---|---|
| Wild Boar | 30 | 5 | 1-2 |
| Wolf | 40 | 8 | 3 |
| Dred Minion | 25 | 6 | 2-3 |
| Dred Soldier | 80 | 12 | 4-5 |
| Dred Woldred | 60 | 10 | 5-6 |
| Nilvar Scout | 50 | 8 melee / 6 ranged | 7-8 |
| Nilvar Warrior | 100 | 15 | 8-9 |
| Nilvar Shaman | 60 | 12 (spell) | 9 |
| Corrupted Broodmother | 500 | 30 (lunge) | 10 (boss) |
| The Risen Knight | 1000 | 35 (overhead) | 10 (boss) |

---

## Appendix B: Implementation Checklist

Build order aligned with the vertical slice milestones (M2, M3, M10 from vertical-slice-v2.md):

**M2 (Basic Melee Combat) -- Build these first**:
- [ ] CombatComponent with combo counter and state tracking
- [ ] StaminaComponent with drain, regen, delay, and exhaustion
- [ ] HealthComponent with damage reception and death
- [ ] Light attack montages for Sword (2-hit combo)
- [ ] Heavy attack montage for Sword
- [ ] AnimNotifies: AN_TraceStart, AN_TraceEnd, AN_ComboWindowOpen, AN_ComboWindowClose
- [ ] Sphere trace hit detection during attack windows
- [ ] Block (RMB hold, additive pose, stamina drain on hit)
- [ ] Parry (200ms window check, stagger response)
- [ ] Dodge (Shift, i-frames, stamina cost)
- [ ] HUD: HP bar, Stamina bar
- [ ] Hit pause (2-3 frame freeze on contact)
- [ ] DT_Weapons with Iron Sword row
- [ ] DT_StaminaCosts with base values
- [ ] DT_CombatTuning with parry/dodge/combo values

**M3 (Enemy AI) -- Build these second**:
- [ ] Base enemy Blueprint with HealthComponent
- [ ] Behavior Tree: shared base (Idle, Patrol, Detect, Chase, Attack, Die)
- [ ] Perception Component (sight + sound)
- [ ] Aggro leashing and de-aggro timer
- [ ] Dred Minion behavior (swarm, offset positions)
- [ ] Dred Soldier behavior (block, charge if player at range)
- [ ] Wolf pack behavior (AttackToken shared Blackboard key)
- [ ] DT_Enemies with rows for Boar, Wolf, Minion, Soldier
- [ ] Hit reaction montages on enemies
- [ ] Enemy death (ragdoll or death anim + loot drop)
- [ ] XP grant on kill

**M10 (Spell System) -- Build these third**:
- [ ] ManaComponent with regen and delay
- [ ] SpellComponent with 5 slots, cooldown tracking, cast state
- [ ] DT_Spells with 5 vertical slice spell rows
- [ ] Spell casting montages (projectile cast, channel, ground AoE targeting)
- [ ] AN_SpellRelease AnimNotify
- [ ] BP_SpellProjectile (Firebolt, Frost Shard) with projectile movement and collision
- [ ] Cone trace for Thorn Lash
- [ ] Ground AoE targeting decal + effect actor for Entangle
- [ ] Self-heal channel for Mend
- [ ] Mana bar on HUD
- [ ] Spell slot icons on HUD with cooldown overlay
- [ ] Spell tome pickup (world item -> teaches spell)

**M11 (Dungeon Bosses) -- Build these fourth**:
- [ ] BT_Broodmother: phase check, attack selection, summon spiderlings
- [ ] Broodmother attacks: Lunge Bite, Web Spit, Leg Swipe
- [ ] Broodmother punish window (2s stun after Lunge Bite)
- [ ] BT_RisenKnight: 2-phase behavior, attack rotation, heal channel
- [ ] Risen Knight Phase 1: Overhead Slam, Sweep, Shield Rush, 3-attack punish window
- [ ] Risen Knight Phase 2: transition cutscene, faster telegraphs, Vine Eruption, Dark Shout
- [ ] Risen Knight heal channel at 25% + Scout spawns + interrupt logic
- [ ] Boss health bar (UMG widget, appears at top of screen)
- [ ] Boss loot drops (guaranteed rare + Dark Ore Fragment)
