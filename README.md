# Reven Lore Studio

Multi-agent AI orchestration system for building the Reven canon and writing book-ready story material.

## Status

**Current Phase: Lore Phase 1 — Canon Foundation & Series Architecture**
**Project Mode: Lore-first authoring (game development paused)**

## Current Narrative Direction

The project has pivoted to a writing-first canon program centered on:

- A compendium arc of four books:
    - Before the Ages
    - Dawn of Mortals
    - The First Mayrn War
    - Mayrn Diaspora
- A first trilogy anchored by Shadow Conquest:
    - Shadow Conquest
    - The Magic Wars
    - Wicked War
- A third-age follow-on trilogy beginning with TechnoMagic (working title)

Game development remains intentionally paused until the lore and book-era canon are stabilized.

## Repository Structure

```
ai-studio/                  # Orchestration layer (agent defs, pipelines, rules)
├── master_orchestrator.yaml
├── approval_gates.yaml
├── context_ingestion.yaml
├── lore_protection_rules.yaml
└── pipelines/
    ├── lore_books_pipeline.yaml
    ├── research_pipeline.yaml
    ├── design_pipeline.yaml
    ├── development_pipeline.yaml
    ├── testing_pipeline.yaml
    ├── multiplayer_security_pipeline.yaml
    ├── management_pipeline.yaml
    └── verification_pipeline.yaml

docs/                       # All documents (lore, books, processing)
├── raw/                    # Drop messy notes here
├── processing/             # Agent work-in-progress
├── canon/                  # Approved lore (human-approved only)
├── non-canon/              # Rejected ideas (kept for reference)
├── unresolved/             # Needs human decision
├── design/                 # Legacy game design specs (on hold)
└── books/                  # Book architecture, outlines, and manuscripts

research/                   # Decision documents from Research Team
src/                        # Game source code (empty until Phase 3)
data/                       # Game data files (empty until Phase 3)
tests/                      # Test code (empty until Phase 3)
reviews/                    # Code review outputs
testing/                    # QA reports (bugs, perf, balance)
multiplayer/                # MP design docs and audits (empty until Phase 6)
```

## How to Use

### Adding Lore / Notes
Drop any file into `docs/raw/`. Any format, any state of completeness.
Then ask Copilot to process it using the context ingestion pipeline.

### Adding Book Outlines
Drop chapter notes, scene ideas, or whole-book outlines into `docs/raw/`.
Tag the filename with the target book when possible (example: `Shadow-Conquest-outline-v1.md`).
Then run the lore books pipeline to:
- extract claims and timeline anchors,
- run canon and contradiction checks,
- and generate an editable manuscript planning pack.

See `docs/books/reven-compendium-program.yaml` for the master series map.

### All Major Decisions Require Human Approval
See `ai-studio/approval_gates.yaml` for the complete list.

### Scope Filter
Every proposed feature is scored on 6 dimensions (1-5 each).
- Score <= 15: BUILD
- Score 16-20: DEFER
- Score > 20: CUT (unless critical to game identity)

## Pause Notice

- Current priority: canon and books
- Current hold: gameplay systems, vertical-slice implementation, multiplayer, and release planning
