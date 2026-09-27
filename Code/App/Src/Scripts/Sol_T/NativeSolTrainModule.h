#ifndef FRONTIERX_NATIVE_SOL_TRAIN_MODULE_H
#define FRONTIERX_NATIVE_SOL_TRAIN_MODULE_H

#if defined(_WIN32)
#error NativeSolTrainModule.h is only for the native script path
#endif

// Called after the production parser and heap have been initialized.
// Aborts when a required schema, heap, localization or parser contract fails.
extern "C" void FrontierXInitializeSolTrainScript(void);

#endif
