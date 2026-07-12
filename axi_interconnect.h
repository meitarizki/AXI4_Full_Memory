#ifndef AXI_INTERCONNECT_H
#define AXI_INTERCONNECT_H

#include <systemc.h>

SC_MODULE(axi_interconnect) {
    sc_in<bool> ACLK, ARESETN;

    // --- WRITE CHANNELS ---
    sc_in<sc_uint<32>> AWADDR_M0, WDATA_M0; sc_in<sc_uint<8>> AWLEN_M0; sc_in<bool> AWVALID_M0, WVALID_M0, WLAST_M0, BREADY_M0;
    sc_in<bool> AWLOCK_M0; // Dispatcher Lock for M0 (Load)
    sc_out<bool> AWREADY_M0, WREADY_M0, BVALID_M0; sc_out<sc_uint<2>> BRESP_M0;

    sc_in<sc_uint<32>> AWADDR_M1, WDATA_M1; sc_in<sc_uint<8>> AWLEN_M1; sc_in<bool> AWVALID_M1, WVALID_M1, WLAST_M1, BREADY_M1;
    sc_in<bool> AWLOCK_M1; // Dispatcher Lock for M1 (Compute)
    sc_out<bool> AWREADY_M1, WREADY_M1, BVALID_M1; sc_out<sc_uint<2>> BRESP_M1;

    sc_in<sc_uint<32>> AWADDR_M2, WDATA_M2; sc_in<sc_uint<8>> AWLEN_M2; sc_in<bool> AWVALID_M2, WVALID_M2, WLAST_M2, BREADY_M2;
    sc_in<bool> AWLOCK_M2; // Dispatcher Lock for M2 (Store)
    sc_out<bool> AWREADY_M2, WREADY_M2, BVALID_M2; sc_out<sc_uint<2>> BRESP_M2;

    sc_in<sc_uint<32>> AWADDR_M3, WDATA_M3; sc_in<sc_uint<8>> AWLEN_M3; sc_in<bool> AWVALID_M3, WVALID_M3, WLAST_M3, BREADY_M3;
    sc_in<bool> AWLOCK_M3; // Dispatcher Lock for M3 (Fetch)
    sc_out<bool> AWREADY_M3, WREADY_M3, BVALID_M3; sc_out<sc_uint<2>> BRESP_M3;

    sc_out<sc_uint<32>> AWADDR_OUT, WDATA_OUT; sc_out<sc_uint<8>> AWLEN_OUT; sc_out<bool> AWVALID_OUT, WVALID_OUT, WLAST_OUT, BREADY_OUT;
    sc_in<bool> AWREADY_IN, WREADY_IN, BVALID_IN; sc_in<sc_uint<2>> BRESP_IN;

    // --- READ CHANNELS ---
    sc_in<sc_uint<32>> ARADDR_M0; sc_in<bool> ARVALID_M0; sc_out<bool> ARREADY_M0; sc_in<sc_uint<8>> ARLEN_M0;
    sc_in<bool> ARLOCK_M0; // Dispatcher Lock for M0
    sc_out<sc_uint<32>> RDATA_M0; sc_out<sc_uint<2>> RRESP_M0; sc_out<bool> RVALID_M0; sc_in<bool> RREADY_M0; sc_out<bool> RLAST_M0;

    sc_in<sc_uint<32>> ARADDR_M1; sc_in<bool> ARVALID_M1; sc_out<bool> ARREADY_M1; sc_in<sc_uint<8>> ARLEN_M1;
    sc_in<bool> ARLOCK_M1; // Dispatcher Lock for M1
    sc_out<sc_uint<32>> RDATA_M1; sc_out<sc_uint<2>> RRESP_M1; sc_out<bool> RVALID_M1; sc_in<bool> RREADY_M1; sc_out<bool> RLAST_M1;

    sc_in<sc_uint<32>> ARADDR_M2; sc_in<bool> ARVALID_M2; sc_out<bool> ARREADY_M2; sc_in<sc_uint<8>> ARLEN_M2;
    sc_in<bool> ARLOCK_M2; // Dispatcher Lock for M2
    sc_out<sc_uint<32>> RDATA_M2; sc_out<sc_uint<2>> RRESP_M2; sc_out<bool> RVALID_M2; sc_in<bool> RREADY_M2; sc_out<bool> RLAST_M2;

    sc_in<sc_uint<32>> ARADDR_M3; sc_in<bool> ARVALID_M3; sc_out<bool> ARREADY_M3; sc_in<sc_uint<8>> ARLEN_M3;
    sc_in<bool> ARLOCK_M3; // Dispatcher Lock for M3
    sc_out<sc_uint<32>> RDATA_M3; sc_out<sc_uint<2>> RRESP_M3; sc_out<bool> RVALID_M3; sc_in<bool> RREADY_M3; sc_out<bool> RLAST_M3;

    sc_out<sc_uint<32>> ARADDR_OUT; sc_out<bool> ARVALID_OUT; sc_in<bool> ARREADY_IN; sc_out<sc_uint<8>> ARLEN_OUT;
    sc_in<sc_uint<32>> RDATA_IN; sc_in<sc_uint<2>> RRESP_IN; sc_in<bool> RVALID_IN; sc_out<bool> RREADY_OUT; sc_in<bool> RLAST_IN;

    // --- STATE SIGNALS ---
    sc_signal<int> active_master;
    sc_signal<int> active_read_master;

    // Combinational routing logic
    void route_signals() {
        // --- WRITE CHANNEL ROUTING ---
        int target_write = -1;
        if (active_master.read() != -1) {
            target_write = active_master.read();
        } else {
            // Priority selection if arbiter is idle
            if (AWLOCK_M0.read() == 1 && AWVALID_M0.read() == 1) target_write = 0;
            else if (AWLOCK_M1.read() == 1 && AWVALID_M1.read() == 1) target_write = 1;
            else if (AWLOCK_M2.read() == 1 && AWVALID_M2.read() == 1) target_write = 2;
            else if (AWLOCK_M3.read() == 1 && AWVALID_M3.read() == 1) target_write = 3;
            else if (AWVALID_M0.read() == 1) target_write = 0;
            else if (AWVALID_M1.read() == 1) target_write = 1;
            else if (AWVALID_M2.read() == 1) target_write = 2;
            else if (AWVALID_M3.read() == 1) target_write = 3;
        }

        if (target_write == 0) {
            AWADDR_OUT.write(AWADDR_M0.read());
            AWLEN_OUT.write(AWLEN_M0.read());
            AWVALID_OUT.write(AWVALID_M0.read());
            AWREADY_M0.write(AWREADY_IN.read());
            AWREADY_M1.write(0); AWREADY_M2.write(0); AWREADY_M3.write(0);

            WDATA_OUT.write(WDATA_M0.read());
            WVALID_OUT.write(WVALID_M0.read());
            WLAST_OUT.write(WLAST_M0.read());
            WREADY_M0.write(WREADY_IN.read());
            WREADY_M1.write(0); WREADY_M2.write(0); WREADY_M3.write(0);

            BRESP_M0.write(BRESP_IN.read());
            BVALID_M0.write(BVALID_IN.read());
            BVALID_M1.write(0); BVALID_M2.write(0); BVALID_M3.write(0);
            BREADY_OUT.write(BREADY_M0.read());
        } else if (target_write == 1) {
            AWADDR_OUT.write(AWADDR_M1.read());
            AWLEN_OUT.write(AWLEN_M1.read());
            AWVALID_OUT.write(AWVALID_M1.read());
            AWREADY_M1.write(AWREADY_IN.read());
            AWREADY_M0.write(0); AWREADY_M2.write(0); AWREADY_M3.write(0);

            WDATA_OUT.write(WDATA_M1.read());
            WVALID_OUT.write(WVALID_M1.read());
            WLAST_OUT.write(WLAST_M1.read());
            WREADY_M1.write(WREADY_IN.read());
            WREADY_M0.write(0); WREADY_M2.write(0); WREADY_M3.write(0);

            BRESP_M1.write(BRESP_IN.read());
            BVALID_M1.write(BVALID_IN.read());
            BVALID_M0.write(0); BVALID_M2.write(0); BVALID_M3.write(0);
            BREADY_OUT.write(BREADY_M1.read());
        } else if (target_write == 2) {
            AWADDR_OUT.write(AWADDR_M2.read());
            AWLEN_OUT.write(AWLEN_M2.read());
            AWVALID_OUT.write(AWVALID_M2.read());
            AWREADY_M2.write(AWREADY_IN.read());
            AWREADY_M0.write(0); AWREADY_M1.write(0); AWREADY_M3.write(0);

            WDATA_OUT.write(WDATA_M2.read());
            WVALID_OUT.write(WVALID_M2.read());
            WLAST_OUT.write(WLAST_M2.read());
            WREADY_M2.write(WREADY_IN.read());
            WREADY_M0.write(0); WREADY_M1.write(0); WREADY_M3.write(0);

            BRESP_M2.write(BRESP_IN.read());
            BVALID_M2.write(BVALID_IN.read());
            BVALID_M0.write(0); BVALID_M1.write(0); BVALID_M3.write(0);
            BREADY_OUT.write(BREADY_M2.read());
        } else if (target_write == 3) {
            AWADDR_OUT.write(AWADDR_M3.read());
            AWLEN_OUT.write(AWLEN_M3.read());
            AWVALID_OUT.write(AWVALID_M3.read());
            AWREADY_M3.write(AWREADY_IN.read());
            AWREADY_M0.write(0); AWREADY_M1.write(0); AWREADY_M2.write(0);

            WDATA_OUT.write(WDATA_M3.read());
            WVALID_OUT.write(WVALID_M3.read());
            WLAST_OUT.write(WLAST_M3.read());
            WREADY_M3.write(WREADY_IN.read());
            WREADY_M0.write(0); WREADY_M1.write(0); WREADY_M2.write(0);

            BRESP_M3.write(BRESP_IN.read());
            BVALID_M3.write(BVALID_IN.read());
            BVALID_M0.write(0); BVALID_M1.write(0); BVALID_M2.write(0);
            BREADY_OUT.write(BREADY_M3.read());
        } else {
            AWADDR_OUT.write(0);
            AWLEN_OUT.write(0);
            AWVALID_OUT.write(0);
            AWREADY_M0.write(0); AWREADY_M1.write(0); AWREADY_M2.write(0); AWREADY_M3.write(0);

            WDATA_OUT.write(0);
            WVALID_OUT.write(0);
            WLAST_OUT.write(0);
            WREADY_M0.write(0); WREADY_M1.write(0); WREADY_M2.write(0); WREADY_M3.write(0);

            BVALID_M0.write(0); BVALID_M1.write(0); BVALID_M2.write(0); BVALID_M3.write(0);
            BREADY_OUT.write(0);
        }

        // --- READ CHANNEL ROUTING ---
        int target_read = -1;
        if (active_read_master.read() != -1) {
            target_read = active_read_master.read();
        } else {
            // Priority selection if read arbiter is idle
            if (ARLOCK_M0.read() == 1 && ARVALID_M0.read() == 1) target_read = 0;
            else if (ARLOCK_M1.read() == 1 && ARVALID_M1.read() == 1) target_read = 1;
            else if (ARLOCK_M2.read() == 1 && ARVALID_M2.read() == 1) target_read = 2;
            else if (ARLOCK_M3.read() == 1 && ARVALID_M3.read() == 1) target_read = 3;
            else if (ARVALID_M0.read() == 1) target_read = 0;
            else if (ARVALID_M1.read() == 1) target_read = 1;
            else if (ARVALID_M2.read() == 1) target_read = 2;
            else if (ARVALID_M3.read() == 1) target_read = 3;
        }

        if (target_read == 0) {
            ARADDR_OUT.write(ARADDR_M0.read());
            ARLEN_OUT.write(ARLEN_M0.read());
            ARVALID_OUT.write(ARVALID_M0.read());
            ARREADY_M0.write(ARREADY_IN.read());
            ARREADY_M1.write(0); ARREADY_M2.write(0); ARREADY_M3.write(0);

            RDATA_M0.write(RDATA_IN.read());
            RRESP_M0.write(RRESP_IN.read());
            RVALID_M0.write(RVALID_IN.read());
            RLAST_M0.write(RLAST_IN.read());
            RVALID_M1.write(0); RVALID_M2.write(0); RVALID_M3.write(0);
            RREADY_OUT.write(RREADY_M0.read());
        } else if (target_read == 1) {
            ARADDR_OUT.write(ARADDR_M1.read());
            ARLEN_OUT.write(ARLEN_M1.read());
            ARVALID_OUT.write(ARVALID_M1.read());
            ARREADY_M1.write(ARREADY_IN.read());
            ARREADY_M0.write(0); ARREADY_M2.write(0); ARREADY_M3.write(0);

            RDATA_M1.write(RDATA_IN.read());
            RRESP_M1.write(RRESP_IN.read());
            RVALID_M1.write(RVALID_IN.read());
            RLAST_M1.write(RLAST_IN.read());
            RVALID_M0.write(0); RVALID_M2.write(0); RVALID_M3.write(0);
            RREADY_OUT.write(RREADY_M1.read());
        } else if (target_read == 2) {
            ARADDR_OUT.write(ARADDR_M2.read());
            ARLEN_OUT.write(ARLEN_M2.read());
            ARVALID_OUT.write(ARVALID_M2.read());
            ARREADY_M2.write(ARREADY_IN.read());
            ARREADY_M0.write(0); ARREADY_M1.write(0); ARREADY_M3.write(0);

            RDATA_M2.write(RDATA_IN.read());
            RRESP_M2.write(RRESP_IN.read());
            RVALID_M2.write(RVALID_IN.read());
            RLAST_M2.write(RLAST_IN.read());
            RVALID_M0.write(0); RVALID_M1.write(0); RVALID_M3.write(0);
            RREADY_OUT.write(RREADY_M2.read());
        } else if (target_read == 3) {
            ARADDR_OUT.write(ARADDR_M3.read());
            ARLEN_OUT.write(ARLEN_M3.read());
            ARVALID_OUT.write(ARVALID_M3.read());
            ARREADY_M3.write(ARREADY_IN.read());
            ARREADY_M0.write(0); ARREADY_M1.write(0); ARREADY_M2.write(0);

            RDATA_M3.write(RDATA_IN.read());
            RRESP_M3.write(RRESP_IN.read());
            RVALID_M3.write(RVALID_IN.read());
            RLAST_M3.write(RLAST_IN.read());
            RVALID_M0.write(0); RVALID_M1.write(0); RVALID_M2.write(0);
            RREADY_OUT.write(RREADY_M3.read());
        } else {
            ARADDR_OUT.write(0);
            ARLEN_OUT.write(0);
            ARVALID_OUT.write(0);
            ARREADY_M0.write(0); ARREADY_M1.write(0); ARREADY_M2.write(0); ARREADY_M3.write(0);

            RVALID_M0.write(0); RVALID_M1.write(0); RVALID_M2.write(0); RVALID_M3.write(0);
            RREADY_OUT.write(0);
        }
    }

    // Sequential arbitration updates (at clock edge)
    void process_interconnect() {
        if (!ARESETN.read()) {
            active_master.write(-1);
            active_read_master.write(-1);
            return;
        }

        // 1. UPDATE ACTIVE WRITE MASTER
        if (active_master.read() == -1) {
            int selected = -1;
            if (AWLOCK_M0.read() == 1 && AWVALID_M0.read() == 1) selected = 0;
            else if (AWLOCK_M1.read() == 1 && AWVALID_M1.read() == 1) selected = 1;
            else if (AWLOCK_M2.read() == 1 && AWVALID_M2.read() == 1) selected = 2;
            else if (AWLOCK_M3.read() == 1 && AWVALID_M3.read() == 1) selected = 3;
            else if (AWVALID_M0.read() == 1) selected = 0;
            else if (AWVALID_M1.read() == 1) selected = 1;
            else if (AWVALID_M2.read() == 1) selected = 2;
            else if (AWVALID_M3.read() == 1) selected = 3;

            // Lock master when the address handshake occurs
            if (selected != -1 && AWREADY_IN.read() == 1) {
                active_master.write(selected);
            }
        } else {
            // Release master when write response handshake occurs
            bool completed = false;
            int curr = active_master.read();
            if (curr == 0 && BVALID_IN.read() == 1 && BREADY_M0.read() == 1) completed = true;
            else if (curr == 1 && BVALID_IN.read() == 1 && BREADY_M1.read() == 1) completed = true;
            else if (curr == 2 && BVALID_IN.read() == 1 && BREADY_M2.read() == 1) completed = true;
            else if (curr == 3 && BVALID_IN.read() == 1 && BREADY_M3.read() == 1) completed = true;

            if (completed) {
                active_master.write(-1);
            }
        }

        // 2. UPDATE ACTIVE READ MASTER
        if (active_read_master.read() == -1) {
            int selected_read = -1;
            if (ARLOCK_M0.read() == 1 && ARVALID_M0.read() == 1) selected_read = 0;
            else if (ARLOCK_M1.read() == 1 && ARVALID_M1.read() == 1) selected_read = 1;
            else if (ARLOCK_M2.read() == 1 && ARVALID_M2.read() == 1) selected_read = 2;
            else if (ARLOCK_M3.read() == 1 && ARVALID_M3.read() == 1) selected_read = 3;
            else if (ARVALID_M0.read() == 1) selected_read = 0;
            else if (ARVALID_M1.read() == 1) selected_read = 1;
            else if (ARVALID_M2.read() == 1) selected_read = 2;
            else if (ARVALID_M3.read() == 1) selected_read = 3;

            // Lock read master when address handshake occurs
            if (selected_read != -1 && ARREADY_IN.read() == 1) {
                active_read_master.write(selected_read);
            }
        } else {
            // Release read master when read burst completes
            bool completed_read = false;
            int curr_r = active_read_master.read();
            if (curr_r == 0 && RVALID_IN.read() == 1 && RREADY_M0.read() == 1 && RLAST_IN.read() == 1) completed_read = true;
            else if (curr_r == 1 && RVALID_IN.read() == 1 && RREADY_M1.read() == 1 && RLAST_IN.read() == 1) completed_read = true;
            else if (curr_r == 2 && RVALID_IN.read() == 1 && RREADY_M2.read() == 1 && RLAST_IN.read() == 1) completed_read = true;
            else if (curr_r == 3 && RVALID_IN.read() == 1 && RREADY_M3.read() == 1 && RLAST_IN.read() == 1) completed_read = true;

            if (completed_read) {
                active_read_master.write(-1);
            }
        }
    }

    SC_CTOR(axi_interconnect) { 
        SC_METHOD(route_signals);
        sensitive << active_master << active_read_master
                  << AWADDR_M0 << AWLEN_M0 << AWVALID_M0 << WDATA_M0 << WVALID_M0 << WLAST_M0 << BREADY_M0
                  << AWADDR_M1 << AWLEN_M1 << AWVALID_M1 << WDATA_M1 << WVALID_M1 << WLAST_M1 << BREADY_M1
                  << AWADDR_M2 << AWLEN_M2 << AWVALID_M2 << WDATA_M2 << WVALID_M2 << WLAST_M2 << BREADY_M2
                  << AWADDR_M3 << AWLEN_M3 << AWVALID_M3 << WDATA_M3 << WVALID_M3 << WLAST_M3 << BREADY_M3
                  << AWREADY_IN << WREADY_IN << BVALID_IN << BRESP_IN
                  << ARADDR_M0 << ARLEN_M0 << ARVALID_M0 << RREADY_M0
                  << ARADDR_M1 << ARLEN_M1 << ARVALID_M1 << RREADY_M1
                  << ARADDR_M2 << ARLEN_M2 << ARVALID_M2 << RREADY_M2
                  << ARADDR_M3 << ARLEN_M3 << ARVALID_M3 << RREADY_M3
                  << ARREADY_IN << RDATA_IN << RRESP_IN << RVALID_IN << RLAST_IN;

        SC_METHOD(process_interconnect); 
        sensitive << ACLK.pos() << ARESETN.neg(); 
    }
};
#endif