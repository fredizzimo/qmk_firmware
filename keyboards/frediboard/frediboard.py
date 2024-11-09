from dataclasses import dataclass
from build123d import *
from ocp_vscode import *

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


switch_hoder = SwitchHolder(cherry_mx_switch_holder_cfg, 3)

show(
    switch_hoder,
    reset_camera=Camera.KEEP,
)
