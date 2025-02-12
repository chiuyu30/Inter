
`timescale 1ns/10ps
module LBP ( clk, reset, gray_addr, gray_req, gray_ready, gray_data, lbp_addr, lbp_valid, lbp_data, finish);
    input   	    clk;
    input   	    reset;
    output  [13:0] 	gray_addr;
    output         	gray_req;
    input   	    gray_ready;
    input   [7:0] 	gray_data;
    output  [13:0] 	lbp_addr;
    output  	    lbp_valid;
    output  [7:0] 	lbp_data;
    output  	    finish;
    /**********************************************************************
                                declaration
    **********************************************************************/
    reg [13:0]  gray_addr;
    reg         gray_req;
    reg [13:0]  lbp_addr;
    reg         lbp_valid;
    reg [7:0]   lbp_data;
    reg         finish;

    reg [1:0]   state_cr;
    reg [1:0]   state_nt;
    parameter   READ = 0;
    parameter   CALC = 1;
    parameter   WRITE = 2;

    reg [7:0]   img_buf     [0:16383];
    reg [7:0]   thrshld     [0:16383];
    reg [7:0]   cent;
    reg [13:0]  cent_point;
    integer i;

     /*********************************************************************
                                 state machine    
    **********************************************************************/
    always@(posedge reset or posedge clk)begin
        if(reset)
            state_cr <= READ;
        else  
            state_cr <= state_nt;
    end
    
    always@(*)begin
        if(reset)begin
            state_nt = READ;
        end
        else begin
            case(state_cr)
                READ:begin
                    if(gray_addr == 14'd16383)
                        state_nt = CALC;
                    else
                        state_nt = READ;
                end
                CALC:begin
                    if(cent_pointt == 14'd16254)
                        state_nt = WRITE;
                    else
                        state_nt = CALC;
                end
                WRITE:begin
                    state_nt = WRITE;
                end
                default:begin
                    state_nt = IDLE;
                end
            endcase
        end
    end

    /**********************************************************************
                                    output    
    **********************************************************************/
    always@(*)begin
        case(state_cr)
            READ:begin
                gray_req = 1'b1;
                lbp_valid = 1'b0;
            end
            CALC:begin
                gray_req = 1'b0;
                lbp_valid = 1'b0;
            end
            WRITE:begin
                gray_req = 1'b0;
                lbp_valid = 1'b1;
            end
            default:begin
                gray_req = 1'b0;
                lbp_valid = 1'b0;
            end
        endcase
    end
    /**********************************************************************
                                  gray_addr    
    **********************************************************************/
    always@(posedge reset or negedge clk)begin
        if(reset)
            gray_addr <= 14'd0;
        else if(state_cr == READ)begin
            if(gray_ready == 1'b1)begin
                if(gray_addr == 14'd16383)
                    gray_addr <= gray_addr;
                else
                    gray_addr <= gray_addr + 14'd1;
            end
        end
        else begin
            gray_addr <= gray_addr;
        end
    end
    /**********************************************************************
                                  img_buf    
    **********************************************************************/
    always@(posedge clk)begin
        case(state_nt)
            READ:begin
                img_buf[gray_addr] <= gray_data;
            end
            CALC:begin
                if(cent_point == )
                img_buf <= img_buf;
            end
            WRITE:begin
                img_buf <= thrshld;
            end    
            default:begin
              img_buf <= img_buf;
            end 
        end
    end
    /**********************************************************************
                                  thrshld    
    **********************************************************************/
    always@(posedge clk)begin
        case(state_cr)
            READ:begin
                
            end
            CALC:begin
            end
            WRITE:begin
            end
        endcase
    end


//====================================================================
endmodule
