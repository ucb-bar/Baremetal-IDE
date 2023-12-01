# PCB Footprint Naming Convention

All footprint names should be using ALL CAPS.





## Resistors

`RES-{package}`



## Capacitors

### Unpolarized (ceramic)

`CAPA-{package}`



### Electrolytic

`CAPPR-D{diameter}_L{length}_W{width}`





## Headers

`HDR-{gender}-{pitch}_{rows}X{columns}`



## Connectors

`CONN-{SMD|TH}_{name}`



## Switches

`SW-{SMD|TH}_{lead_count}P-L{length}-W{width}-P{pitch}-LS{}`





## IC Packages



### BGA (Ball Grid Array)

`BGA-{ball_count}_L{length}-W{width}-R{rows}-C{columns}-P{pitch}-{corner}`



### FBGA (Fine-pitch Ball Grid Array)

`FBGA-{ball_count}_L{length}-W{width}-R{rows}-C{columns}-P{pitch}-{corner}`



### SOIC (Small Outline Integrated Circuit)

`SOIC-{pin_count}_L{length}-W{width}-P{pitch}-LS{}-{corner}`



### SOP (Small Outline Package)

`SOP-{pin_count}_L{length}-W{width}-P{pitch}-LS{}-{corner}`



### SOT (Small Outline Transistor)

`SOT-{package}_L{length}-W{width}-P{pitch}-{corner}`



### TSSOP (Thin Shrink Small Outline Package)

`TSSOP-{package}_L{length}-W{width}-P{pitch}-{corner}`





















