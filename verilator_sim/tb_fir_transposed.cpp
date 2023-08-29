// Verilator Example
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <set>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <verilated_cov.h>
#include "Vfir_transposed.h"
#include "Vfir_transposed_fir_transposed.h"   //to get parameter values, after they've been made visible in SV


#define POSEDGE(ns, period, phase) \
    ((ns) % (period) == (phase))

#define NEGEDGE(ns, period, phase) \
    ((ns) % (period) == ((phase) + (period)) / 2 % (period))

#define CLK_A_PERIOD 30
#define CLK_A_PHASE 0


#define MAX_SIM_TIME 300
#define VERIF_START_TIME 2*CLK_A_PERIOD
vluint64_t sim_time = 0;
vluint64_t posedge_cnt = 0;

// input interface transaction item class
class InTx {
    private:
    public:
        int i_en;
        int i_sample;
};


// output interface transaction item class
class OutTx {
    public:
        int o_result;
};

//in domain Coverage
class InCoverage{
    private:
        std::set <int> in_cvg;
    
    public:
        void write_coverage(InTx *tx){
            in_cvg.insert(tx->i_sample);
        }

        bool is_covered(int A){            
            return in_cvg.find(A) == in_cvg.end();
        }
};

//out domain Coverage
class OutCoverage {
    private:
        std::set <int> coverage;
        int cvg_size = 0;

    public:
        void write_coverage(OutTx* tx){
            coverage.insert(tx->o_result);
            cvg_size++;
        }

        bool is_full_coverage(){
            return cvg_size == (1 << (Vfir_transposed_fir_transposed::G_I_W))-1;
            // return coverage.size() == (1 << (Vfir_transposed_fir_transposed::G_I_W));
        }
};


// ALU scoreboard
class Scb {
    private:
        // std::deque<InTx*> in_q;
        // std::deque<OutTx*> out_q;
        std::vector<int> in_vec;
        std::vector<int> out_vec;

        int get_2s_compl (int value, int num_of_bits) {
            if((value + (1<<(num_of_bits-1))) >(1<<(num_of_bits))-1) {
                return value - (1 << num_of_bits);
            }
            else {
                return value;
            }
        }

        // Function to slice a given vector
        // from range X to Y
        int slice_and_dot(std::vector<int>& arr,std::vector<int>& coeff_vec,
                            int X, int Y)
        {
         
            // Starting and Ending iterators
            auto start = arr.begin() + X;
            auto end = arr.begin() + Y + 1;
         
            // To store the sliced vector
            std::vector<int> result(Y - X + 1);
         
            // Copy vector using copy function()
            copy(start, end, result.begin());
         
            // // Return the final sliced vector
            // return result;
            int dot_result = 0;
            for (int i = 0; i< result.size(); i++){
            // for (auto i = result.begin(); i != result.end(); i++){
                // ma_sum += *i;
                dot_result += result[i]*coeff_vec[i];
            }
            // dot_result = ma_sum / (1 << Vfir_transposed_fir_transposed::G_M_W);
            return dot_result;
        }

    public:
        // Input interface monitor port
        void writeIn(InTx *tx){
            // Push the received transaction item into a queue for later
            // in_q.push_back(tx);
            in_vec.push_back(tx->i_sample);
            delete tx;
        }

        // Output interface monitor port
        void writeOut(OutTx *tx){
            // Push the received transaction item into a queue for later
            // out_q.push_back(tx);
            out_vec.push_back(tx->o_result);
            delete tx;
        }

        void checkPhase(){
            int expected_result = 0;
            const int G_COEFF_A = get_2s_compl(Vfir_transposed_fir_transposed::G_COEFF_A,Vfir_transposed_fir_transposed::G_T_W);
            const int G_COEFF_B = get_2s_compl(Vfir_transposed_fir_transposed::G_COEFF_B,Vfir_transposed_fir_transposed::G_T_W);
            const int G_COEFF_C = get_2s_compl(Vfir_transposed_fir_transposed::G_COEFF_C,Vfir_transposed_fir_transposed::G_T_W);
            const int G_COEFF_D = get_2s_compl(Vfir_transposed_fir_transposed::G_COEFF_D,Vfir_transposed_fir_transposed::G_T_W);

            std::vector<int> coeff_vec = {G_COEFF_D,G_COEFF_C,G_COEFF_B,G_COEFF_A};

            for (int i = Vfir_transposed_fir_transposed::G_TAPS-1; i<out_vec.size()-1; i++) {
                expected_result = slice_and_dot(in_vec,coeff_vec,i - Vfir_transposed_fir_transposed::G_TAPS+1, i);

                if(expected_result != get_2s_compl(out_vec[i],Vfir_transposed_fir_transposed::G_O_W)){
                    std::cout << "Test Failure!" << std::endl;
                    std::cout << "Expected : " <<  get_2s_compl(expected_result,Vfir_transposed_fir_transposed::G_O_W) << std::endl;
                    std::cout << "Got : " << get_2s_compl(out_vec[i],Vfir_transposed_fir_transposed::G_O_W) << std::endl;
                    exit(1);
                } else {
                    std::cout << "Test PASS!" << std::endl;
                    std::cout << "Expected : " <<  get_2s_compl(expected_result,Vfir_transposed_fir_transposed::G_O_W) << std::endl;
                    std::cout << "Got : " << get_2s_compl(out_vec[i],Vfir_transposed_fir_transposed::G_O_W) << std::endl;   
                }
            }
        }

};

// interface driver
class InDrv {
    private:
        // Vfir_transposed *dut;
        std::shared_ptr<Vfir_transposed> dut;
        int state;
    public:
        InDrv(std::shared_ptr<Vfir_transposed> dut){
            this->dut = dut;
            state = 0;
        }

        void drive(InTx *tx, int & new_tx_ready,int is_a_pos){

            // Don't drive anything if a transaction item doesn't exist


            if(tx != NULL && is_a_pos == 1){
                dut->i_en = 1;
                dut->i_sample = tx->i_sample;

                new_tx_ready = 1;
                delete tx;
            }

        }
};

// input interface monitor
class InMon {
    private:
        // Vfir_transposed *dut;
        std::shared_ptr<Vfir_transposed> dut;
        // Scb *scb;
        std::shared_ptr<Scb>  scb;
        // InCoverage *cvg;
        std::shared_ptr<InCoverage> cvg;
    public:
        InMon(std::shared_ptr<Vfir_transposed> dut, std::shared_ptr<Scb>  scb, std::shared_ptr<InCoverage> cvg){
            this->dut = dut;
            this->scb = scb;
            this->cvg = cvg;
        }

        void monitor(int is_a_pos){
            // if(dut->i_valid == 1){
            if(is_a_pos ==1 && dut->i_en == 1) {
                InTx *tx = new InTx();
                tx->i_sample = dut->i_sample;
                // then pass the transaction item to the scoreboard
                scb->writeIn(tx);
                cvg->write_coverage(tx);
            }
        }
};

// ALU output interface monitor
class OutMon {
    private:
        // Vfir_transposed *dut;
        std::shared_ptr<Vfir_transposed> dut;
        // Scb *scb;
        std::shared_ptr<Scb> scb;
        // OutCoverage *cvg;
        std::shared_ptr<OutCoverage> cvg;
        int state;
    public:
        OutMon(std::shared_ptr<Vfir_transposed> dut, std::shared_ptr<Scb> scb, std::shared_ptr<OutCoverage> cvg){
            this->dut = dut;
            this->scb = scb;
            this->cvg = cvg;
            state = 0;
        }

        void monitor(int is_a_pos){


            switch(state) {
                case 0:
                    if(is_a_pos == 1 && dut->i_en == 1) {
                        state = 1;
                     }

                    break;
                case 1:
                    if(is_a_pos == 1 && dut->i_en == 1) {
                        state = 2;
                    }
                    break;
                case 2:
                    if(is_a_pos == 1 && dut->i_en == 1) {
                        state = 3;
                    }
                    break;
                case 3: 
                    if(is_a_pos == 1 && dut->i_en == 1) {
                        state = 3;
                        OutTx *tx = new OutTx();
                        tx->o_result = dut->o_result;

                        // then pass the transaction item to the scoreboard
                        scb->writeOut(tx);
                        cvg->write_coverage(tx);
                    }
                    break;
                default:
                    state = 0;
            }
        }
};

//sequence (transaction generator)
// coverage-driven random transaction generator
// This will allocate memory for an InTx
// transaction item, randomise the data, until it gets
// input values that have yet to be covered and
// return a pointer to the transaction item object
class Sequence{
    private:
        InTx* in;
        // InCoverage *cvg;
        std::shared_ptr<InCoverage> cvg;
    public:
        Sequence(std::shared_ptr<InCoverage> cvg){
            this->cvg = cvg;
        }

        InTx* genTx(int & new_tx_ready){
            in = new InTx();
            // std::shared_ptr<InTx> in(new InTx());
            if(new_tx_ready == 1){
                in->i_sample = rand() % (1 << (Vfir_transposed_fir_transposed::G_I_W -1));   

                while(cvg->is_covered(in->i_sample) == false){
                    in->i_sample = rand() % (1 << (Vfir_transposed_fir_transposed::G_I_W -1));  

                }
                return in;
            } else {
                return NULL;
            }
        }
};


void dut_reset (std::shared_ptr<Vfir_transposed> dut, vluint64_t &sim_time){
    dut->i_rst = 0;
    if(sim_time >= 0 && sim_time < VERIF_START_TIME-1){
        dut->i_rst = 1;
    }
}

void simulation_eval(std::shared_ptr<Vfir_transposed> dut,VerilatedVcdC *m_trace, vluint64_t & ns)
{
    dut->eval();
    m_trace->dump(ns);
}

void simulation_tick_posedge(std::shared_ptr<Vfir_transposed> dut)
{   
    dut->i_clk = 1;
    
}

void simulation_tick_negedge(std::shared_ptr<Vfir_transposed> dut)
{
    dut->i_clk = 0;
    
}


int main(int argc, char** argv, char** env) {
    srand (time(NULL));
    Verilated::commandArgs(argc, argv);
    // Vfir_transposed *dut = new Vfir_transposed;
    std::shared_ptr<Vfir_transposed> dut(new Vfir_transposed);

    Verilated::traceEverOn(true);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    dut->trace(m_trace, 5);
    m_trace->open("waveform.vcd");

    InTx   *tx;
    int new_tx_ready = 1;

    // Here we create the driver, scoreboard, input and output monitor and coverage blocks
    std::unique_ptr<InDrv> drv(new InDrv(dut));
    std::shared_ptr<Scb> scb(new Scb());
    std::shared_ptr<InCoverage> inCoverage(new InCoverage());
    std::shared_ptr<OutCoverage> outCoverage(new OutCoverage());
    std::unique_ptr<InMon> inMon(new InMon(dut,scb,inCoverage));
    std::unique_ptr<OutMon> outMon(new OutMon(dut,scb,outCoverage));
    std::unique_ptr<Sequence> sequence(new Sequence(inCoverage));

    while (outCoverage->is_full_coverage() == false) {
    // while(sim_time < MAX_SIM_TIME*20) {
        // random reset 
        // 0-> all 0s
        // 1 -> all 1s
        // 2 -> all random
        Verilated::randReset(2); 
        dut_reset(dut,sim_time);
        

        if (POSEDGE(sim_time, CLK_A_PERIOD, CLK_A_PHASE)) {
                simulation_tick_posedge(dut);
        }
        if (NEGEDGE(sim_time, CLK_A_PERIOD, CLK_A_PHASE)) {
                simulation_tick_negedge(dut);
        }
        
        simulation_eval(dut, m_trace, sim_time);


        if (sim_time >= VERIF_START_TIME) {
            // Generate a randomised transaction item 
            tx = sequence->genTx(new_tx_ready);
            // Pass the generated transaction item in the driver
            //to convert it to pin wiggles
            //operation similar to than of a connection between
            //a sequencer and a driver in a UVM tb
            drv->drive(tx,new_tx_ready,POSEDGE(sim_time, CLK_A_PERIOD, CLK_A_PHASE));
            // Monitor the input interface
            // also writes recovered transaction to
            // input coverage and scoreboard
            inMon->monitor(POSEDGE(sim_time, CLK_A_PERIOD, CLK_A_PHASE));
            // Monitor the output interface
            // also writes recovered result (out transaction) to
            // output coverage and scoreboard 
            outMon->monitor(POSEDGE(sim_time, CLK_A_PERIOD, CLK_A_PHASE));
        }
        sim_time++;
    }
    scb->checkPhase();

    VerilatedCov::write();
    m_trace->close();  
    exit(EXIT_SUCCESS);
}
