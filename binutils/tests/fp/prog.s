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
  
  beqf $1, $2, lbl
  bnef $1, $2, lbl
  blef $1, $2, lbl
  bltf $1, $2, lbl

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

  addf $1, $2, $3
  subf $1, $2, $3
  mulf $1, $2, $3
  divf $1, $2, $3

  ci2f $20, $21
  cf2i $20, $21

