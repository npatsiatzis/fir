![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/regression.yml/badge.svg)
![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/coverage.yml/badge.svg)
![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/formal.yml/badge.svg)

### FIR filter (transposed form) RTL implementation


- CoCoTB testbench for functional verification
    - $ make
- CoCoTB-test unit testing to exercise the CoCoTB tests across a range of values for the generic parameters
    - $  SIM=ghdl pytest -n auto -o log_cli=True --junitxml=test-results.xml --cocotbxml=test-cocotb.xml
- formal verification using SymbiYosys (sby), (properties specified in PSL)
    - $ make formal

