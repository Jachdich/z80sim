ld hl, 0xABCD
ld (0x1000), hl
ld hl, 0x1234
ld (0x1002), hl
ld hl, 0x2345
ld (0x1004), hl
ld hl, 0x9876
ld (0x1006), hl
ld hl, 0x6969
ld (0x1008), hl

ld hl, 0x1000
ld de, 0x2000
ld bc, 0x000A
ldir
halt