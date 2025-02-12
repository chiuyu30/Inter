`timescale 1ns/10ps
`include "test.v"
module test_tb();   
    reg clk;
    reg reset;
    reg [3:0] cmd;
    reg cmd_valid;
    reg [7:0] IROM_Q;
    wire IROM_rd;
    wire [5:0] IROM_A;
    wire IRAM_valid;
    wire [7:0] IRAM_D;
    wire [5:0] IRAM_A;
    wire busy;
    wire done;

    test test(clk,
        reset,
        cmd,
        cmd_valid,
        IROM_Q,
        IROM_rd,
        IROM_A,
        IRAM_valid,
        IRAM_D,
        IRAM_A,
        busy,
        done);

    initial begin
        $dumpfile("wave.vcd");
        $dumpvars(0, test_tb);
    end

    initial @(posedge done) #100 $finish;

    initial begin
        clk         = 1'b0;
        reset       = 1'b0;
        cmd         = 4'd0;
        cmd_valid   = 1'b1;
        IROM_Q      = 8'd0;
    end

    initial begin
        @(negedge clk)  reset = 1'b1;
        #20        reset = 1'b0;                           
    end 
    
    always #5 clk = ~clk;

endmodule