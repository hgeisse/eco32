lbl:
.code
  ble $1, $2, lbl
  bleu $1, $2, lbl
  blt $1, $2, lbl
  bltu $1, $2, lbl
  bge $1, $2, lbl
  bgeu $1, $2, lbl
  bgt $1, $2, lbl
  bgtu $1, $2, lbl

  dci
  dcf
  ici
  cci
  ccs

  tbs
  tbwr
  tbri
  tbwi

  trap

  fadd $1, $2, $3
  fsub $1, $2, $3
  fmul $1, $2, $3
  fdiv $1, $2, $3

