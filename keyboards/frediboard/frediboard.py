from dataclasses import dataclass
from copy import copy
from pathlib import Path
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


@dataclass
class ColumnConfig:
    num_keys: int
    middle_key: int
    offset: float


@dataclass
class KeyboardConfig:
    switch_holder: SwitchHolderConfig
    columns: list[ColumnConfig]


# Ergodox
# -4.75
# -4.75
# 0
# 2.4
# 0
# -2.4

keyboard = KeyboardConfig(
    switch_holder=cherry_mx_switch_holder_cfg,
    columns=[
        ColumnConfig(num_keys=3, middle_key=1, offset=-12),
        ColumnConfig(num_keys=3, middle_key=1, offset=-12),
        ColumnConfig(num_keys=3, middle_key=1, offset=1.0),
        ColumnConfig(num_keys=3, middle_key=1, offset=5.4),
        ColumnConfig(num_keys=3, middle_key=1, offset=0.0),
        ColumnConfig(num_keys=3, middle_key=1, offset=-2.4),
    ],
)

def draw_pdf(name):
    def to_points(mm):
        inches = mm / 25.4
        return inches * 72.0

    doc = pymupdf.open()
    page = doc.new_page(width=to_points(210), height=to_points(297))
    shape = page.new_shape()

    def draw_keyboard_shape(shape, top, left, mirror):
        left_offset = (keyboard.switch_holder.width - keyboard.switch_holder.hole_width) * 0.5
        right_offset = left_offset + keyboard.switch_holder.hole_width
        top_offset = (keyboard.switch_holder.length - keyboard.switch_holder.hole_length) * 0.5
        bottom_offset = top_offset + keyboard.switch_holder.hole_length
        columns = reversed(keyboard.columns) if mirror else keyboard.columns
        for i, column_config in enumerate(columns):
            x = left + i * keyboard.switch_holder.width
            for j in range(column_config.num_keys):

                offset = column_config.offset - column_config.middle_key * keyboard.switch_holder.length
                y = top + j * keyboard.switch_holder.length - offset
                shape.draw_rect(
                        pymupdf.Rect(to_points(x + left_offset),
                                     to_points(y + top_offset),
                                     to_points(x + right_offset),
                                     to_points(y + bottom_offset)))



    draw_keyboard_shape(shape, 30, 30, False)
    draw_keyboard_shape(shape, 30 + 3 * keyboard.switch_holder.length + 30, 30, True)
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
        start = config.offset - config.middle_key * length
        locations = [Pos(Y=start + i * length) for i in range(config.num_keys)]
        switches = [copy(switch_holder).move(loc) for loc in locations]

        super().__init__(shapes=Part() + switches)


class Plate(Part):
    def __init__(self, config: KeyboardConfig, plate_thickness):
        width = config.switch_holder.width
        switch_holder_config = config.switch_holder
        columns = [
            Pos(X=i * width) * Column(column_config, switch_holder_config, plate_thickness)
            for i, column_config in enumerate(config.columns)
        ]
        super().__init__(shapes=columns)


plate_thickness = 3
plate = Plate(keyboard, plate_thickness)

show(
    plate,
    reset_camera=Camera.KEEP,
)
