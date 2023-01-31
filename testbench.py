import cocotb
from cocotb.clock import Clock
from cocotb.triggers import Timer,RisingEdge,FallingEdge,ClockCycles
from cocotb.result import TestFailure
from cocotb.binary import BinaryValue
import random
from cocotb_coverage.coverage import CoverCross,CoverPoint,coverage_db
from cocotb_coverage import crv 
import numpy as np

g_i_w = int(cocotb.top.g_i_w)
g_t_W = int(cocotb.top.g_t_w)
g_o_w = int(cocotb.top.g_o_w)
g_taps = int(cocotb.top.g_taps)

g_coeff_A =  BinaryValue(value=str(cocotb.top.g_coeff_A),bigEndian=False ,n_bits=32,binaryRepresentation=2)
g_coeff_A = g_coeff_A.integer

g_coeff_B =  BinaryValue(value=str(cocotb.top.g_coeff_B),bigEndian=False ,n_bits=32,binaryRepresentation=2)
g_coeff_B = g_coeff_B.integer

g_coeff_C =  BinaryValue(value=str(cocotb.top.g_coeff_C),bigEndian=False ,n_bits=32,binaryRepresentation=2)
g_coeff_C = g_coeff_C.integer

g_coeff_D =  BinaryValue(value=str(cocotb.top.g_coeff_D),bigEndian=False ,n_bits=32,binaryRepresentation=2)
g_coeff_D = g_coeff_D.integer

g_sys_clk = 400000
period_ns = 10**9 / g_sys_clk

class crv_inputs(crv.Randomized):
	def __init__(self,data):
		crv.Randomized.__init__(self)
		self.data = data
		self.add_rand("data",list(range(-2**(g_i_w-1),2**(g_i_w-1))))


covered_value = []

full = False
# #Callback function to capture the bin content showing
def notify_full():
	global full
	full = True

# at_least = value is superfluous, just shows how you can determine the amount of times that
# a bin must be hit to considered covered
# actually the bins must go up to 2**8 and also add other coverage criteria regarding other features
# here i just exercize the basic functionality
@CoverPoint("top.i_data",xf = lambda x : x, bins = list(range(-2**(g_i_w-1),2**(g_i_w-1))), at_least=1)
def number_cover(x):
	covered_value.append(x)

async def reset(dut,cycles=1):
	dut.i_rst.value = 1
	dut.i_en.value = 0 
	dut.i_sample.value = 0
	await ClockCycles(dut.i_clk,cycles)
	dut.i_rst.value = 0
	await RisingEdge(dut.i_clk)
	dut._log.info("the core was reset")

@cocotb.test()
async def test(dut):
	"""Check results and coverage for spi_master"""

	cocotb.start_soon(Clock(dut.i_clk, period_ns, units="ns").start())
	await reset(dut,5)	


	sample_lst = []
	fir_lst = []

	inputs = crv_inputs(0)
	inputs.randomize()

	sample = 0
	result = 0
	fir_coeff = [g_coeff_A, g_coeff_B, g_coeff_C, g_coeff_D]

	dut.i_en.value = 1
	if(inputs.data != 0):
		dut.i_sample.value = BinaryValue(value=inputs.data,bigEndian=False ,n_bits=g_i_w,binaryRepresentation=2)
	else:
		dut.i_sample.value = BinaryValue(value=str(0),bigEndian=False ,n_bits=g_i_w,binaryRepresentation=2)

	while(full != True):
		await RisingEdge(dut.i_clk)

		result = BinaryValue(value=str(dut.o_result.value),binaryRepresentation=2)
		sample = BinaryValue(value=str(dut.i_sample.value),binaryRepresentation=2)
		
		fir_lst.append(result.integer)
		sample_lst.append(sample.integer)
		number_cover(sample.integer)
		coverage_db["top.i_data"].add_threshold_callback(notify_full, 100)
		if(full == True):
			break
		else:
			# inputs.randomize()
			while (inputs.data in covered_value):
				inputs.randomize()
			if(inputs.data != 0):
				dut.i_sample.value = BinaryValue(value=inputs.data,bigEndian=False ,n_bits=g_i_w,binaryRepresentation=2)
			else:
				dut.i_sample.value = BinaryValue(value=str(0),bigEndian=False ,n_bits=g_i_w,binaryRepresentation=2)



	for i in range(3):
		await RisingEdge(dut.i_clk)
		result = BinaryValue(value=str(dut.o_result.value),binaryRepresentation=2)
		fir_lst.append(result.integer)
	
	for i in range(3):
		fir_lst.pop(0)

	print("sample lst is {}".format(sample_lst))
	print("fir lst is {}".format(fir_lst))

	for i in range(len(sample_lst)):
		if(i>= g_taps):
			# pass
			# assert not (int(sum(sample_lst[(i-2**g_m_W +1):i+1]) / (2**g_m_W)) != fir_lst[i]),"Different expected to actual read data"
			assert not (np.dot(sample_lst[i- g_taps +1 :i+1][::-1],fir_coeff) != fir_lst[i]),"Different expected to actual read data"
		else:
			# assert not (int(sum(sample_lst[0:i+1]) / (2**g_m_W)) != fir_lst[i]),"Different expected to actual read data"
			assert not (np.dot(sample_lst[0:i+1][::-1],fir_coeff[0:i+1]) != fir_lst[i]),"Different expected to actual read data"

	coverage_db.report_coverage(cocotb.log.info,bins=True)
	coverage_db.export_to_xml(filename="coverage.xml")

		
