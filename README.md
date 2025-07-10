# True Faction System (TFS)

> ⚠️ **Warning: This mod is still a WIP and might break some quests. Use with caution!**
>
> [![Download Latest Release](https://img.shields.io/github/v/release/Dagait/TES5-TrueFactionSystem)](https://github.com/Dagait/TES5-TrueFactionSystem/releases/latest)

<p align="center">
    <img src="https://github.com/user-attachments/assets/11b815a0-8bae-4a8a-8f2d-9a6bc0b2d119" width="400" />
</p>


The **True Faction System** is an SKSE plugin for Skyrim Special Edition that allows the player to act as a member of various factions depending on the armor worn. The plugin uses dynamic faction recognition based on faction armors and adjusts NPC behavior accordingly.

If you just want to do a quick test, there's a test cell where each room has its own faction NPCs. At the start, you'll find a barrel with important items for the test.
```
coc npeTFSTestCell
```

## Features

- **Dynamic Faction Affiliation**: When the player wears armor associated with a specific faction (e.g., Bandits, Imperials, Stormcloaks), they are recognized as an ally by NPCs of that faction and are not attacked.

<div align="center">

| **Faction**         | **Tag**              | **Working** | **Reason** |
|---------------------|----------------------|:---:|----------------------|
| Bandit Faction      | npeBanditFaction         | <ul><li>[x] </li></ul> | |
| Imperial Faction    | npeImperialFaction       | <ul><li>[x] </li></ul> | |
| Blades Faction      | npeBladesFaction         | <ul><li>[x] </li></ul> | |
| Companions Faction  | npeCompanionsFaction     | <ul><li>[x] </li></ul> | |
| Dawnguard Faction   | npeDawnguardFaction      | <ul><li>[ ] </li></ul> | Beacuse of the individual load order |
| Dawnstar Faction    | npeDawnstarFaction       | <ul><li>[x] </li></ul> | |
| Falkreath Faction   | npeFalkreathFaction      | <ul><li>[x] </li></ul> | |
| Forsworn Faction    | npeForswornFaction       | <ul><li>[x] </li></ul> | |
| Markarth Faction    | npeMarkarthFaction       | <ul><li>[x] </li></ul> | |
| Morthal Faction     | npeMorthalFaction        | <ul><li>[x] </li></ul> | |
| Nightingale Faction | npeNightingaleFaction    | <ul><li>[x] </li></ul> | |
| Riften Faction      | npeRiftenFaction         | <ul><li>[x] </li></ul> | |
| Solitude Faction    | npeSolitudeFaction       | <ul><li>[x] </li></ul> | |
| Solstheim Faction   | npeSolstheimFaction      | <ul><li>[ ] </li></ul> | Beacuse of the individual load order |
| Stormcloaks Faction | npeStormcloaksFaction    | <ul><li>[x] </li></ul> | |
| Whiterun Faction    | npeWhiterunFaction       | <ul><li>[x] </li></ul> | |
| Windhelm Faction    | npeWindhelmFaction       | <ul><li>[x] </li></ul> | |
| Winterhold Faction  | npeWinterholdFaction     | <ul><li>[x] </li></ul> | |
| Thalmor Faction     | npeThalmorFaction        | <ul><li>[x] </li></ul> | |
| Silverhand Faction     | npeSilverHandFaction        | <ul><li>[x] </li></ul> | |

Table 1: Currently implemented factions and their corresponding armor tags

</div>
There is also an exclusive tag called "npeCoveredFace". This means that the player's disguise is at 100.0 and cannot be detected (exposed) until he attacks the faction or disguises himself.
  
- **Multiple Faction Support**: The player can wear armor pieces from different factions simultaneously and will be recognized as an ally by NPCs from multiple factions accordingly.

- **Detection Based on Disguise Value**: The likelihood of NPCs recognizing the player as an enemy depends on the disguise value of the armor worn and proximity to the NPCs. If the Disguise value is less than 5.0, the faction will be removed. A disguise value of 100.0 would be perfect disguise, which can be achieved by wearing a full outfit (such as outfits that cover the player's face).

<div align="center">

| **Armor Slot**   | **Disguise Value (Weight)** |
|------------------|:---:|
| Chest Armor      | 40.0                        |
| Helmet           | 27.0                        |
| Gloves           | 4.0                         |
| Forearm Armor    | 8.0                         |
| Shoes/Boots      | 5.0                         |
| Circlet          | 1.0                         |

Table 2: Armor slot - Disguise value representation

</div>

<div align="center">

| **Amount of armor pieces**   | **Bonus (Weight)** |
|:------------------:|:---:|
| >= 4      | 20.0                        |
| >= 3           | 10.0                        |
| >= 2           | 5.0                         |

Table 3: Armor pieces and bonus

</div>

- **Improved Detection Logic**: The detection system takes into account: NPC level, line of sight, the surrounding environment (only time of day for now), in addition to proximity and disguise value.

- **Background Check**: A background process regularly checks (every 2 seconds for now) faction affiliation and detection probability without requiring the player to change their armor.


## Detection Probability Formula

The detection probability is calculated based on the player's distance from NPCs and the player's disguise value. The probability decreases smoothly with distance and is determined by the following formula:

```math
\text{distanceFactor} = \frac{1.0}{1.0 + \exp((\text{distance} - \text{DETECTION}_\text{RADIUS}) \cdot 0.1)}
```


This creates a smooth transition in the detection probability as the player gets further from the NPC. The graph below illustrates how detection probability decreases with distance:

![Detection Probability vs. Distance](images/tfs_comparison.png)

## Field of View (FoV) Detection

The NPC's ability to detect the player (expose the players disguise) is also influenced by their **Field of View (FoV)**. The FoV is the angular range in which the NPC can observe the player. If the player is within this angular range, detection is possible.

The **FoV detection** is calculated using the **dot product** between the NPC's forward direction and the vector pointing from the NPC to the player. This determines the angle between these vectors, and if the angle is smaller than half of the defined FoV (e.g., 45° for a 90° field of view), the player is considered to be visible.

$$\cos(\theta) = \frac{\vec{npcForward} \cdot \vec{npcToPlayer}}{|\vec{npcForward}| |\vec{npcToPlayer}|}$$

Where:
- $\theta$ is the angle between the NPC's forward direction and the vector pointing to the player.
- $\vec{npcForward}$ is the normalized vector representing the NPC's facing direction.
- $\vec{npcToPlayer}$ is the normalized vector from the NPC to the player.

The angle $\theta$ between the two vectors is computed using the inverse cosine function:

$$\theta = \cos^{-1}(\text{dotProduct})$$

If the angle $\theta$ is less than or equal to half of the field of view, the player is within the NPC's line of sight and detection becomes possible:

$$\theta \leq \frac{\text{fieldOfViewDegrees}}{2}$$

## Installation Instructions

1. [**SKSE64**](https://skse.silverlock.org/) (1.6.1170) must be installed.
2. [SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604) must also be installed, because of the new implemented MCM.
3. [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444) must also be installed.
4. Copy all the files  into your `Skyrim Special Edition/Data/` folder.
5. Start the game and test the plugin by wearing faction armors and approaching the corresponding faction NPCs.


## Usage Instructions

- **Wearing Faction Armor**: When you wear armor from a faction (e.g., Bandit Armor), you can move undetected within that faction's camps. NPCs will only attack you if they recognize that you do not truly belong to their faction.

- **Mixing Faction Armors**: You can combine armor pieces from different factions. The stealth value for each faction is calculated individually based on the worn armor pieces belonging to that faction.

- **Approaching NPCs**: The closer you get to NPCs, the higher the chance they will recognize you as an enemy, especially if your stealth value for their faction is low.

- **Switching Gear**: The stealth value is automatically updated when you equip or unequip items. An event handler monitors equipment changes and adjusts faction affiliation accordingly.

## Development and Future Goals

### Current Features:

- Extended faction detection: Support for multiple factions simultaneously, based on worn armor pieces.
- Dynamic stealth value: Calculation of stealth value for each faction individually, depending on worn faction armors.
- Background process: Automatic checking of faction affiliation and detection probability every 5 seconds without player intervention.
- Combat system: NPCs will attack the player when they recognize them as an enemy. Attacking faction NPCs removes the player from that faction.

### Planned Features:

- [ ] **Quest integration**: Possible integration into existing quests to adjust NPC behavior based on the player’s disguise.
- [ ] **Mod support**: Support for custom faction armors and factions from mods (MCM) (Partially implemented, looking for a smarter way in doing this).
- [x] **Configurable settings**: Allow players to customize settings such as check intervals, stealth thresholds, and more.
- [x] **Location-Based NPC Memory**: NPCs will have the ability to remember the player's appearance based on the location. If the player is recognized in a certain area, the disguise may not work when they return to that location, making it harder to fool the same NPCs again. Currently, the NPC's FormID is stored for 2 hours of in-game time, during which time there is a higher probability that the NPC will be able to detect the player's disguise again.
- [x] **Race-Based Disguise**: Certain races will receive bonuses or penalties when wearing faction armors. For example, High Elves (Altmer) may have an easier time disguising as Thalmor agents, while Orcs or Nords may find it more difficult to pass unnoticed in Imperial factions. This adds an additional layer of immersion and strategy, as the player's race will influence how effectively they can blend in with different factions.

## Known Issues

- ~~**Faction change delay**: There may be a slight delay before the player's faction affiliation is updated, as the check occurs at intervals.~~
- ~~**Detection logic**: Currently, the detection probability is primarily based on stealth value and distance. Additional factors are not yet implemented.~~
- **Compatibility with other mods**: The plugin may conflict with mods that alter factions or armor keywords. Comprehensive mod compatibility is planned (Partially implemented).

# New Native Functions
See [Wiki](https://github.com/Dagait/TES5-TrueFactionSystem/wiki/API-Documentation) for more information.
