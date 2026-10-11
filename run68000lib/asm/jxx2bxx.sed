s/\bjra\b/bra/g
s/\bjeq\b/beq/g
s/\bjne\b/bne/g
s/\bjcs\b/bcs/g
s/\bjcc\b/bcc/g
s/\bjhi\b/bhi/g
s/\bjls\b/bls/g
s/\bjpl\b/bpl/g
s/\bjmi\b/bmi/g
s/\bjge\b/bge/g
s/\bjlt\b/blt/g
s/\bjgt\b/bgt/g
s/\bjle\b/ble/g
s/\bjvs\b/bvs/g
s/\bjvc\b/bvc/g
s/\blink\.w\b/link/g
# --- Convert add.l <ea>,%aN -> adda.l <ea>,%aN ---
# Only if destination is an address register
s/\badd\.l\([^,]*\),\(%a[0-7]\)/adda.l\1,\2/g

# --- Convert cmp.l #imm,<ea> -> cmpi.l #imm,<ea> ---
# Only if source is immédiat
s/\bcmp\.l[ \t]*#\([^,]*\),/cmpi.l #\1,/g
