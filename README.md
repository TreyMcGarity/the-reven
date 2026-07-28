# AI Game Dev Studio

Multi-agent AI orchestration system for solo-developing a 3D fantasy game.

## Status

**Current Phase: 0 — Studio Setup**
**Engine: Unreal Engine**

## Repository Structure

```
ai-studio/                  # Orchestration layer (agent defs, pipelines, rules)
├── master_orchestrator.yaml
├── approval_gates.yaml
├── context_ingestion.yaml
├── lore_protection_rules.yaml
└── pipelines/
    ├── research_pipeline.yaml
    ├── design_pipeline.yaml
    ├── development_pipeline.yaml
    ├── testing_pipeline.yaml
    ├── multiplayer_security_pipeline.yaml
    ├── management_pipeline.yaml
    └── verification_pipeline.yaml

docs/                       # All documents (lore, design, processing)
├── raw/                    # Drop messy notes here
├── processing/             # Agent work-in-progress
├── canon/                  # Approved lore (human-approved only)
├── non-canon/              # Rejected ideas (kept for reference)
├── unresolved/             # Needs human decision
└── design/                 # Approved game design specs

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
Then ask Claude to process it using the context ingestion pipeline.

### All Major Decisions Require Human Approval
See `ai-studio/approval_gates.yaml` for the complete list.

### Scope Filter
Every proposed feature is scored on 6 dimensions (1-5 each).
- Score <= 15: BUILD
- Score 16-20: DEFER
- Score > 20: CUT (unless critical to game identity)
