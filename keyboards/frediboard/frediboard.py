from dataclasses import dataclass
from copy import copy
from pathlib import Path
import math
from math import atan
from build123d import *
from ocp_vscode import *
import pymupdf


@dataclass
class SwitchHolderConfig:
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
    width=19.05,
    length=19.05,
    hole_width=13.9,
    hole_length=13.9,
    slot_width=5.0,
    slot_top=1.3,
    slot_depth=0.5,
    plate_to_keycap_height=6.7,
)

# TODO: Fix this
choc_switch_holder_cfg = SwitchHolderConfig(
    width=18.0,
    length=17.0,
    hole_width=13.9,
    hole_length=13.9,
    slot_width=5.0,
    slot_top=1.3,
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
    ColumnConfig(num_keys=3, offset=2.0),
    ColumnConfig(num_keys=3, offset=5.4),
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
)


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


class SwitchHolder(Part):
    def __init__(self, config: SwitchHolderConfig, plate_thickness):
        holder = Box(config.width, config.length, plate_thickness) - [
            Box(config.hole_width, config.hole_length, plate_thickness),
            Box(config.slot_width, config.hole_length, plate_thickness),
            Box(config.slot_width, config.hole_length + 2 * config.slot_depth, plate_thickness).move(
                Pos(Z=-config.slot_top)
            ),
        ]
        self.top_middle_joint = RigidJoint(
            label="top_middle", to_part=holder, joint_location=Location((0.0, 0.0, plate_thickness / 2.0))
        )
        super().__init__(shapes=[holder])


class Column(Part):
    def __init__(self, config: ColumnConfig, switch_holder_config: SwitchHolderConfig, plate_thickness):
        switch_holder = SwitchHolder(switch_holder_config, plate_thickness)
        length = switch_holder_config.length
        start = config.offset
        locations = [
            Pos(X=switch_holder_config.length / 2.0, Y=start + i * length + switch_holder_config.width / 2.0)
            for i in range(config.num_keys)
        ]
        switches = [copy(switch_holder).move(loc) for loc in locations]

        super().__init__(shapes=Part() + switches)


class ThumbCluster(Part):
    def __init__(self, config: ThumbConfig, plate_thickness):
        switch_holder = SwitchHolder(config.switch_holder, plate_thickness)
        width = config.switch_holder.width
        offset = Pos(X=config.switch_holder.width / 2.0, Y=config.switch_holder.length / 2.0)
        locations = [
            Pos(X=i * width + config.switch_holder.width / 2.0, Y=config.switch_holder.length / 2.0)
            for i in range(config.num_keys)
        ]
        switches = [
            copy(switch_holder).move(loc).rotate(Axis.Z, -25).move(Pos(X=config.x, Y=config.y)) for loc in locations
        ]

        super().__init__(shapes=Part() + switches)


class Plate(Part):
    def __init__(self, config: KeyboardConfig, plate_thickness):
        width = config.switch_holder.width
        switch_holder_config = config.switch_holder
        columns = [
            Pos(X=i * width) * Column(column_config, switch_holder_config, plate_thickness)
            for i, column_config in enumerate(config.columns)
        ]
        thumb_cluster = ThumbCluster(config.thumb, plate_thickness)
        super().__init__(shapes=columns + [thumb_cluster])


plate_thickness = 3
plate = Plate(keyboard, plate_thickness)

show(
    plate,
    reset_camera=Camera.KEEP,
)
