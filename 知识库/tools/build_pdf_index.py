#!/usr/bin/env python3
"""Build reproducible PDF manifests and category indexes without modifying sources."""

from __future__ import annotations

import csv
import hashlib
import json
import logging
import re
from collections import Counter, defaultdict
from datetime import datetime
from pathlib import Path

from pypdf import PdfReader


ROOT = Path(__file__).resolve().parents[2]
KB = ROOT / "知识库"
INDEX_DIR = KB / "索引"
CATEGORY_ORDER = ("开发板", "芯片", "模块", "比赛规则")

logging.getLogger("pypdf").setLevel(logging.ERROR)


def file_sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest().upper()


def pdf_date(value: str) -> str:
    match = re.search(r"(?:D:)?(\d{4})(\d{2})(\d{2})", value or "")
    if not match:
        return ""
    return f"{match.group(1)}-{match.group(2)}-{match.group(3)}"


def clean_text(value: str, limit: int) -> str:
    return re.sub(r"\s+", " ", value or "").strip()[:limit]


def categories_for(rel: str) -> list[str]:
    categories: list[str] = []
    name = Path(rel).name.lower()

    is_competition = (
        rel.startswith("历年赛题/")
        or rel.startswith("2025竞赛题目列表（本科）/")
        or rel.startswith("2025竞赛题目列表（高职高专）/")
        or rel == "电赛材料.pdf"
    )
    is_motor_module = rel.startswith("双路电机驱动模块/")
    is_gimbal_module = rel.startswith("(CODBOT-SG02)简易二自由度云台/")

    board_roots = {
        "boostxl-drv8301 硬件用户指南.pdf",
        "launchxl-f28027 c2000.pdf",
        "msp-lito-g3507 evaluation module.pdf",
        "mspm0g3507.pdf",
    }
    chip_roots = {
        "3507.pdf",
        "boostxl-drv8301 硬件用户指南.pdf",
        "launchxl-f28027 c2000.pdf",
        "mcu jtag.pdf",
        "msp-lito-g3507 evaluation module.pdf",
        "mspm0g3507.pdf",
    }
    module_roots = {
        "boostxl-drv8301 硬件用户指南.pdf",
        "mcu jtag.pdf",
        "xds110 debug probe.pdf",
    }

    if name in board_roots or (is_motor_module and any(part in rel for part in ("2、", "3、", "4、", "5、", "6、", "7、", "8、"))):
        categories.append("开发板")
    if name in chip_roots or is_motor_module:
        categories.append("芯片")
    if name in module_roots or is_motor_module or is_gimbal_module:
        categories.append("模块")
    if is_competition:
        categories.append("比赛规则")

    return [category for category in CATEGORY_ORDER if category in categories]


def subtype_for(rel: str) -> str:
    name = Path(rel).name.lower()
    if rel.startswith("历年赛题/") or rel.startswith("2025竞赛题目列表"):
        if "参赛报告" in rel:
            return "参赛报告"
        return "竞赛题目"
    if rel == "电赛材料.pdf":
        return "仪器与器材清单"
    if "原理图" in rel:
        return "原理图"
    if name == "3507.pdf":
        return "芯片数据手册"
    if name in {
        "boostxl-drv8301 硬件用户指南.pdf",
        "launchxl-f28027 c2000.pdf",
        "msp-lito-g3507 evaluation module.pdf",
        "mspm0g3507.pdf",
        "xds110 debug probe.pdf",
    }:
        return "用户指南"
    if name == "mcu jtag.pdf":
        return "调试指南"
    if "例程" in rel or "驱动案例" in rel or "数据获取" in rel or "双路驱动" in rel:
        return "驱动示例"
    if "舵机996" in rel or "贰自由度云台" in rel:
        return "机械图"
    if "接线" in rel or "控制原理" in rel:
        return "接线与原理"
    if "产品手册" in rel:
        return "产品目录"
    if "技术支持" in rel:
        return "支持信息"
    if "简介" in rel or name == "注意事项.pdf":
        return "模块说明"
    return "参考资料"


def tags_for(rel: str, sample: str) -> list[str]:
    haystack = f"{rel} {sample}".lower()
    definitions = (
        ("LP-MSPM0G3507", ("lp-mspm0g3507",)),
        ("MSP-LITO-G3507", ("msp-lito-g3507",)),
        ("LAUNCHXL-F28027", ("launchxl-f28027",)),
        ("BOOSTXL-DRV8301", ("boostxl-drv8301",)),
        ("XDS110", ("xds110",)),
        ("C2000", ("c2000", "f28027")),
        ("MSPM0G3507", ("mspm0g3507", "mspm0g350x")),
        ("STM32F103C8T6", ("stm32f103c8t6", "c8t6例程")),
        ("STM32F103RCT6", ("stm32f103rct6", "rct6例程")),
        ("Arduino UNO R3", ("arduino uno", "arduino例程")),
        ("Raspberry Pi", ("树莓派主板", "树莓派 3b", "树莓派例程")),
        ("Raspberry Pi Pico", ("树莓派pico", "树莓派 pico")),
        ("Jetson Nano", ("jetson nano",)),
        ("RDK X3", ("rdk-x3", "rdk x3")),
        ("AT8236", ("at8236", "双路电机驱动模块")),
        ("DRV8301", ("drv8301",)),
        ("CODBOT-SG02", ("codbot-sg02", "二自由度云台", "贰自由度云台")),
        ("MG996", ("mg996", "舵机996")),
        ("MG995", ("mg995",)),
        ("SG90/MG90S", ("sg90", "mg90s")),
    )
    tags = [label for label, needles in definitions if any(needle in haystack for needle in needles)]

    year = re.search(r"(?<!\d)(20\d{2})(?!\d)", rel)
    if year:
        tags.append(year.group(1))
    if "本科" in rel:
        tags.append("本科组")
    if "高职高专" in rel or "专科组" in rel:
        tags.append("高职高专组")
    if "稳压版" in rel:
        tags.append("稳压版")
    if "精简版" in rel:
        tags.append("精简版")
    return list(dict.fromkeys(tags))


def explicit_version(rel: str, metadata_title: str, sample: str) -> str:
    # Revision labels in drawing title blocks often collapse into "RevisionSize"
    # during extraction, so only trusted metadata and the filename are searched.
    corpus = f"{metadata_title} {Path(rel).name}"
    revision = re.search(r"\bRev(?:ision)?\.?\s*([A-Z0-9.]+)", corpus, re.IGNORECASE)
    if revision:
        return f"Rev. {revision.group(1).upper().rstrip('.')}"
    filename_version = re.search(r"(?<![A-Za-z0-9])V(\d+(?:\.\d+)*)", Path(rel).stem, re.IGNORECASE)
    if filename_version:
        return f"V{filename_version.group(1)}"
    return "未标注"


def source_document_id(metadata_title: str, sample: str) -> str:
    corpus = f"{metadata_title} {sample}"
    match = re.search(r"\b(?:SLAS|SLAU|SLVU|SPRU|SPRUI)[A-Z0-9-]+\b", corpus, re.IGNORECASE)
    return match.group(0).upper() if match else ""


def read_pdf(path: Path) -> dict[str, object]:
    rel = path.relative_to(ROOT).as_posix()
    record: dict[str, object] = {
        "path": rel,
        "filename": path.name,
        "size_bytes": path.stat().st_size,
        "modified_at": datetime.fromtimestamp(path.stat().st_mtime).astimezone().isoformat(timespec="seconds"),
        "sha256": file_sha256(path),
        "pages": "",
        "metadata_title": "",
        "metadata_author": "",
        "pdf_created": "",
        "text_chars_first_3_pages": 0,
        "text_status": "解析失败",
        "parse_error": "",
    }
    sample = ""
    try:
        reader = PdfReader(str(path), strict=False)
        metadata = reader.metadata or {}
        record["pages"] = len(reader.pages)
        record["metadata_title"] = clean_text(str(metadata.get("/Title") or ""), 200)
        record["metadata_author"] = clean_text(str(metadata.get("/Author") or ""), 120)
        record["pdf_created"] = pdf_date(str(metadata.get("/CreationDate") or ""))
        text_parts: list[str] = []
        for page in reader.pages[:3]:
            try:
                text_parts.append(page.extract_text() or "")
            except Exception:
                text_parts.append("")
        sample = clean_text(" ".join(text_parts), 8000)
        record["text_chars_first_3_pages"] = len(sample)
        record["text_status"] = "可检索" if len(sample) >= 30 else ("文本稀少" if sample else "无可提取文本")
    except Exception as exc:  # Keep broken PDFs in the manifest.
        record["parse_error"] = f"{type(exc).__name__}: {exc}"[:300]

    record["categories"] = categories_for(rel)
    record["subtype"] = subtype_for(rel)
    record["tags"] = tags_for(rel, sample)
    record["explicit_version"] = explicit_version(rel, str(record["metadata_title"]), sample)
    record["source_document_id"] = source_document_id(str(record["metadata_title"]), sample)
    record["sample"] = sample[:600]
    record["duplicate_group"] = ""
    record["duplicate_of"] = ""
    return record


def mark_duplicates(records: list[dict[str, object]]) -> None:
    by_hash: dict[str, list[dict[str, object]]] = defaultdict(list)
    for record in records:
        by_hash[str(record["sha256"])].append(record)

    duplicate_sets = [group for group in by_hash.values() if len(group) > 1]
    duplicate_sets.sort(key=lambda group: min(str(item["path"]) for item in group))
    for number, group in enumerate(duplicate_sets, start=1):
        group_id = f"DUP-PDF-{number:03d}"
        canonical = min(
            group,
            key=lambda item: (
                bool(re.search(r"\(\d+\)(?=\.pdf$)", str(item["path"]), re.IGNORECASE)),
                str(item["path"]).count("/"),
                len(str(item["path"])),
                str(item["path"]),
            ),
        )
        canonical_path = str(canonical["path"])
        for record in group:
            record["duplicate_group"] = group_id
            if record is not canonical:
                record["duplicate_of"] = canonical_path


def md_escape(value: object) -> str:
    return str(value).replace("|", "\\|").replace("\n", " ")


def pdf_link(record: dict[str, object]) -> str:
    target = "../../" + str(record["path"]).replace(">", "%3E")
    return f"[{md_escape(record['filename'])}](<{target}>)"


def write_csv(records: list[dict[str, object]]) -> None:
    fields = (
        "path",
        "filename",
        "categories",
        "subtype",
        "tags",
        "pages",
        "size_bytes",
        "sha256",
        "duplicate_group",
        "duplicate_of",
        "explicit_version",
        "source_document_id",
        "pdf_created",
        "modified_at",
        "metadata_title",
        "metadata_author",
        "text_status",
        "text_chars_first_3_pages",
        "parse_error",
        "sample",
    )
    with (INDEX_DIR / "PDF总索引.csv").open("w", encoding="utf-8-sig", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        for record in records:
            row = dict(record)
            row["categories"] = ";".join(record["categories"])
            row["tags"] = ";".join(record["tags"])
            writer.writerow({field: row.get(field, "") for field in fields})


def write_json(records: list[dict[str, object]]) -> None:
    payload = {
        "schema_version": 1,
        "generated_at": datetime.now().astimezone().isoformat(timespec="seconds"),
        "root": ".",
        "pdf_count": len(records),
        "records": records,
    }
    (INDEX_DIR / "pdf_manifest.json").write_text(
        json.dumps(payload, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )


def write_total_index(records: list[dict[str, object]]) -> None:
    duplicate_groups = {str(record["duplicate_group"]) for record in records if record["duplicate_group"]}
    unique_count = len({str(record["sha256"]) for record in records})
    category_counts = Counter(category for record in records for category in record["categories"])
    lines = [
        "# PDF 总索引",
        "",
        f"> 自动生成于 {datetime.now().astimezone().isoformat(timespec='seconds')}。不要手工编辑；运行 `python 知识库/tools/build_pdf_index.py` 更新。",
        "",
        f"- PDF 文件：{len(records)}",
        f"- 唯一内容：{unique_count}",
        f"- 精确重复组：{len(duplicate_groups)}",
        "- 分类计数（允许交叉分类）：" + "，".join(f"{name} {category_counts[name]}" for name in CATEGORY_ORDER),
        "",
        "| # | PDF | 分类 | 类型 | 关键标签 | 页数 | 显式版本 | 重复状态 |",
        "|---:|---|---|---|---|---:|---|---|",
    ]
    for number, record in enumerate(records, start=1):
        duplicate = str(record["duplicate_group"])
        if record["duplicate_of"]:
            duplicate += "，副本"
        elif duplicate:
            duplicate += "，保留引用"
        else:
            duplicate = "-"
        lines.append(
            "| "
            + " | ".join(
                (
                    str(number),
                    pdf_link(record),
                    " / ".join(record["categories"]),
                    str(record["subtype"]),
                    "、".join(record["tags"]) or "-",
                    str(record["pages"]) or "-",
                    str(record["explicit_version"]),
                    duplicate,
                )
            )
            + " |"
        )
    (INDEX_DIR / "PDF总索引.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_category_indexes(records: list[dict[str, object]]) -> None:
    descriptions = {
        "开发板": "开发板、评估板、扩展板及针对具体主板的驱动示例。",
        "芯片": "芯片数据手册、芯片相关评估板资料及以该芯片为核心的模块资料。",
        "模块": "电机驱动、云台、舵机、调试探针等模块的说明、原理图和例程。",
        "比赛规则": "竞赛题目、参赛报告、器材清单及题面内附的参赛注意事项。这里的分类名沿用知识库四类约定，不表示每份文档都是完整规则。",
    }
    for category in CATEGORY_ORDER:
        selected = [record for record in records if category in record["categories"]]
        lines = [
            f"# {category}索引",
            "",
            descriptions[category],
            "",
            f"共 {len(selected)} 个 PDF；同一文档可同时出现在多个分类中。",
            "",
            "| PDF | 类型 | 关键标签 | 页数 | 版本/变体 | 重复状态 |",
            "|---|---|---|---:|---|---|",
        ]
        for record in selected:
            variant = [tag for tag in record["tags"] if tag in ("稳压版", "精简版")]
            version = str(record["explicit_version"])
            if variant:
                version += "；" + "、".join(variant)
            duplicate = str(record["duplicate_group"]) or "-"
            if record["duplicate_of"]:
                duplicate += "，副本"
            lines.append(
                "| "
                + " | ".join(
                    (
                        pdf_link(record),
                        str(record["subtype"]),
                        "、".join(record["tags"]) or "-",
                        str(record["pages"]) or "-",
                        version,
                        duplicate,
                    )
                )
                + " |"
            )
        (INDEX_DIR / f"{category}.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> None:
    INDEX_DIR.mkdir(parents=True, exist_ok=True)
    paths = sorted(
        (path for path in ROOT.rglob("*") if path.is_file() and path.suffix.lower() == ".pdf"),
        key=lambda path: path.relative_to(ROOT).as_posix().lower(),
    )
    records = [read_pdf(path) for path in paths]
    mark_duplicates(records)
    write_csv(records)
    write_json(records)
    write_total_index(records)
    write_category_indexes(records)
    duplicate_groups = len({str(record["duplicate_group"]) for record in records if record["duplicate_group"]})
    print(f"Indexed {len(records)} PDFs, {len({record['sha256'] for record in records})} unique contents, {duplicate_groups} duplicate groups.")


if __name__ == "__main__":
    main()
