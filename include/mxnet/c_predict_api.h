/*!
 *  Copyright (c) 2015 by Contributors
 * \file c_predict_api.h
 * \brief C predict API of mxnet, contains a minimum API to run prediction.
 *  This file is self-contained, and do not dependent on any other files.
 */
#ifndef MXNET_C_PREDICT_API_H_
#define MXNET_C_PREDICT_API_H_

#ifdef __cplusplus
#define MXNET_EXTERN_C extern "C"
#else
#define MXNET_EXTERN_C
#endif

#ifdef _WIN32
#ifdef MXNET_EXPORTS
#define MXNET_DLL MXNET_EXTERN_C __declspec(dllexport)
#else
#define MXNET_DLL MXNET_EXTERN_C __declspec(dllimport)
#endif
#else
#define MXNET_DLL MXNET_EXTERN_C
#endif

/*! \brief manually define unsigned int */
typedef unsigned int mx_uint;
/*! \brief manually define float */
typedef float mx_float;
/*! \brief handle to Predictor */
typedef void *PredictorHandle;
/*! \brief handle to NDArray list */
typedef void *NDListHandle;

/*!
 * \brief Get the last error happeneed.
 * \return The last error happened at the predictor.
 */
MXNET_DLL const char* MXGetLastError();
/*!
 * \brief create a predictor
 * \param symbol_json_str The JSON string of the symbol.
 * \param param_bytes The in-memory raw bytes of parameter ndarray file.
 * \param param_size The size of parameter ndarray file.
 * \param dev_type The device type, 1: cpu, 2:gpu
 * \param dev_id The device id of the predictor.
 * \param num_input_nodes Number of input nodes to the net,
 *    For feedforward net, this is 1.
 * \param input_keys The name of input argument.
 *    For feedforward net, this is {"data"}
 * \param input_shape_indptr Index pointer of shapes of each input node.
 *    The length of this array = num_input_nodes + 1.
 *    For feedforward net that takes 4 dimensional input, this is {0, 4}.
 * \param input_shape_data A flatted data of shapes of each input node.
 *    For feedforward net that takes 4 dimensional input, this is the shape data.
 * \param out The created predictor handle.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXPredCreate(const char* symbol_json_str,
                           const char* param_bytes,
                           size_t param_size,
                           int dev_type, int dev_id,
                           mx_uint num_input_nodes,
                           const char** input_keys,
                           const mx_uint* input_shape_indptr,
                           const mx_uint* input_shape_data,
                           PredictorHandle* out);
/*!
 * \brief Get the shape of output node.
 *  The returned shape_data and shape_ndim is only valid before next call to MXPred function.
 * \param handle The handle of the predictor.
 * \param index The index of output node, set to 0 if there is only one output.
 * \param shape_data Used to hold pointer to the shape data
 * \param shape_ndim Used to hold shape dimension.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXPredGetOutputShape(PredictorHandle handle,
                                   mx_uint index,
                                   mx_uint** shape_data,
                                   mx_uint* shape_ndim);
/*!
 * \brief Set the input data of predictor.
 * \param handle The predictor handle.
 * \param key The name of input node to set.
 *     For feedforward net, this is "data".
 * \param data The pointer to the data to be set, with the shape specified in MXPredCreate.
 * \param size The size of data array, used for safety check.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXPredSetInput(PredictorHandle handle,
                             const char* key,
                             const mx_float* data,
                             mx_uint size);
/*!
 * \brief Run a forward pass to get the output.
 * \param handle The handle of the predictor.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXPredForward(PredictorHandle handle);
/*!
 * \brief Run a interactive forward pass to get the output.
 *  This is helpful for displaying progress of prediction which can be slow.
 *  User must call PartialForward from step=0, keep increasing it until step_left=0.
 * \code
 * int step_left = 1;
 * for (int step = 0; step_left != 0; ++step) {
 *    MXPredPartialForward(handle, step, &step_left);
 *    printf("Current progress [%d/%d]\n", step, step + step_left + 1);
 * }
 * \endcode
 * \param handle The handle of the predictor.
 * \param step The current step to run forward on.
 * \param step_left The number of steps left
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXPredPartialForward(PredictorHandle handle, int step, int* step_left);
/*!
 * \brief Get the output value of prediction.
 * \param handle The handle of the predictor.
 * \param index The index of output node, set to 0 if there is only one output.
 * \param data User allocated data to hold the output.
 * \param size The size of data array, used for safe checking.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXPredGetOutput(PredictorHandle handle,
                              mx_uint index,
                              mx_float* data,
                              mx_uint size);
/*!
 * \brief Free a predictor handle.
 * \param handle The handle of the predictor.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXPredFree(PredictorHandle handle);
/*!
 * \brief Create a NDArray List by loading from ndarray file.
 *     This can be used to load mean image file.
 * \param nd_file_bytes The byte contents of nd file to be loaded.
 * \param nd_file_size The size of the nd file to be loaded.
 * \param out The out put NDListHandle
 * \param out_length Length of the list.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXNDListCreate(const char* nd_file_bytes,
                             size_t nd_file_size,
                             NDListHandle *out,
                             mx_uint* out_length);
/*!
 * \brief Get an element from list
 * \param handle The handle to the NDArray
 * \param index The index in the list
 * \param out_key The output key of the item
 * \param out_data The data region of the item
 * \param out_shape The shape of the item.
 * \param out_ndim The number of dimension in the shape.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXNDListGet(NDListHandle handle,
                          mx_uint index,
                          const char** out_key,
                          const mx_float** out_data,
                          const mx_uint** out_shape,
                          mx_uint* out_ndim);
/*!
 * \brief Free a predictor handle.
 * \param handle The handle of the predictor.
 * \return 0 when success, -1 when failure.
 */
MXNET_DLL int MXNDListFree(NDListHandle handle);

#endif  // MXNET_C_PREDICT_API_H_
