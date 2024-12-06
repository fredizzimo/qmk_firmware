from dataclasses import dataclass
from enum import Enum, auto
from copy import copy
from pathlib import Path
import math
from math import atan
from build123d import *
from ocp_vscode import *
import pymupdf

show_clear()

class SwitchHolderType(Enum):
    MX = auto(),
    CHOC = auto()

@dataclass
class SwitchHolderConfig:
    switch_type: SwitchHolderType
    width: float
    length: float
    hole_width: float
    hole_length: float
    slot_width: float
    slot_top: float
    slot_depth: float
    # This should be near the bottom of the keycap, where the keys are the cloest to each other
    plate_to_keycap_height: float


cherry_mx_switch_holder_cfg = SwitchHolderConfig(
    switch_type=SwitchHolderType.MX,
    width=19.05,
    length=19.05,
    hole_width=13.9,
    hole_length=13.9,
    slot_width=5.0,
    slot_top=1.4,
    slot_depth=0.5,
    plate_to_keycap_height=6.7,
)

# TODO: Fix this
choc_switch_holder_cfg = SwitchHolderConfig(
    switch_type=SwitchHolderType.CHOC,
    width=18.0,
    length=17.0,
    hole_width=13.8,
    hole_length=13.8,
    slot_width=11.5,
    slot_top=1.2,
    slot_depth=0.5,
    plate_to_keycap_height=6.7,
)


@dataclass
class ColumnConfig:
    num_keys: int
    offset: float


@dataclass
class ThumbConfig:
    switch_holder: SwitchHolderConfig
    num_keys: int
    angle: float
    x: float
    y: float


@dataclass
class KeyboardConfig:
    switch_holder: SwitchHolderConfig
    columns: list[ColumnConfig]
    thumb: ThumbConfig
    inner_wall_thickness: float


@dataclass
class PlateConfig:
    thickness: float
    top_thickness: float
    border: float


@dataclass
class DraftConfig:
    plate: PlateConfig
    wall_thickness: float
    total_height: float

@dataclass
class ControllerConfig:
    width: float
    length: float
    side_clearance: float


# Ergodox
# -4.75
# -4.75
# 0
# 2.4
# 0
# -2.4

columns = [
    ColumnConfig(num_keys=3, offset=-8.0),
    ColumnConfig(num_keys=3, offset=-8.0),
    ColumnConfig(num_keys=3, offset=5.0),
    ColumnConfig(num_keys=3, offset=8.0),
    ColumnConfig(num_keys=3, offset=0.0),
    ColumnConfig(num_keys=3, offset=-2.4),
]

thumb = ThumbConfig(
    switch_holder=choc_switch_holder_cfg,
    num_keys=3,
    angle=30,
    x=cherry_mx_switch_holder_cfg.width * (len(columns) - 1)
    + (cherry_mx_switch_holder_cfg.width - choc_switch_holder_cfg.width) / 2.0
    - 5.0,
    y=columns[5].offset - choc_switch_holder_cfg.length,
)

keyboard = KeyboardConfig(
    switch_holder=cherry_mx_switch_holder_cfg,
    columns=columns,
    thumb=thumb,
    inner_wall_thickness=1,
)

draft_config = DraftConfig(plate=PlateConfig(thickness=1.5, top_thickness=0, border=3), total_height=8, wall_thickness=1)
plate_config = PlateConfig(thickness = 0.32 + 10 * 0.2, top_thickness=6, border=3)
controller_config = ControllerConfig(width=18.60, length=20, side_clearance=0.5)


def draw_pdf(name):
    def to_points(mm):
        inches = mm / 25.4
        return inches * 72.0

    doc = pymupdf.open()
    page = doc.new_page(width=to_points(210), height=to_points(297))
    shape = page.new_shape()

    def draw_switch(shape, config: SwitchHolderConfig, m: pymupdf.Matrix):
        x_offset = (config.width - config.hole_width) * 0.5
        y_offset = (config.length - config.hole_length) * 0.5
        shape.draw_quad(
            pymupdf.Rect(
                to_points(x_offset),
                to_points(-y_offset),
                to_points(config.width - x_offset),
                to_points(-config.length + y_offset),
            ).quad.transform(m)
        )

    def draw_keyboard_shape(shape, top, left, mirror):
        if mirror:
            mirror_matrix = pymupdf.Matrix(1, 0, 0, 1, 0, 0)
        else:
            mirror_matrix = pymupdf.Matrix(-1, 0, 0, 1, 0, 0)
            m = pymupdf.Matrix(pymupdf.Identity).pretranslate(to_points(210), to_points(0))
            mirror_matrix = mirror_matrix.concat(mirror_matrix, m)
        for i, column_config in enumerate(keyboard.columns):
            x = left + i * keyboard.switch_holder.width
            for j in range(column_config.num_keys):
                offset = column_config.offset
                y = top - j * keyboard.switch_holder.length - offset
                m = pymupdf.Matrix(pymupdf.Identity).pretranslate(to_points(x), to_points(y))
                m = m.concat(m, mirror_matrix)
                draw_switch(shape, keyboard.switch_holder, m)
        # Thumb
        y = top - keyboard.thumb.y
        for i in range(keyboard.thumb.num_keys):
            x = i * keyboard.thumb.switch_holder.width
            m1 = pymupdf.Matrix(pymupdf.Identity).pretranslate(to_points(x), to_points(0.0))
            m2 = pymupdf.Matrix(keyboard.thumb.angle)
            m3 = pymupdf.Matrix(pymupdf.Identity).pretranslate(
                to_points(left + keyboard.thumb.x), to_points(top - keyboard.thumb.y)
            )
            m = pymupdf.Matrix().concat(m1, m2)
            m = m.concat(m, m3)
            m = m.concat(m, mirror_matrix)
            draw_switch(shape, keyboard.thumb.switch_holder, m)

    draw_keyboard_shape(shape, 4 * keyboard.switch_holder.length, 30, False)
    draw_keyboard_shape(shape, 10 * keyboard.switch_holder.length, 30, True)
    shape.finish()

    shape.commit()
    doc.save(Path.cwd() / name)


draw_pdf("keyboard.pdf")


def make_switch_cutout(config: SwitchHolderConfig, plate_thickness):
    if config.switch_type == SwitchHolderType.MX:
        holder = Part() + (
                Box(config.hole_width, config.hole_length, plate_thickness),
                Box(config.slot_width, config.hole_length + 2 * config.slot_depth, plate_thickness).move(Pos(Z=-config.slot_top)),
            )
    else:
        holder = Part() + (
                Box(config.hole_width, config.hole_length, plate_thickness),
                Box(config.hole_width + 2 * config.slot_depth, config.slot_width, plate_thickness).move(Pos(Z=-config.slot_top)),
            )
    holder = holder + Box(config.width, config.length, 20.0, align=(Align.CENTER, Align.CENTER, Align.MIN)).moved(Pos(Z=plate_thickness / 2.0))
    return holder.moved(Pos(Z=plate_thickness / 2.0))

def make_switch_holder(config: SwitchHolderConfig, plate_thickness):
        return Box(config.width, config.length, plate_thickness, align=(Align.CENTER, Align.CENTER, Align.MIN)) - make_switch_cutout(config, plate_thickness)

def make_column_cutout(cutout: Part, config: ColumnConfig, switch_holder_config: SwitchHolderConfig, plate_thickness):
    length = switch_holder_config.length
    start = config.offset
    locations = [
        Pos(X=switch_holder_config.length / 2.0, Y=start + i * length + switch_holder_config.width / 2.0)
        for i in range(config.num_keys)
    ]
    return Part() + [copy(cutout).move(loc) for loc in locations]


def make_thumb_cutout(config: ThumbConfig, plate_thickness):
    switch_cutout = make_switch_cutout(config.switch_holder, plate_thickness)
    width = config.switch_holder.width
    offset = Pos(X=config.switch_holder.width / 2.0, Y=config.switch_holder.length / 2.0)
    locations = [
        Pos(X=i * width + config.switch_holder.width / 2.0, Y=config.switch_holder.length / 2.0)
        for i in range(config.num_keys)
    ]
    return Part() + [
        copy(switch_cutout).move(loc).rotate(Axis.Z, -config.angle).move(Pos(X=config.x, Y=config.y))
        for loc in locations
    ]


def make_plate_cutout(config: KeyboardConfig, plate_thickness):
    column_width = config.switch_holder.width
    switch_holder_config = config.switch_holder
    switch_cutout = make_switch_cutout(keyboard.switch_holder, plate_thickness)
    column_cutouts = [
        Pos(X=i * column_width)
        * make_column_cutout(switch_cutout, column_config, switch_holder_config, plate_thickness)
        for i, column_config in enumerate(config.columns)
    ]
    thumb_cutouts = make_thumb_cutout(config.thumb, plate_thickness)
    return Part() + column_cutouts + thumb_cutouts


def find_line_common_point(line1: Wire, line2: Wire) -> tuple[int, int]:
    for p1 in (0, 1):
        for p2 in (0, 1):
            if ((line1 @ p1) - (line2 @ p2)).length < 0.0001:
                return p1, p2

    raise ValueError("No common point found")


def trim(line, p1, p2):
    if p1 < p2:
        return line.trim(p1, p2)
    else:
        return line.trim(p2, p1)


def line_fillet(line1, line2, radius) -> tuple[BaseLineObject, TangentArc, BaseLineObject]:
    p1, p2 = find_line_common_point(line1, line2)
    tangent1 = line1 % p1
    tangent2 = line2 % p2
    if p1 == 1:
        tangent1 = -tangent1
    if p2 == 1:
        tangent2 = -tangent2
    angle = tangent1.get_angle(tangent2)
    half_angle = angle / 2.0
    side_length = radius / math.tan(math.radians(half_angle))
    end_point1 = line1 @ p1 + tangent1 * side_length
    end_point2 = line2 @ p2 + tangent2 * side_length
    fillet_arc = TangentArc(end_point1, end_point2, tangent=-tangent1)
    line1 = trim(line1, 1 - p1, line1.param_at_point(end_point1))
    line2 = trim(line2, 1 - p2, line2.param_at_point(end_point2))
    return line1, fillet_arc, line2


def make_plate_shape(config: KeyboardConfig, plate_config: PlateConfig, controller_config: ControllerConfig|None) -> Curve:
    border = plate_config.border
    shape = []
    controller_reserved = 0.0
    if controller_config is not None:
        controller_reserved = controller_config.width + 2 * controller_config.side_clearance + config.inner_wall_thickness
    e1 = Line(
        (-border, config.columns[0].offset),
        (-border, columns[3].offset + columns[3].num_keys * config.switch_holder.length + controller_reserved),
    )
    e2 = JernArc(e1 @ 1, e1 % 1, border, -90)
    e3 = Line(e2 @ 1, e2 @ 1 + (6 * config.switch_holder.width, 0))
    e4 = JernArc(e3 @ 1, e3 % 1, border, -90)

    e7 = PolarLine(
        (0, config.thumb.switch_holder.length + border), config.thumb.num_keys * config.thumb.switch_holder.width, 0
    )
    e8 = JernArc(e7 @ 1, e7 % 1, border, -90)
    e9 = PolarLine(e8 @ 1, config.thumb.switch_holder.length, -90)
    e10 = JernArc(e9 @ 1, e9 % 1, border, -90)
    e11 = PolarLine(e10 @ 1, e7.length, -180)

    thumb_location = Pos(config.thumb.x, config.thumb.y) * Rotation(Z=-config.thumb.angle)
    for e in (e7, e8, e9, e10, e11):
        e.move(thumb_location)

    e5 = IntersectingLine(e4 @ 1, (0, -1), e7)
    e7 = e7.trim(e7.param_at_point(e5 @ 1), 1)
    e5, e6, e7 = line_fillet(e5, e7, border)

    e14 = JernArc(e1 @ 0, -(e1 % 0), border, 90).reversed()
    e13 = Line(((e11 @ 1).X, (e14 @ 0).Y), e14 @ 0)
    e11 = IntersectingLine(e10 @ 1, e11 % 0, e13)
    e13 = e13.trim(e13.param_at_point(e11 @ 1), 1)

    e11, e12, e13 = line_fillet(e11, e13, border)

    return Wire() + [e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14]


def make_test_plate(keyboard_config: KeyboardConfig, draft_config: DraftConfig):
    plate_config = draft_config.plate
    cutout = make_plate_cutout(keyboard_config, plate_config.thickness)
    shape = make_plate_shape(keyboard_config, plate_config, None)
    sketch = make_face(shape.edges())
    plate = extrude(sketch, plate_config.thickness, Axis.Z.direction)
    shell = Shell() + [
        Shell.extrude(edge, (0, 0, -draft_config.total_height + plate_config.thickness)) for edge in shape.edges()
    ]
    walls = thicken(shell, draft_config.wall_thickness)
    return (plate - cutout) + walls

def make_top(keyboard_config: KeyboardConfig, plate_config: PlateConfig, controller_config: ControllerConfig):
    shape = make_plate_shape(keyboard_config, plate_config, controller_config)
    sketch = make_face(shape.edges())
    plate = extrude(sketch, plate_config.thickness + plate_config.top_thickness, Axis.Z.direction)
    cutout = make_plate_cutout(keyboard_config, plate_config.thickness)
    plate = plate - cutout
    pass

top = make_top(keyboard, plate_config, controller_config)

mx_switch_holder = make_switch_holder(cherry_mx_switch_holder_cfg, plate_config.thickness)
choc_switch_holder = make_switch_holder(choc_switch_holder_cfg, plate_config.thickness)
mx_switch_holder.export_step("mx_holder.step")
choc_switch_holder.export_step("choc_holder.step")

test_plate = make_test_plate(keyboard, draft_config)

test_plate.export_step("test_left.step")
test_plate.mirror(Plane.ZY).export_step("test_right.step")

show_object(
    test_plate,
    reset_camera=Camera.KEEP,
)


