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
    reg [13:0]  gray_addr;//ok
    reg         gray_req;//ok
    reg [13:0]  lbp_addr;
    reg         lbp_valid;//ok
    reg [7:0]   lbp_data;
    reg         finish;

    reg [1:0]   state_cr;
    reg [1:0]   state_nt;
    parameter   READ = 0;
    parameter   CALC = 1;
    parameter   WRITE = 2;

    reg [7:0]   img_buf     [0:16383];
    reg [7:0]   thrshld     [0:16383];
    wire [7:0]  cent;
    reg [13:0]  cent_point;
    reg         g7; 
    reg         g6;
    reg         g5;
    reg         g4;
    reg         g3;
    reg         g2;
    reg         g1;
    reg         g0;
    integer i;
//==============================================================================================================
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
                    if(cent_point == 14'd16254)
                        state_nt = WRITE;
                    else
                        state_nt = CALC;
                end
                WRITE:begin
                    state_nt = WRITE;
                end
                default:begin
                    state_nt = READ;
                end
            endcase
        end
    end

    /**********************************************************************
                                    state output    
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
        else if(gray_ready)begin
            if(gray_addr == 14'd16383)
                gray_addr <= gray_addr;
            else
                gray_addr <= gray_addr + 14'd1;
        end
        else begin
            gray_addr <= gray_addr;
        end
    end
    /**********************************************************************
                                  img_buf    
    **********************************************************************/
    always@(posedge clk)begin
        case(state_cr)
            READ:begin
                img_buf[gray_addr] <= gray_data;
            end
            CALC:begin
                //img_buf <= img_buf;
            end
            WRITE:begin
                //img_buf <= img_buf;
            end    
            default:begin
                //img_buf <= img_buf;
            end 
        endcase

        /*
        if(gray_ready)
            img_buf[gray_addr] <= gray_data;
        else if (lbp_valid)
            img_buf <= thrshld;
        else
            img_buf <= img_buf;
        */
    end
    /**********************************************************************
                                  thrshld    
    **********************************************************************/
    
    always@(posedge reset or posedge clk)begin
        if(reset)begin
            for(i = 0; i < 16384; i = i + 1)
                thrshld[i] <= 8'd0;
        end  
        else begin  
            case(state_cr)
                READ:begin
                    //thrshld <= thrshld;
                end
                CALC:begin
                    thrshld[cent_point] <= cent;
                end
                WRITE:begin
                    //thrshld <= thrshld;
                end
            endcase
        end
    end

    //cent point
    always@(posedge clk)begin
        if(reset)
            cent_point <= 14'd128;
        else begin
            case(state_cr)
                READ:begin
                    cent_point <= cent_point;
                end
                CALC:begin
                    if(cent_point == 14'd16254)
                        cent_point <= cent_point;
                    else if(cent_point % 14'd128 == 14'd126)
                        cent_point <= cent_point + 14'd3;
                    else
                        cent_point <= cent_point + 14'd1;
                end
                WRITE:begin
                        cent_point <= cent_point;
                end
            endcase
        end
    end

    //side point calculate
    always@(*)begin
        if (cent_point >= 14'd129)  g0 = (img_buf[cent_point - 14'd129] >= img_buf[cent_point]) ? 1'b1 : 1'b0;
        else g0 = 1'b0;

        if (cent_point >= 14'd128)  g1 = (img_buf[cent_point - 14'd128] >= img_buf[cent_point]) ? 1'b1 : 1'b0;
        else g1 = 1'b0;

        if (cent_point >= 14'd127)  g2 = (img_buf[cent_point - 14'd127] >= img_buf[cent_point]) ? 1'b1 : 1'b0;
        else g2 = 1'b0;

        if (cent_point >= 14'd1)  g3 = (img_buf[cent_point - 14'd1] >= img_buf[cent_point]) ? 1'b1 : 1'b0;
        else g3 = 1'b0;

        if (cent_point <= 14'd16382)  g4 = (img_buf[cent_point + 14'd1] >= img_buf[cent_point]) ? 1'b1 : 1'b0;
        else g4 = 1'b0;
        
        if (cent_point <= 14'd16256)  g5 = (img_buf[cent_point + 14'd127] >= img_buf[cent_point]) ? 1'b1 : 1'b0;
        else g5 = 1'b0;

        if (cent_point <= 14'd16255)  g6 = (img_buf[cent_point + 14'd128] >= img_buf[cent_point]) ? 1'b1 : 1'b0;
        else g6 = 1'b0;

        if (cent_point <= 14'd16254)  g7 = (img_buf[cent_point + 14'd129] >= img_buf[cent_point]) ? 1'b1 : 1'b0;
        else g7 = 1'b0;
    end

    assign cent = {g7, g6, g5, g4, g3, g2, g1, g0};

    /**********************************************************************
                                  lbp_addr    
    **********************************************************************/
    always@(posedge reset or posedge clk)begin
        if(reset)
            lbp_addr <= 14'd128;
        else if(lbp_valid)begin
            if(lbp_addr == 14'd16254)
                lbp_addr <= lbp_addr;
            else
                lbp_addr <= lbp_addr + 14'd1;
        end
        else begin
            lbp_addr <= lbp_addr;
        end
    end
    /**********************************************************************
                                  lbp_data    
    **********************************************************************/
    always@(posedge clk)begin
        if(lbp_valid)
            lbp_data <= thrshld[lbp_addr];
        else
            lbp_data <= lbp_data;
    end
    /**********************************************************************
                                  finish    
    **********************************************************************/
    always@(posedge clk)begin
        if(lbp_addr == 14'd16254)
            finish <= 1'b1;
        else
            finish <= 1'b0;
    end

//====================================================================
endmodule
