from cocotb_test.simulator import run
from cocotb.binary import BinaryValue
import pytest
import os

vhdl_compile_args = "--std=08"
sim_args = "--wave=wave.ghw"


tests_dir = os.path.abspath(os.path.dirname(__file__)) #gives the path to the test(current) directory in which this test.py file is placed
rtl_dir = tests_dir                                    #path to hdl folder where .vhdd files are placed
# or better yet make rtl_dir = tests_dir+"../rtl/VHDL/S"
      

#run tests with generic values for length
@pytest.mark.parametrize("g_i_W", [str(i) for i in range(7,9,1)])
@pytest.mark.parametrize("g_coeff_A", [str(i) for i in range(-32,31,40)])
@pytest.mark.parametrize("g_coeff_B", [str(i) for i in range(-32,31,35)])
@pytest.mark.parametrize("g_coeff_C", [str(i) for i in range(-32,31,25)])
@pytest.mark.parametrize("g_coeff_D", [str(i) for i in range(-32,31,15)])
def test_spi(g_i_W,g_coeff_A,g_coeff_B,g_coeff_C,g_coeff_D):

    module = "testbench"
    toplevel = "fir_transposed"   
    vhdl_sources = [
        os.path.join(rtl_dir, "../rtl/VHDL/fir_transposed.vhd"),
        ]


    parameter = {}
    parameter['g_i_W'] = g_i_W
    parameter['g_coeff_A'] = g_coeff_A
    parameter['g_coeff_B'] = g_coeff_B
    parameter['g_coeff_C'] = g_coeff_C
    parameter['g_coeff_D'] = g_coeff_D


    run(
        python_search=[tests_dir],                         #where to search for all the python test files
        vhdl_sources=vhdl_sources,
        toplevel=toplevel,
        module=module,

        vhdl_compile_args=[vhdl_compile_args],
        toplevel_lang="vhdl",
        parameters=parameter,                              #parameter dictionary
        extra_env=parameter,
        sim_build="sim_build/"
        + "_".join(("{}={}".format(*i) for i in parameter.items())),
    )
