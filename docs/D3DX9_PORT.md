# D3DX9 replacement inventory

Evidence: production-source inventory, source review and isolated GCC/Clang tests, 2026-09-16.
No native Windows rebuild, retail runtime comparison, shader execution or rendered frame was
performed.

## Current measured surface

After replacing the two mesh hit-test helpers, the deterministic platform inventory reports 155
lexical D3DX9 matches in eight production files. The count includes types and interface names as
well as function calls; it is not a count of independent replacement implementations.

| Owner | Matches | Feature groups |
|---|---:|---|
| `Code/App/Src/MeshRender.cpp` | 65 | Environment/specular 4x4 matrices, tangent generation, vector normalization, effect handles |
| `Code/Libs/Src/MeshManager/MeshRender.cpp` | 60 | Near-duplicate matrix, tangent and effect paths |
| ParticleEffect headers | 17 | `D3DXVECTOR2/3` storage/layout casts and `D3DXCOLOR` construction |
| `Direct3D_pipe.cpp` | 11 | Effect creation/use, cubemap decode, view/projection matrix upload |
| Public D3D type and vertex-buffer files | 2 | Compatibility declarations/conversions |

The two `MeshRender.cpp` implementations account for 125 matches. They need a shared behavioral
owner before replacement; editing both independently would risk drift.

## Completed geometry slice

`MeshInstance::ComputeHitTest` previously treated `Vector` and the beginning of `MeshVertex` as
`D3DXVECTOR3` through C-style pointer casts, then called `D3DXBoxBoundProbe` and
`D3DXIntersectTri`. `FrontierX/RayGeometry.h` now owns equivalent portable ray/AABB and
double-sided ray/triangle operations. The production call site converts named `Vector` fields
explicitly, so it no longer relies on D3DX layout compatibility or on `MeshVertex::pos` remaining
the first member.

The box routine accepts an unnormalized nonzero direction, starts the valid ray interval at zero,
handles parallel slabs and includes boundary contact. The triangle routine returns barycentric
`u/v` and the parameter distance, accepts either winding, rejects parallel/degenerate triangles,
and rejects intersections behind the origin. The interface mirrors the behavior required by
Microsoft's archived [D3DXBoxBoundProbe](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxboxboundprobe)
and [D3DXIntersectTri](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxintersecttri)
documentation without exposing DirectX types.

`frontierx_ray_geometry_probe` covers scaled directions, origins inside a box, parallel misses,
rays pointing away, zero directions, triangle winding, barycentric values, edge contact,
out-of-triangle, parallel and behind-origin cases. FMA contraction is disabled for the probe.

This is production source integration, but acceptance still requires a native Windows oracle for
near-parallel triangles, signed zero, NaN/infinity, extreme coordinates and exact boundary
tolerances. The Linux test proves the documented portable contract, not bit-for-bit D3DX parity.
The existing Windows project still links D3DX9 because its renderer sources use other facilities.

## Remaining replacements by feature

1. **Effect system:** `ID3DXEffect`, `D3DXCreateEffect`, techniques, parameters and pass sequencing
   are renderer behavior, not simple type aliases. Capture the effect files, parameter names,
   state ownership and pass order from lawful retail data before choosing a shader pipeline.
2. **Cubemap loading:** replace `D3DXCreateCubeTextureFromFileInMemory` with a bounded image/container
   decoder plus renderer upload. Record supported retail encodings before selecting a dependency.
3. **4x4 renderer matrices:** introduce a renderer-local matrix type only after characterizing
   row/column order, multiplication order and transpose-at-upload behavior in both MeshRender copies
   and `Direct3D_pipe.cpp`.
4. **Tangent generation:** consolidate the duplicate algorithm first, then replace D3DX vector
   helpers while preserving degenerate-UV and zero-length behavior.
5. **Particle vertex/color values:** replace explicit D3DX storage types with renderer-neutral
   values at the vertex boundary; verify byte layout before changing any GPU declaration.

The first render vertical slice is not complete until at least one real mesh reaches a native
window through the selected backend with measured image output. Removing names alone is not a
rendering milestone.
