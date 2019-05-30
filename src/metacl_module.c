//Force MetaMorph to include the OpenCL code
#ifndef WITH_OPENCL
#define WITH_OPENCL
#endif
#include "metamorph.h"
#include "metacl_module.h"
extern cl_context meta_context;
extern cl_command_queue meta_queue;
extern cl_device_id meta_device;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_ocl_kernels_custom_args = NULL;
struct __meta_gen_opencl_metacl_module_frame * __meta_gen_opencl_metacl_module_current_frame = NULL;

struct __meta_gen_opencl_metacl_module_frame * __meta_gen_opencl_metacl_module_lookup_frame(cl_command_queue queue) {
  struct __meta_gen_opencl_metacl_module_frame * current = __meta_gen_opencl_metacl_module_current_frame;
  while (current != NULL) {
    if (current->queue == queue) break;
    current = current->next_frame;
  }
  return current;
}
a_module_record * meta_gen_opencl_metacl_module_registration = NULL;
a_module_record * meta_gen_opencl_metacl_module_registry(a_module_record * record) {
  if (record == NULL) return meta_gen_opencl_metacl_module_registration;
  a_module_record * old_registration = meta_gen_opencl_metacl_module_registration;
  if (old_registration == NULL) {
    record->implements = module_implements_opencl;
    record->module_init = &meta_gen_opencl_metacl_module_init;
    record->module_deinit = &meta_gen_opencl_metacl_module_deinit;
    record->module_registry_func = &meta_gen_opencl_metacl_module_registry;
    meta_gen_opencl_metacl_module_registration = record;
  }
  if (old_registration != NULL && old_registration != record) return record;
  if (old_registration == record) meta_gen_opencl_metacl_module_registration = NULL;
  return old_registration;
}
void meta_gen_opencl_metacl_module_init() {
  cl_int buildError, createError;
  if (meta_gen_opencl_metacl_module_registration == NULL) {
    meta_register_module(&meta_gen_opencl_metacl_module_registry);
    return;
  }
  if (meta_context == NULL) metaOpenCLFallBack();
  struct __meta_gen_opencl_metacl_module_frame * new_frame = (struct __meta_gen_opencl_metacl_module_frame *) calloc(1, sizeof(struct __meta_gen_opencl_metacl_module_frame));
  new_frame->next_frame = __meta_gen_opencl_metacl_module_current_frame;
  new_frame->device = meta_device;
  new_frame->queue = meta_queue;
  new_frame->context = meta_context;
  __meta_gen_opencl_metacl_module_current_frame = new_frame;
#ifdef WITH_INTELFPGA
  __meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen = metaOpenCLLoadProgramSource("ocl_kernels.aocx", &__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progSrc);
  if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) {
     __meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progSrc, NULL, &buildError);
#else
  __meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen = metaOpenCLLoadProgramSource("ocl_kernels.cl", &__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progSrc);
  if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) {
    __meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progSrc, &__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen, &buildError);
#endif
  if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, 1, &meta_device, __meta_gen_opencl_ocl_kernels_custom_args ? __meta_gen_opencl_ocl_kernels_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
  if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->ocl_kernels_init = 1;
    }
  }
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->sweep_cell_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "sweep_cell", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->reduce_angular_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "reduce_angular", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->reduce_angular_cell_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "reduce_angular_cell", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->reduce_moments_cell_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "reduce_moments_cell", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->calc_denominator_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "calc_denominator", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->calc_time_delta_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "calc_time_delta", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->calc_dd_coefficients_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "calc_dd_coefficients", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->calc_total_cross_section_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "calc_total_cross_section", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->calc_scattering_cross_section_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "calc_scattering_cross_section", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->calc_outer_source_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "calc_outer_source", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->calc_inner_source_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "calc_inner_source", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) __meta_gen_opencl_metacl_module_current_frame->zero_edge_array_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog, "zero_edge_array", &createError);
  if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  meta_gen_opencl_metacl_module_registration->initialized = 1;
}

void meta_gen_opencl_metacl_module_deinit() {
  cl_int releaseError;
  if (__meta_gen_opencl_metacl_module_current_frame != NULL) {
    if (__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen != -1) {
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->sweep_cell_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->reduce_angular_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->reduce_angular_cell_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->reduce_moments_cell_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_denominator_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_time_delta_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_dd_coefficients_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_total_cross_section_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_scattering_cross_section_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_outer_source_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_inner_source_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
    releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->zero_edge_array_kernel);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_prog);
  if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->ocl_kernels_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->ocl_kernels_init = 0;
    }
    struct __meta_gen_opencl_metacl_module_frame * next_frame = __meta_gen_opencl_metacl_module_current_frame->next_frame;
    free(__meta_gen_opencl_metacl_module_current_frame);
    __meta_gen_opencl_metacl_module_current_frame = next_frame;
    if (__meta_gen_opencl_metacl_module_current_frame == NULL && meta_gen_opencl_metacl_module_registration != NULL) {
      meta_gen_opencl_metacl_module_registration->initialized = 0;
    }
  }
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param istep scalar parameter of type "cl_int"
\param jstep scalar parameter of type "cl_int"
\param kstep scalar parameter of type "cl_int"
\param oct scalar parameter of type "cl_uint"
\param ichunk scalar parameter of type "cl_uint"
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param noct scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param dd_i scalar parameter of type "cl_double"
\param dd_j a cl_mem buffer, must internally store cl_double types
\param dd_k a cl_mem buffer, must internally store cl_double types
\param mu a cl_mem buffer, must internally store cl_double types
\param scat_coef a cl_mem buffer, must internally store cl_double types
\param time_delta a cl_mem buffer, must internally store cl_double types
\param total_cross_section a cl_mem buffer, must internally store cl_double types
\param flux_in a cl_mem buffer, must internally store cl_double types
\param flux_out a cl_mem buffer, must internally store cl_double types
\param flux_i a cl_mem buffer, must internally store cl_double types
\param flux_j a cl_mem buffer, must internally store cl_double types
\param flux_k a cl_mem buffer, must internally store cl_double types
\param source a cl_mem buffer, must internally store cl_double types
\param denom a cl_mem buffer, must internally store cl_double types
\param cell_index a cl_mem buffer, must internally store struct cell types
\param groups_todo a cl_mem buffer, must internally store cl_uint types
\param num_groups_todo scalar parameter of type "cl_uint"
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_sweep_cell(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_int istep, cl_int jstep, cl_int kstep, cl_uint oct, cl_uint ichunk, cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nang, cl_uint noct, cl_uint cmom, cl_double dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * mu, cl_mem * scat_coef, cl_mem * time_delta, cl_mem * total_cross_section, cl_mem * flux_in, cl_mem * flux_out, cl_mem * flux_i, cl_mem * flux_j, cl_mem * flux_k, cl_mem * source, cl_mem * denom, cl_mem * cell_index, cl_mem * groups_todo, cl_uint num_groups_todo, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 0, sizeof(cl_int), &istep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"istep\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 1, sizeof(cl_int), &jstep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"jstep\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 2, sizeof(cl_int), &kstep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"kstep\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 3, sizeof(cl_uint), &oct);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"oct\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 4, sizeof(cl_uint), &ichunk);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ichunk\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 5, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 6, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 7, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 8, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 9, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 10, sizeof(cl_uint), &noct);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"noct\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 11, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 12, sizeof(cl_double), &dd_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_i\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 13, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 14, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 15, sizeof(cl_mem), mu);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mu\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 16, sizeof(cl_mem), scat_coef);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_coef\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 17, sizeof(cl_mem), time_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"time_delta\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 18, sizeof(cl_mem), total_cross_section);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"total_cross_section\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 19, sizeof(cl_mem), flux_in);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_in\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 20, sizeof(cl_mem), flux_out);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_out\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 21, sizeof(cl_mem), flux_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_i\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 22, sizeof(cl_mem), flux_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_j\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 23, sizeof(cl_mem), flux_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_k\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 24, sizeof(cl_mem), source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"source\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 25, sizeof(cl_mem), denom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"denom\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 26, sizeof(cl_mem), cell_index);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cell_index\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 27, sizeof(cl_mem), groups_todo);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"groups_todo\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_cell_kernel, 28, sizeof(cl_uint), &num_groups_todo);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"num_groups_todo\", host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->sweep_cell_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_sweep_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param noct scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param weights a cl_mem buffer, must internally store cl_double types
\param scat_coef a cl_mem buffer, must internally store cl_double types
\param angular0 a cl_mem buffer, must internally store cl_double types
\param angular1 a cl_mem buffer, must internally store cl_double types
\param angular2 a cl_mem buffer, must internally store cl_double types
\param angular3 a cl_mem buffer, must internally store cl_double types
\param angular4 a cl_mem buffer, must internally store cl_double types
\param angular5 a cl_mem buffer, must internally store cl_double types
\param angular6 a cl_mem buffer, must internally store cl_double types
\param angular7 a cl_mem buffer, must internally store cl_double types
\param angular_prev0 a cl_mem buffer, must internally store cl_double types
\param angular_prev1 a cl_mem buffer, must internally store cl_double types
\param angular_prev2 a cl_mem buffer, must internally store cl_double types
\param angular_prev3 a cl_mem buffer, must internally store cl_double types
\param angular_prev4 a cl_mem buffer, must internally store cl_double types
\param angular_prev5 a cl_mem buffer, must internally store cl_double types
\param angular_prev6 a cl_mem buffer, must internally store cl_double types
\param angular_prev7 a cl_mem buffer, must internally store cl_double types
\param time_delta a cl_mem buffer, must internally store cl_double types
\param scalar a cl_mem buffer, must internally store cl_double types
\param scalar_mom a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_reduce_angular(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint noct, cl_uint cmom, cl_mem * weights, cl_mem * scat_coef, cl_mem * angular0, cl_mem * angular1, cl_mem * angular2, cl_mem * angular3, cl_mem * angular4, cl_mem * angular5, cl_mem * angular6, cl_mem * angular7, cl_mem * angular_prev0, cl_mem * angular_prev1, cl_mem * angular_prev2, cl_mem * angular_prev3, cl_mem * angular_prev4, cl_mem * angular_prev5, cl_mem * angular_prev6, cl_mem * angular_prev7, cl_mem * time_delta, cl_mem * scalar, cl_mem * scalar_mom, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 5, sizeof(cl_uint), &noct);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"noct\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 6, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 7, sizeof(cl_mem), weights);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"weights\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 8, sizeof(cl_mem), scat_coef);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_coef\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 9, sizeof(cl_mem), angular0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular0\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 10, sizeof(cl_mem), angular1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular1\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 11, sizeof(cl_mem), angular2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular2\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 12, sizeof(cl_mem), angular3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular3\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 13, sizeof(cl_mem), angular4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular4\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 14, sizeof(cl_mem), angular5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular5\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 15, sizeof(cl_mem), angular6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular6\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 16, sizeof(cl_mem), angular7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular7\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 17, sizeof(cl_mem), angular_prev0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev0\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 18, sizeof(cl_mem), angular_prev1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev1\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 19, sizeof(cl_mem), angular_prev2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev2\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 20, sizeof(cl_mem), angular_prev3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev3\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 21, sizeof(cl_mem), angular_prev4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev4\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 22, sizeof(cl_mem), angular_prev5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev5\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 23, sizeof(cl_mem), angular_prev6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev6\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 24, sizeof(cl_mem), angular_prev7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev7\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 25, sizeof(cl_mem), time_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"time_delta\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 26, sizeof(cl_mem), scalar);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_kernel, 27, sizeof(cl_mem), scalar_mom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_mom\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->reduce_angular_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param noct scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param scratch_num_local_elems allocate __local memory space for this many cl_double elements
\param weights a cl_mem buffer, must internally store cl_double types
\param scat_coef a cl_mem buffer, must internally store cl_double types
\param angular0 a cl_mem buffer, must internally store cl_double types
\param angular1 a cl_mem buffer, must internally store cl_double types
\param angular2 a cl_mem buffer, must internally store cl_double types
\param angular3 a cl_mem buffer, must internally store cl_double types
\param angular4 a cl_mem buffer, must internally store cl_double types
\param angular5 a cl_mem buffer, must internally store cl_double types
\param angular6 a cl_mem buffer, must internally store cl_double types
\param angular7 a cl_mem buffer, must internally store cl_double types
\param angular_prev0 a cl_mem buffer, must internally store cl_double types
\param angular_prev1 a cl_mem buffer, must internally store cl_double types
\param angular_prev2 a cl_mem buffer, must internally store cl_double types
\param angular_prev3 a cl_mem buffer, must internally store cl_double types
\param angular_prev4 a cl_mem buffer, must internally store cl_double types
\param angular_prev5 a cl_mem buffer, must internally store cl_double types
\param angular_prev6 a cl_mem buffer, must internally store cl_double types
\param angular_prev7 a cl_mem buffer, must internally store cl_double types
\param time_delta a cl_mem buffer, must internally store cl_double types
\param scalar a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_reduce_angular_cell(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint noct, cl_uint cmom, size_t scratch_num_local_elems, cl_mem * weights, cl_mem * scat_coef, cl_mem * angular0, cl_mem * angular1, cl_mem * angular2, cl_mem * angular3, cl_mem * angular4, cl_mem * angular5, cl_mem * angular6, cl_mem * angular7, cl_mem * angular_prev0, cl_mem * angular_prev1, cl_mem * angular_prev2, cl_mem * angular_prev3, cl_mem * angular_prev4, cl_mem * angular_prev5, cl_mem * angular_prev6, cl_mem * angular_prev7, cl_mem * time_delta, cl_mem * scalar, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 5, sizeof(cl_uint), &noct);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"noct\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 6, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 7, sizeof(cl_double) * scratch_num_local_elems, NULL);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scratch\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 8, sizeof(cl_mem), weights);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"weights\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 9, sizeof(cl_mem), scat_coef);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_coef\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 10, sizeof(cl_mem), angular0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular0\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 11, sizeof(cl_mem), angular1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular1\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 12, sizeof(cl_mem), angular2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular2\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 13, sizeof(cl_mem), angular3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular3\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 14, sizeof(cl_mem), angular4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular4\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 15, sizeof(cl_mem), angular5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular5\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 16, sizeof(cl_mem), angular6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular6\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 17, sizeof(cl_mem), angular7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular7\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 18, sizeof(cl_mem), angular_prev0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev0\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 19, sizeof(cl_mem), angular_prev1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev1\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 20, sizeof(cl_mem), angular_prev2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev2\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 21, sizeof(cl_mem), angular_prev3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev3\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 22, sizeof(cl_mem), angular_prev4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev4\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 23, sizeof(cl_mem), angular_prev5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev5\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 24, sizeof(cl_mem), angular_prev6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev6\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 25, sizeof(cl_mem), angular_prev7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev7\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 26, sizeof(cl_mem), time_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"time_delta\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_angular_cell_kernel, 27, sizeof(cl_mem), scalar);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->reduce_angular_cell_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_angular_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param noct scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param scratch_num_local_elems allocate __local memory space for this many cl_double elements
\param weights a cl_mem buffer, must internally store cl_double types
\param scat_coef a cl_mem buffer, must internally store cl_double types
\param angular0 a cl_mem buffer, must internally store cl_double types
\param angular1 a cl_mem buffer, must internally store cl_double types
\param angular2 a cl_mem buffer, must internally store cl_double types
\param angular3 a cl_mem buffer, must internally store cl_double types
\param angular4 a cl_mem buffer, must internally store cl_double types
\param angular5 a cl_mem buffer, must internally store cl_double types
\param angular6 a cl_mem buffer, must internally store cl_double types
\param angular7 a cl_mem buffer, must internally store cl_double types
\param angular_prev0 a cl_mem buffer, must internally store cl_double types
\param angular_prev1 a cl_mem buffer, must internally store cl_double types
\param angular_prev2 a cl_mem buffer, must internally store cl_double types
\param angular_prev3 a cl_mem buffer, must internally store cl_double types
\param angular_prev4 a cl_mem buffer, must internally store cl_double types
\param angular_prev5 a cl_mem buffer, must internally store cl_double types
\param angular_prev6 a cl_mem buffer, must internally store cl_double types
\param angular_prev7 a cl_mem buffer, must internally store cl_double types
\param time_delta a cl_mem buffer, must internally store cl_double types
\param scalar_mom a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_reduce_moments_cell(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint noct, cl_uint cmom, size_t scratch_num_local_elems, cl_mem * weights, cl_mem * scat_coef, cl_mem * angular0, cl_mem * angular1, cl_mem * angular2, cl_mem * angular3, cl_mem * angular4, cl_mem * angular5, cl_mem * angular6, cl_mem * angular7, cl_mem * angular_prev0, cl_mem * angular_prev1, cl_mem * angular_prev2, cl_mem * angular_prev3, cl_mem * angular_prev4, cl_mem * angular_prev5, cl_mem * angular_prev6, cl_mem * angular_prev7, cl_mem * time_delta, cl_mem * scalar_mom, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 5, sizeof(cl_uint), &noct);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"noct\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 6, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 7, sizeof(cl_double) * scratch_num_local_elems, NULL);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scratch\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 8, sizeof(cl_mem), weights);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"weights\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 9, sizeof(cl_mem), scat_coef);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_coef\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 10, sizeof(cl_mem), angular0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular0\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 11, sizeof(cl_mem), angular1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular1\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 12, sizeof(cl_mem), angular2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular2\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 13, sizeof(cl_mem), angular3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular3\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 14, sizeof(cl_mem), angular4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular4\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 15, sizeof(cl_mem), angular5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular5\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 16, sizeof(cl_mem), angular6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular6\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 17, sizeof(cl_mem), angular7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular7\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 18, sizeof(cl_mem), angular_prev0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev0\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 19, sizeof(cl_mem), angular_prev1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev1\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 20, sizeof(cl_mem), angular_prev2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev2\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 21, sizeof(cl_mem), angular_prev3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev3\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 22, sizeof(cl_mem), angular_prev4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev4\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 23, sizeof(cl_mem), angular_prev5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev5\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 24, sizeof(cl_mem), angular_prev6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev6\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 25, sizeof(cl_mem), angular_prev7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_prev7\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 26, sizeof(cl_mem), time_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"time_delta\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_moments_cell_kernel, 27, sizeof(cl_mem), scalar_mom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_mom\", host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->reduce_moments_cell_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_reduce_moments_cell\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param total_cross_section a cl_mem buffer, must internally store cl_double types
\param time_delta a cl_mem buffer, must internally store cl_double types
\param mu a cl_mem buffer, must internally store cl_double types
\param dd_i scalar parameter of type "cl_double"
\param dd_j a cl_mem buffer, must internally store cl_double types
\param dd_k a cl_mem buffer, must internally store cl_double types
\param denom a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_calc_denominator(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_mem * total_cross_section, cl_mem * time_delta, cl_mem * mu, cl_double dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * denom, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 5, sizeof(cl_mem), total_cross_section);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"total_cross_section\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 6, sizeof(cl_mem), time_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"time_delta\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 7, sizeof(cl_mem), mu);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mu\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 8, sizeof(cl_double), &dd_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_i\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 9, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 10, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_kernel, 11, sizeof(cl_mem), denom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"denom\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_denominator_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param dt scalar parameter of type "cl_double"
\param velocity a cl_mem buffer, must internally store cl_double types
\param time_delta a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_calc_time_delta(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_double dt, cl_mem * velocity, cl_mem * time_delta, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_time_delta_kernel, 0, sizeof(cl_double), &dt);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dt\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_time_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_time_delta_kernel, 1, sizeof(cl_mem), velocity);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_time_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_time_delta_kernel, 2, sizeof(cl_mem), time_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"time_delta\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_time_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_time_delta_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_time_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_time_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param dy scalar parameter of type "cl_double"
\param dz scalar parameter of type "cl_double"
\param eta a cl_mem buffer, must internally store cl_double types
\param xi a cl_mem buffer, must internally store cl_double types
\param dd_j a cl_mem buffer, must internally store cl_double types
\param dd_k a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_calc_dd_coefficients(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_double dy, cl_double dz, cl_mem * eta, cl_mem * xi, cl_mem * dd_j, cl_mem * dd_k, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_dd_coefficients_kernel, 0, sizeof(cl_double), &dy);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dy\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_dd_coefficients\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coefficients_kernel, 1, sizeof(cl_double), &dz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dz\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_dd_coefficients\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coefficients_kernel, 2, sizeof(cl_mem), eta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"eta\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_dd_coefficients\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coefficients_kernel, 3, sizeof(cl_mem), xi);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"xi\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_dd_coefficients\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coefficients_kernel, 4, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_dd_coefficients\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coefficients_kernel, 5, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_dd_coefficients\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_dd_coefficients_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_dd_coefficients\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_dd_coefficients\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param nmat scalar parameter of type "cl_uint"
\param xs a cl_mem buffer, must internally store cl_double types
\param map a cl_mem buffer, must internally store cl_uint types
\param total_cross_section a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_calc_total_cross_section(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nmat, cl_mem * xs, cl_mem * map, cl_mem * total_cross_section, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_total_cross_section_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_total_cross_section_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_total_cross_section_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_total_cross_section_kernel, 3, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_total_cross_section_kernel, 4, sizeof(cl_uint), &nmat);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmat\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_total_cross_section_kernel, 5, sizeof(cl_mem), xs);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"xs\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_total_cross_section_kernel, 6, sizeof(cl_mem), map);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"map\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_total_cross_section_kernel, 7, sizeof(cl_mem), total_cross_section);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"total_cross_section\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_total_cross_section_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_total_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param nmom scalar parameter of type "cl_uint"
\param nmat scalar parameter of type "cl_uint"
\param gg_cs a cl_mem buffer, must internally store cl_double types
\param map a cl_mem buffer, must internally store cl_uint types
\param scat_cs a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_calc_scattering_cross_section(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nmom, cl_uint nmat, cl_mem * gg_cs, cl_mem * map, cl_mem * scat_cs, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 3, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 4, sizeof(cl_uint), &nmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 5, sizeof(cl_uint), &nmat);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmat\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 6, sizeof(cl_mem), gg_cs);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"gg_cs\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 7, sizeof(cl_mem), map);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"map\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_scattering_cross_section_kernel, 8, sizeof(cl_mem), scat_cs);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_cs\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_scattering_cross_section_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_scattering_cross_section\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param nmom scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param nmat scalar parameter of type "cl_uint"
\param map a cl_mem buffer, must internally store cl_int types
\param gg_cs a cl_mem buffer, must internally store cl_double types
\param fixed_source a cl_mem buffer, must internally store cl_double types
\param lma a cl_mem buffer, must internally store cl_int types
\param scalar a cl_mem buffer, must internally store cl_double types
\param scalar_mom a cl_mem buffer, must internally store cl_double types
\param g2g_source a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_calc_outer_source(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nmom, cl_uint cmom, cl_uint nmat, cl_mem * map, cl_mem * gg_cs, cl_mem * fixed_source, cl_mem * lma, cl_mem * scalar, cl_mem * scalar_mom, cl_mem * g2g_source, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 3, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 4, sizeof(cl_uint), &nmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 5, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 6, sizeof(cl_uint), &nmat);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmat\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 7, sizeof(cl_mem), map);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"map\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 8, sizeof(cl_mem), gg_cs);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"gg_cs\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 9, sizeof(cl_mem), fixed_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"fixed_source\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 10, sizeof(cl_mem), lma);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"lma\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 11, sizeof(cl_mem), scalar);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 12, sizeof(cl_mem), scalar_mom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_mom\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_outer_source_kernel, 13, sizeof(cl_mem), g2g_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"g2g_source\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_outer_source_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param nmom scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param g2g_source a cl_mem buffer, must internally store cl_double types
\param scat_cs a cl_mem buffer, must internally store cl_double types
\param scalar a cl_mem buffer, must internally store cl_double types
\param scalar_mom a cl_mem buffer, must internally store cl_double types
\param lma a cl_mem buffer, must internally store cl_int types
\param source a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_calc_inner_source(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint nmom, cl_uint cmom, cl_mem * g2g_source, cl_mem * scat_cs, cl_mem * scalar, cl_mem * scalar_mom, cl_mem * lma, cl_mem * source, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 3, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 4, sizeof(cl_uint), &nmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 5, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 6, sizeof(cl_mem), g2g_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"g2g_source\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 7, sizeof(cl_mem), scat_cs);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_cs\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 8, sizeof(cl_mem), scalar);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 9, sizeof(cl_mem), scalar_mom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_mom\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 10, sizeof(cl_mem), lma);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"lma\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_inner_source_kernel, 11, sizeof(cl_mem), source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"source\", host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_inner_source_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param array a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_ocl_kernels_zero_edge_array(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_mem * array, int async, cl_event * event , int workdim) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->ocl_kernels_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (*block_size)[0];
    grid[1] = (*grid_size)[1] * (*block_size)[1];
    grid[2] = (*block_size)[2];
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->zero_edge_array_kernel, 0, sizeof(cl_mem), array);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"array\", host wrapper: \"meta_gen_opencl_ocl_kernels_zero_edge_array\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->zero_edge_array_kernel, workdim , NULL, grid, block, 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_ocl_kernels_zero_edge_array\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_ocl_kernels_zero_edge_array\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

