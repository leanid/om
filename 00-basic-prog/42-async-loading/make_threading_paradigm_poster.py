#!/usr/bin/env python3
from __future__ import annotations

import html
import subprocess
import textwrap
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable


EMU_PER_INCH = 914_400
SLIDE_W = int(11.69 * EMU_PER_INCH)
SLIDE_H = int(16.54 * EMU_PER_INCH)
FONT_FAMILY = "JetBrains Mono"

OUT_DIR = Path(__file__).resolve().parent
PPTX_PATH = OUT_DIR / "threading-paradigm-poster.pptx"
PDF_PATH = OUT_DIR / "threading-paradigm-poster.pdf"

BG = "F8FAFC"
TEXT = "0F172A"
MUTED = "475569"
BORDER = "CBD5E1"
BLUE = "38BDF8"
BLUE_DARK = "0369A1"
PURPLE = "A78BFA"
PURPLE_DARK = "7E22CE"
GREEN = "34D399"
GREEN_DARK = "15803D"
AMBER = "FBBF24"
AMBER_DARK = "B45309"
ROSE = "FB7185"
ROSE_DARK = "BE123C"
SLATE = "64748B"
CODE_BLUE = "2563EB"
CODE_PURPLE = "7C3AED"
CODE_GREEN = "15803D"
CODE_AMBER = "B45309"

HIGHLIGHT_TERMS = [
    ("oneTBB Task Scheduler", CODE_PURPLE),
    ("Task Scheduler", CODE_PURPLE),
    ("Scheduler", CODE_PURPLE),
    ("std::condition_variable", CODE_PURPLE),
    ("std::packaged_task", CODE_PURPLE),
    ("std::future::get()", CODE_GREEN),
    ("std::shared_mutex", CODE_PURPLE),
    ("std::shared_lock", CODE_PURPLE),
    ("std::unique_lock", CODE_PURPLE),
    ("std::stop_token", CODE_PURPLE),
    ("std::jthread", CODE_PURPLE),
    ("std::future", CODE_PURPLE),
    ("std::mutex", CODE_PURPLE),
    ("Flow Graph", CODE_PURPLE),
    ("oneTBB", CODE_PURPLE),
    ("Graph", CODE_PURPLE),
    ("Task", CODE_PURPLE),
    ("Flow", CODE_PURPLE),
    ("job_pool", CODE_GREEN),
    ("wait/sleep", CODE_AMBER),
    ("без", CODE_AMBER),
]


def inch(value: float) -> int:
    return int(value * EMU_PER_INCH)


def esc(value: str) -> str:
    return html.escape(value, quote=True)


@dataclass
class element:
    kind: str
    args: tuple
    kwargs: dict = field(default_factory=dict)


@dataclass
class poster:
    elements: list[element] = field(default_factory=list)

    def add(self, kind: str, *args, **kwargs) -> None:
        self.elements.append(element(kind, args, kwargs))


class pptx_writer:
    def __init__(self) -> None:
        self.shape_id = 1

    def next_id(self) -> int:
        self.shape_id += 1
        return self.shape_id

    def shape(
        self,
        x: int,
        y: int,
        w: int,
        h: int,
        *,
        fill: str,
        line: str | None = None,
        preset: str = "roundRect",
        name: str = "Shape",
    ) -> str:
        sid = self.next_id()
        line_xml = (
            f"<a:ln w=\"18000\"><a:solidFill><a:srgbClr val=\"{line}\"/></a:solidFill></a:ln>"
            if line
            else "<a:ln><a:noFill/></a:ln>"
        )
        return f"""
        <p:sp>
          <p:nvSpPr><p:cNvPr id="{sid}" name="{esc(name)} {sid}"/><p:cNvSpPr/><p:nvPr/></p:nvSpPr>
          <p:spPr>
            <a:xfrm><a:off x="{x}" y="{y}"/><a:ext cx="{w}" cy="{h}"/></a:xfrm>
            <a:prstGeom prst="{preset}"><a:avLst/></a:prstGeom>
            <a:solidFill><a:srgbClr val="{fill}"/></a:solidFill>{line_xml}
          </p:spPr>
          <p:txBody><a:bodyPr/><a:lstStyle/><a:p/></p:txBody>
        </p:sp>
        """

    def text_box(
        self,
        x: int,
        y: int,
        w: int,
        h: int,
        text: str | Iterable[str],
        *,
        size: int,
        color: str = TEXT,
        bold: bool = False,
        align: str = "l",
        fill: str | None = None,
        line: str | None = None,
        preset: str = "rect",
        margin: int = 65_000,
        name: str = "Text",
    ) -> str:
        sid = self.next_id()
        fill_xml = f"<a:solidFill><a:srgbClr val=\"{fill}\"/></a:solidFill>" if fill else "<a:noFill/>"
        line_xml = (
            f"<a:ln w=\"16000\"><a:solidFill><a:srgbClr val=\"{line}\"/></a:solidFill></a:ln>"
            if line
            else "<a:ln><a:noFill/></a:ln>"
        )
        lines = list(text) if not isinstance(text, str) else text.split("\n")
        paragraphs = "".join(self.paragraph(line, size=size, color=color, bold=bold, align=align) for line in lines)
        return f"""
        <p:sp>
          <p:nvSpPr><p:cNvPr id="{sid}" name="{esc(name)} {sid}"/><p:cNvSpPr txBox="1"/><p:nvPr/></p:nvSpPr>
          <p:spPr>
            <a:xfrm><a:off x="{x}" y="{y}"/><a:ext cx="{w}" cy="{h}"/></a:xfrm>
            <a:prstGeom prst="{preset}"><a:avLst/></a:prstGeom>
            {fill_xml}{line_xml}
          </p:spPr>
          <p:txBody>
            <a:bodyPr wrap="square" lIns="{margin}" tIns="{margin}" rIns="{margin}" bIns="{margin}" anchor="mid"/>
            <a:lstStyle/>
            {paragraphs}
          </p:txBody>
        </p:sp>
        """

    def paragraph(self, text: str, *, size: int, color: str, bold: bool, align: str) -> str:
        bold_attr = ' b="1"' if bold else ""
        return f"""
        <a:p>
          <a:pPr algn="{align}"/>
          <a:r>
            <a:rPr lang="ru-RU" sz="{size * 100}"{bold_attr} dirty="0">
              <a:solidFill><a:srgbClr val="{color}"/></a:solidFill>
              <a:latin typeface="{FONT_FAMILY}"/><a:ea typeface="{FONT_FAMILY}"/><a:cs typeface="{FONT_FAMILY}"/>
            </a:rPr>
            <a:t xml:space="preserve">{esc(text)}</a:t>
          </a:r>
          <a:endParaRPr lang="ru-RU" sz="{size * 100}" dirty="0"/>
        </a:p>
        """

    def rich_text_box(
        self,
        x: int,
        y: int,
        w: int,
        h: int,
        lines: Iterable[list[tuple[str, str, bool]]],
        *,
        size: int,
        align: str = "l",
        margin: int = 65_000,
        name: str = "RichText",
    ) -> str:
        sid = self.next_id()
        paragraphs = "".join(self.rich_paragraph(line, size=size, align=align) for line in lines)
        return f"""
        <p:sp>
          <p:nvSpPr><p:cNvPr id="{sid}" name="{esc(name)} {sid}"/><p:cNvSpPr txBox="1"/><p:nvPr/></p:nvSpPr>
          <p:spPr>
            <a:xfrm><a:off x="{x}" y="{y}"/><a:ext cx="{w}" cy="{h}"/></a:xfrm>
            <a:prstGeom prst="rect"><a:avLst/></a:prstGeom>
            <a:noFill/><a:ln><a:noFill/></a:ln>
          </p:spPr>
          <p:txBody>
            <a:bodyPr wrap="square" lIns="{margin}" tIns="{margin}" rIns="{margin}" bIns="{margin}" anchor="mid"/>
            <a:lstStyle/>
            {paragraphs}
          </p:txBody>
        </p:sp>
        """

    def rich_paragraph(self, runs: list[tuple[str, str, bool]], *, size: int, align: str) -> str:
        run_xml = []
        for text, color, bold in runs:
            bold_attr = ' b="1"' if bold else ""
            run_xml.append(
                f"""
          <a:r>
            <a:rPr lang="ru-RU" sz="{size * 100}"{bold_attr} dirty="0">
              <a:solidFill><a:srgbClr val="{color}"/></a:solidFill>
              <a:latin typeface="{FONT_FAMILY}"/><a:ea typeface="{FONT_FAMILY}"/><a:cs typeface="{FONT_FAMILY}"/>
            </a:rPr>
            <a:t xml:space="preserve">{esc(text)}</a:t>
          </a:r>"""
            )
        return f"""
        <a:p>
          <a:pPr algn="{align}"/>
          {''.join(run_xml)}
          <a:endParaRPr lang="ru-RU" sz="{size * 100}" dirty="0"/>
        </a:p>
        """

    def line(self, x1: int, y1: int, x2: int, y2: int, *, color: str = SLATE, width: int = 22_000, arrow: bool = True) -> str:
        sid = self.next_id()
        x = min(x1, x2)
        y = min(y1, y2)
        cx = abs(x2 - x1)
        cy = abs(y2 - y1)
        flips = []
        if x2 < x1:
            flips.append('flipH="1"')
        if y2 < y1:
            flips.append('flipV="1"')
        flip_attrs = (" " + " ".join(flips)) if flips else ""
        marker = '<a:tailEnd type="triangle"/>' if arrow else ""
        return f"""
        <p:cxnSp>
          <p:nvCxnSpPr><p:cNvPr id="{sid}" name="Arrow {sid}"/><p:cNvCxnSpPr/><p:nvPr/></p:nvCxnSpPr>
          <p:spPr>
            <a:xfrm{flip_attrs}><a:off x="{x}" y="{y}"/><a:ext cx="{cx}" cy="{cy}"/></a:xfrm>
            <a:prstGeom prst="line"><a:avLst/></a:prstGeom>
            <a:ln w="{width}"><a:solidFill><a:srgbClr val="{color}"/></a:solidFill>{marker}</a:ln>
          </p:spPr>
        </p:cxnSp>
        """

    def poster_xml(self, item: poster) -> str:
        self.shape_id = 1
        pieces = [self.shape(0, 0, SLIDE_W, SLIDE_H, fill=BG, preset="rect", name="Background")]
        for el in item.elements:
            pieces.append(getattr(self, el.kind)(*el.args, **el.kwargs))
        return f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<p:sld xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
       xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"
       xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main">
  <p:cSld>
    <p:spTree>
      <p:nvGrpSpPr><p:cNvPr id="1" name=""/><p:cNvGrpSpPr/><p:nvPr/></p:nvGrpSpPr>
      <p:grpSpPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="0" cy="0"/><a:chOff x="0" y="0"/><a:chExt cx="0" cy="0"/></a:xfrm></p:grpSpPr>
      {''.join(pieces)}
    </p:spTree>
  </p:cSld>
  <p:clrMapOvr><a:masterClrMapping/></p:clrMapOvr>
</p:sld>
"""

    def write(self, item: poster, path: Path) -> None:
        with zipfile.ZipFile(path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
            zf.writestr("[Content_Types].xml", content_types())
            zf.writestr("_rels/.rels", root_rels())
            zf.writestr("docProps/core.xml", core_xml())
            zf.writestr("docProps/app.xml", app_xml())
            zf.writestr("ppt/presentation.xml", presentation_xml())
            zf.writestr("ppt/_rels/presentation.xml.rels", presentation_rels())
            zf.writestr("ppt/theme/theme1.xml", theme_xml())
            zf.writestr("ppt/presProps.xml", "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><p:presentationPr xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\"/>")
            zf.writestr("ppt/viewProps.xml", "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><p:viewPr xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\"/>")
            zf.writestr("ppt/tableStyles.xml", "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><a:tblStyleLst xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" def=\"{5C22544A-7EE6-4342-B048-85BDC9FD1C3A}\"/>")
            zf.writestr("ppt/slides/slide1.xml", self.poster_xml(item))
            zf.writestr("ppt/slides/_rels/slide1.xml.rels", "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\"/>")


def content_types() -> str:
    return """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml" ContentType="application/xml"/>
  <Override PartName="/ppt/presentation.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.presentation.main+xml"/>
  <Override PartName="/ppt/theme/theme1.xml" ContentType="application/vnd.openxmlformats-officedocument.theme+xml"/>
  <Override PartName="/ppt/presProps.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.presProps+xml"/>
  <Override PartName="/ppt/viewProps.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.viewProps+xml"/>
  <Override PartName="/ppt/tableStyles.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.tableStyles+xml"/>
  <Override PartName="/ppt/slides/slide1.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.slide+xml"/>
  <Override PartName="/docProps/core.xml" ContentType="application/vnd.openxmlformats-package.core-properties+xml"/>
  <Override PartName="/docProps/app.xml" ContentType="application/vnd.openxmlformats-officedocument.extended-properties+xml"/>
</Types>
"""


def root_rels() -> str:
    return """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="ppt/presentation.xml"/>
  <Relationship Id="rId2" Type="http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties" Target="docProps/core.xml"/>
  <Relationship Id="rId3" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties" Target="docProps/app.xml"/>
</Relationships>
"""


def presentation_xml() -> str:
    return f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<p:presentation xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
                xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"
                xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main">
  <p:sldSz cx="{SLIDE_W}" cy="{SLIDE_H}"/>
  <p:notesSz cx="6858000" cy="9144000"/>
  <p:sldIdLst><p:sldId id="256" r:id="rId1"/></p:sldIdLst>
  <p:defaultTextStyle>
    <a:defPPr><a:defRPr lang="ru-RU"><a:latin typeface="{FONT_FAMILY}"/><a:ea typeface="{FONT_FAMILY}"/><a:cs typeface="{FONT_FAMILY}"/></a:defRPr></a:defPPr>
  </p:defaultTextStyle>
</p:presentation>
"""


def presentation_rels() -> str:
    return """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/slide" Target="slides/slide1.xml"/>
  <Relationship Id="rId2" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme" Target="theme/theme1.xml"/>
  <Relationship Id="rId3" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/presProps" Target="presProps.xml"/>
  <Relationship Id="rId4" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/viewProps" Target="viewProps.xml"/>
  <Relationship Id="rId5" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/tableStyles" Target="tableStyles.xml"/>
</Relationships>
"""


def core_xml() -> str:
    return """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<cp:coreProperties xmlns:cp="http://schemas.openxmlformats.org/package/2006/metadata/core-properties"
                   xmlns:dc="http://purl.org/dc/elements/1.1/"
                   xmlns:dcterms="http://purl.org/dc/terms/"
                   xmlns:dcmitype="http://purl.org/dc/dcmitype/"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <dc:title>Выбор парадигмы многопоточного программирования</dc:title>
  <dc:creator>Cursor</dc:creator>
  <cp:lastModifiedBy>Cursor</cp:lastModifiedBy>
  <dcterms:created xsi:type="dcterms:W3CDTF">2026-05-15T00:00:00Z</dcterms:created>
  <dcterms:modified xsi:type="dcterms:W3CDTF">2026-05-15T00:00:00Z</dcterms:modified>
</cp:coreProperties>
"""


def app_xml() -> str:
    return """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/extended-properties"
            xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">
  <Application>Cursor generated poster</Application>
  <PresentationFormat>A3 portrait</PresentationFormat>
  <Slides>1</Slides>
</Properties>
"""


def theme_xml() -> str:
    return f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<a:theme xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" name="ThreadingPoster">
  <a:themeElements>
    <a:clrScheme name="ThreadingPoster">
      <a:dk1><a:srgbClr val="{TEXT}"/></a:dk1><a:lt1><a:srgbClr val="FFFFFF"/></a:lt1>
      <a:dk2><a:srgbClr val="{MUTED}"/></a:dk2><a:lt2><a:srgbClr val="{BG}"/></a:lt2>
      <a:accent1><a:srgbClr val="{BLUE}"/></a:accent1><a:accent2><a:srgbClr val="{PURPLE}"/></a:accent2>
      <a:accent3><a:srgbClr val="{GREEN}"/></a:accent3><a:accent4><a:srgbClr val="{AMBER}"/></a:accent4>
      <a:accent5><a:srgbClr val="{ROSE}"/></a:accent5><a:accent6><a:srgbClr val="{SLATE}"/></a:accent6>
      <a:hlink><a:srgbClr val="{BLUE_DARK}"/></a:hlink><a:folHlink><a:srgbClr val="{PURPLE_DARK}"/></a:folHlink>
    </a:clrScheme>
    <a:fontScheme name="JetBrainsMono"><a:majorFont><a:latin typeface="{FONT_FAMILY}"/><a:ea typeface="{FONT_FAMILY}"/><a:cs typeface="{FONT_FAMILY}"/></a:majorFont><a:minorFont><a:latin typeface="{FONT_FAMILY}"/><a:ea typeface="{FONT_FAMILY}"/><a:cs typeface="{FONT_FAMILY}"/></a:minorFont></a:fontScheme>
    <a:fmtScheme name="ThreadingPoster"><a:fillStyleLst><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:fillStyleLst><a:lnStyleLst><a:ln w="9525"><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:ln></a:lnStyleLst><a:effectStyleLst><a:effectStyle/></a:effectStyleLst><a:bgFillStyleLst><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:bgFillStyleLst></a:fmtScheme>
  </a:themeElements>
</a:theme>
"""


def wrap(text: str, width: int) -> list[str]:
    return textwrap.wrap(text, width=width, break_long_words=False, break_on_hyphens=False)


def highlight_terms(text: str) -> list[tuple[str, str, bool]]:
    runs: list[tuple[str, str, bool]] = []
    index = 0
    while index < len(text):
        match: tuple[str, str] | None = None
        for term, color in sorted(HIGHLIGHT_TERMS, key=lambda item: len(item[0]), reverse=True):
            if text.startswith(term, index):
                match = (term, color)
                break
        if match:
            term, color = match
            runs.append((term, color, True))
            index += len(term)
            continue

        next_index = index + 1
        while next_index < len(text):
            if any(text.startswith(term, next_index) for term, _ in HIGHLIGHT_TERMS):
                break
            next_index += 1
        runs.append((text[index:next_index], TEXT, False))
        index = next_index
    return runs or [(" ", TEXT, False)]


def highlighted_lines(lines: Iterable[str]) -> list[list[tuple[str, str, bool]]]:
    return [highlight_terms(line) for line in lines]


def box(
    p: poster,
    x: float,
    y: float,
    w: float,
    h: float,
    title: str,
    body: str,
    *,
    fill: str,
    line: str,
    title_color: str,
    body_size: int = 11,
    title_size: int = 14,
) -> None:
    p.add("shape", inch(x), inch(y), inch(w), inch(h), fill=fill, line=line, preset="roundRect")
    p.add("text_box", inch(x + 0.15), inch(y + 0.12), inch(w - 0.3), inch(0.35), title, size=title_size + 1, color=title_color, bold=True, margin=0)
    p.add("rich_text_box", inch(x + 0.15), inch(y + 0.53), inch(w - 0.3), inch(h - 0.62), highlighted_lines(wrap(body, max(14, int(w * 8.8)))), size=body_size + 1, margin=0)


def decision(p: poster, x: float, y: float, w: float, h: float, text: str, *, fill: str, line: str) -> None:
    p.add("shape", inch(x), inch(y), inch(w), inch(h), fill=fill, line=line, preset="diamond")
    p.add("text_box", inch(x + 0.24), inch(y + 0.38), inch(w - 0.48), inch(h - 0.36), wrap(text, max(10, int(w * 5.4))), size=13, color=TEXT, bold=True, align="ctr", margin=0)


def build_poster() -> poster:
    p = poster()
    p.add("text_box", inch(0.6), inch(0.32), inch(10.5), inch(0.55), "Как выбрать парадигму многопоточного программирования", size=21, color=TEXT, bold=True, align="ctr", margin=0)
    p.add("text_box", inch(0.85), inch(0.9), inch(10.0), inch(0.35), "Идем сверху вниз: тип приложения -> форма работы -> инструмент", size=10, color=MUTED, align="ctr", margin=0)

    decision(p, 4.15, 1.55, 3.4, 1.05, "Что пишем?", fill="FFFFFF", line=SLATE)

    box(
        p,
        0.65,
        3.0,
        4.45,
        1.35,
        "Консольная утилита",
        "Можно грузить все ядра долгими операциями. Пользователь в любой момент остановит процесс через CTRL+C.",
        fill="E0F2FE",
        line=BLUE,
        title_color=BLUE_DARK,
    )
    box(
        p,
        6.15,
        3.0,
        4.9,
        1.35,
        "Графическая программа / игра",
        "Главный UI-поток не делает долгую работу. 60 Гц = 16 мс на кадр, следующий кадр должен выходить всегда.",
        fill="FFE4E6",
        line=ROSE,
        title_color=ROSE_DARK,
    )

    p.add("line", inch(5.85), inch(2.6), inch(2.88), inch(3.0), color=BLUE)
    p.add("line", inch(5.85), inch(2.6), inch(8.6), inch(3.0), color=ROSE)

    p.add("text_box", inch(0.95), inch(4.58), inch(3.85), inch(0.38), "Парадигма: свободный parallel/batch", size=11, color=BLUE_DARK, bold=True, align="ctr", fill="DBEAFE", line=BLUE, preset="roundRect")

    decision(p, 4.0, 5.0, 3.75, 1.05, "Если UI/игра: какая форма работы?", fill="FFFFFF", line=ROSE)
    p.add("line", inch(8.6), inch(4.35), inch(5.88), inch(5.0), color=ROSE)

    lanes = [
        (
            0.45,
            "Поток данных",
            "Продюсер-Консьюмер",
            "Один поток производит данные, другой потребляет: streaming, загрузка чанков, очередь событий.",
            "E0F2FE",
            BLUE,
            BLUE_DARK,
        ),
        (
            3.25,
            "Разделяемое состояние",
            "Много читателей, один писатель",
            "Данные часто читаются и редко меняются: конфиги, таблицы, кэш метаданных.",
            "DCFCE7",
            GREEN,
            GREEN_DARK,
        ),
        (
            6.05,
            "Одна большая задача",
            "Долгая и отменяемая",
            "Загрузка уровня, импорт, расчет, который может быть отменен или завершиться исключением.",
            "FEF3C7",
            AMBER,
            AMBER_DARK,
        ),
        (
            8.85,
            "Много задач",
            "Pool или граф задач",
            "Много коротких независимых задач или сложный граф зависимостей между задачами.",
            "F3E8FF",
            PURPLE,
            PURPLE_DARK,
        ),
    ]

    for x, label, title, body, fill, line, title_color in lanes:
        p.add("line", inch(5.88), inch(6.05), inch(x + 1.2), inch(6.82), color=line)
        p.add("text_box", inch(x), inch(6.4), inch(2.4), inch(0.36), label, size=9, color=title_color, bold=True, align="ctr", margin=0)
        box(p, x, 6.82, 2.4, 1.55, title, body, fill=fill, line=line, title_color=title_color, body_size=9, title_size=11)

    box(
        p,
        0.45,
        8.85,
        2.4,
        1.65,
        "2 потока",
        "Достаточно одной очереди, одного std::mutex и одного std::condition_variable.",
        fill="FFFFFF",
        line=BLUE,
        title_color=BLUE_DARK,
        body_size=9,
        title_size=11,
    )
    box(
        p,
        3.25,
        8.85,
        2.4,
        1.65,
        "Readers / writer",
        "Используем std::shared_mutex: std::shared_lock для чтения, std::unique_lock для записи.",
        fill="FFFFFF",
        line=GREEN,
        title_color=GREEN_DARK,
        body_size=9,
        title_size=11,
    )
    box(
        p,
        6.05,
        8.85,
        2.4,
        1.65,
        "Cancellable task",
        "std::jthread + std::stop_token для отмены. std::packaged_task + std::future для результата.",
        fill="FFFFFF",
        line=AMBER,
        title_color=AMBER_DARK,
        body_size=9,
        title_size=11,
    )
    box(
        p,
        8.85,
        8.85,
        2.4,
        1.65,
        "Независимые jobs",
        "job_pool. Задачи короткие, CPU-bound, без wait/sleep внутри worker job.",
        fill="FFFFFF",
        line=PURPLE,
        title_color=PURPLE_DARK,
        body_size=9,
        title_size=11,
    )

    p.add("line", inch(1.65), inch(8.37), inch(1.65), inch(8.85), color=BLUE)
    p.add("line", inch(4.45), inch(8.37), inch(4.45), inch(8.85), color=GREEN)
    p.add("line", inch(7.25), inch(8.37), inch(7.25), inch(8.85), color=AMBER)
    p.add("line", inch(10.05), inch(8.37), inch(10.05), inch(8.85), color=PURPLE)

    box(
        p,
        6.05,
        10.9,
        2.4,
        1.65,
        "Ошибки",
        "Исключение из worker не теряем: std::future::get() перебросит его в точке получения результата.",
        fill="FFF7ED",
        line=AMBER,
        title_color=AMBER_DARK,
        body_size=9,
        title_size=11,
    )
    box(
        p,
        8.85,
        10.9,
        2.4,
        1.65,
        "Сложный граф",
        "Если задач много и они зависят друг от друга: oneTBB Task Scheduler / Flow Graph.",
        fill="FFFFFF",
        line=PURPLE,
        title_color=PURPLE_DARK,
        body_size=9,
        title_size=11,
    )
    p.add("line", inch(7.25), inch(10.5), inch(7.25), inch(10.9), color=AMBER)
    p.add("line", inch(10.05), inch(10.5), inch(10.05), inch(10.9), color=PURPLE)

    p.add("shape", inch(0.65), inch(12.95), inch(10.4), inch(1.95), fill="FFFFFF", line=BORDER, preset="roundRect")
    p.add("text_box", inch(0.95), inch(13.12), inch(9.8), inch(0.34), "Финальная проверка перед выбором", size=14, color=TEXT, bold=True, align="ctr", margin=0)
    checks = [
        "UI/main поток никогда не блокируется на долгий wait/sleep/join.",
        "Отмена кооперативная: request_stop()/interrupt() + регулярные safe points.",
        "Результат типизирован: success / failure / cancelled, никаких pointer+size.",
        "Мелкие зависимости не гоняются через main queue, иначе скорость ограничится FPS.",
    ]
    p.add("text_box", inch(0.95), inch(13.6), inch(9.8), inch(1.05), checks, size=9, color=MUTED, align="l", margin=0)

    p.add("text_box", inch(0.8), inch(15.55), inch(10.1), inch(0.35), "Правило плаката: сначала форма работы и ограничения UI, потом конкретный примитив синхронизации.", size=9, color=SLATE, align="ctr", margin=0)
    return p


def export_pdf() -> None:
    subprocess.run(
        [
            "libreoffice",
            "--headless",
            "--convert-to",
            "pdf",
            "--outdir",
            str(OUT_DIR),
            str(PPTX_PATH),
        ],
        check=True,
    )


def main() -> None:
    item = build_poster()
    pptx_writer().write(item, PPTX_PATH)
    export_pdf()
    if not PDF_PATH.exists():
        raise RuntimeError(f"LibreOffice did not create {PDF_PATH}")
    print(f"wrote {PPTX_PATH}")
    print(f"wrote {PDF_PATH}")


if __name__ == "__main__":
    main()
