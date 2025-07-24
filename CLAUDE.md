# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Project_RS is an Unreal Engine 5.5 third-person action game built in C++. The project features character combat systems, AI behavior, and item management mechanics.

## Build and Development Commands

### Building the Project
```bash
# Open project in Unreal Editor (from Windows)
start Project_RS.uproject

# Build from command line (requires UE5 installed)
"C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" Project_RS Win64 Development -Project="D:\Projects\Project_RS\Project_RS.uproject"
```

### Generating Project Files
```bash
# Regenerate Visual Studio project files
"C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="D:\Projects\Project_RS\Project_RS.uproject" -game -rocket -progress
```

## Architecture Overview

### Core Systems

**Character System** (`Source/Project_RS/Character/`)
- `ARSCharacterBase`: Base character class with combat, stats, and UI integration
- `ARSCharacterPlayer`: Player-controlled character with input handling
- `ARSCharacterNonPlayer`: AI-controlled characters (NPCs/enemies)

**AI System** (`Source/Project_RS/AI/`)
- `ARSAIController`: Main AI controller using Behavior Trees and Blackboards
- Behavior Tree tasks: Attack, Patrol, Detection, Movement
- Custom decorators and services for combat ranges and target detection

**Combat System**
- Attack combo system with montage-based animations
- Data-driven attack configurations via `URSAttackActionData`
- Hit detection through animation notifies (`AnimNotify_AttackHitCheck`)

**Stats & Progression** (`Source/Project_RS/CharacterStat/`)
- `URSCharacterStatComponent`: Health, damage, and character stats
- Data-driven character stats loaded from CSV files (`GameData/RSCharacterStatTable.csv`)
- Game singleton pattern for global data access (`URSGameSingleton`)

**Item System** (`Source/Project_RS/Item/`)
- Base item classes with data assets
- Medical items for healing mechanics
- Interface-based design for character-item interactions

### Key Architectural Patterns

1. **Interface-Driven Design**: Extensive use of UE interfaces for loose coupling
   - `IRSWidgetInterface`: UI integration
   - `IRSAnimationAttackInterface`: Animation callbacks
   - `IRSCharacterAIInterface`: AI communication
   - `IRSCharacterItemInterface`: Item interactions

2. **Data-Driven Configuration**: 
   - Character stats from CSV files
   - Control schemes via data assets (`URSCharacterControlData`)
   - Attack configurations through data assets

3. **Component-Based Architecture**:
   - `URSCharacterStatComponent`: Character statistics
   - `URSWidgetComponent`: UI elements attached to characters

## File Organization

### C++ Source Structure
```
Source/Project_RS/
├── Character/          # Character classes (base, player, NPC)
├── AI/                 # AI controllers, behavior trees, tasks
├── Animation/          # Animation instances, notifies
├── CharacterStat/      # Stat management components
├── Game/               # Game mode and core game systems
├── GameData/           # Singleton and data management
├── Interface/          # UE interfaces for system communication
├── Item/               # Item system classes
├── Player/             # Player controller
├── UI/                 # Widget classes and UI components
├── Physics/            # Collision and physics definitions
└── Enums/              # Game-specific enumerations
```

### Content Structure
```
Content/Project_RS/
├── Maps/               # Game levels
├── Blueprint/          # Blueprint classes
├── Animation/          # Animation assets, montages
├── CharacterAction/    # Character action data assets
├── GameData/           # Data tables and configuration
├── AI/                 # Behavior trees, blackboards
└── UI/                 # Widget blueprints
```

## Development Guidelines

### Building Characters
- Inherit from `ARSCharacterBase` for all character types
- Use `URSCharacterStatComponent` for health/stats management
- Implement required interfaces based on character capabilities
- Configure character data in `GameData/RSCharacterStatTable.csv`

### AI Development
- Extend `ARSAIController` for custom AI behaviors
- Create custom behavior tree tasks inheriting from UE base classes
- Use blackboard keys for AI state management
- Implement `IRSCharacterAIInterface` for character-AI communication

### Animation Integration
- Use `URSAnimInstance` as base for character animations
- Implement attack hit detection via `AnimNotify_AttackHitCheck`
- Create montages for combat actions and state transitions

### Custom Collision Channels
- `RSAction`: Custom trace channel for gameplay interactions
- `RSCapsule`: Character capsule collision profile
- `RSTrigger`: Trigger volume collision profile

## Engine Configuration

- **Engine Version**: Unreal Engine 5.5
- **Target Platform**: Windows (primary), with DirectX 12 rendering
- **Key Modules**: Core, Engine, AIModule, UMG, EnhancedInput, NavigationSystem
- **Rendering**: Lumen for dynamic lighting, ray tracing enabled
- **Default Game Mode**: `ARSGameMode` with custom game singleton

## Data Management

Character statistics are managed through:
1. CSV data files in `GameData/` directory
2. Runtime access via `URSGameSingleton::Get().GetCharacterStat()`
3. Component-based stat management through `URSCharacterStatComponent`

When adding new characters, update the CSV file and corresponding enum values in `ECharacterName.h`.