# Reven Fine-Tuning Pipeline

This pipeline prepares a supervised fine-tuning (SFT) dataset from repo canon and trains a LoRA adapter.

## Important
- Fine-tuning is optional.
- On older machines, RAG + guardrails may be more efficient than training.
- GPU is strongly recommended for training.

## Files
- `configs/lora_train.yaml`: training and LoRA settings
- `scripts/build_sft_dataset.py`: builds `data/reven_sft.jsonl` from `docs/canon/INDEX.yaml`
- `scripts/train_lora.py`: runs LoRA SFT training
- `requirements-finetune.txt`: training dependencies

## Steps

1. Install training dependencies:
```powershell
cd lore-llm
pip install -r finetune\requirements-finetune.txt
```

2. Build dataset:
```powershell
python finetune\scripts\build_sft_dataset.py
```

3. Train LoRA adapter:
```powershell
python finetune\scripts\train_lora.py
```

## Output
- Adapter and tokenizer artifacts under `finetune/artifacts/reven-lora`.

## Notes on Local Use
- This scaffold checks for CUDA and aborts if no GPU is detected.
- If no GPU is available, use the existing local Ollama + RAG pipeline instead.
