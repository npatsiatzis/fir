-- Transposed form FIR filter with fixed tap values
-- More practical and efficient FPGA implementation compared to Direct form FIR

library ieee;
	use ieee.std_logic_1164.all;
	use ieee.numeric_std.all;

entity fir_transposed is
	generic (
		g_taps    : natural := 4;
		g_i_w     : natural := 9;
		g_t_w     : natural := 8;
		g_o_w     : natural := 23;
		g_coeff_A : integer := -1;
		g_coeff_B : integer := -22;
		g_coeff_C : integer := 13;
		g_coeff_D : integer := -44
	);
	port (
		i_clk 	 : in    std_ulogic;
		i_rst 	 : in    std_ulogic;
		i_en 	 : in    std_ulogic;
		i_sample : in    std_ulogic_vector(g_i_w - 1 downto 0);
		o_result : out   std_ulogic_vector(g_o_w - 1 downto 0)
	);
end entity fir_transposed;

architecture rtl of fir_transposed is

	-- Vivado does not infer DSP for constant multiplier so force DSP
	-- http://www.xilinx.com/support/answers/60913.html
	attribute use_dsp : string;
	attribute use_dsp of rtl : architecture is "yes";

	type coeff_array is array(0 to g_taps - 1) of signed(g_t_w - 1 downto 0);

	type mul_array is array(0 to g_taps - 1) of signed(g_i_w + g_t_w - 1 downto 0);

	type dsp_array is array(0 to g_taps - 1) of signed(g_o_w - 1 downto 0);

	signal r_sample : signed(g_i_w - 1 downto 0);
	constant r_tap  : coeff_array :=
	(to_signed(g_coeff_A, g_t_w), to_signed(g_coeff_B, g_t_w), to_signed(g_coeff_C, g_t_w), to_signed(g_coeff_D, g_t_w));
	signal r_mul 	: mul_array;
	signal r_mac	: dsp_array;

begin

	fir_transp : process (i_clk) is
	begin

		if (i_rst = '1') then
			r_sample <= (others => '0');
			r_mul 	 <= (others => (others => '0'));
			r_mac	 <= (others => (others => '0'));
		elsif (rising_edge(i_clk)) then
			if (i_en = '1') then
				r_sample <= signed(i_sample);

				for i in 0 to g_taps - 1 loop

					if (i< g_taps - 1) then
						r_mul(i) <= r_sample * r_tap(i);
						r_mac(i) <= r_mul(i) + r_mac(i + 1);
					elsif (i = g_taps - 1) then
						r_mul(i) <= r_sample * r_tap(i);
						r_mac(i) <= resize(r_mul(i), g_o_w);
					end if;

				end loop;

			end if;
		end if;

	end process fir_transp;

	o_result <= std_ulogic_vector(r_mac(0));

end architecture rtl;