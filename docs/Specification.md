
## Requirements Specification


### 1. SCOPE

1. **Scope**

   This document establishes the requirements for an Intellectual Property (IP) that provides a fir filter function.
1. **Purpose**
 
   These requirements shall apply to a fir filter core with a simple interface for inclusion as a component.
1. **Classification**
    
   This document defines the requirements for a hardware design.


### 2. DEFINITIONS

1. **Width**

   Number of bits of samples.
2. **Tap**
   
   A FIR's taps are coefficient values. The number of taps is the amount of memory (length) needed to implement the filter.

### 3. APPLICABLE DOCUMENTS 

1. **Government Documents**

   None
1. **Non-government Documents**

   None


### 4. ARCHITECTURAL OVERVIEW

1. **Introduction**

   The fir filter component shall represent a design written in an HDL (VHDL and/or SystemVerilog) that can easily be incorporateed into a larger design.This fir filter shall include the following features : 
     1. Parameterized sample width, number of taps, tap width and tap values.

   The CPU interface in this case is a simple valid interface.

1. **System Application**
   
    The fir filter can be applied to a variety of system configurations. An example use case is to be used to transform data between an upstream producer and a downstream consumer.

### 5. PHYSICAL LAYER

1. en, input data valid
6. i_sample, input data word
7. o_result, fir filter output
7. clk, system clock
8. rst, system reset, synchronous active high

### 6. PROTOCOL LAYER

The fir average filter operates on valid input data samples being averaged with their neighbors and producing a filtered value.

### 7. ROBUSTNESS

Does not apply.

### 8. HARDWARE AND SOFTWARE

1. **Parameterization**

   The fir filter shall provide for the following parameters used for the definition of the implemented hardware during hardware build:

   | Param. Name | Description |
   | :------: | :------: |
   | word width | width of the CPU data interface |
   | baud rate | symbol(bits) / sec |
   | system clock frequency | frequency of the clock provided to the fir filter core |
   | oversample rate | rate for which the receiver oversamples the line (i.e multiple of the baud rate) |
   | parity type | even or odd parity of the word 

1. **CPU interface**

   The CPU shall write into the fir filter data for transmission and also read from the receive register.

   | addr | we (+ stb) | Description |
   | :------: | :------: | :------: | 
   | 0 | 1 | set word to transmit |
   | 1 | 0 | read word from receiver |

   (* Obviously, only the bare minimum of fir filter features are implemented, namely the transmit and receive   functionality).

### 9. PERFORMANCE

1. **Frequency**
1. **Power Dissipation**
1. **Environmental**
 
   Does not apply.
1. **Technology**

   The design shall be adaptable to any technology because the design shall be portable and defined in an HDL.

### 10. TESTABILITY
None required.

### 11. MECHANICAL
Does not apply.
