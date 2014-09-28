setMode -bscan
setCable -p auto
addDevice -position 1 -part xc3s500e -file ./flash-500.bit
addDevice -position 2 -part xcf04s
addDevice -position 3 -part xc2c64a
program -e -v -p 1
quit
