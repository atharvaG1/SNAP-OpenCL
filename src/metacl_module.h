
extern const char * __meta_gen_opencl_ocl_kernels_custom_args;
struct __meta_gen_opencl_metacl_module_frame;
struct __meta_gen_opencl_metacl_module_frame {
  struct __meta_gen_opencl_metacl_module_frame * next_frame;
  cl_device_id device;
  cl_context context;
  cl_command_queue queue;
  const char * ocl_kernels_progSrc;
  size_t ocl_kernels_progLen;
  cl_program ocl_kernels_prog;
  cl_int ocl_kernels_init;
  cl_kernel sweep_cell_kernel;
  cl_kernel reduce_angular_kernel;
  cl_kernel reduce_angular_cell_kernel;
  cl_kernel reduce_moments_cell_kernel;
  cl_kernel calc_denominator_kernel;
  cl_kernel calc_time_delta_kernel;
  cl_kernel calc_dd_coefficients_kernel;
  cl_kernel calc_total_cross_section_kernel;
  cl_kernel calc_scattering_cross_section_kernel;
  cl_kernel calc_outer_source_kernel;
  cl_kernel calc_inner_source_kernel;
  cl_kernel zero_edge_array_kernel;
};
#ifdef __cplusplus
extern "C" {
#endif
a_module_record * meta_gen_opencl_metacl_module_registry(a_module_record * record);
struct __meta_gen_opencl_metacl_module_frame * __meta_gen_opencl_metacl_module_lookup_frame(cl_command_queue queue);
void meta_gen_opencl_metacl_module_init();
void meta_gen_opencl_metacl_module_deinit();
cl_int meta_gen_opencl_ocl_kernels_sweep_cell(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_int istep, cl_int jstep, cl_int kstep, cl_uint oct, cl_uint ichunk, cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nang, cl_uint noct, cl_uint cmom, cl_double dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * mu, cl_mem * scat_coef, cl_mem * time_delta, cl_mem * total_cross_section, cl_mem * flux_in, cl_mem * flux_out, cl_mem * flux_i, cl_mem * flux_j, cl_mem * flux_k, cl_mem * source, cl_mem * denom, cl_mem * cell_index, cl_mem * groups_todo, cl_uint num_groups_todo, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_reduce_angular(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint noct, cl_uint cmom, cl_mem * weights, cl_mem * scat_coef, cl_mem * angular0, cl_mem * angular1, cl_mem * angular2, cl_mem * angular3, cl_mem * angular4, cl_mem * angular5, cl_mem * angular6, cl_mem * angular7, cl_mem * angular_prev0, cl_mem * angular_prev1, cl_mem * angular_prev2, cl_mem * angular_prev3, cl_mem * angular_prev4, cl_mem * angular_prev5, cl_mem * angular_prev6, cl_mem * angular_prev7, cl_mem * time_delta, cl_mem * scalar, cl_mem * scalar_mom, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_reduce_angular_cell(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint noct, cl_uint cmom, size_t scratch_num_local_elems, cl_mem * weights, cl_mem * scat_coef, cl_mem * angular0, cl_mem * angular1, cl_mem * angular2, cl_mem * angular3, cl_mem * angular4, cl_mem * angular5, cl_mem * angular6, cl_mem * angular7, cl_mem * angular_prev0, cl_mem * angular_prev1, cl_mem * angular_prev2, cl_mem * angular_prev3, cl_mem * angular_prev4, cl_mem * angular_prev5, cl_mem * angular_prev6, cl_mem * angular_prev7, cl_mem * time_delta, cl_mem * scalar, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_reduce_moments_cell(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint noct, cl_uint cmom, size_t scratch_num_local_elems, cl_mem * weights, cl_mem * scat_coef, cl_mem * angular0, cl_mem * angular1, cl_mem * angular2, cl_mem * angular3, cl_mem * angular4, cl_mem * angular5, cl_mem * angular6, cl_mem * angular7, cl_mem * angular_prev0, cl_mem * angular_prev1, cl_mem * angular_prev2, cl_mem * angular_prev3, cl_mem * angular_prev4, cl_mem * angular_prev5, cl_mem * angular_prev6, cl_mem * angular_prev7, cl_mem * time_delta, cl_mem * scalar_mom, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_calc_denominator(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_mem * total_cross_section, cl_mem * time_delta, cl_mem * mu, cl_double dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * denom, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_calc_time_delta(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_double dt, cl_mem * velocity, cl_mem * time_delta, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_calc_dd_coefficients(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_double dy, cl_double dz, cl_mem * eta, cl_mem * xi, cl_mem * dd_j, cl_mem * dd_k, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_calc_total_cross_section(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nmat, cl_mem * xs, cl_mem * map, cl_mem * total_cross_section, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_calc_scattering_cross_section(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nmom, cl_uint nmat, cl_mem * gg_cs, cl_mem * map, cl_mem * scat_cs, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_calc_outer_source(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nmom, cl_uint cmom, cl_uint nmat, cl_mem * map, cl_mem * gg_cs, cl_mem * fixed_source, cl_mem * lma, cl_mem * scalar, cl_mem * scalar_mom, cl_mem * g2g_source, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_calc_inner_source(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nmom, cl_uint cmom, cl_mem * g2g_source, cl_mem * scat_cs, cl_mem * scalar, cl_mem * scalar_mom, cl_mem * lma, cl_mem * source, int async, cl_event * event , int workdim);
cl_int meta_gen_opencl_ocl_kernels_zero_edge_array(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_mem * array, int async, cl_event * event , int workdim);
#ifdef __cplusplus
}
#endif
