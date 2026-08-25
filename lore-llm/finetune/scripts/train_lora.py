from __future__ import annotations

import json
from pathlib import Path
from typing import Any

import yaml


def load_config(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as f:
        return yaml.safe_load(f) or {}


def require_torch_cuda() -> None:
    import torch

    if not torch.cuda.is_available():
        raise RuntimeError(
            "CUDA GPU was not detected. LoRA fine-tuning is likely too slow on CPU for practical use. "
            "Use a GPU machine, smaller model, or skip fine-tuning and rely on RAG."
        )


def load_jsonl_dataset(path: Path) -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    with path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            rows.append(json.loads(line))
    return rows


def main() -> None:
    root = Path(__file__).resolve().parents[2]
    config_path = root / "finetune" / "configs" / "lora_train.yaml"
    cfg = load_config(config_path)

    require_torch_cuda()

    from datasets import Dataset
    from peft import LoraConfig, get_peft_model
    from transformers import AutoModelForCausalLM, AutoTokenizer, BitsAndBytesConfig, TrainingArguments
    from trl import SFTTrainer

    base_model = cfg["base_model"]
    dataset_path = root / cfg["dataset_path"]
    output_dir = root / cfg["output_dir"]

    rows = load_jsonl_dataset(dataset_path)
    if not rows:
        raise RuntimeError(f"No rows found in dataset: {dataset_path}")

    ds = Dataset.from_list(
        [
            {
                "text": f"<|system|>{r['system']}\n<|user|>{r['user']}\n<|assistant|>{r['assistant']}"
            }
            for r in rows
        ]
    )

    runtime = cfg.get("runtime", {})
    use_4bit = bool(runtime.get("use_4bit", True))

    quant_config = None
    if use_4bit:
        quant_config = BitsAndBytesConfig(
            load_in_4bit=True,
            bnb_4bit_quant_type="nf4",
            bnb_4bit_use_double_quant=True,
            bnb_4bit_compute_dtype="bfloat16",
        )

    tokenizer = AutoTokenizer.from_pretrained(base_model, use_fast=True)
    if tokenizer.pad_token is None:
        tokenizer.pad_token = tokenizer.eos_token

    model = AutoModelForCausalLM.from_pretrained(
        base_model,
        quantization_config=quant_config,
        device_map="auto",
    )

    lora_cfg = cfg["lora"]
    peft_cfg = LoraConfig(
        r=int(lora_cfg["r"]),
        lora_alpha=int(lora_cfg["alpha"]),
        lora_dropout=float(lora_cfg["dropout"]),
        target_modules=list(lora_cfg["target_modules"]),
        bias="none",
        task_type="CAUSAL_LM",
    )
    model = get_peft_model(model, peft_cfg)

    tr_cfg = cfg["training"]
    args = TrainingArguments(
        output_dir=str(output_dir),
        per_device_train_batch_size=int(tr_cfg["batch_size"]),
        gradient_accumulation_steps=int(tr_cfg["gradient_accumulation_steps"]),
        num_train_epochs=float(tr_cfg["epochs"]),
        learning_rate=float(tr_cfg["learning_rate"]),
        logging_steps=int(tr_cfg["logging_steps"]),
        save_steps=int(tr_cfg["save_steps"]),
        eval_steps=int(tr_cfg["eval_steps"]),
        warmup_ratio=float(tr_cfg["warmup_ratio"]),
        fp16=bool(runtime.get("fp16_fallback", True)),
        bf16=bool(runtime.get("bf16_if_available", True)),
        report_to="none",
    )

    trainer = SFTTrainer(
        model=model,
        tokenizer=tokenizer,
        train_dataset=ds,
        dataset_text_field="text",
        max_seq_length=int(tr_cfg["max_seq_length"]),
        args=args,
    )

    trainer.train()
    trainer.model.save_pretrained(str(output_dir))
    tokenizer.save_pretrained(str(output_dir))

    print(f"Saved LoRA adapter to {output_dir}")


if __name__ == "__main__":
    main()
