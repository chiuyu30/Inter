`define IMAGE "image2.dat"

module LCD_CTRL(clk,
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
    input clk;
    input reset;
    input [3:0] cmd;
    input cmd_valid;
    input [7:0] IROM_Q;
    output IROM_rd;
    output [5:0] IROM_A;
    output IRAM_valid;
    output [7:0] IRAM_D;
    output [5:0] IRAM_A;
    output busy;
    output done;
    /*****************************************************************
                              declaration    
    *****************************************************************/
    //parameter
    parameter READ_IMG = 0;
    parameter READ_CMD = 1;
    parameter OP = 2;
    parameter WRITE = 3;
    
    //reg and wire
    reg [1:0] state_cr;
    reg [1:0] state_nt;
    reg [7:0] img_buffer [0:63];
    reg [5:0] RAM_counter;
    reg [5:0] op_point1;
    reg [7:0] max_point;
    reg [7:0] min_point;
    reg [9:0] avr_buf;
    wire [5:0] op_point2;
    wire [5:0] op_point3;
    wire [5:0] op_point4;

    reg IROM_rd;
    reg [5:0] IROM_A;
    reg IRAM_valid;
    reg [7:0] IRAM_D;
    reg [5:0] IRAM_A;
    reg busy;
    reg done;

    /*****************************************************************
                                 state machine    
    *****************************************************************/
    always@(posedge clk or posedge reset)begin
        if(reset)
            state_cr <= READ_IMG;
        else 
            state_cr <= state_nt;
    end

    always@(*)begin
        if(reset)
            state_nt = READ_IMG;
        else begin
            case(state_cr)
                READ_IMG:begin
                    if(IROM_A == 6'd63) state_nt = READ_CMD;
                    else state_nt = READ_IMG;
                end
                READ_CMD:begin
                    if(cmd_valid && cmd != 4'd0) state_nt = OP;
                    else if (cmd_valid && cmd == 4'd0) state_nt = WRITE;
                    else state_nt = READ_CMD;
                end
                OP:begin
                    state_nt = READ_CMD;
                end
                WRITE:begin
                    state_nt = WRITE;
                end
                default:
                    state_cr = READ_IMG;
            endcase
        end
    end
    /*****************************************************************
                                    output    
    *****************************************************************/
    //state output
    always@(*)begin
        case(state_cr)
            READ_IMG:begin
                IROM_rd =   1'b1;
                IRAM_valid =1'b0;
                busy =      1'b1;
            end
            READ_CMD:begin
                IROM_rd =   1'b0;
                IRAM_valid =1'b0;
                busy =      1'b0;
            end
            OP:begin
                IROM_rd =   1'b0;
                IRAM_valid =1'b0;
                busy =      1'b1;
            end
            WRITE:begin
                IROM_rd =   1'b0;
                IRAM_valid =1'b1;
                busy =      1'b1;
            end
            default:begin
                IROM_rd =   1'b0;
                IRAM_valid =1'b0;
                busy =      1'b0;
            end
        endcase
    end

    //IROM_A
    always@(posedge clk or posedge reset)begin
        if(reset)begin
            IROM_A <= 6'd0;
        end
        else if(IROM_rd) begin
            if(IROM_A == 6'd63) IROM_A <= 6'd0;
            else IROM_A <= IROM_A + 6'd1;
        end
        else begin
            IROM_A <= IROM_A;
        end 
    end

    //IRAM_A
    always@(posedge clk or posedge reset)begin
        if(reset)begin
            RAM_counter <= 6'd0;
        end
        else if(state_cr == WRITE) begin
            if(RAM_counter == 6'd63) RAM_counter <= RAM_counter;
            else RAM_counter <= RAM_counter + 1;
        end
        else begin
            RAM_counter <= RAM_counter;
        end 
    end
    
    always@(posedge clk or posedge reset)begin
        if(reset)begin
            IRAM_A <= 6'd0;
        end
        else begin
            IRAM_A <= RAM_counter;
        end 
    end

    //IRAM_D
    always@(posedge clk)begin
        if(state_cr == WRITE) begin
            IRAM_D <= img_buffer[RAM_counter];
        end
        else begin
            IRAM_D <= IRAM_D;
        end
    end
    //done
    always @(posedge clk)
    begin
        if (IRAM_A == 6'd63) done <= 1'd1;
        else done <= 1'd0;
    end 
    /*****************************************************************
                                    option    
    *****************************************************************/
    assign op_point2 = op_point1 + 6'd1;
    assign op_point3 = op_point1 + 6'd8;
    assign op_point4 = op_point1 + 6'd9;

    //op_point
    always@(posedge clk)begin
        if(reset)begin
            op_point1 <= 6'h1b;
        end
        else if(state_cr == READ_CMD)begin
            case(cmd)
                //shift up
                4'b0001:begin
                    if(op_point1 == 6'h0 || op_point1 == 6'h1 || op_point1 == 6'h2 || op_point1 == 6'h3 || op_point1 == 6'h4 || op_point1 == 6'h5 || op_point1 == 6'h6)begin
                        op_point1 <= op_point1;
                    end
                    else begin
                        op_point1 <= op_point1 - 6'd8;
                    end
                end
                //shift down
                4'b0010:begin
                    if(op_point1 == 6'h30 || op_point1 == 6'h31 || op_point1 == 6'h32 || op_point1 == 6'h33 || op_point1 == 6'h34 || op_point1 == 6'h35 || op_point1 == 6'h36)begin
                        op_point1 <= op_point1;
                    end
                    else begin
                        op_point1 <= op_point1 + 6'd8;
                    end
                end
                //shift left
                4'b0011:begin
                    if(op_point1 == 6'h0 || op_point1 == 6'h8 || op_point1 == 6'h10 || op_point1 == 6'h18 || op_point1 == 6'h20 || op_point1 == 6'h28 || op_point1 == 6'h30)begin
                        op_point1 <= op_point1;
                    end
                    else begin
                        op_point1 <= op_point1 - 6'd1;
                    end
                end
                //shift right
                4'b0100:begin
                    if(op_point1 == 6'h6 || op_point1 == 6'he || op_point1 == 6'h16 || op_point1 == 6'h1e || op_point1 == 6'h26 || op_point1 == 6'h2e || op_point1 == 6'h36)begin
                        op_point1 <= op_point1;
                    end
                    else begin
                        op_point1 <= op_point1 + 6'd1;
                    end
                end
                //default
                default:begin
                    op_point1 <= op_point1;
                end
            endcase
        end
    end

    //max
    always@(*)begin
        if(img_buffer[op_point1] >= img_buffer[op_point2] && img_buffer[op_point1] >= img_buffer[op_point3] && img_buffer[op_point1] >= img_buffer[op_point4])begin
            max_point <= op_point1;
        end
        else if(img_buffer[op_point2] >= img_buffer[op_point1] && img_buffer[op_point2] >= img_buffer[op_point3] && img_buffer[op_point2] >= img_buffer[op_point4])begin
            max_point <= op_point2;
        end
        else if(img_buffer[op_point3] >= img_buffer[op_point1] && img_buffer[op_point3] >= img_buffer[op_point2] && img_buffer[op_point3] >= img_buffer[op_point4])begin
            max_point <= op_point3;
        end
        else begin
            max_point <= op_point4;
        end
    end

    //min
    always@(*)begin
        if(img_buffer[op_point1] <= img_buffer[op_point2] && img_buffer[op_point1] <= img_buffer[op_point3] && img_buffer[op_point1] <= img_buffer[op_point4])begin
            min_point <= op_point1;
        end
        else if(img_buffer[op_point2] <= img_buffer[op_point1] && img_buffer[op_point2] <= img_buffer[op_point3] && img_buffer[op_point2] <= img_buffer[op_point4])begin
            min_point <= op_point2;
        end
        else if(img_buffer[op_point3] <= img_buffer[op_point1] && img_buffer[op_point3] <= img_buffer[op_point2] && img_buffer[op_point3] <= img_buffer[op_point4])begin
            min_point <= op_point3;
        end
        else begin
            min_point <= op_point4;
        end
    end

    //average
    always@(*)begin
        avr_buf <= img_buffer[op_point1] + img_buffer[op_point2] + img_buffer[op_point3] + img_buffer[op_point4];
    end

    /*****************************************************************
                                    img buffer    
    *****************************************************************/
    //img_buffer
    always@(posedge clk)begin
        case(state_cr)
            READ_IMG:begin
                if(IROM_rd == 1'b1) img_buffer[IROM_A] <= IROM_Q;
            end
            READ_CMD:begin
                case(cmd)
                    //max
                    4'b0101:begin
                        img_buffer[op_point1] <= img_buffer[max_point];
                        img_buffer[op_point2] <= img_buffer[max_point];
                        img_buffer[op_point3] <= img_buffer[max_point];
                        img_buffer[op_point4] <= img_buffer[max_point];
                    end
                    //min
                    4'b0110:begin
                        img_buffer[op_point1] <= img_buffer[min_point];
                        img_buffer[op_point2] <= img_buffer[min_point];
                        img_buffer[op_point3] <= img_buffer[min_point];
                        img_buffer[op_point4] <= img_buffer[min_point];
                    end
                    //average
                    4'b0111:begin
                        img_buffer[op_point1] <= avr_buf[9:2];
                        img_buffer[op_point2] <= avr_buf[9:2];
                        img_buffer[op_point3] <= avr_buf[9:2];
                        img_buffer[op_point4] <= avr_buf[9:2];
                    end
                    //counterclockwise rotation
                    4'b1000:begin
                        img_buffer[op_point1] <= img_buffer[op_point2];
                        img_buffer[op_point2] <= img_buffer[op_point4];
                        img_buffer[op_point3] <= img_buffer[op_point1];
                        img_buffer[op_point4] <= img_buffer[op_point3];
                    end
                    //clockwise rotation
                    4'b1001:begin
                        img_buffer[op_point1] <= img_buffer[op_point3];
                        img_buffer[op_point2] <= img_buffer[op_point1];
                        img_buffer[op_point3] <= img_buffer[op_point4];
                        img_buffer[op_point4] <= img_buffer[op_point2];
                    end
                    //mirror x
                    4'b1010:begin                        
                        img_buffer[op_point1] <= img_buffer[op_point3];
                        img_buffer[op_point2] <= img_buffer[op_point4];
                        img_buffer[op_point3] <= img_buffer[op_point1];
                        img_buffer[op_point4] <= img_buffer[op_point2];
                    end
                    //mirror y
                    4'b1011:begin
                        img_buffer[op_point1] <= img_buffer[op_point2];
                        img_buffer[op_point2] <= img_buffer[op_point1];
                        img_buffer[op_point3] <= img_buffer[op_point4];
                        img_buffer[op_point4] <= img_buffer[op_point3];
                    end
                    default:begin
                        img_buffer[op_point1] <= img_buffer[op_point1];
                        img_buffer[op_point2] <= img_buffer[op_point2];
                        img_buffer[op_point3] <= img_buffer[op_point3];
                        img_buffer[op_point4] <= img_buffer[op_point4];
                    end
                endcase    
            end
        endcase
    end

endmodule
    
    
    
