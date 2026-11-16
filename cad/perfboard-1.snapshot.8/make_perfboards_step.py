"""Build Perfboards.step: one named solid per configuration of "Perfboards Config.SLDPRT".

The rules come from the equations and the design table inside the SLDPRT.
Run with a Python that has cadquery installed:  python make_perfboards_step.py
"""
from pathlib import Path

import cadquery as cq

# Design table: configuration name -> (board width, board height), in mm.
BOARDS = {
    "80mm x 20mm": (80, 20),
    "70mm x 30mm": (70, 30),
    "60mm x 40mm": (60, 40),
    "70mm x 50mm": (70, 50),
    "90mm x 70mm": (90, 70),
}

# Equations from the SLDPRT.
HOLE_SPACING = 2.54
THROUGH_HOLE_D = 1.0
MOUNT_HOLE_D = 2.3
MOUNT_HOLE_OFFSET = MOUNT_HOLE_D / 2 + 0.6
HORZ_FIRST_OFFSET = THROUGH_HOLE_D / 2 + 5.5
VERT_FIRST_OFFSET = THROUGH_HOLE_D / 2 + 3.1
SIDE_PAD_W = 3.5
SIDE_PAD_H = 2.25

# Values measured in the IGES export.
BOARD_T = 1.5
COPPER_T = 0.05
PAD_D = 2.0
SIDE_PAD_INSET = 2.0  # board edge to side pad center

GREEN = cq.Color(0.05, 0.75, 0.15)
# The original model used a polished aluminum look for the pads (tinned copper).
PAD_COLOR = cq.Color(0.82, 0.82, 0.84)


def counts(w, h):
    nh = round((w - 2 * HORZ_FIRST_OFFSET) / HOLE_SPACING + 1)
    nv = round((h - 2 * VERT_FIRST_OFFSET) / HOLE_SPACING + 1)
    return nh, nv


def perfboard(w, h):
    nh, nv = counts(w, h)
    x0, y0 = -w / 2 + HORZ_FIRST_OFFSET, h / 2 - VERT_FIRST_OFFSET
    holes = [(x0 + i * HOLE_SPACING, y0 - j * HOLE_SPACING) for i in range(nh) for j in range(nv)]
    side_pads = [
        (sx * (w / 2 - SIDE_PAD_INSET), y0 - j * HOLE_SPACING)
        for sx in (-1, 1)
        for j in range(1, nv - 1)
    ]
    mounts = [(sx * (w / 2 - MOUNT_HOLE_OFFSET), sy * (h / 2 - MOUNT_HOLE_OFFSET)) for sx in (-1, 1) for sy in (-1, 1)]

    # Copper-free area of one face: the board outline minus the pads.
    trench = (
        cq.Sketch()
        .rect(w, h)
        .push(holes).circle(PAD_D / 2, mode="s")
        .push(side_pads).slot(SIDE_PAD_W - SIDE_PAD_H, SIDE_PAD_H, mode="s")
        .reset()
    )
    z = BOARD_T / 2 + COPPER_T
    top = cq.Workplane("XY", origin=(0, 0, BOARD_T / 2)).placeSketch(trench).extrude(COPPER_T)
    bottom = cq.Workplane("XY", origin=(0, 0, -z)).placeSketch(trench).extrude(COPPER_T)
    bores = (
        cq.Workplane("XY", origin=(0, 0, -z))
        .pushPoints(holes).circle(THROUGH_HOLE_D / 2)
        .pushPoints(mounts).circle(MOUNT_HOLE_D / 2)
        .extrude(2 * z)
    )
    body = cq.Workplane("XY").box(w, h, 2 * z).cut(top).cut(bottom).cut(bores).clean()
    return body.val(), nh, nv


def main():
    out = Path(__file__).with_name("Perfboards.step")
    asm = cq.Assembly(name="Perfboards")
    for cfg, (w, h) in BOARDS.items():
        solid, nh, nv = perfboard(w, h)
        print(f"{cfg}: {nh} x {nv} holes, {2 * (nv - 2)} side pads, valid={solid.isValid()}")
        board = cq.Assembly(solid, name=f"Perfboard {cfg}", color=GREEN)
        # Copper faces: pad tops, pad sides and plated bores all reach past the substrate.
        z = BOARD_T / 2 + 1e-3
        for f in solid.Faces():
            bb = f.BoundingBox()
            if bb.zmax > z or bb.zmin < -z:
                board.addSubshape(f, color=PAD_COLOR)
        asm.add(board)
    asm.export(str(out))
    print("wrote", out)


if __name__ == "__main__":
    main()
