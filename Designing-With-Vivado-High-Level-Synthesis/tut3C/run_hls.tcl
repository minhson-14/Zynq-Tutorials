# ============================================================
# run_hls.tcl — Tcl Script cho Bài tập 3C Interface Synthesis
# ============================================================
open_project -reset matrix_mult_prj
set_top matrix_mult
add_files matrix_mult.cpp
add_files matrix_mult.h
add_files -tb matrix_mult_test.cpp
open_solution solution1 -flow_target vivado
set_part {xc7z020clg484-1}
create_clock -period 10 -name default
# Chỉ tạo project, không synthesis ngay
# (Synthesis sẽ được thực hiện trong GUI)
close_project