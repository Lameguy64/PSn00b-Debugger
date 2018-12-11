; cop0 register constants
BPC			equs	"r3"
BDA			equs	"r5"
JUMPDEST	equs	"r6"
DCIC		equs	"r7"
BADVADDR	equs	"r8"
BDAM		equs	"r9"
BPCM		equs	"r11"
SR			equs	"r12"
CAUSE		equs	"r13"
EPC			equs	"r14"
PRID		equs	"r15"

	rsreset
baddr		rw	1
binst		rw	1
bsize		rw	1
	
	rsreset
rzero		rw	1
rat			rw	1
rv0			rw	1
rv1			rw	1
ra0			rw	1
ra1			rw	1
ra2			rw	1
ra3			rw	1
rt0			rw	1
rt1			rw	1
rt2			rw	1
rt3			rw	1
rt4			rw	1
rt5			rw	1
rt6			rw	1
rt7			rw	1
rs0			rw	1
rs1			rw	1
rs2			rw	1
rs3			rw	1
rs4			rw	1
rs5			rw	1
rs6			rw	1
rs7			rw	1
rt8			rw	1
rt9			rw	1
rk0			rw	1
rk1			rw	1
rgp			rw	1
rsp			rw	1
rfp			rw	1
rra			rw	1
rhi			rw	1
rlo			rw	1
rstatus		rw	1
rcause		rw	1
repc		rw	1
regsize		rw	1
