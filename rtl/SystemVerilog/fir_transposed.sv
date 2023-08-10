// Transposed form FIR filter with fixed tap values (and also tap number, just for test)
// More practical and efficient FPGA implementation compared to Direct form FIR

`begin_keywords "1800-2017"
`default_nettype none

module fir_transposed
    #(
        parameter int G_TAPS = 4,
        parameter int G_I_W = 9,
        parameter int G_T_W = 8,
        parameter int G_O_W = 23,

        parameter logic signed [G_T_W -1 : 0] G_COEFF_A = -1,
        parameter logic signed [G_T_W -1 : 0] G_COEFF_B = -22,
        parameter logic signed [G_T_W -1 : 0] G_COEFF_C = 13,
        parameter logic signed [G_T_W -1 : 0] G_COEFF_D = -44
    )

    (
        input  logic i_clk,
        input  logic i_rst,
        input  logic i_en,
        input  logic [G_I_W - 1 : 0] i_sample,

        output logic [G_O_W - 1 : 0] o_result
    );

    // split long line (over 100 character long originally)
    const logic signed [G_T_W -1 : 0] r_tap_arr [G_TAPS]
    = '{G_COEFF_A,G_COEFF_B,G_COEFF_C,G_COEFF_D};

    logic signed [G_I_W + G_T_W -1 : 0] r_mul_res [G_TAPS];
    logic signed [G_O_W -1 : 0] r_final_res [G_TAPS];



    logic [G_I_W - 1 : 0] r_sample;

    always_ff @(posedge i_clk) begin : fir_transp
        if(i_rst) begin
            r_sample <= '0;
        end else begin
            r_sample <= i_sample;

            for (int i =0; i < G_TAPS; i++) begin
                if(i < G_TAPS - 1) begin
                    r_mul_res[i] <= r_sample * r_tap_arr[i];
                    // size cast to appease verilator, no problem though without it
                    r_final_res[i] <= G_O_W'(r_mul_res[i]) + r_final_res[i-1];
                end else begin
                    r_mul_res[i] <= r_sample * r_tap_arr[i];
                    // size cast to appease verilator, no problem though without it
                    r_final_res[i] <= G_O_W'(r_mul_res[i]);
                end
            end
        end
    end

    assign o_result = r_final_res[0];

endmodule : fir_transposed
