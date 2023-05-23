![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/regression_pyuvm.yml/badge.svg)
![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/coverage_pyuvm.yml/badge.svg)

### FIR filter (transposed form) RTL implementation


- pyuvm testbench for functional verification
    - $ make
- CoCoTB-test unit testing to exercise the pyuvm tests across a range of values for the generic parameters
    - $  SIM=ghdl pytest -n auto -o log_cli=True --junitxml=test-results.xml --cocotbxml=test-cocotb.xml
- formal verification using SymbiYosys (sby), (properties specified in PSL)
    - $ make formal

