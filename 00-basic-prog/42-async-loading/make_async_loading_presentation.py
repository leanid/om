#!/usr/bin/env python3
from __future__ import annotations

import html
import subprocess
import textwrap
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable


SLIDE_W = 12_192_000
SLIDE_H = 6_858_000
EMU_PER_INCH = 914_400
FONT_FAMILY = "JetBrains Mono"

OUT_DIR = Path(__file__).resolve().parent
PPTX_PATH = OUT_DIR / "async-loading-presentation.pptx"
PDF_PATH = OUT_DIR / "async-loading-presentation.pdf"


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
class slide:
    title: str
    subtitle: str = ""
    background: str = "0B1020"
    accent: str = "7DD3FC"
    elements: list[element] = field(default_factory=list)

    def add(self, kind: str, *args, **kwargs) -> None:
        self.elements.append(element(kind, args, kwargs))


class pptx_writer:
    def __init__(self) -> None:
        self.shape_id = 1

    def next_id(self) -> int:
        self.shape_id += 1
        return self.shape_id

    def text_box(
        self,
        x: int,
        y: int,
        w: int,
        h: int,
        text: str | Iterable[str],
        *,
        size: int = 28,
        color: str = "FFFFFF",
        font: str = FONT_FAMILY,
        bold: bool = False,
        align: str = "l",
        fill: str | None = None,
        line: str | None = None,
        radius: bool = False,
        margin: int = 90_000,
        name: str = "Text",
    ) -> str:
        sid = self.next_id()
        shape = "roundRect" if radius else "rect"
        fill_xml = f"<a:solidFill><a:srgbClr val=\"{fill}\"/></a:solidFill>" if fill else "<a:noFill/>"
        line_xml = (
            f"<a:ln w=\"14000\"><a:solidFill><a:srgbClr val=\"{line}\"/></a:solidFill></a:ln>"
            if line
            else "<a:ln><a:noFill/></a:ln>"
        )
        paragraphs = list(text) if not isinstance(text, str) else text.split("\n")
        paragraph_xml = "".join(
            self.paragraph(
                item,
                size=size,
                color=color,
                font=font,
                bold=bold,
                align=align,
            )
            for item in paragraphs
        )
        return f"""
        <p:sp>
          <p:nvSpPr><p:cNvPr id="{sid}" name="{esc(name)} {sid}"/><p:cNvSpPr txBox="1"/><p:nvPr/></p:nvSpPr>
          <p:spPr>
            <a:xfrm><a:off x="{x}" y="{y}"/><a:ext cx="{w}" cy="{h}"/></a:xfrm>
            <a:prstGeom prst="{shape}"><a:avLst/></a:prstGeom>
            {fill_xml}{line_xml}
          </p:spPr>
          <p:txBody>
            <a:bodyPr wrap="square" lIns="{margin}" tIns="{margin}" rIns="{margin}" bIns="{margin}" anchor="mid"/>
            <a:lstStyle/>
            {paragraph_xml}
          </p:txBody>
        </p:sp>
        """

    def paragraph(self, text: str, *, size: int, color: str, font: str, bold: bool, align: str) -> str:
        bold_attr = ' b="1"' if bold else ""
        return f"""
        <a:p>
          <a:pPr algn="{align}"/>
          <a:r>
            <a:rPr lang="ru-RU" sz="{size * 100}"{bold_attr} dirty="0">
              <a:solidFill><a:srgbClr val="{color}"/></a:solidFill>
              <a:latin typeface="{esc(font)}"/><a:ea typeface="{esc(font)}"/><a:cs typeface="{esc(font)}"/>
            </a:rPr>
            <a:t>{esc(text)}</a:t>
          </a:r>
          <a:endParaRPr lang="ru-RU" sz="{size * 100}" dirty="0"/>
        </a:p>
        """

    def rect(
        self,
        x: int,
        y: int,
        w: int,
        h: int,
        *,
        fill: str,
        line: str | None = None,
        radius: bool = False,
        name: str = "Shape",
    ) -> str:
        sid = self.next_id()
        shape = "roundRect" if radius else "rect"
        line_xml = (
            f"<a:ln w=\"16000\"><a:solidFill><a:srgbClr val=\"{line}\"/></a:solidFill></a:ln>"
            if line
            else "<a:ln><a:noFill/></a:ln>"
        )
        return f"""
        <p:sp>
          <p:nvSpPr><p:cNvPr id="{sid}" name="{esc(name)} {sid}"/><p:cNvSpPr/><p:nvPr/></p:nvSpPr>
          <p:spPr>
            <a:xfrm><a:off x="{x}" y="{y}"/><a:ext cx="{w}" cy="{h}"/></a:xfrm>
            <a:prstGeom prst="{shape}"><a:avLst/></a:prstGeom>
            <a:solidFill><a:srgbClr val="{fill}"/></a:solidFill>{line_xml}
          </p:spPr>
          <p:txBody><a:bodyPr/><a:lstStyle/><a:p/></p:txBody>
        </p:sp>
        """

    def ellipse(
        self,
        x: int,
        y: int,
        w: int,
        h: int,
        *,
        fill: str,
        line: str | None = None,
        name: str = "Circle",
    ) -> str:
        sid = self.next_id()
        line_xml = (
            f"<a:ln w=\"16000\"><a:solidFill><a:srgbClr val=\"{line}\"/></a:solidFill></a:ln>"
            if line
            else "<a:ln><a:noFill/></a:ln>"
        )
        return f"""
        <p:sp>
          <p:nvSpPr><p:cNvPr id="{sid}" name="{esc(name)} {sid}"/><p:cNvSpPr/><p:nvPr/></p:nvSpPr>
          <p:spPr>
            <a:xfrm><a:off x="{x}" y="{y}"/><a:ext cx="{w}" cy="{h}"/></a:xfrm>
            <a:prstGeom prst="ellipse"><a:avLst/></a:prstGeom>
            <a:solidFill><a:srgbClr val="{fill}"/></a:solidFill>{line_xml}
          </p:spPr>
          <p:txBody><a:bodyPr/><a:lstStyle/><a:p/></p:txBody>
        </p:sp>
        """

    def line(self, x1: int, y1: int, x2: int, y2: int, *, color: str = "FFFFFF", width: int = 24_000, arrow: bool = True) -> str:
        sid = self.next_id()
        head = '<a:headEnd type="triangle"/>' if arrow else ""
        return f"""
        <p:cxnSp>
          <p:nvCxnSpPr><p:cNvPr id="{sid}" name="Arrow {sid}"/><p:cNvCxnSpPr/><p:nvPr/></p:nvCxnSpPr>
          <p:spPr>
            <a:xfrm><a:off x="{x1}" y="{y1}"/><a:ext cx="{x2 - x1}" cy="{y2 - y1}"/></a:xfrm>
            <a:prstGeom prst="line"><a:avLst/></a:prstGeom>
            <a:ln w="{width}"><a:solidFill><a:srgbClr val="{color}"/></a:solidFill>{head}</a:ln>
          </p:spPr>
        </p:cxnSp>
        """

    def slide_xml(self, item: slide, index: int) -> str:
        self.shape_id = 1
        pieces = [self.rect(0, 0, SLIDE_W, SLIDE_H, fill=item.background, name="Background")]
        pieces.append(self.rect(0, 0, inch(0.18), SLIDE_H, fill=item.accent, name="Accent"))
        pieces.append(
            self.text_box(
                inch(0.55),
                inch(0.22),
                inch(11.65),
                inch(0.62),
                item.title,
                size=24,
                bold=True,
                color="FFFFFF",
                fill=None,
                margin=0,
                name="Title",
            )
        )
        if item.subtitle:
            pieces.append(
                self.text_box(
                    inch(0.58),
                    inch(0.82),
                    inch(11.0),
                    inch(0.40),
                    item.subtitle,
                size=11,
                    color="A7B3C8",
                    margin=0,
                    name="Subtitle",
                )
            )
        for el in item.elements:
            pieces.append(getattr(self, el.kind)(*el.args, **el.kwargs))
        pieces.append(
            self.text_box(
                inch(11.25),
                inch(7.12),
                inch(0.7),
                inch(0.2),
                f"{index:02d}",
                size=9,
                color="64748B",
                align="r",
                margin=0,
                name="Page",
            )
        )
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

    def write(self, slides: list[slide], path: Path) -> None:
        with zipfile.ZipFile(path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
            zf.writestr("[Content_Types].xml", content_types(len(slides)))
            zf.writestr("_rels/.rels", root_rels())
            zf.writestr("docProps/core.xml", core_xml())
            zf.writestr("docProps/app.xml", app_xml(len(slides)))
            zf.writestr("ppt/presentation.xml", presentation_xml(len(slides)))
            zf.writestr("ppt/_rels/presentation.xml.rels", presentation_rels(len(slides)))
            zf.writestr("ppt/theme/theme1.xml", theme_xml())
            zf.writestr("ppt/presProps.xml", "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><p:presentationPr xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\"/>")
            zf.writestr("ppt/viewProps.xml", "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><p:viewPr xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\"/>")
            zf.writestr("ppt/tableStyles.xml", "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><a:tblStyleLst xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" def=\"{5C22544A-7EE6-4342-B048-85BDC9FD1C3A}\"/>")
            for idx, sld in enumerate(slides, 1):
                zf.writestr(f"ppt/slides/slide{idx}.xml", self.slide_xml(sld, idx))
                zf.writestr(f"ppt/slides/_rels/slide{idx}.xml.rels", "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\"/>")


def content_types(count: int) -> str:
    slides = "\n".join(
        f'<Override PartName="/ppt/slides/slide{i}.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.slide+xml"/>'
        for i in range(1, count + 1)
    )
    return f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml" ContentType="application/xml"/>
  <Override PartName="/ppt/presentation.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.presentation.main+xml"/>
  <Override PartName="/ppt/theme/theme1.xml" ContentType="application/vnd.openxmlformats-officedocument.theme+xml"/>
  <Override PartName="/ppt/presProps.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.presProps+xml"/>
  <Override PartName="/ppt/viewProps.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.viewProps+xml"/>
  <Override PartName="/ppt/tableStyles.xml" ContentType="application/vnd.openxmlformats-officedocument.presentationml.tableStyles+xml"/>
  <Override PartName="/docProps/core.xml" ContentType="application/vnd.openxmlformats-package.core-properties+xml"/>
  <Override PartName="/docProps/app.xml" ContentType="application/vnd.openxmlformats-officedocument.extended-properties+xml"/>
  {slides}
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


def presentation_xml(count: int) -> str:
    slide_ids = "\n".join(f'<p:sldId id="{255 + i}" r:id="rId{i}"/>' for i in range(1, count + 1))
    return f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<p:presentation xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
                xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"
                xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main">
  <p:sldSz cx="{SLIDE_W}" cy="{SLIDE_H}" type="wide"/>
  <p:notesSz cx="6858000" cy="9144000"/>
  <p:sldIdLst>{slide_ids}</p:sldIdLst>
  <p:defaultTextStyle>
    <a:defPPr><a:defRPr lang="ru-RU"><a:latin typeface="{FONT_FAMILY}"/><a:ea typeface="{FONT_FAMILY}"/><a:cs typeface="{FONT_FAMILY}"/></a:defRPr></a:defPPr>
  </p:defaultTextStyle>
</p:presentation>
"""


def presentation_rels(count: int) -> str:
    rels = "\n".join(
        f'<Relationship Id="rId{i}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/slide" Target="slides/slide{i}.xml"/>'
        for i in range(1, count + 1)
    )
    return f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  {rels}
  <Relationship Id="rId{count + 1}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme" Target="theme/theme1.xml"/>
  <Relationship Id="rId{count + 2}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/presProps" Target="presProps.xml"/>
  <Relationship Id="rId{count + 3}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/viewProps" Target="viewProps.xml"/>
  <Relationship Id="rId{count + 4}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/tableStyles" Target="tableStyles.xml"/>
</Relationships>
"""


def core_xml() -> str:
    return """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<cp:coreProperties xmlns:cp="http://schemas.openxmlformats.org/package/2006/metadata/core-properties"
                   xmlns:dc="http://purl.org/dc/elements/1.1/"
                   xmlns:dcterms="http://purl.org/dc/terms/"
                   xmlns:dcmitype="http://purl.org/dc/dcmitype/"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <dc:title>Асинхронная загрузка без зависаний</dc:title>
  <dc:creator>Cursor</dc:creator>
  <cp:lastModifiedBy>Cursor</cp:lastModifiedBy>
  <dcterms:created xsi:type="dcterms:W3CDTF">2026-05-11T00:00:00Z</dcterms:created>
  <dcterms:modified xsi:type="dcterms:W3CDTF">2026-05-11T00:00:00Z</dcterms:modified>
</cp:coreProperties>
"""


def app_xml(count: int) -> str:
    return f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/extended-properties"
            xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">
  <Application>Cursor generated presentation</Application>
  <PresentationFormat>16:9</PresentationFormat>
  <Slides>{count}</Slides>
</Properties>
"""


def theme_xml() -> str:
    return f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<a:theme xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" name="AsyncLoading">
  <a:themeElements>
    <a:clrScheme name="AsyncLoading">
      <a:dk1><a:srgbClr val="0B1020"/></a:dk1><a:lt1><a:srgbClr val="FFFFFF"/></a:lt1>
      <a:dk2><a:srgbClr val="111827"/></a:dk2><a:lt2><a:srgbClr val="E5E7EB"/></a:lt2>
      <a:accent1><a:srgbClr val="7DD3FC"/></a:accent1><a:accent2><a:srgbClr val="A78BFA"/></a:accent2>
      <a:accent3><a:srgbClr val="34D399"/></a:accent3><a:accent4><a:srgbClr val="FBBF24"/></a:accent4>
      <a:accent5><a:srgbClr val="FB7185"/></a:accent5><a:accent6><a:srgbClr val="94A3B8"/></a:accent6>
      <a:hlink><a:srgbClr val="38BDF8"/></a:hlink><a:folHlink><a:srgbClr val="C084FC"/></a:folHlink>
    </a:clrScheme>
    <a:fontScheme name="JetBrainsMono"><a:majorFont><a:latin typeface="{FONT_FAMILY}"/><a:ea typeface="{FONT_FAMILY}"/><a:cs typeface="{FONT_FAMILY}"/></a:majorFont><a:minorFont><a:latin typeface="{FONT_FAMILY}"/><a:ea typeface="{FONT_FAMILY}"/><a:cs typeface="{FONT_FAMILY}"/></a:minorFont></a:fontScheme>
    <a:fmtScheme name="Async"><a:fillStyleLst><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:fillStyleLst><a:lnStyleLst><a:ln w="9525"><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:ln></a:lnStyleLst><a:effectStyleLst><a:effectStyle/></a:effectStyleLst><a:bgFillStyleLst><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:bgFillStyleLst></a:fmtScheme>
  </a:themeElements>
</a:theme>
"""


def wrap(text: str, width: int = 24) -> list[str]:
    return textwrap.wrap(text, width=width, break_long_words=False, break_on_hyphens=False)


def card(s: slide, x: float, y: float, w: float, h: float, title: str, body: str, *, color: str = "172033", accent: str = "7DD3FC") -> None:
    body_width = max(10, int(w * 9.2))
    s.add("rect", inch(x), inch(y), inch(w), inch(h), fill=color, line=accent, radius=True)
    s.add("text_box", inch(x + 0.14), inch(y + 0.13), inch(w - 0.28), inch(0.28), title, size=11, bold=True, color=accent, fill=None, margin=0)
    s.add("text_box", inch(x + 0.14), inch(y + 0.43), inch(w - 0.28), inch(max(0.36, h - 0.55)), wrap(body, body_width), size=11, bold=False, color="E5E7EB", fill=None, margin=0)


def big_number(s: slide, x: float, y: float, number: str, label: str, color: str) -> None:
    s.add("ellipse", inch(x), inch(y), inch(1.62), inch(1.62), fill=color, line="FFFFFF")
    s.add("text_box", inch(x), inch(y + 0.3), inch(1.62), inch(0.42), number, size=19, bold=True, color="0B1020", align="ctr", margin=0)
    s.add("text_box", inch(x - 0.34), inch(y + 1.76), inch(2.3), inch(0.55), wrap(label, 16), size=10, color="CBD5E1", align="ctr", margin=0)


def code_slide(s: slide, code: str, *, y: float = 1.35, size: int = 13) -> None:
    s.add("text_box", inch(0.68), inch(y), inch(11.05), inch(5.55), code.splitlines(), size=size, color="E2E8F0", font=FONT_FAMILY, fill="111827", line="334155", radius=True, margin=95_000)


def build_slides() -> list[slide]:
    slides: list[slide] = []

    s = slide("Асинхронная загрузка без зависаний", "Главный поток, отмена, исключения, job_pool и FPS", background="08111F", accent="38BDF8")
    s.add("ellipse", inch(7.7), inch(1.25), inch(2.6), inch(2.6), fill="1E3A8A", line="38BDF8")
    s.add("ellipse", inch(8.65), inch(2.2), inch(2.6), inch(2.6), fill="581C87", line="C084FC")
    s.add("text_box", inch(0.85), inch(2.0), inch(7.0), inch(1.35), "400 МБ ресурсов\nне должны остановить игру", size=25, bold=True, color="FFFFFF", fill=None, margin=0)
    s.add("text_box", inch(0.9), inch(4.05), inch(6.25), inch(0.9), "Цель: загрузить в фоне, уметь отменить, не потерять инварианты.", size=15, color="CBD5E1", fill=None, margin=0)
    slides.append(s)

    s = slide("Какие вопросы закрываем", "После презентации должна сложиться полная картина", background="0F172A", accent="A78BFA")
    items = [
        ("Главный поток", "почему нельзя долго работать"),
        ("tb::job_pool", "почему не каждая очередь задач подходит"),
        ("Отмена", "как остановить загрузку культурно"),
        ("Тайминги", "16 мс, 0.8 с, 2 с, 5 с ANR"),
        ("Потоки", "почему нельзя просто убить thread"),
        ("Callbacks", "почему try/catch обязателен"),
        ("FPS", "как зависимости задач упираются в кадры"),
    ]
    for i, (title, body) in enumerate(items):
        x = 0.75 + (i % 3) * 3.75
        y = 1.35 + (i // 3) * 1.55
        card(s, x, y, 3.25, 1.05, title, body, color="151F33", accent=["38BDF8", "A78BFA", "34D399"][i % 3])
    slides.append(s)

    s = slide("Главный поток: сердце приложения", "Он качает события, обновление и рендер", background="0B1020", accent="38BDF8")
    blocks = [("events", "обработка ввода"), ("update", "логика игры"), ("render", "кадр на экран")]
    for i, (title, body) in enumerate(blocks):
        x = 1.0 + i * 3.5
        card(s, x, 2.05, 2.65, 1.25, title, body, color="102033", accent="38BDF8")
        if i < len(blocks) - 1:
            s.add("line", inch(x + 2.72), inch(2.67), inch(x + 3.28), inch(2.67), color="7DD3FC")
    s.add("line", inch(8.95), inch(3.45), inch(1.05), inch(3.45), color="64748B")
    s.add("line", inch(1.05), inch(3.45), inch(1.05), inch(2.05), color="64748B")
    s.add("text_box", inch(1.65), inch(4.32), inch(8.8), inch(0.82), "Если один блок завис, игрок видит не «работу», а мертвое окно.", size=17, bold=True, color="FFFFFF", align="ctr", fill=None, margin=0)
    slides.append(s)

    s = slide("Бюджет кадра: всего 16 мс", "60 FPS = 1 / 60 секунды", background="111827", accent="34D399")
    segments = [("events", 2, "38BDF8"), ("physics", 4, "A78BFA"), ("render", 8, "34D399"), ("logic", 2, "FBBF24")]
    x = 1.0
    for name, ms, color in segments:
        w = 10.0 * ms / 16
        s.add("rect", inch(x), inch(2.5), inch(w), inch(0.85), fill=color, line=None, radius=True)
        s.add("text_box", inch(x), inch(2.62), inch(w), inch(0.42), f"{name}\n{ms} мс", size=10, bold=True, color="0B1020", align="ctr", margin=0)
        x += w
    s.add("text_box", inch(0.85), inch(3.75), inch(10.3), inch(0.82), "Любой wait(), sleep(), join() или синхронный I/O крадет этот бюджет.", size=17, color="E5E7EB", align="ctr", fill=None, margin=0)
    slides.append(s)

    s = slide("Загрузка уровня: 400 МБ", "На диске это данные, для игрока это ожидание", background="0F172A", accent="FBBF24")
    s.add("rect", inch(1.1), inch(2.0), inch(4.7), inch(2.7), fill="78350F", line="FBBF24", radius=True)
    s.add("text_box", inch(1.1), inch(2.46), inch(4.7), inch(0.72), "400 МБ", size=34, bold=True, color="FEF3C7", align="ctr", margin=0)
    s.add("text_box", inch(6.55), inch(1.48), inch(4.55), inch(3.55), ["прочитать файлы", "распаковать", "распарсить", "создать GPU-ресурсы", "сообщить игре"], size=18, color="E5E7EB", fill="172033", line="334155", radius=True)
    s.add("line", inch(5.9), inch(3.35), inch(6.55), inch(3.35), color="FBBF24")
    slides.append(s)

    s = slide("Почему нельзя долго работать на main", "Потому что main отвечает за ощущение жизни", background="111827", accent="FB7185")
    for i, (title, body) in enumerate([
        ("События не читаются", "ОС считает окно зависшим"),
        ("Кадры не рисуются", "игрок видит фриз"),
        ("Ввод не работает", "кнопка «назад» бесполезна"),
        ("Закрытие тормозит", "можно получить ANR"),
    ]):
        card(s, 0.9 + (i % 2) * 5.4, 1.7 + (i // 2) * 2.0, 4.7, 1.35, title, body, color="1F2937", accent="FB7185")
    slides.append(s)

    s = slide("Тайминги, которые держим в голове", "Не точные законы природы, а инженерные ориентиры", background="0B1020", accent="FBBF24")
    big_number(s, 0.85, 2.0, "16мс", "кадр при 60 FPS", "38BDF8")
    big_number(s, 3.35, 2.0, "0.8с", "заметная реакция", "34D399")
    big_number(s, 5.85, 2.0, "2с", "ожидание нервирует", "FBBF24")
    big_number(s, 8.35, 2.0, "5с", "верхняя зона ANR", "FB7185")
    s.add("text_box", inch(1.0), inch(5.0), inch(10.0), inch(0.85), "Практическое правило: целевой лимит делим на 2 и проверяем отмену чаще.", size=17, bold=True, color="FFFFFF", align="ctr", margin=0)
    slides.append(s)

    s = slide("Старый способ: кооперативная многозадачность", "Одно ядро, один поток, много добровольных уступок", background="0F172A", accent="A78BFA")
    steps = [("рисуем экран", "loading..."), ("грузим пачку", "N текстур"), ("обрабатываем ввод", "не зависаем"), ("повторяем", "пока готово")]
    for i, (title, body) in enumerate(steps):
        x = 0.75 + i * 2.85
        card(s, x, 2.2, 2.25, 1.25, title, body, color="1E1B4B", accent="A78BFA")
        if i < 3:
            s.add("line", inch(x + 2.35), inch(2.83), inch(x + 2.75), inch(2.83), color="C084FC")
    s.add("text_box", inch(0.95), inch(4.6), inch(10.1), inch(0.82), "Современная отмена устроена похоже: поток сам регулярно проверяет «продолжать?»", size=17, color="EDE9FE", align="ctr", margin=0)
    slides.append(s)

    s = slide("Современный путь: отдельный поток", "Main остается отзывчивым, worker делает долгую работу", background="08111F", accent="38BDF8")
    card(s, 0.95, 2.2, 3.05, 1.35, "main thread", "события, update, render", color="102033", accent="38BDF8")
    card(s, 4.85, 1.55, 3.05, 1.25, "worker thread", "читает и парсит ресурсы", color="1E1B4B", accent="A78BFA")
    card(s, 8.35, 3.1, 2.85, 1.25, "main queue", "сигнал «уровень готов»", color="064E3B", accent="34D399")
    s.add("line", inch(4.05), inch(2.77), inch(4.75), inch(2.25), color="7DD3FC")
    s.add("line", inch(7.95), inch(2.25), inch(8.35), inch(3.55), color="34D399")
    slides.append(s)

    s = slide("Почему «просто поток» быстро усложняется", "Появляются жизненный цикл, ошибки и синхронизация", background="111827", accent="FB7185")
    risks = [
        ("экран закрыт", "объекты уже уничтожены"),
        ("файл не прочитан", "исключение ушло в thread"),
        ("вечный цикл", "готовность не придет"),
        ("mutex захвачен", "join или kill ломает всех"),
        ("render ждет texture", "main ждет закрытие"),
        ("очередь main", "мелкие задачи стали 60/сек"),
    ]
    for i, (title, body) in enumerate(risks):
        card(s, 0.7 + (i % 3) * 3.85, 1.45 + (i // 3) * 1.85, 3.25, 1.25, title, body, color="1F2937", accent="FB7185")
    slides.append(s)

    s = slide("Если main завершился раньше worker", "ОС не делает красивую отмену вашего кода", background="0B1020", accent="FB7185")
    card(s, 1.0, 1.9, 3.3, 1.35, "main умер", "объекты и глобальные ресурсы уходят", color="1F2937", accent="FB7185")
    card(s, 4.85, 1.9, 3.3, 1.35, "worker снят", "ядро просто прекращает выполнение", color="1F2937", accent="FB7185")
    card(s, 8.7, 1.9, 2.75, 1.35, "инварианты?", "не гарантированы", color="1F2937", accent="FB7185")
    s.add("line", inch(4.35), inch(2.58), inch(4.75), inch(2.58), color="FB7185")
    s.add("line", inch(8.2), inch(2.58), inch(8.6), inch(2.58), color="FB7185")
    s.add("text_box", inch(0.95), inch(4.42), inch(10.3), inch(0.9), "Файл кэша, лог, mutex, OpenGL context: всё может остаться в промежуточном состоянии.", size=16, color="FECACA", align="ctr", margin=0)
    slides.append(s)

    s = slide("join() тоже может быть проблемой", "Корректность не должна превращаться в зависание при выходе", background="0F172A", accent="FBBF24")
    card(s, 1.0, 1.8, 3.25, 1.35, "пользователь закрывает", "main хочет быть вежливым", color="172033", accent="FBBF24")
    card(s, 4.75, 1.8, 3.25, 1.35, "main делает join()", "ждет worker слишком долго", color="172033", accent="FBBF24")
    card(s, 8.5, 1.8, 2.85, 1.35, "ОС видит ANR", "и вытесняет процесс", color="3B1D0A", accent="FB7185")
    s.add("line", inch(4.3), inch(2.48), inch(4.65), inch(2.48), color="FBBF24")
    s.add("line", inch(8.05), inch(2.48), inch(8.4), inch(2.48), color="FB7185")
    s.add("text_box", inch(0.95), inch(4.42), inch(10.3), inch(0.85), "Отмена должна быть запрошена заранее и быстро доходить до interruption point.", size=16, color="FEF3C7", align="ctr", margin=0)
    slides.append(s)

    s = slide("Почему нельзя убивать поток через API ОС", "Thread kill обрывает код между двумя инструкциями", background="111827", accent="FB7185")
    card(s, 0.95, 1.55, 3.25, 1.35, "RAII не сработал", "деструкторы не раскрутили стек", color="1F2937", accent="FB7185")
    card(s, 4.55, 1.55, 3.25, 1.35, "mutex остался", "синхронизация сломана", color="1F2937", accent="FB7185")
    card(s, 8.15, 1.55, 3.25, 1.35, "файл оборван", "кэш потерял инвариант", color="1F2937", accent="FB7185")
    s.add("text_box", inch(0.85), inch(4.02), inch(10.5), inch(1.15), "Правильный путь: попросить остановиться, дойти до безопасной точки, выйти обычным return или исключением, которое поймано.", size=17, bold=True, color="FFFFFF", align="ctr", margin=0)
    slides.append(s)

    s = slide("Владение объектами между потоками", "Классический ответ: shared_ptr + weak_ptr", background="0B1020", accent="34D399")
    card(s, 1.0, 1.7, 3.25, 1.35, "shared_ptr", "удерживает объект живым", color="052E2B", accent="34D399")
    card(s, 4.75, 1.7, 3.25, 1.35, "weak_ptr", "не продлевает жизнь", color="052E2B", accent="34D399")
    card(s, 8.5, 1.7, 2.85, 1.35, "lock()", "либо объект, либо null", color="052E2B", accent="34D399")
    s.add("line", inch(4.3), inch(2.38), inch(4.65), inch(2.38), color="34D399")
    s.add("line", inch(8.05), inch(2.38), inch(8.4), inch(2.38), color="34D399")
    s.add("text_box", inch(0.95), inch(4.32), inch(10.3), inch(0.82), "Глобальный объект не «вечный»: при завершении процесса он тоже разрушается.", size=16, color="DCFCE7", align="ctr", margin=0)
    slides.append(s)

    s = slide("std::jthread + stop_token", "C++20 дает авто-join и кооперативную отмену", background="08111F", accent="38BDF8")
    code_slide(s, """void do_work(std::stop_token stoken) {
    for (int i = 0; i < 100; ++i) {
        if (stoken.stop_requested()) {
            return; // безопасная точка остановки
        }

        std::this_thread::sleep_for(100ms);
        load_next_chunk();
    }
}

std::jthread worker(do_work);""", size=12)
    slides.append(s)

    s = slide("Отмена остается кооперативной", "stop_token сам ничего не прерывает", background="0F172A", accent="38BDF8")
    for i, (title, body) in enumerate([
        ("часто проверяем", "перед I/O, после пачки, в циклах"),
        ("быстро выходим", "return из безопасной точки"),
        ("чистим RAII", "стек раскручивается штатно"),
        ("не блокируем main", "join только с понятным лимитом"),
    ]):
        card(s, 0.9 + (i % 2) * 5.4, 1.6 + (i // 2) * 1.85, 4.7, 1.25, title, body, color="102033", accent="38BDF8")
    slides.append(s)

    s = slide("Boost: interruption points", "Старый, но выразительный паттерн отмены", background="111827", accent="A78BFA")
    code_slide(s, """void do_work() {
    try {
        for (int i = 0; i < 100; ++i) {
            boost::this_thread::interruption_point();

            boost::this_thread::sleep_for(100ms);
            load_next_chunk();
        }
    } catch (const boost::thread_interrupted&) {
        // graceful shutdown
    }
}

thread.interrupt();
thread.join();""", size=12)
    slides.append(s)

    s = slide("Почему try/catch обязателен в callback", "Исключение в чужом потоке не вернется магически на main", background="0B1020", accent="FB7185")
    card(s, 0.85, 1.7, 3.35, 1.45, "callback бросил", "нет файла, ошибка парсинга, assert", color="1F2937", accent="FB7185")
    card(s, 4.55, 1.7, 3.35, 1.45, "исключение не поймано", "std::terminate / падение процесса", color="1F2937", accent="FB7185")
    card(s, 8.25, 1.7, 3.0, 1.45, "лог потерян", "причину сложно найти", color="1F2937", accent="FB7185")
    s.add("text_box", inch(0.85), inch(4.18), inch(10.5), inch(1.1), "Граница потока должна ловить все, логировать, переводить ошибку в результат и уведомлять владельца.", size=17, bold=True, color="FFFFFF", align="ctr", margin=0)
    slides.append(s)

    s = slide("Шаблон безопасной границы worker", "Пользовательский код не должен валить весь процесс без лога", background="111827", accent="34D399")
    code_slide(s, """void worker_entry(std::function<void()> user_callback) {
    try {
        user_callback();
        post_result(success{});
    } catch (const std::exception& e) {
        log_error(e.what());
        post_result(failure{e.what()});
    } catch (...) {
        log_error("unknown worker exception");
        post_result(failure{"unknown"});
    }
}""", size=12)
    slides.append(s)

    s = slide("Где ставить точки отмены", "Ответ рождается из таймингов", background="0F172A", accent="FBBF24")
    for i, (title, body) in enumerate([
        ("до длинного I/O", "чтобы не начинать лишнее"),
        ("после каждой пачки", "текстура, mesh, chunk"),
        ("в больших циклах", "не реже целевого лимита"),
        ("перед callback", "владелец мог исчезнуть"),
    ]):
        card(s, 0.9 + (i % 2) * 5.4, 1.55 + (i // 2) * 1.85, 4.7, 1.25, title, body, color="172033", accent="FBBF24")
    s.add("text_box", inch(0.95), inch(5.22), inch(10.3), inch(0.55), "Для закрытия без ANR: не зависать дольше ~2.5 секунд локально.", size=14, color="FEF3C7", align="ctr", margin=0)
    slides.append(s)

    s = slide("Что хорошего в job_pool", "Он отлично утилизирует ядра для коротких независимых задач", background="08111F", accent="34D399")
    for i in range(4):
        card(s, 1.0 + i * 2.65, 2.0, 2.05, 1.15, f"job {i + 1}", "короткий CPU task", color="052E2B", accent="34D399")
    s.add("text_box", inch(0.95), inch(4.15), inch(10.3), inch(0.85), "Консольная утилита или batch processing: да, можно загрузить железо на максимум.", size=16, color="DCFCE7", align="ctr", margin=0)
    slides.append(s)

    s = slide("Проблема загрузки через tb::job_pool", "В графическом приложении долгие задачи могут отравить пул", background="111827", accent="FB7185")
    for i, (title, body) in enumerate([
        ("wait/sleep внутри job", "воркер занят, новые задачи стоят"),
        ("нет контроля отмены", "чужой пул не знает ваш stop_token"),
        ("исключения опасны", "callback может убить процесс"),
        ("I/O непредсказуем", "latency ломает FPS-план"),
    ]):
        card(s, 0.9 + (i % 2) * 5.4, 1.6 + (i // 2) * 1.85, 4.7, 1.25, title, body, color="1F2937", accent="FB7185")
    slides.append(s)

    s = slide("Нужна отдельная модель загрузки", "Не «любой job», а управляемая async-операция", background="0B1020", accent="38BDF8")
    card(s, 0.85, 1.55, 2.8, 1.25, "handle", "request_cancel()", color="102033", accent="38BDF8")
    card(s, 4.0, 1.55, 2.8, 1.25, "worker", "checks + RAII", color="102033", accent="38BDF8")
    card(s, 7.15, 1.55, 2.8, 1.25, "result", "success / error / cancelled", color="102033", accent="38BDF8")
    card(s, 4.0, 3.75, 2.8, 1.25, "owner", "weak_ptr.lock()", color="064E3B", accent="34D399")
    s.add("line", inch(3.72), inch(2.18), inch(3.92), inch(2.18), color="7DD3FC")
    s.add("line", inch(6.87), inch(2.18), inch(7.07), inch(2.18), color="7DD3FC")
    s.add("line", inch(5.4), inch(3.0), inch(5.4), inch(3.65), color="34D399")
    slides.append(s)

    s = slide("Состояния cancellable loading", "Отмена - это штатный результат, а не авария", background="0F172A", accent="A78BFA")
    states = [("queued", "задача ждет"), ("running", "грузит пачками"), ("cancelling", "доходит до safe point"), ("done", "result")]
    for i, (title, body) in enumerate(states):
        card(s, 0.8 + i * 2.9, 2.25, 2.25, 1.25, title, body, color="1E1B4B", accent="A78BFA")
        if i < 3:
            s.add("line", inch(3.1 + i * 2.9), inch(2.88), inch(3.55 + i * 2.9), inch(2.88), color="C084FC")
    s.add("text_box", inch(1.0), inch(4.72), inch(10.0), inch(0.72), "done может означать: success, failure или cancelled.", size=16, color="EDE9FE", align="ctr", margin=0)
    slides.append(s)

    s = slide("Main queue: только крупные события", "Не привязывайте каждую мелкую загрузку к FPS", background="08111F", accent="FBBF24")
    card(s, 0.85, 1.8, 3.2, 1.35, "плохо", "texture готова -> main -> следующая texture", color="3B1D0A", accent="FB7185")
    card(s, 4.6, 1.8, 3.2, 1.35, "лучше", "worker queue запускает следующую сразу", color="052E2B", accent="34D399")
    card(s, 8.35, 1.8, 2.85, 1.35, "main", "получает «уровень готов»", color="102033", accent="38BDF8")
    s.add("text_box", inch(0.85), inch(4.3), inch(10.5), inch(0.9), "Сигнал на main раз в кадр - нормально для крупных фаз, плохо для сотен мелких зависимых задач.", size=16, color="FEF3C7", align="ctr", margin=0)
    slides.append(s)

    s = slide("Как зависимость задач упирается в FPS", "Если следующая загрузка стартует только из main queue", background="111827", accent="FB7185")
    s.add("text_box", inch(0.75), inch(1.6), inch(10.95), inch(0.7), "texture A готова -> событие main -> кадр -> старт texture B", size=18, bold=True, color="FFFFFF", align="ctr", margin=0)
    for i in range(6):
        x = 1.0 + i * 1.75
        s.add("rect", inch(x), inch(3.0), inch(1.35), inch(0.7), fill="1F2937", line="64748B", radius=True)
        s.add("text_box", inch(x), inch(3.15), inch(1.35), inch(0.3), f"frame {i + 1}", size=9, color="CBD5E1", align="ctr", margin=0)
        if i in [0, 1, 2, 3]:
            s.add("ellipse", inch(x + 0.42), inch(2.25), inch(0.5), inch(0.5), fill="FB7185", line=None)
    s.add("text_box", inch(0.95), inch(4.6), inch(10.3), inch(0.9), "Итог: максимум около 60 таких переходов в секунду при 60 FPS.", size=17, bold=True, color="FECACA", align="ctr", margin=0)
    slides.append(s)

    s = slide("Когда результат A влияет на задачу B", "Опасность: граф зависимостей спрятан в callback'ах", background="0B1020", accent="A78BFA")
    card(s, 0.9, 1.75, 2.7, 1.25, "load manifest", "получили список текстур", color="1E1B4B", accent="A78BFA")
    card(s, 4.15, 1.75, 2.7, 1.25, "load textures", "зависит от manifest", color="1E1B4B", accent="A78BFA")
    card(s, 7.4, 1.75, 2.7, 1.25, "create material", "зависит от textures", color="1E1B4B", accent="A78BFA")
    s.add("line", inch(3.68), inch(2.38), inch(4.05), inch(2.38), color="C084FC")
    s.add("line", inch(6.93), inch(2.38), inch(7.3), inch(2.38), color="C084FC")
    s.add("text_box", inch(0.85), inch(4.42), inch(10.5), inch(0.9), "Если каждый переход идет через main, граф загрузки начинает тикать с частотой игры.", size=16, color="EDE9FE", align="ctr", margin=0)
    slides.append(s)

    s = slide("Render thread и контекст графики", "Еще одна причина не блокировать закрытие", background="111827", accent="38BDF8")
    card(s, 0.95, 1.65, 3.05, 1.25, "main", "закрывает приложение", color="102033", accent="38BDF8")
    card(s, 4.7, 1.65, 3.05, 1.25, "render", "держит OpenGL context", color="102033", accent="38BDF8")
    card(s, 8.45, 1.65, 2.85, 1.25, "loader", "ждет texture upload", color="102033", accent="38BDF8")
    s.add("line", inch(4.05), inch(2.28), inch(4.6), inch(2.28), color="7DD3FC")
    s.add("line", inch(7.8), inch(2.28), inch(8.35), inch(2.28), color="7DD3FC")
    s.add("text_box", inch(0.9), inch(4.28), inch(10.4), inch(1.0), "Цикл ожиданий между потоками превращает «закрыть приложение» в ANR.", size=18, bold=True, color="FFFFFF", align="ctr", margin=0)
    slides.append(s)

    s = slide("Минимальный протокол результата", "Main должен получить не только «готово»", background="0F172A", accent="34D399")
    for i, (title, body) in enumerate([
        ("success", "ресурсы готовы"),
        ("failure", "ошибка с диагностикой"),
        ("cancelled", "владелец передумал"),
        ("owner missing", "weak_ptr не залочился"),
    ]):
        card(s, 0.9 + (i % 2) * 5.4, 1.55 + (i // 2) * 1.85, 4.7, 1.25, title, body, color="052E2B", accent="34D399")
    slides.append(s)

    s = slide("Практический чек-лист", "Что должно быть в реальной async-загрузке", background="08111F", accent="FBBF24")
    checklist = [
        "main никогда не делает долгий wait/sleep/join",
        "у операции есть handle и request_cancel()",
        "worker регулярно проверяет stop/interruption",
        "граница worker ловит все исключения",
        "результат типизирован: success/failure/cancelled",
        "владельцы передаются через weak_ptr",
        "мелкие зависимости не идут через main queue",
    ]
    s.add("text_box", inch(0.85), inch(1.32), inch(10.7), inch(5.0), [f"✓ {item}" for item in checklist], size=16, color="E5E7EB", fill="172033", line="FBBF24", radius=True)
    slides.append(s)

    s = slide("Ментальная модель", "Никакой серебряной пули нет", background="0B1020", accent="38BDF8")
    s.add("text_box", inch(0.85), inch(1.58), inch(10.5), inch(1.2), "Асинхронность не отменяет кооперативность.", size=25, bold=True, color="FFFFFF", align="ctr", margin=0)
    s.add("text_box", inch(1.0), inch(3.08), inch(10.2), inch(1.7), "Потоки работают безопасно только когда вы явно описали владение, отмену, ошибки, очереди и тайминги.", size=19, color="CBD5E1", align="ctr", margin=0)
    s.add("text_box", inch(1.85), inch(5.35), inch(8.5), inch(0.55), "Сначала инварианты, потом производительность.", size=14, color="7DD3FC", align="ctr", margin=0)
    slides.append(s)

    return slides


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
    slides = build_slides()
    pptx_writer().write(slides, PPTX_PATH)
    export_pdf()
    if not PDF_PATH.exists():
        raise RuntimeError(f"LibreOffice did not create {PDF_PATH}")
    print(f"wrote {PPTX_PATH}")
    print(f"wrote {PDF_PATH}")


if __name__ == "__main__":
    main()
