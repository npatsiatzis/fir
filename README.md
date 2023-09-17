![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/regression.yml/badge.svg)
![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/coverage.yml/badge.svg)
![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/formal.yml/badge.svg)
![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/regression_pyuvm.yml/badge.svg)
![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/coverage_pyuvm.yml/badge.svg)
![example workflow](https://github.com/npatsiatzis/fir/actions/workflows/regression.yml/badge.svg)
[![codecov](https://codecov.io/gh/npatsiatzis/fir/graph/badge.svg?token=RZL1230739)](https://codecov.io/gh/npatsiatzis/fir)

### FIR filter (transposed form) RTL implementation

-- RTL code in:
- [VHDL](https://github.com/npatsiatzis/fir/tree/main/rtl/VHDL)
- [SystemVerilog](https://github.com/npatsiatzis/fir/tree/main/rtl/SystemVerilog)

-- Functional verification with methodologies:
- [cocotb](https://github.com/npatsiatzis/fir/tree/main/cocotb_sim)
- [pyuvm](https://github.com/npatsiatzis/fir/tree/main/pyuvm_sim)
- [verilator](https://github.com/npatsiatzis/fir/tree/main/verilator_sim)
- [formal](https://github.com/npatsiatzis/fir/tree/main/formal)


| Folder | Description |
| ------ | ------ |
| [rtl/SystemVerilog](https://github.com/npatsiatzis/fir/tree/main/rtl/SystemVerilog) | SV RTL implementation files |
| [rtl/VHDL](https://github.com/npatsiatzis/fir/tree/main/rtl/VHDL) | VHDL RTL implementation files |
| [cocotb_sim](https://github.com/npatsiatzis/fir/tree/main/cocotb_sim) | Functional Verification with CoCoTB (Python-based) |
| [pyuvm_sim](https://github.com/npatsiatzis/fir/tree/main/pyuvm_sim) | Functional Verification with pyUVM (Python impl. of UVM standard) |
| [verilator_sim](https://github.com/npatsiatzis/fir/tree/main/verilator_sim) | Functional Verification with Verilator (C++ based) |


<!-- 
This is the tree view of the strcture of the repo.
<pre>
<font size = "2">
.
├── <font size = "4"><b><a href="https://github.com/npatsiatzis/fir/tree/main/rtl">rtl</a></b> </font>
│   ├── <font size = "4"><a href="https://github.com/npatsiatzis/fir/tree/main/rtl/SystemVerilog">SystemVerilog</a> </font>
│   │   └── SV files
│   └── <font size = "4"><a href="https://github.com/npatsiatzis/fir/tree/main/rtl/VHDL">VHDL</a> </font>
│       └── VHD files
├── <font size = "4"><b><a href="https://github.com/npatsiatzis/fir/tree/main/cocotb_sim">cocotb_sim</a></b></font>
│   ├── Makefile
│   └── python files
├── <font size = "4"><b><a 
 href="https://github.com/npatsiatzis/fir/tree/main/pyuvm_sim">pyuvm_sim</a></b></font>
│   ├── Makefile
│   └── python files
└── <font size = "4"><b><a href="https://github.com/npatsiatzis/fir/tree/main/verilator_sim">verilator_sim</a></b></font>
    ├── Makefile
    └── verilator tb

</pre> -->