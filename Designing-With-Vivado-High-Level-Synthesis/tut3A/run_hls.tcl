# ============================================================
# run_hls.tcl — Tcl Script cho Vitis HLS 2022.2
# Tạo project nhân ma trận (matrix multiplication)
# ============================================================

# 1. Tạo project mới (reset nếu đã tồn tại)
open_project -reset matrix_mult_prj

# 2. Đặt hàm top-level
set_top matrix_mult

# 3. Thêm tệp nguồn
add_files matrix_mult.cpp
add_files matrix_mult.h

# 4. Thêm tệp testbench
add_files -tb matrix_mult_test.cpp

# 5. Tạo và cấu hình solution
#    -flow_target vivado: xuất IP cho Vivado (thay vì Vitis Kernel)
open_solution "solution1" -flow_target vivado

# 6. Chọn thiết bị FPGA (ZedBoard — Zynq-7020)
set_part {xc7z020clg484-1}

# 7. Cấu hình clock (10 ns = 100 MHz)
create_clock -period 10 -name default

# 8. Chạy C Simulation
csim_design

# 9. Đóng project
close_project
