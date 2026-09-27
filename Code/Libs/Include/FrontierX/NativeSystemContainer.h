#pragma once

#if !defined(_WIN32)
#include "SYSTEM.H"

// Retain the inner identity of a newly added native aggregate for targeted release.
GENRESULT AddNativeSystemComponent(ISystemContainer * container,
                                   const AGGDESC * descriptor,
                                   IDAComponent ** inner);
// Remove an AddComponent aggregate after its consumers stop.
GENRESULT RemoveNativeSystemComponent(ISystemContainer * container,
                                      IDAComponent * inner);
#endif
